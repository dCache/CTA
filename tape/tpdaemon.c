/*
 * $Id: tpdaemon.c,v 1.23 2009/08/18 09:43:02 waldron Exp $
 *
 * Copyright (C) 1990-2003 by CERN/IT/PDP/DM
 * All rights reserved
 */

#include <errno.h>
#include <stdio.h>
#include <pwd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "Cinit.h"
#include "Ctape.h"
#include "Ctape_api.h"
#include "Cdomainname.h"
#include "marshall.h"
#undef  unmarshall_STRING
#define unmarshall_STRING(ptr,str)  { str = ptr ; INC_PTR(ptr,strlen(str)+1) ; }
#include "net.h"
#include "sacct.h"
#include "serrno.h"
#ifdef TPCSEC
#include "Csec_api.h"
#endif 
#include "tplogger_api.h"
#include <unistd.h>
#include "vdqm_api.h"
#include "vdqm_constants.h"
#include <time.h>

struct confq *confqp;	/* pointer to config queue */
extern char *den2aden();
char domainname[CA_MAXHOSTNAMELEN+1];
struct tpdpdg *dpdg;	/* pointer to drives per device group table */
char func[16];
int jid;
int maxfds;
unsigned int nbdgp;		/* number of device groups */
int nbjobs = 1;		/* number of jobs + 1 (tape daemon itself) */
unsigned int nbtpdrives;		/* number of tape drives */
fd_set readfd, readmask;
struct rlsq *rlsqp;	/* pointer to rls queue */
int rpfd;
#ifdef TPCSEC
uid_t Csec_uid;
gid_t Csec_gid;
int Csec_service_type;
#endif
struct sigaction sa;
struct tprrt tpdrrt;	/* global resource reservation table */
struct tptab *tptabp;	/* pointer to tape drive table */

#ifdef TPCSEC
#define RESETID(UID,GID) resetid(&UID, &GID);
void resetid(uid_t *u, gid_t *g) {
  if (Csec_service_type < 0) {
    *u = Csec_uid;
    *g = Csec_gid;
  }
}
#else
#define RESETID(UID,GID)
#endif 

time_t time();
void wait4child();
void check_child_exit();
void clean4jobdied();

int reldrive( struct tptab*, char*, int, int );
int initdrvtab( void );
int initrrt( void );

static void procreq( int, char*, char* );
static void procconfreq( char*, char* );
static void procfrdrvreq( char*, char* );
static void procinforeq( char*, char* );
static void procuvsnreq( char*, char* );
static void procufilreq( char*, char* );
static void procstatreq( char*, char* );
static void procmountreq( char*, char* );
static void prockilltreq( char*, char* );
static void procposreq( char*, char* );
static void procrlsreq( char*, char* );
static void procrsltreq( char*, char* );
static void procrsvreq( char*, char* );
static void procrstatreq( char*, char* );

char *getconfent();

static int tpdrvidle( struct tptab* );
static int vdqmdrvstate( struct tptab* );

int tpd_main() {
	int c, l;
	char *clienthost;
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);
	struct hostent *hp;
	int magic;
	int msglen;
	int n;
	int on = 1;	/* for REUSEADDR */
	char *p;
	char *rbp;
	char req_data[REQBUFSZ-3*LONGSIZE];
	char req_hdr[3*LONGSIZE];
	int req_type;
	int rqfd;
	int s;
	struct sockaddr_in sin;
	struct servent *sp;
	struct timeval timeval;
	time_t lasttime, tm;
        time_t lasttime_vdqm_update;
        int vdqmchkintvl = VDQMCHKINTVLDFT;
#ifdef TPCSEC
	Csec_context_t sec_ctx;
#endif
        /* initialize tplogger */ 
        p = getconfent ("TAPE", "TPLOGGER", 0);
        if (p && (0 == strcasecmp(p, "SYSLOG"))) {
                tl_init_handle( &tl_tpdaemon, "syslog" ); 
        } else {
                tl_init_handle( &tl_tpdaemon, "dlf" );  
        }
        tl_tpdaemon.tl_init( &tl_tpdaemon, 0 );

	strncpy (func, "taped", 16);
	jid = getpid();
	tplogit (func, "started\n");
        tl_tpdaemon.tl_log( &tl_tpdaemon, 111, 2,
                            "func",    TL_MSG_PARAM_STR, func,
                            "Message", TL_MSG_PARAM_STR, "tape daemon has started." );
	tapeacct (TPDSTART, 0, 0, jid, "", "", "", 0, 0);

	if (Cdomainname (domainname, sizeof(domainname)) < 0) {

		tplogit (func, "Unable to get domainname\n");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 103, 2, 
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "Unable to get domainname." );
		exit (SYERR);
	}

        /* initialize drive table */

        if ((c = initdrvtab()))
		exit (c);

	/* initialize global resource reservation table */

	if ((c = initrrt()))
		exit (c);

	lasttime = time(0);
        lasttime_vdqm_update = 0;
	FD_ZERO (&readmask);
	FD_ZERO (&readfd);
	signal (SIGPIPE,SIG_IGN);
	signal (SIGXFSZ,SIG_IGN);

	/* open request socket */

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		tplogit (func, TP002, "socket", neterror());
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                    "func",    TL_MSG_PARAM_STR, func, 
                                    "Message", TL_MSG_PARAM_STR, "socket",
                                    "Value",   TL_MSG_PARAM_STR, neterror() );
		exit (CONFERR);
	}
	memset ((char *)&sin, 0, sizeof(struct sockaddr_in)) ;
	sin.sin_family = AF_INET ;
#ifdef TPCSEC
	if ((p = getenv ("STAPE_PORT")) || (p = getconfent ("STAPE", "PORT", 0))) {
		sin.sin_port = htons ((unsigned short)atoi (p));
	} else if ((sp = getservbyname ("stape", "tcp"))) {
		sin.sin_port = sp->s_port;
	} else {
		sin.sin_port = htons ((unsigned short)STAPE_PORT);
	}
#else
	if ((p = getenv ("TAPE_PORT")) || (p = getconfent ("TAPE", "PORT", 0))) {
		sin.sin_port = htons ((unsigned short)atoi (p));
	} else if ((sp = getservbyname ("tape", "tcp"))) {
		sin.sin_port = sp->s_port;
	} else {
		sin.sin_port = htons ((unsigned short)TAPE_PORT);
	}
#endif
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) {
		tplogit (func, TP002, "setsockopt", neterror());
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "setsockopt",
                                    "Value",   TL_MSG_PARAM_STR, neterror() );

        }
	if (bind (s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		tplogit (func, TP002, "bind", neterror());
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "bind",
                                    "Value",   TL_MSG_PARAM_STR, neterror() );                
		exit (CONFERR);
	}
	listen (s, 5) ;

	FD_SET (s, &readmask);

		/* main loop */

	while (1) {
		check_child_exit();

		if (FD_ISSET (s, &readfd)) {
#ifdef TPCSEC
		        char *mech, *clientid;
#endif
			FD_CLR (s, &readfd);
			rqfd = accept (s, (struct sockaddr *) &from, &fromlen);
			serrno = 0;

#ifdef TPCSEC
			Csec_server_reinitContext(&sec_ctx, CSEC_SERVICE_TYPE_TAPE, NULL);
			if (Csec_server_establishContext(&sec_ctx, rqfd) < 0) {
			  tplogit(func, "CSEC: Could not establish context: %s !\n", Csec_getErrorMessage());
                          tl_tpdaemon.tl_log( &tl_tpdaemon, 103, 3,
                                              "func",    TL_MSG_PARAM_STR, func,
                                              "Message", TL_MSG_PARAM_STR, "CSEC: Could not establish context!",
                                              "Value",   TL_MSG_PARAM_STR, Csec_getErrorMessage() );
			  close (rqfd);
			  continue;
			}
			/* Connection could be done from another castor service */
			Csec_getClientId(&sec_ctx, &mech, &clientid);
			if ((c = Csec_server_is_castor_service(&sec_ctx)) >= 0) {
			  tplogit(func, "CSEC: Client is castor service type: %d\n", c);
                          tl_tpdaemon.tl_log( &tl_tpdaemon, 104, 3,
                                              "func",    TL_MSG_PARAM_STR, func,
                                              "Message", TL_MSG_PARAM_STR, "CSEC: Client is castor service type",
                                              "Value",   TL_MSG_PARAM_INT, c );
			  Csec_service_type = c;
			}
			else {
			  if (Csec_server_get_client_username(&sec_ctx, &Csec_uid, &Csec_gid) != NULL) {
			    tplogit(func, "CSEC: Client is %s (%d/%d)\n",
				      Csec_server_get_client_username(&sec_ctx, NULL, NULL),
				      Csec_uid,
				      Csec_gid);
                            tl_tpdaemon.tl_log( &tl_tpdaemon, 104, 4, 
                                                "func",    TL_MSG_PARAM_STR, func,
                                                "Message", TL_MSG_PARAM_STR, "CSEC: Client is",
                                                "UID",     TL_MSG_PARAM_INT, Csec_uid,
                                                "GID",     TL_MSG_PARAM_INT, Csec_gid );
			    Csec_service_type = -1;
			  }
			  else {
			    tplogit(func, "CSEC: Can't get client username\n");
                            tl_tpdaemon.tl_log( &tl_tpdaemon, 103, 2,
                                                "func",    TL_MSG_PARAM_STR, func,
                                                "Message", TL_MSG_PARAM_STR, "CSEC: Can't get client username!" );
			    close (rqfd);
			    continue;
			  }
			}
#endif


			l = netread_timeout (rqfd, req_hdr, sizeof(req_hdr), TPTIMEOUT);
			if (l == sizeof(req_hdr)) {
				rbp = req_hdr;
				unmarshall_LONG (rbp, magic);
				unmarshall_LONG (rbp, req_type);
				unmarshall_LONG (rbp, msglen);
				if (msglen > REQBUFSZ) {
					tplogit (func, TP046, REQBUFSZ);
                                        tl_tpdaemon.tl_log( &tl_tpdaemon, 46, 2,
                                                            "func",    TL_MSG_PARAM_STR, func,
                                                            "REQBUFSZ", TL_MSG_PARAM_INT, REQBUFSZ );
					close (rqfd);
					continue;
				}
				l = msglen - sizeof(req_hdr);
				n = netread_timeout (rqfd, req_data, l, TPTIMEOUT);
				if(n != l) {
					const char *errMsg = "UNKNOWN";
					switch(n) {
					case 0:
						errMsg = "Connection closed by the remote end";
						break;
					case -1:
						errMsg = neterror();
						break;
					default:
						errMsg = "Read wrong number of bytes";
					}
					tplogit (func, TP002, "netread_timeout",
						errMsg);
                                        tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                                            "func",    TL_MSG_PARAM_STR, func,
                                                            "Message", TL_MSG_PARAM_STR, "netread_timeout",
                                                            "Value",   TL_MSG_PARAM_STR, errMsg);
					close (rqfd);
					continue;
				}
				rpfd = rqfd;
				if (getpeername (rqfd, (struct sockaddr *) &from,
				    &fromlen) < 0) {
					tplogit (func, TP002, "getpeername",
						neterror());
                                        tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                                            "func",    TL_MSG_PARAM_STR, func,
                                                            "Message", TL_MSG_PARAM_STR, "getpeername",
                                                            "Value",   TL_MSG_PARAM_STR, neterror() );
				}
				hp = gethostbyaddr ((char *)(&from.sin_addr),
					sizeof(struct in_addr),from.sin_family);
				if (hp == NULL)
					clienthost = inet_ntoa (from.sin_addr);
				else
                                        clienthost = hp->h_name ;
				procreq (req_type, req_data, clienthost);
			} else {
				close (rqfd);
				if (l > 0) {
					tplogit (func, TP004, l);
                                        tl_tpdaemon.tl_log( &tl_tpdaemon, 4, 3,
                                                            "func",   TL_MSG_PARAM_STR, func,
                                                            "l",      TL_MSG_PARAM_INT, l,
                                                            "Length", TL_MSG_PARAM_INT, l );
                                }
				else if (l < 0) {
					tplogit (func, TP002, "netread", neterror());
                                        tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                                            "func",    TL_MSG_PARAM_STR, func,
                                                            "Message", TL_MSG_PARAM_STR, "netread",
                                                            "Value",   TL_MSG_PARAM_STR, neterror() );
                                }
			}
		}

		/* look for jobs that died */
		if (((tm = time (0)) - lasttime) >= CLNREQI) {
			clean4jobdied();  
			lasttime = tm;
		}
        /* 
        ** Confirm to VDQM if drives are free 
        */
        if ((((tm = time (0)) - lasttime_vdqm_update) >= vdqmchkintvl)) {               

            unsigned int i;
            struct tptab *tunp = tptabp;  
                        
            /* 
               Placing the config check into the outer IF avoids to
               open the config file every CHECKI (i.e. 10) seconds, but 
               preserves the capability to do config changes w/o tape daemon 
               restart. However, changes to the config file to take effect 
               can take as long as (old) vdqmchkintvl seconds. 
            */
            p = getconfent( "TAPE", "CONFIRM_DRIVE_FREE", 0 );
            if ((NULL == p) || (0 == strcasecmp( p, "YES" ))) {
                                                                                
                p = getconfent( "TAPE", "CONFIRM_DRIVE_FREE_INTVL", 0 );
                if (NULL != p) {
                    vdqmchkintvl = atoi(p);
                    if (vdqmchkintvl < 0) {
                        vdqmchkintvl = VDQMCHKINTVLDFT;   
                    }
                } else {
                    vdqmchkintvl = VDQMCHKINTVLDFT;  
                }

                for (i = 0; i < nbtpdrives; i++) {

                    /* 
                       If the drive has been unassigned since the last check and 
                       tape daemon thinks the drive is idle ... 
                    */
                    int drvidle = tpdrvidle(tunp);
                    if (drvidle && (tunp->unasn_time > lasttime_vdqm_update)) {
                                        
                        /*  
                            ... but VDQM does not ... 
                        */
                        int vdqmstate;
                        vdqmstate = vdqmdrvstate(tunp);
                        if ( (vdqmstate == (VDQM_UNIT_UP|VDQM_UNIT_BUSY|VDQM_UNIT_ASSIGN)) ||                      /* RUNNING */
                             (vdqmstate == (VDQM_UNIT_UP|VDQM_UNIT_BUSY|VDQM_UNIT_RELEASE|VDQM_UNIT_UNKNOWN)) ||   /* RELEASE */
                             (vdqmstate == (VDQM_UNIT_UP|VDQM_UNIT_BUSY|VDQM_UNIT_UNKNOWN)) ) {                    /* bad UNKNOWN (0x221)*/ 

                            /* 
                               ... update VDQM. We explicitely do not check (vdqmstate == UP|FREE) here, since
                               VDQM changes the state to UP|BUSY before contacting tape daemon. The latter should 
                               not reset the state to UP|FREE in that case. 
                            */
                            int vdqm_rc;
                            int vdqm_status = VDQM_UNIT_UP;
                            tplogit (func, "calling vdqm_UnitStatus to confirm drive (%s) FREE\n", tunp->drive);
                            tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 3,
                                                "func"   , TL_MSG_PARAM_STR, func,
                                                "Message", TL_MSG_PARAM_STR, "calling vdqm_UnitStatus to confirm drive FREE",
                                                "Drive"  , TL_MSG_PARAM_STR, tunp->drive );

                            vdqm_rc = vdqm_UnitStatus (NULL, NULL, tunp->dgn, NULL, tunp->drive,
                                                       &vdqm_status, NULL, 0);

                            tplogit (func, "vdqm_UnitStatus returned %s\n",
                                     vdqm_rc ? sstrerror(serrno) : "ok");
                            tl_tpdaemon.tl_log( &tl_tpdaemon, vdqm_rc?104:110, 3,
                                                "func",    TL_MSG_PARAM_STR, func, 
                                                "Message", TL_MSG_PARAM_STR, "vdqm_UnitStatus returned",
                                                "Error",   TL_MSG_PARAM_STR, vdqm_rc ? sstrerror(serrno) : "ok");
                        } else {
                                                
                            tplogit (func, "no VDQM update required: states not inconsistent (drive %s, vdqmstate 0x%x)\n",
                                     tunp->drive, vdqmstate );
                            tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 4,
                                                "func"     , TL_MSG_PARAM_STR, func,
                                                "Message"  , TL_MSG_PARAM_STR, "no VDQM update required, states consistent",
                                                "vdqmstate", TL_MSG_PARAM_INT, vdqmstate,
                                                "Drive"    , TL_MSG_PARAM_STR, tunp->drive );
                        }
                                        
                    } else {
                                        
                        tplogit (func, "no VDQM update required: drive %s %s\n", 
                                 tunp->drive, drvidle ? "recently checked/set" : "not FREE" );
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 4,
                                            "func"             , TL_MSG_PARAM_STR, func,
                                            "Message"          , TL_MSG_PARAM_STR, "no VDQM update required",
                                            "Reason"           , TL_MSG_PARAM_STR, drvidle ? "recently checked/set" : "drive not FREE",
                                            "Drive"            , TL_MSG_PARAM_STR, tunp->drive );
                    }
                    tunp++;                                
                }
                lasttime_vdqm_update = tm;
            }
        }
		memcpy (&readfd, &readmask, sizeof(readmask));
		timeval.tv_sec = CHECKI;	/* must set each time for linux */
		timeval.tv_usec = 0;
		if (select (maxfds, &readfd, (fd_set *)0, (fd_set *)0, &timeval) < 0) {
			FD_ZERO (&readfd);
		}
	}
}

int main(int argc,
         char **argv)
{
	(void)argc;
	(void)argv;
	if ((maxfds = Cinitdaemon ("taped", wait4child)) < 0)
		exit (1);
	exit (tpd_main());
}

int chk_den(struct tptab *tunp,
            int den,
            struct tpdev **cdevp)
{
	/* check if the device supports the requested density */

	int i;

	*cdevp = tunp->devp;
	for (i = 0; i < tunp->devnum; i++, (*cdevp)++) {
		if (den == 0) 	/* density was not specified */
			return (0);
		if ((*cdevp)->den == 0) 	/* drive density = any or N/A */
			return (0);
		if ((*cdevp)->den == den)
			return (0);
	}
	return (1);
}

void clean4jobdied()
{
	int c, j, n;
        unsigned int i;
	int *jids;
	int nb_drives_asn;
	struct tptab *tunp;
	struct tprrt *rrtp;

	j = 0;
	rrtp = tpdrrt.next;
	if (! rrtp) return;
	jids = malloc (nbjobs * sizeof(int));
	while (rrtp) {
		jids[j++] = rrtp->jid;
		rrtp = rrtp->next;
	}
	jids[j] = 0;

	/* This function will fill in place the jids array with the ids of the processes
	 * that are no here anymore (no /proc/<pid> entry).
	 * n is their number.
	 */
	n = checkjobdied (jids);

	for (j = 0; j < n; j++) {
		tplogit (func, "cleaning up: job %d has died\n", jids[j]);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 104, 3,
                                    "func"   , TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "Cleaning up: job has died",
                                    "JobID"  , TL_MSG_PARAM_INT, jids[j] );                
		rrtp = tpdrrt.next;
		while (rrtp) {
			if (rrtp->jid == jids[j]) break;
			rrtp = rrtp->next;
		}
		nb_drives_asn = 0;
		tunp = tptabp;
		for (i = 0; i < nbtpdrives; i++) {
			if (tunp->jid == jids[j] && tunp->asn != 0) {
				nb_drives_asn++;
				if (tunp->asn == 1) {
					if (tunp->mntovly_pid) {
						tplogit (func, "killing process %d\n",
							tunp->mntovly_pid);
                                                tl_tpdaemon.tl_log( &tl_tpdaemon, 104, 4,
                                                                    "func",    TL_MSG_PARAM_STR, func,
                                                                    "Message", TL_MSG_PARAM_STR, "Killing process",
                                                                    "PID",     TL_MSG_PARAM_INT, tunp->mntovly_pid,
                                                                    "JobID"  , TL_MSG_PARAM_INT, jids[j] );
						kill (tunp->mntovly_pid, SIGINT);
						tunp->mntovly_pid = 0;
					} else {
						c = reldrive (tunp, rrtp->user,
						    -1, TPRLS_UNLOAD);
						if (c == 0) rrtp->unldcnt++;
					}
				}
			}
			tunp++;
		}
		if (nb_drives_asn == 0) {	/* no drive assigned, free only rsv */
			(rrtp->prev)->next = rrtp->next;
			if (rrtp->next) (rrtp->next)->prev = rrtp->prev;
			free (rrtp->dg);
			free (rrtp);
			nbjobs--;
		}
	}
	free (jids);
}

int confdrive(struct tptab *tunp,
              int rpfd,
              int status,
              int reason)
{
	int c;
	int i;
	int pid;
	struct confq *prev;
	struct confq *rqp;
	struct stat sbuf;
	struct tpdev *tdp;

	c = 0;
	if (status) {	/* tpconfig up */
		tplogit (func, TP035, tunp->drive, "up");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 70, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Drive",   TL_MSG_PARAM_STR, tunp->drive,
                                    "Message", TL_MSG_PARAM_STR, "up" );
		for (i = 0, tdp = tunp->devp; i < tunp->devnum; i++, tdp++)
			if (stat (tdp->dvn, &sbuf) < 0) {
				tplogit (func, "TP002 - %s : stat error : %s\n",
					tdp->dvn, strerror(errno));
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                                    "func",       TL_MSG_PARAM_STR, func,
                                                    "Message",    TL_MSG_PARAM_STR, tdp->dvn,
                                                    "Stat error", TL_MSG_PARAM_STR, strerror(errno) );
                        }
			else
				tdp->majmin = sbuf.st_rdev;
	} else {
		tplogit (func, TP035, tunp->drive, "down");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 77, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Drive",   TL_MSG_PARAM_STR, tunp->drive,
                                    "Message", TL_MSG_PARAM_STR, "down" );
	}
	prev = 0;
	rqp = confqp;
	while (rqp) {
		prev = rqp;
		rqp = rqp->next;
	}
	rqp = (struct confq *) calloc (1, sizeof(struct confq));
	if (prev) {
		prev->next = rqp;
		rqp->prev = prev;
	} else
		confqp = rqp;
	rqp->status = status;
	rqp->ux = tunp->ux;

	/* fork and exec the process to process drive configuration */

	c = 0;
	tplogit (func, "forking a confdrive process\n");
        tl_tpdaemon.tl_log( &tl_tpdaemon, 108, 2,
                            "func",    TL_MSG_PARAM_STR, func,
                            "Message", TL_MSG_PARAM_STR, "Forking a confdrive process" );

	rqp->ovly_pid = fork ();
	pid = rqp->ovly_pid;
	if (pid < 0) {
		usrmsg (func, TP002, "fork", strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "fork",
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );
		c = ETSYS;
	} else if (pid == 0) {	/* we are in the child */

		/* set up the parameters for the process confdrive */

		char arg_gid[11], arg_jid[11], arg_reason[2], arg_rpfd[3];
		char arg_status[2], arg_uid[11];
		char progfullpath[CA_MAXPATHLEN+1];

		sprintf (progfullpath, "%s/confdrive", BIN);
		sprintf (arg_rpfd, "%d", rpfd);
		sprintf (arg_uid, "%d", tunp->uid);
		sprintf (arg_gid, "%d", tunp->gid);
		sprintf (arg_jid, "%d", tunp->jid);
		sprintf (arg_status, "%d", status);
		sprintf (arg_reason, "%d", reason);

		tplogit (func, "execing confdrive, pid=%d\n", getpid());
                /* tl_log msg wouldn't be seen in the DB due to the execlp() */

		execlp (progfullpath, "confdrive", tunp->drive,
			tunp->devp->dvn, arg_rpfd, arg_uid, arg_gid,
			arg_jid, tunp->dgn, "tape", arg_status,
			arg_reason, NULL);

		tplogit (func, "TP002 - confdrive : execlp error : %s\n",
		    strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "confdrive : execlp error",
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );

                tl_tpdaemon.tl_exit( &tl_tpdaemon, 0 );
		exit (errno);
	}
	return (c);
}

int initdrvtab()
{
	char aden[CA_MAXDENLEN+1];
	char buf[100];
	char drive[CA_MAXUNMLEN+1];
	int errflag;
	char *fgets();
	FILE *fopen(), *s;
	unsigned int i;
        int j;
	char instat[5];
	int lineno;
	char *p_den;
	char *p_dgn;
	char *p_dvn;
	char *p_dvt;
	char *p_ldr;
	char *p_stat;
	char *p_unm;
	char prevdrive[CA_MAXUNMLEN+1];
	struct tpdev *tdp;
	struct tptab *tunp;

	if ((s = fopen ("/etc/castor/TPCONFIG", "r")) == NULL) {
		tplogit (func, TP008, "/etc/castor/TPCONFIG");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 8, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "/etc/castor/TPCONFIG" );
		return (ETSYS);
	}
	/* 1st pass: count number of tape drives defined */
	errflag = 0;
	lineno = 0;
	nbtpdrives = 0;
	prevdrive[0] = '\0';
	while (fgets (buf, sizeof(buf), s) != NULL) {
		lineno++;
		if (buf[0] == '#') continue;	/* comment line */
		if (buf[strlen (buf)-1] != '\n') {
			tplogit (func, TP043, buf);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 43, 2,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Message", TL_MSG_PARAM_STR, buf );                        
			errflag++;
		}
		if (strspn (buf, " \t") == (strlen (buf) - 1)) continue; /* blank line */
		p_unm = strtok (buf, " \t");
		if ((p_dgn = strtok (NULL, " \t")) == NULL ||
		    (p_dvn = strtok (NULL, " \t")) == NULL ||
		    (p_den = strtok (NULL, " \t")) == NULL ||
		    (p_stat = strtok (NULL, " \t")) == NULL ||
		    (p_ldr = strtok (NULL, " \t")) == NULL ||
		    (p_dvt = strtok (NULL, " \t")) == NULL) {
			tplogit (func, TP032, lineno, "missing fields");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Line",    TL_MSG_PARAM_INT, lineno,
                                            "Message", TL_MSG_PARAM_STR, "missing fields" );
			errflag++;
			continue;
		}
		if (strlen (p_unm) <= CA_MAXUNMLEN) {
			if (strcmp (buf, prevdrive)) {
				nbtpdrives++;
				strcpy (prevdrive, buf);
			}
		} else {
			tplogit (func, TP032, lineno, "invalid drive name");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Line",    TL_MSG_PARAM_INT, lineno,
                                            "Message", TL_MSG_PARAM_STR, "invalid drive name" );                        
			errflag++;
		}
		if (strlen (p_dgn) > CA_MAXDGNLEN) {
			tplogit (func, TP032, lineno, "invalid device group");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Line",    TL_MSG_PARAM_INT, lineno,
                                            "Message", TL_MSG_PARAM_STR, "invalid device group" );                        
			errflag++;
		}
		if (strlen (p_dvn) > CA_MAXDVNLEN || strncmp (p_dvn, "/dev", 4)) {
			tplogit (func, TP032, lineno, "invalid device name");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Line",    TL_MSG_PARAM_INT, lineno,
                                            "Message", TL_MSG_PARAM_STR, "invalid device name" );                        
			errflag++;
		}
		if (strlen (p_den) > CA_MAXDENLEN || cvtden (p_den) < 0) {
			tplogit (func, TP032, lineno, "invalid density");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Line",    TL_MSG_PARAM_INT, lineno,
                                            "Message", TL_MSG_PARAM_STR, "invalid density" );                        
			errflag++;
		}
		if (strcmp (p_stat, "up") && strcmp (p_stat, "down")) {
			tplogit (func, TP032, lineno, "invalid status");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Line",    TL_MSG_PARAM_INT, lineno,
                                            "Message", TL_MSG_PARAM_STR, "invalid status" );                        
			errflag++;
		}
		if (strlen (p_ldr) > CA_MAXRBTNAMELEN) {
			tplogit (func, TP032, lineno, "invalid loader");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Line",    TL_MSG_PARAM_INT, lineno,
                                            "Message", TL_MSG_PARAM_STR, "invalid loader" );                        
			errflag++;
		}
		if (strlen (p_dvt) > CA_MAXDVTLEN) {
			tplogit (func, TP032, lineno, "invalid device type");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Line",    TL_MSG_PARAM_INT, lineno,
                                            "Message", TL_MSG_PARAM_STR, "invalid device type" );                        
			errflag++;
		}

		if(!deviceTypeIsSupported(p_dvt)) {
		       tplogit (func, TP032, lineno, "unsupported device type");
		       tl_tpdaemon.tl_log( &tl_tpdaemon, 32, 3,
			                    "func",    TL_MSG_PARAM_STR, func,
			                    "Line",    TL_MSG_PARAM_INT, lineno,
			                    "Message", TL_MSG_PARAM_STR, "unsupported device type" );
			errflag++;
		}
	}
	if (errflag) {
          fclose(s);
          return (ETSYS);
        }

	/* allocate space for tape drive table */
	tptabp = (struct tptab *) calloc (nbtpdrives, sizeof(struct tptab));

	/* 2nd pass: count number of devices defined for each drive */
	rewind (s);
	tunp = tptabp;
	nbtpdrives = 0;
	prevdrive[0] = '\0';
	while (fgets (buf, sizeof(buf), s) != NULL) {
		if (buf[0] == '#') continue;	/* comment line */
		if (strspn (buf, " \t") == (strlen (buf) - 1)) continue; /* blank line */
		sscanf (buf, "%s", drive);
		if (strcmp (drive, prevdrive)) {
			if (nbtpdrives != 0) tunp++;
			nbtpdrives++;
			strcpy (prevdrive, drive);
		}
		tunp->devnum++;
	}
	/* 3rd pass: store information */
	rewind (s);
	tunp = tptabp;
	for (i = 0; i< nbtpdrives; i++) {
		tunp->ux = i;
		tdp = (struct tpdev *) calloc (tunp->devnum, sizeof(struct tpdev));
		tunp->devp = tdp;
		j = 0;
		while (j < tunp->devnum) {
			fgets (buf, sizeof(buf), s);
			if (buf[0] == '#') continue;	/* comment line */
			if (strspn (buf, " \t") == (strlen (buf) - 1)) continue;
			sscanf (buf, "%s%s%s%s%s%s%s", 
				tunp->drive, tunp->dgn, tdp->dvn, aden,
				instat, tunp->loader, tunp->devtype);
			tdp->den = cvtden (aden);
			tdp++;
			j++;
		}
		(void) confdrive (tunp, -1, (instat[0] == 'u') ? CONF_UP:CONF_DOWN, 0);
		tunp++;
	}
	fclose (s);
	return (0);
}

int initrrt()
{
	int errflag;
	unsigned int i, j;
	char prevdgn[CA_MAXDGNLEN+1];
	unsigned int prevgrp;
	struct tptab *tunp;

	tunp = tptabp;
	nbdgp = 0;
	tpdrrt.jid = jid;

	/* count the number of device groups */

	prevdgn[0] = '\0';
	for (i = 0; i < nbtpdrives; i++) {
		if (strcmp (tunp->dgn, prevdgn)) {
			strcpy (prevdgn, tunp->dgn);
			nbdgp++;
		}
		tunp++;
	}
	tpdrrt.dg = (struct tpdgrt *) calloc (nbdgp, sizeof(struct tpdgrt));
	dpdg = (struct tpdpdg *) calloc (nbdgp, sizeof(struct tpdpdg));

	tunp = tptabp;
	errflag = 0;
	nbdgp = 0;
	for (i = 0; i < nbtpdrives; i++) {
		for (j = 0; j < nbdgp; j++) {
			if (strcmp (tunp->dgn, tpdrrt.dg[j].name) == 0) break;
		}
		if (tpdrrt.dg[j].name[0] == '\0') {
			strcpy (tpdrrt.dg[j].name, tunp->dgn);
			strcpy ((dpdg+j)->name, tunp->dgn);
			(dpdg+j)->first = tunp;
			(dpdg+j)->next = tunp;
			nbdgp++;
			prevgrp = j;
		} else {
			if (j != prevgrp) {
				tplogit (func, TP034);
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 34, 1,
                                                    "func", TL_MSG_PARAM_STR, func );                        
				errflag++;
			}
		}
		(dpdg+j)->last = tunp;
		tunp++;
	}
	if (errflag) return (ETSYS);
	return (0);
}

static void procreq(int req_type,
             char *req_data,
             char *clienthost)
{
	switch (req_type) {
	case TPRSV:
		procrsvreq (req_data, clienthost);
		break;
	case TPMOUNT:
		procmountreq (req_data, clienthost);
		break;
	case TPSTAT:
		procstatreq (req_data, clienthost);
		break;
	case TPRSTAT:
		procrstatreq (req_data, clienthost);
		break;
	case TPCONF:
		procconfreq (req_data, clienthost);
		break;
	case TPRLS:
		procrlsreq (req_data, clienthost);
		break;
	case UPDVSN:
		procuvsnreq (req_data, clienthost);
		break;
	case TPKILL:
		prockilltreq (req_data, clienthost);
		break;
	case FREEDRV:
		procfrdrvreq (req_data, clienthost);
		break;
	case RSLT:
		procrsltreq (req_data, clienthost);
		break;
	case UPDFIL:
		procufilreq (req_data, clienthost);
		break;
	case TPINFO:
		procinforeq (req_data, clienthost);
		break;
	case TPPOS:
		procposreq (req_data, clienthost);
		break;
	default:
		usrmsg (func, TP003, req_type);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 3, 2,
                                    "func",     TL_MSG_PARAM_STR, func,
                                    "req_type", TL_MSG_PARAM_INT, req_type );                
		sendrep (rpfd, TAPERC, USERR);
	}
}

static void procconfreq(char *req_data,
                 char *clienthost)
{
	int c;
	char *drive;
	gid_t gid;
	unsigned int j;
	char *rbp;
	int reason;
	int status;
	struct tptab *tunp;
	uid_t uid;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "conf", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "conf",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
        unmarshall_STRING (rbp, drive);
        unmarshall_WORD (rbp, status);
        unmarshall_WORD (rbp, reason);

	c = 0;
	tunp = tptabp;
	for (j = 0; j < nbtpdrives; j++) {
		if (strcmp (drive, tunp->drive) == 0) break;
		tunp++;
	}
	if (j == nbtpdrives) {    /* unknown drive name */
		usrmsg (func, TP015);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 15, 2,
                                    "func" , TL_MSG_PARAM_STR, func,
                                    "Drive", TL_MSG_PARAM_STR, drive );          
                c = ETIDN;
		goto reply;
	}
	if (tunp->up == status || (status == CONF_DOWN && tunp->up < 0)) {
		sendrep (rpfd, TAPERC, 0);
		return;
	}
	for (j = 0; j < nbdgp; j++) {
		if (strcmp (tunp->dgn, tpdrrt.dg[j].name) == 0) break;
	}
	if (tunp->asn != 0) {	/* drive currently assigned */
		tplogit (func, TP048, tunp->drive);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 48, 2,
                                    "func",  TL_MSG_PARAM_STR, func,
                                    "Drive", TL_MSG_PARAM_STR, tunp->drive );
		if (status == CONF_UP) { /* conf down, then up while assigned */
			tunp->up = 1;
			tpdrrt.dg[j].rsvd++;
		} else {
			tunp->up = -reason;
			tpdrrt.dg[j].rsvd--;
		}
		c = ETDCA;
		goto reply;
	}
	if (status == CONF_DOWN) {
		tunp->up = 0;
		tpdrrt.dg[j].rsvd--;
	}
	c = confdrive (tunp, rpfd, status, reason);
reply:
	if (c)
		sendrep (rpfd, TAPERC, c);
	else
		close (rpfd);
}

static void procfrdrvreq(char *req_data,
                  char *clienthost)
{
	int found;
	gid_t gid;
	unsigned int j;
	int jid;
	int rlsflags;
	char *rbp;
	int rls_rpfd;
	struct rlsq *rqp;
	struct tprrt *rrtp;
	struct tptab *tunp;
	uid_t uid;
	int ux;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "free drive", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 76, 5,
                        "func",       TL_MSG_PARAM_STR, func,
                        "Message",    TL_MSG_PARAM_STR, "free drive",
                        "UID",        TL_MSG_PARAM_UID, uid,
                        "GID",        TL_MSG_PARAM_GID, gid,
                        "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_LONG (rbp, jid);
	unmarshall_WORD (rbp, rlsflags);
	unmarshall_WORD (rbp, rls_rpfd);
	unmarshall_WORD (rbp, ux);
	tunp = tptabp + ux;
	rrtp = tpdrrt.next;
	while (rrtp) {
		if (jid == rrtp->jid) break;
		rrtp = rrtp->next;
	}
    if (rrtp) { /* could be a replayed release */
        for (j = 0; j < nbdgp; j++) {
            if (strcmp (rrtp->dg[j].name, tunp->dgn) == 0) break;
        }
        rrtp->dg[j].used--;	/* decrement usage count */
    }
    if (j < nbdgp) {
        tpdrrt.dg[j].used--;	/* decrement global usage count */
    }
    if (rrtp) {
        if ((rlsflags & TPRLS_KEEP_RSV) == 0 || tunp->asn == -2) {
            rrtp->dg[j].rsvd--;	/* decrement user reservation for dev group */
            rrtp->totrsvd--;	/* decrement global user reservation */
        }        
        rrtp->unldcnt--;		/* decrement number of unloads in progress */
    }
    if (tunp) {
        tunp->asn = 0;		/* unassign drive */
        tunp->asn_time = 0;
        tunp->unasn_time = time(0);
        free (tunp->filp);	/* release tape file description */
        tunp->filp = 0;
        if ((rlsflags & TPRLS_NOUNLOAD) == 0) {
            tunp->vid[0] = '\0';
            tunp->vsn[0] = '\0';
        }
        tunp->tobemounted = 0;
        tapeacct (TPFREE, tunp->uid, tunp->gid, tunp->jid,
                  tunp->dgn, tunp->drive, "", 0, 0);
        if (tunp->up <= 0) {
            (void) confdrive (tunp, -1, CONF_DOWN, -tunp->up);
            tunp->up = 0;
        }
    }
    if (rrtp) {
        if (rrtp->totrsvd <= 0) {	/* no more reserved devices, free entry */
            (rrtp->prev)->next = rrtp->next;
            if (rrtp->next) (rrtp->next)->prev = rrtp->prev;
            free (rrtp->dg);
            free (rrtp);
            nbjobs--;
        }
    }
	found = 0;
	rqp = rlsqp;
	while (rqp) {
		if (rqp->rpfd == rls_rpfd) {
			found = 1;
			break;
		}
		rqp = rqp->next;
	}
	if (found && --rqp->unldcnt <= 0) {	/* all unloads are terminated for this rls req */
		if (rqp->prev) (rqp->prev)->next = rqp->next;
		else rlsqp = rqp->next;
		if (rqp->next) (rqp->next)->prev = rqp->prev;
		free (rqp);
		sendrep (rls_rpfd, TAPERC, 0);
	}
	close (rpfd);
}

static void procinforeq(char *req_data,
                 char *clienthost)
{
	int c;
	int cfseq;
	int found;
	gid_t gid;
	unsigned int i;
	char *path;
	char *rbp;
	char repbuf[REPBUFSZ];
	char *sbp;
	struct tptab *tunp;
	uid_t uid;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "info", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "info",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_STRING (rbp, path);

	c = 0;
	found = 0;
	tunp = tptabp;
	for (i = 0; i < nbtpdrives; i++) {
		if (tunp->asn != 0 &&
		    strcmp (tunp->filp->path, path) == 0) {
			found = 1;
			break;
		}
		tunp++;
	}
	if (! found) {
		usrmsg (func, TP037, path);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 37, 2,
                                    "func", TL_MSG_PARAM_STR, func,
                                    "path", TL_MSG_PARAM_STR, path );                
		c = ENOENT;
		goto reply;
	} else {
		sbp = repbuf;
                marshall_LONG (sbp, tunp->filp->blksize);
                marshall_OPAQUE (sbp, tunp->filp->blockid, 4);
		marshall_STRING (sbp, den2aden(tunp->cdevp->den));
                marshall_STRING (sbp, tunp->devtype);
                marshall_STRING (sbp, tunp->drive);
                marshall_STRING (sbp, tunp->filp->fid);
		cfseq = tunp->filp->cfseq;
		if (tunp->filp->Qfirst) cfseq -= tunp->filp->Qfirst - 1;
                marshall_LONG (sbp, cfseq);
                marshall_LONG (sbp, tunp->filp->lrecl);
                marshall_STRING (sbp, tunp->filp->recfm);
		sendrep (rpfd, MSG_DATA, sbp - repbuf, repbuf);
		goto reply;
	}
reply:
	sendrep (rpfd, TAPERC, c);
}

static void prockilltreq(char *req_data,
                  char *clienthost)
{
	unsigned int i;
	gid_t gid;
	int jid;
	char *path;
	char *rbp;
	struct tprrt *rrtp;
	struct tptab *tunp;
	uid_t uid;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	unmarshall_LONG (rbp, jid);
	unmarshall_STRING (rbp, path);

	tplogit (func, TP056, "kill", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 6,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "kill",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "JobID",      TL_MSG_PARAM_INT, jid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        

	/* check if reserv done */

	rrtp = tpdrrt.next;
	while (rrtp) {
		if (jid == rrtp->jid) break;
		rrtp = rrtp->next;
	}
	if (rrtp != 0) {
		tunp = tptabp;
		for (i = 0; i < nbtpdrives; i++) {
			if (tunp->jid == jid && tunp->asn == 1 &&
				strcmp (tunp->filp->path, path) == 0) {
				if (tunp->mntovly_pid) {
					tplogit (func, "killing process %d\n",
						tunp->mntovly_pid);
                                        tl_tpdaemon.tl_log( &tl_tpdaemon, 104, 4,
                                                            "func",    TL_MSG_PARAM_STR, func,
                                                            "Message", TL_MSG_PARAM_STR, "Killing process",
                                                            "PID",     TL_MSG_PARAM_INT, tunp->mntovly_pid,
                                                            "JobID",   TL_MSG_PARAM_INT, jid );                        
					kill (tunp->mntovly_pid, SIGINT);
					tunp->mntovly_pid = 0;
				}
				break;
			}
			tunp++;
		}
	}
	close (rpfd);
}

static void procmountreq(char *req_data,
                  char *clienthost)
{
	char *acctname;
	int c;
	struct tpdev *cdevp;
	int den;
	char *density;
	char *dgn;
	char *drive;
	int errflg = 0;
	int found;
	gid_t gid;
	unsigned int i, j;
	int jid;
	char *lbltype;
	int mode;
	char *path;
	int pid;
	short prelabel;
	char *rbp;
	struct tprrt *rrtp;
	int side;
	struct tptab *tunp;
	uid_t uid;
	int vdqm_reqid;
	char *vid;
	char *vsn;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "mount", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "mount",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_LONG (rbp, jid);
        unmarshall_STRING (rbp, acctname);
        unmarshall_STRING (rbp, path);
        unmarshall_STRING (rbp, vid);
        unmarshall_WORD (rbp, side);
        unmarshall_STRING (rbp, dgn);
        unmarshall_STRING (rbp, density);
        unmarshall_STRING (rbp, drive);
        unmarshall_WORD (rbp, mode);
        unmarshall_STRING (rbp, vsn);
        unmarshall_STRING (rbp, lbltype);
        unmarshall_WORD (rbp, prelabel);
	unmarshall_LONG (rbp, vdqm_reqid);

	tplogit (func, "mount request %s\n", vid);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 72, 5,
                            "func"   , TL_MSG_PARAM_STR  , func,
                            "Message", TL_MSG_PARAM_STR  , "Mount request",
                            "JobID"  , TL_MSG_PARAM_INT  , jid,
                            "VID"    , TL_MSG_PARAM_STR  , vid,
                            "TPVID"  , TL_MSG_PARAM_TPVID, vid );                        
	c = 0;

	/* Only local requests for mounts are permitted */
	{
		char localHostName[256];
		if(-1 == gethostname(localHostName, sizeof(localHostName))) {
			strncpy(localHostName, "localhost.localdomain",
				sizeof(localHostName));
		}
		localHostName[sizeof(localHostName) - 1] = '\0';
		if(0 != strcmp(clienthost, localHostName)) {
			usrmsg (func,
			  "TP002 - Mount of %s aborted: Permission denied\n",
			  vid);
			tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 5,
			  "func"   , TL_MSG_PARAM_STR, func,
			  "Message", TL_MSG_PARAM_STR,
			    "TP002 - Mount aborted: Permission denied",
			  "JobID"  , TL_MSG_PARAM_INT  , jid,
			  "VID"    , TL_MSG_PARAM_STR  , vid,
			  "TPVID"  , TL_MSG_PARAM_TPVID, vid );

			c = EACCES; /* Permission denied */
			goto reply;
		}
	}

	/* check if reserv done */

	rrtp = tpdrrt.next;
	while (rrtp) {
		if (jid == rrtp->jid) break;
		rrtp = rrtp->next;
	}
	if (rrtp == 0) {
		usrmsg (func, TP014);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 14, 2,
                                    "func",  TL_MSG_PARAM_STR, func,
                                    "JobID", TL_MSG_PARAM_INT, jid );                
		c = ETNRS;		/* reserve not done */
		goto reply;
	}

	/* check device group name validity */

	for (j = 0; j < nbdgp; j++)
		if (strcmp (rrtp->dg[j].name, dgn) == 0) break;
	if (j == nbdgp) {
		usrmsg (func, TP013);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 13, 2,
                                    "func",  TL_MSG_PARAM_STR, func,
                                    "JobID", TL_MSG_PARAM_INT, jid );                 
		c = ETIDG;		/* invalid device group name */
		goto reply;
	}

	/* check validity of the other arguments */

	if ((den = cvtden (density)) < 0) {
		usrmsg (func, TP006, "density");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "density",
                                    "JobID",   TL_MSG_PARAM_INT, jid );                
		errflg++;
	}

	/* Only the AUL label type is supported */
	if (strcmp (lbltype, "aul")) {
		usrmsg (func,
		  "TP006 - Invalid value for lbltype: actual %s,"
		  " expected aul\n", lbltype);
		tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 5,
		  "func"   , TL_MSG_PARAM_STR, func,
		  "Message", TL_MSG_PARAM_STR, "lbltype",
		  "JobID"  , TL_MSG_PARAM_INT  , jid,
		  "VID"    , TL_MSG_PARAM_STR  , vid,
		  "TPVID"  , TL_MSG_PARAM_TPVID, vid );
		errflg++;
	}
/********************************************************************
 Please note that if we ever support the BLP label type then we must
 include the following if statement
	if (lblcode == BLP && mode == WRITE_ENABLE) {
		usrmsg (func, TP017);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 17, 2,
                                    "func",  TL_MSG_PARAM_STR, func,
                                    "JobID", TL_MSG_PARAM_INT, jid );                
		errflg++;
	}
*********************************************************************/
	if (errflg) {
		c = EINVAL;
		goto reply;
	}

	if (*drive) {	/* request is for a specific drive */
		tunp = tptabp;
		for (i = 0; i < nbtpdrives; i++) {
			if (strcmp (tunp->drive, drive) == 0) break;
			tunp++;
		}
		if (i == nbtpdrives) {
			usrmsg (func, TP015);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 15, 3,
                                            "func",  TL_MSG_PARAM_STR, func,
                                            "JobID", TL_MSG_PARAM_INT, jid,
                                            "Drive", TL_MSG_PARAM_STR, drive );
			c = ETIDN;	/* non existing drive */
			goto reply;
		}
		if (strcmp (tunp->dgn, dgn) != 0) {
			usrmsg (func, TP013);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 13, 2,
                                            "func",  TL_MSG_PARAM_STR, func,
                                            "JobID", TL_MSG_PARAM_INT, jid );                
			c = ETIDG;	/* device group name does not match */
			goto reply;
		}
		if (rrtp->dg[j].used >= rrtp->dg[j].rsvd) {
			usrmsg (func, TP012);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 12, 3,
                                            "func",  TL_MSG_PARAM_STR, func,
                                            "JobID", TL_MSG_PARAM_INT, jid,
                                            "Drive", TL_MSG_PARAM_STR, drive );                
			c = ETNDV;	/* request would exceed # of drives rsvd */
			goto reply;
		}
		if (chk_den (tunp, den, &cdevp) != 0) {
			usrmsg (func, TP015);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 15, 3,
                                            "func",  TL_MSG_PARAM_STR, func,
                                            "JobID", TL_MSG_PARAM_INT, jid,
                                            "Drive", TL_MSG_PARAM_STR, drive );
			c = ETIDN;	/* drive does not have requested density */
			goto reply;
		}
		if (tunp->asn != 0 || tunp->up != 1 ||
		    (*tunp->vid && (strcmp (tunp->vid, vid) || strcmp (tunp->vsn, vsn)))) {
			usrmsg (func, TP057, drive);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 57, 5,
                                            "func"     , TL_MSG_PARAM_STR, func,
                                            "Drive"    , TL_MSG_PARAM_STR, drive,
                                            "Assigned" , TL_MSG_PARAM_INT, tunp->asn,
                                            "Up"       , TL_MSG_PARAM_INT, tunp->up,
                                            "JobID"    , TL_MSG_PARAM_INT, jid );
			c = EBUSY;
			goto reply;
		}

	} else {	/* non specific drive request */
		if (rrtp->dg[j].used >= rrtp->dg[j].rsvd) {
			usrmsg (func, TP012);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 12, 3,
                                            "func" ,  TL_MSG_PARAM_STR, func,
                                            "JobID", TL_MSG_PARAM_INT , jid,
                                            "Drive", TL_MSG_PARAM_STR , drive );
			c = ETNDV;	/* request would exceed # of drives rsvd */
			goto reply;
		}

		/* check if there exists one drive not assigned, not down
		 * with the requested options in this device group.
		 * allocate drives in a circular way.
		 */

		found = 0;
		tunp = dpdg[j].next;
		do {
			if (tunp->asn == 0 && tunp->up == 1 &&
			    (! *tunp->vid ||
				(strcmp (tunp->vid, vid) == 0 &&
				 strcmp (tunp->vsn, vsn) == 0)) &&
			    chk_den (tunp, den, &cdevp) == 0) {
				found = 1;
				break;
			}
			tunp = (tunp == dpdg[j].last) ? dpdg[j].first : tunp + 1;
		} while (tunp != dpdg[j].next);
		if (! found) {
			for (tunp = dpdg[j].first; tunp <= dpdg[j].last; tunp++) {
				if (chk_den (tunp, den, &cdevp) == 0) {
					found = 1;
					break;
				}
			}
			if (! found) {
				usrmsg (func, TP015);
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 15, 2,
                                                    "func" , TL_MSG_PARAM_STR, func,
                                                    "Drive", TL_MSG_PARAM_STR, drive );                
				c = ETIDN;	/* no drive with requested density */
				goto reply;
			} else {
				usrmsg (func, TP058);
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 58, 1,
                                                    "func", TL_MSG_PARAM_STR, func );
				c = EBUSY;
				goto reply;
			}
		}
		dpdg[j].next = (tunp == dpdg[j].last) ? dpdg[j].first : tunp + 1;
	}

	rrtp->dg[j].used++;	/* increment usage count in user reservation table */
	tpdrrt.dg[j].used++;	/* increment usage count in global reservation table */
	tunp->asn = 1;
	tunp->asn_time = time (0);
        tunp->unasn_time = 0;
	tunp->cdevp = cdevp;
	tunp->uid = uid;
	tunp->gid = gid;
	strcpy (tunp->acctname, acctname);
	tunp->jid = jid;

	tapeacct (TPASSIGN, tunp->uid, tunp->gid, tunp->jid,
		tunp->dgn, tunp->drive, vid, 0, 0);

	tunp->filp = (struct tpfil *) calloc (1, sizeof(struct tpfil));

	strcpy (tunp->filp->path, path);
 
	/* create special device */

	if (mknod (path, 0020700, tunp->cdevp->majmin) < 0) {
		if (errno == EEXIST) {
			usrmsg (func, TP022);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 22, 1,
                                            "func", TL_MSG_PARAM_STR, func );
                }
		else {
			usrmsg (func, "TP002 - %s : mknod error : %s\n",
			    path, strerror(errno));
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 5,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Message", TL_MSG_PARAM_STR, "mknod error",
                                            "path",    TL_MSG_PARAM_STR, path,
                                            "JobID",   TL_MSG_PARAM_INT, jid,
                                            "Error",   TL_MSG_PARAM_STR, strerror(errno) );                                        
                        
                }
		c = errno;
		goto reply;
	}
	if (chown (path, uid, gid) < 0) {
		usrmsg (func, "TP002 - %s : chown error : %s\n",
		    path, strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 5,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "chown error",
                                    "path",    TL_MSG_PARAM_STR, path,
                                    "JobID",   TL_MSG_PARAM_INT, jid,
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );                                        
		c = errno;
		goto reply;
	}

	/* fork and exec the process to process tape mount and VOL1 checking */

	c = 0;
	tplogit (func, "forking a mounttape process\n");
        tl_tpdaemon.tl_log( &tl_tpdaemon, 108, 2,
                            "func",    TL_MSG_PARAM_STR, func,
                            "Message", TL_MSG_PARAM_STR, "Forking a mounttape process" );                        

	tunp->mntovly_pid = fork ();
	pid = tunp->mntovly_pid;
        
        /* reset the rls retry counter */
        tunp->rlsrtryctr = 0;

	if (pid < 0) {
		usrmsg (func, TP002, "fork", strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 5,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "fork",
                                    "path",    TL_MSG_PARAM_STR, path,
                                    "JobID",   TL_MSG_PARAM_INT, jid,
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );
		c = errno;
	} else if (pid == 0) {	/* we are in the child */

		/* set up the parameters for the process mounttape */

		char arg_den[4], arg_gid[11], arg_jid[11], arg_lbl[2], arg_mode[2];
		char arg_prelabel[3], arg_rpfd[3], arg_side[2];
		char arg_tpmounted[2], arg_uid[11], arg_ux[3], arg_vdqmid[10];
		char progfullpath[CA_MAXPATHLEN+1];
		const int lblcode = AUL; /* Only the AUL label type is supported */

		sprintf (progfullpath, "%s/mounttape", BIN);
		sprintf (arg_rpfd, "%d", rpfd);
		sprintf (arg_uid, "%d", tunp->uid);
		sprintf (arg_gid, "%d", tunp->gid);
		sprintf (arg_jid, "%d", tunp->jid);
		sprintf (arg_ux, "%d", tunp->ux);
		sprintf (arg_mode, "%d", mode);
		sprintf (arg_lbl, "%d", lblcode);
		sprintf (arg_den, "%d", tunp->cdevp->den);
		sprintf (arg_prelabel, "%d", prelabel);
		sprintf (arg_vdqmid, "%d", vdqm_reqid);
		sprintf (arg_tpmounted, "%d", tunp->vid[0] ? 1 : 0);
		sprintf (arg_side, "%d", side);

		tplogit (func, "execing mounttape, pid=%d\n", getpid());
                
                tplogit (func, "mounttape %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
                         tunp->drive, vid, tunp->cdevp->dvn, arg_rpfd, arg_uid, arg_gid, 
                         rrtp->user, tunp->acctname, arg_jid, arg_ux, tunp->dgn,
                         tunp->devtype, "tape", tunp->loader, arg_mode, arg_lbl,
                         vsn, tunp->filp->path, arg_den, arg_prelabel,
                         arg_vdqmid, arg_tpmounted, arg_side, clienthost, NULL); 

                /* tl_log msg wouldn't be seen in the DB due to the execlp() */

		execlp (progfullpath, "mounttape", tunp->drive, vid, 
			tunp->cdevp->dvn, arg_rpfd, arg_uid, arg_gid,
			rrtp->user, tunp->acctname, arg_jid, arg_ux, tunp->dgn,
			tunp->devtype, "tape", tunp->loader, arg_mode,
			arg_lbl, vsn, tunp->filp->path, arg_den, arg_prelabel,
			arg_vdqmid, arg_tpmounted, arg_side, clienthost, NULL);
		tplogit (func, "TP002 - mounttape : execlp error : %s\n",
		    strerror(errno));

                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 4,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "mounttape : execlp error",
                                    "JobID",   TL_MSG_PARAM_INT, jid,
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );

                tl_tpdaemon.tl_exit( &tl_tpdaemon, 0 );
                exit (errno);                
	}
reply:
	if (c)
		sendrep (rpfd, TAPERC, c);
	else
		close (rpfd);
}

static void procposreq(char *req_data,
                char *clienthost)
{
	int blksize;
	unsigned char blockid[4];
	int c;
	int errflg = 0;
	char *fid;
	struct tpfil *filp;
	int filstat;
	int flags;
	int found;
	int fsec;
	int fseq;
	char *fsid;
	gid_t gid;
	unsigned int i;
	int jid;
	int lrecl;
	int method;
	char *path;
	int pid;
	int Qfirst;
	int Qlast;
	char *rbp;
	char *recfm;
	int retentd;
	struct tprrt *rrtp;
	struct tptab *tunp;
	uid_t uid;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "position", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "position",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_LONG (rbp, jid);
	unmarshall_STRING (rbp, path);
	unmarshall_WORD (rbp, method);
	if (method == TPPOSIT_FSEQ || method == TPPOSIT_BLKID) {
		unmarshall_LONG (rbp, fseq);
	} else if (method == TPPOSIT_EOI)
		fseq = -1;
	else if (method == TPPOSIT_FID)
		fseq = -2;
	else
		fseq = 0;
	if (method == TPPOSIT_FSEQ || method == TPPOSIT_FID) {
		unmarshall_WORD (rbp, fsec);
	} else
		fsec = 1;
	if (method == TPPOSIT_BLKID) {
		unmarshall_OPAQUE (rbp, blockid, 4);
	} else
		memset (blockid, 0, 4);
	unmarshall_LONG (rbp, Qfirst);
	unmarshall_LONG (rbp, Qlast);
	unmarshall_WORD (rbp, filstat);
	unmarshall_STRING (rbp, fid);
	unmarshall_STRING (rbp, fsid);
	unmarshall_STRING (rbp, recfm);
	unmarshall_LONG (rbp, blksize);
	unmarshall_LONG (rbp, lrecl);
	unmarshall_WORD (rbp, retentd);
	unmarshall_WORD (rbp, flags);

	/* check if reserv done */

	rrtp = tpdrrt.next;
	while (rrtp) {
		if (jid == rrtp->jid) break;
		rrtp = rrtp->next;
	}
	if (rrtp == 0) {
		usrmsg (func, TP014);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 14, 1,
                                    "func", TL_MSG_PARAM_STR, func ); 
		c = ETNRS;		/* reserve not done */
		goto reply;
	}

	/* check if the tape is mounted */

	c = 0;
	found = 0;
	tunp = tptabp;
	for (i = 0; i < nbtpdrives; i++) {
		if (tunp->asn != 0 &&
		    strcmp (tunp->filp->path, path) == 0) {
			found = 1;
			break;
		}
		tunp++;
	}
	if (! found) {
		usrmsg (func, TP037, path);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 37, 2,
                                    "func", TL_MSG_PARAM_STR, func,
                                    "path", TL_MSG_PARAM_STR, path );
		errflg++;
	}

	/* check validity of the other arguments */

	if (method != TPPOSIT_FSEQ && method != TPPOSIT_FID &&
	    method != TPPOSIT_EOI && method != TPPOSIT_BLKID) {
		usrmsg (func, TP006, "positioning method");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "positioning method" );
		errflg++;
	}
	if (method == TPPOSIT_FSEQ && fseq <= 0) {
		usrmsg (func, TP006, "file sequence number");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "file sequence number" );
		errflg++;
	}
	if (method == TPPOSIT_FSEQ && fsec <= 0) {
		usrmsg (func, TP006, "file section number");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "file section number" );
		errflg++;
	}
	if (method == TPPOSIT_FID && *fid == '\0') {
		usrmsg (func, TP007);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 7, 1,
                                    "func", TL_MSG_PARAM_STR, func );
		errflg++;
	}
	if (method == TPPOSIT_FID && (tunp->lblcode == NL || tunp->lblcode == BLP)) {
		usrmsg (func, TP064);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 64, 1,
                                    "func", TL_MSG_PARAM_STR, func );
		errflg++;
	}
	if (filstat != APPEND && filstat != NEW_FILE && filstat != CHECK_FILE &&
	    filstat != NOFILECHECK) {
		usrmsg (func, TP006, "file status");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "file status" );
		errflg++;
	}
	if (method == TPPOSIT_BLKID && filstat == APPEND) {
		usrmsg (func, TP060);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 60, 1, 
                                    "func", TL_MSG_PARAM_STR, func );
		errflg++;
	}
	if ((filstat == NEW_FILE || filstat == APPEND) &&
	    tunp->mode == WRITE_DISABLE) {
		usrmsg (func, TP061);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 61, 1,
                                    "func", TL_MSG_PARAM_STR, func );
		errflg++;
	}
	if (*recfm && strcmp (recfm, "F") && strcmp (recfm, "FB") &&
	    strcmp (recfm, "FBS")  && strcmp (recfm, "FS") &&
	    strcmp (recfm, "U")) {
		usrmsg (func, TP006, "record format");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "record format" );
		errflg++;
	}
	if (flags & IGNOREEOI &&
	    (tunp->lblcode == AL || tunp->lblcode == SL)) {
		usrmsg (func, TP049);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 49, 1,
                                    "func", TL_MSG_PARAM_STR, func );
		errflg++;
	}
	if (errflg) {
		c = EINVAL;
		goto reply;
	}
	if ((flags & NOPOS) && tunp->filp->cfseq == 0)
		flags &= ~NOPOS;

	/* build tape file description */

	filp = tunp->filp;
	filp->blksize = blksize;
	strcpy (filp->fid, fid);
	filp->filstat = filstat;
	filp->fsec = fsec;
	filp->fseq = fseq;
	memcpy (filp->blockid, blockid, 4);
	filp->lrecl = lrecl;
	filp->Qfirst = Qfirst;
	filp->Qlast = Qlast;
	strcpy (filp->recfm, recfm);
	filp->retentd = retentd;
	filp->flags = flags;

	/* fork and exec the process to process tape positioning */

	c = 0;
	tplogit (func, "forking a posovl process\n");
        tl_tpdaemon.tl_log( &tl_tpdaemon, 108, 2,
                            "func",    TL_MSG_PARAM_STR, func,
                            "Message", TL_MSG_PARAM_STR, "Forking a posovl process" );                        

	tunp->mntovly_pid = fork ();
	pid = tunp->mntovly_pid;
	if (pid < 0) {
		usrmsg (func, TP002, "fork", strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 4,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "fork", 
                                    "JobID",   TL_MSG_PARAM_INT, jid,
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );
		c = ETSYS;
	} else if (pid == 0) {	/* we are in the child */

		/* set up the parameters for the process posovl */

		char arg_blksize[9], arg_blockid[9], arg_cfseq[11], arg_den[4];
		char arg_filstat[2], arg_flags[3], arg_fsec[5], arg_fseq[11];
		char arg_gid[11], arg_jid[11], arg_lbl[2], arg_lrecl[9];
		char arg_method[2], arg_mode[2], arg_Qfirst[11], arg_Qlast[11];
		char arg_retentd[5], arg_rpfd[3], arg_uid[11], arg_ux[3];
		char progfullpath[CA_MAXPATHLEN+1];

		sprintf (progfullpath, "%s/posovl", BIN);
		sprintf (arg_rpfd, "%d", rpfd);
		sprintf (arg_uid, "%d", uid);
		sprintf (arg_gid, "%d", gid);
		sprintf (arg_jid, "%d", jid);
		sprintf (arg_ux, "%d", tunp->ux);
		sprintf (arg_mode, "%d", tunp->mode);
		sprintf (arg_lbl, "%d", tunp->lblcode);
		sprintf (arg_blockid, "%02x%02x%02x%02x", filp->blockid[0],
		    filp->blockid[1], filp->blockid[2], filp->blockid[3]);
		sprintf (arg_cfseq, "%d", filp->cfseq);
		sprintf (arg_filstat, "%d", filstat);
		sprintf (arg_fsec, "%d", filp->fsec);
		sprintf (arg_fseq, "%d", filp->fseq);
		sprintf (arg_method, "%d", method);
		sprintf (arg_Qfirst, "%d", Qfirst);
		sprintf (arg_Qlast, "%d", Qlast);
		sprintf (arg_retentd, "%d", retentd);
		sprintf (arg_blksize, "%d", blksize);
		sprintf (arg_lrecl, "%d", lrecl);
		sprintf (arg_den, "%d", tunp->cdevp->den);
		sprintf (arg_flags, "%d", flags);

		tplogit (func, "execing posovl, pid=%d\n", getpid());
                /* tl_log msg wouldn't be seen in the DB due to the execlp() */

		execlp (progfullpath, "posovl", tunp->drive, tunp->vid, 
			arg_rpfd, arg_uid, arg_gid, rrtp->user, arg_jid, arg_ux,
			tunp->dgn, tunp->devtype, "tape", arg_mode, arg_lbl,
			tunp->vsn, arg_blockid, arg_cfseq, filp->fid, arg_filstat,
			arg_fsec, arg_fseq, arg_method, filp->path, arg_Qfirst,
			arg_Qlast, arg_retentd, filp->recfm, arg_blksize, arg_lrecl,
			arg_den, arg_flags, fsid, domainname, NULL);
		tplogit (func, "TP002 - posovl : execlp error : %s\n",
		    strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 4,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "posovl : execlp error",
                                    "JobID",   TL_MSG_PARAM_INT, jid,
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );                        

                tl_tpdaemon.tl_exit( &tl_tpdaemon, 0 );
		exit (errno);
	}
reply:
	if (c)
		sendrep (rpfd, TAPERC, c);
	else
		close (rpfd);
}

static void procrlsreq(char *req_data,
                char *clienthost)
{
	int c, n;
        unsigned int i;
	int errflg = 0;
	int flags;
	gid_t gid;
	int jid;
	char *path;
	char *rbp;
	struct tprrt *rrtp;
	struct tptab *tunp;
	uid_t uid;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "rls", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 74, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "rls",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_LONG (rbp, jid);
        unmarshall_WORD (rbp, flags);
	unmarshall_STRING (rbp, path);

	tplogit (func, "rls request path=<%s>, flags=%d\n", path, flags);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 108, 4, 
                            "func",    TL_MSG_PARAM_STR, func,
                            "Message", TL_MSG_PARAM_STR, "rls request",
                            "Path",    TL_MSG_PARAM_STR, path,
                            "Flags",   TL_MSG_PARAM_INT, flags );                        
	c = 0;

	/* check validity of the arguments */

	if (flags <= 0 || flags > 63 ||
	    (flags & TPRLS_ALL && flags & TPRLS_KEEP_RSV) ||
	    (flags & TPRLS_ALL && flags & TPRLS_PATH)) {
		usrmsg (func, TP006, "rls flags");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "rls flags" );
		errflg++;
	}
	if ((flags & TPRLS_PATH) && ! *path) {
		usrmsg (func, TP036, flags);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 36, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "flags", TL_MSG_PARAM_INT, flags );
		errflg++;
	}
	if (errflg) {
		c = EINVAL;
		goto reply;
	}

	/* check if reserv done */

	rrtp = tpdrrt.next;
	while (rrtp) {
		if (jid == rrtp->jid) break;
		rrtp = rrtp->next;
	}
	if (rrtp == 0)
		goto reply;

	if (flags & TPRLS_PATH) { /* release a specific path */
		tunp = tptabp;
		for (i = 0; i < nbtpdrives; i++) {
			if (tunp->jid == jid && tunp->asn != 0 &&
				strcmp (tunp->filp->path, path) == 0) {
				if (tunp->asn == 1) {
					c = reldrive (tunp, rrtp->user,
					    (flags & TPRLS_NOWAIT) ? -1 : rpfd,
					    (tunp->up <= 0) ?
						flags | TPRLS_UNLOAD : flags);
					if (c == 0) rrtp->unldcnt++;
				} else {	/* rls in progress */
					if ((flags & TPRLS_KEEP_RSV) == 0)
						tunp->asn = -2;
					c = ETRLSP;
				}
				break;
			}
			tunp++;
		}
	} else {			/* release everything */
		tunp = tptabp;
		for (i = 0; i < nbtpdrives; i++) {
			if (tunp->jid == jid && tunp->asn != 0) {
				if (tunp->asn == 1) {
					n = reldrive (tunp, rrtp->user,
					    (flags & TPRLS_NOWAIT) ? -1 : rpfd,
					    (tunp->up <= 0) ?
						flags | TPRLS_UNLOAD : flags);
					if (n == 0) rrtp->unldcnt++;
					else c = n;
				} else {	/* rls in progress */
					tunp->asn = -2;
					c = ETRLSP;
				}
			}
			tunp++;
		}
		if (rrtp->unldcnt == 0) {	/* no drive assigned, free only rsv */
			(rrtp->prev)->next = rrtp->next;
			if (rrtp->next) (rrtp->next)->prev = rrtp->prev;
			free (rrtp->dg);
			free (rrtp);
			rrtp = 0;
			nbjobs--;
		}
	}
reply:
	if (c || rrtp == 0 || rrtp->unldcnt == 0 || flags & TPRLS_NOWAIT)
		sendrep (rpfd, TAPERC, c);
}

static void procrsltreq(char *req_data,
                 char *clienthost)
{
	int c;
        unsigned int i;
	struct tpdev *cdevp;
	gid_t gid;
	int jid;
	char *newdrive;
	char *olddrive;
	struct tptab *oldtunp;
	char *rbp;
	char repbuf[REPBUFSZ];
	struct tprrt *rrtp;
	char *sbp;
	struct tptab *tunp;
	uid_t uid;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "reselect", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "reselect",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_LONG (rbp, jid);
	unmarshall_STRING (rbp, olddrive);
	unmarshall_STRING (rbp, newdrive);

	c = 0;

	/* check if reserv done */

	rrtp = tpdrrt.next;
	while (rrtp) {
		if (jid == rrtp->jid) break;
		rrtp = rrtp->next;
	}
	if (rrtp == 0) {
		usrmsg (func, TP014);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 14, 1,
                                    "func", TL_MSG_PARAM_STR, func );
		c = ETNRS;	/* reserve not done */
		goto reply;
	}

	/* Look for drive previously assigned */

	oldtunp = tptabp;
	for (i = 0; i < nbtpdrives; i++) {
		if (oldtunp->jid == jid && oldtunp->asn == 1 &&
			strcmp (oldtunp->drive, olddrive) == 0) break;
		oldtunp++;
	}

	/* Look for requested drive */

	tunp = tptabp;
	for (i = 0; i < nbtpdrives; i++) {
		if (strcmp (tunp->drive, newdrive) == 0) break;
		tunp++;
	}
	if (i == nbtpdrives) {
		usrmsg (func, TP015);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 15, 2,
                                    "func" , TL_MSG_PARAM_STR, func,
                                    "Drive", TL_MSG_PARAM_STR, newdrive );
		c = ETIDN;	/* non existing drive */
		goto reply;
	}
	if (strcmp (tunp->dgn, oldtunp->dgn) != 0) {
		usrmsg (func, TP013);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 13, 1,
                                    "func", TL_MSG_PARAM_STR, func );
		c = ETIDG;	/* device group name does not match */
		goto reply;
	}
	if (chk_den (tunp, oldtunp->cdevp->den, &cdevp) != 0) {
		usrmsg (func, TP015);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 15, 2,
                                    "func" , TL_MSG_PARAM_STR, func,
                                    "Drive", TL_MSG_PARAM_STR, newdrive );
		c = ETIDN;	/* drive does not have requested density */
		goto reply;
	}
	if (tunp->asn != 0 || tunp->up != 1) {
		usrmsg (func, TP057, newdrive);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 57, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "newdrive", TL_MSG_PARAM_STR, newdrive );
		c = EBUSY;
		goto reply;
	}

	tunp->asn = 1;		/* assign new drive */
	tunp->asn_time = time (0);
        tunp->unasn_time = 0;
	tunp->cdevp = cdevp;
	tunp->filp = oldtunp->filp;
	tunp->uid = oldtunp->uid;
	tunp->gid = oldtunp->gid;
	strcpy (tunp->acctname, oldtunp->acctname);
	tunp->jid = oldtunp->jid;
	tunp->mntovly_pid = oldtunp->mntovly_pid;

	oldtunp->asn = 0;          /* unassign previous drive */
	oldtunp->asn_time = 0;
        oldtunp->unasn_time = time(0);
	oldtunp->filp = 0;
	oldtunp->vid[0] = '\0';
	oldtunp->vsn[0] = '\0';
	oldtunp->tobemounted = 0;

	unlink (tunp->filp->path);	/* delete previous path */

	tapeacct (TPFREE, tunp->uid, tunp->gid, tunp->jid,
		oldtunp->dgn, oldtunp->drive, "", 0, 0);
	tapeacct (TPASSIGN, tunp->uid, tunp->gid, tunp->jid,
		tunp->dgn, tunp->drive, "", 0, 0);
	if (oldtunp->up <= 0) {
		(void) confdrive (tunp, -1, CONF_DOWN, -oldtunp->up);
		oldtunp->up = 0;
	}

	/* create special device */

	if (mknod (tunp->filp->path, 0020700, tunp->cdevp->majmin) < 0) {
		usrmsg (func, "TP002 - %s : mknod error : %s\n",
		    tunp->filp->path, strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 5,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "mknod error",
                                    "path",    TL_MSG_PARAM_STR, tunp->filp->path,
                                    "JobID",   TL_MSG_PARAM_INT, tunp->jid,
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );
		c = errno;
		goto reply;
	}
	if (chown (tunp->filp->path, tunp->uid, tunp->gid) < 0) {
		usrmsg (func, "TP002 - %s : chown error : %s\n",
		    tunp->filp->path, strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 5,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "chown error",
                                    "path",    TL_MSG_PARAM_STR, tunp->filp->path,
                                    "JobID",   TL_MSG_PARAM_INT, tunp->jid,
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );
		c = ETSYS;
		goto reply;
	}

	sbp = repbuf;
	marshall_WORD (sbp, tunp->ux);
	marshall_STRING (sbp, tunp->loader);
	marshall_STRING (sbp, tunp->cdevp->dvn);
	sendrep (rpfd, MSG_DATA, sbp - repbuf, repbuf);
reply:
	sendrep (rpfd, TAPERC, c);
}

static void procrsvreq(char *req_data,
                char *clienthost)
{
	int c;
	int count;
	char *dgn;
	gid_t gid;
	int i;
        unsigned int j;
	int jid;
	int num;
	struct tprrt *orrtp;
	struct tprrt *prev;
	struct passwd *pwd;
	char *rbp;
	struct tprrt *rrtp = NULL;
	uid_t uid;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "reserv", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 71, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "reserv",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_LONG (rbp, jid);
	unmarshall_WORD (rbp, count);

	if ((pwd = getpwuid (uid)) == NULL) {
		usrmsg (func, TP063, uid);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 63, 2,
                                    "func", TL_MSG_PARAM_STR, func,
                                    "UID",  TL_MSG_PARAM_UID, uid );
		c = SEUSERUNKN;
		goto reply;
	}

	c = 0;
	orrtp = &tpdrrt;
	while (orrtp) {
		if (jid == orrtp->jid) {
			usrmsg (func, TP010);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 10, 1,
                                            "func", TL_MSG_PARAM_STR, func );
			c = ETRSV;	/* resources already reserved for this job */
			goto reply;
		}
		prev = orrtp;
		orrtp = orrtp->next;
	}
	if (count <= 0) {
		usrmsg (func, TP006, "count");
                tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 2,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "count" );
		c = EINVAL;
		goto reply;
	}
	rrtp = (struct tprrt *) calloc (1, sizeof(struct tprrt));
	rrtp->uid = uid;
	strcpy (rrtp->user, pwd->pw_name);
	rrtp->jid = jid;
	rrtp->dg = (struct tpdgrt *) calloc (nbdgp, sizeof(struct tpdgrt));
	for (j = 0; j < nbdgp; j++) {
		strcpy (rrtp->dg[j].name, tpdrrt.dg[j].name);
	}
	for (i = 0; i < count; i++) {
		unmarshall_STRING (rbp, dgn);
		unmarshall_WORD (rbp, num);
		for (j = 0; j < nbdgp; j++) {
			if (strcmp (rrtp->dg[j].name, dgn) == 0) {
				if (num <= tpdrrt.dg[j].rsvd)
					break;
				else {
					usrmsg (func, TP012);
                                        tl_tpdaemon.tl_log( &tl_tpdaemon, 12, 1,
                                                            "func", TL_MSG_PARAM_STR, func );                
					c = ETNDV;	/* too many drives requested */
					goto reply;
				}
			}
		}
		if (j == nbdgp) {
			usrmsg (func, TP013);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 13, 1,
                                            "func", TL_MSG_PARAM_STR, func );                
			c = ETIDG;	/* invalid device group name */
			goto reply;
		}
		if (num <= 0) {
			usrmsg (func, TP006, "num");
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 6, 2,
                                            "func",    TL_MSG_PARAM_STR, func,
                                            "Message", TL_MSG_PARAM_STR, "num" );
			c = EINVAL;
			goto reply;
		}
		rrtp->dg[j].rsvd = num;
		rrtp->totrsvd += num;	/* total user reservation */
	}
	prev->next = rrtp;
	rrtp->prev = prev;
	nbjobs++;
reply:
	if (c) {
		if (rrtp) {
			if (rrtp->dg)
				free (rrtp->dg);
			free (rrtp);
		}
	}
	sendrep (rpfd, TAPERC, c);
}

static void procrstatreq(char *req_data,
                  char *clienthost)
{
	gid_t gid;
	unsigned int j;
	char *rbp;
	char repbuf[REPBUFSZ];
	struct tprrt *rrtp;
	char *sbp;
	uid_t uid;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "reservation status", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "reservation status",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        

	sbp = repbuf;
	rrtp = &tpdrrt;
	marshall_WORD (sbp, nbjobs);
	while (rrtp) {
		marshall_LONG (sbp, rrtp->uid);
		marshall_LONG (sbp, rrtp->jid);
		marshall_WORD (sbp, nbdgp);
		for (j = 0; j < nbdgp; j++) {
			marshall_STRING (sbp, rrtp->dg[j].name);
			marshall_WORD (sbp, rrtp->dg[j].rsvd);
			marshall_WORD (sbp, rrtp->dg[j].used);
		}
		rrtp = rrtp->next;
	}
	sendrep (rpfd, MSG_DATA, sbp - repbuf, repbuf);
	sendrep (rpfd, TAPERC, 0);
}

static void procstatreq(char *req_data,
                 char *clienthost)
{
	gid_t gid;
	unsigned int j;
	static char labels[6][4] = {"", "al", "nl", "sl", "blp", "aul"};
	char *rbp;
	char repbuf[REPBUFSZ];
	char *sbp;
	struct tptab *tunp;
	uid_t uid;
	int zero = 0;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "status", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "status",
                            "UID",        TL_MSG_PARAM_INT, uid,
                            "GID",        TL_MSG_PARAM_INT, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        

	sbp = repbuf;
	tunp = tptabp;
	marshall_WORD (sbp, nbtpdrives);
	for (j = 0; j < nbtpdrives; j++) {
		marshall_LONG (sbp, tunp->uid);
		marshall_LONG (sbp, tunp->jid);
		marshall_STRING (sbp, tunp->dgn);
		marshall_WORD (sbp, tunp->up);
		marshall_WORD (sbp, tunp->asn);
		marshall_LONG (sbp, tunp->asn_time);
		marshall_STRING (sbp, tunp->drive);
		marshall_WORD (sbp, tunp->mode);
		marshall_STRING (sbp, labels[tunp->lblcode]);
		marshall_WORD (sbp, tunp->tobemounted);
		marshall_STRING (sbp, tunp->vid);
		marshall_STRING (sbp, tunp->vsn);
		if (tunp->filp) {
			marshall_LONG (sbp, tunp->filp->cfseq);
		} else {
			marshall_LONG (sbp, zero);
		}
		tunp++;
	}
	sendrep (rpfd, MSG_DATA, sbp - repbuf, repbuf);
	sendrep (rpfd, TAPERC, 0);
}

static void procufilreq(char *req_data,
                 char *clienthost)
{
	int blksize;
	unsigned char blockid[4];
	int cfseq;
	char *fid;
	gid_t gid;
	int jid;
	int lrecl;
	char *rbp;
	char *recfm;
	struct tptab *tunp;
	uid_t uid;
	int ux;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "update file info", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func, 
                            "Message",    TL_MSG_PARAM_STR, "update file info",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_LONG (rbp, jid);
	unmarshall_WORD (rbp, ux);
	unmarshall_LONG (rbp, blksize);
	unmarshall_OPAQUE (rbp, blockid, 4);
	unmarshall_LONG (rbp, cfseq);
	unmarshall_STRING (rbp, fid);
	unmarshall_LONG (rbp, lrecl);
        unmarshall_STRING (rbp, recfm);

	tunp = tptabp + ux;
	tunp->filp->blksize = blksize;
	memcpy (tunp->filp->blockid, blockid, 4);
	tunp->filp->cfseq = cfseq;
	strcpy (tunp->filp->fid, fid);
	tunp->filp->lrecl = lrecl;
	strcpy (tunp->filp->recfm, recfm);
	close (rpfd);
}

static void procuvsnreq(char *req_data,
                 char *clienthost)
{
	gid_t gid;
	int jid;
	int lblcode;
	int mode;
	char *rbp;
	int tobemounted;
	struct tptab *tunp;
	uid_t uid;
	int ux;
	char *vid, *vsn;

	rbp = req_data;
	unmarshall_LONG (rbp, uid);
	unmarshall_LONG (rbp, gid);

	RESETID(uid,gid);

	tplogit (func, TP056, "update vsn", uid, gid, clienthost);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 56, 5,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "update vsn",
                            "UID",        TL_MSG_PARAM_UID, uid,
                            "GID",        TL_MSG_PARAM_GID, gid,
                            "Clienthost", TL_MSG_PARAM_STR, clienthost );                        
	unmarshall_LONG (rbp, jid);
	unmarshall_WORD (rbp, ux);
	unmarshall_STRING (rbp, vid);
	unmarshall_STRING (rbp, vsn);
	unmarshall_WORD (rbp, tobemounted);
	unmarshall_WORD (rbp, lblcode);
        unmarshall_WORD (rbp, mode);

	tunp = tptabp + ux;
	strcpy (tunp->vid, vid);
	strcpy (tunp->vsn, vsn);
	tunp->tobemounted = tobemounted;
	tunp->lblcode = lblcode;
	tunp->mode = mode;
	close (rpfd);
}

int reldrive(struct tptab *tunp,
             char *user,
             int rpfd,
             int rlsflags)
{
	int c;
	int found;
	int pid;
	struct rlsq *prev, *rqp;

	c = 0;
	unlink (tunp->filp->path);	/* delete user path to the drive */
	tunp->asn = -1;			/* drive is unloading */

	/* fork and exec the process to process tape unload */

	tplogit (func, "forking an rlstape process\n");
        tl_tpdaemon.tl_log( &tl_tpdaemon, 108, 2,
                            "func",    TL_MSG_PARAM_STR, func,
                            "Message", TL_MSG_PARAM_STR, "Forking an rlstape process" );                        

	pid = fork ();
	tunp->rlsovly_pid = pid;
	if (pid < 0) {
		usrmsg (func, TP002, "fork", strerror(errno));
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "fork",
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );
		c = ETSYS;
	} else if (pid == 0) {	/* we are in the child */

		/* set up the parameters for the process rlstape */

		char arg_den[4], arg_gid[11], arg_jid[11], arg_rlsflags[3];
		char arg_mode[2], arg_rpfd[3], arg_uid[11], arg_ux[3];
		char progfullpath[CA_MAXPATHLEN+1];

		sprintf (progfullpath, "%s/rlstape", BIN);
		sprintf (arg_rpfd, "%d", rpfd);
		sprintf (arg_uid, "%d", tunp->uid);
		sprintf (arg_gid, "%d", tunp->gid);
		sprintf (arg_jid, "%d", tunp->jid);
		sprintf (arg_ux, "%d", tunp->ux);
		sprintf (arg_rlsflags, "%d", rlsflags);
		sprintf (arg_mode, "%d", tunp->mode);
		sprintf (arg_den, "%d", tunp->cdevp->den);

		tplogit (func, "execing rlstape, pid=%d\n", getpid());
                tplogit (func, "rlstape %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
                         tunp->drive, tunp->vid, tunp->cdevp->dvn, arg_rpfd, arg_uid, arg_gid, 
                         user, tunp->acctname, arg_jid, arg_ux, arg_rlsflags, tunp->dgn,
                         tunp->devtype, "tape", tunp->loader, arg_mode, arg_den);
                /* tl_log msg wouldn't be seen in the DB due to the execlp() */

		execlp (progfullpath, "rlstape", tunp->drive, tunp->vid,
			tunp->cdevp->dvn, arg_rpfd, arg_uid, arg_gid, user,
			tunp->acctname, arg_jid, arg_ux, arg_rlsflags, tunp->dgn,
			tunp->devtype, "tape", tunp->loader, arg_mode,
			arg_den, NULL);
		tplogit (func, "TP002 - rlstape : execlp error : %s\n",
		    strerror(errno));
                
                tl_tpdaemon.tl_log( &tl_tpdaemon, 2, 3,
                                    "func",    TL_MSG_PARAM_STR, func,
                                    "Message", TL_MSG_PARAM_STR, "rlstape : execlp error",
                                    "Error",   TL_MSG_PARAM_STR, strerror(errno) );                        

                tl_tpdaemon.tl_exit( &tl_tpdaemon, 0 );
		exit (errno);
	}
	if (c == 0 && rpfd >= 0) {	/* increment count of unload request */
					/* if not called from clean4jobdied */
		found = 0;
		prev = 0;
		rqp = rlsqp;
		while (rqp) {
			if (rqp->rpfd == rpfd) {
				found = 1;	/* other unloads for same req */
				break;
			}
			prev = rqp;
			rqp = rqp->next;
		}
		if (! found) {
			rqp = (struct rlsq *) calloc (1, sizeof(struct rlsq));
			if (prev) {
				prev->next = rqp;
				rqp->prev = prev;
			} else
				rlsqp = rqp;
			rqp->rpfd = rpfd;
		}
		rqp->unldcnt++;
	}
	return (c);
}

void wait4child()
{
}

void check_child_exit()
{
	unsigned int i;
	int pid;
	struct confq *rqp;
	int status;
	struct tptab *tunp;
        struct tprrt *rrtp;

	while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
        tplogit (func, "process %d exiting with status %x\n",
                 pid, status & 0xFFFF);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 108, 4,
                            "func",    TL_MSG_PARAM_STR, func,
                            "Message", TL_MSG_PARAM_STR, "Process exiting",
                            "PID",     TL_MSG_PARAM_INT, pid,
                            "Status",  TL_MSG_PARAM_INT, status & 0xFFFF );                        

		/* is it a mountape or a posittape process? */

		tunp = tptabp;
		for (i = 0; i < nbtpdrives; i++) {
			if (tunp->mntovly_pid == pid) {
				tunp->mntovly_pid = 0;
				break;
			}
			tunp++;
		}
		if (i < nbtpdrives) continue;

		/* is it a config process? */

		rqp = confqp;
		while (rqp) {
			if (rqp->ovly_pid == pid) {
				tunp = tptabp + rqp->ux;
				for (i = 0; i < nbdgp; i++) {
					if (strcmp (tunp->dgn, tpdrrt.dg[i].name) == 0)
						break;
				}
				if (rqp->status) {     /* tpconfig up */
					if (status == 0) {	/* successful config */
						tunp->up = rqp->status;
						tpdrrt.dg[i].rsvd++;
					} 
				}
				if (rqp->prev) (rqp->prev)->next = rqp->next;
				else confqp = rqp->next;
				if (rqp->next) (rqp->next)->prev = rqp->prev;
				free (rqp);
				break;
			}
			rqp = rqp->next;
		}

        /* is it a rlstape process? */

        tunp = tptabp;
		for (i = 0; i < nbtpdrives; i++) {
            unsigned int j;
            int c;
            char *p;
			if (tunp->rlsovly_pid == pid) {
                tunp->rlsovly_pid = 0;
                if (status) {
                    tplogit (func, "rlstape process %d found dead (jid %d)\n", pid, tunp->jid);
                    tl_tpdaemon.tl_log( &tl_tpdaemon, 103, 6,
                                        "func"   , TL_MSG_PARAM_STR  , func,
                                        "Message", TL_MSG_PARAM_STR  , "rlstape process found dead",
                                        "pid"    , TL_MSG_PARAM_INT  , pid,
                                        "JobID"  , TL_MSG_PARAM_INT  , tunp->jid,
                                        "vid"    , TL_MSG_PARAM_STR  , tunp->vid,
                                        "TPVID"  , TL_MSG_PARAM_TPVID, tunp->vid );
                    p = getconfent( "TAPE", "CRASHED_RLS_HANDLING", 0 );

                        if (NULL == p || 0 == strcasecmp( p, "RETRY" )) { 

                            /* 
                            **  retry the release or ...
                            */
                            
                            int maxRetries =  3;
                            if ((p = getconfent ("TAPE", "CRASHED_RLS_HANDLING_RETRIES", 0))) {
                                maxRetries = atoi(p)>0?atoi(p):3;
                            }     

                            /* pretend this is the first release attempt
                               and the drive is still assigned         */
                            tunp->asn = 1;

                            rrtp = tpdrrt.next;
                            while (rrtp) {
                                if (tunp->jid == rrtp->jid) break;
                                rrtp = rrtp->next;
                            }
                            if (tunp->rlsrtryctr < maxRetries) {

                                tplogit (func, "retry the release, attempt: %d\n", tunp->rlsrtryctr+1);
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 5,
                                                    "func"   , TL_MSG_PARAM_STR  , func,
                                                    "Message", TL_MSG_PARAM_STR  , "retry the release",
                                                    "attempt", TL_MSG_PARAM_INT  , tunp->rlsrtryctr+1,
                                                    "vid"    , TL_MSG_PARAM_STR  , tunp->vid,
                                                    "TPVID"  , TL_MSG_PARAM_TPVID, tunp->vid );
                                
                                c = reldrive (tunp, "cleanup", -1, TPRLS_UNLOAD|TPRLS_DELAY);
                                if ((c == 0) && rrtp) rrtp->unldcnt++;
                                tunp->rlsrtryctr++;
                                tplogit (func, "reldrive returned %d\n", c);
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 5,
                                                    "func"   , TL_MSG_PARAM_STR  , func,
                                                    "Message", TL_MSG_PARAM_STR  , "reldrive returned",
                                                    "rc"     , TL_MSG_PARAM_INT  , c,
                                                    "vid"    , TL_MSG_PARAM_STR  , tunp->vid,
                                                    "TPVID"  , TL_MSG_PARAM_TPVID, tunp->vid );

                            } else {

                                tplogit (func, "rlstape crashed, release failed, configure the drive down\n");
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 103, 5,
                                                    "func"   , TL_MSG_PARAM_STR  , func,
                                                    "Message", TL_MSG_PARAM_STR  , "rlstape crashed, release failed, configure the drive down",
                                                    "JobID"  , TL_MSG_PARAM_INT  , tunp->jid,
                                                    "vid"    , TL_MSG_PARAM_STR  , tunp->vid,
                                                    "TPVID"  , TL_MSG_PARAM_TPVID, tunp->vid );

                                for (j = 0; j < nbdgp; j++) {
                                    if (strcmp (tunp->dgn, tpdrrt.dg[j].name) == 0) break;
                                }                                
                                tunp->asn = 0;
                                tunp->asn_time = 0;
                                tunp->unasn_time = time(0);
                                tunp->filp = 0;
                                tunp->vid[0] = '\0';
                                tunp->vsn[0] = '\0';
                                tunp->tobemounted = 0;
                                tpdrrt.dg[j].rsvd--;
                                        
                                /* reset the rls retry counter */
                                tunp->rlsrtryctr = 0;

                                c = confdrive (tunp, -1, CONF_DOWN, -tunp->up);
                                tunp->up = 0;
                                tplogit (func, "confdrive returned: %d\n", c);
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 2,
                                                    "func"   , TL_MSG_PARAM_STR  , func,
                                                    "Message", TL_MSG_PARAM_STR  , "rlstape crashed, release failed, configure the drive down" );
                            }

                        } else if (0 == strcasecmp( p, "DOWN" )) {

                            /* 
                            **  ... put the drive down or ...
                            */

                            tplogit (func, "rlstape crashed, configure the drive down\n");
                            tl_tpdaemon.tl_log( &tl_tpdaemon, 103, 5,
                                                "func"   , TL_MSG_PARAM_STR  , func,
                                                "Message", TL_MSG_PARAM_STR  , "rlstape crashed, configure the drive down",
                                                "JobID"  , TL_MSG_PARAM_INT  , tunp->jid,
                                                "vid"    , TL_MSG_PARAM_STR  , tunp->vid,
                                                "TPVID"  , TL_MSG_PARAM_TPVID, tunp->vid );

                            for (j = 0; j < nbdgp; j++) {
                                if (strcmp (tunp->dgn, tpdrrt.dg[j].name) == 0) break;
                            }                                
                            tunp->asn = 0;
                            tunp->asn_time = 0;
                            tunp->unasn_time = time(0);
                            tunp->filp = 0;
                            tunp->vid[0] = '\0';
                            tunp->vsn[0] = '\0';
                            tunp->tobemounted = 0;
                            tpdrrt.dg[j].rsvd--;
                                                        
                            /* reset the rls retry counter */
                            tunp->rlsrtryctr = 0;
                                                        
                            c = confdrive (tunp, -1, CONF_DOWN, -tunp->up);
                            tunp->up = 0;
                            tplogit (func, "confdrive returned: %d\n", c);
                            tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 2,
                                                "func"   , TL_MSG_PARAM_STR  , func,
                                                "Message", TL_MSG_PARAM_STR  , "rlstape crashed, release failed, configure the drive down" );

                        } else {

                            /* 
                            **  ... do nothing at all.
                            */

                            tplogit (func, "rlstape crashed, no action defined\n"); 
                            tl_tpdaemon.tl_log( &tl_tpdaemon, 103, 5,
                                                "func"   , TL_MSG_PARAM_STR  , func,
                                                "Message", TL_MSG_PARAM_STR  , "rlstape crashed, no action defined",
                                                "JobID"  , TL_MSG_PARAM_INT  , tunp->jid,
                                                "vid"    , TL_MSG_PARAM_STR  , tunp->vid,
                                                "TPVID"  , TL_MSG_PARAM_TPVID, tunp->vid );
                        }

                } else {
                    
                    tplogit (func, "rlstape process %d exited normally (jid %d)\n", pid, tunp->jid); 
                    tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 4,
                                        "func"   , TL_MSG_PARAM_STR  , func,
                                        "Message", TL_MSG_PARAM_STR  , "rlstape exited normally",
                                        "pid"    , TL_MSG_PARAM_INT  , pid,
                                        "JobID"  , TL_MSG_PARAM_INT  , tunp->jid );
                }
			}
			tunp++;
		}
		if (i < nbtpdrives) continue;                
	}
}

/* 
** Check if the drive is available for use. Since the tape daemon
** is the only one changing the assignment status of drives,
** there should be no race with the child changing the drive's
** state in VDQM. 
*/
static int tpdrvidle( struct tptab *tunp ) {

    int free = 0;

    /* drive is not assigned, but up */
    if ((0 == tunp->asn) && (1 == tunp->up)) {
                
        free = 1;
    }

    return free;
}

/*
** Get a drive's state in VDQM 
*/
static int vdqmdrvstate(struct tptab *tunp) {
        
        int status;
        int rc = -1, value;
        int ctr = 0, maxCtr = 3, sleepIntvl = 300;
        
        while (ctr < maxCtr) {

                status = VDQM_UNIT_QUERY;
                rc = vdqm_UnitStatus(NULL, NULL, tunp->dgn, NULL, tunp->drive,
                                     &status, &value, 0);

                /* all went fine, report the status */
                if (0 == rc) {
                        tplogit( func, "vdqm_UnitStatus reported status: 0x%x\n", status);
                        return status;
                }

                /* sleep and retry */
                tplogit( func, "vdqm_UnitStatus failed to query status: %d, attempt %d, retry in %d seconds\n", 
                         rc, ctr, sleepIntvl);
                sleep(sleepIntvl);

                ctr++;
        }
        
        tplogit( func, "vdqm_UnitStatus failed to query status: %d, giving up, config drive down\n", rc);
        (void) confdrive (tunp, -1, CONF_DOWN, -tunp->up);
        tunp->up = 0;
        return rc;
}


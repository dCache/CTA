/*
 * Copyright (C) 2001-2002 by CERN/IT/PDP/DM
 * All rights reserved
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "h/Cinit.h"
#include "h/marshall.h"
#include "h/net.h"
#include "h/rmc_constants.h"
#include "h/rmc_logit.h"
#include "h/rmc_sendrep.h"
#include "h/rmc_server_api.h"
#include "h/rmc_smcsubr.h"
#include "h/scsictl.h"
#include "h/serrno.h"
#include "h/Cdomainname.h"
#include "h/tplogger_api.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <Ctape_api.h>
#include "h/sendscsicmd.h"

/* Forward declaration */
static int getreq(const int s, int *const req_type, char *const req_data,
  char **const clienthost);
static void procreq(const int rpfd, const int req_type, char *const req_data,
  char *const clienthost);
static void rmc_doit(const int rpfd);

int jid;
char localhost[CA_MAXHOSTNAMELEN+1];
int maxfds;
struct extended_robot_info extended_robot_info;

int rmc_main(struct main_args *main_args)
{
	int c;
	unsigned char cdb[12];
	char domainname[CA_MAXHOSTNAMELEN+1];
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);
	char *getconfent();
	char *msgaddr;
	int nb_sense_ret;
	int on = 1;	/* for REUSEADDR */
	char plist[40];
	fd_set readfd, readmask;
	char *robot;
	int s;
    int n=0;
	char sense[MAXSENSE];
	struct sockaddr_in sin;
	struct smc_status smc_status;
	struct servent *sp;
	struct timeval timeval;
	char func[16];

	strncpy (func, "rmc_serv", sizeof(func));
	func[sizeof(func) - 1] = '\0';

        /* init the tplogger interface */
        {
                const mode_t save_mask = umask(0);

                /* only syslog support */
                tl_init_handle( &tl_rmcdaemon, "syslog" );
                tl_rmcdaemon.tl_init( &tl_rmcdaemon, 2 );                
                tl_rmcdaemon.tl_log( &tl_rmcdaemon, 109, 2,
                                     "func"   , TL_MSG_PARAM_STR, "rmc_main",
                                     "Message", TL_MSG_PARAM_STR, "Logging initialized" );
                
                umask(save_mask);
        }

	jid = getpid();
	rmc_logit (func, "started\n");
        tl_rmcdaemon.tl_log( &tl_rmcdaemon, 109, 2,
                             "func"   , TL_MSG_PARAM_STR, "rmc_main",
                             "Message", TL_MSG_PARAM_STR, "Daemon started" );        
        
	gethostname (localhost, CA_MAXHOSTNAMELEN+1);
	if (strchr (localhost, '.') == NULL) {
		if (Cdomainname (domainname, sizeof(domainname)) < 0) {
			rmc_logit (func, "Unable to get domainname\n");
                        tl_rmcdaemon.tl_log( &tl_rmcdaemon, 103, 2,
                                             "func"   , TL_MSG_PARAM_STR, "rmc_main",
                                             "Message", TL_MSG_PARAM_STR, "Unable to get domainname" );
		}
		strcat (localhost, ".");
		strcat (localhost, domainname);
	}

	if (main_args->argc != 2) {
		rmc_logit (func, RMC01);
                tl_rmcdaemon.tl_log( &tl_rmcdaemon, 1, 1,
                                     "func", TL_MSG_PARAM_STR, "rmc_main" );
		exit (USERR);
	}
	robot = main_args->argv[1];
	if (*robot == '\0' ||
	    (strlen (robot) + (*robot == '/') ? 0 : 5) > CA_MAXRBTNAMELEN) {
		rmc_logit (func, RMC06, "robot");
                tl_rmcdaemon.tl_log( &tl_rmcdaemon, 6, 2,
                                     "func", TL_MSG_PARAM_STR, "rmc_main",
                                     "For" , TL_MSG_PARAM_STR, "robot" );
		exit (USERR);
	}
	if (*robot == '/')
		strcpy (extended_robot_info.smc_ldr, robot);
	else
		sprintf (extended_robot_info.smc_ldr, "/dev/%s", robot);

	extended_robot_info.smc_fd = -1;

	/* get robot geometry, try 2 times */
   
    while (n < 2) {
      if ((c = smc_get_geometry (extended_robot_info.smc_fd,
                                 extended_robot_info.smc_ldr,
                                 &extended_robot_info.robot_info))) {
            c = smc_lasterror (&smc_status, &msgaddr);
            rmc_logit (func, RMC02, "get_geometry", msgaddr);
            tl_rmcdaemon.tl_log( &tl_rmcdaemon, 2, 4,
                                 "func"    , TL_MSG_PARAM_STR, "rmc_main",
                                 "On"      , TL_MSG_PARAM_STR, "get_geometry",
                                 "Message" , TL_MSG_PARAM_STR, msgaddr,
                                 "NextStep", TL_MSG_PARAM_STR, "Retry" );

            rmc_logit (func,"trying again get_geometry\n");
            tl_rmcdaemon.tl_log( &tl_rmcdaemon, 110, 2,
                                 "func"   , TL_MSG_PARAM_STR, "rmc_main",
                                 "Message", TL_MSG_PARAM_STR, "trying again get_geometry" );
            n++;
            if (n==2) {
              rmc_logit (func, RMC02, "get_geometry", msgaddr);
              tl_rmcdaemon.tl_log( &tl_rmcdaemon, 2, 4,
                                   "func"    , TL_MSG_PARAM_STR, "rmc_main",
                                   "On"      , TL_MSG_PARAM_STR, "get_geometry",
                                   "Message" , TL_MSG_PARAM_STR, msgaddr,
                                   "NextStep", TL_MSG_PARAM_STR, "Exiting" );
              exit(c);
            }
	     } else {
             n = 0;
             break;
         }
    }
	/* check if robot support Volume Tag */

	extended_robot_info.smc_support_voltag = 1;
	memset (cdb, 0, sizeof(cdb));
	cdb[0] = 0xB6;		/* send volume tag */
	cdb[5] = 5;
	cdb[9] = 40;
	memset (plist, 0, sizeof(plist));
	strcpy (plist, "DUMMY0");
	c = send_scsi_cmd (extended_robot_info.smc_fd,
                     extended_robot_info.smc_ldr, 0, cdb, 12, (unsigned char*)plist, 40,
	    sense, 38, 900000, SCSI_OUT, &nb_sense_ret, &msgaddr);
	if (c < 0) {
		if (c == -4 && nb_sense_ret >= 14 && (sense[2] & 0xF) == 5 &&
		    sense[12] == 0x20) {
			extended_robot_info.smc_support_voltag = 0;
		} else {
			rmc_logit (func, RMC02, "find_cartridge", msgaddr);
                        tl_rmcdaemon.tl_log( &tl_rmcdaemon, 2, 4,
                                             "func"    , TL_MSG_PARAM_STR, "rmc_main",
                                             "On"      , TL_MSG_PARAM_STR, "find_cartridge",
                                             "Message" , TL_MSG_PARAM_STR, msgaddr,
                                             "NextStep", TL_MSG_PARAM_STR, "Exiting/SYERR" );
			exit (SYERR);
		}
	}
	FD_ZERO (&readmask);
	FD_ZERO (&readfd);
	signal (SIGPIPE, SIG_IGN);
	signal (SIGXFSZ, SIG_IGN);

	/* open request socket */

	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		rmc_logit (func, RMC02, "socket", neterror());
                tl_rmcdaemon.tl_log( &tl_rmcdaemon, 2, 4,
                                     "func"    , TL_MSG_PARAM_STR, "rmc_main",
                                     "On"      , TL_MSG_PARAM_STR, "socket",
                                     "Message" , TL_MSG_PARAM_STR, msgaddr,
                                     "NextStep", TL_MSG_PARAM_STR, "Exiting/CONFERR" );
		exit (CONFERR);
	}
	memset ((char *)&sin, 0, sizeof(struct sockaddr_in)) ;
	sin.sin_family = AF_INET ;
	{
		const char *p;
		if ((p = getenv ("RMC_PORT")) || (p = getconfent ("RMC", "PORT", 0))) {
			sin.sin_port = htons ((unsigned short)atoi (p));
		} else if ((sp = getservbyname ("rmc", "tcp"))) {
			sin.sin_port = sp->s_port;
		} else {
			sin.sin_port = htons ((unsigned short)RMC_PORT);
		}
	}
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0) {
		rmc_logit (func, RMC02, "setsockopt", neterror());
                tl_rmcdaemon.tl_log( &tl_rmcdaemon, 2, 3,
                                     "func"    , TL_MSG_PARAM_STR, "rmc_main",
                                     "On"      , TL_MSG_PARAM_STR, "setsockopt",
                                     "Message" , TL_MSG_PARAM_STR, msgaddr );
        }
	if (bind (s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		rmc_logit (func, RMC02, "bind", neterror());
                tl_rmcdaemon.tl_log( &tl_rmcdaemon, 2, 4,
                                     "func"    , TL_MSG_PARAM_STR, "rmc_main",
                                     "On"      , TL_MSG_PARAM_STR, "bind",
                                     "Message" , TL_MSG_PARAM_STR, msgaddr,
                                     "NextStep", TL_MSG_PARAM_STR, "Exiting/CONFERR" );
		exit (CONFERR);
	}
	listen (s, 5) ;

	FD_SET (s, &readmask);

		/* main loop */

	while (1) {
		if (FD_ISSET (s, &readfd)) {
			FD_CLR (s, &readfd);
			const int rpfd =
				accept (s, (struct sockaddr *) &from, &fromlen);
			(void) rmc_doit (rpfd);
		}
		memcpy (&readfd, &readmask, sizeof(readmask));
		timeval.tv_sec = RMC_CHECKI;
		timeval.tv_usec = 0;
		if (select (maxfds, &readfd, (fd_set *)0, (fd_set *)0, &timeval) < 0) {
			FD_ZERO (&readfd);
		}
	}
        /* never reached */
        tl_rtcpd.tl_exit( &tl_rmcdaemon, 0 );
}

int main(int argc,
         char **argv)
{
	struct main_args main_args;

	main_args.argc = argc;
	main_args.argv = argv;
	if ((maxfds = Cinitdaemon ("rmcd", NULL)) < 0)
		exit (SYERR);
	exit (rmc_main (&main_args));
}

static void rmc_doit(const int rpfd)
{
	int c;
	char *clienthost;
	char req_data[RMC_REQBUFSZ-3*LONGSIZE];
	int req_type = 0;

	if ((c = getreq (rpfd, &req_type, req_data, &clienthost)) == 0)
		procreq (rpfd, req_type, req_data, clienthost);
	else if (c > 0)
		rmc_sendrep (rpfd, RMC_RC, c);
	else
		close (rpfd);
}

static int getreq(
  const int s,
  int *const req_type,
  char *const req_data,
  char **const clienthost)
{
	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);
	struct hostent *hp;
	int l;
	int magic;
	int msglen;
	int n;
	char *rbp;
	char req_hdr[3*LONGSIZE];
	char func[16];

	strncpy (func, "rmc_serv", sizeof(func));
	func[sizeof(func) - 1] = '\0';

	l = netread_timeout (s, req_hdr, sizeof(req_hdr), RMC_TIMEOUT);
	if (l == sizeof(req_hdr)) {
		rbp = req_hdr;
		unmarshall_LONG (rbp, magic);
		unmarshall_LONG (rbp, n);
		*req_type = n;
		unmarshall_LONG (rbp, msglen);
		if (msglen > RMC_REQBUFSZ) {
			rmc_logit (func, RMC46, RMC_REQBUFSZ);
                        tl_rmcdaemon.tl_log( &tl_rmcdaemon, 46, 2,
                                             "func"   , TL_MSG_PARAM_STR, "getreq",
                                             "MaxSize", TL_MSG_PARAM_INT,  RMC_REQBUFSZ);                        
			return (-1);
		}
		l = msglen - sizeof(req_hdr);
		n = netread_timeout (s, req_data, l, RMC_TIMEOUT);
		if (getpeername (s, (struct sockaddr *) &from, &fromlen) < 0) {
			rmc_logit (func, RMC02, "getpeername", neterror());
                        tl_rmcdaemon.tl_log( &tl_rmcdaemon, 2, 4,
                                             "func"   , TL_MSG_PARAM_STR, "getreq",
                                             "On"     , TL_MSG_PARAM_STR, "getpeername",
                                             "Message", TL_MSG_PARAM_STR,  neterror(),
                                             "Return" , TL_MSG_PARAM_STR, "ERMCUNREC" );
			return (ERMCUNREC);
		}
		hp = gethostbyaddr ((char *)(&from.sin_addr),
			sizeof(struct in_addr), from.sin_family);
		if (hp == NULL)
			*clienthost = inet_ntoa (from.sin_addr);
		else
			*clienthost = hp->h_name ;
		return (0);
	} else {
		if (l > 0) {
			rmc_logit (func, RMC04, l);
                        tl_rmcdaemon.tl_log( &tl_rmcdaemon, 4, 3,
                                             "func"   , TL_MSG_PARAM_STR, "getreq",
                                             "netread", TL_MSG_PARAM_INT, l,
                                             "Return" , TL_MSG_PARAM_STR, "ERMCUNREC" );
		} else if (l < 0) {
			rmc_logit (func, RMC02, "netread", strerror(errno));
                        tl_rmcdaemon.tl_log( &tl_rmcdaemon, 2, 4,
                                             "func"   , TL_MSG_PARAM_STR, "getreq",
                                             "On"     , TL_MSG_PARAM_STR, "netread",
                                             "Message", TL_MSG_PARAM_STR, strerror(errno) ,
                                             "Return" , TL_MSG_PARAM_STR, "ERMCUNREC" );
                }
		return (ERMCUNREC);
	}
}

static void procreq(
  const int rpfd,
  const int req_type,
  char *const req_data,
  char *const clienthost)
{
	int c;

	switch (req_type) {
	case RMC_MOUNT:
		c = rmc_srv_mount (req_data, clienthost);
		break;
	case RMC_UNMOUNT:
		c = rmc_srv_unmount (req_data, clienthost);
		break;
	case RMC_EXPORT:
		c = rmc_srv_export (req_data, clienthost);
		break;
	case RMC_IMPORT:
		c = rmc_srv_import (req_data, clienthost);
		break;
	case RMC_GETGEOM:
		c = rmc_srv_getgeom (req_data, clienthost);
		break;
	case RMC_READELEM:
		c = rmc_srv_readelem (req_data, clienthost);
		break;
	case RMC_FINDCART:
		c = rmc_srv_findcart (req_data, clienthost);
		break;
	default:
		rmc_sendrep (rpfd, MSG_ERR, RMC03, req_type);
		c = ERMCUNREC;
	}
	rmc_sendrep (rpfd, RMC_RC, c);
}

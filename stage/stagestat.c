/*
 * $Id: stagestat.c,v 1.29 2002/06/16 04:58:12 jdurand Exp $
 */

/*
 * Copyright (C) 1996-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: stagestat.c,v $ $Revision: 1.29 $ $Date: 2002/06/16 04:58:12 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#ifndef _WIN32
#include <unistd.h>
#else
#include <winsock2.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>                     /* For INT_MIN and INT_MAX */
#include "stage_api.h"
#include "sacct.h"
#include <time.h>
#include <osdep.h>
#include "Cpwd.h"
#include "Cgrp.h"
#include "Cgetopt.h"
#include "serrno.h"
#include "Castor_limits.h"
#include "rfio_api.h"
#include "u64subr.h"

/* Macro to swap byte order */

#define swap_it(a) swab((char *)&a,(char *)&a,sizeof(a));\
									 a=((unsigned int)a<<16)|((unsigned int)a>>16);
int getacctrec _PROTO((int, struct accthdr *, char *, int *));
int match_2_stgin _PROTO((struct acctstage2 *));
time_t cvt_datime _PROTO((char *));
void print_acct _PROTO((struct accthdr *, struct acctstage2 *));

EXTERN_C int DLL_DECL rfio_parseln _PROTO((char *, char **, char **, int));

struct file_inf{
	int uid;				/* user id number */	
	int gid;				/* group id number */
	char poolname[CA_MAXPOOLNAMELEN + 1];	/* pool name */
	time_t last_stgin;			/* time file was last staged in */
	time_t total_stgin_gc;	/* total stagein time if clrd by garbage collector using last access */
	time_t total_stgin_uc;	/* total stagein time if clrd by user using last access */	
	time_t total_stgin_gc2;	/* total stagein time if clrd by garbage collector using creation time */
	time_t total_stgin_uc2;	/* total stagein time if clrd by user using creation time */	
	union{
		struct{				/* tape files */
			char vid[7];		/* volume id */
			char fseq[CA_MAXFSEQLEN + 1];	/* file sequence number */
		}t;
		struct{				/* disk and allocated files (t_or_d == 'd' or t_or_d == 'a') */
			char xfile[CA_MAXHOSTNAMELEN + 1 + MAXPATH];	/* filename and path */
		}d;
		struct{				/* non-CASTOR HSM files */
			char xfile[STAGE_MAX_HSMLENGTH+1];
		}m;
		struct{				/* CASTOR HSM files */
			char xfile[STAGE_MAX_HSMLENGTH+1];
			u_signed64 fileid;
		}h;
	}u1;
	int stage_status;			/* current stage status */
	int num_periods_gc;		/* number periods terminated by garbage collector */
	int num_periods_uc;		/* number periods terminated by user */
	int num_periods_gc2;		/* number periods terminated by garbage collector using creation time */
	int num_periods_uc2;		/* number periods terminated by user using creation time */
	int num_accesses;		/* number times file accessed in current time period */
	char t_or_d;				/* identifies if file is from tape or allocation or disk or non-CASTOR-HSM or CASTOR-HSM */
	struct file_inf *next;
};
struct file_inf *file_in_last, *file_in_list = NULL;
struct file_inf *file_wrt_last, *file_wrt_list = NULL;

struct stg_inf{
	int reqid;				/* request id of stagein command */
	int num_files;			/* total number of files requested */
	struct stg_inf *next;
};
struct stg_inf *stg_last, *srec_match, *stage_list = NULL;

struct frec_nbaccs{
	struct file_inf *rec;		/* pointer to a file_inf record */
	int nbaccesses;			/* number of accesses */
	char poolname[CA_MAXPOOLNAMELEN + 1];	/* pool name */
};

struct frec_avg{
	struct file_inf *rec;		/* pointer to a file_inf record */
	float avg_life;			/* total average life for the file using last access */
	float avg_life2;		/* total average life for the file using creation time */
	char poolname[CA_MAXPOOLNAMELEN + 1];	/* pool name */
};

struct pool_inf{
	char poolname[CA_MAXPOOLNAMELEN + 1];	/* pool name */
	float num_files;			/* total number of files requested by pool */
	int num_frecs;			/* total num tape files requested */
	int num_drecs;			/* total num disk files requested */
	int num_mrecs;			/* total num non-CASTOR-HSM requested */
	int num_hrecs;			/* total num CASTOR-HSM requested */
	int acc;		/* num files accessed, not stgd in or clrd during time_period */
	int acc_clrd;	/* num files accesses and clrd, not staged in */
	int stg_acc;		/* num files staged in and accessed, not clrd */
	int stg_acc_clrd;	/* num files staged in, accessed and clrd */
	int total_stged_files;  /* num files staged in, accd, clrd by garbage collector using last access time */ 
	int total_stged_files2;  /* num files staged in, accd, clrd by garbage collector using creation time */ 
	float total_accesses;   /* total num of file accesses for pool */
	float total_avg_life;   /* total avg life of files clrd by garbage collector using last access time */
	float total_avg_life2;  /* total avg life of files clrd by garbage collector using creation time */
	struct pool_inf *next;
};
struct pool_inf *pool_last, *pool_list = NULL;

 
int nb_req[17];			/* array to store START + num requests for each command - see stage_constants.h (STAGEIN,STAGEOUT,etc...) */	
int nb_apireq[17];		/* array to store START + num requests for each API function - see stage_constants.h (STAGE_IN,STAGE_OUT,etc...) */	
int nb_restart = 0;		/* number of times stager restarted */
int rc[17][12];		/* array to store num of errors and warnings for each command - see stage_constants.h (see USERR,SYERR,etc...) */
int num_in_remount_requests = 0;		/* number of tape files remounted during time period in stagein mode */
int num_wrt_remount_requests = 0;		/* number of tape files remounted during time period in stagewrt/stageput mode */
unsigned int num_frecs = 0;		/* total number of tape files */
unsigned int num_drecs = 0;		/* total number of disk files */
unsigned int num_mrecs = 0;		/* total number of non-CASTOR-HSM files */
unsigned int num_hrecs = 0;		/* total number of CASTOR-HSM files */
char hostname[CA_MAXHOSTNAMELEN + 1];
int num_fork_exec_stager = 0;			/* number of fork of a stager */
int verbose = 0;
int debug = 0;
u_signed64 size_total, size_read;

void create_filelist _PROTO((struct file_inf **, struct file_inf**, struct acctstage2 *));
void create_poollist _PROTO((struct acctstage2 *));
void create_stglist _PROTO((struct acctstage2 *));
void enter_filc_details _PROTO((struct acctstage2 *, struct accthdr));
void enter_fils_details _PROTO((struct acctstage2 *, struct accthdr));
void enter_filw_details _PROTO((struct acctstage2 *, struct accthdr));
void enter_pool_details _PROTO((struct acctstage2 *));
void sort_poolinf _PROTO(());
void print_poolstat _PROTO((int, int, int, char *));
void swap_fields _PROTO((struct acctstage2 *));
void usage _PROTO((char *));
void print_fdetails _PROTO((struct file_inf *));
void print_globstat _PROTO((time_t, time_t, int, int));
void print_time_interval _PROTO((time_t, time_t));
void print_ddetails _PROTO((struct file_inf *));
void print_mdetails _PROTO((struct file_inf *));
void print_hdetails _PROTO((struct file_inf *));
void show_progress _PROTO(());
char *req_type_2_char _PROTO((int));
char *subtype_2_char _PROTO((int));

char *Tflag = NULL;
char *Dflag = NULL;
char *Mflag = NULL;
char *Hflag = NULL;
int use_c_time = 0;
int all_stageclrs = 0;

int main(argc, argv)
	int argc;
	char **argv;
{
	char acctfile[CA_MAXPATHLEN+1];			/* accounting file name */
	char poolname[CA_MAXPOOLNAMELEN + 1];	/* pool name */
 
	struct accthdr accthdr;		/* accthdr record */
	struct acctstage2 rp;		/* pointer to accstage2 record */
	struct stg_inf *srecord;		/* pointer to stg_inf record */

	time_t endtime = 0;			/* time up to which to obtain information */
	time_t starttime = 0;		/* time from which to obtain information */
	
	int c;				
	int errflg = 0;			/* error flag */
	int fd_acct;				/* file identifier */
	int nrec = 0;			/* number of records read */
	int nrecnotst = 0;			/* number of records refused because not STAGE ones */
	int nrecerror = 0;			/* number of records refused */
	int nrecfiltered = 0;			/* number of records accepted but filtered */
	int nrecok = 0;			/* number of records accept and processed */
	int num_multireqs = 0;		/* number of multifile requests */
	int swapped = 0;			/* flag set if byte order swapped */
	int pflag = 0;			/* pool group specified */
	int tflag = 0;			/* sort on lifetime */
	int aflag = 0;			/* sort on num accesses */	
	int rflag = 0;            /* Select a reqid */
	struct stat mystat;
#if defined(_WIN32)
	WSADATA 	wsadata;
	int 	rcode;
#endif
  
	acctfile[0] = '\0';
	poolname[0] = '\0';

	Coptind = 1;
	Copterr = 1;
	while ((c = Cgetopt (argc, argv, "acdD:e:f:hH:M:p:r:S:s:T:v")) != -1) {
		switch (c) {
		case 'a':
			all_stageclrs++;
			break;
		case 'c':
			use_c_time++;
			break;
		case 'd':
			debug++;
			break;
		case 'D':
 			Dflag = Coptarg;
			break;
		case 'e':
			if ((endtime = cvt_datime (Coptarg)) < 0) {
				fprintf (stderr, "incorrect time value %s\n", Coptarg);
				errflg++;
			}
			break;
		case 'f':
			if (strlen(Coptarg) > CA_MAXPATHLEN) {
				fprintf(stderr,"%s : too long (max %d characters)\n", Coptarg, CA_MAXPATHLEN);
			}
			strcpy (acctfile, Coptarg);
			break;
		case 'h':
			usage (argv[0]);
			exit (0);
			break;
		case 'H':
 			Hflag = Coptarg;
			break;
		case 'M':
 			Mflag = Coptarg;
			break;
		case 'p':
			pflag++;
			strcpy (poolname, Coptarg);
			break;
		case 'r':
			if ((rflag = atoi(Coptarg)) <= 0) {
				fprintf (stderr, "incorrect request id value %s\n", Coptarg);
				errflg++;
			}
			break;
		case 'S':
			if (strcmp (Coptarg, "t") == 0) tflag++;
			else if (strcmp (Coptarg, "a") == 0) aflag++;
			else {
				fprintf (stderr,"Incorrect or no sort criteria given %s\n",
						 Coptarg);
				errflg++;
			}
			break;
		case 's':
			if ((starttime = cvt_datime (Coptarg)) < 0) {
				fprintf (stderr, "incorrect time value %s\n", Coptarg);
				errflg++;
			}
			break;
		case 'T':
 			Tflag = Coptarg;
			break;
		case 'v':
			verbose++;
			break;
		case '?':
			errflg++;
			break;
		default:
			errflg++;
			break;
		}
	}
	if (errflg) {
		usage (argv[0]);
		exit (USERR);
	}

	if (acctfile[0] == '\0') {
		fprintf(stderr,"[INFO] Using default account file \"%s\"\n", ACCTFILE);
		if (strlen(ACCTFILE) > CA_MAXPATHLEN) {
			fprintf(stderr,"%s : too long (max %d characters)\n", ACCTFILE, CA_MAXPATHLEN);
			exit(USERR);
		}
		strcpy (acctfile, ACCTFILE);
	}

#if defined(_WIN32)
	rcode = WSAStartup(MAKEWORD(2, 0), &wsadata);	/* initialization of WinSock DLL */
	if( rcode )  {	
		fprintf(stderr,"WSAStartup: %s\n", ws_strerr(rcode) );
		exit(SYERR);
	}
#endif /* if WIN32 */

	{
		char *host = NULL;
		char *filename = NULL;
		char save_acctfile[CA_MAXPATHLEN+1];

		strcpy(save_acctfile,acctfile);
		(void) rfio_parseln (save_acctfile, &host, &filename, NORDLINKS);
		if (host != NULL) {
			strncpy(hostname,host,CA_MAXHOSTNAMELEN);
			hostname[CA_MAXHOSTNAMELEN] = '\0';
		} else {
#ifndef _WIN32
			gethostname (hostname, CA_MAXHOSTNAMELEN + 1);
#else
			strcpy(hostname,"localhost");
#endif
		}
	}

	rfio_errno = serrno = 0;
	if ((fd_acct = rfio_open (acctfile, O_RDONLY)) < 0) {
		fprintf (stderr, "%s : rfio_open error : %s\n", acctfile, rfio_serror());
#if defined(_WIN32)
		WSACleanup();
#endif     
		exit (USERR);
	}

	if (rfio_stat(acctfile, &mystat) < 0) {
		fprintf (stderr, "%s : rfio_stat error : %s\n", acctfile, rfio_serror());
#if defined(_WIN32)
		WSACleanup();
#endif     
		exit (USERR);
	}
	size_total = mystat.st_size;
	size_read = 0;

	memset(&rp, 0, sizeof(struct acctstage));
	while (getacctrec (fd_acct, &accthdr, (char *) &rp, &swapped) > 0) {
		if (verbose || debug) {
			show_progress();
		}
		nrec++;
		if ((accthdr.package != ACCTSTAGE) && (accthdr.package != ACCTSTAGE2)) {
			nrecnotst++;
			continue;
		}
		if (swapped) {
			swap_fields (&rp);
			swapped = 0;
		}
		if (debug) {
			print_acct(&accthdr, &rp);
		} else {
			if (rflag && rp.reqid == rflag) {
				print_acct(&accthdr, &rp);
			} else {
				switch (rp.u2.s.t_or_d) {
				case 't':
					if (Tflag && strstr(rp.u2.s.u1.t.vid,Tflag)) print_acct(&accthdr, &rp);
					break;
				case 'd':
					if (Dflag && strstr(rp.u2.s.u1.d.xfile,Dflag)) print_acct(&accthdr, &rp);
				break;
				case 'm':
					if (Mflag && strstr(rp.u2.s.u1.m.xfile,Mflag)) print_acct(&accthdr, &rp);
					break;
				case 'h':
					if (Hflag && strstr(rp.u2.s.u1.h.xfile,Hflag)) print_acct(&accthdr, &rp);
					break;
				default:
					break;
				}
			}
		}
		if (!starttime && nrec == 1) starttime = accthdr.timestamp;
		if (accthdr.timestamp < starttime) {
			nrecfiltered++;
			continue;
		}
		if (endtime && accthdr.timestamp > endtime) {
			break;
		}
		if (rflag && rp.reqid != rflag) {
			nrecfiltered++;
			continue;
		}
		if (rp.subtype == STGSTART) {		/* stgdaemon started */
			nb_restart++;
			nrecok++;
		} else if (rp.subtype == STGCMDR) {	/* command received */
			if (rp.req_type > STAGE_00) {
				/* This is an API function call */
				if ((rp.req_type - STAGE_00) > 16) {
					fprintf(stderr,"[ERROR] API's req_type %d !?\n", rp.req_type);
					nrecerror++;
					if (verbose && ! debug) print_acct(&accthdr, &rp);
					continue;
				}
				nb_apireq[rp.req_type - STAGE_00]++;
				/* We make rest believe it is a command-line request */
				rp.req_type -= STAGE_00;
			}
			if (rp.req_type > 16) {
				fprintf(stderr,"[ERROR] Command-line's req_type %d !?\n", rp.req_type);
				nrecerror++;
				if (verbose && ! debug) print_acct(&accthdr, &rp);
				continue;
			}
			/* Every API call has a command-line equivalent - we will use the nb_apireq to know the proportion between API and command-line */
			nb_req[rp.req_type]++;
			if (rp.req_type == 1) create_stglist (&rp);
			nrecok++;
	    } else if (rp.subtype == STGCMDS) {
			num_fork_exec_stager++;
		} else if (rp.subtype == STGCMDS) { /* stager started */
			nrecok++;
		} else if (rp.subtype == STGFILS && (rp.req_type == STAGEIN)) {
			if (pflag && strcmp (poolname, rp.u2.s.poolname) != 0) {
				nrecfiltered++;
				continue;
			}
			enter_fils_details (&rp, accthdr);
			enter_pool_details (&rp);
			nrecok++;
		} else if (rp.subtype == STGFILS && ((rp.req_type == STAGEWRT) || (rp.req_type == STAGEPUT))) {
			if (pflag && strcmp (poolname, rp.u2.s.poolname) != 0) {
				nrecfiltered++;
				continue;
			}
			enter_filw_details (&rp, accthdr);
			enter_pool_details (&rp);
			nrecok++;
		} else if (rp.subtype == STGFILC) {	/* file cleared */
			if (pflag && strcmp (poolname, rp.u2.s.poolname) != 0) {
				nrecfiltered++;
				continue;
			}
			enter_filc_details (&rp, accthdr);	
			nrecok++;
		} else if (rp.subtype == STGCMDC) {  /* command completed */
			if (rp.exitcode < 5) {
				rc[rp.req_type][rp.exitcode]++; /* 1==USERR, 2==SYERR, 3==(unknown), 4==CONFERR */
				nrecok++;
			} else if (rp.exitcode == SEUSERUNKN || /* User unknown */
					   rp.exitcode == ESTGROUP   || /* Group unknown */
					   rp.exitcode == SECOMERR   || /* Communication error */
					   rp.exitcode == SECONNDROP    /* Connection closed by remote end */
				) { 
				rc[rp.req_type][2]++;
				nrecok++;
			} else if (rp.exitcode == EINVAL     ||
					   rp.exitcode == SEOPNOTSUP    /* Operation not supported */
				) {
				rc[rp.req_type][1]++;
				nrecok++;
			} else if (rp.exitcode == EISDIR) {
				rc[rp.req_type][1]++;
				nrecok++;
			} else if (rp.exitcode == ENOSPC) {
				rc[rp.req_type][5]++;
				nrecok++;
			} else if ((rp.exitcode == CLEARED) || (rp.exitcode == ESTCLEARED)) {
				rc[rp.req_type][6]++;
				nrecok++;
			} else if ((rp.exitcode == REQKILD) || (rp.exitcode == ESTKILLED)) {
				rc[rp.req_type][8]++;
				nrecok++;
			} else if ((rp.exitcode == BLKSKPD) || (rp.exitcode == ERTBLKSKPD) ||
					   (rp.exitcode == TPE_LSZ) || (rp.exitcode == ERTTPE_LSZ) ||
					   (rp.exitcode == MNYPARI) || (rp.exitcode == ETPARIT) || (rp.exitcode == ERTMNYPARY) ||
					   (rp.exitcode == LIMBYSZ) || (rp.exitcode == ERTLIMBYSZ)) {
				rc[rp.req_type][7]++;
				nrecok++;
			} else if (rp.exitcode == ESTNACT) {
				rc[rp.req_type][9]++; /* Should never be accounted */
				nrecok++;
			} else if ((rp.exitcode == ETHELD) || (rp.exitcode == ETHELDERR)) {
				rc[rp.req_type][10]++;
				nrecok++;
			} else if (rp.exitcode == EBUSY) {
				rc[rp.req_type][11]++;
				nrecok++;
			} else {
				rc[rp.req_type][3]++; /* 1==USERR, 2==SYERR, 3==(unknown), 4==CONFERR */
				nrecok++;
			}
		} else {
			fprintf(stderr,"[WARNING] rp.subtype=%d (rp.req_type=%d, rp.u2.s.t_or_d=%c)!?\n", rp.subtype, rp.req_type, rp.u2.s.t_or_d != 0 ? rp.u2.s.t_or_d : ' ');
			nrecerror++;
			if (verbose) print_acct(&accthdr, &rp);
		}
		memset(&rp, 0, sizeof(struct acctstage));
	}
	if (!endtime && nrec) endtime = accthdr.timestamp;
	close (fd_acct);
	
	if (pflag && pool_list == NULL){
		printf ("\nNo records found for Pool : %s\n", poolname);
		pflag = 0;
	}

	/* read through stg_inf list and determine number of multifile requests */

	srecord = stage_list;
	while (srecord != NULL) {
		if (srecord->num_files > 1) num_multireqs++;
		srecord = srecord->next;
	}

	/* print out statistics */

	printf("\n");
	printf("Number of read account records: %10d\n", nrec);
	printf("Number of     accepted records: %10d\n", nrecok);
	printf("Number of     rejected records: %10d\n", (nrecnotst + nrecerror + nrecfiltered));
	printf("                                ... %10d (not a STAGE account record)\n", nrecnotst);
	printf("                                ... %10d (error)\n", nrecerror);
	printf("                                ... %10d (filtered)\n", nrecfiltered);
	printf("\n");
	print_globstat (starttime, endtime, num_fork_exec_stager, num_multireqs);
	print_poolstat (tflag, aflag, pflag, poolname);

#if defined(_WIN32)
	WSACleanup();
#endif     

	if (errflg)
		exit (SYERR);
	else
		exit (0);
}



/* Function to create a file record for each new file found */

void create_filelist (file_list, file_last, rp)
	struct file_inf **file_list;
	struct file_inf **file_last;
	struct acctstage2 *rp;
{
	struct file_inf *frec;			/* pointer to file_inf record */ 

	frec = (struct file_inf*) calloc (1, sizeof (struct file_inf));
	frec->next = NULL;
	frec->uid = rp->uid;
	frec->gid = rp->gid;	
	strcpy (frec->poolname, rp->u2.s.poolname);
	if (rp->u2.s.t_or_d == 't') {
		strcpy (frec->u1.t.vid, rp->u2.s.u1.t.vid);
		strcpy (frec->u1.t.fseq, rp->u2.s.u1.t.fseq);
		frec->t_or_d = 't';
		num_frecs++;
	} else if (rp->u2.s.t_or_d == 'd') {
		strcpy (frec->u1.d.xfile, rp->u2.s.u1.d.xfile);
		frec->t_or_d = 'd';
		num_drecs++;
	} else if (rp->u2.s.t_or_d == 'm') {
		strcpy (frec->u1.m.xfile, rp->u2.s.u1.m.xfile);
		frec->t_or_d = 'm';
		num_mrecs++;
	} else if (rp->u2.s.t_or_d == 'h') {
		strcpy (frec->u1.h.xfile, rp->u2.s.u1.h.xfile);
		frec->u1.h.fileid = rp->u2.s.u1.h.fileid;
		frec->t_or_d = 'h';
		num_hrecs++;
	}
					
	if (*file_list == NULL) {
		*file_list = frec;
		*file_last = frec;
	}
	else {
		(*file_last)->next = frec;
		*file_last = frec;
	}
}

/* Function to create record for each pool group found */

void create_pool_list (rp)
	struct acctstage2 *rp;
{
	struct pool_inf *pf;				/* pointer to pool_inf record */

	pf = (struct pool_inf*) calloc (1, sizeof (struct pool_inf));
	pf->next = NULL;
	strcpy (pf->poolname, rp->u2.s.poolname);
	if (pool_list == NULL) {
		pool_list = pf;
		pool_last = pf;
	}
	else {
		pool_last->next = pf;
		pool_last = pf;
	}
}



/* Function to create record for each stagein request */

void create_stglist (rp)
	struct acctstage2 *rp;
{
	struct stg_inf *sp;				/* pointer to stg_inf record */
	
	if ((sp = (struct stg_inf*) calloc (1, sizeof (struct stg_inf))) == NULL) {
		fprintf(stderr,"calloc error : %s\n", strerror(errno));
		return;
	}
	sp->next = NULL;
	sp->reqid = rp->reqid;
	if (stage_list == NULL) {
		stage_list = sp;
		stg_last = sp;
	} else {
		stg_last->next = sp;
		stg_last = sp;
	}
}


/* Function to covert date and time into seconds */

time_t
cvt_datime (arg)
	char *arg;
{
	time_t current_time;
	static int lastd[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
	int n;
	struct tm t;
	struct tm *tm;

	memset ((char *) &t, 0, sizeof(struct tm));
	time (&current_time);		/* Get current time */
	tm = localtime (&current_time);
	n = sscanf (arg, "%2d%2d%2d%2d%2d", &t.tm_mon, &t.tm_mday, &t.tm_hour,
				&t.tm_min, &t.tm_year);
	if (n < 4) return (-1);
	if (n == 4)
		t.tm_year = tm->tm_year;
	else if (t.tm_year >= 0 && t.tm_year <= 37)
		t.tm_year += 100;
	else if (t.tm_year >= 38 && t.tm_year <= 69)
		return (-1);
	if ((t.tm_mon < 1) || (t.tm_mon > 12)) return (-1);
	if ((t.tm_mon == 2) && (t.tm_mday == 29) && (t.tm_year % 4 != 0)) return (-1);
	if ((t.tm_mday < 1) || (t.tm_mday > lastd[t.tm_mon-1])) return (-1);

	t.tm_mon--;
#if defined(sun) && ! defined(SOLARIS)
	return (timelocal (&t));
#else
	t.tm_isdst = -1;
	return (mktime (&t));
#endif
}

/* Function to enter file record details for each FILC */

void enter_filc_details (rp, accthdr)
	struct acctstage2 *rp;
	struct accthdr accthdr;
{
	int found = 0;			/* record found flag */
	struct file_inf *frecord;		/* pointer to file_inf record */
	
	/* search through file list for record relating to same file */ 
	
	frecord = file_in_list;
	while (frecord != NULL) {
		if (frecord->t_or_d == 't') {
			if ((strcmp (rp->u2.s.u1.t.vid, frecord->u1.t.vid) == 0) && 
				(strcmp (rp->u2.s.u1.t.fseq, frecord->u1.t.fseq) == 0)&&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)) {
				found = 1;
				break;
			}
		} else if (frecord->t_or_d == 'd') {
			if ((strcmp (rp->u2.s.u1.d.xfile, frecord->u1.d.xfile) == 0) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)) {
				found = 1;
				break;
			}
		} else if (frecord->t_or_d == 'm') {
			if ((strcmp (rp->u2.s.u1.m.xfile, frecord->u1.m.xfile) == 0) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)) {
				found = 1;
				break;
			}
		} else if (frecord->t_or_d == 'h') {
			if ((strcmp (rp->u2.s.u1.h.xfile, frecord->u1.h.xfile) == 0) &&
				(rp->u2.s.u1.h.fileid == frecord->u1.h.fileid) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)) {
				found = 1;
				break;
			}
		}
		frecord = frecord->next;
	}

	/* if record found reset stage_status to 0 and calculate staged in time if staged */
	/* in during the relevant time period */

	if (found) {
		if (frecord->last_stgin > 0 && frecord->stage_status == 1) {
			if (rp->uid == 0) {		/* garbage collected */
				frecord->total_stgin_gc = frecord->total_stgin_gc + 
					(accthdr.timestamp - frecord->last_stgin);
				frecord->num_periods_gc++;
				if (rp->u2.s.c_time > 0) {
					frecord->total_stgin_gc2 = frecord->total_stgin_gc2 + 
						(accthdr.timestamp - rp->u2.s.c_time);
					frecord->num_periods_gc2++;
				}
			} else {				/* user cleared */
				frecord->total_stgin_uc = frecord->total_stgin_uc +
					(accthdr.timestamp - frecord->last_stgin);
				frecord->num_periods_uc++;
				if (rp->u2.s.c_time > 0) {
					frecord->total_stgin_uc2 = frecord->total_stgin_uc2 + 
						(accthdr.timestamp - rp->u2.s.c_time);
					frecord->num_periods_uc2++;
				}
			}
		}
		frecord->stage_status = 0;
	} else if ((use_c_time) && (rp->u2.s.c_time > 0)) {
		/* Record not staged in during relevant time period but use_c_time */
		/* is in action and we have the creation_time information */
		/* This mean we can simulate a sucessful stagein by creation the frecord */
		create_filelist (&file_in_list, &file_in_last, rp);
		enter_pool_details (rp);
		frecord = file_in_last;
		frecord->last_stgin = rp->u2.s.c_time;
		frecord->num_accesses++;
		frecord->stage_status = 1;
		if (rp->uid == 0) {		/* garbage collected */
			frecord->total_stgin_gc2 = frecord->total_stgin_gc2 + 
				(accthdr.timestamp - rp->u2.s.c_time);
			frecord->num_periods_gc2++;
		} else {				/* user cleared */
			frecord->total_stgin_uc2 = frecord->total_stgin_uc2 + 
				(accthdr.timestamp - rp->u2.s.c_time);
			frecord->num_periods_uc2++;
		}
	}
}


/* Function to enter details of each file successful stagein command */

void enter_fils_details (rp, accthdr)
	struct acctstage2 *rp;
	struct accthdr accthdr;
{
	int found = 0;			/* record found flag */
	int match = 0;			/* stg_inf record match flag */
	struct file_inf *frecord;		/* pointer to file_inf record */

	/* search file list for matching file and pool */

	frecord = file_in_list;
	while (frecord != NULL) {
		if (rp->u2.s.t_or_d == 't' && frecord->t_or_d == 't') {
			if ((strcmp (rp->u2.s.u1.t.vid, frecord->u1.t.vid) == 0) && 
				(strcmp (rp->u2.s.u1.t.fseq, frecord->u1.t.fseq) == 0) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)) {
				found = 1;
				break;
			}
		} else if (rp->u2.s.t_or_d == 'd' && frecord->t_or_d == 'd') {
			if ((strcmp (rp->u2.s.u1.d.xfile, frecord->u1.d.xfile) == 0) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)){
				found = 1;
				break;
			}
		} else if (rp->u2.s.t_or_d == 'm' && frecord->t_or_d == 'm') {
			if ((strcmp (rp->u2.s.u1.m.xfile, frecord->u1.m.xfile) == 0) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)){
				found = 1;
				break;
			}
		} else if (rp->u2.s.t_or_d == 'h' && frecord->t_or_d == 'h') {
			if ((strcmp (rp->u2.s.u1.h.xfile, frecord->u1.h.xfile) == 0) &&
				(rp->u2.s.u1.h.fileid == frecord->u1.h.fileid) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)){
				found = 1;
				break;
			}
		}
		frecord = frecord->next;
	}

	/* if a matching record is not found then create one, else increment num_in_remount_requests */

	if (!found) {
		create_filelist (&file_in_list, &file_in_last, rp);
		frecord = file_in_last;
	}	else if (rp->u2.s.nbaccesses <= 1) {
		num_in_remount_requests++; 
	}

	/* if number of accesses is <= 1 and no error on exit of command, set last stgin time */
	/* to timestamp of the current record */	

	if ((rp->u2.s.nbaccesses <= 1) &&
		((rp->exitcode == 0) ||
		 (rp->exitcode == 193) ||
		 (rp->exitcode == 194) ||
		 (rp->exitcode == 195) ||
		 (rp->exitcode == 197)
			)
		) {
		frecord->last_stgin = (use_c_time && rp->u2.s.c_time > 0) ? rp->u2.s.c_time : accthdr.timestamp;
	}

	if (rp->retryn == 0) {
		match = match_2_stgin (rp);
		frecord->num_accesses++;
		frecord->stage_status = 1;
		if (match) srec_match->num_files++;
	}
}
			

/* Function to enter details of each file successful stagewrt/stageput command */

void enter_filw_details (rp, accthdr)
	struct acctstage2 *rp;
	struct accthdr accthdr;
{
	int found = 0;			/* record found flag */
	/* int match = 0; */			/* stg_inf record match flag */
	struct file_inf *frecord;		/* pointer to file_inf record */

	/* search file list for matching file and pool */

	frecord = file_wrt_list;
	while (frecord != NULL) {
		if (rp->u2.s.t_or_d == 't' && frecord->t_or_d == 't') {
			if ((strcmp (rp->u2.s.u1.t.vid, frecord->u1.t.vid) == 0) && 
				(strcmp (rp->u2.s.u1.t.fseq, frecord->u1.t.fseq) == 0) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)) {
				found = 1;
				break;
			}
		} else if (rp->u2.s.t_or_d == 'd' && frecord->t_or_d == 'd') {
			if ((strcmp (rp->u2.s.u1.d.xfile, frecord->u1.d.xfile) == 0) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)){
				found = 1;
				break;
			}
		} else if (rp->u2.s.t_or_d == 'm' && frecord->t_or_d == 'm') {
			if ((strcmp (rp->u2.s.u1.m.xfile, frecord->u1.m.xfile) == 0) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)){
				found = 1;
				break;
			}
		} else if (rp->u2.s.t_or_d == 'h' && frecord->t_or_d == 'h') {
			if ((strcmp (rp->u2.s.u1.h.xfile, frecord->u1.h.xfile) == 0) &&
				(rp->u2.s.u1.h.fileid == frecord->u1.h.fileid) &&
				(strcmp (rp->u2.s.poolname, frecord->poolname) == 0)){
				found = 1;
				break;
			}
		}
		frecord = frecord->next;
	}

	/* if a matching record is not found then create one, else increment num_wrt_remount_requests */

	if (!found) {
		create_filelist (&file_wrt_list, &file_wrt_last, rp);
		frecord = file_wrt_last;
	}	else if (rp->u2.s.nbaccesses <= 1) {
		num_wrt_remount_requests++; 
	}

	/* if number of accesses is <= 1 and no error on exit of command, set last stgin time */
	/* to timestamp of the current record */	

	/*
	  if ((rp->u2.s.nbaccesses <= 1) &&
      ((rp->exitcode == 0) ||
	  (rp->exitcode == 193) ||
	  (rp->exitcode == 194) ||
	  (rp->exitcode == 195) ||
	  (rp->exitcode == 197)
	  )
      ) {
	  frecord->last_stgin = accthdr.timestamp;
	  }

	  if (rp->retryn == 0) {
	  match = match_2_stgin (rp);
	  frecord->num_accesses++;
	  frecord->stage_status = 1;
	  if (match) srec_match->num_files++;
	  }
	*/
}
			

/* Function to enter details for each pool */

void enter_pool_details (rp)
	struct acctstage2 *rp;
{
	struct pool_inf *pf;			/* pointer to pool_inf record */
	int found = 0;

	pf = pool_list;
	while (pf != NULL) {
		if (strcmp (pf->poolname, rp->u2.s.poolname) == 0) {
			found = 1;
			break;
		}
		else pf = pf->next;
	}

	if (strcmp (rp->u2.s.poolname, "") != 0 && !found)
		create_pool_list (rp);
}

/* Function to read each record from the accounting file */

int getacctrec (fd_acct, accthdr, buf,swapped)
	int fd_acct;
	struct accthdr *accthdr;
	char *buf;
	int *swapped;
{
	int c;
	struct acctstage rp;		/* pointer to accstage record (backward compatibility) */
	struct acctstage2 rp2;

	rfio_errno = serrno = 0;
	if ((c = rfio_read (fd_acct,accthdr,sizeof(struct accthdr))) != sizeof(struct accthdr)) {
		if (c == 0) return (0);
		if (c > 0)
			fprintf (stderr, "rfio_read returns %d\n", c);
		else
			fprintf (stderr, "rfio_read error : %s\n", rfio_serror());
		exit (SYERR);
	}

	size_read += c;
	
	/* If package is > 255 then byte order needs swopping */

	if (accthdr->package > 255) {
		swap_it (accthdr->package);
		swap_it (accthdr->len);
		swap_it (accthdr->timestamp);
		*swapped = 1;
	}

	if ((accthdr->package != ACCTSTAGE) && (accthdr->package != ACCTSTAGE2)) {
		/* Not a STAGE accouting record - we just seek the pointer */
		rfio_errno = serrno = 0;
		if (rfio_lseek(fd_acct, accthdr->len, SEEK_CUR) < 0) {
			fprintf (stderr, "rfio_lseek error : %s\n", rfio_serror());
			exit (SYERR);
		}
		size_read += accthdr->len;
		return (accthdr->len);
	}

	rfio_errno = serrno = 0;
	switch (accthdr->package) {
	case ACCTSTAGE2:
		if ((c = rfio_read (fd_acct, buf, accthdr->len)) != accthdr->len) {
			if (c >= 0)
				fprintf (stderr, "rfio_read returns %d\n",c);
			else
				fprintf (stderr, "rfio_read error : %s\n", rfio_serror());
			exit (SYERR);
		}
		break;
	case ACCTSTAGE:
		if ((c = rfio_read (fd_acct, (char *) &rp, accthdr->len)) != accthdr->len) {
			if (c >= 0)
				fprintf (stderr, "rfio_read returns %d\n",c);
			else
				fprintf (stderr, "rfio_read error : %s\n", rfio_serror());
			exit (SYERR);
		}
		/* Convert acctstage to acctstage2 */
		rp2.uid = rp.uid;
		rp2.gid = rp.gid;
		rp2.u2.s.actual_size = rp.u2.s.actual_size;
		rp2.u2.s.c_time = 0; /* Not set in acctstage */
		rp2.subtype = rp.subtype;
		rp2.reqid = rp.reqid;
		rp2.req_type = rp.req_type;
		rp2.retryn = rp.retryn;
		rp2.exitcode = rp.exitcode;
		strcpy(rp2.u2.clienthost,rp.u2.clienthost);
		rp2.u2.s.t_or_d = rp.u2.s.t_or_d;
		if (rp.u2.s.t_or_d == 't' || rp.u2.s.t_or_d == 'd' || rp.u2.s.t_or_d == 'm' || rp.u2.s.t_or_d == 'h') {
			strcpy(rp2.u2.s.poolname,rp.u2.s.poolname);
			rp2.u2.s.nbaccesses = rp.u2.s.nbaccesses;
			switch (rp.u2.s.t_or_d) {
			case 't':				
				rp2.u2.s.u1.t.side = 0; /* Not set in acctstage */
				strcpy(rp2.u2.s.u1.t.dgn, rp.u2.s.u1.t.dgn);
				strcpy(rp2.u2.s.u1.t.fseq, rp.u2.s.u1.t.fseq);
				strcpy(rp2.u2.s.u1.t.vid, rp.u2.s.u1.t.vid);
				strcpy(rp2.u2.s.u1.t.tapesrvr, rp.u2.s.u1.t.tapesrvr);
				break;
			case 'd':
				strcpy(rp2.u2.s.u1.d.xfile, rp.u2.s.u1.d.xfile);
				break;
			case 'm':
				strcpy(rp2.u2.s.u1.m.xfile, rp.u2.s.u1.m.xfile);
				break;
			case 'h':
				strcpy(rp2.u2.s.u1.h.xfile, rp.u2.s.u1.h.xfile);
				rp2.u2.s.u1.h.fileid = rp.u2.s.u1.h.fileid;
				break;
			}
		}
		memcpy((void *) buf,(void *) &rp2,sizeof(struct acctstage2));
		break;
	}
	size_read += c;
	return (accthdr->len);
}

/* Function to match a FILS request with a stagein request */

int match_2_stgin (rp)
	struct acctstage2 *rp;
{
	int matched =0;				/* record matched flag */
	struct stg_inf *srec;			/* pointer to stg_inf record */
	
	srec = stage_list;
	while (srec != NULL) {
		if (rp->reqid == srec->reqid) {
			matched = 1;
			srec_match = srec;
			break;
		} else {
			srec = srec->next;
		}
	}
	return(matched);
}

	
/* Function to print out a record */

void print_fdetails (frecord)
	struct file_inf *frecord;
{
	struct passwd *pwd;				/* password structure */
	struct group *grp;				/* group structure */
	float avg = 0.0;				/* average lifetime using last access */
	float avg2 = 0.0;				/* average lifetime using creation time */
	struct passwd pwd_unknown;
	struct group grp_unknown;
	char pw_name_unknown[21];
	char gr_name_unknown[21];
	
	/* get the password and group name for each record */
	
	pwd_unknown.pw_name = pw_name_unknown;
	grp_unknown.gr_name = gr_name_unknown;
	pwd = Cgetpwuid (frecord->uid);
	if (pwd == NULL) {
		if (frecord->uid >= INT_MAX) {
			sprintf(pwd_unknown.pw_name,"<undef>");
		} else {
			sprintf(pwd_unknown.pw_name,"%d",frecord->uid);
		}
		pwd = &pwd_unknown;
	}
	grp = Cgetgrgid (frecord->gid);
	if (grp == NULL) {
		if (frecord->gid >= INT_MAX) {
			sprintf(grp_unknown.gr_name,"<undef>");
		} else {
			sprintf(grp_unknown.gr_name,"%d",frecord->gid);
		}
		grp = &grp_unknown;
	}

	/* Calculate the average life time of the file. If the file has been garbage */
	/* collected then average life is calculated using the garbage collected info */
	/* only.  If the file has not been garbage collected, then the average life */
	/* is calculated using the user cleared information */

	
	if (frecord->num_periods_gc > 0) 
		avg = (frecord->total_stgin_gc / 3600.0) / frecord->num_periods_gc;
	if (all_stageclrs && frecord->num_periods_uc > 0)
		avg += (frecord->total_stgin_uc / 3600.0) / frecord->num_periods_uc;

	if (frecord->num_periods_gc2 > 0) 
		avg2 = (frecord->total_stgin_gc2 / 3600.0) / frecord->num_periods_gc2;
	if (all_stageclrs && frecord->num_periods_uc2 > 0)
		avg2 += (frecord->total_stgin_uc2 / 3600.0) / frecord->num_periods_uc2;

	if (use_c_time) {
		if ((frecord->num_periods_gc2 > 0 && frecord->total_stgin_gc2 > 0) ||
			(frecord->total_stgin_uc2 > 0 && frecord->num_periods_uc2 > 0))
			printf ("\n%8s %8s    %8.2f  %8d %12s %8s", frecord->u1.t.vid,
					frecord->u1.t.fseq, avg2, frecord->num_accesses, pwd->pw_name, 
					grp->gr_name);
		else if (frecord->stage_status == 1)
			printf ("\n%8s %8s    %8s  %8d %12s %8s", frecord->u1.t.vid,
					frecord->u1.t.fseq,"      nc", frecord->num_accesses, 
					pwd->pw_name, grp->gr_name);
		else if (frecord->last_stgin == 0)
			printf ("\n%8s %8s    %8s  %8d %12s %8s", frecord->u1.t.vid,
					frecord->u1.t.fseq,"      ac", frecord->num_accesses, 
					pwd->pw_name, grp->gr_name);
		else printf ("\n%8s %8s    %8.2f  %8d %12s %8s", frecord->u1.t.vid,
					 frecord->u1.t.fseq, 0., frecord->num_accesses, pwd->pw_name, 
					 grp->gr_name);

	} else {
		if ((frecord->num_periods_gc > 0 && frecord->total_stgin_gc > 0) ||
			(frecord->total_stgin_uc > 0 && frecord->num_periods_uc > 0))
			printf ("\n%8s %8s    %8.2f  %8d %12s %8s", frecord->u1.t.vid,
					frecord->u1.t.fseq, avg, frecord->num_accesses, pwd->pw_name, 
					grp->gr_name);
		else if (frecord->stage_status == 1)
			printf ("\n%8s %8s    %8s  %8d %12s %8s", frecord->u1.t.vid,
					frecord->u1.t.fseq,"      nc", frecord->num_accesses, 
					pwd->pw_name, grp->gr_name);
		else if (frecord->last_stgin == 0)
			printf ("\n%8s %8s    %8s  %8d %12s %8s", frecord->u1.t.vid,
					frecord->u1.t.fseq,"      ac", frecord->num_accesses, 
					pwd->pw_name, grp->gr_name);
		else printf ("\n%8s %8s    %8.2f  %8d %12s %8s", frecord->u1.t.vid,
					 frecord->u1.t.fseq, 0., frecord->num_accesses, pwd->pw_name, 
					 grp->gr_name);

	}
}

/* Function to print out global statistics */

void print_globstat (starttime, endtime, num_fork_exec_stager, num_multireqs)
	time_t starttime;
	time_t endtime;
	int num_fork_exec_stager;
	int num_multireqs;
{
	int num_pos_requests_avoided = 0;

	printf ("\t%s Stager statistics", hostname);
	print_time_interval (starttime, endtime);
	printf ("\nCommand    No Reqs API(%%) Success Warning Userr Syserr Unerr Conferr Held Busy ENOSPC Cleared Killed\n\n");
	printf ("stagein    %7d %6.2f %7d %7d %5d %6d %5d %7d %4d %4d %6d %7d %6d\n",
			nb_req[1], nb_req[1] > 0 ? (float) (100 * nb_apireq[1])/nb_req[1] : 0, rc[1][0], rc[1][7], rc[1][1], rc[1][2], rc[1][3],
			rc[1][4], rc[1][10], rc[1][11], rc[1][5], rc[1][6], rc[1][8]);
	printf ("stageout   %7d %6.2f %7d %7d %5d %6d %5d %7d %4d %4d %6d %7d %6d\n",
			nb_req[2], nb_req[2] > 0 ? (float) (100 * nb_apireq[2])/nb_req[2] : 0, rc[2][0], rc[2][7], rc[2][1], rc[2][2], rc[2][3],
			rc[2][4], rc[2][10], rc[2][11], rc[2][5], rc[2][6], rc[2][8]);
	printf ("stagewrt   %7d %6.2f %7d %7d %5d %6d %5d %7d %4d %4d %6d %7d %6d\n",
			nb_req[3], nb_req[3] > 0 ? (float) (100 * nb_apireq[3])/nb_req[3] : 0, rc[3][0], rc[3][7], rc[3][1], rc[3][2], rc[3][3],
			rc[3][4], rc[3][10], rc[3][11], rc[3][5], rc[3][6], rc[3][8]);
	printf ("stageput   %7d %6.2f %7d %7d %5d %6d %5d %7d %4d %4d %6d %7d %6d\n",
			nb_req[4], nb_req[4] > 0 ? (float) (100 * nb_apireq[4])/nb_req[4] : 0, rc[4][0], rc[4][7], rc[4][1], rc[4][2], rc[4][3],
			rc[4][4], rc[4][10], rc[4][11], rc[4][5], rc[4][6], rc[4][8]);
	printf ("stagealloc %7d %6.2f %7d %7d %5d %6d %5d %7d %4d %4d %6d %7d %6d\n",
			nb_req[11], nb_req[11] > 0 ? (float) (100 * nb_apireq[11])/nb_req[11] : 0, rc[11][0], rc[11][7], rc[11][1], rc[11][2], rc[11][3],
			rc[11][4], rc[11][10], rc[11][11], rc[11][5], rc[11][6], rc[11][8]);

	printf ("stageqry   %7d %6.2f\n", nb_req[5], nb_req[5] > 0 ? (float) (100 * nb_apireq[5])/nb_req[5] : 0);
	printf ("stageclr   %7d %6.2f\n", nb_req[6], nb_req[6] > 0 ? (float) (100 * nb_apireq[6])/nb_req[6] : 0);
	printf ("stagekill  %7d %6.2f\n", nb_req[7], nb_req[7] > 0 ? (float) (100 * nb_apireq[7])/nb_req[7] : 0);
	printf ("stageupdc  %7d %6.2f\n", nb_req[8], nb_req[8] > 0 ? (float) (100 * nb_apireq[8])/nb_req[8] : 0);
	printf ("stageinit  %7d %6.2f\n", nb_req[9], nb_req[9] > 0 ? (float) (100 * nb_apireq[9])/nb_req[9] : 0);
	printf ("stagecat   %7d %6.2f\t\n", nb_req[10], nb_req[10] > 0 ? (float) (100 * nb_apireq[10])/nb_req[10] : 0);
	printf ("stageget   %7d %6.2f\t\n", nb_req[12], nb_req[12] > 0 ? (float) (100 * nb_apireq[12])/nb_req[12] : 0);
	printf ("stagechng  %7d %6.2f\t\n", nb_req[14], nb_req[14] > 0 ? (float) (100 * nb_apireq[14])/nb_req[14] : 0);
	printf ("stageshutdown %4d %6.2f\t\n", nb_req[15], nb_req[15] > 0 ? (float) (100 * nb_apireq[15])/nb_req[15] : 0);
	printf ("stageping  %7d %6.2f\t\n", nb_req[16], nb_req[16] > 0 ? (float) (100 * nb_apireq[16])/nb_req[16] : 0);
	printf ("restart(s) %7d\t\n", nb_restart);
	printf ("\nStagein Statistics :\n");
	printf ("\tNumber of forked I/O process\t\t\t%d\n", num_fork_exec_stager);
	printf ("\tNumber of tape remount requests (write mode)\t\t\t%d\n", num_wrt_remount_requests);
	printf ("\tNumber of tape remount requests (read  mode)\t\t\t%d\n", num_in_remount_requests);
	num_pos_requests_avoided = rc[1][0] - num_fork_exec_stager;
	if (num_pos_requests_avoided < 0 ) num_pos_requests_avoided = 0;
	printf ("\tNumber of tape positionning requests avoided by stager (read mode)\t%d\n", num_pos_requests_avoided);
	printf ("\tNumber of multifile requests\t\t%d\n", num_multireqs);
}

/* Function to print out the time interval */

void print_time_interval (starttime, endtime)
	time_t starttime;
	time_t endtime;
{
	struct tm *tm;

	tm = localtime (&starttime);
	printf (" (%02d/%02d/%04d %02d:%02d:%02d",
			tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min,
			tm->tm_sec);
	tm = localtime (&endtime);
	printf ("  -  %02d/%02d/%04d %02d:%02d:%02d)\n",
			tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min,
			tm->tm_sec);
}


/*Function to print out a Disk record*/

void print_ddetails (frecord)
	struct file_inf * frecord;
{
	struct passwd *pwd;				/* password structure */
	struct group *grp;				/* group structure */
	float avg = 0.0;				/* average life */
	float avg2 = 0.0;				/* average life */
	struct passwd pwd_unknown;
	struct group grp_unknown;
	char pw_name_unknown[21];
	char gr_name_unknown[21];

	/* get the password and group name for each record */
	
	pwd_unknown.pw_name = pw_name_unknown;
	grp_unknown.gr_name = gr_name_unknown;
	pwd = Cgetpwuid (frecord->uid);
	if (pwd == NULL) {
		if (frecord->uid >= INT_MAX) {
			sprintf(pwd_unknown.pw_name,"<undef>");
		} else {
			sprintf(pwd_unknown.pw_name,"%d",frecord->uid);
		}
		pwd = &pwd_unknown;
	}
	grp = Cgetgrgid (frecord->gid);
	if (grp == NULL) {
		if (frecord->gid >= INT_MAX) {
			sprintf(grp_unknown.gr_name,"<undef>");
		} else {
			sprintf(grp_unknown.gr_name,"%d",frecord->gid);
		}
		grp = &grp_unknown;
	}

	/* Calculate the average life time of the file. If the file has been garbage */
	/* collected then average life is calculated using the garbage collected info */
	/* only.  If the file has not been garbage collected, then the average life */
	/* is calculated using the user cleared information */

	if (frecord->num_periods_gc > 0) 
		avg = (frecord->total_stgin_gc / 3600.0) / frecord->num_periods_gc;
	if (all_stageclrs && frecord->num_periods_uc > 0)
		avg += (frecord->total_stgin_uc / 3600.0) / frecord->num_periods_uc;

	if (frecord->num_periods_gc2 > 0) 
		avg2 = (frecord->total_stgin_gc2 / 3600.0) / frecord->num_periods_gc2;
	if (all_stageclrs && frecord->num_periods_uc2 > 0)
		avg2 += (frecord->total_stgin_uc2 / 3600.0) / frecord->num_periods_uc2;

	if (use_c_time) {
		if ((frecord->num_periods_gc2 > 0 && frecord->total_stgin_gc2 > 0) ||
			(frecord->num_periods_uc2 > 0 && frecord->total_stgin_uc2 > 0))
			printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.d.xfile,
					avg2, frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->stage_status == 1)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.d.xfile,
					"      nc", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->last_stgin == 0)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.d.xfile,
					"      ac", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.d.xfile,
					 0., frecord->num_accesses, pwd->pw_name, grp->gr_name);
	} else {
		if ((frecord->num_periods_gc > 0 && frecord->total_stgin_gc > 0) ||
			(frecord->num_periods_uc > 0 && frecord->total_stgin_uc > 0))
			printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.d.xfile,
					avg, frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->stage_status == 1)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.d.xfile,
					"      nc", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->last_stgin == 0)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.d.xfile,
					"      ac", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.d.xfile,
					 0., frecord->num_accesses, pwd->pw_name, grp->gr_name);
	}
}

/*Function to print out a Hsm record*/

void print_mdetails (frecord)
	struct file_inf * frecord;
{
	struct passwd *pwd;				/* password structure */
	struct group *grp;				/* group structure */
	float avg = 0.0;				/* average life */
	float avg2 = 0.0;				/* average life */
	struct passwd pwd_unknown;
	struct group grp_unknown;
	char pw_name_unknown[21];
	char gr_name_unknown[21];

	/* get the password and group name for each record */
	
	pwd_unknown.pw_name = pw_name_unknown;
	grp_unknown.gr_name = gr_name_unknown;
	pwd = Cgetpwuid (frecord->uid);
	if (pwd == NULL) {
		if (frecord->uid >= INT_MAX) {
			sprintf(pwd_unknown.pw_name,"<undef>");
		} else {
			sprintf(pwd_unknown.pw_name,"%d",frecord->uid);
		}
		pwd = &pwd_unknown;
	}
	grp = Cgetgrgid (frecord->gid);
	if (grp == NULL) {
		if (frecord->gid >= INT_MAX) {
			sprintf(grp_unknown.gr_name,"<undef>");
		} else {
			sprintf(grp_unknown.gr_name,"%d",frecord->gid);
		}
		grp = &grp_unknown;
	}


	/* Calculate the average life time of the file. If the file has been garbage */
	/* collected then average life is calculated using the garbage collected info */
	/* only.  If the file has been also cleared by user then total average life */
	/* is computed if all_stageclrs is in action */

	if (frecord->num_periods_gc > 0) 
		avg = (frecord->total_stgin_gc / 3600.0) / frecord->num_periods_gc;
	if (all_stageclrs && frecord->num_periods_uc > 0)
		avg += (frecord->total_stgin_uc / 3600.0) / frecord->num_periods_uc;

	if (frecord->num_periods_gc2 > 0) 
		avg2 = (frecord->total_stgin_gc2 / 3600.0) / frecord->num_periods_gc2;
	if (all_stageclrs && frecord->num_periods_uc2 > 0)
		avg2 += (frecord->total_stgin_uc2 / 3600.0) / frecord->num_periods_uc2;

	if (use_c_time) {
		if ((frecord->num_periods_gc2 > 0 && frecord->total_stgin_gc2 > 0) ||
			(frecord->num_periods_uc2 > 0 && frecord->total_stgin_uc2 > 0))
			printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.m.xfile,
					avg2, frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->stage_status == 1)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.m.xfile,
					"      nc", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->last_stgin == 0)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.m.xfile,
					"      ac", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.m.xfile,
					 0., frecord->num_accesses, pwd->pw_name, grp->gr_name);
	} else {
		if ((frecord->num_periods_gc > 0 && frecord->total_stgin_gc > 0) ||
			(frecord->num_periods_uc > 0 && frecord->total_stgin_uc > 0))
			printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.m.xfile,
					avg, frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->stage_status == 1)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.m.xfile,
					"      nc", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->last_stgin == 0)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.m.xfile,
					"      ac", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.m.xfile,
					 0., frecord->num_accesses, pwd->pw_name, grp->gr_name);
	}
}

/*Function to print out a CASTOR record*/

void print_hdetails (frecord)
	struct file_inf * frecord;
{
	struct passwd *pwd;				/* password structure */
	struct group *grp;				/* group structure */
	float avg = 0.0;				/* average life using last access */
	float avg2 = 0.0;				/* average life using creation time */
	struct passwd pwd_unknown;
	struct group grp_unknown;
	char pw_name_unknown[21];
	char gr_name_unknown[21];

	/* get the password and group name for each record */
	
	pwd_unknown.pw_name = pw_name_unknown;
	grp_unknown.gr_name = gr_name_unknown;
	pwd = Cgetpwuid (frecord->uid);
	if (pwd == NULL) {
		if (frecord->uid >= INT_MAX) {
			sprintf(pwd_unknown.pw_name,"<undef>");
		} else {
			sprintf(pwd_unknown.pw_name,"%d",frecord->uid);
		}
		pwd = &pwd_unknown;
	}
	grp = Cgetgrgid (frecord->gid);
	if (grp == NULL) {
		if (frecord->gid >= INT_MAX) {
			sprintf(grp_unknown.gr_name,"<undef>");
		} else {
			sprintf(grp_unknown.gr_name,"%d",frecord->gid);
		}
		grp = &grp_unknown;
	}

	/* Calculate the average life time of the file. If the file has been garbage */
	/* collected then average life is calculated using the garbage collected info */
	/* only.  If the file has not been garbage collected, then the average life */
	/* is calculated using the user cleared information */

	if (frecord->num_periods_gc > 0) 
		avg = (frecord->total_stgin_gc / 3600.0) / frecord->num_periods_gc;
	if (all_stageclrs && frecord->num_periods_uc > 0)
		avg += (frecord->total_stgin_uc / 3600.0) / frecord->num_periods_uc;

	if (frecord->num_periods_gc2 > 0) 
		avg2 = (frecord->total_stgin_gc2 / 3600.0) / frecord->num_periods_gc2;
	if (all_stageclrs && frecord->num_periods_uc2 > 0)
		avg2 += (frecord->total_stgin_uc2 / 3600.0) / frecord->num_periods_uc2;

	if (use_c_time) {
		if ((frecord->num_periods_gc2 > 0 && frecord->total_stgin_gc2 > 0) ||
			(frecord->num_periods_uc2 > 0 && frecord->total_stgin_uc2 > 0))
			printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.h.xfile,
					avg2, frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->stage_status == 1)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.h.xfile,
					"      nc", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->last_stgin == 0)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.h.xfile,
					"      ac", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.h.xfile,
					 0., frecord->num_accesses, pwd->pw_name, grp->gr_name);
	} else {
		if ((frecord->num_periods_gc > 0 && frecord->total_stgin_gc > 0) ||
			(frecord->num_periods_uc > 0 && frecord->total_stgin_uc > 0))
			printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.h.xfile,
					avg, frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->stage_status == 1)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.h.xfile,
					"      nc", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else if (frecord->last_stgin == 0)
			printf ("\n%-35s %8s  %8d   %15s %15s", frecord->u1.h.xfile,
					"      ac", frecord->num_accesses, pwd->pw_name, grp->gr_name);
		else printf ("\n%-35s %8.2f  %8d   %15s %15s", frecord->u1.h.xfile,
					 0., frecord->num_accesses, pwd->pw_name, grp->gr_name);
	}
}


/* Function to create  pool information */

void sort_poolinf ()
{
	struct pool_inf *pf;				/* pointer to pool_inf record */
	struct file_inf *frecord;			/* pointer to file_inf record */

	/* for each record, match to its pool and set the relevant variables in the pool */
	/* structure  */

	frecord = file_in_list;
	while (frecord != NULL) {
		pf = pool_list;
		while (pf != NULL) {
			if (strcmp (frecord->poolname, pf->poolname) == 0){
				if (frecord->stage_status == 1 && frecord->last_stgin == 0) pf->acc++;
				else if (frecord->stage_status == 1 && frecord->last_stgin > 0) 
					pf->stg_acc++;
				else if (frecord->stage_status == 0 && frecord->last_stgin > 0) 
					pf->stg_acc_clrd++; 
				else if (frecord->stage_status == 0 && frecord->last_stgin == 0)
					pf->acc_clrd++;
				if (use_c_time) {
					if (frecord->num_periods_gc2>0 && frecord->total_stgin_gc2 > 0) {
						pf->total_stged_files2++;
						pf->total_avg_life2 += ((frecord->total_stgin_gc2 / 3600.0) / frecord->num_periods_gc2);
					}
					if ((all_stageclrs) && (frecord->num_periods_uc2>0 && frecord->total_stgin_uc2 > 0)) {
						pf->total_stged_files2++;
						pf->total_avg_life2 += ((frecord->total_stgin_uc2 / 3600.0) / frecord->num_periods_uc2);
					}
				} else {
					if (frecord->num_periods_gc>0 && frecord->total_stgin_gc > 0) {
						pf->total_stged_files++;
						pf->total_avg_life += ((frecord->total_stgin_gc / 3600.0) / frecord->num_periods_gc);
					}
					if ((all_stageclrs) && (frecord->num_periods_uc>0 && frecord->total_stgin_uc > 0)) {
						pf->total_stged_files++;
						pf->total_avg_life += ((frecord->total_stgin_uc / 3600.0) / frecord->num_periods_uc);
					}
				}
				if (frecord->t_or_d == 't') pf->num_frecs++;
				else if (frecord->t_or_d == 'd') pf->num_drecs++;
				else if (frecord->t_or_d == 'm') pf->num_mrecs++;
				else if (frecord->t_or_d == 'h') pf->num_hrecs++;
				pf->num_files++;
				pf->total_accesses = pf->total_accesses + frecord->num_accesses;
				break;
			}
			else pf = pf->next;
		}
		frecord = frecord->next;
	}
}


/* Function to create pool records and print results */

void print_poolstat (tflag, aflag, pflag, poolname)
	int tflag;
	int aflag;
	int pflag;
	char poolname[CA_MAXPOOLNAMELEN + 1];
{
	int i; 						/* counter */ 
	int fi = 0; 					/* counter for tape files */
	int di = 0;					/* counter for disk files */
	int mi = 0;					/* counter for non-CASTOR-HSM files */
	int hi = 0;					/* counter for CASTOR-HSM files */
	struct frec_nbaccs *faccs = NULL;		/* arrays to store address of each record */
	struct frec_nbaccs *daccs = NULL;		/* along with either the average life or the */
	struct frec_nbaccs *maccs = NULL;
	struct frec_nbaccs *haccs = NULL;
	struct frec_avg *favg = NULL;			/* number of accesses and poolname.  These */
	struct frec_avg *davg = NULL;			/* arrays are used to sort the data */
	struct frec_avg *mavg = NULL;
	struct frec_avg *havg = NULL;			/* arrays are used to sort the data */
	char current_pool[CA_MAXPOOLNAMELEN + 1];		/* current pool name */
	struct pool_inf *pf;				/* pointer to pool_inf record */
	struct file_inf *frecord;			/* pointer to file_inf record */
	int comp ();
	int comp2 ();

	/* If a sort criteria has been given then create the relevant array to store the */
	/* data */

	if (aflag && num_frecs > 0)
		faccs = (struct frec_nbaccs*) calloc (num_frecs, sizeof (struct frec_nbaccs));
	else if (tflag && num_frecs > 0)
		favg = (struct frec_avg*) calloc (num_frecs, sizeof (struct frec_avg));

	if (aflag && num_drecs > 0)
		daccs = (struct frec_nbaccs*) calloc (num_drecs, sizeof (struct frec_nbaccs));
	else if (tflag && num_drecs > 0)
		davg = (struct frec_avg*) calloc (num_drecs, sizeof (struct frec_avg));

	if (aflag && num_mrecs > 0)
		maccs = (struct frec_nbaccs*) calloc (num_mrecs, sizeof (struct frec_nbaccs));
	else if (tflag && num_drecs > 0)
		mavg = (struct frec_avg*) calloc (num_mrecs, sizeof (struct frec_avg));

	if (aflag && num_hrecs > 0)
		haccs = (struct frec_nbaccs*) calloc (num_hrecs, sizeof (struct frec_nbaccs));
	else if (tflag && num_hrecs > 0)
		havg = (struct frec_avg*) calloc (num_hrecs, sizeof (struct frec_avg));

	/* once arrays have been created cycle through the file records and copy the */
	/* relevant data into the sort array */
		
	if (aflag || tflag) {
		frecord = file_in_list;
		while (frecord != NULL){
			if (num_frecs > 0 && frecord->t_or_d == 't') {
				if (aflag) {
					faccs[fi].rec = frecord;
					strcpy (faccs[fi].poolname, frecord->poolname);
					faccs[fi].nbaccesses = frecord->num_accesses;
				} else if (tflag) {
					favg[fi].rec = frecord;
					strcpy (favg[fi].poolname, frecord->poolname);
					if (use_c_time) {
						if (frecord->num_periods_gc2 > 0 && frecord->total_stgin_gc2 > 0)
							favg[fi].avg_life2 = (frecord->total_stgin_gc2 / 3600.0)
								/ frecord->num_periods_gc2;
						if (all_stageclrs && frecord->num_periods_uc2 > 0 && frecord->total_stgin_uc2
								 > 0)
							favg[fi].avg_life2 += (frecord->total_stgin_uc2 / 3600.0)
								/ frecord->num_periods_uc2;
					} else {
						if (frecord->num_periods_gc > 0 && frecord->total_stgin_gc > 0)
							favg[fi].avg_life = (frecord->total_stgin_gc / 3600.0)
								/ frecord->num_periods_gc;
						if (all_stageclrs && frecord->num_periods_uc > 0 && frecord->total_stgin_uc
								 > 0)
							favg[fi].avg_life += (frecord->total_stgin_uc / 3600.0)
								/ frecord->num_periods_uc;
					}
				}
				fi++;
			}
			if (num_drecs > 0 && frecord->t_or_d == 'd') {
				if (aflag) {
					daccs[di].rec = frecord;
					strcpy (daccs[di].poolname, frecord->poolname);
					daccs[di].nbaccesses = frecord->num_accesses;
				} else if (tflag) {
					davg[di].rec = frecord;
					strcpy (davg[di].poolname, frecord->poolname);
					if (use_c_time) {
						if (frecord->num_periods_gc2 > 0 && frecord->total_stgin_gc2 > 0)
							davg[di].avg_life2 = (frecord->total_stgin_gc2 / 3600.0)
								/ frecord->num_periods_gc2;
						if (all_stageclrs && frecord->num_periods_uc2 > 0 && frecord->total_stgin_uc2
								 > 0)
							davg[di].avg_life2 += (frecord->total_stgin_uc2 / 3600.0)
								/ frecord->num_periods_uc2;
					} else {
						if (frecord->num_periods_gc > 0 && frecord->total_stgin_gc > 0)
							davg[di].avg_life = (frecord->total_stgin_gc / 3600.0)
								/ frecord->num_periods_gc;
						if (all_stageclrs && frecord->num_periods_uc > 0 && frecord->total_stgin_uc
								 > 0)
							davg[di].avg_life += (frecord->total_stgin_uc / 3600.0)
								/ frecord->num_periods_uc;
					}
				}
				di++;
			}
			if (num_mrecs > 0 && frecord->t_or_d == 'm') {
				if (aflag) {
					maccs[mi].rec = frecord;
					strcpy (maccs[mi].poolname, frecord->poolname);
					maccs[mi].nbaccesses = frecord->num_accesses;
				} else if (tflag) {
					mavg[mi].rec = frecord;
					strcpy (mavg[mi].poolname, frecord->poolname);
					if (use_c_time) {
						if (frecord->num_periods_gc2 > 0 && frecord->total_stgin_gc2 > 0)
							mavg[mi].avg_life2 = (frecord->total_stgin_gc2 / 3600.0)
								/ frecord->num_periods_gc2;
						if (all_stageclrs && frecord->num_periods_uc2 > 0 && frecord->total_stgin_uc2
								 > 0)
							mavg[mi].avg_life2 += (frecord->total_stgin_uc2 / 3600.0)
								/ frecord->num_periods_uc2;
					} else {
						if (frecord->num_periods_gc > 0 && frecord->total_stgin_gc > 0)
							mavg[mi].avg_life = (frecord->total_stgin_gc / 3600.0)
								/ frecord->num_periods_gc;
						if (all_stageclrs && frecord->num_periods_uc > 0 && frecord->total_stgin_uc
								 > 0)
							mavg[mi].avg_life += (frecord->total_stgin_uc / 3600.0)
								/ frecord->num_periods_uc;
					}
				}
				mi++;
			}
			if (num_hrecs > 0 && frecord->t_or_d == 'h') {
				if (aflag) {
					haccs[hi].rec = frecord;
					strcpy (haccs[hi].poolname, frecord->poolname);
					haccs[hi].nbaccesses = frecord->num_accesses;
				} else if (tflag) {
					havg[hi].rec = frecord;
					strcpy (havg[hi].poolname, frecord->poolname);
					if (use_c_time) {
						if (frecord->num_periods_gc2 > 0 && frecord->total_stgin_gc2 > 0)
							havg[hi].avg_life2 = (frecord->total_stgin_gc2 / 3600.0)
								/ frecord->num_periods_gc2;
						if (all_stageclrs && frecord->num_periods_uc2 > 0 && frecord->total_stgin_uc2
								 > 0)
							havg[hi].avg_life2 += (frecord->total_stgin_uc2 / 3600.0)
								/ frecord->num_periods_uc2;
					} else {
						if (frecord->num_periods_gc > 0 && frecord->total_stgin_gc > 0)
							havg[hi].avg_life = (frecord->total_stgin_gc / 3600.0)
								/ frecord->num_periods_gc;
						if (all_stageclrs && frecord->num_periods_uc > 0 && frecord->total_stgin_uc
								 > 0)
							havg[hi].avg_life += (frecord->total_stgin_uc / 3600.0)
								/ frecord->num_periods_uc;
					}
				}
				hi++;
			}
			frecord = frecord->next;
		}
	}

	/* if sort criteria specified sort the arrays into ascending order */

	if  (aflag && num_frecs > 0) 
		qsort (faccs, num_frecs, sizeof (struct frec_nbaccs), comp);
	else if (tflag && num_frecs > 0) 
		qsort (favg, num_frecs, sizeof (struct frec_avg), comp2);

	if (aflag && num_drecs > 0) 
		qsort (daccs, num_drecs, sizeof (struct frec_nbaccs), comp);
	else if (tflag && num_drecs > 0) 
		qsort (davg, num_drecs, sizeof (struct frec_avg), comp2);

	if (aflag && num_mrecs > 0) 
		qsort (maccs, num_mrecs, sizeof (struct frec_nbaccs), comp);
	else if (tflag && num_mrecs > 0) 
		qsort (mavg, num_mrecs, sizeof (struct frec_avg), comp2);

	if (aflag && num_hrecs > 0) 
		qsort (haccs, num_hrecs, sizeof (struct frec_nbaccs), comp);
	else if (tflag && num_hrecs > 0) 
		qsort (havg, num_hrecs, sizeof (struct frec_avg), comp2);

	/* enter the pool information into the pool)inf records */

	sort_poolinf();	

	/* print out pool statistics */

	pf = pool_list;
	while (pf != NULL) {
		strcpy (current_pool, pf->poolname);
		if (pflag && strcmp (current_pool,poolname) != 0) continue;
		printf ("\f\nFile Request Details for Pool : %10s\n\n", current_pool);
		printf ("Number of requests started before time period began\t\t\t%d\n",
				pf->acc_clrd + pf->acc);
		printf ("Out of these:\tNumber accessed before being cleared\t\t\t%d\n",
				pf->acc_clrd);
		printf ("\t\tNumber accessed but not cleared during time period\t%d\n",
				pf->acc);
		printf ("\nNumber of requests started after beginning of time period\t\t%d\n",
				pf->stg_acc+pf->stg_acc_clrd);
		printf ("Out of these:\tNumber accessed but not cleared\t\t\t\t%d\n",
				pf->stg_acc);
		printf ("\t\tNumber accessed and then cleared\t\t\t%d\n", pf->stg_acc_clrd);

		if (pf->num_files > 0)
			printf ("\nAverage number of file accesses \t:\t %8.2f",
					pf->total_accesses/pf->num_files);
		if (use_c_time) {
			if (pf->total_avg_life2 > 0.0 && pf->total_stged_files2 > 0)
				printf ("\n Average lifetime of staged and then garbaged%s file using creation time \t:\t %8.2f hours\n",
						(all_stageclrs ? " or cleared_by_user" : ""),
						(float) (pf->total_avg_life2/pf->total_stged_files2));
		} else {
			if (pf->total_avg_life > 0.0 && pf->total_stged_files > 0)
				printf ("\n Average lifetime of a staged and then deleted file using last access time \t:\t %8.2f hours\n",
						(float) (pf->total_avg_life/pf->total_stged_files));
		}
		
		/* if sort criteria given print out file details */

		if (aflag && pf->num_frecs > 0) {
			printf ("\n\nLifetime and accesses per file sorted by number of ");
			printf ("accesses\n");
			printf ("\nTape VID     Fseq    Avg Life   Number    Login Name");
			printf ("     Group");
			printf ("\n                        (hrs)  Accesses");
			for (i = 0; i < num_frecs; i++){
				frecord = faccs[i].rec;
				if (strcmp (frecord->poolname, current_pool) == 0)
					print_fdetails (frecord);
			}
		}
		else if (tflag && pf->num_frecs > 0) {
			printf ("\n\nLifetime and accesses per file sorted by Average ");
			printf ("Lifetime\n");
			printf ("\nTape VID     Fseq    Avg Life   Number    Login Name ");
			printf ("    Group");
			printf ("\n                        (hrs)  Accesses");
			for (i = 0; i<num_frecs ; i++) {
				frecord = favg[i].rec;
				if ((strcmp (frecord->poolname, current_pool) == 0) &&
					(frecord->total_stgin_gc > 0 || frecord->total_stgin_uc > 0 || frecord->total_stgin_gc2 > 0 || frecord->total_stgin_uc2 > 0))
					print_fdetails (frecord);
			}
		}

		if (aflag && pf->num_drecs > 0) {
			printf ("\n\nLifetime and accesses per Disk File sorted by number of ");
			printf ("accesses");
			printf ("\n File Name                          Avg Life   Number");  
			printf ("        Login Name      Group ID ");
			printf ("\n                                       (hrs)  Accesses");
			for(i=0; i < num_drecs; i++) {
				frecord = daccs[i].rec;
				if (strcmp (frecord->poolname, current_pool) == 0)
					print_ddetails (frecord);
			}
		}
		else if (tflag && pf->num_drecs > 0) {
			printf ("\n\nLifetime and accesses per Disk File sorted by average");
			printf ("lifetime");
			printf ("\n File Name                          Avg Life   Number");        
			printf ("        Login Name      Group ID ");
			printf ("\n                                       (hrs)  Accesses");
			for(i=0; i< num_drecs; i++) {
				frecord = davg[i].rec;
				if ((strcmp (frecord->poolname, current_pool) == 0) &&
					(frecord->total_stgin_gc > 0 || frecord->total_stgin_uc > 0 || frecord->total_stgin_gc2 > 0 || frecord->total_stgin_uc2 > 0))
					print_ddetails (frecord);
			}
		}

		if (aflag && pf->num_mrecs > 0) {
			printf ("\n\nLifetime and accesses per non-CASTOR HSM File sorted by number of ");
			printf ("accesses");
			printf ("\n File Name                          Avg Life   Number");  
			printf ("        Login Name      Group ID ");
			printf ("\n                                       (hrs)  Accesses");
			for(i=0; i < num_mrecs; i++) {
				frecord = maccs[i].rec;
				if (strcmp (frecord->poolname, current_pool) == 0)
					print_mdetails (frecord);
			}
		}
		else if (tflag && pf->num_mrecs > 0) {
			printf ("\n\nLifetime and accesses per non-CASTOR HSM File sorted by average");
			printf ("lifetime");
			printf ("\n File Name                          Avg Life   Number");        
			printf ("        Login Name      Group ID ");
			printf ("\n                                       (hrs)  Accesses");
			for(i=0; i< num_mrecs; i++) {
				frecord = mavg[i].rec;
				if ((strcmp (frecord->poolname, current_pool) == 0) &&
					(frecord->total_stgin_gc > 0 || frecord->total_stgin_uc > 0 || frecord->total_stgin_gc2 > 0 || frecord->total_stgin_uc2 > 0))
					print_mdetails (frecord);
			}
		}

		if (aflag && pf->num_hrecs > 0) {
			printf ("\n\nLifetime and accesses per CASTOR HSM sorted by number of ");
			printf ("accesses");
			printf ("\n File Name                          Avg Life   Number");  
			printf ("        Login Name      Group ID ");
			printf ("\n                                       (hrs)  Accesses");
			for(i=0; i < num_hrecs; i++) {
				frecord = haccs[i].rec;
				if (strcmp (frecord->poolname, current_pool) == 0)
					print_hdetails (frecord);
			}
		}
		else if (tflag && pf->num_hrecs > 0) {
			printf ("\n\nLifetime and accesses per CASTOR HSM sorted by average");
			printf ("lifetime");
			printf ("\n File Name                          Avg Life   Number");        
			printf ("        Login Name      Group ID ");
			printf ("\n                                       (hrs)  Accesses");
			for(i=0; i< num_mrecs; i++) {
				frecord = mavg[i].rec;
				if ((strcmp (frecord->poolname, current_pool) == 0) &&
					(frecord->total_stgin_gc > 0 || frecord->total_stgin_uc > 0 || frecord->total_stgin_gc2 > 0 || frecord->total_stgin_uc2 > 0))
					print_mdetails (frecord);
			}
		}
		printf("\n"); 
		pf = pf->next;
	}
}

/* Functions to sort records into ascending order */

int comp (a, b)
	struct frec_nbaccs *a;
	struct frec_nbaccs *b;
{
	int c = 0;

	if ((c = strcmp(a->poolname, b->poolname)) != 0) return (c);
	else if (a->nbaccesses <  b->nbaccesses) return (1);
	else if (a->nbaccesses ==  b->nbaccesses) return (0);
	else return (-1); 
}

int comp2 (a, b)
	struct frec_avg  *a;
	struct frec_avg  *b;
{
	int c = 0;

	if (use_c_time) {
		if ((c = strcmp(a->poolname, b->poolname)) != 0) return (c);
		else if (a->avg_life2 < b->avg_life2) return (1);
		else if (a->avg_life2 == b->avg_life2) return (0);
		else return (-1);
	} else {
		if ((c = strcmp(a->poolname, b->poolname)) != 0) return (c);
		else if (a->avg_life < b->avg_life) return (1);
		else if (a->avg_life == b->avg_life) return (0);
		else return (-1);
	}
}

void swap_fields (rp)
	struct acctstage2 *rp;
{
	swap_it (rp->subtype);
	swap_it (rp->uid);
	swap_it (rp->gid);
	swap_it (rp->reqid);
	swap_it (rp->req_type);
	swap_it (rp->retryn);
	swap_it (rp->exitcode);
	swap_it (rp->u2.s.nbaccesses);
}

void usage (cmd)
	char *cmd;
{
	fprintf (stderr, "usage: %s ", cmd);
	fprintf (stderr, "%s",
			 "[-a][-c][-d][-e end_time][-f accounting_file][-h][-s start_time][-p pool_name][-S <a or t>][-v]\n"
			 "[-D diskfile][-H castorfile][-M noncastorfile][-T vid]\n"
			 "\n"
			 "      -a                 Use files deleted by user and admin for lifetime analysis - Default to admin (== garbage collector) only\n"
			 "where -c                 Use creation time for doing lifetime analysis\n"
			 "      -d                 Debug mode\n"
			 "      -e end_time        Limits analysis up to mmddhhmm[yy]\n"
			 "      -f accounting_file For using \"accounting_file\" (rfio syntax)\n"
			 "      -h accounting_file This help\n"
			 "      -s start_time      Limits analysis from mmddhhmm[yy]\n"
			 "      -p pool_name       Limits analysis to pool \"pool_name\"\n"
			 "      -S <a or t>        Sorting criteria (\"a\" per access number, \"t\" per lifetime)\n"
			 "      -v                 Verbose mode\n"
			 "where -D diskfile        is for searching and dumping accounting for DISK entries matching \"diskfile\"\n"
			 "      -H castorfile      is for searching and dumping accounting for CASTOR entries matching \"castorfile\"\n"
			 "      -M hsmfile         is for searching and dumping accounting for HSM and NON-CASTOR entries matching \"hsmfile\"\n"
			 "      -T vid             is for searching and dumping accounting for TAPE entries matching \"vid\"\n"
		);
}


void print_acct(accthdr,rp)
	struct accthdr *accthdr;
	struct acctstage2 *rp;
{
	char timestr[64] ;        /* Time in its ASCII format */
	char timestr2[64] ;        /* Time in its ASCII format */
	struct passwd *pwd;				/* password structure */
	struct group *grp;				/* group structure */
	struct passwd pwd_unknown;
	struct group grp_unknown;
	char pw_name_unknown[21];
	char gr_name_unknown[21];
	char tmpbuf[21];
	char tmpbuf2[21];
	
	pwd_unknown.pw_name = pw_name_unknown;
	grp_unknown.gr_name = gr_name_unknown;
	stage_util_time(accthdr->timestamp,timestr);
	pwd = Cgetpwuid (rp->uid);
	if (pwd == NULL) {
		if (rp->uid >= INT_MAX) {
			sprintf(pwd_unknown.pw_name,"<undef>");
		} else {
			sprintf(pwd_unknown.pw_name,"%d",rp->uid);
		}
		pwd = &pwd_unknown;
	}
	grp = Cgetgrgid (rp->gid);
	if (grp == NULL) {
		if (rp->gid >= INT_MAX) {
			sprintf(grp_unknown.gr_name,"<undef>");
		} else {
			sprintf(grp_unknown.gr_name,"%d",rp->gid);
		}
		grp = &grp_unknown;
	}
 	u64tostru((u_signed64) rp->u2.s.actual_size, tmpbuf, 0);
 	u64tostr((u_signed64) rp->u2.s.c_time, tmpbuf2, 0);
	stage_util_time(rp->u2.s.c_time,timestr2);
	
	fprintf(stderr,"... -------------------\n");
	fprintf(stderr,"... Timestamp  : %s\n", timestr);
	fprintf(stderr,"... Package    : %d\n", accthdr->package);
	fprintf(stderr,"... Length     : %d bytes\n", accthdr->len);
	fprintf(stderr,"... Subtype    : %d (%s)\n", rp->subtype, subtype_2_char(rp->subtype));
	fprintf(stderr,"... Uid        : %d (%s)\n", rp->uid, pwd->pw_name);
	fprintf(stderr,"... Gid        : %d (%s)\n", rp->gid, grp->gr_name);
	fprintf(stderr,"... Reqid      : %d\n", rp->reqid);
	fprintf(stderr,"... Reqtype    : %d (%s)\n", rp->req_type, req_type_2_char(rp->req_type));
	fprintf(stderr,"... Retryn     : %d\n", rp->retryn);
	fprintf(stderr,"... Exitcode   : %d\n", rp->exitcode);
	fprintf(stderr,"... Clienthost : %s\n", rp->u2.clienthost);

	if (rp->u2.s.t_or_d != 't' &&
		rp->u2.s.t_or_d != 'd' &&
		rp->u2.s.t_or_d != 'm' &&
		rp->u2.s.t_or_d != 'h'
		) return;

	fprintf(stderr,"... Poolname   : %s\n", rp->u2.s.poolname);
	fprintf(stderr,"... Actualsize : %s\n", tmpbuf);
	fprintf(stderr,"... Creat time : %s (%s)\n", tmpbuf2, timestr2);
	fprintf(stderr,"... Nbaccesses : %d\n", rp->u2.s.nbaccesses);
	fprintf(stderr,"... t_or_d     : %c\n", rp->u2.s.t_or_d);
	switch (rp->u2.s.t_or_d) {
	case 't':
		fprintf(stderr,"... Side       : %d\n", rp->u2.s.u1.t.side);
		fprintf(stderr,"... Dgn        : %s\n", rp->u2.s.u1.t.dgn);
		fprintf(stderr,"... Fseq       : %s\n", rp->u2.s.u1.t.fseq);
		fprintf(stderr,"... Vid        : %s\n", rp->u2.s.u1.t.vid);
		fprintf(stderr,"... Tapesrvr   : %s\n", rp->u2.s.u1.t.tapesrvr);
		break;
	case 'd':
		fprintf(stderr,"... Xfile      : %s\n", rp->u2.s.u1.d.xfile);
		break;
	case 'm':
		fprintf(stderr,"... Xfile      : %s\n", rp->u2.s.u1.m.xfile);
		break;
	case 'h':
		fprintf(stderr,"... Xfile      : %s\n", rp->u2.s.u1.h.xfile);
		u64tostr((u_signed64) rp->u2.s.u1.h.fileid, tmpbuf, 0);
		fprintf(stderr,"... Fileid     : %s\n", tmpbuf);
		break;
	}
}

void show_progress() {
	int done;
	static char last_slash = '\\';
	char bar[41];
	int i;
	static int last_done = 0;

	if (size_total == 0) {
		done = 4000;
	} else if (size_read >= size_total) {
		done = 4000;
	} else {
		done = (4000 * size_read) / size_total;
	}
	if (done != last_done) {
		for (i = 0; i < 40; i++) {
			if (i <= (done/100)) {
				bar[i] = '=';
			} else {
				bar[i] = ' ';
			}
		}
		bar[40] = '\0';
		/* We print the progress */
		switch (last_slash) {
		case '\\':
			last_slash = '|';
			break;
		case '|':
			last_slash = '/';
			break;
		case '/':
			last_slash = '-';
			break;
		case '-':
			last_slash = '\\';
			break;
		}
		if (((done/100) != (last_done/100)) || (last_done == 0)) {
			/* The '=' do change */
			fprintf(stdout,"\r|%40s| (%7.2f %%) %c", bar, (float) (100. * (signed64) size_read / (signed64) size_total), last_slash);
			fflush(stdout);
		} else {
			/* The '=' do not change */
			fprintf(stdout,"\b\b\b\b\b\b\b\b\b\b\b\b%7.2f %%) %c", (float) (100. * (signed64) size_read / (signed64) size_total), last_slash);
			fflush(stdout);
		}
		last_done = done;
	}
}

char *subtype_2_char(subtype)
	int subtype;
{
	static char *unknown_subtype = "Unknown";
	static char *STGSTART_subtype = "Stager daemon started";
	static char *STGCMDR_subtype = "Command received";
	static char *STGFILS_subtype = "File staged";
	static char *STGCMDC_subtype = "Command finished";
	static char *STGCMDS_subtype = "Stager started";
	static char *STGFILC_subtype = "File cleared";

	switch (subtype) {
	case STGSTART:
		return(STGSTART_subtype);
	case STGCMDR:
		return(STGCMDR_subtype);
	case STGFILS:
		return(STGFILS_subtype);
	case STGCMDC:
		return(STGCMDC_subtype);
	case STGCMDS:
		return(STGCMDS_subtype);
	case STGFILC:
		return(STGFILC_subtype);
	default:
		return(unknown_subtype);
	}
}

char *req_type_2_char(req_type)
	int req_type;
{
	static char *unknown_req_type = "Unknown";
	static char *STAGEIN_req_type = "STAGEIN";
	static char *STAGEOUT_req_type = "STAGEOUT";
	static char *STAGEWRT_req_type = "STAGEWRT";
	static char *STAGEPUT_req_type = "STAGEPUT";
	static char *STAGEQRY_req_type = "STAGEQRY";
	static char *STAGECLR_req_type = "STAGECLR";
	static char *STAGEKILL_req_type = "STAGEKILL";
	static char *STAGEUPDC_req_type = "STAGEUPDC";
	static char *STAGEINIT_req_type = "STAGEINIT";
	static char *STAGECAT_req_type = "STAGECAT";
	static char *STAGEALLOC_req_type = "STAGEALLOC";
	static char *STAGEGET_req_type = "STAGEGET";
	static char *STAGEMIGPOOL_req_type = "STAGEMIGPOOL";
	static char *STAGEFILCHG_req_type = "STAGEFILCHG";
	static char *STAGESHUTDOWN_req_type = "STAGESHUTDOWN";
	static char *STAGEPING_req_type = "STAGEPING";
    static char *STAGE_IN_req_type = "STAGE_IN";
    static char *STAGE_OUT_req_type = "STAGE_OUT";
    static char *STAGE_WRT_req_type = "STAGE_WRT";
    static char *STAGE_PUT_req_type = "STAGE_PUT";
    static char *STAGE_QRY_req_type = "STAGE_QRY";
    static char *STAGE_CLR_req_type = "STAGE_CLR";
    static char *STAGE_KILL_req_type = "STAGE_KILL";
    static char *STAGE_UPDC_req_type = "STAGE_UPDC";
    static char *STAGE_INIT_req_type = "STAGE_INIT";
    static char *STAGE_CAT_req_type = "STAGE_CAT";
    static char *STAGE_ALLOC_req_type = "STAGE_ALLOC";
    static char *STAGE_GET_req_type = "STAGE_GET";
    static char *STAGE_FILCHG_req_type = "STAGE_FILCHG";
    static char *STAGE_SHUTDOWN_req_type = "STAGE_SHUTDOWN";
    static char *STAGE_PING_req_type = "STAGE_PING";

	switch (req_type) {
	case STAGEIN:
		return(STAGEIN_req_type);
		break;
	case STAGEOUT:
		return(STAGEOUT_req_type);
		break;
	case STAGEWRT:
		return(STAGEWRT_req_type);
		break;
	case STAGEPUT:
		return(STAGEPUT_req_type);
		break;
	case STAGEQRY:
		return(STAGEQRY_req_type);
		break;
	case STAGECLR:
		return(STAGECLR_req_type);
		break;
	case STAGEKILL:
		return(STAGEKILL_req_type);
		break;
	case STAGEUPDC:
		return(STAGEUPDC_req_type);
		break;
	case STAGEINIT:
		return(STAGEINIT_req_type);
		break;
	case STAGECAT:
		return(STAGECAT_req_type);
		break;
	case STAGEALLOC:
		return(STAGEALLOC_req_type);
		break;
	case STAGEGET:
		return(STAGEGET_req_type);
		break;
	case STAGEMIGPOOL:
		return(STAGEMIGPOOL_req_type);
		break;
	case STAGEFILCHG:
		return(STAGEFILCHG_req_type);
		break;
	case STAGESHUTDOWN:
		return(STAGESHUTDOWN_req_type);
		break;
	case STAGEPING:
		return(STAGEPING_req_type);
		break;
    case STAGE_IN:
		return(STAGE_IN_req_type);
		break;
    case STAGE_OUT:
		return(STAGE_OUT_req_type);
		break;
    case STAGE_WRT:
		return(STAGE_WRT_req_type);
		break;
    case STAGE_PUT:
		return(STAGE_PUT_req_type);
		break;
    case STAGE_QRY:
		return(STAGE_QRY_req_type);
		break;
    case STAGE_CLR:
		return(STAGE_CLR_req_type);
		break;
    case STAGE_KILL:
		return(STAGE_KILL_req_type);
		break;
    case STAGE_UPDC:
		return(STAGE_UPDC_req_type);
		break;
    case STAGE_INIT:
		return(STAGE_INIT_req_type);
		break;
    case STAGE_CAT:
		return(STAGE_CAT_req_type);
		break;
    case STAGE_ALLOC:
		return(STAGE_ALLOC_req_type);
		break;
    case STAGE_GET:
		return(STAGE_GET_req_type);
		break;
    case STAGE_FILCHG:
		return(STAGE_FILCHG_req_type);
		break;
    case STAGE_SHUTDOWN:
		return(STAGE_SHUTDOWN_req_type);
		break;
    case STAGE_PING:
		return(STAGE_PING_req_type);
		break;
	default:
		return(unknown_req_type);
	}
}

/*
 * $Id: procping.c,v 1.15 2002/10/19 14:33:53 jdurand Exp $
 */

/*
 * Copyright (C) 2001 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: procping.c,v $ $Revision: 1.15 $ $Date: 2002/10/19 14:33:53 $ CERN IT-PDP/DM Jean-Damien Durand";
#endif /* not lint */

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include "osdep.h"
#include "stage_constants.h"
#include "stage_messages.h"
#include "Cgrp.h"
#include "Castor_limits.h"
#include "stage_api.h"
#include "Cgetopt.h"
#include "marshall.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/resource.h> /* For getrlimit()/setrlimit() */
#endif
#define local_unmarshall_STRING(ptr,str)  { str = ptr ; INC_PTR(ptr,strlen(str)+1) ; }
#if SACCT
#include "../h/sacct.h"
#endif
#include "patchlevel.h"
#define __BASEVERSION__ "?"
#define __PATCHLEVEL__ 0

void procpingreq _PROTO((int, int, char *, char *));
extern void stageacct _PROTO((int, uid_t, gid_t, char *, int, int, int, int, struct stgcat_entry *, char *, char));
extern int stglogit _PROTO(());
extern char *stglogflags _PROTO((char *, char *, u_signed64));
extern int req2argv _PROTO((char *, char ***));
#if (defined(IRIX64) || defined(IRIX5) || defined(IRIX6))
extern int sendrep _PROTO((int *, int, ...));
#else
extern int sendrep _PROTO(());
#endif

extern char func[16];
extern int rpfd;
extern int reqid;

static int noretry_flag = 0;

void procpingreq(req_type, magic, req_data, clienthost)
		 int req_type;
		 int magic;
		 char *req_data;
		 char *clienthost;
{
	int c;
	int rc = 0;
	char *rbp;
	char *user;
	gid_t gid;
	struct group *gr;
	extern char *localhost; /* Fully qualified hostname */
	extern time_t started_time;
	char timestr[64] ;   /* Time in its ASCII format             */
	static struct Coptions longopts[] =
	{
		{"host",               REQUIRED_ARGUMENT,  NULL,      'h'},
		{"verbose",            NO_ARGUMENT,        NULL,      'v'},
		{"noretry",            NO_ARGUMENT,     &noretry_flag,  1},
		{NULL,                 0,                  NULL,        0}
	};
	u_signed64 flags = 0;
	int nargs = 0;
	int errflg = 0;
	char **argv = NULL;
#if defined(HPSSCLIENT)
	char hpss_aware = 'H';
#else
	char hpss_aware = ' ';
#endif
	int verbose_flag = 0;
	int api_out = 0;
	
	rbp = req_data;
	local_unmarshall_STRING (rbp, user);	/* login name */
	if (req_type > STAGE_00) {
		unmarshall_LONG (rbp, gid);
		unmarshall_HYPER (rbp, flags);
		stglogit (func, "STG92 - %s request by %s (,%d) from %s\n", "stage_ping", user, gid, clienthost);
    } else {
		unmarshall_WORD (rbp, gid);
		stglogit (func, "STG92 - %s request by %s (,%d) from %s\n", "stageping", user, gid, clienthost);
		nargs = req2argv (rbp, &argv);
	}
#if SACCT
	stageacct (STGCMDR, -1, gid, clienthost,
						 reqid, req_type, 0, 0, NULL, "", (char) 0);
#endif
	
	if (req_type > STAGE_00) {
		api_out = 1;
	}
	
	if ((gr = Cgetgrgid (gid)) == NULL) {
		if (errno != ENOENT) sendrep (&rpfd, MSG_ERR, STG33, "Cgetgrgid", strerror(errno));
		sendrep (&rpfd, MSG_ERR, STG36, gid);
		c = (api_out != 0) ? ESTGROUP : SESYSERR;
		goto reply;
	}

	if (req_type > STAGE_00) {
		if ((flags & STAGE_VERBOSE) == STAGE_VERBOSE) {
			verbose_flag++;
		}
		/* Print the flags */
		stglogflags("stage_ping",LOGFILE,(u_signed64) flags);
	} else {
		Coptind = 1;
		Copterr = 0;
		while ((c = Cgetopt_long (nargs, argv, "h:v", longopts, NULL)) != -1) {
			switch (c) {
			case 'h':
				break;
			case 'v':
				verbose_flag++;
				break;
			case 0:
				/* These are the long options */
				break;
			default:
				errflg++;
				break;
			}
			if (errflg != 0) break;
		}
	}

	if (errflg != 0) {
		rc = EINVAL;
		goto reply;
	}

	/* We send util information */
	/* The list of object linked to produce stgdaemon is: */
	/* stgdaemon.o packfseq.o poolmgr.o procalloc.o procclr.o  stgdb_Cdb_ifce.o Cstage_db.o Cstage_ifce.o  procio.o procping.o procqry.o procupd.o procfilchg.o sendrep.o stglogit.o stageacct.o */

	if (verbose_flag) {
		extern int nwaitq_with_connection;
		extern int nbhost;
		struct rlimit rlim;
#define RESERVED_FD (4 + nwaitq_with_connection + 2 * nbhost)
#define FREE_FD (sysconf(_SC_OPEN_MAX) - RESERVED_FD)

		stage_util_time(started_time,timestr);
		sendrep (&rpfd, MSG_OUT, "Stager daemon on %s - CASTOR %s.%d%c\n", localhost, BASEVERSION, PATCHLEVEL, hpss_aware);
		sendrep (&rpfd, MSG_OUT, "Generated %s around %s\n", __DATE__, __TIME__);
		sendrep (&rpfd, MSG_OUT, "Running since %s, pid=%d\n", timestr, (int) getpid());
		sendrep (&rpfd, MSG_OUT, "Maximum/reserved/available number of opened file descriptors: %d/%d/%d\n", sysconf(_SC_OPEN_MAX), RESERVED_FD, FREE_FD);
#ifdef RLIMIT_NPROC
		if (getrlimit(RLIMIT_NPROC,&rlim) != 0) {
			sendrep (&rpfd, MSG_OUT, "Maximum number of processes: ... getrlimit(RLIMIT_NPROC,&rlim) error : %s\n", strerror(errno));
		} else {
 			sendrep (&rpfd, MSG_OUT, "Maximum number of processes: { cur=%d, max=%d }\n", rlim.rlim_cur, rlim.rlim_max);
		}
#else
		sendrep (&rpfd, MSG_OUT, "... RLIMIT_NPROC undefined on platform where runs stager daemon\n");
#endif
	}
	reply:
	if (argv != NULL) free (argv);
	sendrep (&rpfd, STAGERC, STAGEPING, magic, rc);
	return;
}

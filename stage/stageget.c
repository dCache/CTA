/*
 * $Id: stageget.c,v 1.15 2001/02/01 18:09:29 jdurand Exp $
 */

/*
 * Copyright (C) 1996-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: stageget.c,v $ $Revision: 1.15 $ $Date: 2001/02/01 18:09:29 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif
#include "marshall.h"
#include "stage.h"
#include "stage_api.h"
#include "Cpwd.h"
#include "Cgetopt.h"

EXTERN_C int  DLL_DECL  send2stgd_cmd _PROTO((char *, char *, int, int, char *, int));  /* Command-line version */
extern	char	*getenv();
extern	char	*getconfent();
static gid_t gid;
static struct passwd *pw;
char *stghost;

void cleanup _PROTO((int));
void usage _PROTO((char *));

int main(argc, argv)
		 int	argc;
		 char	**argv;
{
	int c, i;
	char *dp;
	int errflg = 0;
	int fun = 0;
	int msglen;
	int nargs;
	int ntries = 0;
	char *q;
	char path[CA_MAXHOSTNAMELEN + 1 + MAXPATH];
	char *pool_user = NULL;
	char *sbp;
	char sendbuf[REQBUFSZ];
	int uflag = 0;
	uid_t uid;
	/* char repbuf[CA_MAXPATHLEN+1]; */

	nargs = argc;
	uid = getuid();
	gid = getgid();
	Coptind = 1;
	Copterr = 1;
	while ((c = Cgetopt (argc, argv, "h:Pp:U:u:")) != -1) {
		switch (c) {
		case 'h':
			stghost = Coptarg;
			break;
		case 'U':
			fun = strtol (Coptarg, &dp, 10);
			if (*dp != '\0') {
				fprintf (stderr, STG06, "-U\n");
				errflg++;
			}
			break;
		case 'u':
			uflag++;
			break;
		case '?':
			errflg++;
			break;
		}
	}
	if (Coptind >= argc && fun == 0) {
		fprintf (stderr, STG07);
		errflg++;
	}
	if (errflg) {
		usage (argv[0]);
		exit (1);
	}

	/* default parameters will be added to the argv list */

	if (fun)
		nargs++;
	if (uflag == 0 &&
			(pool_user = getenv ("STAGE_USER")) != NULL)
		nargs += 2;

	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, STGMAGIC);
	marshall_LONG (sbp, STAGEGET);
	q = sbp;	/* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);

	/* Build request body */

	pw = Cgetpwuid (uid);
	marshall_STRING (sbp, pw->pw_name);	/* login name */
	marshall_WORD (sbp, uid);
	marshall_WORD (sbp, gid);

	marshall_WORD (sbp, nargs);
	for (i = 0; i < Coptind; i++)
		marshall_STRING (sbp, argv[i]);
	if (uflag == 0 && pool_user) {
		marshall_STRING (sbp, "-u");
		marshall_STRING (sbp, pool_user);
	}
	if ((c = build_linkname (argv[i], path, sizeof(path), STAGEGET)) == SYERR)
		exit (SYERR);
	if (c) {
		errflg++;
	} else {
		if (sbp + strlen (path) - sendbuf >= sizeof(sendbuf)) {
			fprintf (stderr, STG38);
			errflg++;
		}
		marshall_STRING (sbp, path);
	}
	if (fun) {
		if (build_Upath (fun, path, sizeof(path), STAGEGET))
			exit (SYERR);
		if (sbp + strlen (path) - sendbuf >= sizeof(sendbuf)) {
			fprintf (stderr, STG38);
			errflg++;
		} else
			marshall_STRING (sbp, path);
	}
	if (errflg) {
		usage (argv[0]);
		exit (1);
	}

	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */

#ifndef _WIN32
	signal (SIGHUP, cleanup);
#endif
	signal (SIGINT, cleanup);
#ifndef _WIN32
	signal (SIGQUIT, cleanup);
#endif
	signal (SIGTERM, cleanup);

	while (1) {
		c = send2stgd_cmd (stghost, sendbuf, msglen, 1, NULL, 0);
		if (c == 0 || serrno == EINVAL) break;
		if (serrno != ESTNACT && ntries++ > MAXRETRY) break;
		sleep (RETRYI);
	}
	exit (c == 0 ? 0 : serrno);
}

void cleanup(sig)
		 int sig;
{
	signal (sig, SIG_IGN);

	exit (USERR);
}

void usage(cmd)
		 char *cmd;
{
	fprintf (stderr, "usage: %s ", cmd);
	fprintf (stderr, "%s",
					 "[-h stage_host] [-P] [-p pool] [-U fun] [-u user] pathname\n");
}

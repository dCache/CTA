/*
 * $Id: stageclr.c,v 1.6 1999/12/09 13:47:33 jdurand Exp $
 */

/*
 * Copyright (C) 1993-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: stageclr.c,v $ $Revision: 1.6 $ $Date: 1999/12/09 13:47:33 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "marshall.h"
#include "stage.h"
extern	char	*getconfent();
extern	char	*optarg;
extern	int	optind;

main(argc, argv)
int	argc;
char	**argv;
{
	int c, i, n;
	void cleanup();
	int errflg = 0;
	int Gflag = 0;
	uid_t Guid;
	gid_t gid;
	struct group *gr;
	int Iflag = 0;
	char ibuf[MAXHOSTNAMELEN + MAXPATH];
	int iflag = 0;
	int Lflag = 0;
        int Mflag = 0;
	int msglen;
	int ntries = 0;
	int numvid;
	int Pflag = 0;
	char *p;
	char path[MAXHOSTNAMELEN + MAXPATH];
	struct passwd *pw;
	char *q;
	char *sbp;
	char *sbpp;
	char sendbuf[REQBUFSZ];
	char *stghost = NULL;
	uid_t uid;
	char vid[MAXVSN][7];
#if defined(_WIN32)
	WSADATA wsadata;
#endif

	uid = getuid();
	gid = getgid();
#if defined(_WIN32)
	if (uid < 0 || gid < 0) {
		fprintf (stderr, STG52);
		exit (USERR);
	}
#endif
	numvid = 0;
#if defined(_WIN32)
	if (WSAStartup (MAKEWORD (2, 0), &wsadata)) {
		fprintf (stderr, STG51);
		exit (SYERR);
	}
#endif
	while ((c = getopt (argc, argv, "cGh:I:iL:l:M:m:P:p:q:r:V:")) != EOF) {
		switch (c) {
		case 'G':
			Gflag++;
			if ((gr = getgrgid (gid)) == NULL) {
				fprintf (stderr, STG36, gid);
#if defined(_WIN32)
				WSACleanup();
#endif
				exit (SYERR);
			}
#if !defined(vms)
			if ((p = getconfent ("GRPUSER", gr->gr_name, 0)) == NULL) {
				fprintf (stderr, STG10, gr->gr_name);
				errflg++;
			} else {
				if ((pw = getpwnam (p)) == NULL) {
					fprintf (stderr, STG11, p);
					errflg++;
				} else
					Guid = pw->pw_uid;
			}
#else
			if ((q = getconfent ("GRPUSER", gr->gr_name, 1)) == NULL) {
				fprintf (stderr, STG10, gr->gr_name);
				errflg++;
			} else {
				if ((p = strtok (q, " \n")) == NULL) {
					fprintf (stderr, STG80, q);
					errflg++;
				}
				if ((p = strtok (NULL, " \n")) == NULL) {
					fprintf (stderr, STG81, q);
					errflg++;
				}
				Guid = atoi (p);
				if ((p = strtok (NULL, " \n")) == NULL) {
					fprintf (stderr, STG82, q);
					errflg++;
				}
				gid = atoi (p);
			}
#endif
			break;
		case 'h':
			stghost = optarg;
			break;
		case 'I':
			Iflag++;
			break;
		case 'i':
			iflag++;
			break;
		case 'L':
			Lflag = optind - 1;
			if (n = optarg - argv[Lflag])
				strncpy (path, argv[Lflag], n);
			if ((c = build_linkname (optarg, path+n, sizeof(path)-n, STAGECLR)) == SYERR) {
#if defined(_WIN32)
				WSACleanup();
#endif
				exit (SYERR);
			} else if (c)
				errflg++;
			break;
		case 'M':
			Mflag++;
			break;
		case 'P':
			Pflag = optind - 1;
			if (n = optarg - argv[Pflag])
				strncpy (path, argv[Pflag], n);
			if ((c = build_linkname (optarg, path+n, sizeof(path)-n, STAGECLR)) == SYERR) {
#if defined(_WIN32)
				WSACleanup();
#endif
				exit (SYERR);
			} else if (c)
				errflg++;
			break;
		case 'r':
			if (strcmp (optarg, "emove_from_hsm") != 0)
				errflg++;
			break;
		case 'V':
			errflg += getlist_of_vid ("-V", vid, &numvid);
			break;
		case '?':
			errflg++;
			break;
		default:
			break;
		}
	}
	if (argc > optind) {
		fprintf (stderr, STG16);
		errflg++;
	}
	if (numvid == 0 && Iflag == 0 && iflag == 0 && Lflag == 0 && Pflag == 0 && Mflag == 0) {
		fprintf (stderr, STG46);
		errflg++;
	}
	if ((iflag != 0) + (Lflag != 0) + (Pflag != 0) + (Mflag != 0) > 1) {
		fprintf (stderr, STG35, "-i, -L, -M", "-P");
		errflg++;
	}

#if defined(vms)
	if (!Gflag) {
		fprintf (stderr, STG83);
		errflg++;
	}
#endif
	if (errflg) {
		usage (argv[0]);
#if defined(_WIN32)
		WSACleanup();
#endif
		exit (1);
	}

	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, STGMAGIC);
	marshall_LONG (sbp, STAGECLR);
	q = sbp;	/* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);

	/* Build request body */

#if !defined(vms)
	if ((pw = getpwuid (uid)) == NULL) {
		char uidstr[8];
		sprintf (uidstr, "%d", uid);
		p = uidstr;
#else
	if ((pw = getpwnam (p = cuserid(NULL))) == NULL) {
#endif
		fprintf (stderr, STG11, p);
#if defined(_WIN32)
		WSACleanup();
#endif
		exit (SYERR);
	}
	marshall_STRING (sbp, pw->pw_name);	/* login name */
	if (Gflag) {
		marshall_WORD (sbp, Guid);
	} else {
		marshall_WORD (sbp, uid);
	}
	marshall_WORD (sbp, gid);

#if ! defined(_WIN32)
	signal (SIGHUP, cleanup);
#endif
	signal (SIGINT, cleanup);
#if ! defined(_WIN32)
	signal (SIGQUIT, cleanup);
#endif
	signal (SIGTERM, cleanup);

	if (iflag) {
		int rc = 0;
		marshall_WORD (sbp, argc+2);
		for (i = 0; i < argc; i++)
			marshall_STRING (sbp, argv[i]);
		marshall_STRING (sbp, "-P");
		sbpp = sbp;
		while (fgets (ibuf, sizeof(ibuf), stdin) != NULL) {
			if (p = strchr (ibuf, '\n')) *p = '\0';
			if ((c = build_linkname (ibuf, path, sizeof(path), STAGECLR)) == SYERR) {
#if defined(_WIN32)
				WSACleanup();
#endif
				exit (SYERR);
			} else if (c) {
				if (! rc) rc = USERR;
				continue;
			}
			if (sbp + strlen (path) - sendbuf >= sizeof(sendbuf)) {
				fprintf (stderr, STG38);
				if (! rc) rc = USERR;
				continue;
			} else
				marshall_STRING (sbp, path);

			msglen = sbp - sendbuf;
			sbp = q;
			marshall_LONG (sbp, msglen);	/* update length field */

			while (c = send2stgd (stghost, sendbuf, msglen, 1)) {
				if (c == 0 || c == USERR || c == EBUSY) break;
				if (c == ENOUGHF) break;
				if (c != ESTNACT && ntries++ > MAXRETRY) break;
				sleep (RETRYI);
			}
			if (c == ENOUGHF) break;
			if (c && c != EBUSY && ! rc) rc = c;
			sbp = sbpp;
		}
#if defined(_WIN32)
		WSACleanup();
#endif
		exit (rc);
	} else {
		marshall_WORD (sbp, argc);
		for (i = 0; i < argc; i++)
			if ((Pflag && i == Pflag) || (Lflag && i == Lflag)) {
				marshall_STRING (sbp, path);
			} else {
				marshall_STRING (sbp, argv[i]);
			}

		msglen = sbp - sendbuf;
		marshall_LONG (q, msglen);	/* update length field */

		while (c = send2stgd (stghost, sendbuf, msglen, 1)) {
			if (c == 0 || c == USERR) break;
			if (c == ENOUGHF) {
				c = 0;
				break;
			}
			if (c != ESTNACT && ntries++ > MAXRETRY) break;
			sleep (RETRYI);
		}
#if defined(_WIN32)
		WSACleanup();
#endif
		exit (c);
	}
}

void cleanup(sig)
int sig;
{
	signal (sig, SIG_IGN);

#if defined(_WIN32)
	WSACleanup();
#endif
	exit (USERR);
}

usage(cmd)
char *cmd;
{
	fprintf (stderr, "usage: %s ", cmd);
	fprintf (stderr, "%s%s%s",
	  "[-c] [-h stage_host] [-G] [-I external_filename] [-i] [-L link]\n",
	  "[-l label_type] [-M hsmfile] [-m minfree] [-P path] [-p pool]\n",
          "[-q file_sequence_number(s)] [-remove_from_hsm] [-V visual_identifier(s)]\n");
}

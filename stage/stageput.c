/*
 * $Id: stageput.c,v 1.33 2002/04/30 13:08:35 jdurand Exp $
 */

/*
 * Copyright (C) 1993-2000 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: stageput.c,v $ $Revision: 1.33 $ $Date: 2002/04/30 13:08:35 $ CERN IT-PDP/DM Jean-Philippe Baud Jean-Damien Durand";
#endif /* not lint */

#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#endif
#include "marshall.h"
#include "stage_api.h"
#include "Cpwd.h"
#include "Cgrp.h"
#include "Cgetopt.h"
#include "serrno.h"

EXTERN_C int  DLL_DECL  send2stgd_cmd _PROTO((char *, char *, int, int, char *, int));  /* Command-line version */
extern	char	*getenv();
extern	char	*getconfent();
#if !defined(linux)
extern	char	*sys_errlist[];
#endif
static gid_t gid;
static int pid;
static struct passwd *pw;
char *stghost;
int tppool_flag = 0;
int nowait_flag = 0;
int noretry_flag = 0;

void usage _PROTO((char *));
void cleanup _PROTO((int));
void freehsmfiles _PROTO((int, char **));

int main(argc, argv)
		 int	argc;
		 char	**argv;
{
	int c, i;
	char *dp;
	int errflg = 0;
	int fun = 0;
	int Gflag = 0;
	char Gname[15];
	uid_t Guid;
	struct group *gr;
	char *hsm_host;
	char hsm_path[CA_MAXHOSTNAMELEN + 1 + MAXPATH];
	int Iflag = 0;
	int Mflag = 0;
	int msglen;
	int nargs;
	int ntries = 0;
	int nstg161 = 0;
	int numvid;
	char *p, *q;
	char path[CA_MAXHOSTNAMELEN + 1 + MAXPATH];
	char *sbp;
	char sendbuf[REQBUFSZ];
	uid_t uid;
	char **hsmfiles = NULL;
	int nhsmfiles = 0;
	int stagemig = 0;
#if defined(_WIN32)
	WSADATA wsadata;
#endif
	char *tppool = NULL;
	int maxretry = MAXRETRY;
	static struct Coptions longopts[] =
	{
		{"grpuser",            NO_ARGUMENT,        NULL,      'G'},
		{"host",               REQUIRED_ARGUMENT,  NULL,      'h'},
		{"external_filename",  REQUIRED_ARGUMENT,  NULL,      'I'},
		{"migration_filename", REQUIRED_ARGUMENT,  NULL,      'M'},
		{"file_sequence",      REQUIRED_ARGUMENT,  NULL,      'q'},
		{"fortran_unit",       REQUIRED_ARGUMENT,  NULL,      'U'},
		{"vid",                REQUIRED_ARGUMENT,  NULL,      'V'},
		{"nowait",             NO_ARGUMENT,  &nowait_flag,      1},
		{"noretry",            NO_ARGUMENT,  &noretry_flag,      1},
		{"tppool",             REQUIRED_ARGUMENT, &tppool_flag, 1},
		{NULL,                 0,                  NULL,        0}
	};

	nargs = argc;
	uid = Guid = getuid();
	gid = getgid();
#if defined(_WIN32)
	if (uid < 0 || gid < 0) {
		fprintf (stderr, STG52);
		exit (USERR);
	}
#endif
	numvid = 0;
	Coptind = 1;
	Copterr = 1;
	while ((c = Cgetopt_long (argc, argv, "Gh:I:M:q:U:V:", longopts, NULL)) != -1) {
		switch (c) {
		case 'G':
			Gflag++;
			if ((gr = Cgetgrgid (gid)) == NULL) {
				if (errno != ENOENT) fprintf (stderr, STG33, "Cgetgrgid", strerror(errno));
				fprintf (stderr, STG36, gid);
				exit (SYERR);
			}
			if ((p = getconfent ("GRPUSER", gr->gr_name, 0)) == NULL) {
				fprintf (stderr, STG10, gr->gr_name);
				errflg++;
			} else {
				strcpy (Gname, p);
				if ((pw = Cgetpwnam (p)) == NULL) {
					if (errno != ENOENT) fprintf (stderr, STG33, "Cgetpwnam", strerror(errno));
					fprintf (stderr, STG11, p);
					errflg++;
				} else
					Guid = pw->pw_uid;
			}
			break;
		case 'h':
			stghost = Coptarg;
			break;
		case 'I':
			Iflag = 1;
			break;
		case 'M':
			Mflag = 1;
			if (stagemig == nhsmfiles) {
				if (nhsmfiles == 0) {
					if ((hsmfiles = (char **) malloc(sizeof(char *))) == NULL) {
						fprintf(stderr,"malloc error (%s)\n",strerror(errno));
						errflg++;
					} else {
						stagemig++;
						hsmfiles[0] = NULL;
					}
				} else {
					char **dummy;
					if ((dummy = (char **) realloc(hsmfiles,(nhsmfiles+1) * sizeof(char *))) == NULL) {
						fprintf(stderr,"realloc error (%s)\n",strerror(errno));
						errflg++;
					} else {
						hsmfiles = dummy;
						stagemig++;
						hsmfiles[nhsmfiles] = NULL;
					}
				}
				if (stagemig == nhsmfiles + 1) {
					int attached = 0;
					char *dummy;

					/* Check if the option -M is attached or not */
					if (strstr(argv[Coptind - 1],"-M") == argv[Coptind - 1]) {
						attached = 1;
					}
					/* We want to know if there is no ':' in the string or, if there is such a ':' */
					/* if there is no '/' before (then is will indicate a hostname)                */
					if (! ISCASTOR(Coptarg)) {
						/* We prepend HSM_HOST only for non CASTOR-like files */
						if ((dummy = strchr(Coptarg,':')) == NULL || (dummy != Coptarg && strrchr(dummy,'/') == NULL)) {
							if ((hsm_host = getenv("HSM_HOST")) != NULL) {
								if (hsm_host[0] != '\0') {
									strcpy (hsm_path, hsm_host);
									strcat (hsm_path, ":");
								}
								strcat (hsm_path, Coptarg);
								if ((hsmfiles[nhsmfiles] = (char *) malloc((attached != 0 ? 2 : 0) + strlen(hsm_path) + 1)) == NULL) {
									fprintf(stderr,"malloc error (%s)\n",strerror(errno));
									errflg++;
								} else {
									if (attached != 0) {
										strcpy(hsmfiles[nhsmfiles],"-M");
										strcat(hsmfiles[nhsmfiles++],hsm_path);
									} else {
										strcpy(hsmfiles[nhsmfiles++],hsm_path);
									}
								}
							} else if ((hsm_host = getconfent("STG", "HSM_HOST",0)) != NULL) {
								if (hsm_host[0] != '\0') {
									strcpy (hsm_path, hsm_host);
									strcat (hsm_path, ":");
								}
								strcat (hsm_path, Coptarg);
								if ((hsmfiles[nhsmfiles] = (char *) malloc((attached != 0 ? 2 : 0) + strlen(hsm_path) + 1)) == NULL) {
									fprintf(stderr,"malloc error (%s)\n",strerror(errno));
									errflg++;
								} else {
									if (attached != 0) {
										strcpy(hsmfiles[nhsmfiles],"-M");
										strcat(hsmfiles[nhsmfiles++],hsm_path);
									} else {
										strcpy(hsmfiles[nhsmfiles++],hsm_path);
									}
								}
							} else {
								fprintf (stderr, STG54);
								errflg++;
							}
							argv[Coptind - 1] = hsmfiles[nhsmfiles - 1];
						} else {
							/* Here we believe that the user gave a hostname */
							hsmfiles[nhsmfiles++] = NULL;
                	    }
					} else {
						/* Here we believe that the user gave CASTOR-like file */
						hsmfiles[nhsmfiles++] = NULL;
					}
				} else {
					fprintf (stderr, "Cannot parse hsm file %s\n", Coptarg);
					errflg++;
				}
			} else {
				fprintf (stderr, "Cannot parse hsm file %s\n", Coptarg);
				errflg++;
			}
			break;
		case 'q':
			break;
		case 'U':
			stage_strtoi(&fun, Coptarg, &dp, 10);
			if (*dp != '\0') {
				fprintf (stderr, STG06, "-U\n");
				errflg++;
			}
			break;
		case 'V':
			if ((int) strlen (Coptarg) <= 6)
				numvid = 1;
			else {
				fprintf (stderr, STG06, "V");
				errflg++;
			}
			break;
		case 0:
			/* Long option without short option correspondance */
			if (tppool_flag != 0) {
				tppool = Coptarg;
			}
			break;
		case '?':
			errflg++;
			break;
		}
        if (errflg != 0) break;
	}
	if (Coptind >= argc && fun == 0 && numvid == 0 && Iflag == 0 && Mflag == 0) {
		fprintf (stderr, STG46);
		errflg++;
	}

	if (Mflag && (argc > Coptind)) {
		fprintf (stderr, STG16);
		errflg++;
	}

	if (fun)
		nargs++;

	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, STGMAGIC);
	marshall_LONG (sbp, STAGEPUT);
	q = sbp;	/* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);

	/* Build request body */

	if ((pw = Cgetpwuid (uid)) == NULL) {
		char uidstr[8];
		if (errno != ENOENT) fprintf (stderr, STG33, "Cgetpwuid", strerror(errno));
		sprintf (uidstr, "%d", uid);
		p = uidstr;
		fprintf (stderr, STG11, p);
        freehsmfiles(nhsmfiles, hsmfiles);
		exit (SYERR);
	}
	marshall_STRING (sbp, pw->pw_name);	/* login name */
	if (Gflag) {
		marshall_STRING (sbp, Gname);
		marshall_WORD (sbp, Guid);
	} else {
		marshall_STRING (sbp, pw->pw_name);
		marshall_WORD (sbp, uid);
	}
	marshall_WORD (sbp, gid);
	pid = getpid();
	marshall_WORD (sbp, pid);
	
	marshall_WORD (sbp, nargs);
	for (i = 0; i < Coptind; i++)
		marshall_STRING (sbp, argv[i]);
#if defined(_WIN32)
	if (WSAStartup (MAKEWORD (2, 0), &wsadata)) {
		fprintf (stderr, STG51);
        freehsmfiles(nhsmfiles, hsmfiles);
		exit (SYERR);
	}
#endif
	for (i = Coptind; i < argc; i++) {
		if ((c = build_linkname (argv[i], path, sizeof(path), STAGEPUT)) == SESYSERR) {
#if defined(_WIN32)
			WSACleanup();
#endif
            freehsmfiles(nhsmfiles, hsmfiles);
			exit (SYERR);
		} else if (c) {
			errflg++;
			continue;
		} else {
			if (sbp + strlen (path) - sendbuf >= sizeof(sendbuf)) {
				fprintf (stderr, STG38);
				errflg++;
				break;
			}
			marshall_STRING (sbp, path);
		}
	}
	if (fun) {
		if ((c = build_Upath (fun, path, sizeof(path), STAGEPUT)) == SESYSERR) {
#if defined(_WIN32)
			WSACleanup();
#endif
            freehsmfiles(nhsmfiles, hsmfiles);
			exit (SYERR);
		} else if (c)
			errflg++;
		else if (sbp + strlen (path) - sendbuf >= sizeof(sendbuf)) {
			fprintf (stderr, STG38);
			errflg++;
		} else
			marshall_STRING (sbp, path);
	}
	if (errflg != 0) {
#if defined(_WIN32)
		WSACleanup();
#endif
        freehsmfiles(nhsmfiles, hsmfiles);
		exit (1);
	}
	
	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */
	
#if ! defined(_WIN32)
	signal (SIGHUP, cleanup);
#endif
	signal (SIGINT, cleanup);
#if ! defined(_WIN32)
	signal (SIGQUIT, cleanup);
#endif
	signal (SIGTERM, cleanup);
	
	if (noretry_flag != 0) maxretry = 0;
	while (1) {
		c = send2stgd_cmd (stghost, sendbuf, msglen, 1, NULL, 0);
		if (c == 0 || serrno == EINVAL || serrno == ERTLIMBYSZ || serrno == ESTCLEARED ||
				serrno == ENOSPC || serrno == ESTKILLED) break;
		if (serrno == ESTNACT && nstg161++ == 0) fprintf(stderr, STG161);
		if (serrno != ESTNACT && ntries++ > maxretry) break;
		if (noretry_flag != 0) break; /* To be sure we always break if --noretry is in action */
		sleep (RETRYI);
	}
#if defined(_WIN32)
	WSACleanup();
#endif
    freehsmfiles(nhsmfiles, hsmfiles);
	exit (c == 0 ? 0 : rc_castor2shift(serrno));
}

void freehsmfiles(nhsmfiles,hsmfiles)
     int nhsmfiles;
     char **hsmfiles;
{
  int i;

  if (hsmfiles == NULL) return;

  for (i = 0; i < nhsmfiles; i++) {
    if (hsmfiles[i] != NULL) {
      free(hsmfiles[i]);
    }
  }

  free(hsmfiles);
}

void cleanup(sig)
		 int sig;
{
	int c;
	int msglen;
	char *q;
	char *sbp;
	char sendbuf[64];
	/* char repbuf[CA_MAXPATHLEN+1]; */
	

	signal (sig, SIG_IGN);
	
	sbp = sendbuf;
	marshall_LONG (sbp, STGMAGIC);
	marshall_LONG (sbp, STAGEKILL);
	q = sbp;	/* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);
	marshall_STRING (sbp, pw->pw_name);	/* login name */
	marshall_WORD (sbp, gid);
	marshall_WORD (sbp, pid);
	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */
	c = send2stgd_cmd (stghost, sendbuf, msglen, 0, NULL, 0);
#if defined(_WIN32)
	WSACleanup();
#endif
	exit (USERR);
}

void usage(cmd)
		 char *cmd;
{
	fprintf (stderr, "usage: %s ", cmd);
	fprintf (stderr, "%s", "[-G] [-h stage_host] [-U fun] pathname(s)\n");
	fprintf (stderr, "       %s ", cmd);
	fprintf (stderr, "%s",
					 "[-G] [-h stage_host] [-q file_sequence_number(s)] [--tppool tape pool] -V visual_identifier\n");
}

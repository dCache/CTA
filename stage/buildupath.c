/*
 * $Id: buildupath.c,v 1.10 2000/12/07 08:19:31 jdurand Exp $
 */

/*
 * Copyright (C) 1994-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: buildupath.c,v $ $Revision: 1.10 $ $Date: 2000/12/07 08:19:31 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#if defined(_WIN32)
#include <winsock2.h>
#endif
#include "stage.h"
#if !defined(linux)
extern char *sys_errlist[];
#endif

static char cwd[256];
static char hostname[CA_MAXHOSTNAMELEN + 1];
static int initialized = 0;
static char nfsroot[MAXPATH];

init_cwd_hostname()
{
	char *getconfent();
	char *getcwd();
	int n = 0;
	char *p;

	initialized = 1;
	p = getconfent ("RFIO", "NFS_ROOT", 0);
#ifdef NFSROOT
	if (p == NULL) p = NFSROOT;
#endif

	if (p != NULL)
		strcpy (nfsroot, p);
	else
		nfsroot[0] = '\0';
	gethostname (hostname, CA_MAXHOSTNAMELEN + 1);
#if defined(_WIN32)
	p = getcwd (cwd, sizeof(cwd));
#else
	while ((p = getcwd (cwd, sizeof(cwd))) == NULL && errno == ETIMEDOUT) {
		if (n++ == 0)
			fprintf (stderr, STG02, "", "getcwd", sys_errlist[errno]);
		sleep (RETRYI);
	}
#endif
	if (p == NULL) {
		fprintf (stderr, STG02, "", "getcwd", sys_errlist[errno]);
		return (SYERR);
	}
	return (0);
}

resolvelinks(argvi, buf, buflen, req_type)
		 char *argvi;
		 char *buf;
		 int buflen;
		 int req_type;
{
	char *dir;
	char dsksrvr[CA_MAXHOSTNAMELEN + 1];
	char *getcwd();
	char linkbuf[CA_MAXHOSTNAMELEN + 1 + MAXPATH];
	char *p, *q;

	if ((p = strstr (argvi, ":/")) != NULL) {
		strncpy (dsksrvr, argvi, p - argvi);
		dsksrvr[p - argvi] = '\0';
		dir = ++p;
		q = dir + strlen(nfsroot);
		if (! *nfsroot ||
				strncmp (dir, nfsroot, strlen (nfsroot)) || *q != '/')	
			/* server:/xxxx    where xxxx is not nfsroot */
			if (strcmp (dsksrvr, hostname)) {
				/* server is not the local machine */
				if (strlen (argvi) + 1 > buflen) return (-1);
				strcpy (buf, argvi);
				return (0);
			}
	} else
		dir = argvi;
	q = dir + strlen(nfsroot);
	if (*nfsroot &&
			strncmp (dir, nfsroot, strlen (nfsroot)) == 0 && *q == '/' &&
			(p = strchr (q + 1, '/')) != NULL) {
		/* /shift syntax */
		strncpy (dsksrvr, q + 1, p - q - 1);
		dsksrvr[p - q - 1] = '\0';
		if (strcmp (dsksrvr, hostname)) { /* not on local machine */
			if (strlen (dir) + 1 > buflen) return (-1);
			strcpy (buf, dir);
			return (0);
		}
	}
#if defined(_WIN32)
	strcpy (linkbuf, dir);	/* links are not supported */
#else
	if (chdir (dir) < 0) {
		fprintf (stderr, STG02, dir, "chdir", sys_errlist[errno]);
		if (req_type == STAGECLR) {	/* let stgdaemon try */
			if (strlen (dir) + 1 > buflen) return (-1);
			strcpy (buf, dir);
			return (0);
		} else
			return (-1);
	}
	if (getcwd (linkbuf, sizeof(linkbuf)) == NULL) {
		fprintf (stderr, STG02, argvi, "getcwd", sys_errlist[errno]);
		return (-1);
	}
	if (chdir (cwd) < 0) {
		fprintf (stderr, STG02, cwd, "chdir", sys_errlist[errno]);
		return (-1);
	}
#endif
	if ((! *nfsroot ||
			 strncmp (linkbuf, nfsroot, strlen (nfsroot)) ||
			 *(linkbuf + strlen(nfsroot)) != '/') /* not /shift syntax */
			&& (strncmp (linkbuf, "/afs/", 5) || req_type == STAGEWRT))
		if ((int) strlen (hostname) + (int) strlen (linkbuf) + 2 > buflen)
			return (-1);
		else
			sprintf (buf, "%s:%s", hostname, linkbuf);
	else
		if ((int) strlen (linkbuf) + 1 > buflen)
			return (-1);
		else
			strcpy (buf, linkbuf);
	return (0);
}

build_linkname(argvi, path, size, req_type)
		 char *argvi;
		 char *path;
		 int size;
		 int req_type;
{
	char buf[256];
	int c;
	char *p;

	if (! initialized && (c = init_cwd_hostname())) return (c);
	if (*argvi != '/' && strstr (argvi, ":/") == NULL) {
		if ((! *nfsroot ||
				 strncmp (cwd, nfsroot, strlen (nfsroot)) ||
				 *(cwd + strlen(nfsroot)) != '/') /* not /shift syntax */
				&& (strncmp (cwd, "/afs/", 5) || req_type == STAGEWRT))
			if ((int) strlen (hostname) + (int) strlen (cwd) +
					(int) strlen (argvi) + 3 > size) {
				fprintf (stderr, STG08, argvi);
				return (USERR);
			} else
				sprintf (path, "%s:%s/%s", hostname, cwd, argvi);
		else
			if ((int) strlen (cwd) + (int) strlen (argvi) + 2 > size) {
				fprintf (stderr, STG08, argvi);
				return (USERR);
			} else
				sprintf (path, "%s/%s", cwd, argvi);
	} else {
		strcpy (buf, argvi);
		if ((p = strrchr (buf, '/')) != NULL) {
			*p = '\0';
			if (resolvelinks (buf, path, size - strlen (p+1) - 1, req_type) < 0) {
				fprintf (stderr, STG08, argvi);
				return (USERR);
			}
			*p = '/';
		} else {
			fprintf (stderr, STG08, argvi);
			return (USERR);
		}
		strcat (path, p);
	}
	return (0);
}

build_Upath(fun, path, size, req_type)
		 int fun;
		 char *path;
		 int size;
		 int req_type;
{
#if !defined(_WIN32)
	char buf[16];
	int c;

	if (! initialized && (c = init_cwd_hostname())) return (c);
#if _IBMESA
	sprintf (buf, "FILE.FT%d.F01", fun);
#else
#if hpux
	sprintf (buf, "ftn%02d", fun);
#else
	sprintf (buf, "fort.%d", fun);
#endif
#endif
	if ((! *nfsroot ||
			 strncmp (cwd, nfsroot, strlen (nfsroot)) ||
			 *(cwd + strlen(nfsroot)) != '/') /* not /shift syntax */
			&& (strncmp (cwd, "/afs/", 5) || req_type == STAGEWRT))
		if ((int) strlen (hostname) + (int) strlen (cwd) +
				(int) strlen (buf) + 3 > size) {
			fprintf (stderr, STG08, buf);
			return (USERR);
		} else
			sprintf (path, "%s:%s/%s", hostname, cwd, buf);
	else
		if ((int) strlen (cwd) + (int) strlen (buf) + 2 > size) {
			fprintf (stderr, STG08, buf);
			return (USERR);
		} else
			sprintf (path, "%s/%s", cwd, buf);
#endif
	return (0);
}

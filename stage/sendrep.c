/*
 * $Id: sendrep.c,v 1.4 1999/07/21 20:09:05 jdurand Exp $
 *
 * $Log: sendrep.c,v $
 * Revision 1.4  1999/07/21 20:09:05  jdurand
 * Initialize all variable pointers to NULL
 *
 * Revision 1.3  1999/07/20 17:29:19  jdurand
 * Added Id and Log CVS's directives
 *
 */

/*
 * Copyright (C) 1993-1995 by CERN/CN/PDP/DH
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)sendrep.c	1.3 08/03/95 CERN CN-PDP/DH Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <varargs.h>
#include <netinet/in.h>
#include "marshall.h"
#include "net.h"
#include "stage.h"
extern char *sys_errlist[];
sendrep(va_alist) va_dcl
{
	va_list args;
	char *file1 = NULL;
    char *file2 = NULL;
	char func[16];
	char *msg = NULL;
	char *p = NULL;
	char prtbuf[PRTBUFSZ];
	char *q = NULL;
	char *rbp = NULL;
	int rc;
	int rep_type;
	int req_type;
	char repbuf[REPBUFSZ];
	int repsize;
	int rpfd;
	static char savebuf[256];
	static int saveflag = 0;

	strcpy (func, "sendrep");
	rbp = repbuf;
	marshall_LONG (rbp, STGMAGIC);
	va_start (args);
	rpfd = va_arg (args, int);
	rep_type = va_arg (args, int);
	marshall_LONG (rbp, rep_type);
	switch (rep_type) {
	case MSG_OUT:
	case MSG_ERR:
	case RTCOPY_OUT:
		msg = va_arg (args, char *);
		vsprintf (prtbuf, msg, args);
		marshall_LONG (rbp, strlen (prtbuf) + 1);
		marshall_STRING (rbp, prtbuf);
		if (rep_type == MSG_ERR) stglogit (func, "%s", prtbuf);
		if (rep_type == RTCOPY_OUT) {
			p = prtbuf;
			if (saveflag) {
				q = strchr (p, '\n');
				if (! q) {	/* line is still incomplete */
					strcat (savebuf, p);
					goto sndmsg;
				}
				*q = '\0';
				strcat (savebuf, p);
				switch (iserrmsg (savebuf)) {
				case 0:
					break;
				case 1:	/* tpread/tpwrite error */
					stglogit (func, "%s\n", savebuf+16);
					break;
				case 2:	/* cptpdsk/cpdsktp error */
					stglogit (func, "%s\n", savebuf);
				}
				saveflag = 0;
				p = q + 1;
			}
			while (q = strchr (p, '\n')) {
				*q = '\0';
				switch (iserrmsg (p)) {
				case 0:
					break;
				case 1:	/* tpread/tpwrite error */
					stglogit (func, "%s\n", p+16);
					break;
				case 2:	/* cptpdsk/cpdsktp error */
					stglogit (func, "%s\n", p);
				}
				p = q + 1;
			}
			if (strlen (p)) {	/* save incomplete line */
				strcpy (savebuf, p);
				saveflag = 1;
			}
		}
		break;
	case STAGERC:
		req_type = va_arg (args, int);
		rc = va_arg (args, int);
		marshall_LONG (rbp, rc);
		if (req_type != STAGEQRY && req_type != STAGEUPDC)
			stglogit (func, STG99, rc);
		break;
	case SYMLINK:
		file1 = va_arg (args, char *);
		file2 = va_arg (args, char *);
		marshall_LONG (rbp, strlen (file1) + strlen (file2) + 2);
		marshall_STRING (rbp, file1);
		marshall_STRING (rbp, file2);
		break;
	case RMSYMLINK:
		file1 = va_arg (args, char *);
		marshall_LONG (rbp, strlen (file1) + 1);
		marshall_STRING (rbp, file1);
	}
sndmsg:
	va_end (args);
	repsize = rbp - repbuf;
	if (netwrite_timeout (rpfd, repbuf, repsize, STGTIMEOUT) != repsize) {
		stglogit (func, STG02, "", "write", sys_errlist[errno]);
		if (rep_type == STAGERC)
			close (rpfd);
		return (-1);
	}
	if (rep_type == STAGERC)
		close (rpfd);
	return (0);
}

iserrmsg(p)
char *p;
{
	char *q = NULL;

	if (*p == '\0') return (0);
	if (strncmp (p, " CP", 3) == 0)
		if (*(p+9) == '!')
			return (2);
		else
			return (0);
	if (strncmp (p+16, "tpread", 6) &&
	    strncmp (p+16, "tpwrite", 7)) return (2);
	if ((q = strchr (p, ']')) == NULL) return (0);
	if (*(q+3) == '!')
		return (1);
	else
		return (0);
}

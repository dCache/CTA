/*
 * $Id: stage_migpool.c,v 1.1 2000/05/08 10:23:50 jdurand Exp $
 */

/*
 * Copyright (C) 1993-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "$RCSfile: stage_migpool.c,v $ $Revision: 1.1 $ $Date: 2000/05/08 10:23:50 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#endif
#include "marshall.h"
#include "serrno.h"
#include "stage_api.h"
#include "stage.h"

int DLL_DECL stage_migpool(stghost,diskpool,migpool)
		 char *stghost;
		 char *diskpool;
		 char *migpool;
{
	int c;
	gid_t gid;
	int msglen;
	int nargs;
	int ntries = 0;
	struct passwd *pw;
	char *q, *q2;
	char *rbp;
	char repbuf[CA_MAXPATHLEN+1];
	char *sbp;
	char sendbuf[REQBUFSZ];
	uid_t uid;
    char *diskpoolok;
    char *diskpooldefault = "-";
    char *stghost_ok = NULL;

	uid = getuid();
	gid = getgid();
#if defined(_WIN32)
	if (uid < 0 || gid < 0) {
		serrno = SENOMAPFND;
		return (-1);
	}
#endif

    /* Makes sure stghost is valid if any */
    if (stghost != NULL) {
      if (stghost[0] != '\0') {
        stghost_ok = stghost;
      }
    }

    if (diskpool != NULL) {
      if (diskpool[0] != '\0') {
        diskpoolok = diskpool;
      } else {
        diskpoolok = diskpooldefault;
      }
    } else {
      diskpoolok = diskpooldefault;
    }

	/* Init repbuf to null */
	repbuf[0] = '\0';

	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, STGMAGIC);
	marshall_LONG (sbp, STAGEMIGPOOL);
	q = sbp;	/* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);

	/* Build request body */

	if ((pw = getpwuid (uid)) == NULL) {
		serrno = SENOMAPFND;
		return (-1);
	}
	marshall_STRING (sbp, pw->pw_name);	/* login name */
	marshall_WORD (sbp, uid);
	marshall_WORD (sbp, gid);
	q2 = sbp;	/* save pointer. The next field will be updated */
	nargs = 1;
	marshall_WORD (sbp, nargs);
	marshall_STRING (sbp, "stage_migpool");

	marshall_STRING (sbp, diskpoolok);
	nargs += 1;

	marshall_WORD (q2, nargs);	/* update nargs */

	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */

	while (1) {
		c = send2stgd (stghost_ok, sendbuf, msglen, 1, repbuf, sizeof(repbuf));
		if (c == 0) break;
		if (serrno != ESTNACT || ntries++ > MAXRETRY) break;
		sleep (RETRYI);
	}
	if (c == 0 && migpool != NULL && repbuf[0] != '\0') {
		rbp = repbuf;
		unmarshall_STRING (rbp, migpool);
	}
	return (c == 0 ? 0 : -1);
}

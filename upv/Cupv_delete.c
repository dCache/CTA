/*
 * Copyright (C) 1999-2000 by CERN/IT/PDP/DM
 * All rights reserved
 */
 
#ifndef lint
static char sccsid[] = "@(#)$RCSfile: Cupv_delete.c,v $ $Revision: 1.6 $ $Date: 2003/11/04 14:24:43 $ CERN IT-DS/HSM Ben Couturier";
#endif /* not lint */
 

#include <errno.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#endif
#include "marshall.h"
#include "Cupv_api.h"
#include "Cupv.h"
#include "serrno.h"

Cupv_delete(uid_t priv_uid, gid_t priv_gid, const char *src, const char *tgt)
{
	int c;
	char func[16];
	int msglen;
	char *q;
	char *dp;
	char *sbp;
	char sendbuf[REQBUFSZ];
	struct Cupv_api_thread_info *thip;
	uid_t uid;
	gid_t gid;
	int lensrc, lentgt;

        strcpy (func, "Cupv_delete");
        if (Cupv_apiinit (&thip))
                return (-1);
        uid = geteuid();
        gid = getegid();
#if defined(_WIN32)
        if (uid < 0 || gid < 0) {
                Cupv_errmsg (func, CUP53);
                serrno = SENOMAPFND;
                return (-1);
        }
#endif

	if (priv_uid < 0 || priv_gid < 0) {
	  serrno = EINVAL;
	  return (-1);
	}


	if (src == NULL || tgt == NULL) {
	  serrno = EINVAL;
	  return(-1);
	}


	lensrc = strlen(src);
	lentgt = strlen(tgt);

	if (lensrc == 0 || lentgt == 0
	    || lensrc > CA_MAXREGEXPLEN || lentgt > CA_MAXREGEXPLEN) {
		serrno = EINVAL;
		return (-1);
	}
 
#ifndef HAVE_CUPV_DAEMON
	serrno = EPERM;
	return(-1);
#else
	/* Build request header */
	sbp = sendbuf;
	marshall_LONG (sbp, CUPV_MAGIC);
	marshall_LONG (sbp, CUPV_DELETE);
	q = sbp;        /* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG(sbp, msglen);

	marshall_LONG (sbp, uid);
	marshall_LONG (sbp, gid);
	marshall_LONG (sbp, priv_uid);
	marshall_LONG (sbp, priv_gid);
	marshall_STRING (sbp, src);
	marshall_STRING (sbp, tgt);
 
	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */

	while ((c = send2Cupv (NULL, sendbuf, msglen, NULL, 0)) &&
	    serrno == ECUPVNACT)
		sleep (RETRYI);
	return (c);
#endif
}











/*
 * Copyright (C) 1999 by CERN/IT/PDP/DM
 * All rights reserved
 */
 
#ifndef lint
static char sccsid[] = "@(#)$RCSfile: vmgr_gettape.c,v $ $Revision: 1.2 $ $Date: 1999/12/17 13:03:57 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */
 
/*      vmgr_gettape - get a tape volume to store a given amount of data */

#include <errno.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#endif
#include "marshall.h"
#include "vmgr_api.h"
#include "vmgr.h"
#include "serrno.h"

vmgr_gettape(const char *poolname, int Size, const char *Condition, char *vid, char *vsn, char *dgn, char *density, char *lbltype, int *fseq, unsigned int *blockid)
{
	int c;
	char func[16];
	gid_t gid;
	int msglen;
	char *q;
	char *rbp;
	char repbuf[CA_MAXVIDLEN+17];
	char *sbp;
	struct vmgr_api_thread_info *thip;
	char sendbuf[REQBUFSZ];
	uid_t uid;

        strcpy (func, "vmgr_gettape");
        if (vmgr_apiinit (&thip))
                return (-1);
        uid = geteuid();
        gid = getegid();
#if defined(_WIN32)
        if (uid < 0 || gid < 0) {
                vmgr_errmsg (func, VMG53);
                serrno = SENOMAPFND;
                return (-1);
        }
#endif

	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, VMGR_MAGIC);
	marshall_LONG (sbp, VMGR_GETTAPE);
	q = sbp;        /* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);

	/* Build request body */
 
	marshall_LONG (sbp, uid);
	marshall_LONG (sbp, gid);
	if (poolname) {
		marshall_STRING (sbp, poolname);
	} else {
		marshall_STRING (sbp, "");
	}
	marshall_LONG (sbp, Size);
	if (Condition) {
		marshall_STRING (sbp, Condition);
	} else {
		marshall_STRING (sbp, "");
	}
 
	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */

	c = send2vmgr (NULL, sendbuf, msglen, repbuf, sizeof(repbuf));
	if (c == 0) {
		rbp = repbuf;
		unmarshall_STRING (rbp, vid);
		unmarshall_STRING (rbp, vsn);
		unmarshall_STRING (rbp, dgn);
		unmarshall_STRING (rbp, density);
		unmarshall_STRING (rbp, lbltype);
		unmarshall_LONG (rbp, *fseq);
		unmarshall_LONG (rbp, *blockid);
	}
	return (c);
}

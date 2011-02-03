/*
 * Copyright (C) 2000 by CERN/IT/PDP/DM
 * All rights reserved
 */
 
/*      vmgr_querypool - query about a tape pool */

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include "marshall_trunk_r21843.h"
#include "vmgr_api_trunk_r21843.h"
#include "vmgr_trunk_r21843.h"
#include "serrno_trunk_r21843.h"
#include <string.h>

int vmgr_querypool(const char *pool_name, uid_t *pool_user, gid_t *pool_group, u_signed64 *capacity, u_signed64 *tot_free_space)
{
	int c;
	char func[15];
	gid_t gid;
	int msglen;
	int n;
	char *q;
	char *rbp;
	char repbuf[24];
	char *sbp;
	char sendbuf[REQBUFSZ];
	struct vmgr_api_thread_info *thip;
	uid_t uid;
	u_signed64 u64;

        strncpy (func, "vmgr_querypool", 15);
        if (vmgr_apiinit (&thip))
                return (-1);
        uid = geteuid();
        gid = getegid();

	if (! pool_name) {
		serrno = EFAULT;
		return (-1);
	}

	if (strlen (pool_name) > CA_MAXPOOLNAMELEN) {
		serrno = EINVAL;
		return (-1);
	}
 
	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, VMGR_MAGIC);
	marshall_LONG (sbp, VMGR_QRYPOOL);
	q = sbp;        /* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);

	/* Build request body */
 
	marshall_LONG (sbp, uid);
	marshall_LONG (sbp, gid);
	marshall_STRING (sbp, pool_name);
 
	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */

	while ((c = send2vmgr (NULL, sendbuf, msglen, repbuf, sizeof(repbuf))) &&
	    serrno == EVMGRNACT)
		sleep (RETRYI);
	if (c == 0) {
		rbp = repbuf;
		unmarshall_LONG (rbp, n);
		if (pool_user)
			*pool_user = n;
		unmarshall_LONG (rbp, n);
		if (pool_group)
			*pool_group = n;
		unmarshall_HYPER (rbp, u64);
		if (capacity)
			*capacity = u64;
		unmarshall_HYPER (rbp, u64);
		if (tot_free_space)
			*tot_free_space = u64;
	}
	return (c);
}

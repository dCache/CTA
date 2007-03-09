/*
 * Copyright (C) 2001-2003 by CERN/IT/PDP/DM
 * All rights reserved
 */

#include <errno.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "marshall.h"
#include "net.h"
#include "rmc.h"
int sendrep(int rpfd, int rep_type, ...)
{
	va_list args;
	char func[16];
	char *msg;
	int n;
	char prtbuf[PRTBUFSZ];
	char *rbp;
	int rc;
	char repbuf[REPBUFSZ];
	int repsize;

	strcpy (func, "sendrep");
	rbp = repbuf;
	marshall_LONG (rbp, RMC_MAGIC);
	va_start (args, rep_type);
	marshall_LONG (rbp, rep_type);
	switch (rep_type) {
	case MSG_ERR:
		msg = va_arg (args, char *);
		vsprintf (prtbuf, msg, args);
		marshall_LONG (rbp, strlen (prtbuf) + 1);
		marshall_STRING (rbp, prtbuf);
		rmclogit (func, "%s", prtbuf);
		break;
	case MSG_DATA:
		n = va_arg (args, int);
		marshall_LONG (rbp, n);
		msg = va_arg (args, char *);
		memcpy (rbp, msg, n);	/* marshalling already done */
		rbp += n;
		break;
	case RMC_RC:
		rc = va_arg (args, int);
		marshall_LONG (rbp, rc);
		break;
	}
	va_end (args);
	repsize = rbp - repbuf;
	if (netwrite (rpfd, repbuf, repsize) != repsize) {
		rmclogit (func, RMC02, "send", neterror());
		if (rep_type == RMC_RC)
			netclose (rpfd);
		return (-1);
	}
	if (rep_type == RMC_RC)
		netclose (rpfd);
	return (0);
}

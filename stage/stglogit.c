/*
 * Copyright (C) 1993-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: stglogit.c,v $ $Revision: 1.5 $ $Date: 1999/12/08 15:57:39 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <varargs.h>
#include "stage.h"
extern int reqid;

stglogit(va_alist) va_dcl
{
	va_list args;
	char *func;
	char *msg;
	char prtbuf[PRTBUFSZ];
	struct tm *tm;
	time_t current_time;
	int fd_log;

	va_start (args);
	func = va_arg (args, char *);
	msg = va_arg (args, char *);
	time (&current_time);		/* Get current time */
	tm = localtime (&current_time);
	sprintf (prtbuf, "%02d/%02d %02d:%02d:%02d %5d %s: ", tm->tm_mon+1,
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, reqid, func);
	vsprintf (prtbuf+strlen(prtbuf), msg, args);
	va_end (args);
	fd_log = open (LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0664);
	write (fd_log, prtbuf, strlen(prtbuf));
	close (fd_log);
}

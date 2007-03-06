/*
 * Copyright (C) 1990-2003 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
/* static char sccsid[] = "@(#)$RCSfile: tplogit.c,v $ $Revision: 1.5 $ $Date: 2007/03/06 16:46:35 $ CERN IT-PDP/DM Jean-Philippe Baud"; */
#endif /* not lint */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include "Ctape.h"
extern int jid;

int tplogit(char *func, char *msg, ...)
{
	va_list args;
	char prtbuf[PRTBUFSZ];
	int save_errno;
	struct tm *tm;
	time_t current_time;
	int fd_log;

	save_errno = errno;
	va_start (args, msg);
	time (&current_time);		/* Get current time */
	tm = localtime (&current_time);
	sprintf (prtbuf, "%02d/%02d %02d:%02d:%02d %5d %s: ", tm->tm_mon+1,
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, jid, func);
	vsprintf (prtbuf+strlen(prtbuf), msg, args);
	va_end (args);
	fd_log = open (LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0664);
	write (fd_log, prtbuf, strlen(prtbuf));
	close (fd_log);
	errno = save_errno;
	return (0);
}

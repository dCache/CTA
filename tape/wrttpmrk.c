/*
 * Copyright (C) 1990-2000 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: wrttpmrk.c,v $ $Revision: 1.3 $ $Date: 2000/05/03 06:46:34 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#if defined(_AIX) && defined(_IBMR2)
#include <sys/tape.h>
#define mtop stop
#define mt_op st_op
#define mt_count st_count
#define MTIOCTOP STIOCTOP
#define MTWEOF STWEOF
#else
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <sys/mtio.h>
#endif
#endif
#include "Ctape.h"
wrttpmrk(tapefd, path, n)
#if defined(_WIN32)
HANDLE tapefd;
#else
int tapefd;
#endif
char *path;
int n;
{
	char func[16];
#if !defined(_WIN32)
	struct mtop mtop;
#endif

	ENTRY (wrttpmrk);
#if !defined(_WIN32)
	mtop.mt_op = MTWEOF;	/* write tape mark */
	mtop.mt_count = n;
	if (ioctl (tapefd, MTIOCTOP, &mtop) < 0) {
#else
	if (WriteTapemark (tapefd, TAPE_FILEMARKS, n, n ? (BOOL)1 : (BOOL)0)) {
#endif
		int rc;
		rc = rpttperror (func, tapefd, path, "ioctl");
		RETURN (-rc);
	}
	RETURN (0);
}

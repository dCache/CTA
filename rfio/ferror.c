/*
 * $Id: ferror.c,v 1.2 1999/07/20 12:47:57 jdurand Exp $
 *
 * $Log: ferror.c,v $
 * Revision 1.2  1999/07/20 12:47:57  jdurand
 * 20-JUL-1999 Jean-Damien Durand
 *   Timeouted version of RFIO. Using netread_timeout() and netwrite_timeout
 *   on all control and data sockets.
 *
 */

/*
 * Copyright (C) 1990,1991 by CERN/CN/SW/DC
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)ferror.c	3.3 5/6/93 CERN CN-SW/DC Antoine Trannoy";
#endif /* not lint */

/* ferror.c      Remote File I/O - tell if an error happened            */

#define RFIO_KERNEL     1       /* KERNEL part of the routines          */

#include "rfio.h"               /* Remote File I/O general definitions  */

int rfio_ferror(fp)
	FILE * fp ; 
{
	return (ferror(fp)) ;
}


/*
 * $Id: lockf.c,v 1.3 1999/12/09 08:48:21 baran Exp $
 *
 * $Log: lockf.c,v $
 * Revision 1.3  1999/12/09 08:48:21  baran
 * Thread-safe version
 *
 * Revision 1.2  1999/07/20 12:48:00  jdurand
 * 20-JUL-1999 Jean-Damien Durand
 *   Timeouted version of RFIO. Using netread_timeout() and netwrite_timeout
 *   on all control and data sockets.
 *
 */

/*
 * Copyright (C) 1994 by CERN/CN/PDP
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)lockf.c	1.2 5/6/98 CERN CN-PDP Antony Simmins";
#endif /* not lint */

/* lockf.c       Remote File I/O - record locking on files		*/

#define RFIO_KERNEL     1       /* KERNEL part of the routines          */

#include "rfio.h"               /* Remote File I/O general definitions  */


int  rfio_lockf(sd, op, siz)   	/* Remote lockf	                	*/
int		sd,		/* file descriptor	 		*/
   op;		/* lock operation	            	*/
long		siz;		/* locked region			*/
{
   static char     buf[256];       /* General input/output buffer          */
   int             status;         /* remote lockf() status        */
   int     	len;
   char    	*p=buf;
   int 		rt ;
   int 		rcode ;


   INIT_TRACE("RFIO_TRACE");
   TRACE(1, "rfio", "rfio_lockf(%d, %d, %ld)", sd, op, siz);

   /* 
    * The file is local
    */
   if ((sd >= MAXRFD) || (rfilefdt[sd] == NULL)) {
      TRACE(1, "rfio", "rfio_lockf: using local lockf(%d, %d, %ld)",
	    sd, op, siz);
      END_TRACE();
      rfio_errno = 0;
      return(lockf(sd,op,siz));
   }

   len = 2*LONGSIZE;
   marshall_WORD(p, RFIO_MAGIC);
   marshall_WORD(p, RQST_LOCKF);
   marshall_WORD(p, geteuid());
   marshall_WORD(p, getegid());
   marshall_LONG(p, len);
   p= buf + RQSTSIZE;
   marshall_LONG(p, op);
   marshall_LONG(p, siz);
   TRACE(1,"rfio","rfio_lockf: op %d, siz %ld", op, siz);
   TRACE(2,"rfio","rfio_lockf: sending %d bytes",RQSTSIZE+len) ;
   if (netwrite_timeout(sd,buf,RQSTSIZE+len,RFIO_CTRL_TIMEOUT) != (RQSTSIZE+len)) {
      TRACE(2, "rfio", "rfio_lockf: write(): ERROR occurred (errno=%d)", errno);
      (void) close(sd);
      END_TRACE();
      return(-1);
   }
   p = buf;
   TRACE(2, "rfio", "rfio_lockf: reading %d bytes", 2*LONGSIZE);
   if (netread_timeout(sd, buf, 2*LONGSIZE, RFIO_CTRL_TIMEOUT) != (2*LONGSIZE))  {
      TRACE(2, "rfio", "rfio_lockf: read(): ERROR occurred (errno=%d)", errno);
      (void) close(sd);
      END_TRACE();
      return(-1);
   }
   unmarshall_LONG(p, status);
   unmarshall_LONG(p, rcode);
   TRACE(1, "rfio", "rfio_lockf: return %d",status);
   rfio_errno = rcode;
   if (status)     {
      END_TRACE();
      return(-1);
   }
   END_TRACE();
   return (0);
}

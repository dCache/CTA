/*
 * $Id: rewinddir.c,v 1.9 2000/09/20 13:52:52 jdurand Exp $
 */

/*
 * Copyright (C) 1990-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: rewinddir.c,v $ $Revision: 1.9 $ $Date: 2000/09/20 13:52:52 $ CERN/IT/PDP/DM Olof Barring";
#endif /* not lint */

/* rewinddir.c      Remote File I/O - rewind a directory                     */

#define RFIO_KERNEL     1 
#include "rfio.h"        

/*
 * remote directory close
 */

static int skey = -1;

int DLL_DECL rfio_rewinddir(dirp)
RDIR *dirp;
{
   WORD    req ; 
   LONG  rcode ;
   LONG msgsiz ;
   int *s = NULL;
   int status;
   char *p;
   extern RDIR *rdirfdt[MAXRFD];
   char     rfio_buf[BUFSIZ];

   Cglobals_get(&skey, (void **)&s, sizeof(int));
   /*
    * Search internal table for this directory pointer
    * Check first that it's not the last one used
    */
   if ( *s<0 || *s>=MAXRFD || rdirfdt[*s] != dirp )
      for ( *s=0; *s<MAXRFD; (*s)++ ) if ( rdirfdt[*s] == dirp ) break;

   INIT_TRACE("RFIO_TRACE");
   TRACE(1, "rfio", "rfio_rewinddir(0x%x)", dirp);

   /*
    * The directory is local
    */
   if ( *s < 0 || *s >= MAXRFD || rdirfdt[*s] == NULL ) {
      TRACE(2, "rfio", "rfio_rewinddir: check if HSM directory");
      (void)rfio_HsmIf_rewinddir((DIR *)dirp);
      TRACE(2, "rfio", "rfio_rewinddir: using local rewinddir(0x%x)",dirp) ; 
#if defined(_WIN32)
      serrno = SEOPNOTSUP;
      return(-1);
#else /* _WIN32 */
      (void)rewinddir((DIR *)dirp) ; 
#endif /* _WIN32 */
      END_TRACE() ; 
      return(0) ;
   }
   /*
    * Checking magic number
    */
   if ( rdirfdt[*s]->magic != RFIO_MAGIC ) {
      serrno = SEBADVERSION ;
      free((char *)rdirfdt[*s]);
      rdirfdt[*s] = NULL;
      (void) close(*s) ;
      END_TRACE();
      return(-1);
   }
   /*
    * Sending request.
    */
   p= rfio_buf ;
   marshall_WORD(p, RFIO_MAGIC);
   marshall_WORD(p, RQST_REWINDDIR);
   TRACE(2, "rfio", "rfio_rewinddir: sending %d bytes",RQSTSIZE) ;
   if (netwrite_timeout(*s, rfio_buf,RQSTSIZE,RFIO_CTRL_TIMEOUT) != RQSTSIZE) {
      TRACE(2, "rfio", "rfio_rewinddir: write(): ERROR occured (errno=%d)", errno);
      (void) rfio_dircleanup(*s) ;
      END_TRACE() ;
      return -1 ;
   }
   /*
    * Getting data from the network.
    */

   TRACE(2, "rfio", "rfio_rewinddir: reading %d bytes",WORDSIZE+3*LONGSIZE) ; 
   if (netread_timeout(*s,rfio_buf,WORDSIZE+3*LONGSIZE,RFIO_CTRL_TIMEOUT) != (WORDSIZE+3*LONGSIZE)) {
      TRACE(2, "rfio", "rfio_rewinddir: read(): ERROR occured (errno=%d)", errno);
      (void)rfio_dircleanup(*s) ; 
      END_TRACE() ;
      return -1 ; 
   }
   p = rfio_buf ;
   unmarshall_WORD(p,req) ;
   unmarshall_LONG(p,status) ;
   unmarshall_LONG(p, rcode) ;
   unmarshall_LONG(p,msgsiz) ;
   rfio_errno = rcode ;
   dirp->offset = 0;
   dirp->dp.dd_loc = 0;
   TRACE(1, "rfio", "rfio_rewinddir: return status=%d, rcode=%d",status,rcode) ;
   END_TRACE() ; 
   return status ;
}

/*
 * $Id: rfio_errno.h,v 1.3 2000/01/06 08:35:03 jdurand Exp $
 */

/*
 * Copyright (C) 1990-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

/*
 * $RCSfile: rfio_errno.h,v $ $Revision: 1.3 $ $Date: 2000/01/06 08:35:03 $
 */

/* rfio_errno.h   Thread safe rfio_errno  */

#ifndef _RFIO_ERRNO_H_INCLUDED_
#define _RFIO_ERRNO_H_INCLUDED_

#include <osdep.h>                  /* EXTERN_C, DLL_DECL and _PROTO */
#include <sys/types.h>              /* For size_t                    */

#if defined(_REENTRANT) || defined(_THREAD_SAFE) || \
   (defined(_WIN32) && (defined(_MT) || defined(_DLL)))
/*
 * Multi-thread (MT) environment
 */
EXTERN_C int DLL_DECL *__rfio_errno _PROTO((void));

/*
 * Thread safe rfio_errno. Note, __rfio_errno is defined in Cglobals.c rather
 * than rfio/error.c .
 */
#define rfio_errno (*__rfio_errno())

#else /* _REENTRANT || _THREAD_SAFE ... */
/*
 * non-MT environment
 */
extern  int     rfio_errno;                 /* Global error number          */
#endif /* _REENTRANT || _TREAD_SAFE */

EXTERN_C char DLL_DECL *rfio_errmsg_r _PROTO((int, int, char*, size_t));
EXTERN_C char DLL_DECL *rfio_errmsg _PROTO((int, int));
EXTERN_C char DLL_DECL *rfio_serror_r _PROTO((char*, size_t));
EXTERN_C char DLL_DECL *rfio_serror _PROTO((void));
EXTERN_C void DLL_DECL rfio_perror _PROTO((char *));

#endif /* _RFIO_ERRNO_H_INCLUDED_ */

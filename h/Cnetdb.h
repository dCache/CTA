/*
 * Copyright (C) 1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

/*
 * $RCSfile: Cnetdb.h,v $ $Revision: 1.9 $ $Date: 2000/05/12 15:57:50 $ CERN IT-PDP/DM Olof Barring
 */


#ifndef _CNETDB_H
#define _CNETDB_H

#include <osdep.h>

#if defined(_REENTRANT) || defined(_THREAD_SAFE)
#if defined(hpux) || defined(HPUX10) || defined(sgi) || defined(SOLARIS)
EXTERN_C int DLL_DECL *C__h_errno _PROTO((void));
#define h_errno (*C__h_errno())
#endif /* hpux || HPUX10 || sgi || SOLARIS */
#endif /* _REENTRANT || _THREAD_SAFE */


EXTERN_C struct hostent DLL_DECL *Cgethostbyname _PROTO((const char *));
EXTERN_C struct hostent DLL_DECL *Cgethostbyaddr _PROTO((const void *, size_t, int));
EXTERN_C struct servent DLL_DECL *Cgetservbyname _PROTO((const char *, const char *));

#endif /* _CNETDB_H */

/*
 * $Id: rfio_rfilefdt.h,v 1.2 2000/11/20 14:58:03 jdurand Exp $
 */

#ifndef __rfio_rfilefdt_h
#define __rfio_rfilefdt_h

EXTERN_C int rfio_rfilefdt_allocentry (int);
#define FINDRFILE_WITH_SCAN     1
#define FINDRFILE_WITHOUT_SCAN  0
EXTERN_C int rfio_rfilefdt_findentry (int, int);
EXTERN_C int rfio_rfilefdt_findptr (RFILE *, int);
EXTERN_C int rfio_rfilefdt_freeentry (int);

#endif /* __rfio_rfilefdt_h */

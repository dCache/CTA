/*
 * Copyright (C) 1999 by CERN IT-PDP/DM
 * All rights reserved
 */


/*
 * $RCSfile: rtcp_api.h,v $ $Revision: 1.6 $ $Date: 2000/02/08 16:08:13 $ CERN IT-PDP/DM Olof Barring
 */

/*
 * rtcp_api.h - rtcopy client API prototypes
 */

#if !defined(RTCP_API_H)
#define RTCP_API_H

#include <osdep.h>
#include <Ctape_constants.h>
#include <rtcp_constants.h>
#include <rtcp.h>

EXTERN_C int DLL_DECL rtcpc _PROTO((tape_list_t *));
EXTERN_C int DLL_DECL rtcpcCallTMS _PROTO((tape_list_t *));
EXTERN_C void DLL_DECL rtcp_SetErrTxt _PROTO((int, char *, ...));
EXTERN_C int DLL_DECL rtcpc_BuildReq _PROTO((tape_list_t **, int, char **));
EXTERN_C int DLL_DECL rtcp_RetvalSHIFT _PROTO((tape_list_t *, file_list_t *, int *));
EXTERN_C void DLL_DECL rtcpc_FreeReqLists _PROTO((tape_list_t **));
EXTERN_C int DLL_DECL dumpTapeReq _PROTO((tape_list_t *));
EXTERN_C int DLL_DECL dumpFileReq _PROTO((file_list_t *));

#endif /* RTCP_API_H */


/*
 * $Id: stager_catalogInterface.h,v 1.8 2004/11/23 16:31:46 jdurand Exp $
 */

#ifndef __stager_catalogInterface_h
#define __stager_catalogInterface_h

#include "osdep.h"
#include "castor/stager/CastorFile.h"
#include "castor/stager/IStagerSvc.h"
#include "castor/stager/Request.h"
#include "castor/stager/SubRequest.h"
#include "castor/stager/SubRequestStatusCodes.h"
#include "castor/stager/FileRequest.h"
#include "castor/stager/SvcClass.h"
#include "castor/stager/FileClass.h"
#include "castor/Services.h"
#include "castor/Constants.h"
#include "castor/BaseAddress.h"
#include "castor/rh/Response.h"
#include "castor/rh/FileResponse.h"
#include "castor/rh/IOResponse.h"
#include "castor/replier/RequestReplier.h"

EXTERN_C int DLL_DECL stager_getDbSvc _PROTO((struct C_Services_t ***));
EXTERN_C int DLL_DECL stager_getStgAndDbSvc _PROTO((struct Cstager_IStagerSvc_t ***, struct C_Services_t ***));

#endif /* __stager_catalogInterface_h */

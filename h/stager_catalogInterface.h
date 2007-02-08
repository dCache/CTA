/*
 * $Id: stager_catalogInterface.h,v 1.23 2007/02/08 07:31:05 gtaur Exp $
 */

#ifndef __stager_catalogInterface_h
#define __stager_catalogInterface_h

#include "osdep.h"
#include "castor/stager/CastorFile.h"
#include "castor/stager/IStagerSvc.h"
#include "castor/stager/IJobSvc.h"
#include "castor/stager/IFSSvc.h"
#include "castor/stager/Request.h"
#include "castor/stager/SubRequest.h"
#include "castor/stager/SubRequestStatusCodes.h"
#include "castor/stager/FileRequest.h"
#include "castor/stager/SetFileGCWeight.h"
#include "castor/stager/DiskServer.h"
#include "castor/stager/DiskServerStatusCode.h"
#include "castor/stager/DiskPool.h"
#include "castor/stager/SvcClass.h"
#include "castor/stager/FileClass.h"
#include "castor/stager/FileSystem.h"
#include "castor/stager/FileSystemStatusCodes.h"
#include "castor/stager/DiskCopy.h"
#include "castor/stager/DiskCopyStatusCodes.h"
#include "castor/stager/DiskCopyForRecall.h"
#include "castor/Services.h"
#include "castor/Constants.h"
#include "castor/BaseAddress.h"
#include "castor/IObject.h"
#include "castor/IAddress.h"
#include "castor/rh/Response.h"
#include "castor/rh/FileResponse.h"
#include "castor/rh/IOResponse.h"
#include "castor/rh/Client.h"
#include "castor/replier/RequestReplier.h"

EXTERN_C int DLL_DECL stager_getDbSvc _PROTO((struct C_Services_t ***));
EXTERN_C int DLL_DECL stager_getStgAndDbSvc _PROTO((struct Cstager_IStagerSvc_t ***, struct C_Services_t ***));
EXTERN_C int DLL_DECL stager_getJobSvc _PROTO((struct Cstager_IJobSvc_t ***));


#endif /* __stager_catalogInterface_h */

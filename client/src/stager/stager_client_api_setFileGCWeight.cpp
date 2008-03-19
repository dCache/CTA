/*
 * $Id: stager_client_api_setFileGCWeight.cpp,v 1.9 2008/03/19 14:53:45 itglp Exp $
 */

/*
 * Copyright (C) 2005 by CERN/IT/ADC/CA
 * All rights reserved
 */

/* ============== */
/* System headers */
/* ============== */
#include <sys/types.h>
#include <sys/stat.h>

/* ============= */
/* Local headers */
/* ============= */
#include "errno.h"
#include "stager_client_api.h"
#include "stager_client_api_common.hpp"
#include "castor/BaseObject.hpp"
#include "castor/Constants.hpp"
#include "castor/client/VectorResponseHandler.hpp"
#include "castor/client/BaseClient.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/FileRequest.hpp"
#include "castor/stager/SetFileGCWeight.hpp"
#include "castor/rh/Response.hpp"
#include "castor/rh/FileResponse.hpp"

// To be removed when getting rid of 
// request printing
#include "castor/ObjectSet.hpp"


/* ================= */
/* Internal routines */
/* ================= */
static int _processFileRequest(const char *func,
                               castor::stager::SetFileGCWeight& req,
                               struct stage_filereq *requests,
                               int nbreqs,
                               float weight,
                               struct stage_fileresp **responses,
                               int *nbresps,
                               char **requestId,
                               struct stage_options* opts) {
  int ret=0;
  if (requests == NULL
      || nbreqs <= 0
      || responses == NULL
      || nbresps == NULL) {
    serrno = EINVAL;
    stager_errmsg(func, "Invalid input parameter");
    return -1;
  }

  try {
    castor::BaseObject::initLog("", castor::SVC_NOMSG);

    // Uses a BaseClient to handle the request
    castor::client::BaseClient client(stage_getClientTimeout());
    ret=setDefaultOption(opts);
    client.setOption(opts, &req);
    client.setAuthorizationId(); 
    if (ret==-1){free(opts);}

    // Preparing the requests
    req.setWeight(weight);
    for(int i=0; i<nbreqs; i++) {
      castor::stager::SubRequest *subreq = new castor::stager::SubRequest();
      
      if (!(requests[i].filename)) {
        serrno = EINVAL;
        stager_errmsg(func, "filename in request %d is NULL", i);
        return -1;
      }

      req.addSubRequests(subreq);
      std::string sfilename(requests[i].filename);
      subreq->setFileName(sfilename);
      subreq->setRequest(&req);

    }

    // Using the VectorResponseHandler which stores everything in
    // A vector. BEWARE, the responses must be de-allocated afterwards
    std::vector<castor::rh::Response *>respvec;    
    castor::client::VectorResponseHandler rh(&respvec);
    std::string reqid = client.sendRequest(&req, &rh);

    if (requestId != NULL) {
      *requestId = strdup(reqid.c_str());
    }
    

    // Parsing the responses which have been stored in the vector
    int nbResponses =  respvec.size();

    if (nbResponses <= 0) {
      // We got not replies, this is not normal !
      serrno = SEINTERNAL;
      stager_errmsg(func, "No responses received");
      return -1;
    }
    

    // Creating the array of file responses
    // Same size as requests as we only do files for the moment
    *responses = (struct stage_fileresp *) malloc(sizeof(struct stage_fileresp) 
                                                  * nbResponses);
    if (*responses == NULL) {
      serrno = ENOMEM;
      stager_errmsg(func, "Could not allocate memory for responses");
      return -1;
    }
    *nbresps = nbResponses;
    

    for (int i=0; i<(int)respvec.size(); i++) {

      // Casting the response into a FileResponse !
      castor::rh::FileResponse* fr = 
        dynamic_cast<castor::rh::FileResponse*>(respvec[i]);
      if (0 == fr) {
        castor::exception::Exception e(SEINTERNAL);
        e.getMessage() << "Error in dynamic cast, response was NOT a file response";
        throw e;
      }

      (*responses)[i].filename = strdup(fr->castorFileName().c_str());
      (*responses)[i].status = fr->status();
      (*responses)[i].errorCode = fr->errorCode();
      if (fr->errorMessage().length() > 0) {
        (*responses)[i].errorMessage= strdup(fr->errorMessage().c_str());
      } else {
        (*responses)[i].errorMessage=0;
      }
      // The responses should be deallocated by the API !
      delete respvec[i];
    } // for
    
  } catch (castor::exception::Exception e) {
    serrno = e.code();
    stager_errmsg(func, (e.getMessage().str().c_str()));
    return -1;
  }
  
  return 0;
}


/* ================= */
/* External routines */
/* ================= */


////////////////////////////////////////////////////////////
//    stage_setFileGCWeight                                            //
////////////////////////////////////////////////////////////


EXTERN_C int DLL_DECL stage_setFileGCWeight(struct stage_filereq *requests,
                                            int nbreqs,
                                            float weight,
                                            struct stage_fileresp **responses,
                                            int *nbresps,
                                            char **requestId,
                                            struct stage_options* opts) {
  
  const char *func = "stage_setFileGCWeight";
  castor::stager::SetFileGCWeight req;

  return _processFileRequest(func,
                             req,
                             requests,
                             nbreqs,
                             weight,
                             responses,
                             nbresps,
                             requestId,
                             opts);
}

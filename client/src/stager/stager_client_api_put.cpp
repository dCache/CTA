/*
 * $Id: stager_client_api_put.cpp,v 1.17 2005/01/27 16:33:32 bcouturi Exp $
 */

/*
 * Copyright (C) 2004 by CERN/IT/ADC/CA
 * All rights reserved
 */

#ifndef lint
static char *sccsid = "@(#)$RCSfile: stager_client_api_put.cpp,v $ $Revision: 1.17 $ $Date: 2005/01/27 16:33:32 $ CERN IT-ADC/CA Benjamin Couturier";
#endif

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
#include "stager_admin_api.h"
#include "castor/BaseObject.hpp"
#include "castor/Constants.hpp"
#include "castor/client/VectorResponseHandler.hpp"
#include "castor/client/BaseClient.hpp"
#include "castor/stager/RequestHelper.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/StagePrepareToPutRequest.hpp"
#include "castor/stager/StagePutRequest.hpp"
#include "castor/stager/StagePutDoneRequest.hpp"
#include "castor/rh/Response.hpp"
#include "castor/rh/FileResponse.hpp"
#include "castor/rh/IOResponse.hpp"

// To be removed when getting rid of 
// request printing
#include "castor/ObjectSet.hpp"


/* ================= */
/* Internal routines */
/* ================= */

/* ================= */
/* External routines */
/* ================= */


////////////////////////////////////////////////////////////
//    stage_prepareToPut                                  //
////////////////////////////////////////////////////////////

EXTERN_C int DLL_DECL stage_prepareToPut(const char *userTag,
					struct stage_prepareToPut_filereq *requests,
					int nbreqs,
					struct stage_prepareToPut_fileresp **responses,
					int *nbresps,
					char **requestId,
					 struct stage_options* opts) {

  char *func = "stage_prepareToPut";
 
  if (requests == NULL
      || nbreqs <= 0
      || responses == NULL
      || nbresps == NULL) {
    serrno = EINVAL;
    stage_errmsg(func, "Invalid input parameter");
    return -1;
  }


  try {
    castor::BaseObject::initLog("", castor::SVC_NOMSG);
    
	
    // Uses a BaseClient to handle the request
    castor::client::BaseClient client;
    castor::stager::StagePrepareToPutRequest req;

    castor::stager::RequestHelper reqh(&req);
    reqh.setOptions(opts);

    if (0 != userTag) {
      req.setUserTag(std::string(userTag));
    }
    mode_t mask;
    umask (mask = umask(0));
    req.setMask(mask);

    // Preparing the requests
    for(int i=0; i<nbreqs; i++) {
      castor::stager::SubRequest *subreq = new castor::stager::SubRequest();
      
      if (!(requests[i].filename)) {
        serrno = EINVAL;
        stage_errmsg(func, "filename in request %d is NULL", i);
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
      stage_errmsg(func, "No responses received");
      return -1;
    }
    

    // Creating the array of file responses
    // Same size as requests as we only do files for the moment
    *responses = (struct stage_prepareToPut_fileresp *) 
      malloc(sizeof(struct stage_prepareToPut_fileresp) * nbResponses);

    if (*responses == NULL) {
      serrno = ENOMEM;
      stage_errmsg(func, "Could not allocate memory for responses");
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
    stage_errmsg(func, (char *)(e.getMessage().str().c_str()));
    return -1;
  }
  
  return 0;


}


////////////////////////////////////////////////////////////
//    stage_put                                           //
////////////////////////////////////////////////////////////


EXTERN_C int DLL_DECL stage_put(const char *userTag,
				const char *protocol,
				const char *filename,
				mode_t mode,
				u_signed64 size,
				struct stage_io_fileresp ** response,
				char **requestId,
				struct stage_options* opts) {
  
  char *func = "stage_put";

  if (0 == filename
      || 0 ==  response) {
    serrno = EINVAL;
    stage_errmsg(func, "Invalid input parameter");
    return -1;
  }
  

  try {
    castor::BaseObject::initLog("", castor::SVC_NOMSG);

    // Preparing the request
    castor::client::BaseClient client;
    castor::stager::StagePutRequest req;
    castor::stager::SubRequest *subreq = new castor::stager::SubRequest();

    castor::stager::RequestHelper reqh(&req);
    reqh.setOptions(opts);

    if (0 != userTag) {
      req.setUserTag(std::string(userTag));
    }
    mode_t mask;
    umask (mask = umask(0));
    req.setMask(mask);

    if (0 != protocol) {
      subreq->setProtocol(protocol);
    }

    subreq->setFileName(filename);
    subreq->setXsize(size);
    subreq->setModeBits(mode);
    req.addSubRequests(subreq);
    subreq->setRequest(&req);

    // Submitting the request
    std::vector<castor::rh::Response *>respvec;    
    castor::client::VectorResponseHandler rh(&respvec);
    std::string reqid = client.sendRequest(&req, &rh);
    if (requestId != NULL) {
      *requestId = strdup(reqid.c_str());
    }
    


    // Checking the result
    // Parsing the responses which have been stored in the vector
    int nbResponses =  respvec.size();

    if (nbResponses <= 0) {
      // We got not replies, this is not normal !
      serrno = SEINTERNAL;
      stage_errmsg(func, "No responses received");
      return -1;
    }
    

    // Creating the file response
    // Same size as requests as we only do files for the moment
    *response = (struct stage_io_fileresp *) 
      malloc(sizeof(struct stage_io_fileresp));
    
    if (*response == NULL) {
      serrno = ENOMEM;
      stage_errmsg(func, "Could not allocate memory for responses");
      return -1;
    }
    
    // Casting the response into a FileResponse !
    castor::rh::IOResponse* fr = 
      dynamic_cast<castor::rh::IOResponse*>(respvec[0]);
      if (0 == fr) {
        castor::exception::Exception e(SEINTERNAL);
        e.getMessage() << "Error in dynamic cast, response was NOT a file response";
        throw e;
      }
      (*response)->castor_filename = strdup(fr->castorFileName().c_str());
      (*response)->protocol = strdup(fr->protocol().c_str());
      (*response)->server = strdup(fr->server().c_str());
      (*response)->port = fr->port();
      (*response)->filename = strdup(fr->fileName().c_str());
      (*response)->status = fr->status();
      (*response)->errorCode = fr->errorCode();
      if (fr->errorMessage().length() > 0) {
        (*response)->errorMessage= strdup(fr->errorMessage().c_str());
      } else {
        (*response)->errorMessage=0;
      }
      // The responses should be deallocated by the API !
      delete respvec[0];

  } catch (castor::exception::Exception e) {
    serrno = e.code();
    stage_errmsg(func, (char *)(e.getMessage().str().c_str()));
    return -1;
  }
  
  return 0;

}


////////////////////////////////////////////////////////////
//    stage_putDone                                       //
////////////////////////////////////////////////////////////


EXTERN_C int DLL_DECL stage_putDone(char *putRequestId,
				    struct stage_filereq *requests,
                                    int nbreqs,
                                    struct stage_fileresp **responses,
                                    int *nbresps,
                                    char **requestId,
                                    struct stage_options* opts) {

  char *func = "stage_putDone";
 
  if (requests == NULL
      || nbreqs <= 0
      || responses == NULL
      || nbresps == NULL) {
    serrno = EINVAL;
    stage_errmsg(func, "Invalid input parameter");
    return -1;
  }


  try {
    castor::BaseObject::initLog("", castor::SVC_NOMSG);
    
	
    // Uses a BaseClient to handle the request
    castor::client::BaseClient client;
    castor::stager::StagePutDoneRequest req;

    castor::stager::RequestHelper reqh(&req);
    reqh.setOptions(opts);

    // Setting the request ID if specified
    if (putRequestId != NULL) {
      req.setParentUuid(putRequestId);
    }

    // Setting the mask on the request
    mode_t mask;
    umask (mask = umask(0));
    req.setMask(mask);

    // Preparing the requests
    for(int i=0; i<nbreqs; i++) {
      castor::stager::SubRequest *subreq = new castor::stager::SubRequest();
      
      if (!(requests[i].filename)) {
        serrno = EINVAL;
        stage_errmsg(func, "filename in request %d is NULL", i);
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
      stage_errmsg(func, "No responses received");
      return -1;
    }
    

    // Creating the array of file responses
    // Same size as requests as we only do files for the moment
    *responses = (struct stage_fileresp *) malloc(sizeof(struct stage_fileresp) 
                                                  * nbResponses);
    if (*responses == NULL) {
      serrno = ENOMEM;
      stage_errmsg(func, "Could not allocate memory for responses");
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
    stage_errmsg(func, (char *)(e.getMessage().str().c_str()));
    return -1;
  }
  
  return 0;
}

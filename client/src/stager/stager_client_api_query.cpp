/*
 * $Id: stager_client_api_query.cpp,v 1.14 2006/02/07 10:34:10 itglp Exp $
 */

/*
 * Copyright (C) 2004 by CERN/IT/ADC/CA
 * All rights reserved
 */

#ifndef lint
static char *sccsid = "@(#)$RCSfile: stager_client_api_query.cpp,v $ $Revision: 1.14 $ $Date: 2006/02/07 10:34:10 $ CERN IT-ADC/CA Benjamin Couturier";
#endif

/* ============== */
/* System headers */
/* ============== */

/* ============= */
/* Local headers */
/* ============= */
#include "errno.h"
#include "stager_client_api.h"
#include "stager_admin_api.h"
#include "castor/BaseObject.hpp"
#include "castor/Constants.hpp"
#include "castor/client/IResponseHandler.hpp"
#include "castor/client/BaseClient.hpp"
#include "castor/client/VectorResponseHandler.hpp"
#include "castor/rh/FileQryResponse.hpp"
#include "castor/rh/RequestQueryResponse.hpp"
#include "castor/stager/StageRequestQueryRequest.hpp"
#include "castor/stager/StageFileQueryRequest.hpp"
#include "castor/stager/QueryParameter.hpp"
#include "castor/stager/SubRequest.hpp"
#include "stager_client_api_common.h"
#include "castor/stager/RequestHelper.hpp"

/* ================= */
/* External routines */
/* ================= */



////////////////////////////////////////////////////////////
//    stage_filequery                                     //
////////////////////////////////////////////////////////////


EXTERN_C int DLL_DECL stage_filequery(struct stage_query_req *requests,
				      int nbreqs,
				      struct stage_filequery_resp **responses,
				      int *nbresps,
				      struct stage_options* opts){

  
  char *func = "stage_filequery";
 
  if (requests == NULL
      || nbreqs <= 0
      || responses == NULL
      || nbresps == NULL) {
    serrno = EINVAL;
    stage_errmsg(func, "Invalid input parameter");
    return -1;
  }

  stage_trace(3, "%s", func);
  
  try {
    castor::BaseObject::initLog("", castor::SVC_NOMSG);
    
    // Uses a BaseClient to handle the request
    castor::client::BaseClient client(stage_getClientTimeout());
    castor::stager::StageFileQueryRequest req;

    castor::stager::RequestHelper reqh(&req);
    reqh.setOptions(opts);

    
    // Preparing the requests
    for(int i=0; i<nbreqs; i++) {
      
      if (!(requests[i].param)) {
        serrno = EINVAL;
        stage_errmsg(func, "Parameter in request %d is NULL", i);
        return -1;
      }
      
      castor::stager::QueryParameter *par = new castor::stager::QueryParameter();
      par->setQueryType((castor::stager::RequestQueryType)(requests[i].type));
      par->setValue((const char *)requests[i].param);
      par->setQuery(&req);
      req.addParameters(par);
      
      stage_trace(3, "%s type=%d param=%s", 
		  func, requests[i].type, requests[i].param);
    }

    // Using the VectorResponseHandler which stores everything in
    // a vector. BEWARE, the responses must be de-allocated afterwards
    std::vector<castor::rh::Response*>respvec;    
    castor::client::VectorResponseHandler rh(&respvec);
    client.sendRequest(&req, &rh);

    // Parsing the responses which have been stored in the vector
    int nbResponses =  respvec.size();

    if (nbResponses <= 0) {
      // We got no replies, this is not normal !
      serrno = SEINTERNAL;
      stage_errmsg(func, "No responses received");
      return -1;
    }
    

    // Creating the array of file responses
    // Same size as requests as we only do files for the moment
    *responses = (struct stage_filequery_resp *) 
      malloc(sizeof(struct stage_filequery_resp) * nbResponses);

    if (*responses == NULL) {
      serrno = ENOMEM;
      stage_errmsg(func, "Could not allocate memory for responses");
      return -1;
    }
    *nbresps = nbResponses;
    
    for (int i=0; i<(int)respvec.size(); i++) {

      // Casting the response into a FileResponse
      castor::rh::FileQryResponse* fr = 
        dynamic_cast<castor::rh::FileQryResponse*>(respvec[i]);
      if (0 == fr) {
          castor::exception::Exception e(SEINTERNAL);
          e.getMessage() << "Error in dynamic cast, response was NOT a FileQryResponse";
          throw e;
      }

      (*responses)[i].errorCode = fr->errorCode();
      (*responses)[i].errorMessage = strdup(fr->errorMessage().c_str());
      (*responses)[i].filename = strdup(fr->fileName().c_str());
      (*responses)[i].castorfilename = strdup(fr->castorFileName().c_str());
      (*responses)[i].fileid = fr->fileId();
      (*responses)[i].status = fr->status();
      (*responses)[i].size = fr->size();
      (*responses)[i].diskserver = strdup(fr->diskServer().c_str());
      (*responses)[i].poolname = strdup(fr->poolName().c_str());
      (*responses)[i].creationTime = (time_t)fr->creationTime();
      (*responses)[i].accessTime = (time_t)fr->accessTime();
      (*responses)[i].nbAccesses = fr->nbAccesses();

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
//    stage_requestquery                                  //
////////////////////////////////////////////////////////////

EXTERN_C int DLL_DECL stage_requestquery(struct stage_query_req *requests,
					 int nbreqs,
					 struct stage_requestquery_resp **responses,
					 int *nbresps,
					 struct stage_options* opts) {


   char *func = "stage_requestquery";
 
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
    castor::client::BaseClient client(stage_getClientTimeout());
    castor::stager::StageRequestQueryRequest req;
    
    castor::stager::RequestHelper reqh(&req);
    reqh.setOptions(opts);
    
    // Preparing the requests
    for(int i=0; i<nbreqs; i++) {
      
      if (!(requests[i].param)) {
        serrno = EINVAL;
        stage_errmsg(func, "Parameter in request %d is NULL", i);
        return -1;
      }
      
      castor::stager::QueryParameter *par = new castor::stager::QueryParameter();
      // beware, here the order in enum query_type and RequestQueryType must match
      par->setQueryType((castor::stager::RequestQueryType)(requests[i].type)); 
      par->setValue((const char *)requests[i].param);
      req.addParameters(par);
    }

    // Using the VectorResponseHandler which stores everything in
    // A vector. BEWARE, the responses must be de-allocated afterwards
    std::vector<castor::rh::Response *>respvec;    
    castor::client::VectorResponseHandler rh(&respvec);
    client.sendRequest(&req, &rh);

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
    *responses = (struct stage_requestquery_resp *) 
      malloc(sizeof(struct stage_requestquery_resp) * nbResponses);

    if (*responses == NULL) {
      serrno = ENOMEM;
      stage_errmsg(func, "Could not allocate memory for responses");
      return -1;
    }
    *nbresps = nbResponses;
    
    for (int i=0; i<(int)respvec.size(); i++) {

      // Casting the response into a FileResponse !
      castor::rh::RequestQueryResponse* fr = 
        dynamic_cast<castor::rh::RequestQueryResponse*>(respvec[i]); 
      if (0 == fr) {
        castor::exception::Exception e(SEINTERNAL);
        e.getMessage() << "Error in dynamic cast, response was NOT a file query response";
        throw e;
      }
      // XXX

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
//    stage_findrequest                                   //
////////////////////////////////////////////////////////////


EXTERN_C int DLL_DECL stage_findrequest(struct stage_query_req *requests,
					int nbreqs,
					struct stage_findrequest_resp **responses,
					int *nbresps,
					struct stage_options* opts){
  serrno = SEOPNOTSUP;
  return -1;
}


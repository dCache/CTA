/*
 * $Id: QueryRequestSvcThread.cpp,v 1.1 2004/12/17 15:58:25 bcouturi Exp $
 */

/*
 * Copyright (C) 2004 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char *sccsid = "@(#)$RCSfile: QueryRequestSvcThread.cpp,v $ $Revision: 1.1 $ $Date: 2004/12/17 15:58:25 $ CERN IT-ADC/CA Ben Couturier";
#endif

/* ================================================================= */
/* Local headers for threads : to be included before ANYTHING else   */
/* ================================================================= */
#include "Cthread_api.h"
#include "Cmutex.h"

/* ============== */
/* System headers */
/* ============== */
#include <list>
#include <vector>

/* ============= */
/* Local headers */
/* ============= */
#include "castor/Services.hpp"
#include "castor/Constants.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/IObject.hpp"
#include "castor/IClient.hpp"
#include "castor/IService.hpp"
#include "castor/stager/IStagerSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/BaseObject.hpp"
#include "castor/replier/RequestReplier.hpp"
#include "castor/stager/StageFileQueryRequest.hpp"
#include "castor/stager/StageFindRequestRequest.hpp"
#include "castor/stager/StageRequestQueryRequest.hpp"
#include "castor/rh/BasicResponse.hpp"

#undef logfunc

#include "stager_query_service.h"
#include "stager_macros.h"
#include "serrno.h"

#undef NULL
#define NULL 0

/* ------------------------------------- */
/* stager_query_select()                   */
/*                                       */
/* Purpose: Query 'select' part of service */
/*                                       */
/* Input:  n/a                           */
/*                                       */
/* Output:  (void **) output   Selected  */
/*                                       */
/* Return: 0 [OK] or -1 [ERROR, serrno]  */
/* ------------------------------------- */



EXTERN_C int DLL_DECL stager_query_select(void **output) {
  char *func =  "stager_query_select";
  int rc;

  STAGER_LOG_ENTER();

  if (output == NULL) {
    serrno = EFAULT;
    return -1;
  }

  castor::stager::Request* req = 0;
  castor::Services *svcs;
  castor::stager::IStagerSvc *stgSvc;

  try {

    /* Loading services */
    /* ---------------- */
    STAGER_LOG_DEBUG(NULL,"Loading services");
    svcs = castor::BaseObject::services();
    castor::IService* svc =
      svcs->service("OraStagerSvc", castor::SVC_ORASTAGERSVC);
    stgSvc = dynamic_cast<castor::stager::IStagerSvc*>(svc);


    /* Get any new request to do    */
    /* ---------------------------- */
    STAGER_LOG_DEBUG(NULL,"Getting any request to do");
    std::vector<castor::ObjectsIds> types;
    types.push_back(castor::OBJ_StageFileQueryRequest);
    types.push_back(castor::OBJ_StageFindRequestRequest);
    types.push_back(castor::OBJ_StageRequestQueryRequest);
    castor::stager::Request* req = stgSvc->requestToDo(types);

    if (0 == req) {
      /* Nothing to do */
      STAGER_LOG_DEBUG(NULL,"Nothing to do");
      serrno = ENOENT;
      rc = -1;
    } else {
      STAGER_LOG_DEBUG(NULL,"req FOUND");
      *output = req;
      rc = 0;
    }

  } catch (castor::exception::Exception e) {
    serrno = e.code();
    STAGER_LOG_DB_ERROR(NULL,"stager_query_select",
                        e.getMessage().str().c_str());
    rc = -1;
    if (req) delete req;
  }

  // Cleanup
  stgSvc->release();

  // Return
  STAGER_LOG_RETURN(rc);
}


namespace castor {

  namespace stager {

    namespace queryService {

      /**
       * Sends a Response to a client
       * In case of error, on writes a message to the log
       * @param client the client where to send the response
       * @param res the response to send
       */
      void replyToClient(castor::IClient* client,
			 castor::rh::Response* res) {
	char *func =  "castor::stager::replyToClient";
	try {
	  STAGER_LOG_DEBUG(NULL, "Sending Response");
	  castor::replier::RequestReplier *rr =
	    castor::replier::RequestReplier::getInstance();
	  rr->sendResponse(client, res);
	  rr->sendEndResponse(client);
	} catch (castor::exception::Exception e) {
	  serrno = e.code();
	  STAGER_LOG_DB_ERROR(NULL, func,
			      e.getMessage().str().c_str());
	}
      }

      /**
       * Handles a fileQueryRequest and replies to client.
       * @param req the request to handle
       * @param client the client where to send the response
       * @param svcs the Services object to use
       * @param stgSvc the stager service to use
       * @param ad the address where to load/store objects in the DB
       */
      void handle_fileQueryRequest(castor::stager::Request* req,
				   castor::IClient *client,
				   castor::Services* svcs,
				   castor::stager::IStagerSvc* stgSvc,
				   castor::BaseAddress &ad) {
	char *func = "handle_fileQueryRequest";
	STAGER_LOG_DEBUG(NULL,"Handling fileQueryRequest");
      }
    
      /**
       * Handles a findRequestRequest and replies to client.
       * @param req the request to handle
       * @param client the client where to send the response
       * @param svcs the Services object to use
       * @param stgSvc the stager service to use
       * @param ad the address where to load/store objects in the DB
       */
      void handle_findRequestRequest(castor::stager::Request* req,
				     castor::IClient *client,
				     castor::Services* svcs,
				     castor::stager::IStagerSvc* stgSvc,
				     castor::BaseAddress &ad) {
	char *func = "handle_findRequestRequest";
	STAGER_LOG_DEBUG(NULL,"Handling findRequestRequest");
      }
    
      /**
       * Handles a requestQueryRequest and replies to client.
       * @param req the request to handle
       * @param client the client where to send the response
       * @param svcs the Services object to use
       * @param stgSvc the stager service to use
       * @param ad the address where to load/store objects in the DB
       */
      void handle_requestQueryRequest(castor::stager::Request* req,
				      castor::IClient *client,
				      castor::Services* svcs,
				      castor::stager::IStagerSvc* stgSvc,
				      castor::BaseAddress &ad) {
	char *func = "handle_requestQueryRequest";
	STAGER_LOG_DEBUG(NULL,"Handling requestQueryRequest");
      }

    } // End of namespace query service

  } // End of namespace stager

} // End of namespace castor


/* --------------------------------------- */
/* stager_query_process()                  */
/*                                         */
/* Purpose:Query 'process' part of service */
/*                                         */
/* Input:  (void *) output    Selection    */
/*                                         */
/* Output: n/a                             */
/*                                         */
/* Return: 0 [OK] or -1 [ERROR, serrno]    */
/* --------------------------------------- */
EXTERN_C int DLL_DECL stager_query_process(void *output) {

  char *func =  "stager_query_process";
  STAGER_LOG_ENTER();

  serrno = 0;
  if (output == NULL) {
    serrno = EFAULT;
    return -1;
  }

  /* ===========================================================
   *
   * 1) PREPARATION PHASE
   * Retrieving request from the database
   */

  castor::stager::Request* req = 0;
  castor::Services *svcs = 0;
  castor::stager::IStagerSvc *stgSvc = 0;
  castor::IClient *client = 0;

  /* Setting the address to access Oracle */
  /* ------------------------------------ */
  castor::BaseAddress ad;
  ad.setCnvSvcName("OraCnvSvc");
  ad.setCnvSvcType(castor::SVC_ORACNV);

  try {

    /* Loading services */
    /* ---------------- */
    STAGER_LOG_DEBUG(NULL,"Loading services");
    svcs = castor::BaseObject::services();
    castor::IService* svc =
      svcs->service("OraStagerSvc", castor::SVC_ORASTAGERSVC);
    stgSvc = dynamic_cast<castor::stager::IStagerSvc*>(svc);

    /* Casting the request */
    /* ------------------- */
    STAGER_LOG_DEBUG(NULL, "Casting Request");
    req = (castor::stager::Request*)output;
    if (0 == req) {
      castor::exception::Internal e;
      e.getMessage() << "Request cast error";
      throw e;
    }

    /* Getting the client  */
    /* ------------------- */
    svcs->fillObj(&ad, req, castor::OBJ_IClient);
    client = req->client();
    if (0 == client) {
      castor::exception::Internal e;
      e.getMessage() << "No client associated with request ! Cannot answer !";
      throw e;
    }

  } catch (castor::exception::Exception e) {
    // If we fail here, we do NOT have enough information to
    // reply to the client !
    serrno = e.code();
    STAGER_LOG_DB_ERROR(NULL,"stager_query_select",
                        e.getMessage().str().c_str());
    if (req) delete req;
    if (stgSvc) stgSvc->release();
    return -1;
  }

  /* ===========================================================
   *
   * 2) CALLING PHASE
   * At this point we can send a reply to the client
   * We get prepared to call the method
   */

  switch (req->type()) {

  case castor::OBJ_StageFileQueryRequest:
    castor::stager::queryService::handle_fileQueryRequest
      (req, client, svcs, stgSvc, ad);
    break;

  case castor::OBJ_StageFindRequestRequest:
    castor::stager::queryService::handle_findRequestRequest
      (req, client, svcs, stgSvc, ad);
    break;

  case castor::OBJ_StageRequestQueryRequest:
    castor::stager::queryService::handle_requestQueryRequest
      (req, client, svcs, stgSvc, ad);
    break;
 
  default:
    castor::exception::Internal e;
    e.getMessage() << "Unknown Request type : "
                   << castor::ObjectsIdStrings[req->type()];
    if (req) delete req;
    if (stgSvc) stgSvc->release();
    throw e;
  }

  // Cleanup
  if (req) delete req;
  if (stgSvc) stgSvc->release();
  STAGER_LOG_RETURN(serrno == 0 ? 0 : -1);
}

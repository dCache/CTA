
/************************************************************************************************/
/* StagerPutDoneHandler: Constructor and implementation of the PutDone request's handle        */
/******************************************************************************************** */

#include "castor/stager/dbService/StagerRequestHelper.hpp"
#include "castor/stager/dbService/StagerCnsHelper.hpp"
#include "castor/stager/dbService/StagerReplyHelper.hpp"


#include "castor/Services.hpp"
#include "castor/BaseObject.hpp"
#include "castor/IService.hpp"
#include "castor/stager/IJobSvc.hpp"


#include "castor/stager/dbService/StagerRequestHandler.hpp"
#include "castor/stager/dbService/StagerJobRequestHandler.hpp"
#include "castor/stager/dbService/StagerPutDoneHandler.hpp"

#include "stager_uuid.h"
#include "stager_constants.h"
#include "castor/Constants.hpp"
#include "Cns_api.h"
#include "expert_api.h"

#include "Cpwd.h"
#include "Cgrp.h"
#include "castor/IClientFactory.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"
#include "castor/stager/DiskCopyForRecall.hpp"

#include "castor/exception/Exception.hpp"

#include "serrno.h"
#include <errno.h>

#include <iostream>
#include <string>








namespace castor{
  namespace stager{
    namespace dbService{
      
      StagerPutDoneHandler::StagerPutDoneHandler(StagerRequestHelper* stgRequestHelper, StagerCnsHelper* stgCnsHelper) throw(castor::exception::Exception)
      {     	
	this->stgRequestHelper = stgRequestHelper;
	this->stgCnsHelper = stgCnsHelper;
	this->typeRequest = OBJ_StagePutDoneRequest;

	
      }

      void StagerPutDoneHandler::handle() throw(castor::exception::Exception)
      {
	StagerReplyHelper* stgReplyHelper= NULL;
	try{

	  /**************************************************************************/
	  /* common part for all the handlers: get objects, link, check/create file*/
	  preHandle();
	  /**********/
	 
	  jobOriented();/* until it will be explored */
	 
	  
	  if(stgRequestHelper->stagerService->processPutDone(stgRequestHelper->subrequest)){
	 
	    /* for the PutDone, if everything is ok, we archive the subrequest */
	    stgRequestHelper->stagerService->archiveSubReq(stgRequestHelper->subrequest->id());
	    
	    /* replyToClient Part: */
	    stgReplyHelper = new StagerReplyHelper(SUBREQUEST_READY);
	    if(stgReplyHelper == NULL){
	      castor::exception::Exception ex(SEINTERNAL);
	      ex.getMessage()<<"(StagerRepackHandler handle) Impossible to get the StagerReplyHelper"<<std::endl;
	      throw ex;
	    }
	    stgReplyHelper->setAndSendIoResponse(stgRequestHelper,stgCnsHelper->cnsFileid,0, "No error");
	    stgReplyHelper->endReplyToClient(stgRequestHelper);
	    delete stgReplyHelper->ioResponse;
	    delete stgReplyHelper;
	  }

	}catch(castor::exception::Exception e){
	  if(stgReplyHelper != NULL){
	    if(stgReplyHelper->ioResponse != NULL) delete stgReplyHelper->ioResponse;
	    delete stgReplyHelper;
	  }
	  castor::exception::Exception ex(e.code());
	  ex.getMessage()<<"(StagerPutDoneHandler) Error"<<e.getMessage().str()<<std::endl;
	  throw ex;
	}
      }





    }//end namespace dbService
  }//end namespace stager
}//end namespace castor


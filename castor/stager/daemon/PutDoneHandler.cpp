
/***************************************************************************************************/
/* StagerPutDoneHandler: Constructor and implementation of the PutDone request's handle        */
/*********************************************************************************************** */

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
#include "serrno.h"
#include "Cns_api.h"
#include "expert_api.h"

#include "Cpwd.h"
#include "Cgrp.h"
#include "castor/IClientFactory.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"
#include "castor/stager/DiskCopyForRecall.hpp"

#include "castor/exception/Exception.hpp"

#include <iostream>
#include <string>








namespace castor{
  namespace stager{
    namespace dbService{
      
      StagerPutDoneHandler::StagerPutDoneHandler(StagerRequestHelper* stgRequestHelper, StagerCnsHelper* stgCnsHelper) throw(castor::exception::Exception)
      {     	
	this->stgRequestHelper = stgRequestHelper;
	this->stgCnsHelper = stgCnsHelper;	

	this->currentSubrequestStatus = stgRequestHelper->subrequest->status();
      }

      void StagerPutDoneHandler::handle() throw(castor::exception::Exception)
      {
	StagerReplyHelper* stgReplyHelper= NULL;
	try{

	 
	  jobOriented();/* until it will be explored */
	 
	  /* ask about the state of the sources */
	  stgRequestHelper->stagerService->isSubRequestToSchedule((stgRequestHelper->subrequest), this->sources);
	  
	  if(sources.size()<=0){
	    castor::exception::Exception ex(EPERM);
	    ex.getMessage()<<"(StagerPutDoneHandler handle) PutDone without a Put (sources.size <0)"<<std::endl;
	    throw ex;
	  }
	  


	  castor::IService* svc = castor::BaseObject::services()->service("JobSvc", castor::SVC_DBJOBSVC);
	  if (0 == svc) {
	    castor::exception::Exception ex(SEINTERNAL);
	    ex.getMessage()<<"(StagerPutDonHandler handle) Impossible to get the jobService"<<std::endl;
	    throw ex;
	  }
	  this->jobService = dynamic_cast<castor::stager::IJobSvc*>(svc);
	  if (0 == this->jobService) {
	    castor::exception::Exception ex(SEINTERNAL);
	    ex.getMessage()<<"(StagerPutDoneHandler handle) Got a bad jobService"<<std::endl;
	    throw ex;
	  }
	  
	  jobService->prepareForMigration(stgRequestHelper->subrequest,0,0);
	  
	 
	  /* for the PutDone, if everything is ok, we archive the subrequest */
	  stgRequestHelper->stagerService->archiveSubReq(stgRequestHelper->subrequest->id());
	 
	  /* we never change the subrequestStatus, but we need the newSubrequestStatus for the replyToClient */	  
	  this->newSubrequestStatus = SUBREQUEST_READY;
	  
	  /* replyToClient Part: */
	  stgReplyHelper = new StagerReplyHelper(this->newSubrequestStatus);
	  if(stgReplyHelper == NULL){
	    castor::exception::Exception ex(SEINTERNAL);
	    ex.getMessage()<<"(StagerRepackHandler handle) Impossible to get the StagerReplyHelper"<<std::endl;
	    throw ex;
	  }
	  stgReplyHelper->setAndSendIoResponse(stgRequestHelper,stgCnsHelper->fileid,0, "No error");
	  stgReplyHelper->endReplyToClient(stgRequestHelper);
	  delete stgReplyHelper->ioResponse;
	  delete stgReplyHelper;
	   
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


      StagerPutDoneHandler::~StagerPutDoneHandler() throw()
      {
      }



    }//end namespace dbService
  }//end namespace stager
}//end namespace castor


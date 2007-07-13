/***********************************************************************************************************/
/* StagerPrepareToUpdatetHandler: Constructor and implementation of the PrepareToUpdate request handler   */
/* Since it is jobOriented, it uses the mostly part of the StagerJobRequestHandler class                 */
/* Depending if the file exist, it can follow the huge flow (scheduled, as Get) or a small one (as Put) */
/* We need to reply to the client (just in case of error )                                             */
/******************************************************************************************************/



#include "castor/stager/dbService/StagerCnsHelper.hpp"
#include "castor/stager/dbService/StagerReplyHelper.hpp"

#include "castor/stager/dbService/StagerRequestHandler.hpp"
#include "castor/stager/dbService/StagerJobRequestHandler.hpp"
#include "castor/stager/dbService/StagerPrepareToUpdateHandler.hpp"

#include "stager_uuid.h"
#include "stager_constants.h"
#include "serrno.h"
#include "Cns_api.h"
#include "expert_api.h"
#include "rm_api.h"
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
     
      StagerPrepareToUpdateHandler::StagerPrepareToUpdateHandler(StagerRequestHelper* stgRequestHelper, StagerCnsHelper* stgCnsHelper, bool toRecreateCastorFile) throw(castor::exception::Exception)
      {
	this->stgRequestHelper = stgRequestHelper;
	this->stgCnsHelper = stgCnsHelper;
	
		
	/* depending on this flag, we ll execute the huge flow or the small one*/
	this->toRecreateCastorFile = toRecreateCastorFile;

	this->maxReplicaNb = this->stgRequestHelper->svcClass->maxReplicaNb();
	this->replicationPolicy = this->stgRequestHelper->svcClass->replicationPolicy();
	
	
	this->useHostlist = false;
#ifdef USE_HOSTLIST
	this->useHostlist=true;
#endif


	/* get the subrequest's size required on disk */
	this->xsize = this->stgRequestHelper->subrequest->xsize();
	if(this->xsize > 0){
	  
	  if( this->xsize < (this->stgCnsHelper->cnsFilestat.filesize) ){
	    /* print warning! */
	  }
	}else{
	  /* we get the defaultFileSize */
	  xsize = stgRequestHelper->svcClass->defaultFileSize();
	  if( xsize <= 0){
	    xsize = DEFAULTFILESIZE;
	  }
	}
	
	
	this->openflags=RM_O_RDWR; /* since we aren't gonna use the rm, we don't really need it */
	this->default_protocol = "rfio";

	this->caseSubrequestFailed = false;	
      }


      
      /****************************************************************************************/
      /* handler for the PrepareToUpdate request  */
      /****************************************************************************************/
      void StagerPrepareToUpdateHandler::handle() throw(castor::exception::Exception)
      {
	/**/
	try{
	  jobOriented();

	  
	  int caseToSchedule;
	  /*************************/
	  /* huge or small flow?? */
	  /***********************/
	  
	  if( toRecreateCastorFile){/* we recreate, small flow*/
	    castor::stager::DiskCopyForRecall* diskCopyForRecall = stgRequestHelper->stagerService->recreateCastorFile(stgRequestHelper->castorFile,stgRequestHelper->subrequest);
	    
	    if(diskCopyForRecall == NULL){
	      /* we don't archiveSubrequest, changeSubrequestStatus or replyToClient */
	      this->caseSubrequestFailed = true;
	    }
	    
	  }else{/* we schedule, huge flow */
	    
	    caseToSchedule = stgRequestHelper->stagerService->isSubRequestToSchedule(stgRequestHelper->subrequest, this->sources);
	    switchScheduling(caseToSchedule);
	    
	    /* build the rmjob struct and submit the job */
	    stgRequestHelper->buildRmJobHelperPart(&(this->rmjob)); /* add euid, egid... on the rmjob struct  */
	    buildRmJobRequestPart();/* add rfs and hostlist strings on the rmjob struct */
	    if(rm_enterjob(NULL,-1,(u_signed64) 0, &(this->rmjob), &(this->nrmjob_out), &(this->rmjob_out)) != 0){
	      castor::exception::Exception ex(SEINTERNAL);
	      ex.getMessage()<<"(StagerPrepareToUpdateHandler handle) Error on rm_enterjob"<<std::endl;
	      throw ex;
	    }
	    rm_freejob(this->rmjob_out);
	  }
	  
	  if(this->caseSubrequestFailed == false){
	    if(toRecreateCastorFile || ( (caseToSchedule != 2) && (caseToSchedule != 0))){
	      /* updateSubrequestStatus Part: */
	      stgRequestHelper->updateSubrequestStatus(SUBREQUEST_READY);
	    }
	    
	    /* replyToClient Part: */
	    this->stgReplyHelper = new StagerReplyHelper;
	    if((this->stgReplyHelper) == NULL){
	      castor::exception::Exception ex(SEINTERNAL);
	      ex.getMessage()<<"(StagerRepackHandler handle) Impossible to get the StagerReplyHelper"<<std::endl;
	      throw(ex);
	    }
	    this->stgReplyHelper->setAndSendIoResponse(stgRequestHelper,stgCnsHelper->fileid,0,"No Error");
	    this->stgReplyHelper->endReplyToClient(stgRequestHelper);
	    delete stgReplyHelper->ioResponse;
	    delete stgReplyHelper;
	  }
	 
	 
	
	}catch(castor::exception::Exception ex){

	  if(stgReplyHelper != NULL){
	    if(stgReplyHelper->ioResponse) delete stgReplyHelper->ioResponse;
	    delete stgReplyHelper;
	  }
	  if(rmjob_out != NULL){
	    rm_freejob(this->rmjob_out);
	  }
	  stgRequestHelper->updateSubrequestStatus(SUBREQUEST_FAILED_FINISHED);
	  throw(ex);
	}
      }


      


      StagerPrepareToUpdateHandler::~StagerPrepareToUpdateHandler()throw(){
	
      }
      
      

      
    }// end dbService namespace
  }// end stager namespace
}//end castor namespace 


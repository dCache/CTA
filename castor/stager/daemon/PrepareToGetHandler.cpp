/**********************************************************************************************/
/* StagerPrepareToGetHandler: Constructor and implementation of the get subrequest's handler */
/* It inherits from the StagerJobRequestHandler and it needs to reply to the client         */
/*******************************************************************************************/


#include "castor/stager/dbService/StagerRequestHelper.hpp"
#include "castor/stager/dbService/StagerCnsHelper.hpp"
#include "castor/stager/dbService/StagerReplyHelper.hpp"

#include "castor/stager/dbService/StagerRequestHandler.hpp"
#include "castor/stager/dbService/StagerJobRequestHandler.hpp"
#include "castor/stager/dbService/StagerPrepareToGetHandler.hpp"

#include "stager_uuid.h"
#include "stager_constants.h"
#include "serno.h"
#include "Cns_api.h"
#include "expert_api.h"
#include "rm_api.h"
#include "Cpwd.h"
#include "Cgrp.h"
#include "castor/IClientFactory.h"
#include "castor/stager/SubRequestStatusCodes.hpp"

#include <iostream>
#include <string>


namespace castor{
  namespace stager{
    namespace dbService{

      StagerPrepareToGetHandler::StagerPrepareToGetHandler(StagerRequestHelper* stgRequestHelper, StagerCnsHelper* stgCnsHelper, std::string message)
      {
	this->stgRequestHelper = stgRequestHelper;
	this->stgCnsHelper = stgCnsHelper;
	/* error message needed for the exceptions, for the replyToClient... */
	this->message(message);

	this->maxReplicaNb= this->stgRequestHelper->svcClass->maxReplicaNb();
	this->replicationPolicy = this->stgRequestHelper->svcClass->replicationPolicy();


	this->useHostlist = false;
#ifdef USE_HOSTLIST
	this->useHostlist=true;
#endif
	
	/* get the request's size required on disk */
	/* depending if the file exist, we ll need to update this variable */
	this->xsize = this->stgRequestHelper->subrequest->xsize();

	if( xsize > 0 ){
	  if(xsize < (stgCnsHelper->cnsFilestat.filesize)){
	    /* warning, user is requesting less bytes than the real size */
	    //just print message
	  }

	  
	}else{
	  this->xsize = stgCnsHelper->cnsFilestat.filesize;
	}


	this->openflags=RM_O_RDONLY;
	this->default_protocol = "rfio";
	
      }


      void StagerPrepareToGetHandler::handle()
      {
	/**/
	try{

	  /* job oriented part */
	  this.jobOriented();

	  /* scheduling Part */
	  /* first use the stager service to get the possible sources for the required file */
	  int caseToSchedule = stgRequestHelper->stagerService->isSubRequestToBeScheduled(stgRequestHelper->subrequest, &(this->sources));
	  
	  switch(caseToSchedule){
	    
	    
	    case 0:
	      /* we archive the subrequest, we don't need to update the subrequest status afterwards */
	      stgRequestHelper->stagerService->archiveSubrequest(stgRequestHelper->subrequest->id());
	      /// ADD!! :  change_subrequest_status = 0
	      break;

	    default:
	      /* second, process as a tapeRecall, as a replica or just change the subrequest.status */
	      /* it corresponds to the huge switch on the stager_db_service.c  */
	      switchScheduling(caseToSchedule);
	      
	      if((rfs != NULL)&&(!rfs.empty())){
		/* if the file exists we don't have any size requirements */
		this->xsize = 0;
	      }

	      /* build the rmjob struct and submit the job */
	      this->rmjob = stgRequestHelper->buildRmJobHelperPart(&(this->rmjob)); /* add euid, egid... on the rmjob struct  */
	      buildRmJobRequestPart();/* add rfs and hostlist strings on the rmjob struct */
	      rm_enterjob(NULL,-1,(u_signed64) 0, &(this->rmjob), &(this->nrmjob_out), &(this->rmjob_out)); //throw exception
	      
	      
	      
	      /* updateSubrequestStatus Part: */
	      SubRequestStatusCode currentSubrequestStatus = stgRequestHelper->subrequest->status();
	      SubRequestStatusCodes newSubrequestStatus = SUBREQUEST_READY;
	      
	      if(newSubrequestStatus != currentSubrequestStatus){//common for the StagerGetRequest
		
		stgRequestHelper->subrequest->setStatus(newSubrequestStatus);
		stgRequestHelper->subrequest->setGetNextStatus(GETNEXTSTATUSFILESTAGED);
		
	      }
	      break;
	  }


	  /* replyToClient Part: */
	  /* to take into account!!!! if an exeption happens, we need also to send the response to the client */
	  /* so copy and paste for the exceptions !!!*/
	  this->stgReplyHelper = new StagerReplyHelper*;
	  
	  this->stgReplyHelper->setAndSendIoResponse(*stgRequestHelper,stgCnsHelper->fileid,serrno, message);
	  this->stgReplyHelper->endReplyToClient(stgRequestHelper);
	  
	  
	  
	}catch{
	}
	
      }


      /********************************************************************************/
      /* we are overwritting this function inherited from the StagerJobRequestHandler */
      /* because of the case 0                                                      */
      /* after asking the stagerService is it is toBeScheduled                     */
      /* - do a normal tape recall                                                */
      /* - check if it is to replicate:                                          */
      /*         +processReplica if it is needed:                               */
      /*                    +make the hostlist if it is needed                 */
      /* now we don't have anymore the case 0 for the PrepareToGet */
      /************************************************************************/
      void StagerPrepareToGetHandler::switchScheduling(int caseToSchedule)
      {
	
	switch(caseToSchedule){

	case 2: //normal tape recall
	 
	  stgRequestHelper->stagerService->createRecallCandidate(stgRequestHelper->subrequest,(const) stgRequestHelper->filerequest->euid(), (const) stgRequestHelper->filerequest->egid());//throw exception
	  
	  break;


	case 1:	 
	  
	  bool isToReplicate=replicaSwitch();
	  
	  if(isToReplicate){

	    processReplicaAndHostlist();
	  }

	  break;

	
	default:
	  //throw exception!
	  break;
	}
	
      }//end switchScheduling
      

      /***********************************************************************/
      /*    destructor                                                      */
      StagerPrepareToGetHandler::~StagerPrepareToGetHandler()
      {
	delete stgReplyHelper;
      }
      
    }//end namespace dbservice
  }//end namespace stager
}//end namespace castor

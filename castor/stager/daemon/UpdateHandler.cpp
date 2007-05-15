/************************************************************************************************************/
/* StagerUpdateHandler: Contructor and implementation of the Update request handler                        */
/* Since it is jobOriented, it uses the mostly part of the StagerJobRequestHandler class                  */
/* Depending if the file exist, it can follow the huge flow (scheduled, as Get) or a small one (as Put)  */
/* We dont  need to reply to the client (just in case of error )                                        */
/*******************************************************************************************************/




#include "castor/stager/dbService/StagerRequestHelper.hpp"
#include "castor/stager/dbService/StagerCnsHelper.hpp"
#include "castor/stager/dbService/StagerReplyHelper.hpp"

#include "castor/stager/dbService/StagerRequestHandler.hpp"
#include "castor/stager/dbService/StagerJobRequestHandler.hpp"
#include "castor/stager/dbService/StagerUpdateHandler.hpp"

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
#include "castor/stager/DiskCopyForRecall.hpp"

#include <iostream>
#include <string>


namespace castor{
  namespace stager{
    namespace dbService{
      
      /* constructor */
      StagerUpdateHandler::StagerUpdateHandler(StagerRequestHelper* stgRequestHelper, StagerCnsHelper* stgCnsHelper, std::string message, bool toRecreateCastorFile)
      {
	this->stgRequestHelper = stgRequestHelper;
	this->stgCnsHelper = stgCnsHelper;
	this->message(message);

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
	

	this->openflags=RM_O_RDWR;
	this->default_protocol = "rfio";	
	
      }
      

      /************************************/
      /* handler for the update request  */
      /**********************************/
      void :StagerUpdateHandler::handle(void *param) throw()
      {
	/**/
	try{
	  
	  
	  this.jobOriented();


	  /* huge or small flow?? */
	  if(toRecreateCastorFile){/* we skip the processReplica part :) */


	    /* use the stagerService to recreate castor file */
	    castor::stager::DiskCopyForRecall* diskCopyForRecall = stgRequestHelper->stagerService->recreateCastorFile(stgRequestHelper->castorFile,stgRequestHelper->subrequest);
	    	    
	    if(diskCopyForRecall == NULL){
	      /*throw exception
	       there is no need to change the subrequest status!!!
	       archive_subrequest
	      */
	    }

	    /* we never replicate... we make by hand the rfs (and we don't fill the hostlist) */
	    std::string diskServername = diskCopyForRecall->diskServerName();
	    std::string mountPoint = diskCopyForRecall->mountPoint();
	    
	    if((!diskServerName.empty())&&(!mountPoint.empty())){
	      this->rfs = diskServerName + ":" + mountPoint;
	    }
	    this->hostlist.clear();
	    
	    /* since the file doesn't exist, we don't need the read flag */
	    this->openflags = RM_O_WRONLY;

	  }else{

	    int caseToSchedule = stgRequestHelper->stagerService->isSubRequestToSchedule(stgRequestHelper->subrequest,&(this->sources));
	    switchScheduling(caseToSchedule);
	    
	    /* since the file exist, we need the read flag */
	    this->openflags = RM_O_RDWR;
	  }
	  /* build the rmjob struct and submit the job */
	  this->rmjob = stgRequestHelper->buildRmJobHelperPart(&(this->rmjob)); /* add euid, egid... on the rmjob struct  */
	  buildRmJobRequestPart();/* add rfs and hostlist strings on the rmjob struct */
	  rm_enterjob(NULL,-1,(u_signed64) 0, &(this->rmjob), &(this->nrmjob_out), &(this->rmjob_out)); //throw exception
	  
	  
	  SubRequestStatusCodes currentSubrequestStatus = stgRequestHelper->subrequest->status();
	  SubRequestStatusCodes newSubrequestStatus = SUBREQUEST_READY;
	  
	  if(newSubrequestStatus != currentSubrequestStatus){
	    
	    stgRequestHelper->subrequest->setStatus(newSubrequestStatus);
	    stgRequestHelper->subrequest->setGetNextStatus(GETNEXTSTATUSFILESTAGED);

	    /* since we don't reply to the client, we have to update explicitly the representation*/
	    stgRequestHelper->dbService->updateRep(stgRequestHelper->baseAddr, iObj_subrequest, STAGER_AUTOCOMMIT_TRUE);
	  }
	  
	  
	  
	  
	  
	}catch{
	}	
      
	
      }
      
      /* destructor */
      StagerUpdateHandler::~StagerUpdateHandler() throw()
      {
      }

    }//end namespace dbService
  }//end namespace stager
}//end namespace castor

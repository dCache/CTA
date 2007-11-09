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
#include "castor/stager/dbService/StagerPutHandler.hpp"
#include "castor/stager/dbService/StagerGetHandler.hpp"

#include "stager_uuid.h"
#include "stager_constants.h"

#include "Cns_api.h"
#include "expert_api.h"

#include "Cpwd.h"
#include "Cgrp.h"
#include "castor/IClientFactory.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/DiskCopyForRecall.hpp"

#include "castor/exception/Exception.hpp"

#include "castor/dlf/Dlf.hpp"
#include "castor/dlf/Message.hpp"
#include "castor/stager/dbService/StagerDlfMessages.hpp"

#include "serrno.h"
#include <errno.h>

#include <iostream>
#include <string>


namespace castor{
  namespace stager{
    namespace dbService{
      
      /* constructor */
      StagerUpdateHandler::StagerUpdateHandler(StagerRequestHelper* stgRequestHelper) throw(castor::exception::Exception)
      {
        this->stgRequestHelper = stgRequestHelper;
        this->typeRequest = OBJ_StageUpdateRequest;     
          
      }
      
      
      /*******************************************************************/
      /* function to set the handler's attributes according to its type */
      /*****************************************************************/
      void StagerUpdateHandler::handlerSettings() throw(castor::exception::Exception)
      {	
	 this->maxReplicaNb = this->stgRequestHelper->svcClass->maxReplicaNb();	
        this->replicationPolicy = this->stgRequestHelper->svcClass->replicationPolicy();
        
        
        
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
      }


      
      /* only handler which overwrite the preprocess part due to the specific behavior related with the fileExist */
      void StagerUpdateHandler::preHandle() throw(castor::exception::Exception)
      {
        
	/* get the uuid request string version and check if it is valid */
	stgRequestHelper->setRequestUuid();

	/* we create the StagerCnsHelper inside and we pass the requestUuid needed for logging */
	this->stgCnsHelper = new StagerCnsHelper(stgRequestHelper->requestUuid);

	/* set the username and groupname needed to print them on the log */
	stgRequestHelper->setUsernameAndGroupname();

        /* get the uuid subrequest string version and check if it is valid */
        /* we can create one !*/
        stgRequestHelper->setSubrequestUuid();
        
        /* get the associated client and set the iClientAsString variable */
        stgRequestHelper->getIClient();
        
        
        /* set the euid, egid attributes on stgCnsHelper (from fileRequest) */ 
        stgCnsHelper->cnsSetEuidAndEgid(stgRequestHelper->fileRequest);
       
        
        /* get the svcClass */
        stgRequestHelper->getSvcClass();
        
        
        /* create and fill request->svcClass link on DB */
        stgRequestHelper->linkRequestToSvcClassOnDB();
        
        /* check the existence of the file, if the user hasTo/can create it and set the fileId and server for the file */
        /* create the file if it is needed/possible */
        this->fileExist = stgCnsHelper->checkAndSetFileOnNameServer(stgRequestHelper->subrequest->fileName(), this->typeRequest, stgRequestHelper->subrequest->flags(), stgRequestHelper->subrequest->modeBits(), stgRequestHelper->svcClass);
        
        /* check if the user (euid,egid) has the right permission for the request's type. otherwise-> throw exception  */
        stgRequestHelper->checkFilePermission();
        
        this->toRecreateCastorFile = !(fileExist && (((stgRequestHelper->subrequest->flags()) & O_TRUNC) == 0));
      }


      /************************************/
      /* handler for the update request  */
      /**********************************/
      void StagerUpdateHandler::handle() throw(castor::exception::Exception)
      {
	try{

	  /**************************************************************************/
	  /* common part for all the handlers: get objects, link, check/create file*/
	  preHandle();
	  /**********/

	  handlerSettings();

	  castor::dlf::Param params[]={castor::dlf::Param(stgRequestHelper->subrequestUuid),
				       castor::dlf::Param("fileName",stgRequestHelper->subrequest->fileName()),
				       castor::dlf::Param("UserName",stgRequestHelper->username),
				       castor::dlf::Param("GroupName", stgRequestHelper->groupname),
				       castor::dlf::Param("SvcClassName",stgRequestHelper->svcClassName)				     
	  };
	  castor::dlf::dlf_writep(stgRequestHelper->requestUuid, DLF_LVL_DEBUG, STAGER_UPDATE, 5 ,params, &(stgCnsHelper->cnsFileid));
	  
	
	  jobOriented();
	  
	  if(toRecreateCastorFile) {
      // delegate to Put
      StagerPutHandler* h = new StagerPutHandler(stgRequestHelper);
      h->handle();
    } else {
      // delegate to Get
      StagerGetHandler* h = new StagerGetHandler(stgRequestHelper);
      h->handle();
    }      
	       
	}catch(castor::exception::Exception e){
	 
	  castor::dlf::Param params[]={castor::dlf::Param("Error Code",sstrerror(e.code())),
				       castor::dlf::Param("Error Message",e.getMessage().str())
	  };
	  
	  castor::dlf::dlf_writep(stgRequestHelper->requestUuid, DLF_LVL_ERROR, STAGER_UPDATE, 2 ,params, &(stgCnsHelper->cnsFileid));
	  throw(e);
	 
	}

      }
      
      
      
      
      /* destructor */
      StagerUpdateHandler::~StagerUpdateHandler() throw()
      {
      }
      
    }//end namespace dbService
  }//end namespace stager
}//end namespace castor

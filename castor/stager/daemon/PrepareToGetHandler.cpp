/**********************************************************************************************/
/* StagerPrepareToGetHandler: Constructor and implementation of the get subrequest's handler */
/* It inherits from the StagerJobRequestHandler and it needs to reply to the client         */
/*******************************************************************************************/


#include "castor/stager/daemon/StagerRequestHelper.hpp"
#include "castor/stager/daemon/StagerCnsHelper.hpp"
#include "StagerReplyHelper.hpp"

#include "castor/stager/daemon/StagerRequestHandler.hpp"
#include "castor/stager/daemon/StagerJobRequestHandler.hpp"
#include "castor/stager/daemon/StagerPrepareToGetHandler.hpp"

#include "stager_uuid.h"
#include "stager_constants.h"

#include "Cns_api.h"
#include "expert_api.h"

#include "Cpwd.h"
#include "Cgrp.h"

#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"

#include "castor/dlf/Dlf.hpp"
#include "castor/dlf/Message.hpp"
#include "castor/stager/daemon/StagerDlfMessages.hpp"


#include "serrno.h"
#include <errno.h>

#include <iostream>
#include <string>


namespace castor{
  namespace stager{
    namespace daemon{
      
      StagerPrepareToGetHandler::StagerPrepareToGetHandler(StagerRequestHelper* stgRequestHelper, StagerCnsHelper* stgCnsHelper) throw(castor::exception::Exception)
      {
        this->stgRequestHelper = stgRequestHelper;
        this->stgCnsHelper = stgCnsHelper;
        this->typeRequest = OBJ_StagePrepareToGetRequest;
      }
      
      /*******************************************************************/
      /* function to set the handler's attributes according to its type */
      /*****************************************************************/
      void StagerPrepareToGetHandler::handlerSettings() throw(castor::exception::Exception)
      {	
        this->maxReplicaNb= this->stgRequestHelper->svcClass->maxReplicaNb();
        this->replicationPolicy = this->stgRequestHelper->svcClass->replicationPolicy();
      }

      
      bool StagerPrepareToGetHandler::switchDiskCopiesForJob() throw (castor::exception::Exception)
      {
        bool result = false;
        switch(stgRequestHelper->stagerService->processPrepareRequest(stgRequestHelper->subrequest)) {
          case -2:
            stgRequestHelper->logToDlf(DLF_LVL_SYSTEM, STAGER_WAITSUBREQ, &(stgCnsHelper->cnsFileid));
            stgRequestHelper->subrequest->setStatus(SUBREQUEST_WAITSUBREQ);
            result = true;
            break;
          
          case -1:
            stgRequestHelper->logToDlf(DLF_LVL_USER_ERROR, STAGER_UNABLETOPERFORM, &(stgCnsHelper->cnsFileid));
            break;
          
          case DISKCOPY_STAGED:   // nothing to do
            stgRequestHelper->subrequest->setStatus(SUBREQUEST_ARCHIVED);
            stgRequestHelper->subrequest->setGetNextStatus(GETNEXTSTATUS_FILESTAGED);
            stgRequestHelper->logToDlf(DLF_LVL_SYSTEM, STAGER_ARCHIVE_SUBREQ, &(stgCnsHelper->cnsFileid));
            
            /* we archive the subrequest */
            stgRequestHelper->stagerService->archiveSubReq(stgRequestHelper->subrequest->id());
            result = true;
            break;
          
          case DISKCOPY_WAITTAPERECALL:   // trigger a recall
            // answer client only if success
            result = stgRequestHelper->stagerService->createRecallCandidate(stgRequestHelper->subrequest, stgRequestHelper->svcClass);
            if(result) {
              stgRequestHelper->logToDlf(DLF_LVL_SYSTEM, STAGER_TAPE_RECALL, &(stgCnsHelper->cnsFileid));
            }
            else {
              // no tape copy found because of Tape0 file, log it
              // any other tape error will throw an exception and will be classified as LVL_ERROR 
              stgRequestHelper->logToDlf(DLF_LVL_USER_ERROR, STAGER_UNABLETOPERFORM, &(stgCnsHelper->cnsFileid));
            }
            break;
        }
        return result;
      }
      
      
      
      void StagerPrepareToGetHandler::handle() throw(castor::exception::Exception)
      {
        
        StagerReplyHelper* stgReplyHelper=NULL;
        try{
          handlerSettings();
          
          stgRequestHelper->logToDlf(DLF_LVL_DEBUG, STAGER_PREPARETOGET, &(stgCnsHelper->cnsFileid));
          jobOriented();	  
          
          /* depending on the value returned by getDiskCopiesForJob */
          /* if needed, we update the subrequestStatus internally  */
          if(switchDiskCopiesForJob()) {
            stgReplyHelper = new StagerReplyHelper();
            stgReplyHelper->setAndSendIoResponse(stgRequestHelper,&(stgCnsHelper->cnsFileid), 0, "");
            stgReplyHelper->endReplyToClient(stgRequestHelper);
            
            delete stgReplyHelper;
            stgReplyHelper = 0;
          }	  
          
        }catch(castor::exception::Exception e){
          
          /* since if an error happens we are gonna reply to the client(and internally, update subreq on DB)*/
          /* we don t execute: daemon->updateRep ..*/
          if(stgReplyHelper != NULL) delete stgReplyHelper;
          
          castor::dlf::Param params[]={castor::dlf::Param("Error Code",sstrerror(e.code())),
            castor::dlf::Param("Error Message",e.getMessage().str())
          };
          
          castor::dlf::dlf_writep(stgRequestHelper->requestUuid, DLF_LVL_ERROR, STAGER_PREPARETOGET, 2 ,params, &(stgCnsHelper->cnsFileid));
          throw(e);
        }        
        
      }
      
      
      /***********************************************************************/
      /*    destructor                                                      */
      StagerPrepareToGetHandler::~StagerPrepareToGetHandler() throw()
      {
        
      }
      
    }//end namespace dbservice
  }//end namespace stager
}//end namespace castor

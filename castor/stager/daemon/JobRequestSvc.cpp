/*********************************************************************************************************/
/* cpp version of the "stager_db_service.c" represented by a thread calling the right request's handler */
/*******************************************************************************************************/

#include "castor/stager/dbService/StagerRequestHelper.hpp"
#include "castor/stager/dbService/StagerCnsHelper.hpp"
#include "castor/stager/dbService/StagerReplyHelper.hpp"
#include "castor/stager/dbService/StagerRequestHandler.hpp"
#include "castor/stager/dbService/StagerJobRequestHandler.hpp"
#include "castor/stager/dbService/JobRequestSvc.hpp"

#include "castor/stager/dbService/StagerGetHandler.hpp"
#include "castor/stager/dbService/StagerPutHandler.hpp"
#include "castor/stager/dbService/StagerUpdateHandler.hpp"

#include "castor/stager/dbService/RequestSvc.hpp"
#include "castor/BaseObject.hpp"
#include "castor/PortsConfig.hpp"
#include "castor/server/BaseServer.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"

#include "stager_constants.h"
#include "Cns_api.h"
#include "expert_api.h"
#include "serrno.h"

#include "dlf_api.h"
#include "castor/dlf/Dlf.hpp"
#include "castor/dlf/Param.hpp"

#include "osdep.h"
#include "Cnetdb.h"
#include "Cpwd.h"
#include "Cgrp.h"
#include "stager_uuid.h"
#include "Cuuid.h"
#include "u64subr.h"
#include "marshall.h"
#include "net.h"

#include "castor/IAddress.hpp"
#include "castor/IObject.hpp"
#include "castor/Services.hpp"
#include "castor/stager/IStagerSvc.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/Constants.hpp"

#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"


#include "serrno.h"
#include <errno.h>
#include <iostream>
#include <string>


namespace castor{
  namespace stager{
    namespace dbService{
      
      
      /****************/
      /* constructor */
      /**************/
      JobRequestSvc::JobRequestSvc() throw(castor::exception::Exception) :
      RequestSvc("JobReqSvc")
      {
        m_jobManagerHost = castor::PortsConfig::getInstance()->getHostName(castor::CASTOR_JOBMANAGER);
        m_jobManagerPort = castor::PortsConfig::getInstance()->getNotifPort(castor::CASTOR_JOBMANAGER);
      }
      

      void JobRequestSvc::process(castor::IObject* subRequestToProcess) throw () {
       
        StagerRequestHelper* stgRequestHelper = NULL;
        StagerJobRequestHandler* stgRequestHandler = NULL;
        
        try {
          int typeRequest=0;
          stgRequestHelper = new StagerRequestHelper(dynamic_cast<castor::stager::SubRequest*>(subRequestToProcess), typeRequest);

          switch(typeRequest){
            
            case OBJ_StageGetRequest:
              stgRequestHandler = new StagerGetHandler(stgRequestHelper);
              break;
            
            case OBJ_StagePutRequest:
              stgRequestHandler = new StagerPutHandler(stgRequestHelper);
              break;
            
            case OBJ_StageUpdateRequest:
              stgRequestHandler = new StagerUpdateHandler(stgRequestHelper);
              break;
              
            default:
              // XXX should never happen, but happens?!
              castor::exception::Internal e;
              e.getMessage() << "Request type " << typeRequest << " not correct for stager svc " << m_name;
              stgRequestHelper->logToDlf(DLF_LVL_ERROR, STAGER_INVALID_TYPE, 0);
              throw e;
          }
          
          stgRequestHandler->preHandle();
          stgRequestHandler->handle();
          
          if (stgRequestHandler->notifyJobManager()) {
            castor::server::BaseServer::sendNotification(m_jobManagerHost, m_jobManagerPort, 'D');
          }
          
          delete stgRequestHandler;
          delete stgRequestHelper;         
          
        }catch(castor::exception::Exception ex){ /* process the exception an replyToClient */
          
          handleException(stgRequestHelper, (stgRequestHandler ? stgRequestHandler->getStgCnsHelper() : 0), ex.code(), ex.getMessage().str());
          
          /* we delete our objects */
          if(stgRequestHandler) delete stgRequestHandler;
          if(stgRequestHelper) delete stgRequestHelper;
        }
        
      }/* end JobRequestSvc::process */
      
      
    }//end namespace dbService
  }//end namespace stager
}//end namespace castor

















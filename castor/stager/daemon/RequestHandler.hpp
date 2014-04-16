/*******************************************************************************************************/
/* Base class for OpenRequestHandler and all the fileRequest handlers                            */
/* Basically: handle() as METHOD  and  (reqHelper,reqHelper,stgReplyHelper)  as ATTRIBUTES */
/****************************************************************************************************/

#pragma once

#include <string>
#include <iostream>

#include "castor/stager/daemon/RequestHelper.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/ObjectSet.hpp"


namespace castor{
  namespace stager{
    namespace daemon{
      
      class RequestHandler {
        
      public:
        
        RequestHandler(RequestHelper* requestHelper) throw() :
          reqHelper(requestHelper) {};
        
        virtual ~RequestHandler() throw() {};
        
        /**
         * Main function for the specific request handler.
         * Default behavior is the common part across all user requests processing.
         * @throw exception in case of system/db errors.
         */
        virtual void handle() throw (castor::exception::Exception);
        
        /// The RequestHelper instance
        RequestHelper* reqHelper;	
        
      };
      
    }
  }
}


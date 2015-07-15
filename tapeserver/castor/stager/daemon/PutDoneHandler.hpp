/*************************************************************************************************/
/* PutDoneHandler: Constructor and implementation of the PutDone request's handle */
/***********************************************************************************************/


#pragma once


#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"

#include "serrno.h"
#include <errno.h>
#include <iostream>
#include <string>

#include "castor/stager/daemon/RequestHelper.hpp"
#include "castor/stager/daemon/RequestHandler.hpp"


namespace castor{
  namespace stager{
    namespace daemon{

      class RequestHelper;

      class PutDoneHandler : public RequestHandler {
	
      public:
        /* constructor */
        PutDoneHandler(RequestHelper* reqHelper)  :
          RequestHandler(reqHelper) {};
        /* destructor */
        ~PutDoneHandler() throw() {};

        /* putDone request handler */
        virtual void handle() ;
        
      }; //end PutDoneHandler class

    }//end daemon 
  }//end stager
}//end castor




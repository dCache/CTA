/****************************************************************************************************************************/
/* handler for the Update subrequest, since it is jobOriented, it uses the mostly part of the StagerJobRequestHandler class*/
/* depending if the file exist, it can follow the huge flow (jobOriented, as Get) or a small one                          */
/*************************************************************************************************************************/


#ifndef STAGER_UPDATE_HANDLER_HPP
#define STAGER_UPDATE_HANDLER_HPP 1


#include "castor/stager/daemon/StagerRequestHelper.hpp"
#include "castor/stager/daemon/StagerCnsHelper.hpp"
#include "castor/stager/daemon/StagerReplyHelper.hpp"

#include "castor/stager/daemon/StagerRequestHandler.hpp"
#include "castor/stager/daemon/StagerJobRequestHandler.hpp"


#include "stager_uuid.h"
#include "stager_constants.h"

#include "Cns_api.h"
#include "expert_api.h"

#include "Cpwd.h"
#include "Cgrp.h"
#include "u64subr.h"

#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"

#include "castor/exception/Exception.hpp"

#include "serrno.h"
#include <errno.h>

#include <iostream>
#include <string>


namespace castor{
  namespace stager{
    namespace daemon{
      
      class StagerRequestHelper;
      class StagerCnsHelper;
      
      class StagerUpdateHandler : public virtual StagerJobRequestHandler{
        
      protected:
        
        bool recreate;
        
      public:
      
        /* constructor */
        StagerUpdateHandler(StagerRequestHelper* stgRequestHelper) throw(castor::exception::Exception);
        /* destructor */
        ~StagerUpdateHandler() throw();
        
        /* set the internal attribute "toRecreateCastorFile depending on fileExist" */
        /* which determines the real flow of the handler */
        virtual void preHandle() throw(castor::exception::Exception);
        
        /* handler for the Update request  */
        void handle() throw(castor::exception::Exception);
        
        
      }; //end StagerUpdateHandler class
      
      
    }//end namespace daemon
  }//end namespace stager
}//end namespace castor



#endif

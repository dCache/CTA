/*******************************************************************************************************/
/* Base class for StagerJobRequestHandler and all the fileRequest handlers                            */
/* Basically: handle() as METHOD  and  (stgRequestHelper,stgCnsHelper,stgReplyHelper)  as ATTRIBUTES */
/****************************************************************************************************/

#ifndef STAGER_REQUEST_HANDLER_HPP
#define STAGER_REQUEST_HANDLER_HPP 1

#include "castor/stager/dbService/StagerRequestHelper.hpp"
#include "castor/stager/dbService/StagerCnsHelper.hpp"
#include "castor/stager/dbService/StagerReplyHelper.hpp"


#include "castor/exception/Exception.hpp"
#include "castor/BaseObject.hpp"
#include "castor/ObjectSet.hpp"


#include <string>
#include <iostream>

#define DEFAULTFILESIZE (2 * (u_signed64) 1000000000)


namespace castor{
  namespace stager{
    namespace dbService{

      class StagerRequestHelper;
      class StagerCnsHelper;
      class StagerReplyHelper;

      class StagerRequestHandler : public virtual castor::BaseObject{


      public:
	/* empty destructor */
	virtual ~StagerRequestHandler() throw() {};
	
	/* main function for the specific request handler */
	virtual void handle() throw (castor::exception::Exception) = 0;


      protected:
	StagerRequestHelper *stgRequestHelper;
	StagerCnsHelper *stgCnsHelper;
	StagerReplyHelper *stgReplyHelper;

     

      };/* end StagerRequestHandler class */

    }
  }
}




#endif


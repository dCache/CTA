/****************************************************************************************************************************/
/* handler for the Update subrequest, since it is jobOriented, it uses the mostly part of the StagerJobRequestHandler class*/
/* depending if the file exist, it can follow the huge flow (jobOriented, as Get) or a small one                          */
/*************************************************************************************************************************/


#ifndef STAGER_UPDATE_HANDLER_HPP
#define STAGER_UPDATE_HANDLER_HPP 1


#include "castor/stager/dbService/StagerRequestHelper.hpp"
#include "castor/stager/dbService/StagerCnsHelper.hpp"
#include "castor/stager/dbService/StagerReplyHelper.hpp"

#include "castor/stager/dbService/StagerRequestHandler.hpp"
#include "castor/stager/dbService/StagerJobRequestHandler.hpp"


#include "stager_uuid.h"
#include "stager_constants.h"
#include "serrno.h"
#include "Cns_api.h"
#include "expert_api.h"
#include "rm_api.h"
#include "Cpwd.h"
#include "Cgrp.h"
#include "u64subr.h"
#include "castor/IClientFactory.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"

#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"

#include <iostream>
#include <string>


namespace castor{
  namespace stager{
    namespace dbService{

      class StagerRequestHelper;
      class StagerCnsHelper;

      class StagerUpdateHandler : public virtual StagerJobRequestHandler{

      private:
	/* flag to schedule or to recreateCastorFile depending if the file exists... */
	bool toRecreateCastorFile;
	
      public:
	/* constructor */
	StagerUpdateHandler::StagerUpdateHandler(StagerRequestHelper* stgRequestHelper, StagerCnsHelper* stgCnsHelper, bool toRecreateCastorFile) throw(castor::exception::Exception);
	/* destructor */
	StagerUpdateHandler::~StagerUpdateHandler() throw();

	/* handler for the Update request  */
	void StagerUpdateHandler::handle() throw(castor::exception::Exception);



      	/***********************************************************************************************/
	/* virtual functions inherited from IObject                                                   */
	/*********************************************************************************************/
	virtual void setId(u_signed64 id);
	virtual u_signed64 id() const;
	virtual int type() const;
	virtual IObject* clone();
	virtual void print() const;
	virtual void print(std::ostream& stream, std::string indent, castor::ObjectSet& alreadyPrinted) const;
	
      }; //end StagerUpdateHandler class


    }//end namespace dbService
  }//end namespace stager
}//end namespace castor



#endif

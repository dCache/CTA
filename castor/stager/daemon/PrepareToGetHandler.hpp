/*****************************************************************************
 *                      PrepareToGetHandler.hpp
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Implementation of the prepareToGet subrequest's handler
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef STAGER_PREPARETOGET_HANDLER_HPP
#define STAGER_PREPARETOGET_HANDLER_HPP 1

// Include Files
#include "occi.h"
#include "castor/exception/Exception.hpp"
#include "castor/stager/daemon/OpenRequestHandler.hpp"

namespace castor{
  namespace stager{
    namespace daemon{

      // forward declaration
      class RequestHelper;

      class PrepareToGetHandler : public OpenRequestHandler {
	
      public:

        /* constructor */
        PrepareToGetHandler(RequestHelper* reqHelper) throw(castor::exception::Exception);

        /* destructor */
        ~PrepareToGetHandler() throw() {};
	
        /* Get request handler */
        virtual bool handle() throw (castor::exception::Exception);

      private:

        /// SQL statement for function handleGet
        oracle::occi::Statement *m_handlePGetStatement;
	
      }; // end PrepareToGetHandler class

    }//end namespace daemon
  }//end namespace stager
}//end namespace castor

#endif

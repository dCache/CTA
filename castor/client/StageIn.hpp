/******************************************************************************
 *                      StageIn.hpp
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
 * @(#)$RCSfile: StageIn.hpp,v $ $Revision: 1.1.1.1 $ $Release$ $Date: 2004/05/12 12:13:34 $ $Author: sponcec3 $
 *
 * 
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef CLIENT_STAGEIN_HPP 
#define CLIENT_STAGEIN_HPP 1

// Include Files
#include "BaseClient.hpp"

namespace castor {

  namespace rh {
    // Forward declaration
    class Request;
  }

  namespace client {

    class StageIn : public BaseClient {

    public:

      /**
       * builds the actual request. This method has to be
       * reimplemented in each client.
       * Note that the caller is responsible for the deallocation
       * of the request
       * @return the request to be sent to the request handler
       */
      virtual castor::rh::Request* buildRequest() throw (Exception);

      /**
       * this method is responsible for the output of the
       * request result. This method has to be reimplemented
       * in each client
       */
      virtual void printResult(castor::IObject& result) throw (Exception);

    };

  } // end of namespace client

} // end of namespace castor

#endif // CLIENT_STAGEIN_HPP

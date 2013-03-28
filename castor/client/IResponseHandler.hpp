/******************************************************************************
 *                      IResponseHandler.hpp
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
 *
 * 
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef CLIENT_IRESPONSEHANDLER_HPP 
#define CLIENT_IRESPONSEHANDLER_HPP 1

// Include Files
#include "castor/exception/Exception.hpp"

namespace castor {
  
  namespace rh {
    // Forward declaration
    class Response;
  }
  
  namespace client {
    
    class IResponseHandler {
      
    public:
      
      /**
       * default destructor
       */
      virtual ~IResponseHandler() {}

      /**
       * handles a response when one arrives
       * @param r the response to handle
       */
      virtual void handleResponse(castor::rh::Response& r)
        throw (castor::exception::Exception) = 0;
      
      /**
       * terminates the response handler. This is called
       * when all responses were received.
       */
      virtual void terminate()
        throw (castor::exception::Exception) = 0;

    };

  } // end of namespace client
  
} // end of namespace castor
      
#endif // CLIENT_IRESPONSEHANDLER_HPP

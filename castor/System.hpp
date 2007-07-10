/******************************************************************************
 *                      System.hpp
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
 * @(#)$RCSfile: System.hpp,v $ $Revision: 1.2 $ $Release$ $Date: 2007/07/10 15:27:32 $ $Author: itglp $
 *
 * A class with static methods for system level utilities.
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef CASTOR_SYSTEM_HPP
#define CASTOR_SYSTEM_HPP 1

// Include Files
#include <string>
#include "castor/exception/Exception.hpp"
#if !defined(_WIN32)
#include <netdb.h>
#else
#include <winsock2.h>
#endif

namespace castor {

  class System {

  public:

    /**
     * Gets the host name. Handles all errors that may occur with 
     * the gethostname() API.  
     * @exception in case of an error 
     */
    static std::string getHostName() throw (castor::exception::Exception);
    
    /**
     * Performs an host name lookup. Handles all errors that may occur with 
     * the gethostbyname() API. On linux, the reentrant version is used.
     * @exception in case of an error 
     */
    static struct hostent* getHostByName(std::string host)
      throw (castor::exception::Exception);
     
  };

} // end of namespace castor

#endif // CASTOR_SYSTEM_HPP

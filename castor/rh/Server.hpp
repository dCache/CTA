/******************************************************************************
 *                      Server.hpp
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
 * @(#)$RCSfile: Server.hpp,v $ $Revision: 1.7 $ $Release$ $Date: 2007/08/20 10:27:40 $ $Author: sponcec3 $
 *
 *
 *
 * @author Giuseppe Lo Presti
 *****************************************************************************/

#ifndef RH_SERVER_HPP
#define RH_SERVER_HPP 1

#include "castor/server/BaseServer.hpp"
#include "castor/exception/Exception.hpp"

#define CSP_MSG_MAGIC 0xCA001
#define CSP_RHSERVER_PORT 9002
#define CSP_NOTIFICATION_PORT 9001

namespace castor {

  namespace rh {

    /**
     * Constants
     */
    extern const char* PORT_ENV;
    extern const char* CATEGORY_CONF;
    extern const char* PORT_CONF;
    extern const char* ACCESSLISTS_ENV;
    extern const char* ACCESSLISTS_CONF;

    /**
     * The Request Handler server. This is where client requests
     * arrive. The main task of this component is to store them
     * for future processing.
     */
    class Server : public castor::server::BaseServer {

    public:

      /**
       * Constructor
       */
      Server();

      /**
       * Overloaded method from BaseServer for individual command line parser
       */
      virtual void parseCommandLine(int argc, char *argv[]);

    protected:
 
      /**
       * Prints out the online help
       */
      virtual void help(std::string programName);

    }; // class Server

  } // end of namespace rh

} // end of namespace castor

#endif // RH_SERVER_HPP


/******************************************************************************
 *                      IPlugin.hpp
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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * abstract interface of a stagerJob plugin
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef STAGERJOB_IPLUGIN_HPP
#define STAGERJOB_IPLUGIN_HPP 1

// Include Files
#include "castor/exception/Exception.hpp"

namespace castor {

  namespace job {

    namespace stagerjob {

      // Forward declarations
      struct InputArguments;
      struct PluginContext;

      /**
       * abstract interface of a stagerJob plugin
       */
      class IPlugin {

      public:

        /**
         * gets the port range that can be used by the protocol
         * to listen for client connections
         */
        virtual std::pair<int, int> getPortRange
        (InputArguments &args) throw() = 0;

        /**
         * hook for the code to be executed just before the mover fork
         * empty by default.
         * @param args the arguments given to the stager job
         * @param context the current context (localhost, port, etc...)
         */
        virtual void preForkHook(InputArguments &args,
                                 PluginContext &context)
          throw (castor::exception::Exception) = 0;

        /**
         * hook for the launching of the mover
         * To be implemented in all plugins
         * @param args the arguments given to the stager job
         * @param context the current context (localhost, port, etc...)
         */
        virtual void execMover(InputArguments &args,
                               PluginContext &context)
          throw (castor::exception::Exception) = 0;

        /**
         * hook for the code to be executed just after the mover fork,
         * in the parent process. Empty by default.
         * @param args the arguments given to the stager job
         * @param context the current context (localhost, port, etc...)
         */
        virtual void postForkHook(InputArguments &args,
                                  PluginContext &context)
          throw (castor::exception::Exception) = 0;

      }; // end of class IPlugin

    } // end of namespace stagerjob

  } // end of namespace job

} // end of namespace castor

#endif // STAGERJOB_IPLUGIN_HPP

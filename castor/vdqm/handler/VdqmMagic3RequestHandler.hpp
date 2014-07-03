/******************************************************************************
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once


#include "castor/exception/Exception.hpp"
#include "castor/vdqm/handler/BaseRequestHandler.hpp"
#include "h/vdqm_messages.h"


namespace castor {

  namespace vdqm {

    namespace handler {

      /**
       * This class provides functions to handle VDQM messages with the magic
       * number VDQM_MAGIC3.
       */
      class VdqmMagic3RequestHandler : public BaseRequestHandler {

      public:

        /**
         * Handles the specified delete drive message.
         *
         * @param socket the socket.
         * @param cuuid the unique id of the request. Needed for dlf.
         * @param msg the message.
         */
        void handleDelDrv(castor::io::ServerSocket &socket,
          const Cuuid_t &cuuid, vdqmDelDrv_t &msg)
          ;

        /**
         * Handlers the specified dedicate message.
         *
         * @param cuuid the unique id of the request. Needed for dlf.
         * @param msg the message.
         */
        void handleDedicate(castor::io::ServerSocket &socket,
          const Cuuid_t &cuuid, vdqmDedicate_t &msg)
          ;

      }; // class VdqmMagic3RequestHandler

    } // namespace handler

  } // namespace vdqm

} // namespace castor


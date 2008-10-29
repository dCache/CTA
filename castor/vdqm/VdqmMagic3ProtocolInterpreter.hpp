/******************************************************************************
 *                      VdqmMagic3ProtocolInterpreter.hpp
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

#ifndef CASTOR_VDQM_VDQMMAGIC3PROTOCOLINTERPRETER_HPP
#define CASTOR_VDQM_VDQMMAGIC3PROTOCOLINTERPRETER_HPP 1

#include "castor/exception/Exception.hpp"
#include "castor/io/ServerSocket.hpp"
#include "h/vdqm_messages.h"


namespace castor {

  namespace vdqm {
    
    /**
     * This class provides the functions to send and receive messages through a 
     * socket using VDQM messages with the magic number VDQM_MAGIC3.
     */
    class VdqmMagic3ProtocolInterpreter {

    public:

      /**
       * Constructor.
       *
       * @param socket The Object, which includes the actual socket connection
       * to the client
       * @param cuuid the cuuid of the incoming request
       * @exception In case that one of the parameters is NULL
       */
      VdqmMagic3ProtocolInterpreter(castor::io::ServerSocket &socket,
        const Cuuid_t &cuuid)throw (castor::exception::Exception);

      /**
       * Reads the message header of VDQM message with a magic number of
       * VDQM_MAGIC3 from the socket of this protocol interpreter.  Please note
       * that this method assumes the magic number has already been read from
       * the socket.
       *
       * @param magic The already read out magic number which is to be copied
       * into the header ro make it complete
       * @param header The message header
       */
      void readHeader(const unsigned int magic, vdqmHdr_t &header)
        throw(castor::exception::Exception);

      /**
       * Reads the message body of a delete drive message from the socket of
       * this protocol interpreter.  Please note that this method assumes the
       * message header has already been read from the socket.
       *
       * @param len The length of the message body
       * @param msg The message body
       * should be read out into
       */
      void readDelDrv(const int len, vdqmDelDrv_t &msg)
        throw(castor::exception::Exception);

      /**
       * Reads the message body of a dedicate message from the socket of this
       * protocol interpreter.  Please note that this method assumes the
       * message header has already been read from the socket.
       *
       * @param len The length of the message body
       * @param msg The message body
       */
      void readDedicate(const int len, vdqmDedicate_t &msg)
        throw(castor::exception::Exception);

    private:

      /**
       * The object which includes the socket connection to the client
       */
      castor::io::ServerSocket &m_socket;

      /**
       * The cuuid of the request
       */
      const Cuuid_t &m_cuuid;

    }; // class VdqmMagic3ProtocolInterpreter

  } // namespace vdqm

} // namespace castor      

#endif // CASTOR_VDQM_VDQMMAGIC3PROTOCOLINTERPRETER_HPP

/******************************************************************************
 *                      castor/rh/IOResponse.hpp
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
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_RH_IORESPONSE_HPP
#define CASTOR_RH_IORESPONSE_HPP

// Include Files
#include "castor/rh/FileResponse.hpp"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace rh {

    /**
     * class IOResponse
     * Response dealing with an I/O. It gives everything to access the file considered.
     */
    class IOResponse : public FileResponse {

    public:

      /**
       * Empty Constructor
       */
      IOResponse() throw();

      /**
       * Empty Destructor
       */
      virtual ~IOResponse() throw();

      /**
       * Outputs this object in a human readable format
       * @param stream The stream where to print this object
       * @param indent The indentation to use
       * @param alreadyPrinted The set of objects already printed.
       * This is to avoid looping when printing circular dependencies
       */
      virtual void print(std::ostream& stream,
                         std::string indent,
                         castor::ObjectSet& alreadyPrinted) const;

      /**
       * Outputs this object in a human readable format
       */
      virtual void print() const;

      /**
       * Gets the type of this kind of objects
       */
      static int TYPE();

      /********************************************/
      /* Implementation of IObject abstract class */
      /********************************************/
      /**
       * Gets the type of the object
       */
      virtual int type() const;

      /**
       * virtual method to clone any object
       */
      virtual castor::IObject* clone();

      /*********************************/
      /* End of IObject abstract class */
      /*********************************/
      /**
       * Get the value of m_fileName
       * The file name
       * @return the value of m_fileName
       */
      std::string fileName() const {
        return m_fileName;
      }

      /**
       * Set the value of m_fileName
       * The file name
       * @param new_var the new value of m_fileName
       */
      void setFileName(std::string new_var) {
        m_fileName = new_var;
      }

      /**
       * Get the value of m_server
       * The server where to find the file
       * @return the value of m_server
       */
      std::string server() const {
        return m_server;
      }

      /**
       * Set the value of m_server
       * The server where to find the file
       * @param new_var the new value of m_server
       */
      void setServer(std::string new_var) {
        m_server = new_var;
      }

      /**
       * Get the value of m_port
       * The port where to find the file
       * @return the value of m_port
       */
      int port() const {
        return m_port;
      }

      /**
       * Set the value of m_port
       * The port where to find the file
       * @param new_var the new value of m_port
       */
      void setPort(int new_var) {
        m_port = new_var;
      }

      /**
       * Get the value of m_protocol
       * The protocol to use to retrieve the file
       * @return the value of m_protocol
       */
      std::string protocol() const {
        return m_protocol;
      }

      /**
       * Set the value of m_protocol
       * The protocol to use to retrieve the file
       * @param new_var the new value of m_protocol
       */
      void setProtocol(std::string new_var) {
        m_protocol = new_var;
      }

    private:

      /// The file name
      std::string m_fileName;

      /// The server where to find the file
      std::string m_server;

      /// The port where to find the file
      int m_port;

      /// The protocol to use to retrieve the file
      std::string m_protocol;

    }; // end of class IOResponse

  }; // end of namespace rh

}; // end of namespace castor

#endif // CASTOR_RH_IORESPONSE_HPP

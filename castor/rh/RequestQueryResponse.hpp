/******************************************************************************
 *                      castor/rh/RequestQueryResponse.hpp
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

#ifndef CASTOR_RH_REQUESTQUERYRESPONSE_HPP
#define CASTOR_RH_REQUESTQUERYRESPONSE_HPP

// Include Files
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace rh {

    /**
     * class RequestQueryResponse
     * Response to the RequestQueryRequest
     */
    class RequestQueryResponse : public virtual Response {

    public:

      /**
       * Empty Constructor
       */
      RequestQueryResponse() throw();

      /**
       * Empty Destructor
       */
      virtual ~RequestQueryResponse() throw();

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
       * Sets the id of the object
       * @param id The new id
       */
      virtual void setId(u_signed64 id);

      /**
       * gets the id of the object
       */
      virtual u_signed64 id() const;

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
       * Get the value of m_reqId
       * The Cuuid identifying the request given as a human readable string
       * @return the value of m_reqId
       */
      std::string reqId() const {
        return m_reqId;
      }

      /**
       * Set the value of m_reqId
       * The Cuuid identifying the request given as a human readable string
       * @param new_var the new value of m_reqId
       */
      void setReqId(std::string new_var) {
        m_reqId = new_var;
      }

      /**
       * Get the value of m_status
       * The status of the request
       * @return the value of m_status
       */
      unsigned int status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * The status of the request
       * @param new_var the new value of m_status
       */
      void setStatus(unsigned int new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_creationTime
       * Time of the request creation
       * @return the value of m_creationTime
       */
      u_signed64 creationTime() const {
        return m_creationTime;
      }

      /**
       * Set the value of m_creationTime
       * Time of the request creation
       * @param new_var the new value of m_creationTime
       */
      void setCreationTime(u_signed64 new_var) {
        m_creationTime = new_var;
      }

      /**
       * Get the value of m_modificationTime
       * Time of the last modification of the request
       * @return the value of m_modificationTime
       */
      u_signed64 modificationTime() const {
        return m_modificationTime;
      }

      /**
       * Set the value of m_modificationTime
       * Time of the last modification of the request
       * @param new_var the new value of m_modificationTime
       */
      void setModificationTime(u_signed64 new_var) {
        m_modificationTime = new_var;
      }

    private:

    private:

      /// The Cuuid identifying the request given as a human readable string
      std::string m_reqId;

      /// The status of the request
      unsigned int m_status;

      /// Time of the request creation
      u_signed64 m_creationTime;

      /// Time of the last modification of the request
      u_signed64 m_modificationTime;

      /// The id of this object
      u_signed64 m_id;

    }; // end of class RequestQueryResponse

  }; // end of namespace rh

}; // end of namespace castor

#endif // CASTOR_RH_REQUESTQUERYRESPONSE_HPP

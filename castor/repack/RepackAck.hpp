/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/repack/RepackAck.hpp
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

#ifndef CASTOR_REPACK_REPACKACK_HPP
#define CASTOR_REPACK_REPACKACK_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace repack {

    // Forward declarations
    class RepackRequest;

    /**
     * class RepackAck
     */
    class RepackAck : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      RepackAck() throw();

      /**
       * Empty Destructor
       */
      virtual ~RepackAck() throw();

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
       * Get the value of m_errorCode
       * @return the value of m_errorCode
       */
      int errorCode() const {
        return m_errorCode;
      }

      /**
       * Set the value of m_errorCode
       * @param new_var the new value of m_errorCode
       */
      void setErrorCode(int new_var) {
        m_errorCode = new_var;
      }

      /**
       * Get the value of m_errorMessage
       * @return the value of m_errorMessage
       */
      std::string errorMessage() const {
        return m_errorMessage;
      }

      /**
       * Set the value of m_errorMessage
       * @param new_var the new value of m_errorMessage
       */
      void setErrorMessage(std::string new_var) {
        m_errorMessage = new_var;
      }

      /**
       * Get the value of m_id
       * The id of this object
       * @return the value of m_id
       */
      u_signed64 id() const {
        return m_id;
      }

      /**
       * Set the value of m_id
       * The id of this object
       * @param new_var the new value of m_id
       */
      void setId(u_signed64 new_var) {
        m_id = new_var;
      }

      /**
       * Add a RepackRequest* object to the m_requestVector list
       */
      void addRequest(RepackRequest* add_object) {
        m_requestVector.push_back(add_object);
      }

      /**
       * Remove a RepackRequest* object from m_requestVector
       */
      void removeRequest(RepackRequest* remove_object) {
        for (unsigned int i = 0; i < m_requestVector.size(); i++) {
          RepackRequest* item = m_requestVector[i];
          if (item == remove_object) {
            std::vector<RepackRequest*>::iterator it = m_requestVector.begin() + i;
            m_requestVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of RepackRequest* objects held by m_requestVector
       * @return list of RepackRequest* objects held by m_requestVector
       */
      std::vector<RepackRequest*>& request() {
        return m_requestVector;
      }

    private:

      int m_errorCode;

      std::string m_errorMessage;

      /// The id of this object
      u_signed64 m_id;

      std::vector<RepackRequest*> m_requestVector;

    }; // end of class RepackAck

  }; // end of namespace repack

}; // end of namespace castor

#endif // CASTOR_REPACK_REPACKACK_HPP

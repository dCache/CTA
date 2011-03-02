/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/bwlist/BWUser.hpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_BWLIST_BWUSER_HPP
#define CASTOR_BWLIST_BWUSER_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace bwlist {

    // Forward declarations
    class ChangePrivilege;

    /**
     * class BWUser
     * A user, defined by its uid and gid
     */
    class BWUser : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      BWUser() throw();

      /**
       * Empty Destructor
       */
      virtual ~BWUser() throw();

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
       * Get the value of m_euid
       * the user id of this user
       * @return the value of m_euid
       */
      int euid() const {
        return m_euid;
      }

      /**
       * Set the value of m_euid
       * the user id of this user
       * @param new_var the new value of m_euid
       */
      void setEuid(int new_var) {
        m_euid = new_var;
      }

      /**
       * Get the value of m_egid
       * The group id of this user
       * @return the value of m_egid
       */
      int egid() const {
        return m_egid;
      }

      /**
       * Set the value of m_egid
       * The group id of this user
       * @param new_var the new value of m_egid
       */
      void setEgid(int new_var) {
        m_egid = new_var;
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
       * Get the value of m_request
       * @return the value of m_request
       */
      ChangePrivilege* request() const {
        return m_request;
      }

      /**
       * Set the value of m_request
       * @param new_var the new value of m_request
       */
      void setRequest(ChangePrivilege* new_var) {
        m_request = new_var;
      }

    private:

      /// the user id of this user
      int m_euid;

      /// The group id of this user
      int m_egid;

      /// The id of this object
      u_signed64 m_id;

      ChangePrivilege* m_request;

    }; /* end of class BWUser */

  } /* end of namespace bwlist */

} /* end of namespace castor */

#endif // CASTOR_BWLIST_BWUSER_HPP

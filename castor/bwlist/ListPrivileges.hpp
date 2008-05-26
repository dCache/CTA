/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/bwlist/ListPrivileges.hpp
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

#ifndef CASTOR_BWLIST_LISTPRIVILEGES_HPP
#define CASTOR_BWLIST_LISTPRIVILEGES_HPP

// Include Files
#include "castor/stager/Request.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace bwlist {

    /**
     * class ListPrivileges
     * Request for listing privileges based on some service class, user, group, request
     * type or any combination of those
     */
    class ListPrivileges : public virtual castor::stager::Request {

    public:

      /**
       * Empty Constructor
       */
      ListPrivileges() throw();

      /**
       * Empty Destructor
       */
      virtual ~ListPrivileges() throw();

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
       * Get the value of m_user
       * the user for which t privileges. -1 means all users
       * @return the value of m_user
       */
      int user() const {
        return m_user;
      }

      /**
       * Set the value of m_user
       * the user for which t privileges. -1 means all users
       * @param new_var the new value of m_user
       */
      void setUser(int new_var) {
        m_user = new_var;
      }

      /**
       * Get the value of m_group
       * the group for which t privileges. -1 means all groups
       * @return the value of m_group
       */
      int group() const {
        return m_group;
      }

      /**
       * Set the value of m_group
       * the group for which t privileges. -1 means all groups
       * @param new_var the new value of m_group
       */
      void setGroup(int new_var) {
        m_group = new_var;
      }

      /**
       * Get the value of m_requestType
       * the request type for which t privileges. 0 means all request types
       * @return the value of m_requestType
       */
      int requestType() const {
        return m_requestType;
      }

      /**
       * Set the value of m_requestType
       * the request type for which t privileges. 0 means all request types
       * @param new_var the new value of m_requestType
       */
      void setRequestType(int new_var) {
        m_requestType = new_var;
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

    private:

      /// the user for which t privileges. -1 means all users
      int m_user;

      /// the group for which t privileges. -1 means all groups
      int m_group;

      /// the request type for which t privileges. 0 means all request types
      int m_requestType;

      /// The id of this object
      u_signed64 m_id;

    }; /* end of class ListPrivileges */

  } /* end of namespace bwlist */

} /* end of namespace castor */

#endif // CASTOR_BWLIST_LISTPRIVILEGES_HPP

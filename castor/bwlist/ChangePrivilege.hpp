/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/bwlist/ChangePrivilege.hpp
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

#ifndef CASTOR_BWLIST_CHANGEPRIVILEGE_HPP
#define CASTOR_BWLIST_CHANGEPRIVILEGE_HPP

// Include Files
#include "castor/stager/Request.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace bwlist {

    // Forward declarations
    class RequestType;
    class BWUser;

    /**
     * class ChangePrivilege
     * Request allowing to change privileges for a set of users and a set of request
     * types, within a given serviceClass.
     * Note that an empty service class name can be used to target all service
     * classes.
     * In the same spirit, an empty list of requestTypes will mean that all types are
     * targeted.
     * Concerning the user list, an empty list means all users and all groups are
     * concerned and an entry containing a -1 as uid or gid means repectively any uid
     * or any gid.
     */
    class ChangePrivilege : public virtual castor::stager::Request {

    public:

      /**
       * Empty Constructor
       */
      ChangePrivilege() throw();

      /**
       * Empty Destructor
       */
      virtual ~ChangePrivilege() throw();

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
       * Get the value of m_isGranted
       * Whether the change has to be granted or denied
       * @return the value of m_isGranted
       */
      bool isGranted() const {
        return m_isGranted;
      }

      /**
       * Set the value of m_isGranted
       * Whether the change has to be granted or denied
       * @param new_var the new value of m_isGranted
       */
      void setIsGranted(bool new_var) {
        m_isGranted = new_var;
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
       * Add a BWUser* object to the m_usersVector list
       */
      void addUsers(BWUser* add_object) {
        m_usersVector.push_back(add_object);
      }

      /**
       * Remove a BWUser* object from m_usersVector
       */
      void removeUsers(BWUser* remove_object) {
        for (unsigned int i = 0; i < m_usersVector.size(); i++) {
          BWUser* item = m_usersVector[i];
          if (item == remove_object) {
            std::vector<BWUser*>::iterator it = m_usersVector.begin() + i;
            m_usersVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of BWUser* objects held by m_usersVector
       * @return list of BWUser* objects held by m_usersVector
       */
      std::vector<BWUser*>& users() {
        return m_usersVector;
      }

      /**
       * Add a RequestType* object to the m_requestTypesVector list
       */
      void addRequestTypes(RequestType* add_object) {
        m_requestTypesVector.push_back(add_object);
      }

      /**
       * Remove a RequestType* object from m_requestTypesVector
       */
      void removeRequestTypes(RequestType* remove_object) {
        for (unsigned int i = 0; i < m_requestTypesVector.size(); i++) {
          RequestType* item = m_requestTypesVector[i];
          if (item == remove_object) {
            std::vector<RequestType*>::iterator it = m_requestTypesVector.begin() + i;
            m_requestTypesVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of RequestType* objects held by m_requestTypesVector
       * @return list of RequestType* objects held by m_requestTypesVector
       */
      std::vector<RequestType*>& requestTypes() {
        return m_requestTypesVector;
      }

    private:

      /// Whether the change has to be granted or denied
      bool m_isGranted;

      /// The id of this object
      u_signed64 m_id;

      std::vector<BWUser*> m_usersVector;

      std::vector<RequestType*> m_requestTypesVector;

    }; /* end of class ChangePrivilege */

  } /* end of namespace bwlist */

} /* end of namespace castor */

#endif // CASTOR_BWLIST_CHANGEPRIVILEGE_HPP

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/query/DiskPoolQueryResponse.hpp
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

#ifndef CASTOR_QUERY_DISKPOOLQUERYRESPONSE_HPP
#define CASTOR_QUERY_DISKPOOLQUERYRESPONSE_HPP

// Include Files
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace query {

    // Forward declarations
    class DiskServerDescription;

    /**
     * class DiskPoolQueryResponse
     * Response to a DiskPoolQuery
     */
    class DiskPoolQueryResponse : public virtual castor::rh::Response {

    public:

      /**
       * Empty Constructor
       */
      DiskPoolQueryResponse() throw();

      /**
       * Empty Destructor
       */
      virtual ~DiskPoolQueryResponse() throw();

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
       * Get the value of m_diskPoolName
       * Name of the diskpool
       * @return the value of m_diskPoolName
       */
      std::string diskPoolName() const {
        return m_diskPoolName;
      }

      /**
       * Set the value of m_diskPoolName
       * Name of the diskpool
       * @param new_var the new value of m_diskPoolName
       */
      void setDiskPoolName(std::string new_var) {
        m_diskPoolName = new_var;
      }

      /**
       * Get the value of m_freeSpace
       * The total free space in the diskPool
       * @return the value of m_freeSpace
       */
      u_signed64 freeSpace() const {
        return m_freeSpace;
      }

      /**
       * Set the value of m_freeSpace
       * The total free space in the diskPool
       * @param new_var the new value of m_freeSpace
       */
      void setFreeSpace(u_signed64 new_var) {
        m_freeSpace = new_var;
      }

      /**
       * Get the value of m_totalSpace
       * The total space provided by the diskPool
       * @return the value of m_totalSpace
       */
      u_signed64 totalSpace() const {
        return m_totalSpace;
      }

      /**
       * Set the value of m_totalSpace
       * The total space provided by the diskPool
       * @param new_var the new value of m_totalSpace
       */
      void setTotalSpace(u_signed64 new_var) {
        m_totalSpace = new_var;
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
       * Add a DiskServerDescription* object to the m_diskServersVector list
       */
      void addDiskServers(DiskServerDescription* add_object) {
        m_diskServersVector.push_back(add_object);
      }

      /**
       * Remove a DiskServerDescription* object from m_diskServersVector
       */
      void removeDiskServers(DiskServerDescription* remove_object) {
        for (unsigned int i = 0; i < m_diskServersVector.size(); i++) {
          DiskServerDescription* item = m_diskServersVector[i];
          if (item == remove_object) {
            std::vector<DiskServerDescription*>::iterator it = m_diskServersVector.begin() + i;
            m_diskServersVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of DiskServerDescription* objects held by m_diskServersVector
       * @return list of DiskServerDescription* objects held by m_diskServersVector
       */
      std::vector<DiskServerDescription*>& diskServers() {
        return m_diskServersVector;
      }

    private:

      /// Name of the diskpool
      std::string m_diskPoolName;

      /// The total free space in the diskPool
      u_signed64 m_freeSpace;

      /// The total space provided by the diskPool
      u_signed64 m_totalSpace;

      /// The id of this object
      u_signed64 m_id;

      std::vector<DiskServerDescription*> m_diskServersVector;

    }; /* end of class DiskPoolQueryResponse */

  } /* end of namespace query */

} /* end of namespace castor */

#endif // CASTOR_QUERY_DISKPOOLQUERYRESPONSE_HPP

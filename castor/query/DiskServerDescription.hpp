/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/query/DiskServerDescription.hpp
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

#ifndef CASTOR_QUERY_DISKSERVERDESCRIPTION_HPP
#define CASTOR_QUERY_DISKSERVERDESCRIPTION_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace query {

    // Forward declarations
    class FileSystemDescription;
    class DiskPoolQueryResponse;

    /**
     * class DiskServerDescription
     * Describe a Diskserver and its current state
     */
    class DiskServerDescription : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      DiskServerDescription() throw();

      /**
       * Empty Destructor
       */
      virtual ~DiskServerDescription() throw();

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
       * Get the value of m_name
       * Name of the diskserver
       * @return the value of m_name
       */
      std::string name() const {
        return m_name;
      }

      /**
       * Set the value of m_name
       * Name of the diskserver
       * @param new_var the new value of m_name
       */
      void setName(std::string new_var) {
        m_name = new_var;
      }

      /**
       * Get the value of m_status
       * Status of the DiskServer
       * @return the value of m_status
       */
      int status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * Status of the DiskServer
       * @param new_var the new value of m_status
       */
      void setStatus(int new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_freeSpace
       * The total free space on the DiskServer
       * @return the value of m_freeSpace
       */
      u_signed64 freeSpace() const {
        return m_freeSpace;
      }

      /**
       * Set the value of m_freeSpace
       * The total free space on the DiskServer
       * @param new_var the new value of m_freeSpace
       */
      void setFreeSpace(u_signed64 new_var) {
        m_freeSpace = new_var;
      }

      /**
       * Get the value of m_totalSpace
       * The total space provided by this diskServer
       * @return the value of m_totalSpace
       */
      u_signed64 totalSpace() const {
        return m_totalSpace;
      }

      /**
       * Set the value of m_totalSpace
       * The total space provided by this diskServer
       * @param new_var the new value of m_totalSpace
       */
      void setTotalSpace(u_signed64 new_var) {
        m_totalSpace = new_var;
      }

      /**
       * Get the value of m_reservedSpace
       * Amount of reserved space. Reintroduced for backward compatibility with old
       * clients. Should be dropped in the future.
       * @return the value of m_reservedSpace
       */
      u_signed64 reservedSpace() const {
        return m_reservedSpace;
      }

      /**
       * Set the value of m_reservedSpace
       * Amount of reserved space. Reintroduced for backward compatibility with old
       * clients. Should be dropped in the future.
       * @param new_var the new value of m_reservedSpace
       */
      void setReservedSpace(u_signed64 new_var) {
        m_reservedSpace = new_var;
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
       * Get the value of m_query
       * @return the value of m_query
       */
      DiskPoolQueryResponse* query() const {
        return m_query;
      }

      /**
       * Set the value of m_query
       * @param new_var the new value of m_query
       */
      void setQuery(DiskPoolQueryResponse* new_var) {
        m_query = new_var;
      }

      /**
       * Add a FileSystemDescription* object to the m_fileSystemsVector list
       */
      void addFileSystems(FileSystemDescription* add_object) {
        m_fileSystemsVector.push_back(add_object);
      }

      /**
       * Remove a FileSystemDescription* object from m_fileSystemsVector
       */
      void removeFileSystems(FileSystemDescription* remove_object) {
        for (unsigned int i = 0; i < m_fileSystemsVector.size(); i++) {
          FileSystemDescription* item = m_fileSystemsVector[i];
          if (item == remove_object) {
            std::vector<FileSystemDescription*>::iterator it = m_fileSystemsVector.begin() + i;
            m_fileSystemsVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of FileSystemDescription* objects held by m_fileSystemsVector
       * @return list of FileSystemDescription* objects held by m_fileSystemsVector
       */
      std::vector<FileSystemDescription*>& fileSystems() {
        return m_fileSystemsVector;
      }

    private:

      /// Name of the diskserver
      std::string m_name;

      /// Status of the DiskServer
      int m_status;

      /// The total free space on the DiskServer
      u_signed64 m_freeSpace;

      /// The total space provided by this diskServer
      u_signed64 m_totalSpace;

      /// Amount of reserved space. Reintroduced for backward compatibility with old clients. Should be dropped in the future.
      u_signed64 m_reservedSpace;

      /// The id of this object
      u_signed64 m_id;

      DiskPoolQueryResponse* m_query;

      std::vector<FileSystemDescription*> m_fileSystemsVector;

    }; /* end of class DiskServerDescription */

  } /* end of namespace query */

} /* end of namespace castor */

#endif // CASTOR_QUERY_DISKSERVERDESCRIPTION_HPP

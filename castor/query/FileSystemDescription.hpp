/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/query/FileSystemDescription.hpp
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

#ifndef CASTOR_QUERY_FILESYSTEMDESCRIPTION_HPP
#define CASTOR_QUERY_FILESYSTEMDESCRIPTION_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace query {

    // Forward declarations
    class DiskServerDescription;

    /**
     * class FileSystemDescription
     */
    class FileSystemDescription : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      FileSystemDescription() throw();

      /**
       * Empty Destructor
       */
      virtual ~FileSystemDescription() throw();

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
       * Get the value of m_mountPoint
       * The mountpoint of the FileSystem
       * @return the value of m_mountPoint
       */
      std::string mountPoint() const {
        return m_mountPoint;
      }

      /**
       * Set the value of m_mountPoint
       * The mountpoint of the FileSystem
       * @param new_var the new value of m_mountPoint
       */
      void setMountPoint(std::string new_var) {
        m_mountPoint = new_var;
      }

      /**
       * Get the value of m_freeSpace
       * Amount of free space on this fileSystem (in bytes)
       * @return the value of m_freeSpace
       */
      u_signed64 freeSpace() const {
        return m_freeSpace;
      }

      /**
       * Set the value of m_freeSpace
       * Amount of free space on this fileSystem (in bytes)
       * @param new_var the new value of m_freeSpace
       */
      void setFreeSpace(u_signed64 new_var) {
        m_freeSpace = new_var;
      }

      /**
       * Get the value of m_totalSpace
       * Total size of this fileSystem (in bytes)
       * @return the value of m_totalSpace
       */
      u_signed64 totalSpace() const {
        return m_totalSpace;
      }

      /**
       * Set the value of m_totalSpace
       * Total size of this fileSystem (in bytes)
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
       * Get the value of m_minFreeSpace
       * Minimum freespace the garbage collector should keep if possible (values from 0
       * to 1, as a portion of the totalSpace)
       * @return the value of m_minFreeSpace
       */
      float minFreeSpace() const {
        return m_minFreeSpace;
      }

      /**
       * Set the value of m_minFreeSpace
       * Minimum freespace the garbage collector should keep if possible (values from 0
       * to 1, as a portion of the totalSpace)
       * @param new_var the new value of m_minFreeSpace
       */
      void setMinFreeSpace(float new_var) {
        m_minFreeSpace = new_var;
      }

      /**
       * Get the value of m_maxFreeSpace
       * Minimum freespace the garbage collector should create when it runs (values from
       * 0 to 1, as a portion of the totalSpace)
       * @return the value of m_maxFreeSpace
       */
      float maxFreeSpace() const {
        return m_maxFreeSpace;
      }

      /**
       * Set the value of m_maxFreeSpace
       * Minimum freespace the garbage collector should create when it runs (values from
       * 0 to 1, as a portion of the totalSpace)
       * @param new_var the new value of m_maxFreeSpace
       */
      void setMaxFreeSpace(float new_var) {
        m_maxFreeSpace = new_var;
      }

      /**
       * Get the value of m_status
       * status of the FileSystem
       * @return the value of m_status
       */
      int status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * status of the FileSystem
       * @param new_var the new value of m_status
       */
      void setStatus(int new_var) {
        m_status = new_var;
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
       * Get the value of m_diskServer
       * @return the value of m_diskServer
       */
      DiskServerDescription* diskServer() const {
        return m_diskServer;
      }

      /**
       * Set the value of m_diskServer
       * @param new_var the new value of m_diskServer
       */
      void setDiskServer(DiskServerDescription* new_var) {
        m_diskServer = new_var;
      }

    private:

      /// The mountpoint of the FileSystem
      std::string m_mountPoint;

      /// Amount of free space on this fileSystem (in bytes)
      u_signed64 m_freeSpace;

      /// Total size of this fileSystem (in bytes)
      u_signed64 m_totalSpace;

      /// Amount of reserved space. Reintroduced for backward compatibility with old clients. Should be dropped in the future.
      u_signed64 m_reservedSpace;

      /// Minimum freespace the garbage collector should keep if possible (values from 0 to 1, as a portion of the totalSpace)
      float m_minFreeSpace;

      /// Minimum freespace the garbage collector should create when it runs (values from 0 to 1, as a portion of the totalSpace)
      float m_maxFreeSpace;

      /// status of the FileSystem
      int m_status;

      /// The id of this object
      u_signed64 m_id;

      DiskServerDescription* m_diskServer;

    }; /* end of class FileSystemDescription */

  } /* end of namespace query */

} /* end of namespace castor */

#endif // CASTOR_QUERY_FILESYSTEMDESCRIPTION_HPP

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/FileSystem.hpp
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

#ifndef CASTOR_STAGER_FILESYSTEM_HPP
#define CASTOR_STAGER_FILESYSTEM_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/monitoring/AdminStatusCodes.hpp"
#include "castor/stager/FileSystemStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class DiskServer;
    class DiskPool;
    class DiskCopy;

    /**
     * class FileSystem
     * A file system used in a disk pool
     */
    class FileSystem : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      FileSystem() throw();

      /**
       * Empty Destructor
       */
      virtual ~FileSystem() throw();

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
       * Get the value of m_free
       * Free space on the filesystem
       * @return the value of m_free
       */
      u_signed64 free() const {
        return m_free;
      }

      /**
       * Set the value of m_free
       * Free space on the filesystem
       * @param new_var the new value of m_free
       */
      void setFree(u_signed64 new_var) {
        m_free = new_var;
      }

      /**
       * Get the value of m_mountPoint
       * @return the value of m_mountPoint
       */
      std::string mountPoint() const {
        return m_mountPoint;
      }

      /**
       * Set the value of m_mountPoint
       * @param new_var the new value of m_mountPoint
       */
      void setMountPoint(std::string new_var) {
        m_mountPoint = new_var;
      }

      /**
       * Get the value of m_minFreeSpace
       * Minimum free space that should be kept on this FileSystem. This limit can be
       * transgressed but the garbage collector will then be launched. This is given as a
       * fraction of the totalSize.
       * @return the value of m_minFreeSpace
       */
      float minFreeSpace() const {
        return m_minFreeSpace;
      }

      /**
       * Set the value of m_minFreeSpace
       * Minimum free space that should be kept on this FileSystem. This limit can be
       * transgressed but the garbage collector will then be launched. This is given as a
       * fraction of the totalSize.
       * @param new_var the new value of m_minFreeSpace
       */
      void setMinFreeSpace(float new_var) {
        m_minFreeSpace = new_var;
      }

      /**
       * Get the value of m_minAllowedFreeSpace
       * Minimum free space that shall be kept on this FileSystem. If this limit is
       * transgressed no jobs will be scheduled on this FileSystem. This is given as a
       * fraction of the totalSize.
       * @return the value of m_minAllowedFreeSpace
       */
      float minAllowedFreeSpace() const {
        return m_minAllowedFreeSpace;
      }

      /**
       * Set the value of m_minAllowedFreeSpace
       * Minimum free space that shall be kept on this FileSystem. If this limit is
       * transgressed no jobs will be scheduled on this FileSystem. This is given as a
       * fraction of the totalSize.
       * @param new_var the new value of m_minAllowedFreeSpace
       */
      void setMinAllowedFreeSpace(float new_var) {
        m_minAllowedFreeSpace = new_var;
      }

      /**
       * Get the value of m_maxFreeSpace
       * Maximum free space this FileSystem should have. Of course this limit can be
       * transgressed but a Garbage Collector should never go under this limit. This is
       * given as a fraction of the totalSize.
       * @return the value of m_maxFreeSpace
       */
      float maxFreeSpace() const {
        return m_maxFreeSpace;
      }

      /**
       * Set the value of m_maxFreeSpace
       * Maximum free space this FileSystem should have. Of course this limit can be
       * transgressed but a Garbage Collector should never go under this limit. This is
       * given as a fraction of the totalSize.
       * @param new_var the new value of m_maxFreeSpace
       */
      void setMaxFreeSpace(float new_var) {
        m_maxFreeSpace = new_var;
      }

      /**
       * Get the value of m_spaceToBeFreed
       * The space that will be deleted in the future by the GC workers. This are files
       * that were selected by the GC but are not yet physically removed. This value can
       * help another iteration of the GC to know what to delete.
       * @return the value of m_spaceToBeFreed
       */
      u_signed64 spaceToBeFreed() const {
        return m_spaceToBeFreed;
      }

      /**
       * Set the value of m_spaceToBeFreed
       * The space that will be deleted in the future by the GC workers. This are files
       * that were selected by the GC but are not yet physically removed. This value can
       * help another iteration of the GC to know what to delete.
       * @param new_var the new value of m_spaceToBeFreed
       */
      void setSpaceToBeFreed(u_signed64 new_var) {
        m_spaceToBeFreed = new_var;
      }

      /**
       * Get the value of m_totalSize
       * Total size of the fileSystem (given in bytes)
       * @return the value of m_totalSize
       */
      u_signed64 totalSize() const {
        return m_totalSize;
      }

      /**
       * Set the value of m_totalSize
       * Total size of the fileSystem (given in bytes)
       * @param new_var the new value of m_totalSize
       */
      void setTotalSize(u_signed64 new_var) {
        m_totalSize = new_var;
      }

      /**
       * Get the value of m_readRate
       * The read rate for the fileSystem
       * @return the value of m_readRate
       */
      u_signed64 readRate() const {
        return m_readRate;
      }

      /**
       * Set the value of m_readRate
       * The read rate for the fileSystem
       * @param new_var the new value of m_readRate
       */
      void setReadRate(u_signed64 new_var) {
        m_readRate = new_var;
      }

      /**
       * Get the value of m_writeRate
       * The write rate for the fileSystem
       * @return the value of m_writeRate
       */
      u_signed64 writeRate() const {
        return m_writeRate;
      }

      /**
       * Set the value of m_writeRate
       * The write rate for the fileSystem
       * @param new_var the new value of m_writeRate
       */
      void setWriteRate(u_signed64 new_var) {
        m_writeRate = new_var;
      }

      /**
       * Get the value of m_nbReadStreams
       * Nb of read streams running on this fileSystem
       * @return the value of m_nbReadStreams
       */
      unsigned int nbReadStreams() const {
        return m_nbReadStreams;
      }

      /**
       * Set the value of m_nbReadStreams
       * Nb of read streams running on this fileSystem
       * @param new_var the new value of m_nbReadStreams
       */
      void setNbReadStreams(unsigned int new_var) {
        m_nbReadStreams = new_var;
      }

      /**
       * Get the value of m_nbWriteStreams
       * Nb of write streams running on this fileSystem
       * @return the value of m_nbWriteStreams
       */
      unsigned int nbWriteStreams() const {
        return m_nbWriteStreams;
      }

      /**
       * Set the value of m_nbWriteStreams
       * Nb of write streams running on this fileSystem
       * @param new_var the new value of m_nbWriteStreams
       */
      void setNbWriteStreams(unsigned int new_var) {
        m_nbWriteStreams = new_var;
      }

      /**
       * Get the value of m_nbReadWriteStreams
       * Nb of read/write streams running on this fileSystem
       * @return the value of m_nbReadWriteStreams
       */
      unsigned int nbReadWriteStreams() const {
        return m_nbReadWriteStreams;
      }

      /**
       * Set the value of m_nbReadWriteStreams
       * Nb of read/write streams running on this fileSystem
       * @param new_var the new value of m_nbReadWriteStreams
       */
      void setNbReadWriteStreams(unsigned int new_var) {
        m_nbReadWriteStreams = new_var;
      }

      /**
       * Get the value of m_nbMigratorStreams
       * Nb of migrator streams running on this fileSystem
       * @return the value of m_nbMigratorStreams
       */
      unsigned int nbMigratorStreams() const {
        return m_nbMigratorStreams;
      }

      /**
       * Set the value of m_nbMigratorStreams
       * Nb of migrator streams running on this fileSystem
       * @param new_var the new value of m_nbMigratorStreams
       */
      void setNbMigratorStreams(unsigned int new_var) {
        m_nbMigratorStreams = new_var;
      }

      /**
       * Get the value of m_nbRecallerStreams
       * Nb of recaller streams running on this fileSystem
       * @return the value of m_nbRecallerStreams
       */
      unsigned int nbRecallerStreams() const {
        return m_nbRecallerStreams;
      }

      /**
       * Set the value of m_nbRecallerStreams
       * Nb of recaller streams running on this fileSystem
       * @param new_var the new value of m_nbRecallerStreams
       */
      void setNbRecallerStreams(unsigned int new_var) {
        m_nbRecallerStreams = new_var;
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
       * Get the value of m_diskPool
       * @return the value of m_diskPool
       */
      DiskPool* diskPool() const {
        return m_diskPool;
      }

      /**
       * Set the value of m_diskPool
       * @param new_var the new value of m_diskPool
       */
      void setDiskPool(DiskPool* new_var) {
        m_diskPool = new_var;
      }

      /**
       * Add a DiskCopy* object to the m_copiesVector list
       */
      void addCopies(DiskCopy* add_object) {
        m_copiesVector.push_back(add_object);
      }

      /**
       * Remove a DiskCopy* object from m_copiesVector
       */
      void removeCopies(DiskCopy* remove_object) {
        for (unsigned int i = 0; i < m_copiesVector.size(); i++) {
          DiskCopy* item = m_copiesVector[i];
          if (item == remove_object) {
            std::vector<DiskCopy*>::iterator it = m_copiesVector.begin() + i;
            m_copiesVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of DiskCopy* objects held by m_copiesVector
       * @return list of DiskCopy* objects held by m_copiesVector
       */
      std::vector<DiskCopy*>& copies() {
        return m_copiesVector;
      }

      /**
       * Get the value of m_diskserver
       * @return the value of m_diskserver
       */
      DiskServer* diskserver() const {
        return m_diskserver;
      }

      /**
       * Set the value of m_diskserver
       * @param new_var the new value of m_diskserver
       */
      void setDiskserver(DiskServer* new_var) {
        m_diskserver = new_var;
      }

      /**
       * Get the value of m_status
       * @return the value of m_status
       */
      FileSystemStatusCodes status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(FileSystemStatusCodes new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_adminStatus
       * Admin status of the FileSystem
       * @return the value of m_adminStatus
       */
      castor::monitoring::AdminStatusCodes adminStatus() const {
        return m_adminStatus;
      }

      /**
       * Set the value of m_adminStatus
       * Admin status of the FileSystem
       * @param new_var the new value of m_adminStatus
       */
      void setAdminStatus(castor::monitoring::AdminStatusCodes new_var) {
        m_adminStatus = new_var;
      }

    private:

      /// Free space on the filesystem
      u_signed64 m_free;

      std::string m_mountPoint;

      /// Minimum free space that should be kept on this FileSystem. This limit can be transgressed but the garbage collector will then be launched. This is given as a fraction of the totalSize.
      float m_minFreeSpace;

      /// Minimum free space that shall be kept on this FileSystem. If this limit is transgressed no jobs will be scheduled on this FileSystem. This is given as a fraction of the totalSize.
      float m_minAllowedFreeSpace;

      /// Maximum free space this FileSystem should have. Of course this limit can be transgressed but a Garbage Collector should never go under this limit. This is given as a fraction of the totalSize.
      float m_maxFreeSpace;

      /// The space that will be deleted in the future by the GC workers. This are files that were selected by the GC but are not yet physically removed. This value can help another iteration of the GC to know what to delete.
      u_signed64 m_spaceToBeFreed;

      /// Total size of the fileSystem (given in bytes)
      u_signed64 m_totalSize;

      /// The read rate for the fileSystem
      u_signed64 m_readRate;

      /// The write rate for the fileSystem
      u_signed64 m_writeRate;

      /// Nb of read streams running on this fileSystem
      unsigned int m_nbReadStreams;

      /// Nb of write streams running on this fileSystem
      unsigned int m_nbWriteStreams;

      /// Nb of read/write streams running on this fileSystem
      unsigned int m_nbReadWriteStreams;

      /// Nb of migrator streams running on this fileSystem
      unsigned int m_nbMigratorStreams;

      /// Nb of recaller streams running on this fileSystem
      unsigned int m_nbRecallerStreams;

      /// The id of this object
      u_signed64 m_id;

      DiskPool* m_diskPool;

      std::vector<DiskCopy*> m_copiesVector;

      DiskServer* m_diskserver;

      FileSystemStatusCodes m_status;

      /// Admin status of the FileSystem
      castor::monitoring::AdminStatusCodes m_adminStatus;

    }; /* end of class FileSystem */

  } /* end of namespace stager */

} /* end of namespace castor */

#endif // CASTOR_STAGER_FILESYSTEM_HPP

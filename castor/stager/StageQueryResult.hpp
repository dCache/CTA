/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/StageQueryResult.hpp
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

#ifndef CASTOR_STAGER_STAGEQUERYRESULT_HPP
#define CASTOR_STAGER_STAGEQUERYRESULT_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    /**
     * class StageQueryResult
     * Class used as the result set for any StageFileQueryRequest's. It does not need
     * to be streamable.
     */
    class StageQueryResult : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      StageQueryResult() throw();

      /**
       * Empty Destructor
       */
      virtual ~StageQueryResult() throw();

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
       * Get the value of m_fileId
       * fileId of the underlying castorfile
       * @return the value of m_fileId
       */
      u_signed64 fileId() const {
        return m_fileId;
      }

      /**
       * Set the value of m_fileId
       * fileId of the underlying castorfile
       * @param new_var the new value of m_fileId
       */
      void setFileId(u_signed64 new_var) {
        m_fileId = new_var;
      }

      /**
       * Get the value of m_nsHost
       * name server host of the underlying castorfile
       * @return the value of m_nsHost
       */
      std::string nsHost() const {
        return m_nsHost;
      }

      /**
       * Set the value of m_nsHost
       * name server host of the underlying castorfile
       * @param new_var the new value of m_nsHost
       */
      void setNsHost(std::string new_var) {
        m_nsHost = new_var;
      }

      /**
       * Get the value of m_diskCopyPath
       * Physical path where the described diskCopy resides
       * @return the value of m_diskCopyPath
       */
      std::string diskCopyPath() const {
        return m_diskCopyPath;
      }

      /**
       * Set the value of m_diskCopyPath
       * Physical path where the described diskCopy resides
       * @param new_var the new value of m_diskCopyPath
       */
      void setDiskCopyPath(std::string new_var) {
        m_diskCopyPath = new_var;
      }

      /**
       * Get the value of m_size
       * Size of the underlying CastorFile
       * @return the value of m_size
       */
      u_signed64 size() const {
        return m_size;
      }

      /**
       * Set the value of m_size
       * Size of the underlying CastorFile
       * @param new_var the new value of m_size
       */
      void setSize(u_signed64 new_var) {
        m_size = new_var;
      }

      /**
       * Get the value of m_diskCopyStatus
       * Status of the DiskCopy described
       * @return the value of m_diskCopyStatus
       */
      int diskCopyStatus() const {
        return m_diskCopyStatus;
      }

      /**
       * Set the value of m_diskCopyStatus
       * Status of the DiskCopy described
       * @param new_var the new value of m_diskCopyStatus
       */
      void setDiskCopyStatus(int new_var) {
        m_diskCopyStatus = new_var;
      }

      /**
       * Get the value of m_hwStatus
       * Status of the hardware where the described diskCopy resides.
       * @return the value of m_hwStatus
       */
      int hwStatus() const {
        return m_hwStatus;
      }

      /**
       * Set the value of m_hwStatus
       * Status of the hardware where the described diskCopy resides.
       * @param new_var the new value of m_hwStatus
       */
      void setHwStatus(int new_var) {
        m_hwStatus = new_var;
      }

      /**
       * Get the value of m_diskServer
       * Name of the DiskServer on which the Diskcopy resides
       * @return the value of m_diskServer
       */
      std::string diskServer() const {
        return m_diskServer;
      }

      /**
       * Set the value of m_diskServer
       * Name of the DiskServer on which the Diskcopy resides
       * @param new_var the new value of m_diskServer
       */
      void setDiskServer(std::string new_var) {
        m_diskServer = new_var;
      }

      /**
       * Get the value of m_mountPoint
       * mount point of the file system where the DiskCopy resides on the diskserver
       * @return the value of m_mountPoint
       */
      std::string mountPoint() const {
        return m_mountPoint;
      }

      /**
       * Set the value of m_mountPoint
       * mount point of the file system where the DiskCopy resides on the diskserver
       * @param new_var the new value of m_mountPoint
       */
      void setMountPoint(std::string new_var) {
        m_mountPoint = new_var;
      }

      /**
       * Get the value of m_nbAccesses
       * Number of accesses to the CastorFile this DiskCopy is linked with
       * @return the value of m_nbAccesses
       */
      unsigned int nbAccesses() const {
        return m_nbAccesses;
      }

      /**
       * Set the value of m_nbAccesses
       * Number of accesses to the CastorFile this DiskCopy is linked with
       * @param new_var the new value of m_nbAccesses
       */
      void setNbAccesses(unsigned int new_var) {
        m_nbAccesses = new_var;
      }

      /**
       * Get the value of m_lastKnownFileName
       * Last known file name for this file, from the stager point of view. This does not
       * mean it is the current one in the nameserver
       * @return the value of m_lastKnownFileName
       */
      std::string lastKnownFileName() const {
        return m_lastKnownFileName;
      }

      /**
       * Set the value of m_lastKnownFileName
       * Last known file name for this file, from the stager point of view. This does not
       * mean it is the current one in the nameserver
       * @param new_var the new value of m_lastKnownFileName
       */
      void setLastKnownFileName(std::string new_var) {
        m_lastKnownFileName = new_var;
      }

      /**
       * Get the value of m_svcClass
       * The service class that the diskcopy is in
       * @return the value of m_svcClass
       */
      std::string svcClass() const {
        return m_svcClass;
      }

      /**
       * Set the value of m_svcClass
       * The service class that the diskcopy is in
       * @param new_var the new value of m_svcClass
       */
      void setSvcClass(std::string new_var) {
        m_svcClass = new_var;
      }

      /**
       * Get the value of m_creationTime
       * The creation time of the diskcopy
       * @return the value of m_creationTime
       */
      u_signed64 creationTime() const {
        return m_creationTime;
      }

      /**
       * Set the value of m_creationTime
       * The creation time of the diskcopy
       * @param new_var the new value of m_creationTime
       */
      void setCreationTime(u_signed64 new_var) {
        m_creationTime = new_var;
      }

      /**
       * Get the value of m_lastAccessTime
       * The last access time of the diskcopy
       * @return the value of m_lastAccessTime
       */
      u_signed64 lastAccessTime() const {
        return m_lastAccessTime;
      }

      /**
       * Set the value of m_lastAccessTime
       * The last access time of the diskcopy
       * @param new_var the new value of m_lastAccessTime
       */
      void setLastAccessTime(u_signed64 new_var) {
        m_lastAccessTime = new_var;
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

      /// fileId of the underlying castorfile
      u_signed64 m_fileId;

      /// name server host of the underlying castorfile
      std::string m_nsHost;

      /// Physical path where the described diskCopy resides
      std::string m_diskCopyPath;

      /// Size of the underlying CastorFile
      u_signed64 m_size;

      /// Status of the DiskCopy described
      int m_diskCopyStatus;

      /// Status of the hardware where the described diskCopy resides.
      int m_hwStatus;

      /// Name of the DiskServer on which the Diskcopy resides
      std::string m_diskServer;

      /// mount point of the file system where the DiskCopy resides on the diskserver
      std::string m_mountPoint;

      /// Number of accesses to the CastorFile this DiskCopy is linked with
      unsigned int m_nbAccesses;

      /// Last known file name for this file, from the stager point of view. This does not mean it is the current one in the nameserver
      std::string m_lastKnownFileName;

      /// The service class that the diskcopy is in
      std::string m_svcClass;

      /// The creation time of the diskcopy
      u_signed64 m_creationTime;

      /// The last access time of the diskcopy
      u_signed64 m_lastAccessTime;

      /// The id of this object
      u_signed64 m_id;

    }; /* end of class StageQueryResult */

  } /* end of namespace stager */

} /* end of namespace castor */

#endif // CASTOR_STAGER_STAGEQUERYRESULT_HPP

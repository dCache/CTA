/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/SvcClass.hpp
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

#ifndef CASTOR_STAGER_SVCCLASS_HPP
#define CASTOR_STAGER_SVCCLASS_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class TapePool;
    class FileClass;
    class DiskPool;

    /**
     * class SvcClass
     * A service, as seen by the user.
     * A SvcClass is a container of resources and may be given as parameter of the
     * request.
     */
    class SvcClass : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      SvcClass() throw();

      /**
       * Empty Destructor
       */
      virtual ~SvcClass() throw();

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
       * Get the value of m_nbDrives
       * Number of drives to use for this service class.
       * This is the default number, but it could be that occasionnally more drives are
       * used, if a resource is shared with another service class using more drives
       * @return the value of m_nbDrives
       */
      unsigned int nbDrives() const {
        return m_nbDrives;
      }

      /**
       * Set the value of m_nbDrives
       * Number of drives to use for this service class.
       * This is the default number, but it could be that occasionnally more drives are
       * used, if a resource is shared with another service class using more drives
       * @param new_var the new value of m_nbDrives
       */
      void setNbDrives(unsigned int new_var) {
        m_nbDrives = new_var;
      }

      /**
       * Get the value of m_name
       * the name of this SvcClass
       * @return the value of m_name
       */
      std::string name() const {
        return m_name;
      }

      /**
       * Set the value of m_name
       * the name of this SvcClass
       * @param new_var the new value of m_name
       */
      void setName(std::string new_var) {
        m_name = new_var;
      }

      /**
       * Get the value of m_defaultFileSize
       * Default size used for space allocation in the case of a stage put with no size
       * explicitely given (ie size given was 0)
       * @return the value of m_defaultFileSize
       */
      u_signed64 defaultFileSize() const {
        return m_defaultFileSize;
      }

      /**
       * Set the value of m_defaultFileSize
       * Default size used for space allocation in the case of a stage put with no size
       * explicitely given (ie size given was 0)
       * @param new_var the new value of m_defaultFileSize
       */
      void setDefaultFileSize(u_signed64 new_var) {
        m_defaultFileSize = new_var;
      }

      /**
       * Get the value of m_maxReplicaNb
       * Maximum number of replicas for a file created with this service class. If 0, the
       * replication is ruled by the replication policy. If there is none, then 0 means
       * infinity.
       * @return the value of m_maxReplicaNb
       */
      int maxReplicaNb() const {
        return m_maxReplicaNb;
      }

      /**
       * Set the value of m_maxReplicaNb
       * Maximum number of replicas for a file created with this service class. If 0, the
       * replication is ruled by the replication policy. If there is none, then 0 means
       * infinity.
       * @param new_var the new value of m_maxReplicaNb
       */
      void setMaxReplicaNb(int new_var) {
        m_maxReplicaNb = new_var;
      }

      /**
       * Get the value of m_replicationPolicy
       * Policy ruling the replication of files in the diskpools.
       * @return the value of m_replicationPolicy
       */
      std::string replicationPolicy() const {
        return m_replicationPolicy;
      }

      /**
       * Set the value of m_replicationPolicy
       * Policy ruling the replication of files in the diskpools.
       * @param new_var the new value of m_replicationPolicy
       */
      void setReplicationPolicy(std::string new_var) {
        m_replicationPolicy = new_var;
      }

      /**
       * Get the value of m_gcPolicy
       * Policy ruling the garbage collection of files
       * @return the value of m_gcPolicy
       */
      std::string gcPolicy() const {
        return m_gcPolicy;
      }

      /**
       * Set the value of m_gcPolicy
       * Policy ruling the garbage collection of files
       * @param new_var the new value of m_gcPolicy
       */
      void setGcPolicy(std::string new_var) {
        m_gcPolicy = new_var;
      }

      /**
       * Get the value of m_migratorPolicy
       * Policy ruling the migration of files.
       * @return the value of m_migratorPolicy
       */
      std::string migratorPolicy() const {
        return m_migratorPolicy;
      }

      /**
       * Set the value of m_migratorPolicy
       * Policy ruling the migration of files.
       * @param new_var the new value of m_migratorPolicy
       */
      void setMigratorPolicy(std::string new_var) {
        m_migratorPolicy = new_var;
      }

      /**
       * Get the value of m_recallerPolicy
       * Policy ruling the recall of files.
       * @return the value of m_recallerPolicy
       */
      std::string recallerPolicy() const {
        return m_recallerPolicy;
      }

      /**
       * Set the value of m_recallerPolicy
       * Policy ruling the recall of files.
       * @param new_var the new value of m_recallerPolicy
       */
      void setRecallerPolicy(std::string new_var) {
        m_recallerPolicy = new_var;
      }

      /**
       * Get the value of m_hasDiskOnlyBehavior
       * Whether the diskpools under this serviceClass should behave like disk only
       * pools. This include failing jobs that want to allocate space when no space is
       * available and forcing the fileClass of files if forcedFileClass is not empty
       * @return the value of m_hasDiskOnlyBehavior
       */
      bool hasDiskOnlyBehavior() const {
        return m_hasDiskOnlyBehavior;
      }

      /**
       * Set the value of m_hasDiskOnlyBehavior
       * Whether the diskpools under this serviceClass should behave like disk only
       * pools. This include failing jobs that want to allocate space when no space is
       * available and forcing the fileClass of files if forcedFileClass is not empty
       * @param new_var the new value of m_hasDiskOnlyBehavior
       */
      void setHasDiskOnlyBehavior(bool new_var) {
        m_hasDiskOnlyBehavior = new_var;
      }

      /**
       * Get the value of m_streamPolicy
       * @return the value of m_streamPolicy
       */
      std::string streamPolicy() const {
        return m_streamPolicy;
      }

      /**
       * Set the value of m_streamPolicy
       * @param new_var the new value of m_streamPolicy
       */
      void setStreamPolicy(std::string new_var) {
        m_streamPolicy = new_var;
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
       * Add a TapePool* object to the m_tapePoolsVector list
       */
      void addTapePools(TapePool* add_object) {
        m_tapePoolsVector.push_back(add_object);
      }

      /**
       * Remove a TapePool* object from m_tapePoolsVector
       */
      void removeTapePools(TapePool* remove_object) {
        for (unsigned int i = 0; i < m_tapePoolsVector.size(); i++) {
          TapePool* item = m_tapePoolsVector[i];
          if (item == remove_object) {
            std::vector<TapePool*>::iterator it = m_tapePoolsVector.begin() + i;
            m_tapePoolsVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of TapePool* objects held by m_tapePoolsVector
       * @return list of TapePool* objects held by m_tapePoolsVector
       */
      std::vector<TapePool*>& tapePools() {
        return m_tapePoolsVector;
      }

      /**
       * Add a DiskPool* object to the m_diskPoolsVector list
       */
      void addDiskPools(DiskPool* add_object) {
        m_diskPoolsVector.push_back(add_object);
      }

      /**
       * Remove a DiskPool* object from m_diskPoolsVector
       */
      void removeDiskPools(DiskPool* remove_object) {
        for (unsigned int i = 0; i < m_diskPoolsVector.size(); i++) {
          DiskPool* item = m_diskPoolsVector[i];
          if (item == remove_object) {
            std::vector<DiskPool*>::iterator it = m_diskPoolsVector.begin() + i;
            m_diskPoolsVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of DiskPool* objects held by m_diskPoolsVector
       * @return list of DiskPool* objects held by m_diskPoolsVector
       */
      std::vector<DiskPool*>& diskPools() {
        return m_diskPoolsVector;
      }

      /**
       * Get the value of m_forcedFileClass
       * @return the value of m_forcedFileClass
       */
      FileClass* forcedFileClass() const {
        return m_forcedFileClass;
      }

      /**
       * Set the value of m_forcedFileClass
       * @param new_var the new value of m_forcedFileClass
       */
      void setForcedFileClass(FileClass* new_var) {
        m_forcedFileClass = new_var;
      }

    private:

      /*
       * Number of drives to use for this service class.
       * This is the default number, but it could be that occasionnally more drives are used, if a resource is shared with another service class using more drives
      */
      unsigned int m_nbDrives;

      /// the name of this SvcClass
      std::string m_name;

      /// Default size used for space allocation in the case of a stage put with no size explicitely given (ie size given was 0)
      u_signed64 m_defaultFileSize;

      /// Maximum number of replicas for a file created with this service class. If 0, the replication is ruled by the replication policy. If there is none, then 0 means infinity.
      int m_maxReplicaNb;

      /// Policy ruling the replication of files in the diskpools.
      std::string m_replicationPolicy;

      /// Policy ruling the garbage collection of files
      std::string m_gcPolicy;

      /// Policy ruling the migration of files.
      std::string m_migratorPolicy;

      /// Policy ruling the recall of files.
      std::string m_recallerPolicy;

      /// Whether the diskpools under this serviceClass should behave like disk only pools. This include failing jobs that want to allocate space when no space is available and forcing the fileClass of files if forcedFileClass is not empty
      bool m_hasDiskOnlyBehavior;

      std::string m_streamPolicy;

      /// The id of this object
      u_signed64 m_id;

      std::vector<TapePool*> m_tapePoolsVector;

      std::vector<DiskPool*> m_diskPoolsVector;

      /// In case hasDiskOnlyBehavior is set, this is the file class that will be used for all files created in this svcclass, independently of the fileclass of the directory where they are created
      FileClass* m_forcedFileClass;

      /// In case hasDiskOnlyBehavior is set, this is the file class that will be used for all files created in this svcclass, independently of the fileclass of the directory where they are created
      /// In case hasDiskOnlyBehavior is set, this is the file class that will be used for all files created in this svcclass, independently of the fileclass of the directory where they are created
    }; /* end of class SvcClass */

  } /* end of namespace stager */

} /* end of namespace castor */

#endif // CASTOR_STAGER_SVCCLASS_HPP

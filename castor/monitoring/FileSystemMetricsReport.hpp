/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/monitoring/FileSystemMetricsReport.hpp
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

#ifndef CASTOR_MONITORING_FILESYSTEMMETRICSREPORT_HPP
#define CASTOR_MONITORING_FILESYSTEMMETRICSREPORT_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace monitoring {

    // Forward declarations
    class DiskServerMetricsReport;

    /**
     * class FileSystemMetricsReport
     * All metrics concerning a filesystem
     */
    class FileSystemMetricsReport : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      FileSystemMetricsReport() throw();

      /**
       * Empty Destructor
       */
      virtual ~FileSystemMetricsReport() throw();

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
       * Get the value of m_writeRate
       * The mount point of the FileSystem
       * @return the value of m_writeRate
       */
      u_signed64 writeRate() const {
        return m_writeRate;
      }

      /**
       * Set the value of m_writeRate
       * The mount point of the FileSystem
       * @param new_var the new value of m_writeRate
       */
      void setWriteRate(u_signed64 new_var) {
        m_writeRate = new_var;
      }

      /**
       * Get the value of m_mountPoint
       * The number of bytes read from the filesystem per second
       * @return the value of m_mountPoint
       */
      std::string mountPoint() const {
        return m_mountPoint;
      }

      /**
       * Set the value of m_mountPoint
       * The number of bytes read from the filesystem per second
       * @param new_var the new value of m_mountPoint
       */
      void setMountPoint(std::string new_var) {
        m_mountPoint = new_var;
      }

      /**
       * Get the value of m_readRate
       * The number of bytes written to the filesystem per second
       * @return the value of m_readRate
       */
      u_signed64 readRate() const {
        return m_readRate;
      }

      /**
       * Set the value of m_readRate
       * The number of bytes written to the filesystem per second
       * @param new_var the new value of m_readRate
       */
      void setReadRate(u_signed64 new_var) {
        m_readRate = new_var;
      }

      /**
       * Get the value of m_nbReadStreams
       * The number of read streams accessing the filesystem
       * @return the value of m_nbReadStreams
       */
      unsigned int nbReadStreams() const {
        return m_nbReadStreams;
      }

      /**
       * Set the value of m_nbReadStreams
       * The number of read streams accessing the filesystem
       * @param new_var the new value of m_nbReadStreams
       */
      void setNbReadStreams(unsigned int new_var) {
        m_nbReadStreams = new_var;
      }

      /**
       * Get the value of m_nbWriteStreams
       * The number of write streams accessing the filesystem
       * @return the value of m_nbWriteStreams
       */
      unsigned int nbWriteStreams() const {
        return m_nbWriteStreams;
      }

      /**
       * Set the value of m_nbWriteStreams
       * The number of write streams accessing the filesystem
       * @param new_var the new value of m_nbWriteStreams
       */
      void setNbWriteStreams(unsigned int new_var) {
        m_nbWriteStreams = new_var;
      }

      /**
       * Get the value of m_nbReadWriteStreams
       * The number of read-write streams accessing the filesystem
       * @return the value of m_nbReadWriteStreams
       */
      unsigned int nbReadWriteStreams() const {
        return m_nbReadWriteStreams;
      }

      /**
       * Set the value of m_nbReadWriteStreams
       * The number of read-write streams accessing the filesystem
       * @param new_var the new value of m_nbReadWriteStreams
       */
      void setNbReadWriteStreams(unsigned int new_var) {
        m_nbReadWriteStreams = new_var;
      }

      /**
       * Get the value of m_freeSpace
       * Amount of free space in bytes
       * @return the value of m_freeSpace
       */
      u_signed64 freeSpace() const {
        return m_freeSpace;
      }

      /**
       * Set the value of m_freeSpace
       * Amount of free space in bytes
       * @param new_var the new value of m_freeSpace
       */
      void setFreeSpace(u_signed64 new_var) {
        m_freeSpace = new_var;
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
      DiskServerMetricsReport* diskServer() const {
        return m_diskServer;
      }

      /**
       * Set the value of m_diskServer
       * @param new_var the new value of m_diskServer
       */
      void setDiskServer(DiskServerMetricsReport* new_var) {
        m_diskServer = new_var;
      }

    private:

      /// The mount point of the FileSystem
      u_signed64 m_writeRate;

      /// The number of bytes read from the filesystem per second
      std::string m_mountPoint;

      /// The number of bytes written to the filesystem per second
      u_signed64 m_readRate;

      /// The number of read streams accessing the filesystem
      unsigned int m_nbReadStreams;

      /// The number of write streams accessing the filesystem
      unsigned int m_nbWriteStreams;

      /// The number of read-write streams accessing the filesystem
      unsigned int m_nbReadWriteStreams;

      /// Amount of free space in bytes
      u_signed64 m_freeSpace;

      /// The id of this object
      u_signed64 m_id;

      DiskServerMetricsReport* m_diskServer;

    }; /* end of class FileSystemMetricsReport */

  } /* end of namespace monitoring */

} /* end of namespace castor */

#endif // CASTOR_MONITORING_FILESYSTEMMETRICSREPORT_HPP

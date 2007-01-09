/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/monitoring/DiskServerStateReport.hpp
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

#ifndef CASTOR_MONITORING_DISKSERVERSTATEREPORT_HPP
#define CASTOR_MONITORING_DISKSERVERSTATEREPORT_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/monitoring/AdminStatusCodes.hpp"
#include "castor/stager/DiskServerStatusCode.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace monitoring {

    // Forward declarations
    class FileSystemStateReport;

    /**
     * class DiskServerStateReport
     * Describe the state of a DiskServer
     */
    class DiskServerStateReport : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      DiskServerStateReport() throw();

      /**
       * Empty Destructor
       */
      virtual ~DiskServerStateReport() throw();

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
       * The name of the DiskServer
       * @return the value of m_name
       */
      std::string name() const {
        return m_name;
      }

      /**
       * Set the value of m_name
       * The name of the DiskServer
       * @param new_var the new value of m_name
       */
      void setName(std::string new_var) {
        m_name = new_var;
      }

      /**
       * Get the value of m_ram
       * Total amount of ram
       * @return the value of m_ram
       */
      u_signed64 ram() const {
        return m_ram;
      }

      /**
       * Set the value of m_ram
       * Total amount of ram
       * @param new_var the new value of m_ram
       */
      void setRam(u_signed64 new_var) {
        m_ram = new_var;
      }

      /**
       * Get the value of m_memory
       * Total amount of memory
       * @return the value of m_memory
       */
      u_signed64 memory() const {
        return m_memory;
      }

      /**
       * Set the value of m_memory
       * Total amount of memory
       * @param new_var the new value of m_memory
       */
      void setMemory(u_signed64 new_var) {
        m_memory = new_var;
      }

      /**
       * Get the value of m_swap
       * Total amount of swap
       * @return the value of m_swap
       */
      u_signed64 swap() const {
        return m_swap;
      }

      /**
       * Set the value of m_swap
       * Total amount of swap
       * @param new_var the new value of m_swap
       */
      void setSwap(u_signed64 new_var) {
        m_swap = new_var;
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
       * Get the value of m_status
       * @return the value of m_status
       */
      castor::stager::DiskServerStatusCode status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(castor::stager::DiskServerStatusCode new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_adminStatus
       * @return the value of m_adminStatus
       */
      AdminStatusCodes adminStatus() const {
        return m_adminStatus;
      }

      /**
       * Set the value of m_adminStatus
       * @param new_var the new value of m_adminStatus
       */
      void setAdminStatus(AdminStatusCodes new_var) {
        m_adminStatus = new_var;
      }

      /**
       * Add a FileSystemStateReport* object to the m_FileSystemStatesReportsVector list
       */
      void addFileSystemStatesReports(FileSystemStateReport* add_object) {
        m_FileSystemStatesReportsVector.push_back(add_object);
      }

      /**
       * Remove a FileSystemStateReport* object from m_FileSystemStatesReportsVector
       */
      void removeFileSystemStatesReports(FileSystemStateReport* remove_object) {
        for (unsigned int i = 0; i < m_FileSystemStatesReportsVector.size(); i++) {
          FileSystemStateReport* item = m_FileSystemStatesReportsVector[i];
          if (item == remove_object) {
            std::vector<FileSystemStateReport*>::iterator it = m_FileSystemStatesReportsVector.begin() + i;
            m_FileSystemStatesReportsVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of FileSystemStateReport* objects held by
       * m_FileSystemStatesReportsVector
       * @return list of FileSystemStateReport* objects held by
       * m_FileSystemStatesReportsVector
       */
      std::vector<FileSystemStateReport*>& FileSystemStatesReports() {
        return m_FileSystemStatesReportsVector;
      }

    private:

      /// The name of the DiskServer
      std::string m_name;

      /// Total amount of ram
      u_signed64 m_ram;

      /// Total amount of memory
      u_signed64 m_memory;

      /// Total amount of swap
      u_signed64 m_swap;

      /// The id of this object
      u_signed64 m_id;

      castor::stager::DiskServerStatusCode m_status;

      AdminStatusCodes m_adminStatus;

      std::vector<FileSystemStateReport*> m_FileSystemStatesReportsVector;

    }; // end of class DiskServerStateReport

  } // end of namespace monitoring

} // end of namespace castor

#endif // CASTOR_MONITORING_DISKSERVERSTATEREPORT_HPP

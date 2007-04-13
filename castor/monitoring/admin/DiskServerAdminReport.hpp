/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/monitoring/admin/DiskServerAdminReport.hpp
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

#ifndef CASTOR_MONITORING_ADMIN_DISKSERVERADMINREPORT_HPP
#define CASTOR_MONITORING_ADMIN_DISKSERVERADMINREPORT_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/monitoring/AdminStatusCodes.hpp"
#include "castor/stager/DiskServerStatusCode.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace monitoring {

    namespace admin {

      /**
       * class DiskServerAdminReport
       * DiskServer re;ated report that allows to modify the staus and admin status
       */
      class DiskServerAdminReport : public virtual castor::IObject {

      public:

        /**
         * Empty Constructor
         */
        DiskServerAdminReport() throw();

        /**
         * Empty Destructor
         */
        virtual ~DiskServerAdminReport() throw();

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
         * Get the value of m_diskServerName
         * Name of the diskServer
         * @return the value of m_diskServerName
         */
        std::string diskServerName() const {
          return m_diskServerName;
        }

        /**
         * Set the value of m_diskServerName
         * Name of the diskServer
         * @param new_var the new value of m_diskServerName
         */
        void setDiskServerName(std::string new_var) {
          m_diskServerName = new_var;
        }

        /**
         * Get the value of m_recursive
         * Whether to update recursively the status of the filesystems.
         * @return the value of m_recursive
         */
        bool recursive() const {
          return m_recursive;
        }

        /**
         * Set the value of m_recursive
         * Whether to update recursively the status of the filesystems.
         * @param new_var the new value of m_recursive
         */
        void setRecursive(bool new_var) {
          m_recursive = new_var;
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
         * Get the value of m_adminStatus
         * @return the value of m_adminStatus
         */
        castor::monitoring::AdminStatusCodes adminStatus() const {
          return m_adminStatus;
        }

        /**
         * Set the value of m_adminStatus
         * @param new_var the new value of m_adminStatus
         */
        void setAdminStatus(castor::monitoring::AdminStatusCodes new_var) {
          m_adminStatus = new_var;
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

      private:

        /// Name of the diskServer
        std::string m_diskServerName;

        /// Whether to update recursively the status of the filesystems.
        bool m_recursive;

        /// The id of this object
        u_signed64 m_id;

        castor::monitoring::AdminStatusCodes m_adminStatus;

        castor::stager::DiskServerStatusCode m_status;

      }; /* end of class DiskServerAdminReport */

    } /* end of namespace admin */

  } /* end of namespace monitoring */

} /* end of namespace castor */

#endif // CASTOR_MONITORING_ADMIN_DISKSERVERADMINREPORT_HPP

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/FileRecalledNotification.hpp
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

#ifndef CASTOR_TAPE_TAPEGATEWAY_FILERECALLEDNOTIFICATION_HPP
#define CASTOR_TAPE_TAPEGATEWAY_FILERECALLEDNOTIFICATION_HPP

// Include Files
#include "castor/tape/tapegateway/BaseFileInfo.hpp"
#include "osdep.h"
#include <string>

namespace castor {

  namespace tape {

    namespace tapegateway {

      /**
       * class FileRecalledNotification
       */
      class FileRecalledNotification : public BaseFileInfo {

      public:

        /**
         * Empty Constructor
         */
        FileRecalledNotification() throw();

        /**
         * Empty Destructor
         */
        virtual ~FileRecalledNotification() throw();

        /**
         * Get the value of m_path
         * @return the value of m_path
         */
        std::string path() const {
          return m_path;
        }

        /**
         * Set the value of m_path
         * @param new_var the new value of m_path
         */
        void setPath(std::string new_var) {
          m_path = new_var;
        }

        /**
         * Get the value of m_checksumName
         * @return the value of m_checksumName
         */
        std::string checksumName() const {
          return m_checksumName;
        }

        /**
         * Set the value of m_checksumName
         * @param new_var the new value of m_checksumName
         */
        void setChecksumName(std::string new_var) {
          m_checksumName = new_var;
        }

        /**
         * Get the value of m_checksum
         * @return the value of m_checksum
         */
        u_signed64 checksum() const {
          return m_checksum;
        }

        /**
         * Set the value of m_checksum
         * @param new_var the new value of m_checksum
         */
        void setChecksum(u_signed64 new_var) {
          m_checksum = new_var;
        }

      private:

        std::string m_path;

        std::string m_checksumName;

        u_signed64 m_checksum;

      }; /* end of class FileRecalledNotification */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_FILERECALLEDNOTIFICATION_HPP

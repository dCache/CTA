/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/FileMigratedNotification.hpp
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

#ifndef CASTOR_TAPE_TAPEGATEWAY_FILEMIGRATEDNOTIFICATION_HPP
#define CASTOR_TAPE_TAPEGATEWAY_FILEMIGRATEDNOTIFICATION_HPP

// Include Files
#include "castor/tape/tapegateway/BaseFileInfo.hpp"
#include "osdep.h"
#include <string>

namespace castor {

  namespace tape {

    namespace tapegateway {

      /**
       * class FileMigratedNotification
       */
      class FileMigratedNotification : public BaseFileInfo {

      public:

        /**
         * Empty Constructor
         */
        FileMigratedNotification() throw();

        /**
         * Empty Destructor
         */
        virtual ~FileMigratedNotification() throw();

        /**
         * Get the value of m_fileSize
         * @return the value of m_fileSize
         */
        u_signed64 fileSize() const {
          return m_fileSize;
        }

        /**
         * Set the value of m_fileSize
         * @param new_var the new value of m_fileSize
         */
        void setFileSize(u_signed64 new_var) {
          m_fileSize = new_var;
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

        /**
         * Get the value of m_compressedFileSize
         * @return the value of m_compressedFileSize
         */
        u_signed64 compressedFileSize() const {
          return m_compressedFileSize;
        }

        /**
         * Set the value of m_compressedFileSize
         * @param new_var the new value of m_compressedFileSize
         */
        void setCompressedFileSize(u_signed64 new_var) {
          m_compressedFileSize = new_var;
        }

        /**
         * Get the value of m_blockId0
         * @return the value of m_blockId0
         */
        unsigned char blockId0() const {
          return m_blockId0;
        }

        /**
         * Set the value of m_blockId0
         * @param new_var the new value of m_blockId0
         */
        void setBlockId0(unsigned char new_var) {
          m_blockId0 = new_var;
        }

        /**
         * Get the value of m_blockId1
         * @return the value of m_blockId1
         */
        unsigned char blockId1() const {
          return m_blockId1;
        }

        /**
         * Set the value of m_blockId1
         * @param new_var the new value of m_blockId1
         */
        void setBlockId1(unsigned char new_var) {
          m_blockId1 = new_var;
        }

        /**
         * Get the value of m_blockId2
         * @return the value of m_blockId2
         */
        unsigned char blockId2() const {
          return m_blockId2;
        }

        /**
         * Set the value of m_blockId2
         * @param new_var the new value of m_blockId2
         */
        void setBlockId2(unsigned char new_var) {
          m_blockId2 = new_var;
        }

        /**
         * Get the value of m_blockId3
         * @return the value of m_blockId3
         */
        unsigned char blockId3() const {
          return m_blockId3;
        }

        /**
         * Set the value of m_blockId3
         * @param new_var the new value of m_blockId3
         */
        void setBlockId3(unsigned char new_var) {
          m_blockId3 = new_var;
        }

      private:

        u_signed64 m_fileSize;

        std::string m_checksumName;

        u_signed64 m_checksum;

        u_signed64 m_compressedFileSize;

        unsigned char m_blockId0;

        unsigned char m_blockId1;

        unsigned char m_blockId2;

        unsigned char m_blockId3;

      }; /* end of class FileMigratedNotification */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_FILEMIGRATEDNOTIFICATION_HPP

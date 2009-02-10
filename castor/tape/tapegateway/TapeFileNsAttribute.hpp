/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/TapeFileNsAttribute.hpp
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
 * @(#)$RCSfile: TapeFileNsAttribute.hpp,v $ $Revision: 1.9 $ $Release$ $Date: 2009/02/10 16:04:22 $ $Author: gtaur $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_TAPE_TAPEGATEWAY_TAPEFILENSATTRIBUTE_HPP
#define CASTOR_TAPE_TAPEGATEWAY_TAPEFILENSATTRIBUTE_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace tape {

    namespace tapegateway {

      /**
       * class TapeFileNsAttribute
       */
      class TapeFileNsAttribute : public virtual castor::IObject {

      public:

        /**
         * Empty Constructor
         */
        TapeFileNsAttribute() throw();

        /**
         * Empty Destructor
         */
        virtual ~TapeFileNsAttribute() throw();

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
         * Get the value of m_copyNo
         * @return the value of m_copyNo
         */
        int copyNo() const {
          return m_copyNo;
        }

        /**
         * Set the value of m_copyNo
         * @param new_var the new value of m_copyNo
         */
        void setCopyNo(int new_var) {
          m_copyNo = new_var;
        }

        /**
         * Get the value of m_fsec
         * @return the value of m_fsec
         */
        int fsec() const {
          return m_fsec;
        }

        /**
         * Set the value of m_fsec
         * @param new_var the new value of m_fsec
         */
        void setFsec(int new_var) {
          m_fsec = new_var;
        }

        /**
         * Get the value of m_blockId
         * @return the value of m_blockId
         */
        u_signed64 blockId() const {
          return m_blockId;
        }

        /**
         * Set the value of m_blockId
         * @param new_var the new value of m_blockId
         */
        void setBlockId(u_signed64 new_var) {
          m_blockId = new_var;
        }

        /**
         * Get the value of m_compression
         * @return the value of m_compression
         */
        int compression() const {
          return m_compression;
        }

        /**
         * Set the value of m_compression
         * @param new_var the new value of m_compression
         */
        void setCompression(int new_var) {
          m_compression = new_var;
        }

        /**
         * Get the value of m_side
         * @return the value of m_side
         */
        int side() const {
          return m_side;
        }

        /**
         * Set the value of m_side
         * @param new_var the new value of m_side
         */
        void setSide(int new_var) {
          m_side = new_var;
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
         * Get the value of m_vid
         * @return the value of m_vid
         */
        std::string vid() const {
          return m_vid;
        }

        /**
         * Set the value of m_vid
         * @param new_var the new value of m_vid
         */
        void setVid(std::string new_var) {
          m_vid = new_var;
        }

        /**
         * Get the value of m_fseq
         * @return the value of m_fseq
         */
        int fseq() const {
          return m_fseq;
        }

        /**
         * Set the value of m_fseq
         * @param new_var the new value of m_fseq
         */
        void setFseq(int new_var) {
          m_fseq = new_var;
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

        int m_copyNo;

        int m_fsec;

        u_signed64 m_blockId;

        int m_compression;

        int m_side;

        std::string m_checksumName;

        u_signed64 m_checksum;

        std::string m_vid;

        int m_fseq;

        /// The id of this object
        u_signed64 m_id;

      }; /* end of class TapeFileNsAttribute */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_TAPEFILENSATTRIBUTE_HPP

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/EndNotificationFileErrorReport.hpp
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

#ifndef CASTOR_TAPE_TAPEGATEWAY_ENDNOTIFICATIONFILEERRORREPORT_HPP
#define CASTOR_TAPE_TAPEGATEWAY_ENDNOTIFICATIONFILEERRORREPORT_HPP

// Include Files
#include "castor/tape/tapegateway/EndNotificationErrorReport.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace tape {

    namespace tapegateway {

      /**
       * class EndNotificationFileErrorReport
       */
      class EndNotificationFileErrorReport : public EndNotificationErrorReport {

      public:

        /**
         * Empty Constructor
         */
        EndNotificationFileErrorReport() throw();

        /**
         * Empty Destructor
         */
        virtual ~EndNotificationFileErrorReport() throw();

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
         * Get the value of m_fileTransactionId
         * @return the value of m_fileTransactionId
         */
        u_signed64 fileTransactionId() const {
          return m_fileTransactionId;
        }

        /**
         * Set the value of m_fileTransactionId
         * @param new_var the new value of m_fileTransactionId
         */
        void setFileTransactionId(u_signed64 new_var) {
          m_fileTransactionId = new_var;
        }

        /**
         * Get the value of m_nsHost
         * @return the value of m_nsHost
         */
        std::string nsHost() const {
          return m_nsHost;
        }

        /**
         * Set the value of m_nsHost
         * @param new_var the new value of m_nsHost
         */
        void setNsHost(std::string new_var) {
          m_nsHost = new_var;
        }

        /**
         * Get the value of m_fileId
         * @return the value of m_fileId
         */
        u_signed64 fileId() const {
          return m_fileId;
        }

        /**
         * Set the value of m_fileId
         * @param new_var the new value of m_fileId
         */
        void setFileId(u_signed64 new_var) {
          m_fileId = new_var;
        }

        /**
         * Get the value of m_fSeq
         * @return the value of m_fSeq
         */
        int fSeq() const {
          return m_fSeq;
        }

        /**
         * Set the value of m_fSeq
         * @param new_var the new value of m_fSeq
         */
        void setFSeq(int new_var) {
          m_fSeq = new_var;
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
         * Get the value of m_cprc
         * @return the value of m_cprc
         */
        int cprc() const {
          return m_cprc;
        }

        /**
         * Set the value of m_cprc
         * @param new_var the new value of m_cprc
         */
        void setCprc(int new_var) {
          m_cprc = new_var;
        }

      private:

        u_signed64 m_fileTransactionId;

        std::string m_nsHost;

        u_signed64 m_fileId;

        int m_fSeq;

        unsigned char m_blockId0;

        unsigned char m_blockId1;

        unsigned char m_blockId2;

        unsigned char m_blockId3;

        std::string m_path;

        int m_cprc;

      }; /* end of class EndNotificationFileErrorReport */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_ENDNOTIFICATIONFILEERRORREPORT_HPP

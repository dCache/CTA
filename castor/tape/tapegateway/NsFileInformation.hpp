/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/NsFileInformation.hpp
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
 * @(#)$RCSfile: NsFileInformation.hpp,v $ $Revision: 1.2 $ $Release$ $Date: 2009/01/27 09:51:44 $ $Author: gtaur $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_TAPE_TAPEGATEWAY_NSFILEINFORMATION_HPP
#define CASTOR_TAPE_TAPEGATEWAY_NSFILEINFORMATION_HPP

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

      // Forward declarations
      class TapeFileNsAttribute;

      /**
       * class NsFileInformation
       */
      class NsFileInformation : public virtual castor::IObject {

      public:

        /**
         * Empty Constructor
         */
        NsFileInformation() throw();

        /**
         * Empty Destructor
         */
        virtual ~NsFileInformation() throw();

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
         * Get the value of m_nshost
         * @return the value of m_nshost
         */
        std::string nshost() const {
          return m_nshost;
        }

        /**
         * Set the value of m_nshost
         * @param new_var the new value of m_nshost
         */
        void setNshost(std::string new_var) {
          m_nshost = new_var;
        }

        /**
         * Get the value of m_fileid
         * @return the value of m_fileid
         */
        u_signed64 fileid() const {
          return m_fileid;
        }

        /**
         * Set the value of m_fileid
         * @param new_var the new value of m_fileid
         */
        void setFileid(u_signed64 new_var) {
          m_fileid = new_var;
        }

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
         * Get the value of m_lastModificationTime
         * @return the value of m_lastModificationTime
         */
        u_signed64 lastModificationTime() const {
          return m_lastModificationTime;
        }

        /**
         * Set the value of m_lastModificationTime
         * @param new_var the new value of m_lastModificationTime
         */
        void setLastModificationTime(u_signed64 new_var) {
          m_lastModificationTime = new_var;
        }

        /**
         * Get the value of m_lastKnownFileName
         * @return the value of m_lastKnownFileName
         */
        std::string lastKnownFileName() const {
          return m_lastKnownFileName;
        }

        /**
         * Set the value of m_lastKnownFileName
         * @param new_var the new value of m_lastKnownFileName
         */
        void setLastKnownFileName(std::string new_var) {
          m_lastKnownFileName = new_var;
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
         * Get the value of m_tapeFileNsAttribute
         * @return the value of m_tapeFileNsAttribute
         */
        TapeFileNsAttribute* tapeFileNsAttribute() const {
          return m_tapeFileNsAttribute;
        }

        /**
         * Set the value of m_tapeFileNsAttribute
         * @param new_var the new value of m_tapeFileNsAttribute
         */
        void setTapeFileNsAttribute(TapeFileNsAttribute* new_var) {
          m_tapeFileNsAttribute = new_var;
        }

      private:

        std::string m_nshost;

        u_signed64 m_fileid;

        u_signed64 m_fileSize;

        u_signed64 m_lastModificationTime;

        std::string m_lastKnownFileName;

        /// The id of this object
        u_signed64 m_id;

        TapeFileNsAttribute* m_tapeFileNsAttribute;

      }; /* end of class NsFileInformation */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_NSFILEINFORMATION_HPP

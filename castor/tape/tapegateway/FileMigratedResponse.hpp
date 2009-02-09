/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/FileMigratedResponse.hpp
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
 * @(#)$RCSfile: FileMigratedResponse.hpp,v $ $Revision: 1.8 $ $Release$ $Date: 2009/02/09 13:31:16 $ $Author: gtaur $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_TAPE_TAPEGATEWAY_FILEMIGRATEDRESPONSE_HPP
#define CASTOR_TAPE_TAPEGATEWAY_FILEMIGRATEDRESPONSE_HPP

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
      class NsFileInformation;

      /**
       * class FileMigratedResponse
       */
      class FileMigratedResponse : public virtual castor::IObject {

      public:

        /**
         * Empty Constructor
         */
        FileMigratedResponse() throw();

        /**
         * Empty Destructor
         */
        virtual ~FileMigratedResponse() throw();

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
         * Get the value of m_errorCode
         * @return the value of m_errorCode
         */
        int errorCode() const {
          return m_errorCode;
        }

        /**
         * Set the value of m_errorCode
         * @param new_var the new value of m_errorCode
         */
        void setErrorCode(int new_var) {
          m_errorCode = new_var;
        }

        /**
         * Get the value of m_errorMessage
         * @return the value of m_errorMessage
         */
        std::string errorMessage() const {
          return m_errorMessage;
        }

        /**
         * Set the value of m_errorMessage
         * @param new_var the new value of m_errorMessage
         */
        void setErrorMessage(std::string new_var) {
          m_errorMessage = new_var;
        }

        /**
         * Get the value of m_transactionId
         * @return the value of m_transactionId
         */
        u_signed64 transactionId() const {
          return m_transactionId;
        }

        /**
         * Set the value of m_transactionId
         * @param new_var the new value of m_transactionId
         */
        void setTransactionId(u_signed64 new_var) {
          m_transactionId = new_var;
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
         * Get the value of m_nsFileInformation
         * @return the value of m_nsFileInformation
         */
        NsFileInformation* nsFileInformation() const {
          return m_nsFileInformation;
        }

        /**
         * Set the value of m_nsFileInformation
         * @param new_var the new value of m_nsFileInformation
         */
        void setNsFileInformation(NsFileInformation* new_var) {
          m_nsFileInformation = new_var;
        }

      private:

        int m_errorCode;

        std::string m_errorMessage;

        u_signed64 m_transactionId;

        /// The id of this object
        u_signed64 m_id;

        NsFileInformation* m_nsFileInformation;

      }; /* end of class FileMigratedResponse */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_FILEMIGRATEDRESPONSE_HPP

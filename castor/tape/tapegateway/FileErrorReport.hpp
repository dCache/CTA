/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/FileErrorReport.hpp
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

#ifndef CASTOR_TAPE_TAPEGATEWAY_FILEERRORREPORT_HPP
#define CASTOR_TAPE_TAPEGATEWAY_FILEERRORREPORT_HPP

// Include Files
#include "castor/tape/tapegateway/BaseFileInfo.hpp"
#include <string>

namespace castor {

  namespace tape {

    namespace tapegateway {

      /**
       * class FileErrorReport
       */
      class FileErrorReport : public BaseFileInfo {

      public:

        /**
         * Empty Constructor
         */
        FileErrorReport() throw();

        /**
         * Empty Destructor
         */
        virtual ~FileErrorReport() throw();

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

      private:

        int m_errorCode;

        std::string m_errorMessage;

      }; /* end of class FileErrorReport */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_FILEERRORREPORT_HPP

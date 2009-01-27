/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/TapeRequestState.hpp
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

#ifndef CASTOR_TAPE_TAPEGATEWAY_TAPEREQUESTSTATE_HPP
#define CASTOR_TAPE_TAPEGATEWAY_TAPEREQUESTSTATE_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/tape/tapegateway/TapeRequestStateCode.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  // Forward declarations
  namespace stager {

    // Forward declarations
    class Tape;
    class Stream;

  } /* end of namespace stager */

  namespace tape {

    namespace tapegateway {

      /**
       * class TapeRequestState
       */
      class TapeRequestState : public virtual castor::IObject {

      public:

        /**
         * Empty Constructor
         */
        TapeRequestState() throw();

        /**
         * Empty Destructor
         */
        virtual ~TapeRequestState() throw();

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
         * Get the value of m_accessMode
         * @return the value of m_accessMode
         */
        int accessMode() const {
          return m_accessMode;
        }

        /**
         * Set the value of m_accessMode
         * @param new_var the new value of m_accessMode
         */
        void setAccessMode(int new_var) {
          m_accessMode = new_var;
        }

        /**
         * Get the value of m_startTime
         * @return the value of m_startTime
         */
        u_signed64 startTime() const {
          return m_startTime;
        }

        /**
         * Set the value of m_startTime
         * @param new_var the new value of m_startTime
         */
        void setStartTime(u_signed64 new_var) {
          m_startTime = new_var;
        }

        /**
         * Get the value of m_lastVdqmPingTime
         * @return the value of m_lastVdqmPingTime
         */
        u_signed64 lastVdqmPingTime() const {
          return m_lastVdqmPingTime;
        }

        /**
         * Set the value of m_lastVdqmPingTime
         * @param new_var the new value of m_lastVdqmPingTime
         */
        void setLastVdqmPingTime(u_signed64 new_var) {
          m_lastVdqmPingTime = new_var;
        }

        /**
         * Get the value of m_vdqmVolReqId
         * @return the value of m_vdqmVolReqId
         */
        int vdqmVolReqId() const {
          return m_vdqmVolReqId;
        }

        /**
         * Set the value of m_vdqmVolReqId
         * @param new_var the new value of m_vdqmVolReqId
         */
        void setVdqmVolReqId(int new_var) {
          m_vdqmVolReqId = new_var;
        }

        /**
         * Get the value of m_nbRetry
         * @return the value of m_nbRetry
         */
        int nbRetry() const {
          return m_nbRetry;
        }

        /**
         * Set the value of m_nbRetry
         * @param new_var the new value of m_nbRetry
         */
        void setNbRetry(int new_var) {
          m_nbRetry = new_var;
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
         * Get the value of m_streamMigration
         * @return the value of m_streamMigration
         */
        castor::stager::Stream* streamMigration() const {
          return m_streamMigration;
        }

        /**
         * Set the value of m_streamMigration
         * @param new_var the new value of m_streamMigration
         */
        void setStreamMigration(castor::stager::Stream* new_var) {
          m_streamMigration = new_var;
        }

        /**
         * Get the value of m_tapeRecall
         * @return the value of m_tapeRecall
         */
        castor::stager::Tape* tapeRecall() const {
          return m_tapeRecall;
        }

        /**
         * Set the value of m_tapeRecall
         * @param new_var the new value of m_tapeRecall
         */
        void setTapeRecall(castor::stager::Tape* new_var) {
          m_tapeRecall = new_var;
        }

        /**
         * Get the value of m_status
         * @return the value of m_status
         */
        TapeRequestStateCode status() const {
          return m_status;
        }

        /**
         * Set the value of m_status
         * @param new_var the new value of m_status
         */
        void setStatus(TapeRequestStateCode new_var) {
          m_status = new_var;
        }

      private:

        int m_accessMode;

        u_signed64 m_startTime;

        u_signed64 m_lastVdqmPingTime;

        int m_vdqmVolReqId;

        int m_nbRetry;

        /// The id of this object
        u_signed64 m_id;

        castor::stager::Stream* m_streamMigration;

        castor::stager::Tape* m_tapeRecall;

        TapeRequestStateCode m_status;

      }; /* end of class TapeRequestState */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_TAPEREQUESTSTATE_HPP

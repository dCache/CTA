/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/VdqmTapeGatewayRequest.hpp
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

#ifndef CASTOR_TAPE_TAPEGATEWAY_VDQMTAPEGATEWAYREQUEST_HPP
#define CASTOR_TAPE_TAPEGATEWAY_VDQMTAPEGATEWAYREQUEST_HPP

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
       * class VdqmTapeGatewayRequest
       */
      class VdqmTapeGatewayRequest : public virtual castor::IObject {

      public:

        /**
         * Empty Constructor
         */
        VdqmTapeGatewayRequest() throw();

        /**
         * Empty Destructor
         */
        virtual ~VdqmTapeGatewayRequest() throw();

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
         * Get the value of m_taperequest
         * @return the value of m_taperequest
         */
        u_signed64 taperequest() const {
          return m_taperequest;
        }

        /**
         * Set the value of m_taperequest
         * @param new_var the new value of m_taperequest
         */
        void setTaperequest(u_signed64 new_var) {
          m_taperequest = new_var;
        }

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
         * Get the value of m_mountTransactionId
         * @return the value of m_mountTransactionId
         */
        u_signed64 mountTransactionId() const {
          return m_mountTransactionId;
        }

        /**
         * Set the value of m_mountTransactionId
         * @param new_var the new value of m_mountTransactionId
         */
        void setMountTransactionId(u_signed64 new_var) {
          m_mountTransactionId = new_var;
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

        u_signed64 m_taperequest;

        int m_accessMode;

        std::string m_vid;

        u_signed64 m_mountTransactionId;

        /// The id of this object
        u_signed64 m_id;

      public:
        int32_t m_firstFseqToBeWrittenTo;

      }; /* end of class VdqmTapeGatewayRequest */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_VDQMTAPEGATEWAYREQUEST_HPP

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeRequest.hpp
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

#ifndef CASTOR_VDQM_TAPEREQUEST_HPP
#define CASTOR_VDQM_TAPEREQUEST_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/vdqm/TapeRequestStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace vdqm {

    // Forward declarations
    class VdqmTape;
    class TapeServer;
    class TapeDrive;
    class TapeAccessSpecification;
    class DeviceGroupName;
    class ClientIdentification;

    /**
     * class TapeRequest
     * A TapeRequest object is instanciated, when a client wants to read or write a
     * file frome a tape.
     */
    class TapeRequest : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      TapeRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~TapeRequest() throw();

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
       * Get the value of m_priority
       * @return the value of m_priority
       */
      int priority() const {
        return m_priority;
      }

      /**
       * Set the value of m_priority
       * @param new_var the new value of m_priority
       */
      void setPriority(int new_var) {
        m_priority = new_var;
      }

      /**
       * Get the value of m_modificationTime
       * The time, when the tape request has been opend or has been modified
       * @return the value of m_modificationTime
       */
      u_signed64 modificationTime() const {
        return m_modificationTime;
      }

      /**
       * Set the value of m_modificationTime
       * The time, when the tape request has been opend or has been modified
       * @param new_var the new value of m_modificationTime
       */
      void setModificationTime(u_signed64 new_var) {
        m_modificationTime = new_var;
      }

      /**
       * Get the value of m_creationTime
       * The time, when the tape request is beeing created
       * @return the value of m_creationTime
       */
      u_signed64 creationTime() const {
        return m_creationTime;
      }

      /**
       * Set the value of m_creationTime
       * The time, when the tape request is beeing created
       * @param new_var the new value of m_creationTime
       */
      void setCreationTime(u_signed64 new_var) {
        m_creationTime = new_var;
      }

      /**
       * Get the value of m_errorCode
       * [s]errno in case of failure
       * @return the value of m_errorCode
       */
      int errorCode() const {
        return m_errorCode;
      }

      /**
       * Set the value of m_errorCode
       * [s]errno in case of failure
       * @param new_var the new value of m_errorCode
       */
      void setErrorCode(int new_var) {
        m_errorCode = new_var;
      }

      /**
       * Get the value of m_errorMessage
       * Error message in case of failure
       * @return the value of m_errorMessage
       */
      std::string errorMessage() const {
        return m_errorMessage;
      }

      /**
       * Set the value of m_errorMessage
       * Error message in case of failure
       * @param new_var the new value of m_errorMessage
       */
      void setErrorMessage(std::string new_var) {
        m_errorMessage = new_var;
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
       * Get the value of m_tape
       * @return the value of m_tape
       */
      VdqmTape* tape() const {
        return m_tape;
      }

      /**
       * Set the value of m_tape
       * @param new_var the new value of m_tape
       */
      void setTape(VdqmTape* new_var) {
        m_tape = new_var;
      }

      /**
       * Get the value of m_tapeAccessSpecification
       * @return the value of m_tapeAccessSpecification
       */
      TapeAccessSpecification* tapeAccessSpecification() const {
        return m_tapeAccessSpecification;
      }

      /**
       * Set the value of m_tapeAccessSpecification
       * @param new_var the new value of m_tapeAccessSpecification
       */
      void setTapeAccessSpecification(TapeAccessSpecification* new_var) {
        m_tapeAccessSpecification = new_var;
      }

      /**
       * Get the value of m_requestedSrv
       * @return the value of m_requestedSrv
       */
      TapeServer* requestedSrv() const {
        return m_requestedSrv;
      }

      /**
       * Set the value of m_requestedSrv
       * @param new_var the new value of m_requestedSrv
       */
      void setRequestedSrv(TapeServer* new_var) {
        m_requestedSrv = new_var;
      }

      /**
       * Get the value of m_tapeDrive
       * @return the value of m_tapeDrive
       */
      TapeDrive* tapeDrive() const {
        return m_tapeDrive;
      }

      /**
       * Set the value of m_tapeDrive
       * @param new_var the new value of m_tapeDrive
       */
      void setTapeDrive(TapeDrive* new_var) {
        m_tapeDrive = new_var;
      }

      /**
       * Get the value of m_deviceGroupName
       * @return the value of m_deviceGroupName
       */
      DeviceGroupName* deviceGroupName() const {
        return m_deviceGroupName;
      }

      /**
       * Set the value of m_deviceGroupName
       * @param new_var the new value of m_deviceGroupName
       */
      void setDeviceGroupName(DeviceGroupName* new_var) {
        m_deviceGroupName = new_var;
      }

      /**
       * Get the value of m_status
       * @return the value of m_status
       */
      TapeRequestStatusCodes status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(TapeRequestStatusCodes new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_client
       * @return the value of m_client
       */
      ClientIdentification* client() const {
        return m_client;
      }

      /**
       * Set the value of m_client
       * @param new_var the new value of m_client
       */
      void setClient(ClientIdentification* new_var) {
        m_client = new_var;
      }

    private:

      int m_priority;

      /// The time, when the tape request has been opend or has been modified
      u_signed64 m_modificationTime;

      /// The time, when the tape request is beeing created
      u_signed64 m_creationTime;

      /// [s]errno in case of failure
      int m_errorCode;

      /// Error message in case of failure
      std::string m_errorMessage;

      /// The id of this object
      u_signed64 m_id;

      VdqmTape* m_tape;

      TapeAccessSpecification* m_tapeAccessSpecification;

      TapeServer* m_requestedSrv;

      TapeDrive* m_tapeDrive;

      DeviceGroupName* m_deviceGroupName;

      TapeRequestStatusCodes m_status;

      ClientIdentification* m_client;

    }; /* end of class TapeRequest */

  } /* end of namespace vdqm */

} /* end of namespace castor */

#endif // CASTOR_VDQM_TAPEREQUEST_HPP

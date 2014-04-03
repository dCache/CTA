/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeDriveDedication.hpp
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

#pragma once

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace vdqm {

    // Forward declarations
    class TapeDrive;

    /**
     * class TapeDriveDedication
     * This class is used to specify time slots, where a tape drive should be dedicated
     * for a specific tape.
     */
    class TapeDriveDedication : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      TapeDriveDedication() throw();

      /**
       * Empty Destructor
       */
      virtual ~TapeDriveDedication() throw();

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
       * Get the value of m_clientHost
       * the client host
       * @return the value of m_clientHost
       */
      std::string clientHost() const {
        return m_clientHost;
      }

      /**
       * Set the value of m_clientHost
       * the client host
       * @param new_var the new value of m_clientHost
       */
      void setClientHost(std::string new_var) {
        m_clientHost = new_var;
      }

      /**
       * Get the value of m_euid
       * client uid
       * @return the value of m_euid
       */
      int euid() const {
        return m_euid;
      }

      /**
       * Set the value of m_euid
       * client uid
       * @param new_var the new value of m_euid
       */
      void setEuid(int new_var) {
        m_euid = new_var;
      }

      /**
       * Get the value of m_egid
       * client gid
       * @return the value of m_egid
       */
      int egid() const {
        return m_egid;
      }

      /**
       * Set the value of m_egid
       * client gid
       * @param new_var the new value of m_egid
       */
      void setEgid(int new_var) {
        m_egid = new_var;
      }

      /**
       * Get the value of m_vid
       * the tape, which the client want to  access
       * @return the value of m_vid
       */
      std::string vid() const {
        return m_vid;
      }

      /**
       * Set the value of m_vid
       * the tape, which the client want to  access
       * @param new_var the new value of m_vid
       */
      void setVid(std::string new_var) {
        m_vid = new_var;
      }

      /**
       * Get the value of m_accessMode
       * the mode, which the client want to use to access the tape. This can be READ or
       * WRITE access
       * @return the value of m_accessMode
       */
      int accessMode() const {
        return m_accessMode;
      }

      /**
       * Set the value of m_accessMode
       * the mode, which the client want to use to access the tape. This can be READ or
       * WRITE access
       * @param new_var the new value of m_accessMode
       */
      void setAccessMode(int new_var) {
        m_accessMode = new_var;
      }

      /**
       * Get the value of m_startTime
       * The starting time, which specifies the beginning of a time slot for a special
       * tape on a specified tape drive from a specified user
       * @return the value of m_startTime
       */
      u_signed64 startTime() const {
        return m_startTime;
      }

      /**
       * Set the value of m_startTime
       * The starting time, which specifies the beginning of a time slot for a special
       * tape on a specified tape drive from a specified user
       * @param new_var the new value of m_startTime
       */
      void setStartTime(u_signed64 new_var) {
        m_startTime = new_var;
      }

      /**
       * Get the value of m_endTime
       * The ending time, which specifies the beginning of a time slot for a special tape
       * on a specified tape drive from a specified user
       * @return the value of m_endTime
       */
      u_signed64 endTime() const {
        return m_endTime;
      }

      /**
       * Set the value of m_endTime
       * The ending time, which specifies the beginning of a time slot for a special tape
       * on a specified tape drive from a specified user
       * @param new_var the new value of m_endTime
       */
      void setEndTime(u_signed64 new_var) {
        m_endTime = new_var;
      }

      /**
       * Get the value of m_reason
       * the reason for the tape drive dedication
       * @return the value of m_reason
       */
      std::string reason() const {
        return m_reason;
      }

      /**
       * Set the value of m_reason
       * the reason for the tape drive dedication
       * @param new_var the new value of m_reason
       */
      void setReason(std::string new_var) {
        m_reason = new_var;
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

    private:

      /// the client host
      std::string m_clientHost;

      /// client uid
      int m_euid;

      /// client gid
      int m_egid;

      /// the tape, which the client want to  access
      std::string m_vid;

      /// the mode, which the client want to use to access the tape. This can be READ or WRITE access
      int m_accessMode;

      /// The starting time, which specifies the beginning of a time slot for a special tape on a specified tape drive from a specified user
      u_signed64 m_startTime;

      /// The ending time, which specifies the beginning of a time slot for a special tape on a specified tape drive from a specified user
      u_signed64 m_endTime;

      /// the reason for the tape drive dedication
      std::string m_reason;

      /// The id of this object
      u_signed64 m_id;

      TapeDrive* m_tapeDrive;

    }; /* end of class TapeDriveDedication */

  } /* end of namespace vdqm */

} /* end of namespace castor */


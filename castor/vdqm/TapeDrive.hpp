/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeDrive.hpp
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

#ifndef CASTOR_VDQM_TAPEDRIVE_HPP
#define CASTOR_VDQM_TAPEDRIVE_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/vdqm/TapeDriveStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  // Forward declarations
  namespace stager {

    // Forward declarations
    class Tape;

  }; // end of namespace stager

  namespace vdqm {

    // Forward declarations
    class TapeServer;
    class TapeRequest;
    class TapeDriveDedication;
    class TapeDriveCompatibility;
    class ErrorHistory;
    class DeviceGroupName;

    /**
     * class TapeDrive
     * An instance of this class contains the informations of one specific tape drive of
     */
    class TapeDrive : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      TapeDrive() throw();

      /**
       * Empty Destructor
       */
      virtual ~TapeDrive() throw();

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
       * Get the value of m_jobID
       * The jobID is given by the tpdaemon to the tape drive, when a job has been
       * @return the value of m_jobID
       */
      int jobID() const {
        return m_jobID;
      }

      /**
       * Set the value of m_jobID
       * The jobID is given by the tpdaemon to the tape drive, when a job has been
       * @param new_var the new value of m_jobID
       */
      void setJobID(int new_var) {
        m_jobID = new_var;
      }

      /**
       * Get the value of m_modificationTime
       * The time, when the tape drive begins with its job or modified it
       * @return the value of m_modificationTime
       */
      u_signed64 modificationTime() const {
        return m_modificationTime;
      }

      /**
       * Set the value of m_modificationTime
       * The time, when the tape drive begins with its job or modified it
       * @param new_var the new value of m_modificationTime
       */
      void setModificationTime(u_signed64 new_var) {
        m_modificationTime = new_var;
      }

      /**
       * Get the value of m_resettime
       * Last time counters were reset
       * @return the value of m_resettime
       */
      u_signed64 resettime() const {
        return m_resettime;
      }

      /**
       * Set the value of m_resettime
       * Last time counters were reset
       * @param new_var the new value of m_resettime
       */
      void setResettime(u_signed64 new_var) {
        m_resettime = new_var;
      }

      /**
       * Get the value of m_usecount
       * Usage counter (total number of VolReqs so far)
       * @return the value of m_usecount
       */
      int usecount() const {
        return m_usecount;
      }

      /**
       * Set the value of m_usecount
       * Usage counter (total number of VolReqs so far)
       * @param new_var the new value of m_usecount
       */
      void setUsecount(int new_var) {
        m_usecount = new_var;
      }

      /**
       * Get the value of m_errcount
       * Drive error counter
       * @return the value of m_errcount
       */
      int errcount() const {
        return m_errcount;
      }

      /**
       * Set the value of m_errcount
       * Drive error counter
       * @param new_var the new value of m_errcount
       */
      void setErrcount(int new_var) {
        m_errcount = new_var;
      }

      /**
       * Get the value of m_transferredMB
       * MBytes transfered in last request.
       * @return the value of m_transferredMB
       */
      int transferredMB() const {
        return m_transferredMB;
      }

      /**
       * Set the value of m_transferredMB
       * MBytes transfered in last request.
       * @param new_var the new value of m_transferredMB
       */
      void setTransferredMB(int new_var) {
        m_transferredMB = new_var;
      }

      /**
       * Get the value of m_totalMB
       * Total MBytes transfered
       * @return the value of m_totalMB
       */
      u_signed64 totalMB() const {
        return m_totalMB;
      }

      /**
       * Set the value of m_totalMB
       * Total MBytes transfered
       * @param new_var the new value of m_totalMB
       */
      void setTotalMB(u_signed64 new_var) {
        m_totalMB = new_var;
      }

      /**
       * Get the value of m_driveName
       * The name of the drive
       * @return the value of m_driveName
       */
      std::string driveName() const {
        return m_driveName;
      }

      /**
       * Set the value of m_driveName
       * The name of the drive
       * @param new_var the new value of m_driveName
       */
      void setDriveName(std::string new_var) {
        m_driveName = new_var;
      }

      /**
       * Get the value of m_tapeAccessMode
       * The tape access mode is the information, how the tape drive is accessing the
       * @return the value of m_tapeAccessMode
       */
      int tapeAccessMode() const {
        return m_tapeAccessMode;
      }

      /**
       * Set the value of m_tapeAccessMode
       * The tape access mode is the information, how the tape drive is accessing the
       * @param new_var the new value of m_tapeAccessMode
       */
      void setTapeAccessMode(int new_var) {
        m_tapeAccessMode = new_var;
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
      castor::stager::Tape* tape() const {
        return m_tape;
      }

      /**
       * Set the value of m_tape
       * @param new_var the new value of m_tape
       */
      void setTape(castor::stager::Tape* new_var) {
        m_tape = new_var;
      }

      /**
       * Get the value of m_runningTapeReq
       * @return the value of m_runningTapeReq
       */
      TapeRequest* runningTapeReq() const {
        return m_runningTapeReq;
      }

      /**
       * Set the value of m_runningTapeReq
       * @param new_var the new value of m_runningTapeReq
       */
      void setRunningTapeReq(TapeRequest* new_var) {
        m_runningTapeReq = new_var;
      }

      /**
       * Add a ErrorHistory* object to the m_errorHistoryVector list
       */
      void addErrorHistory(ErrorHistory* add_object) {
        m_errorHistoryVector.push_back(add_object);
      }

      /**
       * Remove a ErrorHistory* object from m_errorHistoryVector
       */
      void removeErrorHistory(ErrorHistory* remove_object) {
        for (unsigned int i = 0; i < m_errorHistoryVector.size(); i++) {
          ErrorHistory* item = m_errorHistoryVector[i];
          if (item == remove_object) {
            std::vector<ErrorHistory*>::iterator it = m_errorHistoryVector.begin() + i;
            m_errorHistoryVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of ErrorHistory* objects held by m_errorHistoryVector
       * @return list of ErrorHistory* objects held by m_errorHistoryVector
       */
      std::vector<ErrorHistory*>& errorHistory() {
        return m_errorHistoryVector;
      }

      /**
       * Add a TapeDriveDedication* object to the m_tapeDriveDedicationVector list
       */
      void addTapeDriveDedication(TapeDriveDedication* add_object) {
        m_tapeDriveDedicationVector.push_back(add_object);
      }

      /**
       * Remove a TapeDriveDedication* object from m_tapeDriveDedicationVector
       */
      void removeTapeDriveDedication(TapeDriveDedication* remove_object) {
        for (unsigned int i = 0; i < m_tapeDriveDedicationVector.size(); i++) {
          TapeDriveDedication* item = m_tapeDriveDedicationVector[i];
          if (item == remove_object) {
            std::vector<TapeDriveDedication*>::iterator it = m_tapeDriveDedicationVector.begin() + i;
            m_tapeDriveDedicationVector.erase(it);
            return;
          }
        }
      }

      /**
       */
      std::vector<TapeDriveDedication*>& tapeDriveDedication() {
        return m_tapeDriveDedicationVector;
      }

      /**
       * Add a TapeDriveCompatibility* object to the m_tapeDriveCompatibilitiesVector
       */
      void addTapeDriveCompatibilities(TapeDriveCompatibility* add_object) {
        m_tapeDriveCompatibilitiesVector.push_back(add_object);
      }

      /**
       * Remove a TapeDriveCompatibility* object from m_tapeDriveCompatibilitiesVector
       */
      void removeTapeDriveCompatibilities(TapeDriveCompatibility* remove_object) {
        for (unsigned int i = 0; i < m_tapeDriveCompatibilitiesVector.size(); i++) {
          TapeDriveCompatibility* item = m_tapeDriveCompatibilitiesVector[i];
          if (item == remove_object) {
            std::vector<TapeDriveCompatibility*>::iterator it = m_tapeDriveCompatibilitiesVector.begin() + i;
            m_tapeDriveCompatibilitiesVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of TapeDriveCompatibility* objects held by
       * @return list of TapeDriveCompatibility* objects held by
       */
      std::vector<TapeDriveCompatibility*>& tapeDriveCompatibilities() {
        return m_tapeDriveCompatibilitiesVector;
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
      TapeDriveStatusCodes status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(TapeDriveStatusCodes new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_tapeServer
       * @return the value of m_tapeServer
       */
      TapeServer* tapeServer() const {
        return m_tapeServer;
      }

      /**
       * Set the value of m_tapeServer
       * @param new_var the new value of m_tapeServer
       */
      void setTapeServer(TapeServer* new_var) {
        m_tapeServer = new_var;
      }

    private:

      /// The jobID is given by the tpdaemon to the tape drive, when a job has been assigned.
      int m_jobID;

      /// The time, when the tape drive begins with its job or modified it
      u_signed64 m_modificationTime;

      /// Last time counters were reset
      u_signed64 m_resettime;

      /// Usage counter (total number of VolReqs so far)
      int m_usecount;

      /// Drive error counter
      int m_errcount;

      /// MBytes transfered in last request.
      int m_transferredMB;

      /// Total MBytes transfered
      u_signed64 m_totalMB;

      /// The name of the drive
      std::string m_driveName;

      /// The tape access mode is the information, how the tape drive is accessing the mounted tape. The value could  be WRITE_ENABLE, WRITE_DISABLE or -1 for UNKNOWN.
      int m_tapeAccessMode;

      /// The id of this object
      u_signed64 m_id;

      castor::stager::Tape* m_tape;

      TapeRequest* m_runningTapeReq;

      std::vector<ErrorHistory*> m_errorHistoryVector;

      std::vector<TapeDriveDedication*> m_tapeDriveDedicationVector;

      std::vector<TapeDriveCompatibility*> m_tapeDriveCompatibilitiesVector;

      DeviceGroupName* m_deviceGroupName;

      TapeDriveStatusCodes m_status;

      TapeServer* m_tapeServer;

    }; // end of class TapeDrive

  }; // end of namespace vdqm

}; // end of namespace castor

#endif // CASTOR_VDQM_TAPEDRIVE_HPP

/******************************************************************************
 *                      TapeDriveStatusHandler.cpp
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
 * @(#)RCSfile: TapeDriveStatusHandler.cpp  Revision: 1.0  Release Date: Jul 6, 2005  Author: mbraeger 
 *
 *
 *
 * @author Matthias Braeger
 *****************************************************************************/

#include <rtcp_constants.h> /* RTCOPY_PORT  */
 
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/server/BaseServer.hpp"
#include "castor/vdqm/ClientIdentification.hpp"
#include "castor/vdqm/VdqmTape.hpp"
#include "castor/vdqm/DatabaseHelper.hpp"
#include "castor/vdqm/DeviceGroupName.hpp"
#include "castor/vdqm/newVdqm.h"
#include "castor/vdqm/RTCopyDConnection.hpp"
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeDriveStatusCodes.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "castor/vdqm/VdqmDlfMessageConstants.hpp"
#include "castor/vdqm/handler/TapeDriveStatusHandler.hpp"

#include <net.h>
#include <vdqm_constants.h>


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::handler::TapeDriveStatusHandler::TapeDriveStatusHandler(
  castor::vdqm::TapeDrive* tapeDrive, newVdqmDrvReq_t* driveRequest, 
  Cuuid_t cuuid, u_signed64* newRequestId) throw(castor::exception::Exception) {
    
  m_cuuid = cuuid;
  ptr_newRequestId = newRequestId;

  // ptr_newRequestId will get a valid request ID (value > 0) if there is
  // another request for the same tape
  *ptr_newRequestId = 0;
  
  if ( tapeDrive == NULL || driveRequest == NULL ) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "One of the arguments is NULL";
    throw ex;
  }
  else {
    ptr_tapeDrive = tapeDrive;
    ptr_driveRequest = driveRequest;
  }
}


//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::vdqm::handler::TapeDriveStatusHandler::~TapeDriveStatusHandler() 
  throw() {

}


//------------------------------------------------------------------------------
// handleOldStatus
//------------------------------------------------------------------------------
void castor::vdqm::handler::TapeDriveStatusHandler::handleOldStatus() 
  throw (castor::exception::Exception) {

      


  if ( ptr_tapeDrive->status() != UNIT_DOWN &&  
       ptr_tapeDrive->status() != STATUS_UNKNOWN) {

    if ( (ptr_driveRequest->status & VDQM_UNIT_MBCOUNT) ) {
        /*
         * Update TotalMB counter. Since this request is sent by
         * RTCOPY rather than the tape daemon we cannot yet reset
         * unknown status if it was previously set.
         */
        ptr_tapeDrive->setTransferredMB(ptr_driveRequest->MBtransf);
        ptr_tapeDrive->setTotalMB(ptr_tapeDrive->totalMB() + ptr_driveRequest->MBtransf);
    }
    
    if ( (ptr_driveRequest->status & VDQM_UNIT_ERROR) ) {
        /*
         * Update error counter.
         */
        ptr_tapeDrive->setErrcount(ptr_tapeDrive->errcount() + 1);
    }
    
    
    if ( (ptr_driveRequest->status & VDQM_VOL_MOUNT) ) {
      handleVolMountStatus();
    }
    if ( (ptr_driveRequest->status & VDQM_VOL_UNMOUNT) ) {
      handleVolUnmountStatus();
    }
    if ((ptr_driveRequest->status & VDQM_UNIT_RELEASE) &&
        !(ptr_driveRequest->status & VDQM_UNIT_FREE) ) { 
      handleUnitReleaseStatus();
    } 
    
    /*
     * If unit is free, reset dynamic data in drive record
     */
    if ( (ptr_driveRequest->status & VDQM_UNIT_FREE) ) {
      handleUnitFreeStatus();
    }
  } 
  else { // TapeDrive is DOWN
    /*
     * If drive is down, report error for any requested update.
     */
    if ( ptr_driveRequest->status & (VDQM_UNIT_FREE | VDQM_UNIT_ASSIGN |
        VDQM_UNIT_BUSY | VDQM_UNIT_RELEASE | VDQM_VOL_MOUNT |
        VDQM_VOL_UNMOUNT ) ) {
        
      castor::exception::Exception ex(EVQUNNOTUP);
      ex.getMessage() << "TapeDriveStatusHandler::handleOldStatus(): "
                      << "TapeDrive is DOWN"
                      << std::endl;
                      
      throw ex;                              
    }
  }
}


//------------------------------------------------------------------------------
// handleVolMountStatus
//------------------------------------------------------------------------------
void castor::vdqm::handler::TapeDriveStatusHandler::handleVolMountStatus() 
  throw (castor::exception::Exception) {
    
  TapeRequest* tapeRequest = ptr_tapeDrive->runningTapeReq();
  TapeServer* tapeServer = ptr_tapeDrive->tapeServer();
  castor::vdqm::VdqmTape* mountedTape = NULL;  
  /*
   * A mount volume request. The unit must first have been assigned.
   */
  if ( ptr_tapeDrive->status() != UNIT_ASSIGNED) {
    castor::exception::Exception ex(EVQNOTASS);
    ex.getMessage() << "TapeDriveStatusHandler::handleVolMountStatus(): "
                    << "Mount request of "
                    << ptr_driveRequest->volid << " for jobID "
                    << ptr_driveRequest->jobID
                    << " on non-ASSIGNED unit." << std::endl;
                    
    throw ex;                      
  }
  
  if ( *ptr_driveRequest->volid == '\0' ) {
    castor::exception::Exception ex(EVQBADVOLID);
    ex.getMessage() << "TapeDriveStatusHandler::handleVolMountStatus(): "
                    << "Mount request with empty VOLID for jobID "
                    << ptr_tapeDrive->jobID() << std::endl;
                    
    throw ex;    
  }
  
  /*
   * Make sure that requested volume and assign volume record are the same
   */
  if ( tapeRequest != NULL && 
       strcmp(tapeRequest->tape()->vid().c_str(), ptr_driveRequest->volid) ) {
    castor::exception::Exception ex(EVQBADVOLID);
    ex.getMessage() << "TapeDriveStatusHandler::handleVolMountStatus(): "
                    << "Inconsistent mount "
                    << ptr_driveRequest->volid << " (should be "
                    << tapeRequest->tape()->vid() << ") for jobID "
                    << ptr_driveRequest->jobID << std::endl;                    
    throw ex;                      
  }
  
  /*
   * If there are no assigned volume request it means that this
   * is a local request. Make sure that server and reqhost are
   * the same and that the volume is free.
   */
  if ( tapeRequest == NULL ) {
    if ( strcmp(tapeServer->serverName().c_str(), ptr_driveRequest->reqhost) != 0 ) {
      castor::exception::Exception ex(EPERM);
      ex.getMessage() << "TapeDriveStatusHandler::handleVolMountStatus(): "
                      << "Can only mount a volume without an assigned volume "
                      << "request, if it is a local request." << std::endl;
      throw ex;                    
    }
    
     if ( ptr_IVdqmService->existTapeDriveWithTapeInUse(ptr_driveRequest->volid) ||
      ptr_IVdqmService->existTapeDriveWithTapeMounted(ptr_driveRequest->volid) ) {
      castor::exception::Exception ex(EBUSY);
      ex.getMessage() << "TapeDriveStatusHandler::handleVolMountStatus(): "
                      << "TapeDrive is busy with another request" << std::endl;
      throw ex;        
    }
    
    ptr_tapeDrive->setTapeAccessMode(-1); /* Mode is unknown */
  } 
  else {
    /**
     * Not needed any more!
     */
    ptr_tapeDrive->setTapeAccessMode(
      tapeRequest->tapeAccessSpecification()->accessMode());
  }
  
  if (strcmp(tapeRequest->tape()->vid().c_str(), ptr_driveRequest->volid) == 0) {
    //The tape, which is now in the tape drive
    mountedTape = ptr_IVdqmService->selectTape(ptr_driveRequest->volid, 0, 
                          tapeRequest->tapeAccessSpecification()->accessMode());
    ptr_tapeDrive->setTape(mountedTape);
  }
  else {
    // Normally, this is not needed any more!
    castor::exception::Exception ex(EVQBADVOLID);
    ex.getMessage() << "TapeDriveStatusHandler::handleVolMountStatus(): "
                    << "No Tape with specified volid in db" << std::endl;
                    
    throw ex;    
  }
  
  // Now we can switch from UNIT_ASSIGNED to the next status 
  ptr_tapeDrive->setStatus(VOL_MOUNTED);
  
  /*
   * Update usage counter
   */
  ptr_tapeDrive->setUsecount(ptr_tapeDrive->usecount() + 1);
  
  
  if ( tapeRequest ) {
    // "TapeDriveStatusHandler::handleVolMountStatus(): Tape mounted in tapeDrive"
    castor::dlf::Param params[] =
      {castor::dlf::Param("tapeDrive ID", ptr_tapeDrive->id()),
       castor::dlf::Param("tape ID", mountedTape->id()),
       castor::dlf::Param("tapeRequest ID", tapeRequest->id())};
    castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM, VDQM_HANDLE_VOL_MOUNT_STATUS_MOUNTED, 3, params);
  }
  else {
    // "TapeDriveStatusHandler::handleVolMountStatus(): Tape mounted in tapeDrive"
    castor::dlf::Param params[] =
      {castor::dlf::Param("tapeDrive ID", ptr_tapeDrive->id()),
       castor::dlf::Param("tape ID", mountedTape->id()),
       castor::dlf::Param("tapeRequest ID", "Local Request")};
    castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM, VDQM_HANDLE_VOL_MOUNT_STATUS_MOUNTED, 3, params);
  }
}


//------------------------------------------------------------------------------
// handleVolUnmountStatus
//------------------------------------------------------------------------------
void castor::vdqm::handler::TapeDriveStatusHandler::handleVolUnmountStatus() 
  throw (castor::exception::Exception) {
  
  castor::vdqm::VdqmTape* tape = ptr_tapeDrive->tape();
  delete tape;
  tape = 0;
  
  ptr_tapeDrive->setTape(NULL);
  ptr_tapeDrive->setTapeAccessMode(-1); // UNKNOWN
  
  
  if ( ptr_tapeDrive->status() == WAIT_FOR_UNMOUNT ) {
    /*
     * Set status to FREE if there is no job assigned to the unit
     */
    if ( ptr_tapeDrive->runningTapeReq() == NULL )
      ptr_tapeDrive->setStatus(UNIT_UP);
    else if ( ptr_tapeDrive->jobID() == 0 )
      ptr_tapeDrive->setStatus(UNIT_STARTING);
    else
      ptr_tapeDrive->setStatus(UNIT_ASSIGNED);
      
  }
  else if ( ptr_tapeDrive->status() == FORCED_UNMOUNT ) {
    ptr_tapeDrive->setStatus(UNIT_UP);
  }
  else {
    /**
     * The status of the tape drive must be WAIT_FOR_UNMOUNT or
     * FORCED_UNMOUNT. If it is not the case, we throw an error!
     */
    ptr_tapeDrive->setStatus(STATUS_UNKNOWN);
    
     castor::exception::Exception ex(EVQBADSTAT);
    ex.getMessage() << "TapeDriveStatusHandler::handleVolUnmountStatus(): "
                    << "bad status from tpdaemon! Can't unmount tape in this status! "
                    << "TapeDrive is set to STATUS_UNKNOWN." 
                    << std::endl;      
    throw ex;    
  }
}


//------------------------------------------------------------------------------
// handleUnitReleaseStatus
//------------------------------------------------------------------------------
void castor::vdqm::handler::TapeDriveStatusHandler::handleUnitReleaseStatus() 
  throw (castor::exception::Exception) {
  
  TapeRequest*            tapeRequest = ptr_tapeDrive->runningTapeReq();
  castor::vdqm::VdqmTape* tape        = ptr_tapeDrive->tape();

  // Reset request
  if ( tapeRequest != NULL) {
    castor::vdqm::DatabaseHelper::deleteRepresentation(tapeRequest, m_cuuid);
    delete tapeRequest;
    tapeRequest = 0;
    ptr_tapeDrive->setRunningTapeReq(0);
  }
  
  // Reset job ID
  ptr_tapeDrive->setJobID(0);
  
  // If there is a tape in the drive
  if ( tape != NULL ) {

    // Consistency check: if input request contains a volid it
    // should correspond to the one in the drive record. This
    // is not fatal but should be logged.
    if ( *ptr_driveRequest->volid != '\0' && 
         strcmp(ptr_driveRequest->volid, tape->vid().c_str()) ) {
       
      // "Inconsistent release on tape drive" message 
      castor::dlf::Param params[] = {
        castor::dlf::Param("driveName", ptr_tapeDrive->driveName()),
        castor::dlf::Param("serverName",
          ptr_tapeDrive->tapeServer()->serverName()),
        castor::dlf::Param("volid from client", ptr_driveRequest->volid),
        castor::dlf::Param("volid from tape drive", tape->vid()),
        castor::dlf::Param("jobID", ptr_tapeDrive->jobID())};
      castor::dlf::dlf_writep(m_cuuid, DLF_LVL_ERROR,
        VDQM_INCONSISTENT_RELEASE_ON_DRIVE, 5, params);
    }
    
    // Fill in current volid in case we need to request an unmount.
    if ( *ptr_driveRequest->volid == '\0' ) {
      strcpy(ptr_driveRequest->volid, tape->vid().c_str());
    }
    
    // If a tape is mounted but the current job ended: check if there
    // are any other valid requests for the same volume. The volume
    // can only be re-used on this unit if the modes (R/W) are the 
    // same. If client indicated an error or forced unmount the drive
    // will remain with that state until a VOL_UNMOUNT is received
    // and both drive and volume cannot be reused until then.
    // If the drive status was UNKNOWN at entry we must force an
    // unmount in all cases. This normally happens when a RTCOPY
    // client has aborted and sent VDQM_DEL_VOLREQ to delete his
    // volume request before the RTCOPY server has released the
    // job. 

    if ( ptr_tapeDrive->status() == FORCED_UNMOUNT ||
         (ptr_driveRequest->status & VDQM_FORCE_UNMOUNT)) {
      // "client has requested a forced unmount." message
      castor::dlf::dlf_writep(m_cuuid, DLF_LVL_WARNING,
        VDQM_CLIENT_REQUESTED_FORCED_UNMOUNT);
    } else if ( ptr_tapeDrive->status() == STATUS_UNKNOWN ) {
      // "tape drive in STATUS_UNKNOWN status. Force unmount!" message
      castor::dlf::dlf_writep(m_cuuid, DLF_LVL_WARNING,
        VDQM_DRIVE_STATUS_UNKNOWN_FORCE_UNMOUNT);
    } else {
      // Try to reuse the tape allocation
      *ptr_newRequestId = ptr_IVdqmService->reuseTapeAllocation(tape, ptr_tapeDrive);

      // If the tape allocation was reused
      if ( *ptr_newRequestId > 0) {
        castor::dlf::Param params[] = {
          castor::dlf::Param("driveName", ptr_tapeDrive->driveName()),
          castor::dlf::Param("serverName",
            ptr_tapeDrive->tapeServer()->serverName()),
          castor::dlf::Param("tape vid", tape->vid()),
          castor::dlf::Param("tapeRequest ID", *ptr_newRequestId)}; 
        castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
          VDQM_FOUND_QUEUED_TAPE_REQUEST_FOR_MOUNTED_TAPE, 4, params);

        // Notify the RTCP job submitter threads
        castor::server::BaseServer::sendNotification("localhost", VDQM_PORT,
          'J');

      // Else the tape allocation could not be reused
      } else {
        castor::dlf::Param params[] = {
          castor::dlf::Param("driveName", ptr_tapeDrive->driveName()),
          castor::dlf::Param("serverName",
            ptr_tapeDrive->tapeServer()->serverName()),
          castor::dlf::Param("tape vid", tape->vid())};
        castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
          VDQM_NO_TAPE_REQUEST_FOR_MOUNTED_TAPE, 3, params);
      }
    }
              
    if ( ptr_tapeDrive->status() == STATUS_UNKNOWN ||
         ptr_tapeDrive->status() == FORCED_UNMOUNT ||
         (ptr_driveRequest->status & VDQM_FORCE_UNMOUNT) ||
         *ptr_newRequestId == 0) {

      // No, there wasn't any other job for that volume. Tell the
      // drive to unmount the volume. Put unit in WAIT_FOR_UNMOUNT status
      // until volume has been unmounted to prevent other
      // request from being assigned.
      ptr_driveRequest->status = VDQM_VOL_UNMOUNT;
      ptr_tapeDrive->setStatus(WAIT_FOR_UNMOUNT); 
    }

  // Else there is no tape in the drive
  } else {

    // A RELEASE has been requested, therefore the unit is FREE (no job and no
    // volume assigned to it). Update status accordingly.


    // If there is no FORCE_UNMOUNT
    if ( !(ptr_driveRequest->status & VDQM_FORCE_UNMOUNT) &&
         ptr_tapeDrive->status() != FORCED_UNMOUNT ) {

      // Switch tape Drive to status UNIT_UP
      ptr_tapeDrive->setStatus(UNIT_UP);

    } else {
 
      // Switch tape Drive to status FORCED_UNMOUNT
      // If client specified FORCE_UNMOUNT it means that there is "something"
      // mounted which is unknown to the VDQM. We have to wait for the unmount
      // before resetting the status.
      ptr_tapeDrive->setStatus(FORCED_UNMOUNT);

    }
    
    // Always tell the client to unmount just in case there was an
    // error and VDQM wasn't notified.
    ptr_driveRequest->status = VDQM_VOL_UNMOUNT;
  }
}


//------------------------------------------------------------------------------
// handleUnitFreeStatus
//------------------------------------------------------------------------------
void castor::vdqm::handler::TapeDriveStatusHandler::handleUnitFreeStatus() 
  throw (castor::exception::Exception) {

  TapeRequest* tapeRequest = NULL;
  
  ptr_tapeDrive->setStatus(UNIT_UP);
  
  
  tapeRequest = ptr_tapeDrive->runningTapeReq();
  /*
   * Dequeue request and free memory allocated for previous 
   * volume request for this drive
   */
  if ( tapeRequest != NULL ) {
    
      castor::vdqm::DatabaseHelper::deleteRepresentation(tapeRequest,
        m_cuuid);
      delete tapeRequest;
      
      tapeRequest = 0;
      ptr_tapeDrive->setRunningTapeReq(NULL);
  }
  
  /*
   * Reset job IDs
   */
  ptr_tapeDrive->setJobID(0);    
}

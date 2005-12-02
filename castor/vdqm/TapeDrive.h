/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeDrive.h
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

#ifndef CASTOR_VDQM_TAPEDRIVE_H
#define CASTOR_VDQM_TAPEDRIVE_H

// Include Files and Forward declarations for the C world
#include "castor/vdqm/TapeDriveStatusCodes.h"
#include "osdep.h"
struct C_IObject_t;
struct Cstager_Tape_t;
struct Cvdqm_DeviceGroupName_t;
struct Cvdqm_ErrorHistory_t;
struct Cvdqm_TapeDriveCompatibility_t;
struct Cvdqm_TapeDriveDedication_t;
struct Cvdqm_TapeDrive_t;
struct Cvdqm_TapeRequest_t;
struct Cvdqm_TapeServer_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class TapeDrive
// An instance of this class contains the informations of one specific tape drive of
// a server.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cvdqm_TapeDrive_create(struct Cvdqm_TapeDrive_t** obj);

/**
 * Empty Destructor
 */
int Cvdqm_TapeDrive_delete(struct Cvdqm_TapeDrive_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cvdqm_TapeDrive_getIObject(struct Cvdqm_TapeDrive_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cvdqm_TapeDrive_t* Cvdqm_TapeDrive_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cvdqm_TapeDrive_print(struct Cvdqm_TapeDrive_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cvdqm_TapeDrive_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cvdqm_TapeDrive_type(struct Cvdqm_TapeDrive_t* instance,
                         int* ret);

/**
 * virtual method to clone any object
 */
int Cvdqm_TapeDrive_clone(struct Cvdqm_TapeDrive_t* instance,
                          struct C_IObject_t* ret);

/**
 * Get the value of jobID
 * The jobID is given by the tpdaemon to the tape drive, when a job has been
 * assigned.
 */
int Cvdqm_TapeDrive_jobID(struct Cvdqm_TapeDrive_t* instance, int* var);

/**
 * Set the value of jobID
 * The jobID is given by the tpdaemon to the tape drive, when a job has been
 * assigned.
 */
int Cvdqm_TapeDrive_setJobID(struct Cvdqm_TapeDrive_t* instance, int new_var);

/**
 * Get the value of modificationTime
 * The time, when the tape drive begins with its job or modified it
 */
int Cvdqm_TapeDrive_modificationTime(struct Cvdqm_TapeDrive_t* instance, u_signed64* var);

/**
 * Set the value of modificationTime
 * The time, when the tape drive begins with its job or modified it
 */
int Cvdqm_TapeDrive_setModificationTime(struct Cvdqm_TapeDrive_t* instance, u_signed64 new_var);

/**
 * Get the value of resettime
 * Last time counters were reset
 */
int Cvdqm_TapeDrive_resettime(struct Cvdqm_TapeDrive_t* instance, u_signed64* var);

/**
 * Set the value of resettime
 * Last time counters were reset
 */
int Cvdqm_TapeDrive_setResettime(struct Cvdqm_TapeDrive_t* instance, u_signed64 new_var);

/**
 * Get the value of usecount
 * Usage counter (total number of VolReqs so far)
 */
int Cvdqm_TapeDrive_usecount(struct Cvdqm_TapeDrive_t* instance, int* var);

/**
 * Set the value of usecount
 * Usage counter (total number of VolReqs so far)
 */
int Cvdqm_TapeDrive_setUsecount(struct Cvdqm_TapeDrive_t* instance, int new_var);

/**
 * Get the value of errcount
 * Drive error counter
 */
int Cvdqm_TapeDrive_errcount(struct Cvdqm_TapeDrive_t* instance, int* var);

/**
 * Set the value of errcount
 * Drive error counter
 */
int Cvdqm_TapeDrive_setErrcount(struct Cvdqm_TapeDrive_t* instance, int new_var);

/**
 * Get the value of transferredMB
 * MBytes transfered in last request.
 */
int Cvdqm_TapeDrive_transferredMB(struct Cvdqm_TapeDrive_t* instance, int* var);

/**
 * Set the value of transferredMB
 * MBytes transfered in last request.
 */
int Cvdqm_TapeDrive_setTransferredMB(struct Cvdqm_TapeDrive_t* instance, int new_var);

/**
 * Get the value of totalMB
 * Total MBytes transfered
 */
int Cvdqm_TapeDrive_totalMB(struct Cvdqm_TapeDrive_t* instance, u_signed64* var);

/**
 * Set the value of totalMB
 * Total MBytes transfered
 */
int Cvdqm_TapeDrive_setTotalMB(struct Cvdqm_TapeDrive_t* instance, u_signed64 new_var);

/**
 * Get the value of driveName
 * The name of the drive
 */
int Cvdqm_TapeDrive_driveName(struct Cvdqm_TapeDrive_t* instance, const char** var);

/**
 * Set the value of driveName
 * The name of the drive
 */
int Cvdqm_TapeDrive_setDriveName(struct Cvdqm_TapeDrive_t* instance, const char* new_var);

/**
 * Get the value of tapeAccessMode
 * The tape access mode is the information, how the tape drive is accessing the
 * mounted tape. The value could  be WRITE_ENABLE, WRITE_DISABLE or -1 for UNKNOWN.
 */
int Cvdqm_TapeDrive_tapeAccessMode(struct Cvdqm_TapeDrive_t* instance, int* var);

/**
 * Set the value of tapeAccessMode
 * The tape access mode is the information, how the tape drive is accessing the
 * mounted tape. The value could  be WRITE_ENABLE, WRITE_DISABLE or -1 for UNKNOWN.
 */
int Cvdqm_TapeDrive_setTapeAccessMode(struct Cvdqm_TapeDrive_t* instance, int new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cvdqm_TapeDrive_id(struct Cvdqm_TapeDrive_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cvdqm_TapeDrive_setId(struct Cvdqm_TapeDrive_t* instance, u_signed64 new_var);

/**
 * Get the value of tape
 */
int Cvdqm_TapeDrive_tape(struct Cvdqm_TapeDrive_t* instance, struct Cstager_Tape_t** var);

/**
 * Set the value of tape
 */
int Cvdqm_TapeDrive_setTape(struct Cvdqm_TapeDrive_t* instance, struct Cstager_Tape_t* new_var);

/**
 * Get the value of runningTapeReq
 */
int Cvdqm_TapeDrive_runningTapeReq(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeRequest_t** var);

/**
 * Set the value of runningTapeReq
 */
int Cvdqm_TapeDrive_setRunningTapeReq(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeRequest_t* new_var);

/**
 * Add a struct Cvdqm_ErrorHistory_t* object to the errorHistory list
 */
int Cvdqm_TapeDrive_addErrorHistory(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_ErrorHistory_t* obj);

/**
 * Remove a struct Cvdqm_ErrorHistory_t* object from errorHistory
 */
int Cvdqm_TapeDrive_removeErrorHistory(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_ErrorHistory_t* obj);

/**
 * Get the list of struct Cvdqm_ErrorHistory_t* objects held by errorHistory. Note
 * that the caller is responsible for the deletion of the returned vector.
 */
int Cvdqm_TapeDrive_errorHistory(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_ErrorHistory_t*** var, int* len);

/**
 * Add a struct Cvdqm_TapeDriveDedication_t* object to the tapeDriveDedication list
 */
int Cvdqm_TapeDrive_addTapeDriveDedication(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeDriveDedication_t* obj);

/**
 * Remove a struct Cvdqm_TapeDriveDedication_t* object from tapeDriveDedication
 */
int Cvdqm_TapeDrive_removeTapeDriveDedication(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeDriveDedication_t* obj);

/**
 * Get the list of struct Cvdqm_TapeDriveDedication_t* objects held by
 * tapeDriveDedication. Note that the caller is responsible for the deletion of the
 * returned vector.
 */
int Cvdqm_TapeDrive_tapeDriveDedication(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeDriveDedication_t*** var, int* len);

/**
 * Add a struct Cvdqm_TapeDriveCompatibility_t* object to the
 * tapeDriveCompatibilities list
 */
int Cvdqm_TapeDrive_addTapeDriveCompatibilities(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeDriveCompatibility_t* obj);

/**
 * Remove a struct Cvdqm_TapeDriveCompatibility_t* object from
 * tapeDriveCompatibilities
 */
int Cvdqm_TapeDrive_removeTapeDriveCompatibilities(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeDriveCompatibility_t* obj);

/**
 * Get the list of struct Cvdqm_TapeDriveCompatibility_t* objects held by
 * tapeDriveCompatibilities. Note that the caller is responsible for the deletion of
 * the returned vector.
 */
int Cvdqm_TapeDrive_tapeDriveCompatibilities(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeDriveCompatibility_t*** var, int* len);

/**
 * Get the value of deviceGroupName
 */
int Cvdqm_TapeDrive_deviceGroupName(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_DeviceGroupName_t** var);

/**
 * Set the value of deviceGroupName
 */
int Cvdqm_TapeDrive_setDeviceGroupName(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_DeviceGroupName_t* new_var);

/**
 * Get the value of status
 */
int Cvdqm_TapeDrive_status(struct Cvdqm_TapeDrive_t* instance, enum Cvdqm_TapeDriveStatusCodes_t* var);

/**
 * Set the value of status
 */
int Cvdqm_TapeDrive_setStatus(struct Cvdqm_TapeDrive_t* instance, enum Cvdqm_TapeDriveStatusCodes_t new_var);

/**
 * Get the value of tapeServer
 */
int Cvdqm_TapeDrive_tapeServer(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeServer_t** var);

/**
 * Set the value of tapeServer
 */
int Cvdqm_TapeDrive_setTapeServer(struct Cvdqm_TapeDrive_t* instance, struct Cvdqm_TapeServer_t* new_var);

#endif // CASTOR_VDQM_TAPEDRIVE_H

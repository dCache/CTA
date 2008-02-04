/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeRequest.h
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

#ifndef CASTOR_VDQM_TAPEREQUEST_H
#define CASTOR_VDQM_TAPEREQUEST_H

/* Include Files and Forward declarations for the C world */
#include "castor/vdqm/TapeRequestStatusCodes.h"
#include "osdep.h"
struct C_IObject_t;
struct Cvdqm_ClientIdentification_t;
struct Cvdqm_DeviceGroupName_t;
struct Cvdqm_TapeAccessSpecification_t;
struct Cvdqm_TapeDrive_t;
struct Cvdqm_TapeRequest_t;
struct Cvdqm_TapeServer_t;
struct Cvdqm_VdqmTape_t;

/* --------------------------------------------------------------------------
  This defines a C interface to the following class
// class TapeRequest
// A TapeRequest object is instanciated, when a client wants to read or write a
// file frome a tape.
  -------------------------------------------------------------------------- */

/**
 * Empty Constructor
 */
int Cvdqm_TapeRequest_create(struct Cvdqm_TapeRequest_t** obj);

/**
 * Empty Destructor
 */
int Cvdqm_TapeRequest_delete(struct Cvdqm_TapeRequest_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cvdqm_TapeRequest_getIObject(struct Cvdqm_TapeRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cvdqm_TapeRequest_t* Cvdqm_TapeRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cvdqm_TapeRequest_print(struct Cvdqm_TapeRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cvdqm_TapeRequest_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cvdqm_TapeRequest_type(struct Cvdqm_TapeRequest_t* instance,
                           int* ret);

/**
 * virtual method to clone any object
 */
int Cvdqm_TapeRequest_clone(struct Cvdqm_TapeRequest_t* instance,
                            struct C_IObject_t* ret);

/**
 * Get the value of priority
 */
int Cvdqm_TapeRequest_priority(struct Cvdqm_TapeRequest_t* instance, int* var);

/**
 * Set the value of priority
 */
int Cvdqm_TapeRequest_setPriority(struct Cvdqm_TapeRequest_t* instance, int new_var);

/**
 * Get the value of modificationTime
 * The time, when the tape request has been opend or has been modified
 */
int Cvdqm_TapeRequest_modificationTime(struct Cvdqm_TapeRequest_t* instance, u_signed64* var);

/**
 * Set the value of modificationTime
 * The time, when the tape request has been opend or has been modified
 */
int Cvdqm_TapeRequest_setModificationTime(struct Cvdqm_TapeRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of creationTime
 * The time, when the tape request is beeing created
 */
int Cvdqm_TapeRequest_creationTime(struct Cvdqm_TapeRequest_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * The time, when the tape request is beeing created
 */
int Cvdqm_TapeRequest_setCreationTime(struct Cvdqm_TapeRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of errorCode
 * [s]errno in case of failure
 */
int Cvdqm_TapeRequest_errorCode(struct Cvdqm_TapeRequest_t* instance, int* var);

/**
 * Set the value of errorCode
 * [s]errno in case of failure
 */
int Cvdqm_TapeRequest_setErrorCode(struct Cvdqm_TapeRequest_t* instance, int new_var);

/**
 * Get the value of errorMessage
 * Error message in case of failure
 */
int Cvdqm_TapeRequest_errorMessage(struct Cvdqm_TapeRequest_t* instance, const char** var);

/**
 * Set the value of errorMessage
 * Error message in case of failure
 */
int Cvdqm_TapeRequest_setErrorMessage(struct Cvdqm_TapeRequest_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cvdqm_TapeRequest_id(struct Cvdqm_TapeRequest_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cvdqm_TapeRequest_setId(struct Cvdqm_TapeRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of tape
 */
int Cvdqm_TapeRequest_tape(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_VdqmTape_t** var);

/**
 * Set the value of tape
 */
int Cvdqm_TapeRequest_setTape(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_VdqmTape_t* new_var);

/**
 * Get the value of tapeAccessSpecification
 */
int Cvdqm_TapeRequest_tapeAccessSpecification(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_TapeAccessSpecification_t** var);

/**
 * Set the value of tapeAccessSpecification
 */
int Cvdqm_TapeRequest_setTapeAccessSpecification(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_TapeAccessSpecification_t* new_var);

/**
 * Get the value of requestedSrv
 */
int Cvdqm_TapeRequest_requestedSrv(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_TapeServer_t** var);

/**
 * Set the value of requestedSrv
 */
int Cvdqm_TapeRequest_setRequestedSrv(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_TapeServer_t* new_var);

/**
 * Get the value of tapeDrive
 */
int Cvdqm_TapeRequest_tapeDrive(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_TapeDrive_t** var);

/**
 * Set the value of tapeDrive
 */
int Cvdqm_TapeRequest_setTapeDrive(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_TapeDrive_t* new_var);

/**
 * Get the value of deviceGroupName
 */
int Cvdqm_TapeRequest_deviceGroupName(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_DeviceGroupName_t** var);

/**
 * Set the value of deviceGroupName
 */
int Cvdqm_TapeRequest_setDeviceGroupName(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_DeviceGroupName_t* new_var);

/**
 * Get the value of status
 */
int Cvdqm_TapeRequest_status(struct Cvdqm_TapeRequest_t* instance, enum Cvdqm_TapeRequestStatusCodes_t* var);

/**
 * Set the value of status
 */
int Cvdqm_TapeRequest_setStatus(struct Cvdqm_TapeRequest_t* instance, enum Cvdqm_TapeRequestStatusCodes_t new_var);

/**
 * Get the value of client
 */
int Cvdqm_TapeRequest_client(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_ClientIdentification_t** var);

/**
 * Set the value of client
 */
int Cvdqm_TapeRequest_setClient(struct Cvdqm_TapeRequest_t* instance, struct Cvdqm_ClientIdentification_t* new_var);

#endif /* CASTOR_VDQM_TAPEREQUEST_H */

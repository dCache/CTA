/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeDriveDedication.h
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

#ifndef CASTOR_VDQM_TAPEDRIVEDEDICATION_H
#define CASTOR_VDQM_TAPEDRIVEDEDICATION_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Cvdqm_TapeDriveDedication_t;
struct Cvdqm_TapeDrive_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class TapeDriveDedication
// This class is used to specify time slots, where a tape drive should be dedicated
// for a specific tape.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cvdqm_TapeDriveDedication_create(struct Cvdqm_TapeDriveDedication_t** obj);

/**
 * Empty Destructor
 */
int Cvdqm_TapeDriveDedication_delete(struct Cvdqm_TapeDriveDedication_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cvdqm_TapeDriveDedication_getIObject(struct Cvdqm_TapeDriveDedication_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cvdqm_TapeDriveDedication_t* Cvdqm_TapeDriveDedication_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cvdqm_TapeDriveDedication_print(struct Cvdqm_TapeDriveDedication_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cvdqm_TapeDriveDedication_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cvdqm_TapeDriveDedication_type(struct Cvdqm_TapeDriveDedication_t* instance,
                                   int* ret);

/**
 * virtual method to clone any object
 */
int Cvdqm_TapeDriveDedication_clone(struct Cvdqm_TapeDriveDedication_t* instance,
                                    struct C_IObject_t* ret);

/**
 * Get the value of clientHost
 * the client host
 */
int Cvdqm_TapeDriveDedication_clientHost(struct Cvdqm_TapeDriveDedication_t* instance, int* var);

/**
 * Set the value of clientHost
 * the client host
 */
int Cvdqm_TapeDriveDedication_setClientHost(struct Cvdqm_TapeDriveDedication_t* instance, int new_var);

/**
 * Get the value of euid
 * client uid
 */
int Cvdqm_TapeDriveDedication_euid(struct Cvdqm_TapeDriveDedication_t* instance, int* var);

/**
 * Set the value of euid
 * client uid
 */
int Cvdqm_TapeDriveDedication_setEuid(struct Cvdqm_TapeDriveDedication_t* instance, int new_var);

/**
 * Get the value of egid
 * client gid
 */
int Cvdqm_TapeDriveDedication_egid(struct Cvdqm_TapeDriveDedication_t* instance, int* var);

/**
 * Set the value of egid
 * client gid
 */
int Cvdqm_TapeDriveDedication_setEgid(struct Cvdqm_TapeDriveDedication_t* instance, int new_var);

/**
 * Get the value of vid
 * the tape, which the client want to  access
 */
int Cvdqm_TapeDriveDedication_vid(struct Cvdqm_TapeDriveDedication_t* instance, const char** var);

/**
 * Set the value of vid
 * the tape, which the client want to  access
 */
int Cvdqm_TapeDriveDedication_setVid(struct Cvdqm_TapeDriveDedication_t* instance, const char* new_var);

/**
 * Get the value of accessMode
 * the mode, which the client want to use to access the tape. This can be READ or
 * WRITE access
 */
int Cvdqm_TapeDriveDedication_accessMode(struct Cvdqm_TapeDriveDedication_t* instance, int* var);

/**
 * Set the value of accessMode
 * the mode, which the client want to use to access the tape. This can be READ or
 * WRITE access
 */
int Cvdqm_TapeDriveDedication_setAccessMode(struct Cvdqm_TapeDriveDedication_t* instance, int new_var);

/**
 * Get the value of timePeriods
 * The time periods, in which the user wants to have the specified tape drive
 * dedicated for its soecified tape
 */
int Cvdqm_TapeDriveDedication_timePeriods(struct Cvdqm_TapeDriveDedication_t* instance, const char** var);

/**
 * Set the value of timePeriods
 * The time periods, in which the user wants to have the specified tape drive
 * dedicated for its soecified tape
 */
int Cvdqm_TapeDriveDedication_setTimePeriods(struct Cvdqm_TapeDriveDedication_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cvdqm_TapeDriveDedication_id(struct Cvdqm_TapeDriveDedication_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cvdqm_TapeDriveDedication_setId(struct Cvdqm_TapeDriveDedication_t* instance, u_signed64 new_var);

/**
 * Get the value of tapeDrive
 */
int Cvdqm_TapeDriveDedication_tapeDrive(struct Cvdqm_TapeDriveDedication_t* instance, struct Cvdqm_TapeDrive_t** var);

/**
 * Set the value of tapeDrive
 */
int Cvdqm_TapeDriveDedication_setTapeDrive(struct Cvdqm_TapeDriveDedication_t* instance, struct Cvdqm_TapeDrive_t* new_var);

#endif // CASTOR_VDQM_TAPEDRIVEDEDICATION_H

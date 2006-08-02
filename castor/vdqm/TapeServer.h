/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeServer.h
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

#ifndef CASTOR_VDQM_TAPESERVER_H
#define CASTOR_VDQM_TAPESERVER_H

// Include Files and Forward declarations for the C world
#include "castor/vdqm/TapeServerStatusCodes.h"
#include "osdep.h"
struct C_IObject_t;
struct Cvdqm_TapeDrive_t;
struct Cvdqm_TapeServer_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class TapeServer
// Holds the information about the tape server. Every tape server has several tape
// drives. If its actingMode says that it is in SERVER_INACTIVE, then its tape
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cvdqm_TapeServer_create(struct Cvdqm_TapeServer_t** obj);

/**
 * Empty Destructor
 */
int Cvdqm_TapeServer_delete(struct Cvdqm_TapeServer_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cvdqm_TapeServer_getIObject(struct Cvdqm_TapeServer_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cvdqm_TapeServer_t* Cvdqm_TapeServer_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cvdqm_TapeServer_print(struct Cvdqm_TapeServer_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cvdqm_TapeServer_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cvdqm_TapeServer_type(struct Cvdqm_TapeServer_t* instance,
                          int* ret);

/**
 * virtual method to clone any object
 */
int Cvdqm_TapeServer_clone(struct Cvdqm_TapeServer_t* instance,
                           struct C_IObject_t* ret);

/**
 * Get the value of serverName
 * The name of the tape server
 */
int Cvdqm_TapeServer_serverName(struct Cvdqm_TapeServer_t* instance, const char** var);

/**
 * Set the value of serverName
 * The name of the tape server
 */
int Cvdqm_TapeServer_setServerName(struct Cvdqm_TapeServer_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cvdqm_TapeServer_id(struct Cvdqm_TapeServer_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cvdqm_TapeServer_setId(struct Cvdqm_TapeServer_t* instance, u_signed64 new_var);

/**
 * Add a struct Cvdqm_TapeDrive_t* object to the tapeDrives list
 */
int Cvdqm_TapeServer_addTapeDrives(struct Cvdqm_TapeServer_t* instance, struct Cvdqm_TapeDrive_t* obj);

/**
 * Remove a struct Cvdqm_TapeDrive_t* object from tapeDrives
 */
int Cvdqm_TapeServer_removeTapeDrives(struct Cvdqm_TapeServer_t* instance, struct Cvdqm_TapeDrive_t* obj);

/**
 * Get the list of struct Cvdqm_TapeDrive_t* objects held by tapeDrives. Note that
 */
int Cvdqm_TapeServer_tapeDrives(struct Cvdqm_TapeServer_t* instance, struct Cvdqm_TapeDrive_t*** var, int* len);

/**
 * Get the value of actingMode
 */
int Cvdqm_TapeServer_actingMode(struct Cvdqm_TapeServer_t* instance, enum Cvdqm_TapeServerStatusCodes_t* var);

/**
 * Set the value of actingMode
 */
int Cvdqm_TapeServer_setActingMode(struct Cvdqm_TapeServer_t* instance, enum Cvdqm_TapeServerStatusCodes_t new_var);

#endif // CASTOR_VDQM_TAPESERVER_H

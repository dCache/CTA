/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/ErrorHistory.h
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

#ifndef CASTOR_VDQM_ERRORHISTORY_H
#define CASTOR_VDQM_ERRORHISTORY_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Cstager_Tape_t;
struct Cvdqm_ErrorHistory_t;
struct Cvdqm_TapeDrive_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class ErrorHistory
// This table holds all the errors, which occures for a specified tape drive with a
// dedicated tape.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cvdqm_ErrorHistory_create(struct Cvdqm_ErrorHistory_t** obj);

/**
 * Empty Destructor
 */
int Cvdqm_ErrorHistory_delete(struct Cvdqm_ErrorHistory_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cvdqm_ErrorHistory_getIObject(struct Cvdqm_ErrorHistory_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cvdqm_ErrorHistory_t* Cvdqm_ErrorHistory_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cvdqm_ErrorHistory_print(struct Cvdqm_ErrorHistory_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cvdqm_ErrorHistory_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cvdqm_ErrorHistory_type(struct Cvdqm_ErrorHistory_t* instance,
                            int* ret);

/**
 * virtual method to clone any object
 */
int Cvdqm_ErrorHistory_clone(struct Cvdqm_ErrorHistory_t* instance,
                             struct C_IObject_t* ret);

/**
 * Get the value of errorMessage
 * The error message, which was thrown for the specified tape drive with the
 * specified mounted tape.
 */
int Cvdqm_ErrorHistory_errorMessage(struct Cvdqm_ErrorHistory_t* instance, const char** var);

/**
 * Set the value of errorMessage
 * The error message, which was thrown for the specified tape drive with the
 * specified mounted tape.
 */
int Cvdqm_ErrorHistory_setErrorMessage(struct Cvdqm_ErrorHistory_t* instance, const char* new_var);

/**
 * Get the value of timeStamp
 * The time, when the error occured
 */
int Cvdqm_ErrorHistory_timeStamp(struct Cvdqm_ErrorHistory_t* instance, u_signed64* var);

/**
 * Set the value of timeStamp
 * The time, when the error occured
 */
int Cvdqm_ErrorHistory_setTimeStamp(struct Cvdqm_ErrorHistory_t* instance, u_signed64 new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cvdqm_ErrorHistory_id(struct Cvdqm_ErrorHistory_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cvdqm_ErrorHistory_setId(struct Cvdqm_ErrorHistory_t* instance, u_signed64 new_var);

/**
 * Get the value of tapeDrive
 */
int Cvdqm_ErrorHistory_tapeDrive(struct Cvdqm_ErrorHistory_t* instance, struct Cvdqm_TapeDrive_t** var);

/**
 * Set the value of tapeDrive
 */
int Cvdqm_ErrorHistory_setTapeDrive(struct Cvdqm_ErrorHistory_t* instance, struct Cvdqm_TapeDrive_t* new_var);

/**
 * Get the value of tape
 */
int Cvdqm_ErrorHistory_tape(struct Cvdqm_ErrorHistory_t* instance, struct Cstager_Tape_t** var);

/**
 * Set the value of tape
 */
int Cvdqm_ErrorHistory_setTape(struct Cvdqm_ErrorHistory_t* instance, struct Cstager_Tape_t* new_var);

#endif // CASTOR_VDQM_ERRORHISTORY_H

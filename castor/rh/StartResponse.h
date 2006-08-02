/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/rh/StartResponse.h
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

#ifndef CASTOR_RH_STARTRESPONSE_H
#define CASTOR_RH_STARTRESPONSE_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Crh_Response_t;
struct Crh_StartResponse_t;
struct Cstager_DiskCopy_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StartResponse
// A response dedicated to cases where an IClient and a DiskCopy are returned. This
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Crh_StartResponse_create(struct Crh_StartResponse_t** obj);

/**
 * Empty Destructor
 */
int Crh_StartResponse_delete(struct Crh_StartResponse_t* obj);

/**
 * Cast into Response
 */
struct Crh_Response_t* Crh_StartResponse_getResponse(struct Crh_StartResponse_t* obj);

/**
 * Dynamic cast from Response
 */
struct Crh_StartResponse_t* Crh_StartResponse_fromResponse(struct Crh_Response_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Crh_StartResponse_getIObject(struct Crh_StartResponse_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Crh_StartResponse_t* Crh_StartResponse_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Crh_StartResponse_print(struct Crh_StartResponse_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Crh_StartResponse_TYPE(int* ret);

/*********************************************/
/* Implementation of Response abstract class */
/*********************************************/

/**
 * Get the value of errorCode
 * The error code in case of error
 */
int Crh_StartResponse_errorCode(struct Crh_StartResponse_t* instance, unsigned int* var);

/**
 * Set the value of errorCode
 * The error code in case of error
 */
int Crh_StartResponse_setErrorCode(struct Crh_StartResponse_t* instance, unsigned int new_var);

/**
 * Get the value of errorMessage
 * The error message in case of error
 */
int Crh_StartResponse_errorMessage(struct Crh_StartResponse_t* instance, const char** var);

/**
 * Set the value of errorMessage
 * The error message in case of error
 */
int Crh_StartResponse_setErrorMessage(struct Crh_StartResponse_t* instance, const char* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Crh_StartResponse_type(struct Crh_StartResponse_t* instance,
                           int* ret);

/**
 * virtual method to clone any object
 */
int Crh_StartResponse_clone(struct Crh_StartResponse_t* instance,
                            struct C_IObject_t* ret);

/**
 * Get the value of id
 * The id of this object
 */
int Crh_StartResponse_id(struct Crh_StartResponse_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Crh_StartResponse_setId(struct Crh_StartResponse_t* instance, u_signed64 new_var);

/**
 * Get the value of diskCopy
 */
int Crh_StartResponse_diskCopy(struct Crh_StartResponse_t* instance, struct Cstager_DiskCopy_t** var);

/**
 * Set the value of diskCopy
 */
int Crh_StartResponse_setDiskCopy(struct Crh_StartResponse_t* instance, struct Cstager_DiskCopy_t* new_var);

#endif // CASTOR_RH_STARTRESPONSE_H

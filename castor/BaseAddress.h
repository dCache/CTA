/******************************************************************************
 *                      castor/BaseAddress.h
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

#ifndef CASTOR_BASEADDRESS_H
#define CASTOR_BASEADDRESS_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_BaseAddress_t;
struct C_IAddress_t;
struct C_IObject_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class BaseAddress
// The most basic address : only a type.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int C_BaseAddress_create(struct C_BaseAddress_t** obj);

/**
 * Empty Destructor
 */
int C_BaseAddress_delete(struct C_BaseAddress_t* obj);

/**
 * Cast into IAddress
 */
struct C_IAddress_t* C_BaseAddress_getIAddress(struct C_BaseAddress_t* obj);

/**
 * Dynamic cast from IAddress
 */
struct C_BaseAddress_t* C_BaseAddress_fromIAddress(struct C_IAddress_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* C_BaseAddress_getIObject(struct C_BaseAddress_t* obj);

/**
 * Dynamic cast from IObject
 */
struct C_BaseAddress_t* C_BaseAddress_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int C_BaseAddress_print(struct C_BaseAddress_t* instance);

/**
 * Gets the type of this kind of objects
 */
int C_BaseAddress_TYPE(int* ret);

/****************************************/
/* Implementation of IAddress interface */
/****************************************/

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int C_BaseAddress_type(struct C_BaseAddress_t* instance,
                       int* ret);

/**
 * virtual method to clone any object
 */
int C_BaseAddress_clone(struct C_BaseAddress_t* instance,
                        struct C_IObject_t* ret);

/**
 * Get the value of objType
 * the object type of this address
 */
int C_BaseAddress_objType(struct C_BaseAddress_t* instance, unsigned int* var);

/**
 * Set the value of objType
 * the object type of this address
 */
int C_BaseAddress_setObjType(struct C_BaseAddress_t* instance, unsigned int new_var);

/**
 * Get the value of cnvSvcName
 * the name of the conversion service able to deal with this address
 */
int C_BaseAddress_cnvSvcName(struct C_BaseAddress_t* instance, const char** var);

/**
 * Set the value of cnvSvcName
 * the name of the conversion service able to deal with this address
 */
int C_BaseAddress_setCnvSvcName(struct C_BaseAddress_t* instance, const char* new_var);

/**
 * Get the value of cnvSvcType
 * the type of the conversion service able to deal with this address
 */
int C_BaseAddress_cnvSvcType(struct C_BaseAddress_t* instance, unsigned int* var);

/**
 * Set the value of cnvSvcType
 * the type of the conversion service able to deal with this address
 */
int C_BaseAddress_setCnvSvcType(struct C_BaseAddress_t* instance, unsigned int new_var);

/**
 * Get the value of target
 * The id of the object this address points to, if any
 */
int C_BaseAddress_target(struct C_BaseAddress_t* instance, u_signed64* var);

/**
 * Set the value of target
 * The id of the object this address points to, if any
 */
int C_BaseAddress_setTarget(struct C_BaseAddress_t* instance, u_signed64 new_var);

/**
 * Get the value of id
 * The id of this object
 */
int C_BaseAddress_id(struct C_BaseAddress_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int C_BaseAddress_setId(struct C_BaseAddress_t* instance, u_signed64 new_var);

#endif // CASTOR_BASEADDRESS_H

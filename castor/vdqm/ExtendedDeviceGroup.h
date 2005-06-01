/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/ExtendedDeviceGroup.h
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

#ifndef CASTOR_VDQM_EXTENDEDDEVICEGROUP_H
#define CASTOR_VDQM_EXTENDEDDEVICEGROUP_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Cvdqm_ExtendedDeviceGroup_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class ExtendedDeviceGroup
// The exetended device group table contains all supported tape drive types and
// modes. A tape drive can be associated with several entries, if it is capable to
// support them. 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cvdqm_ExtendedDeviceGroup_create(struct Cvdqm_ExtendedDeviceGroup_t** obj);

/**
 * Empty Destructor
 */
int Cvdqm_ExtendedDeviceGroup_delete(struct Cvdqm_ExtendedDeviceGroup_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cvdqm_ExtendedDeviceGroup_getIObject(struct Cvdqm_ExtendedDeviceGroup_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cvdqm_ExtendedDeviceGroup_t* Cvdqm_ExtendedDeviceGroup_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cvdqm_ExtendedDeviceGroup_print(struct Cvdqm_ExtendedDeviceGroup_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cvdqm_ExtendedDeviceGroup_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cvdqm_ExtendedDeviceGroup_type(struct Cvdqm_ExtendedDeviceGroup_t* instance,
                                   int* ret);

/**
 * virtual method to clone any object
 */
int Cvdqm_ExtendedDeviceGroup_clone(struct Cvdqm_ExtendedDeviceGroup_t* instance,
                                    struct C_IObject_t* ret);

/**
 * Get the value of dgName
 */
int Cvdqm_ExtendedDeviceGroup_dgName(struct Cvdqm_ExtendedDeviceGroup_t* instance, const char** var);

/**
 * Set the value of dgName
 */
int Cvdqm_ExtendedDeviceGroup_setDgName(struct Cvdqm_ExtendedDeviceGroup_t* instance, const char* new_var);

/**
 * Get the value of accessMode
 * WRITE_ENABLE/WRITE_DISABLE from Ctape_constants.h
 */
int Cvdqm_ExtendedDeviceGroup_accessMode(struct Cvdqm_ExtendedDeviceGroup_t* instance, int* var);

/**
 * Set the value of accessMode
 * WRITE_ENABLE/WRITE_DISABLE from Ctape_constants.h
 */
int Cvdqm_ExtendedDeviceGroup_setAccessMode(struct Cvdqm_ExtendedDeviceGroup_t* instance, int new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cvdqm_ExtendedDeviceGroup_id(struct Cvdqm_ExtendedDeviceGroup_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cvdqm_ExtendedDeviceGroup_setId(struct Cvdqm_ExtendedDeviceGroup_t* instance, u_signed64 new_var);

#endif // CASTOR_VDQM_EXTENDEDDEVICEGROUP_H

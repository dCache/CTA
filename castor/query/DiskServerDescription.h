/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/query/DiskServerDescription.h
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

#ifndef CASTOR_QUERY_DISKSERVERDESCRIPTION_H
#define CASTOR_QUERY_DISKSERVERDESCRIPTION_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Cquery_DiskPoolQueryResponse_t;
struct Cquery_DiskServerDescription_t;
struct Cquery_FileSystemDescription_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class DiskServerDescription
// Describe a Diskserver and its current state
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cquery_DiskServerDescription_create(struct Cquery_DiskServerDescription_t** obj);

/**
 * Empty Destructor
 */
int Cquery_DiskServerDescription_delete(struct Cquery_DiskServerDescription_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cquery_DiskServerDescription_getIObject(struct Cquery_DiskServerDescription_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cquery_DiskServerDescription_t* Cquery_DiskServerDescription_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cquery_DiskServerDescription_print(struct Cquery_DiskServerDescription_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cquery_DiskServerDescription_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cquery_DiskServerDescription_type(struct Cquery_DiskServerDescription_t* instance,
                                      int* ret);

/**
 * virtual method to clone any object
 */
int Cquery_DiskServerDescription_clone(struct Cquery_DiskServerDescription_t* instance,
                                       struct C_IObject_t* ret);

/**
 * Get the value of name
 * Name of the diskserver
 */
int Cquery_DiskServerDescription_name(struct Cquery_DiskServerDescription_t* instance, const char** var);

/**
 * Set the value of name
 * Name of the diskserver
 */
int Cquery_DiskServerDescription_setName(struct Cquery_DiskServerDescription_t* instance, const char* new_var);

/**
 * Get the value of status
 * Status of the DiskServer
 */
int Cquery_DiskServerDescription_status(struct Cquery_DiskServerDescription_t* instance, int* var);

/**
 * Set the value of status
 * Status of the DiskServer
 */
int Cquery_DiskServerDescription_setStatus(struct Cquery_DiskServerDescription_t* instance, int new_var);

/**
 * Get the value of freeSpace
 * The total free space on the DiskServer
 */
int Cquery_DiskServerDescription_freeSpace(struct Cquery_DiskServerDescription_t* instance, u_signed64* var);

/**
 * Set the value of freeSpace
 * The total free space on the DiskServer
 */
int Cquery_DiskServerDescription_setFreeSpace(struct Cquery_DiskServerDescription_t* instance, u_signed64 new_var);

/**
 * Get the value of totalSpace
 * The total space provided by this diskServer
 */
int Cquery_DiskServerDescription_totalSpace(struct Cquery_DiskServerDescription_t* instance, u_signed64* var);

/**
 * Set the value of totalSpace
 * The total space provided by this diskServer
 */
int Cquery_DiskServerDescription_setTotalSpace(struct Cquery_DiskServerDescription_t* instance, u_signed64 new_var);

/**
 * Get the value of reservedSpace
 * The space reserved on this diskServer
 */
int Cquery_DiskServerDescription_reservedSpace(struct Cquery_DiskServerDescription_t* instance, u_signed64* var);

/**
 * Set the value of reservedSpace
 * The space reserved on this diskServer
 */
int Cquery_DiskServerDescription_setReservedSpace(struct Cquery_DiskServerDescription_t* instance, u_signed64 new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cquery_DiskServerDescription_id(struct Cquery_DiskServerDescription_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cquery_DiskServerDescription_setId(struct Cquery_DiskServerDescription_t* instance, u_signed64 new_var);

/**
 * Get the value of query
 */
int Cquery_DiskServerDescription_query(struct Cquery_DiskServerDescription_t* instance, struct Cquery_DiskPoolQueryResponse_t** var);

/**
 * Set the value of query
 */
int Cquery_DiskServerDescription_setQuery(struct Cquery_DiskServerDescription_t* instance, struct Cquery_DiskPoolQueryResponse_t* new_var);

/**
 * Add a struct Cquery_FileSystemDescription_t* object to the fileSystems list
 */
int Cquery_DiskServerDescription_addFileSystems(struct Cquery_DiskServerDescription_t* instance, struct Cquery_FileSystemDescription_t* obj);

/**
 * Remove a struct Cquery_FileSystemDescription_t* object from fileSystems
 */
int Cquery_DiskServerDescription_removeFileSystems(struct Cquery_DiskServerDescription_t* instance, struct Cquery_FileSystemDescription_t* obj);

/**
 * Get the list of struct Cquery_FileSystemDescription_t* objects held by
 * fileSystems. Note that the caller is responsible for the deletion of the returned
 * vector.
 */
int Cquery_DiskServerDescription_fileSystems(struct Cquery_DiskServerDescription_t* instance, struct Cquery_FileSystemDescription_t*** var, int* len);

#endif // CASTOR_QUERY_DISKSERVERDESCRIPTION_H

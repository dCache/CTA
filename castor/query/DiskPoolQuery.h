/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/query/DiskPoolQuery.h
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

#ifndef CASTOR_QUERY_DISKPOOLQUERY_H
#define CASTOR_QUERY_DISKPOOLQUERY_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cquery_DiskPoolQuery_t;
struct Cstager_Request_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class DiskPoolQuery
// Request for the description of a diskpool (or all if no name given)
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cquery_DiskPoolQuery_create(struct Cquery_DiskPoolQuery_t** obj);

/**
 * Empty Destructor
 */
int Cquery_DiskPoolQuery_delete(struct Cquery_DiskPoolQuery_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cquery_DiskPoolQuery_getRequest(struct Cquery_DiskPoolQuery_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cquery_DiskPoolQuery_t* Cquery_DiskPoolQuery_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cquery_DiskPoolQuery_getIObject(struct Cquery_DiskPoolQuery_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cquery_DiskPoolQuery_t* Cquery_DiskPoolQuery_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cquery_DiskPoolQuery_print(struct Cquery_DiskPoolQuery_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cquery_DiskPoolQuery_TYPE(int* ret);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cquery_DiskPoolQuery_flags(struct Cquery_DiskPoolQuery_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cquery_DiskPoolQuery_setFlags(struct Cquery_DiskPoolQuery_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cquery_DiskPoolQuery_userName(struct Cquery_DiskPoolQuery_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cquery_DiskPoolQuery_setUserName(struct Cquery_DiskPoolQuery_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cquery_DiskPoolQuery_euid(struct Cquery_DiskPoolQuery_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cquery_DiskPoolQuery_setEuid(struct Cquery_DiskPoolQuery_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cquery_DiskPoolQuery_egid(struct Cquery_DiskPoolQuery_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cquery_DiskPoolQuery_setEgid(struct Cquery_DiskPoolQuery_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cquery_DiskPoolQuery_mask(struct Cquery_DiskPoolQuery_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cquery_DiskPoolQuery_setMask(struct Cquery_DiskPoolQuery_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cquery_DiskPoolQuery_pid(struct Cquery_DiskPoolQuery_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cquery_DiskPoolQuery_setPid(struct Cquery_DiskPoolQuery_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cquery_DiskPoolQuery_machine(struct Cquery_DiskPoolQuery_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cquery_DiskPoolQuery_setMachine(struct Cquery_DiskPoolQuery_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cquery_DiskPoolQuery_svcClassName(struct Cquery_DiskPoolQuery_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cquery_DiskPoolQuery_setSvcClassName(struct Cquery_DiskPoolQuery_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 */
int Cquery_DiskPoolQuery_userTag(struct Cquery_DiskPoolQuery_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 */
int Cquery_DiskPoolQuery_setUserTag(struct Cquery_DiskPoolQuery_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cquery_DiskPoolQuery_reqId(struct Cquery_DiskPoolQuery_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cquery_DiskPoolQuery_setReqId(struct Cquery_DiskPoolQuery_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cquery_DiskPoolQuery_creationTime(struct Cquery_DiskPoolQuery_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cquery_DiskPoolQuery_setCreationTime(struct Cquery_DiskPoolQuery_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cquery_DiskPoolQuery_lastModificationTime(struct Cquery_DiskPoolQuery_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cquery_DiskPoolQuery_setLastModificationTime(struct Cquery_DiskPoolQuery_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cquery_DiskPoolQuery_svcClass(struct Cquery_DiskPoolQuery_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cquery_DiskPoolQuery_setSvcClass(struct Cquery_DiskPoolQuery_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cquery_DiskPoolQuery_client(struct Cquery_DiskPoolQuery_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cquery_DiskPoolQuery_setClient(struct Cquery_DiskPoolQuery_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cquery_DiskPoolQuery_type(struct Cquery_DiskPoolQuery_t* instance,
                              int* ret);

/**
 * virtual method to clone any object
 */
int Cquery_DiskPoolQuery_clone(struct Cquery_DiskPoolQuery_t* instance,
                               struct C_IObject_t* ret);

/**
 * Get the value of diskPoolName
 * Name of the diskpool that should be described, or empty string for a description
 */
int Cquery_DiskPoolQuery_diskPoolName(struct Cquery_DiskPoolQuery_t* instance, const char** var);

/**
 * Set the value of diskPoolName
 * Name of the diskpool that should be described, or empty string for a description
 */
int Cquery_DiskPoolQuery_setDiskPoolName(struct Cquery_DiskPoolQuery_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cquery_DiskPoolQuery_id(struct Cquery_DiskPoolQuery_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cquery_DiskPoolQuery_setId(struct Cquery_DiskPoolQuery_t* instance, u_signed64 new_var);

#endif // CASTOR_QUERY_DISKPOOLQUERY_H

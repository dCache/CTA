/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/SetFileGCWeight.h
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

#ifndef CASTOR_STAGER_SETFILEGCWEIGHT_H
#define CASTOR_STAGER_SETFILEGCWEIGHT_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_FileRequest_t;
struct Cstager_Request_t;
struct Cstager_SetFileGCWeight_t;
struct Cstager_SubRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class SetFileGCWeight
// Request for setting the weight of all diskcopies of a given files. This weight
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_SetFileGCWeight_create(struct Cstager_SetFileGCWeight_t** obj);

/**
 * Empty Destructor
 */
int Cstager_SetFileGCWeight_delete(struct Cstager_SetFileGCWeight_t* obj);

/**
 * Cast into FileRequest
 */
struct Cstager_FileRequest_t* Cstager_SetFileGCWeight_getFileRequest(struct Cstager_SetFileGCWeight_t* obj);

/**
 * Dynamic cast from FileRequest
 */
struct Cstager_SetFileGCWeight_t* Cstager_SetFileGCWeight_fromFileRequest(struct Cstager_FileRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_SetFileGCWeight_getRequest(struct Cstager_SetFileGCWeight_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_SetFileGCWeight_t* Cstager_SetFileGCWeight_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_SetFileGCWeight_getIObject(struct Cstager_SetFileGCWeight_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_SetFileGCWeight_t* Cstager_SetFileGCWeight_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_SetFileGCWeight_print(struct Cstager_SetFileGCWeight_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_SetFileGCWeight_TYPE(int* ret);

/************************************************/
/* Implementation of FileRequest abstract class */
/************************************************/

/**
 * Add a struct Cstager_SubRequest_t* object to the subRequests list
 */
int Cstager_SetFileGCWeight_addSubRequests(struct Cstager_SetFileGCWeight_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Remove a struct Cstager_SubRequest_t* object from subRequests
 */
int Cstager_SetFileGCWeight_removeSubRequests(struct Cstager_SetFileGCWeight_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Get the list of struct Cstager_SubRequest_t* objects held by subRequests. Note
 */
int Cstager_SetFileGCWeight_subRequests(struct Cstager_SetFileGCWeight_t* instance, struct Cstager_SubRequest_t*** var, int* len);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_SetFileGCWeight_flags(struct Cstager_SetFileGCWeight_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_SetFileGCWeight_setFlags(struct Cstager_SetFileGCWeight_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_SetFileGCWeight_userName(struct Cstager_SetFileGCWeight_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_SetFileGCWeight_setUserName(struct Cstager_SetFileGCWeight_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_SetFileGCWeight_euid(struct Cstager_SetFileGCWeight_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_SetFileGCWeight_setEuid(struct Cstager_SetFileGCWeight_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_SetFileGCWeight_egid(struct Cstager_SetFileGCWeight_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_SetFileGCWeight_setEgid(struct Cstager_SetFileGCWeight_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_SetFileGCWeight_mask(struct Cstager_SetFileGCWeight_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_SetFileGCWeight_setMask(struct Cstager_SetFileGCWeight_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_SetFileGCWeight_pid(struct Cstager_SetFileGCWeight_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_SetFileGCWeight_setPid(struct Cstager_SetFileGCWeight_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_SetFileGCWeight_machine(struct Cstager_SetFileGCWeight_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_SetFileGCWeight_setMachine(struct Cstager_SetFileGCWeight_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_SetFileGCWeight_svcClassName(struct Cstager_SetFileGCWeight_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_SetFileGCWeight_setSvcClassName(struct Cstager_SetFileGCWeight_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 */
int Cstager_SetFileGCWeight_userTag(struct Cstager_SetFileGCWeight_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 */
int Cstager_SetFileGCWeight_setUserTag(struct Cstager_SetFileGCWeight_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_SetFileGCWeight_reqId(struct Cstager_SetFileGCWeight_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_SetFileGCWeight_setReqId(struct Cstager_SetFileGCWeight_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_SetFileGCWeight_creationTime(struct Cstager_SetFileGCWeight_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_SetFileGCWeight_setCreationTime(struct Cstager_SetFileGCWeight_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_SetFileGCWeight_lastModificationTime(struct Cstager_SetFileGCWeight_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_SetFileGCWeight_setLastModificationTime(struct Cstager_SetFileGCWeight_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_SetFileGCWeight_svcClass(struct Cstager_SetFileGCWeight_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_SetFileGCWeight_setSvcClass(struct Cstager_SetFileGCWeight_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_SetFileGCWeight_client(struct Cstager_SetFileGCWeight_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_SetFileGCWeight_setClient(struct Cstager_SetFileGCWeight_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_SetFileGCWeight_type(struct Cstager_SetFileGCWeight_t* instance,
                                 int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_SetFileGCWeight_clone(struct Cstager_SetFileGCWeight_t* instance,
                                  struct C_IObject_t* ret);

/**
 * Get the value of weight
 * The new weight of the file(s)
 */
int Cstager_SetFileGCWeight_weight(struct Cstager_SetFileGCWeight_t* instance, float* var);

/**
 * Set the value of weight
 * The new weight of the file(s)
 */
int Cstager_SetFileGCWeight_setWeight(struct Cstager_SetFileGCWeight_t* instance, float new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_SetFileGCWeight_id(struct Cstager_SetFileGCWeight_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_SetFileGCWeight_setId(struct Cstager_SetFileGCWeight_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_SETFILEGCWEIGHT_H

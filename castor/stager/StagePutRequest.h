/******************************************************************************
 *                      castor/stager/StagePutRequest.h
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

#ifndef CASTOR_STAGER_STAGEPUTREQUEST_H
#define CASTOR_STAGER_STAGEPUTREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_FileRequest_t;
struct Cstager_Request_t;
struct Cstager_StagePutRequest_t;
struct Cstager_SubRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StagePutRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StagePutRequest_create(struct Cstager_StagePutRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StagePutRequest_delete(struct Cstager_StagePutRequest_t* obj);

/**
 * Cast into FileRequest
 */
struct Cstager_FileRequest_t* Cstager_StagePutRequest_getFileRequest(struct Cstager_StagePutRequest_t* obj);

/**
 * Dynamic cast from FileRequest
 */
struct Cstager_StagePutRequest_t* Cstager_StagePutRequest_fromFileRequest(struct Cstager_FileRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_StagePutRequest_getRequest(struct Cstager_StagePutRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_StagePutRequest_t* Cstager_StagePutRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_StagePutRequest_getIObject(struct Cstager_StagePutRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_StagePutRequest_t* Cstager_StagePutRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StagePutRequest_print(struct Cstager_StagePutRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StagePutRequest_TYPE(int* ret);

/************************************************/
/* Implementation of FileRequest abstract class */
/************************************************/

/**
 * Add a struct Cstager_SubRequest_t* object to the subRequests list
 */
int Cstager_StagePutRequest_addSubRequests(struct Cstager_StagePutRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Remove a struct Cstager_SubRequest_t* object from subRequests
 */
int Cstager_StagePutRequest_removeSubRequests(struct Cstager_StagePutRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Get the list of struct Cstager_SubRequest_t* objects held by subRequests
 */
int Cstager_StagePutRequest_subRequests(struct Cstager_StagePutRequest_t* instance, struct Cstager_SubRequest_t*** var, int* len);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StagePutRequest_flags(struct Cstager_StagePutRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StagePutRequest_setFlags(struct Cstager_StagePutRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StagePutRequest_userName(struct Cstager_StagePutRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StagePutRequest_setUserName(struct Cstager_StagePutRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StagePutRequest_euid(struct Cstager_StagePutRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StagePutRequest_setEuid(struct Cstager_StagePutRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StagePutRequest_egid(struct Cstager_StagePutRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StagePutRequest_setEgid(struct Cstager_StagePutRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StagePutRequest_mask(struct Cstager_StagePutRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StagePutRequest_setMask(struct Cstager_StagePutRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StagePutRequest_pid(struct Cstager_StagePutRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StagePutRequest_setPid(struct Cstager_StagePutRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StagePutRequest_machine(struct Cstager_StagePutRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StagePutRequest_setMachine(struct Cstager_StagePutRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StagePutRequest_svcClassName(struct Cstager_StagePutRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StagePutRequest_setSvcClassName(struct Cstager_StagePutRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StagePutRequest_userTag(struct Cstager_StagePutRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StagePutRequest_setUserTag(struct Cstager_StagePutRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StagePutRequest_reqId(struct Cstager_StagePutRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StagePutRequest_setReqId(struct Cstager_StagePutRequest_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_StagePutRequest_creationTime(struct Cstager_StagePutRequest_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_StagePutRequest_setCreationTime(struct Cstager_StagePutRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_StagePutRequest_lastModificationTime(struct Cstager_StagePutRequest_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_StagePutRequest_setLastModificationTime(struct Cstager_StagePutRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StagePutRequest_svcClass(struct Cstager_StagePutRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StagePutRequest_setSvcClass(struct Cstager_StagePutRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StagePutRequest_client(struct Cstager_StagePutRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StagePutRequest_setClient(struct Cstager_StagePutRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_StagePutRequest_type(struct Cstager_StagePutRequest_t* instance,
                                 int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_StagePutRequest_clone(struct Cstager_StagePutRequest_t* instance,
                                  struct C_IObject_t* ret);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_StagePutRequest_id(struct Cstager_StagePutRequest_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_StagePutRequest_setId(struct Cstager_StagePutRequest_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_STAGEPUTREQUEST_H

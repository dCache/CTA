/******************************************************************************
 *                      castor/stager/StageRmRequest.h
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

#ifndef CASTOR_STAGER_STAGERMREQUEST_H
#define CASTOR_STAGER_STAGERMREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_FileRequest_t;
struct Cstager_Request_t;
struct Cstager_StageRmRequest_t;
struct Cstager_SubRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StageRmRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StageRmRequest_create(struct Cstager_StageRmRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StageRmRequest_delete(struct Cstager_StageRmRequest_t* obj);

/**
 * Cast into FileRequest
 */
struct Cstager_FileRequest_t* Cstager_StageRmRequest_getFileRequest(struct Cstager_StageRmRequest_t* obj);

/**
 * Dynamic cast from FileRequest
 */
struct Cstager_StageRmRequest_t* Cstager_StageRmRequest_fromFileRequest(struct Cstager_FileRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_StageRmRequest_getRequest(struct Cstager_StageRmRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_StageRmRequest_t* Cstager_StageRmRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_StageRmRequest_getIObject(struct Cstager_StageRmRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_StageRmRequest_t* Cstager_StageRmRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StageRmRequest_print(struct Cstager_StageRmRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StageRmRequest_TYPE(int* ret);

/************************************************/
/* Implementation of FileRequest abstract class */
/************************************************/

/**
 * Add a struct Cstager_SubRequest_t* object to the subRequests list
 */
int Cstager_StageRmRequest_addSubRequests(struct Cstager_StageRmRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Remove a struct Cstager_SubRequest_t* object from subRequests
 */
int Cstager_StageRmRequest_removeSubRequests(struct Cstager_StageRmRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Get the list of struct Cstager_SubRequest_t* objects held by subRequests
 */
int Cstager_StageRmRequest_subRequests(struct Cstager_StageRmRequest_t* instance, struct Cstager_SubRequest_t*** var, int* len);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StageRmRequest_flags(struct Cstager_StageRmRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StageRmRequest_setFlags(struct Cstager_StageRmRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageRmRequest_userName(struct Cstager_StageRmRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageRmRequest_setUserName(struct Cstager_StageRmRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageRmRequest_euid(struct Cstager_StageRmRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageRmRequest_setEuid(struct Cstager_StageRmRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageRmRequest_egid(struct Cstager_StageRmRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageRmRequest_setEgid(struct Cstager_StageRmRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageRmRequest_mask(struct Cstager_StageRmRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageRmRequest_setMask(struct Cstager_StageRmRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StageRmRequest_pid(struct Cstager_StageRmRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StageRmRequest_setPid(struct Cstager_StageRmRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StageRmRequest_machine(struct Cstager_StageRmRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StageRmRequest_setMachine(struct Cstager_StageRmRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StageRmRequest_svcClassName(struct Cstager_StageRmRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StageRmRequest_setSvcClassName(struct Cstager_StageRmRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageRmRequest_userTag(struct Cstager_StageRmRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageRmRequest_setUserTag(struct Cstager_StageRmRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageRmRequest_reqId(struct Cstager_StageRmRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageRmRequest_setReqId(struct Cstager_StageRmRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StageRmRequest_svcClass(struct Cstager_StageRmRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StageRmRequest_setSvcClass(struct Cstager_StageRmRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StageRmRequest_client(struct Cstager_StageRmRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StageRmRequest_setClient(struct Cstager_StageRmRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Sets the id of the object
 */
int Cstager_StageRmRequest_setId(struct Cstager_StageRmRequest_t* instance,
                                 u_signed64 id);

/**
 * gets the id of the object
 */
int Cstager_StageRmRequest_id(struct Cstager_StageRmRequest_t* instance,
                              u_signed64* ret);

/**
 * Gets the type of the object
 */
int Cstager_StageRmRequest_type(struct Cstager_StageRmRequest_t* instance,
                                int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_StageRmRequest_clone(struct Cstager_StageRmRequest_t* instance,
                                 struct C_IObject_t* ret);

#endif // CASTOR_STAGER_STAGERMREQUEST_H

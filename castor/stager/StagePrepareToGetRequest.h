/******************************************************************************
 *                      castor/stager/StagePrepareToGetRequest.h
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

#ifndef CASTOR_STAGER_STAGEPREPARETOGETREQUEST_H
#define CASTOR_STAGER_STAGEPREPARETOGETREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_FileRequest_t;
struct Cstager_Request_t;
struct Cstager_StagePrepareToGetRequest_t;
struct Cstager_SubRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StagePrepareToGetRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StagePrepareToGetRequest_create(struct Cstager_StagePrepareToGetRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StagePrepareToGetRequest_delete(struct Cstager_StagePrepareToGetRequest_t* obj);

/**
 * Cast into FileRequest
 */
struct Cstager_FileRequest_t* Cstager_StagePrepareToGetRequest_getFileRequest(struct Cstager_StagePrepareToGetRequest_t* obj);

/**
 * Dynamic cast from FileRequest
 */
struct Cstager_StagePrepareToGetRequest_t* Cstager_StagePrepareToGetRequest_fromFileRequest(struct Cstager_FileRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_StagePrepareToGetRequest_getRequest(struct Cstager_StagePrepareToGetRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_StagePrepareToGetRequest_t* Cstager_StagePrepareToGetRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_StagePrepareToGetRequest_getIObject(struct Cstager_StagePrepareToGetRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_StagePrepareToGetRequest_t* Cstager_StagePrepareToGetRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StagePrepareToGetRequest_print(struct Cstager_StagePrepareToGetRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StagePrepareToGetRequest_TYPE(int* ret);

/************************************************/
/* Implementation of FileRequest abstract class */
/************************************************/

/**
 * Add a struct Cstager_SubRequest_t* object to the subRequests list
 */
int Cstager_StagePrepareToGetRequest_addSubRequests(struct Cstager_StagePrepareToGetRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Remove a struct Cstager_SubRequest_t* object from subRequests
 */
int Cstager_StagePrepareToGetRequest_removeSubRequests(struct Cstager_StagePrepareToGetRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Get the list of struct Cstager_SubRequest_t* objects held by subRequests
 */
int Cstager_StagePrepareToGetRequest_subRequests(struct Cstager_StagePrepareToGetRequest_t* instance, struct Cstager_SubRequest_t*** var, int* len);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StagePrepareToGetRequest_flags(struct Cstager_StagePrepareToGetRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StagePrepareToGetRequest_setFlags(struct Cstager_StagePrepareToGetRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StagePrepareToGetRequest_userName(struct Cstager_StagePrepareToGetRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StagePrepareToGetRequest_setUserName(struct Cstager_StagePrepareToGetRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StagePrepareToGetRequest_euid(struct Cstager_StagePrepareToGetRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StagePrepareToGetRequest_setEuid(struct Cstager_StagePrepareToGetRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StagePrepareToGetRequest_egid(struct Cstager_StagePrepareToGetRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StagePrepareToGetRequest_setEgid(struct Cstager_StagePrepareToGetRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StagePrepareToGetRequest_mask(struct Cstager_StagePrepareToGetRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StagePrepareToGetRequest_setMask(struct Cstager_StagePrepareToGetRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StagePrepareToGetRequest_pid(struct Cstager_StagePrepareToGetRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StagePrepareToGetRequest_setPid(struct Cstager_StagePrepareToGetRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StagePrepareToGetRequest_machine(struct Cstager_StagePrepareToGetRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StagePrepareToGetRequest_setMachine(struct Cstager_StagePrepareToGetRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StagePrepareToGetRequest_svcClassName(struct Cstager_StagePrepareToGetRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StagePrepareToGetRequest_setSvcClassName(struct Cstager_StagePrepareToGetRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StagePrepareToGetRequest_userTag(struct Cstager_StagePrepareToGetRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StagePrepareToGetRequest_setUserTag(struct Cstager_StagePrepareToGetRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StagePrepareToGetRequest_reqId(struct Cstager_StagePrepareToGetRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StagePrepareToGetRequest_setReqId(struct Cstager_StagePrepareToGetRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StagePrepareToGetRequest_svcClass(struct Cstager_StagePrepareToGetRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StagePrepareToGetRequest_setSvcClass(struct Cstager_StagePrepareToGetRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StagePrepareToGetRequest_client(struct Cstager_StagePrepareToGetRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StagePrepareToGetRequest_setClient(struct Cstager_StagePrepareToGetRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_StagePrepareToGetRequest_type(struct Cstager_StagePrepareToGetRequest_t* instance,
                                          int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_StagePrepareToGetRequest_clone(struct Cstager_StagePrepareToGetRequest_t* instance,
                                           struct C_IObject_t* ret);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_StagePrepareToGetRequest_id(struct Cstager_StagePrepareToGetRequest_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_StagePrepareToGetRequest_setId(struct Cstager_StagePrepareToGetRequest_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_STAGEPREPARETOGETREQUEST_H

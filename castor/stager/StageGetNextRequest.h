/******************************************************************************
 *                      castor/stager/StageGetNextRequest.h
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

#ifndef CASTOR_STAGER_STAGEGETNEXTREQUEST_H
#define CASTOR_STAGER_STAGEGETNEXTREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_FileRequest_t;
struct Cstager_ReqIdRequest_t;
struct Cstager_Request_t;
struct Cstager_StageGetNextRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StageGetNextRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StageGetNextRequest_create(struct Cstager_StageGetNextRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StageGetNextRequest_delete(struct Cstager_StageGetNextRequest_t* obj);

/**
 * Cast into ReqIdRequest
 */
struct Cstager_ReqIdRequest_t* Cstager_StageGetNextRequest_getReqIdRequest(struct Cstager_StageGetNextRequest_t* obj);

/**
 * Dynamic cast from ReqIdRequest
 */
struct Cstager_StageGetNextRequest_t* Cstager_StageGetNextRequest_fromReqIdRequest(struct Cstager_ReqIdRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_StageGetNextRequest_getRequest(struct Cstager_StageGetNextRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_StageGetNextRequest_t* Cstager_StageGetNextRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_StageGetNextRequest_getIObject(struct Cstager_StageGetNextRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_StageGetNextRequest_t* Cstager_StageGetNextRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StageGetNextRequest_print(struct Cstager_StageGetNextRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StageGetNextRequest_TYPE(int* ret);

/*************************************************/
/* Implementation of ReqIdRequest abstract class */
/*************************************************/

/**
 * Get the value of parent
 */
int Cstager_StageGetNextRequest_parent(struct Cstager_StageGetNextRequest_t* instance, struct Cstager_FileRequest_t** var);

/**
 * Set the value of parent
 */
int Cstager_StageGetNextRequest_setParent(struct Cstager_StageGetNextRequest_t* instance, struct Cstager_FileRequest_t* new_var);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StageGetNextRequest_flags(struct Cstager_StageGetNextRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StageGetNextRequest_setFlags(struct Cstager_StageGetNextRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageGetNextRequest_userName(struct Cstager_StageGetNextRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageGetNextRequest_setUserName(struct Cstager_StageGetNextRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageGetNextRequest_euid(struct Cstager_StageGetNextRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageGetNextRequest_setEuid(struct Cstager_StageGetNextRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageGetNextRequest_egid(struct Cstager_StageGetNextRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageGetNextRequest_setEgid(struct Cstager_StageGetNextRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageGetNextRequest_mask(struct Cstager_StageGetNextRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageGetNextRequest_setMask(struct Cstager_StageGetNextRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StageGetNextRequest_pid(struct Cstager_StageGetNextRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StageGetNextRequest_setPid(struct Cstager_StageGetNextRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StageGetNextRequest_machine(struct Cstager_StageGetNextRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StageGetNextRequest_setMachine(struct Cstager_StageGetNextRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StageGetNextRequest_svcClassName(struct Cstager_StageGetNextRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StageGetNextRequest_setSvcClassName(struct Cstager_StageGetNextRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageGetNextRequest_userTag(struct Cstager_StageGetNextRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageGetNextRequest_setUserTag(struct Cstager_StageGetNextRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageGetNextRequest_reqId(struct Cstager_StageGetNextRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageGetNextRequest_setReqId(struct Cstager_StageGetNextRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StageGetNextRequest_svcClass(struct Cstager_StageGetNextRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StageGetNextRequest_setSvcClass(struct Cstager_StageGetNextRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StageGetNextRequest_client(struct Cstager_StageGetNextRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StageGetNextRequest_setClient(struct Cstager_StageGetNextRequest_t* instance, struct C_IClient_t* new_var);

/********************************************/
/* Implementation of IObject abstract class */
/********************************************/

/**
 * Sets the id of the object
 */
int Cstager_StageGetNextRequest_setId(struct Cstager_StageGetNextRequest_t* instance,
                                      u_signed64 id);

/**
 * gets the id of the object
 */
int Cstager_StageGetNextRequest_id(struct Cstager_StageGetNextRequest_t* instance,
                                   u_signed64* ret);

/**
 * Gets the type of the object
 */
int Cstager_StageGetNextRequest_type(struct Cstager_StageGetNextRequest_t* instance,
                                     int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_StageGetNextRequest_clone(struct Cstager_StageGetNextRequest_t* instance,
                                      struct C_IObject_t* ret);

#endif // CASTOR_STAGER_STAGEGETNEXTREQUEST_H

/******************************************************************************
 *                      castor/stager/StageFindRequestRequest.h
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

#ifndef CASTOR_STAGER_STAGEFINDREQUESTREQUEST_H
#define CASTOR_STAGER_STAGEFINDREQUESTREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_QryRequest_t;
struct Cstager_QueryParameter_t;
struct Cstager_Request_t;
struct Cstager_StageFindRequestRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StageFindRequestRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StageFindRequestRequest_create(struct Cstager_StageFindRequestRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StageFindRequestRequest_delete(struct Cstager_StageFindRequestRequest_t* obj);

/**
 * Cast into QryRequest
 */
struct Cstager_QryRequest_t* Cstager_StageFindRequestRequest_getQryRequest(struct Cstager_StageFindRequestRequest_t* obj);

/**
 * Dynamic cast from QryRequest
 */
struct Cstager_StageFindRequestRequest_t* Cstager_StageFindRequestRequest_fromQryRequest(struct Cstager_QryRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_StageFindRequestRequest_getRequest(struct Cstager_StageFindRequestRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_StageFindRequestRequest_t* Cstager_StageFindRequestRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_StageFindRequestRequest_getIObject(struct Cstager_StageFindRequestRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_StageFindRequestRequest_t* Cstager_StageFindRequestRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StageFindRequestRequest_print(struct Cstager_StageFindRequestRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StageFindRequestRequest_TYPE(int* ret);

/***********************************************/
/* Implementation of QryRequest abstract class */
/***********************************************/

/**
 * Add a struct Cstager_QueryParameter_t* object to the parameters list
 */
int Cstager_StageFindRequestRequest_addParameters(struct Cstager_StageFindRequestRequest_t* instance, struct Cstager_QueryParameter_t* obj);

/**
 * Remove a struct Cstager_QueryParameter_t* object from parameters
 */
int Cstager_StageFindRequestRequest_removeParameters(struct Cstager_StageFindRequestRequest_t* instance, struct Cstager_QueryParameter_t* obj);

/**
 * Get the list of struct Cstager_QueryParameter_t* objects held by parameters. Note
 * that the caller is responsible for the deletion of the returned vector.
 */
int Cstager_StageFindRequestRequest_parameters(struct Cstager_StageFindRequestRequest_t* instance, struct Cstager_QueryParameter_t*** var, int* len);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StageFindRequestRequest_flags(struct Cstager_StageFindRequestRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StageFindRequestRequest_setFlags(struct Cstager_StageFindRequestRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageFindRequestRequest_userName(struct Cstager_StageFindRequestRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageFindRequestRequest_setUserName(struct Cstager_StageFindRequestRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageFindRequestRequest_euid(struct Cstager_StageFindRequestRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageFindRequestRequest_setEuid(struct Cstager_StageFindRequestRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageFindRequestRequest_egid(struct Cstager_StageFindRequestRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageFindRequestRequest_setEgid(struct Cstager_StageFindRequestRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageFindRequestRequest_mask(struct Cstager_StageFindRequestRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageFindRequestRequest_setMask(struct Cstager_StageFindRequestRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StageFindRequestRequest_pid(struct Cstager_StageFindRequestRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StageFindRequestRequest_setPid(struct Cstager_StageFindRequestRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StageFindRequestRequest_machine(struct Cstager_StageFindRequestRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StageFindRequestRequest_setMachine(struct Cstager_StageFindRequestRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StageFindRequestRequest_svcClassName(struct Cstager_StageFindRequestRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StageFindRequestRequest_setSvcClassName(struct Cstager_StageFindRequestRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageFindRequestRequest_userTag(struct Cstager_StageFindRequestRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageFindRequestRequest_setUserTag(struct Cstager_StageFindRequestRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageFindRequestRequest_reqId(struct Cstager_StageFindRequestRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageFindRequestRequest_setReqId(struct Cstager_StageFindRequestRequest_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_StageFindRequestRequest_creationTime(struct Cstager_StageFindRequestRequest_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_StageFindRequestRequest_setCreationTime(struct Cstager_StageFindRequestRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_StageFindRequestRequest_lastModificationTime(struct Cstager_StageFindRequestRequest_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_StageFindRequestRequest_setLastModificationTime(struct Cstager_StageFindRequestRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StageFindRequestRequest_svcClass(struct Cstager_StageFindRequestRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StageFindRequestRequest_setSvcClass(struct Cstager_StageFindRequestRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StageFindRequestRequest_client(struct Cstager_StageFindRequestRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StageFindRequestRequest_setClient(struct Cstager_StageFindRequestRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_StageFindRequestRequest_type(struct Cstager_StageFindRequestRequest_t* instance,
                                         int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_StageFindRequestRequest_clone(struct Cstager_StageFindRequestRequest_t* instance,
                                          struct C_IObject_t* ret);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_StageFindRequestRequest_id(struct Cstager_StageFindRequestRequest_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_StageFindRequestRequest_setId(struct Cstager_StageFindRequestRequest_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_STAGEFINDREQUESTREQUEST_H

/******************************************************************************
 *                      castor/stager/PutFailed.h
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

#ifndef CASTOR_STAGER_PUTFAILED_H
#define CASTOR_STAGER_PUTFAILED_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_PutFailed_t;
struct Cstager_Request_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class PutFailed
// Request to inform the stager that a Put Request failed
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_PutFailed_create(struct Cstager_PutFailed_t** obj);

/**
 * Empty Destructor
 */
int Cstager_PutFailed_delete(struct Cstager_PutFailed_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_PutFailed_getRequest(struct Cstager_PutFailed_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_PutFailed_t* Cstager_PutFailed_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_PutFailed_getIObject(struct Cstager_PutFailed_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_PutFailed_t* Cstager_PutFailed_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_PutFailed_print(struct Cstager_PutFailed_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_PutFailed_TYPE(int* ret);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_PutFailed_flags(struct Cstager_PutFailed_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_PutFailed_setFlags(struct Cstager_PutFailed_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_PutFailed_userName(struct Cstager_PutFailed_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_PutFailed_setUserName(struct Cstager_PutFailed_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_PutFailed_euid(struct Cstager_PutFailed_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_PutFailed_setEuid(struct Cstager_PutFailed_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_PutFailed_egid(struct Cstager_PutFailed_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_PutFailed_setEgid(struct Cstager_PutFailed_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_PutFailed_mask(struct Cstager_PutFailed_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_PutFailed_setMask(struct Cstager_PutFailed_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_PutFailed_pid(struct Cstager_PutFailed_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_PutFailed_setPid(struct Cstager_PutFailed_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_PutFailed_machine(struct Cstager_PutFailed_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_PutFailed_setMachine(struct Cstager_PutFailed_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_PutFailed_svcClassName(struct Cstager_PutFailed_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_PutFailed_setSvcClassName(struct Cstager_PutFailed_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_PutFailed_userTag(struct Cstager_PutFailed_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_PutFailed_setUserTag(struct Cstager_PutFailed_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_PutFailed_reqId(struct Cstager_PutFailed_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_PutFailed_setReqId(struct Cstager_PutFailed_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_PutFailed_creationTime(struct Cstager_PutFailed_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_PutFailed_setCreationTime(struct Cstager_PutFailed_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_PutFailed_lastModificationTime(struct Cstager_PutFailed_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_PutFailed_setLastModificationTime(struct Cstager_PutFailed_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_PutFailed_svcClass(struct Cstager_PutFailed_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_PutFailed_setSvcClass(struct Cstager_PutFailed_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_PutFailed_client(struct Cstager_PutFailed_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_PutFailed_setClient(struct Cstager_PutFailed_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_PutFailed_type(struct Cstager_PutFailed_t* instance,
                           int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_PutFailed_clone(struct Cstager_PutFailed_t* instance,
                            struct C_IObject_t* ret);

/**
 * Get the value of subReqId
 * The id of the SubRequest that failed
 */
int Cstager_PutFailed_subReqId(struct Cstager_PutFailed_t* instance, u_signed64* var);

/**
 * Set the value of subReqId
 * The id of the SubRequest that failed
 */
int Cstager_PutFailed_setSubReqId(struct Cstager_PutFailed_t* instance, u_signed64 new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_PutFailed_id(struct Cstager_PutFailed_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_PutFailed_setId(struct Cstager_PutFailed_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_PUTFAILED_H

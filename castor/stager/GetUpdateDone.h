/******************************************************************************
 *                      castor/stager/GetUpdateDone.h
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

#ifndef CASTOR_STAGER_GETUPDATEDONE_H
#define CASTOR_STAGER_GETUPDATEDONE_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_GetUpdateDone_t;
struct Cstager_Request_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class GetUpdateDone
// Request to inform the stager that a Get or an Update request (with no write) was
// successful.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_GetUpdateDone_create(struct Cstager_GetUpdateDone_t** obj);

/**
 * Empty Destructor
 */
int Cstager_GetUpdateDone_delete(struct Cstager_GetUpdateDone_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_GetUpdateDone_getRequest(struct Cstager_GetUpdateDone_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_GetUpdateDone_t* Cstager_GetUpdateDone_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_GetUpdateDone_getIObject(struct Cstager_GetUpdateDone_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_GetUpdateDone_t* Cstager_GetUpdateDone_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_GetUpdateDone_print(struct Cstager_GetUpdateDone_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_GetUpdateDone_TYPE(int* ret);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_GetUpdateDone_flags(struct Cstager_GetUpdateDone_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_GetUpdateDone_setFlags(struct Cstager_GetUpdateDone_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_GetUpdateDone_userName(struct Cstager_GetUpdateDone_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_GetUpdateDone_setUserName(struct Cstager_GetUpdateDone_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_GetUpdateDone_euid(struct Cstager_GetUpdateDone_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_GetUpdateDone_setEuid(struct Cstager_GetUpdateDone_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_GetUpdateDone_egid(struct Cstager_GetUpdateDone_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_GetUpdateDone_setEgid(struct Cstager_GetUpdateDone_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_GetUpdateDone_mask(struct Cstager_GetUpdateDone_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_GetUpdateDone_setMask(struct Cstager_GetUpdateDone_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_GetUpdateDone_pid(struct Cstager_GetUpdateDone_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_GetUpdateDone_setPid(struct Cstager_GetUpdateDone_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_GetUpdateDone_machine(struct Cstager_GetUpdateDone_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_GetUpdateDone_setMachine(struct Cstager_GetUpdateDone_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_GetUpdateDone_svcClassName(struct Cstager_GetUpdateDone_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_GetUpdateDone_setSvcClassName(struct Cstager_GetUpdateDone_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_GetUpdateDone_userTag(struct Cstager_GetUpdateDone_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_GetUpdateDone_setUserTag(struct Cstager_GetUpdateDone_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_GetUpdateDone_reqId(struct Cstager_GetUpdateDone_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_GetUpdateDone_setReqId(struct Cstager_GetUpdateDone_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_GetUpdateDone_creationTime(struct Cstager_GetUpdateDone_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_GetUpdateDone_setCreationTime(struct Cstager_GetUpdateDone_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_GetUpdateDone_lastModificationTime(struct Cstager_GetUpdateDone_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_GetUpdateDone_setLastModificationTime(struct Cstager_GetUpdateDone_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_GetUpdateDone_svcClass(struct Cstager_GetUpdateDone_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_GetUpdateDone_setSvcClass(struct Cstager_GetUpdateDone_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_GetUpdateDone_client(struct Cstager_GetUpdateDone_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_GetUpdateDone_setClient(struct Cstager_GetUpdateDone_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_GetUpdateDone_type(struct Cstager_GetUpdateDone_t* instance,
                               int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_GetUpdateDone_clone(struct Cstager_GetUpdateDone_t* instance,
                                struct C_IObject_t* ret);

/**
 * Get the value of subReqId
 * The id of the SubRequest that is over
 */
int Cstager_GetUpdateDone_subReqId(struct Cstager_GetUpdateDone_t* instance, u_signed64* var);

/**
 * Set the value of subReqId
 * The id of the SubRequest that is over
 */
int Cstager_GetUpdateDone_setSubReqId(struct Cstager_GetUpdateDone_t* instance, u_signed64 new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_GetUpdateDone_id(struct Cstager_GetUpdateDone_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_GetUpdateDone_setId(struct Cstager_GetUpdateDone_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_GETUPDATEDONE_H

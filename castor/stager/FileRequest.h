/******************************************************************************
 *                      castor/stager/FileRequest.h
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

#ifndef CASTOR_STAGER_FILEREQUEST_H
#define CASTOR_STAGER_FILEREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_FileRequest_t;
struct Cstager_Request_t;
struct Cstager_SubRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class FileRequest
// An abstract ancester for all file requests
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_FileRequest_create(struct Cstager_FileRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_FileRequest_delete(struct Cstager_FileRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_FileRequest_getRequest(struct Cstager_FileRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_FileRequest_t* Cstager_FileRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_FileRequest_getIObject(struct Cstager_FileRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_FileRequest_t* Cstager_FileRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_FileRequest_print(struct Cstager_FileRequest_t* instance);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_FileRequest_flags(struct Cstager_FileRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_FileRequest_setFlags(struct Cstager_FileRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_FileRequest_userName(struct Cstager_FileRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_FileRequest_setUserName(struct Cstager_FileRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_FileRequest_euid(struct Cstager_FileRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_FileRequest_setEuid(struct Cstager_FileRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_FileRequest_egid(struct Cstager_FileRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_FileRequest_setEgid(struct Cstager_FileRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_FileRequest_mask(struct Cstager_FileRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_FileRequest_setMask(struct Cstager_FileRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_FileRequest_pid(struct Cstager_FileRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_FileRequest_setPid(struct Cstager_FileRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_FileRequest_machine(struct Cstager_FileRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_FileRequest_setMachine(struct Cstager_FileRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_FileRequest_svcClassName(struct Cstager_FileRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_FileRequest_setSvcClassName(struct Cstager_FileRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_FileRequest_userTag(struct Cstager_FileRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_FileRequest_setUserTag(struct Cstager_FileRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_FileRequest_reqId(struct Cstager_FileRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_FileRequest_setReqId(struct Cstager_FileRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClass
 */
int Cstager_FileRequest_svcClass(struct Cstager_FileRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_FileRequest_setSvcClass(struct Cstager_FileRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_FileRequest_client(struct Cstager_FileRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_FileRequest_setClient(struct Cstager_FileRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Add a struct Cstager_SubRequest_t* object to the subRequests list
 */
int Cstager_FileRequest_addSubRequests(struct Cstager_FileRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Remove a struct Cstager_SubRequest_t* object from subRequests
 */
int Cstager_FileRequest_removeSubRequests(struct Cstager_FileRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Get the list of struct Cstager_SubRequest_t* objects held by subRequests
 */
int Cstager_FileRequest_subRequests(struct Cstager_FileRequest_t* instance, struct Cstager_SubRequest_t*** var, int* len);

#endif // CASTOR_STAGER_FILEREQUEST_H

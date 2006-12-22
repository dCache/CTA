/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/StartRequest.h
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

#ifndef CASTOR_STAGER_STARTREQUEST_H
#define CASTOR_STAGER_STARTREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_Request_t;
struct Cstager_StartRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StartRequest
// Abstract class for all Job start requests
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StartRequest_create(struct Cstager_StartRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StartRequest_delete(struct Cstager_StartRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_StartRequest_getRequest(struct Cstager_StartRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_StartRequest_t* Cstager_StartRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_StartRequest_getIObject(struct Cstager_StartRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_StartRequest_t* Cstager_StartRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StartRequest_print(struct Cstager_StartRequest_t* instance);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StartRequest_flags(struct Cstager_StartRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StartRequest_setFlags(struct Cstager_StartRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StartRequest_userName(struct Cstager_StartRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StartRequest_setUserName(struct Cstager_StartRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StartRequest_euid(struct Cstager_StartRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StartRequest_setEuid(struct Cstager_StartRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StartRequest_egid(struct Cstager_StartRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StartRequest_setEgid(struct Cstager_StartRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StartRequest_mask(struct Cstager_StartRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StartRequest_setMask(struct Cstager_StartRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StartRequest_pid(struct Cstager_StartRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StartRequest_setPid(struct Cstager_StartRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StartRequest_machine(struct Cstager_StartRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StartRequest_setMachine(struct Cstager_StartRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StartRequest_svcClassName(struct Cstager_StartRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StartRequest_setSvcClassName(struct Cstager_StartRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StartRequest_userTag(struct Cstager_StartRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StartRequest_setUserTag(struct Cstager_StartRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StartRequest_reqId(struct Cstager_StartRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StartRequest_setReqId(struct Cstager_StartRequest_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_StartRequest_creationTime(struct Cstager_StartRequest_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_StartRequest_setCreationTime(struct Cstager_StartRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_StartRequest_lastModificationTime(struct Cstager_StartRequest_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_StartRequest_setLastModificationTime(struct Cstager_StartRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StartRequest_svcClass(struct Cstager_StartRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StartRequest_setSvcClass(struct Cstager_StartRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StartRequest_client(struct Cstager_StartRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StartRequest_setClient(struct Cstager_StartRequest_t* instance, struct C_IClient_t* new_var);

/**
 * Get the value of subreqId
 * The id of the subRequest that should be scheduled
 */
int Cstager_StartRequest_subreqId(struct Cstager_StartRequest_t* instance, u_signed64* var);

/**
 * Set the value of subreqId
 * The id of the subRequest that should be scheduled
 */
int Cstager_StartRequest_setSubreqId(struct Cstager_StartRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of diskServer
 * The name of the diskserver on which the selected filesystem for the given
 * SubRequest resides
 */
int Cstager_StartRequest_diskServer(struct Cstager_StartRequest_t* instance, const char** var);

/**
 * Set the value of diskServer
 * The name of the diskserver on which the selected filesystem for the given
 * SubRequest resides
 */
int Cstager_StartRequest_setDiskServer(struct Cstager_StartRequest_t* instance, const char* new_var);

/**
 * Get the value of fileSystem
 * The mount point of the selected filesystem for the given SubRequest
 */
int Cstager_StartRequest_fileSystem(struct Cstager_StartRequest_t* instance, const char** var);

/**
 * Set the value of fileSystem
 * The mount point of the selected filesystem for the given SubRequest
 */
int Cstager_StartRequest_setFileSystem(struct Cstager_StartRequest_t* instance, const char* new_var);

#endif // CASTOR_STAGER_STARTREQUEST_H

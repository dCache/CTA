/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/PutStartRequest.h
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

#ifndef CASTOR_STAGER_PUTSTARTREQUEST_H
#define CASTOR_STAGER_PUTSTARTREQUEST_H

/* Include Files and Forward declarations for the C world */
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_PutStartRequest_t;
struct Cstager_Request_t;
struct Cstager_StartRequest_t;
struct Cstager_SvcClass_t;

/* --------------------------------------------------------------------------
  This defines a C interface to the following class
// class PutStartRequest
// Internal request used when a put job has just started. It creates the link
// between the FileSystem and the DiskCopy associated to the SubRequest. It also
// updates the DiskCopy statement and returns the Client object for the
// SubRequest.
// This request exists to avoid the jobs on the diskservers to handle a connection
// to the database.
  -------------------------------------------------------------------------- */

/**
 * Empty Constructor
 */
int Cstager_PutStartRequest_create(struct Cstager_PutStartRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_PutStartRequest_delete(struct Cstager_PutStartRequest_t* obj);

/**
 * Cast into StartRequest
 */
struct Cstager_StartRequest_t* Cstager_PutStartRequest_getStartRequest(struct Cstager_PutStartRequest_t* obj);

/**
 * Dynamic cast from StartRequest
 */
struct Cstager_PutStartRequest_t* Cstager_PutStartRequest_fromStartRequest(struct Cstager_StartRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_PutStartRequest_getRequest(struct Cstager_PutStartRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_PutStartRequest_t* Cstager_PutStartRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_PutStartRequest_getIObject(struct Cstager_PutStartRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_PutStartRequest_t* Cstager_PutStartRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_PutStartRequest_print(struct Cstager_PutStartRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_PutStartRequest_TYPE(int* ret);

/*************************************************/
/* Implementation of StartRequest abstract class */
/*************************************************/

/**
 * Get the value of subreqId
 * The id of the subRequest that should be scheduled
 */
int Cstager_PutStartRequest_subreqId(struct Cstager_PutStartRequest_t* instance, u_signed64* var);

/**
 * Set the value of subreqId
 * The id of the subRequest that should be scheduled
 */
int Cstager_PutStartRequest_setSubreqId(struct Cstager_PutStartRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of diskServer
 * The name of the diskserver on which the selected filesystem for the given
 * SubRequest resides
 */
int Cstager_PutStartRequest_diskServer(struct Cstager_PutStartRequest_t* instance, const char** var);

/**
 * Set the value of diskServer
 * The name of the diskserver on which the selected filesystem for the given
 * SubRequest resides
 */
int Cstager_PutStartRequest_setDiskServer(struct Cstager_PutStartRequest_t* instance, const char* new_var);

/**
 * Get the value of fileSystem
 * The mount point of the selected filesystem for the given SubRequest
 */
int Cstager_PutStartRequest_fileSystem(struct Cstager_PutStartRequest_t* instance, const char** var);

/**
 * Set the value of fileSystem
 * The mount point of the selected filesystem for the given SubRequest
 */
int Cstager_PutStartRequest_setFileSystem(struct Cstager_PutStartRequest_t* instance, const char* new_var);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_PutStartRequest_flags(struct Cstager_PutStartRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_PutStartRequest_setFlags(struct Cstager_PutStartRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_PutStartRequest_userName(struct Cstager_PutStartRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_PutStartRequest_setUserName(struct Cstager_PutStartRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_PutStartRequest_euid(struct Cstager_PutStartRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_PutStartRequest_setEuid(struct Cstager_PutStartRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_PutStartRequest_egid(struct Cstager_PutStartRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_PutStartRequest_setEgid(struct Cstager_PutStartRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_PutStartRequest_mask(struct Cstager_PutStartRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_PutStartRequest_setMask(struct Cstager_PutStartRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_PutStartRequest_pid(struct Cstager_PutStartRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_PutStartRequest_setPid(struct Cstager_PutStartRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_PutStartRequest_machine(struct Cstager_PutStartRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_PutStartRequest_setMachine(struct Cstager_PutStartRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_PutStartRequest_svcClassName(struct Cstager_PutStartRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_PutStartRequest_setSvcClassName(struct Cstager_PutStartRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_PutStartRequest_userTag(struct Cstager_PutStartRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_PutStartRequest_setUserTag(struct Cstager_PutStartRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_PutStartRequest_reqId(struct Cstager_PutStartRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_PutStartRequest_setReqId(struct Cstager_PutStartRequest_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_PutStartRequest_creationTime(struct Cstager_PutStartRequest_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_PutStartRequest_setCreationTime(struct Cstager_PutStartRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_PutStartRequest_lastModificationTime(struct Cstager_PutStartRequest_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_PutStartRequest_setLastModificationTime(struct Cstager_PutStartRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_PutStartRequest_svcClass(struct Cstager_PutStartRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_PutStartRequest_setSvcClass(struct Cstager_PutStartRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_PutStartRequest_client(struct Cstager_PutStartRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_PutStartRequest_setClient(struct Cstager_PutStartRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_PutStartRequest_type(struct Cstager_PutStartRequest_t* instance,
                                 int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_PutStartRequest_clone(struct Cstager_PutStartRequest_t* instance,
                                  struct C_IObject_t* ret);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_PutStartRequest_id(struct Cstager_PutStartRequest_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_PutStartRequest_setId(struct Cstager_PutStartRequest_t* instance, u_signed64 new_var);

#endif /* CASTOR_STAGER_PUTSTARTREQUEST_H */

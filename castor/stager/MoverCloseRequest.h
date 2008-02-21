/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/MoverCloseRequest.h
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

#ifndef CASTOR_STAGER_MOVERCLOSEREQUEST_H
#define CASTOR_STAGER_MOVERCLOSEREQUEST_H

/* Include Files and Forward declarations for the C world */
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_MoverCloseRequest_t;
struct Cstager_Request_t;
struct Cstager_SvcClass_t;

/* --------------------------------------------------------------------------
  This defines a C interface to the following class
// class MoverCloseRequest
// Internal request used when a file was closed in a mover after writing.
// This request exists to avoid the jobs on the diskservers to handle a connection
// to the database.
  -------------------------------------------------------------------------- */

/**
 * Empty Constructor
 */
int Cstager_MoverCloseRequest_create(struct Cstager_MoverCloseRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_MoverCloseRequest_delete(struct Cstager_MoverCloseRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_MoverCloseRequest_getRequest(struct Cstager_MoverCloseRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_MoverCloseRequest_t* Cstager_MoverCloseRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_MoverCloseRequest_getIObject(struct Cstager_MoverCloseRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_MoverCloseRequest_t* Cstager_MoverCloseRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_MoverCloseRequest_print(struct Cstager_MoverCloseRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_MoverCloseRequest_TYPE(int* ret);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_MoverCloseRequest_flags(struct Cstager_MoverCloseRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_MoverCloseRequest_setFlags(struct Cstager_MoverCloseRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_MoverCloseRequest_userName(struct Cstager_MoverCloseRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_MoverCloseRequest_setUserName(struct Cstager_MoverCloseRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_MoverCloseRequest_euid(struct Cstager_MoverCloseRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_MoverCloseRequest_setEuid(struct Cstager_MoverCloseRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_MoverCloseRequest_egid(struct Cstager_MoverCloseRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_MoverCloseRequest_setEgid(struct Cstager_MoverCloseRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_MoverCloseRequest_mask(struct Cstager_MoverCloseRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_MoverCloseRequest_setMask(struct Cstager_MoverCloseRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_MoverCloseRequest_pid(struct Cstager_MoverCloseRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_MoverCloseRequest_setPid(struct Cstager_MoverCloseRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_MoverCloseRequest_machine(struct Cstager_MoverCloseRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_MoverCloseRequest_setMachine(struct Cstager_MoverCloseRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_MoverCloseRequest_svcClassName(struct Cstager_MoverCloseRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_MoverCloseRequest_setSvcClassName(struct Cstager_MoverCloseRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_MoverCloseRequest_userTag(struct Cstager_MoverCloseRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_MoverCloseRequest_setUserTag(struct Cstager_MoverCloseRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_MoverCloseRequest_reqId(struct Cstager_MoverCloseRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_MoverCloseRequest_setReqId(struct Cstager_MoverCloseRequest_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_MoverCloseRequest_creationTime(struct Cstager_MoverCloseRequest_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_MoverCloseRequest_setCreationTime(struct Cstager_MoverCloseRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_MoverCloseRequest_lastModificationTime(struct Cstager_MoverCloseRequest_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_MoverCloseRequest_setLastModificationTime(struct Cstager_MoverCloseRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_MoverCloseRequest_svcClass(struct Cstager_MoverCloseRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_MoverCloseRequest_setSvcClass(struct Cstager_MoverCloseRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_MoverCloseRequest_client(struct Cstager_MoverCloseRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_MoverCloseRequest_setClient(struct Cstager_MoverCloseRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_MoverCloseRequest_type(struct Cstager_MoverCloseRequest_t* instance,
                                   int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_MoverCloseRequest_clone(struct Cstager_MoverCloseRequest_t* instance,
                                    struct C_IObject_t* ret);

/**
 * Get the value of subReqId
 * The id of the SubRequest for which the file closing took place
 */
int Cstager_MoverCloseRequest_subReqId(struct Cstager_MoverCloseRequest_t* instance, u_signed64* var);

/**
 * Set the value of subReqId
 * The id of the SubRequest for which the file closing took place
 */
int Cstager_MoverCloseRequest_setSubReqId(struct Cstager_MoverCloseRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of fileSize
 * The actual file of the size, once it was closed
 */
int Cstager_MoverCloseRequest_fileSize(struct Cstager_MoverCloseRequest_t* instance, u_signed64* var);

/**
 * Set the value of fileSize
 * The actual file of the size, once it was closed
 */
int Cstager_MoverCloseRequest_setFileSize(struct Cstager_MoverCloseRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of timeStamp
 */
int Cstager_MoverCloseRequest_timeStamp(struct Cstager_MoverCloseRequest_t* instance, u_signed64* var);

/**
 * Set the value of timeStamp
 */
int Cstager_MoverCloseRequest_setTimeStamp(struct Cstager_MoverCloseRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of fileId
 */
int Cstager_MoverCloseRequest_fileId(struct Cstager_MoverCloseRequest_t* instance, u_signed64* var);

/**
 * Set the value of fileId
 */
int Cstager_MoverCloseRequest_setFileId(struct Cstager_MoverCloseRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of nsHost
 */
int Cstager_MoverCloseRequest_nsHost(struct Cstager_MoverCloseRequest_t* instance, const char** var);

/**
 * Set the value of nsHost
 */
int Cstager_MoverCloseRequest_setNsHost(struct Cstager_MoverCloseRequest_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_MoverCloseRequest_id(struct Cstager_MoverCloseRequest_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_MoverCloseRequest_setId(struct Cstager_MoverCloseRequest_t* instance, u_signed64 new_var);

#endif /* CASTOR_STAGER_MOVERCLOSEREQUEST_H */

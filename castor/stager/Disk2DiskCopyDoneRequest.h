/******************************************************************************
 *                      castor/stager/Disk2DiskCopyDoneRequest.h
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

#ifndef CASTOR_STAGER_DISK2DISKCOPYDONEREQUEST_H
#define CASTOR_STAGER_DISK2DISKCOPYDONEREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_Disk2DiskCopyDoneRequest_t;
struct Cstager_Request_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class Disk2DiskCopyDoneRequest
// Internal request for updating the database after a successful Disk to Disk copy.
// This request is there to avoid the jobs on the diskservers to handle a connection
// to the database.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_Disk2DiskCopyDoneRequest_create(struct Cstager_Disk2DiskCopyDoneRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_Disk2DiskCopyDoneRequest_delete(struct Cstager_Disk2DiskCopyDoneRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_Disk2DiskCopyDoneRequest_getRequest(struct Cstager_Disk2DiskCopyDoneRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_Disk2DiskCopyDoneRequest_t* Cstager_Disk2DiskCopyDoneRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_Disk2DiskCopyDoneRequest_getIObject(struct Cstager_Disk2DiskCopyDoneRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_Disk2DiskCopyDoneRequest_t* Cstager_Disk2DiskCopyDoneRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_Disk2DiskCopyDoneRequest_print(struct Cstager_Disk2DiskCopyDoneRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_Disk2DiskCopyDoneRequest_TYPE(int* ret);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_Disk2DiskCopyDoneRequest_flags(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_Disk2DiskCopyDoneRequest_setFlags(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_Disk2DiskCopyDoneRequest_userName(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_Disk2DiskCopyDoneRequest_setUserName(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_Disk2DiskCopyDoneRequest_euid(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_Disk2DiskCopyDoneRequest_setEuid(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_Disk2DiskCopyDoneRequest_egid(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_Disk2DiskCopyDoneRequest_setEgid(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_Disk2DiskCopyDoneRequest_mask(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_Disk2DiskCopyDoneRequest_setMask(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_Disk2DiskCopyDoneRequest_pid(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_Disk2DiskCopyDoneRequest_setPid(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_Disk2DiskCopyDoneRequest_machine(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_Disk2DiskCopyDoneRequest_setMachine(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_Disk2DiskCopyDoneRequest_svcClassName(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_Disk2DiskCopyDoneRequest_setSvcClassName(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_Disk2DiskCopyDoneRequest_userTag(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_Disk2DiskCopyDoneRequest_setUserTag(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_Disk2DiskCopyDoneRequest_reqId(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_Disk2DiskCopyDoneRequest_setReqId(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClass
 */
int Cstager_Disk2DiskCopyDoneRequest_svcClass(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_Disk2DiskCopyDoneRequest_setSvcClass(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_Disk2DiskCopyDoneRequest_client(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_Disk2DiskCopyDoneRequest_setClient(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_Disk2DiskCopyDoneRequest_type(struct Cstager_Disk2DiskCopyDoneRequest_t* instance,
                                          int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_Disk2DiskCopyDoneRequest_clone(struct Cstager_Disk2DiskCopyDoneRequest_t* instance,
                                           struct C_IObject_t* ret);

/**
 * Get the value of diskCopyId
 * The DiskCopy created.
 */
int Cstager_Disk2DiskCopyDoneRequest_diskCopyId(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, u_signed64* var);

/**
 * Set the value of diskCopyId
 * The DiskCopy created.
 */
int Cstager_Disk2DiskCopyDoneRequest_setDiskCopyId(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of status
 * The new status of the diskCopy created
 */
int Cstager_Disk2DiskCopyDoneRequest_status(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, int* var);

/**
 * Set the value of status
 * The new status of the diskCopy created
 */
int Cstager_Disk2DiskCopyDoneRequest_setStatus(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, int new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_Disk2DiskCopyDoneRequest_id(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_Disk2DiskCopyDoneRequest_setId(struct Cstager_Disk2DiskCopyDoneRequest_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_DISK2DISKCOPYDONEREQUEST_H

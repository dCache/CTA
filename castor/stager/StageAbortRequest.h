/******************************************************************************
 *                      castor/stager/StageAbortRequest.h
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

#ifndef CASTOR_STAGER_STAGEABORTREQUEST_H
#define CASTOR_STAGER_STAGEABORTREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct Cstager_FileRequest_t;
struct Cstager_StageAbortRequest_t;
struct Cstager_SubRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StageAbortRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StageAbortRequest_create(struct Cstager_StageAbortRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StageAbortRequest_delete(struct Cstager_StageAbortRequest_t* obj);

/**
 * Cast into FileRequest
 */
struct Cstager_FileRequest_t* Cstager_StageAbortRequest_getFileRequest(struct Cstager_StageAbortRequest_t* obj);

/**
 * Dynamic cast from FileRequest
 */
struct Cstager_StageAbortRequest_t* Cstager_StageAbortRequest_fromFileRequest(struct Cstager_FileRequest_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StageAbortRequest_print(struct Cstager_StageAbortRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StageAbortRequest_TYPE(int* ret);

/************************************************/
/* Implementation of FileRequest abstract class */
/************************************************/

/**
 * Add a struct Cstager_SubRequest_t* object to the subRequests list
 */
int Cstager_StageAbortRequest_addSubRequests(struct Cstager_StageAbortRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Remove a struct Cstager_SubRequest_t* object from subRequests
 */
int Cstager_StageAbortRequest_removeSubRequests(struct Cstager_StageAbortRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Get the list of struct Cstager_SubRequest_t* objects held by subRequests
 */
int Cstager_StageAbortRequest_subRequests(struct Cstager_StageAbortRequest_t* instance, struct Cstager_SubRequest_t*** var, int* len);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StageAbortRequest_flags(struct Cstager_StageAbortRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StageAbortRequest_setFlags(struct Cstager_StageAbortRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageAbortRequest_userName(struct Cstager_StageAbortRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageAbortRequest_setUserName(struct Cstager_StageAbortRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageAbortRequest_euid(struct Cstager_StageAbortRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageAbortRequest_setEuid(struct Cstager_StageAbortRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageAbortRequest_egid(struct Cstager_StageAbortRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageAbortRequest_setEgid(struct Cstager_StageAbortRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageAbortRequest_mask(struct Cstager_StageAbortRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageAbortRequest_setMask(struct Cstager_StageAbortRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StageAbortRequest_pid(struct Cstager_StageAbortRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StageAbortRequest_setPid(struct Cstager_StageAbortRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StageAbortRequest_machine(struct Cstager_StageAbortRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StageAbortRequest_setMachine(struct Cstager_StageAbortRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StageAbortRequest_svcClassName(struct Cstager_StageAbortRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StageAbortRequest_setSvcClassName(struct Cstager_StageAbortRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageAbortRequest_userTag(struct Cstager_StageAbortRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageAbortRequest_setUserTag(struct Cstager_StageAbortRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageAbortRequest_reqId(struct Cstager_StageAbortRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageAbortRequest_setReqId(struct Cstager_StageAbortRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StageAbortRequest_svcClass(struct Cstager_StageAbortRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StageAbortRequest_setSvcClass(struct Cstager_StageAbortRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StageAbortRequest_client(struct Cstager_StageAbortRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StageAbortRequest_setClient(struct Cstager_StageAbortRequest_t* instance, struct C_IClient_t* new_var);

/********************************************/
/* Implementation of IObject abstract class */
/********************************************/

/**
 * Sets the id of the object
 */
int Cstager_StageAbortRequest_setId(struct Cstager_StageAbortRequest_t* instance,
                                    u_signed64 id);

/**
 * gets the id of the object
 */
int Cstager_StageAbortRequest_id(struct Cstager_StageAbortRequest_t* instance,
                                 u_signed64* ret);

/**
 * Gets the type of the object
 */
int Cstager_StageAbortRequest_type(struct Cstager_StageAbortRequest_t* instance,
                                   int* ret);

#endif // CASTOR_STAGER_STAGEABORTREQUEST_H

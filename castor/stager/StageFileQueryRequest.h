/******************************************************************************
 *                      castor/stager/StageFileQueryRequest.h
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

#ifndef CASTOR_STAGER_STAGEFILEQUERYREQUEST_H
#define CASTOR_STAGER_STAGEFILEQUERYREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct Cstager_QryRequest_t;
struct Cstager_StageFileQueryRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StageFileQueryRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StageFileQueryRequest_create(struct Cstager_StageFileQueryRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StageFileQueryRequest_delete(struct Cstager_StageFileQueryRequest_t* obj);

/**
 * Cast into QryRequest
 */
struct Cstager_QryRequest_t* Cstager_StageFileQueryRequest_getQryRequest(struct Cstager_StageFileQueryRequest_t* obj);

/**
 * Dynamic cast from QryRequest
 */
struct Cstager_StageFileQueryRequest_t* Cstager_StageFileQueryRequest_fromQryRequest(struct Cstager_QryRequest_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StageFileQueryRequest_print(struct Cstager_StageFileQueryRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StageFileQueryRequest_TYPE(int* ret);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StageFileQueryRequest_flags(struct Cstager_StageFileQueryRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StageFileQueryRequest_setFlags(struct Cstager_StageFileQueryRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageFileQueryRequest_userName(struct Cstager_StageFileQueryRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StageFileQueryRequest_setUserName(struct Cstager_StageFileQueryRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageFileQueryRequest_euid(struct Cstager_StageFileQueryRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StageFileQueryRequest_setEuid(struct Cstager_StageFileQueryRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageFileQueryRequest_egid(struct Cstager_StageFileQueryRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StageFileQueryRequest_setEgid(struct Cstager_StageFileQueryRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageFileQueryRequest_mask(struct Cstager_StageFileQueryRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StageFileQueryRequest_setMask(struct Cstager_StageFileQueryRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StageFileQueryRequest_pid(struct Cstager_StageFileQueryRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StageFileQueryRequest_setPid(struct Cstager_StageFileQueryRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StageFileQueryRequest_machine(struct Cstager_StageFileQueryRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StageFileQueryRequest_setMachine(struct Cstager_StageFileQueryRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StageFileQueryRequest_svcClassName(struct Cstager_StageFileQueryRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StageFileQueryRequest_setSvcClassName(struct Cstager_StageFileQueryRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageFileQueryRequest_userTag(struct Cstager_StageFileQueryRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_StageFileQueryRequest_setUserTag(struct Cstager_StageFileQueryRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageFileQueryRequest_reqId(struct Cstager_StageFileQueryRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StageFileQueryRequest_setReqId(struct Cstager_StageFileQueryRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StageFileQueryRequest_svcClass(struct Cstager_StageFileQueryRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StageFileQueryRequest_setSvcClass(struct Cstager_StageFileQueryRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StageFileQueryRequest_client(struct Cstager_StageFileQueryRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StageFileQueryRequest_setClient(struct Cstager_StageFileQueryRequest_t* instance, struct C_IClient_t* new_var);

/********************************************/
/* Implementation of IObject abstract class */
/********************************************/

/**
 * Sets the id of the object
 */
int Cstager_StageFileQueryRequest_setId(struct Cstager_StageFileQueryRequest_t* instance,
                                        u_signed64 id);

/**
 * gets the id of the object
 */
int Cstager_StageFileQueryRequest_id(struct Cstager_StageFileQueryRequest_t* instance,
                                     u_signed64* ret);

/**
 * Gets the type of the object
 */
int Cstager_StageFileQueryRequest_type(struct Cstager_StageFileQueryRequest_t* instance,
                                       int* ret);

#endif // CASTOR_STAGER_STAGEFILEQUERYREQUEST_H

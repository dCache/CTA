/******************************************************************************
 *                      castor/stager/QryRequest.h
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

#ifndef CASTOR_STAGER_QRYREQUEST_H
#define CASTOR_STAGER_QRYREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_QryRequest_t;
struct Cstager_Request_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class QryRequest
// An abstract ancester for all query requests
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_QryRequest_create(struct Cstager_QryRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_QryRequest_delete(struct Cstager_QryRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_QryRequest_getRequest(struct Cstager_QryRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_QryRequest_t* Cstager_QryRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_QryRequest_getIObject(struct Cstager_QryRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_QryRequest_t* Cstager_QryRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_QryRequest_print(struct Cstager_QryRequest_t* instance);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_QryRequest_flags(struct Cstager_QryRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_QryRequest_setFlags(struct Cstager_QryRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_QryRequest_userName(struct Cstager_QryRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_QryRequest_setUserName(struct Cstager_QryRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_QryRequest_euid(struct Cstager_QryRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_QryRequest_setEuid(struct Cstager_QryRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_QryRequest_egid(struct Cstager_QryRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_QryRequest_setEgid(struct Cstager_QryRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_QryRequest_mask(struct Cstager_QryRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_QryRequest_setMask(struct Cstager_QryRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_QryRequest_pid(struct Cstager_QryRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_QryRequest_setPid(struct Cstager_QryRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_QryRequest_machine(struct Cstager_QryRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_QryRequest_setMachine(struct Cstager_QryRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_QryRequest_svcClassName(struct Cstager_QryRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_QryRequest_setSvcClassName(struct Cstager_QryRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_QryRequest_userTag(struct Cstager_QryRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_QryRequest_setUserTag(struct Cstager_QryRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_QryRequest_reqId(struct Cstager_QryRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_QryRequest_setReqId(struct Cstager_QryRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClass
 */
int Cstager_QryRequest_svcClass(struct Cstager_QryRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_QryRequest_setSvcClass(struct Cstager_QryRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_QryRequest_client(struct Cstager_QryRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_QryRequest_setClient(struct Cstager_QryRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

#endif // CASTOR_STAGER_QRYREQUEST_H

/******************************************************************************
 *                      castor/rh/RequestQueryResponse.h
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

#ifndef CASTOR_RH_REQUESTQUERYRESPONSE_H
#define CASTOR_RH_REQUESTQUERYRESPONSE_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Crh_RequestQueryResponse_t;
struct Crh_Response_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class RequestQueryResponse
// Response to the RequestQueryRequest
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Crh_RequestQueryResponse_create(struct Crh_RequestQueryResponse_t** obj);

/**
 * Empty Destructor
 */
int Crh_RequestQueryResponse_delete(struct Crh_RequestQueryResponse_t* obj);

/**
 * Cast into Response
 */
struct Crh_Response_t* Crh_RequestQueryResponse_getResponse(struct Crh_RequestQueryResponse_t* obj);

/**
 * Dynamic cast from Response
 */
struct Crh_RequestQueryResponse_t* Crh_RequestQueryResponse_fromResponse(struct Crh_Response_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Crh_RequestQueryResponse_getIObject(struct Crh_RequestQueryResponse_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Crh_RequestQueryResponse_t* Crh_RequestQueryResponse_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Crh_RequestQueryResponse_print(struct Crh_RequestQueryResponse_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Crh_RequestQueryResponse_TYPE(int* ret);

/********************************************/
/* Implementation of IObject abstract class */
/********************************************/

/**
 * Sets the id of the object
 */
int Crh_RequestQueryResponse_setId(struct Crh_RequestQueryResponse_t* instance,
                                   u_signed64 id);

/**
 * gets the id of the object
 */
int Crh_RequestQueryResponse_id(struct Crh_RequestQueryResponse_t* instance,
                                u_signed64* ret);

/**
 * Gets the type of the object
 */
int Crh_RequestQueryResponse_type(struct Crh_RequestQueryResponse_t* instance,
                                  int* ret);

/**
 * Get the value of reqId
 * The Cuuid identifying the request given as a human readable string
 */
int Crh_RequestQueryResponse_reqId(struct Crh_RequestQueryResponse_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the request given as a human readable string
 */
int Crh_RequestQueryResponse_setReqId(struct Crh_RequestQueryResponse_t* instance, const char* new_var);

/**
 * Get the value of status
 * The status of the request
 */
int Crh_RequestQueryResponse_status(struct Crh_RequestQueryResponse_t* instance, unsigned int* var);

/**
 * Set the value of status
 * The status of the request
 */
int Crh_RequestQueryResponse_setStatus(struct Crh_RequestQueryResponse_t* instance, unsigned int new_var);

/**
 * Get the value of creationTime
 * Time of the request creation
 */
int Crh_RequestQueryResponse_creationTime(struct Crh_RequestQueryResponse_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time of the request creation
 */
int Crh_RequestQueryResponse_setCreationTime(struct Crh_RequestQueryResponse_t* instance, u_signed64 new_var);

/**
 * Get the value of modificationTime
 * Time of the last modification of the request
 */
int Crh_RequestQueryResponse_modificationTime(struct Crh_RequestQueryResponse_t* instance, u_signed64* var);

/**
 * Set the value of modificationTime
 * Time of the last modification of the request
 */
int Crh_RequestQueryResponse_setModificationTime(struct Crh_RequestQueryResponse_t* instance, u_signed64 new_var);

#endif // CASTOR_RH_REQUESTQUERYRESPONSE_H

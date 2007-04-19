/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/query/DiskPoolQueryResponse.h
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

#ifndef CASTOR_QUERY_DISKPOOLQUERYRESPONSE_H
#define CASTOR_QUERY_DISKPOOLQUERYRESPONSE_H

/* Include Files and Forward declarations for the C world */
#include "osdep.h"
struct C_IObject_t;
struct Cquery_DiskPoolQueryResponse_t;
struct Cquery_DiskServerDescription_t;
struct Crh_Response_t;

/* --------------------------------------------------------------------------
  This defines a C interface to the following class
// class DiskPoolQueryResponse
// Response to a DiskPoolQuery
  -------------------------------------------------------------------------- */

/**
 * Empty Constructor
 */
int Cquery_DiskPoolQueryResponse_create(struct Cquery_DiskPoolQueryResponse_t** obj);

/**
 * Empty Destructor
 */
int Cquery_DiskPoolQueryResponse_delete(struct Cquery_DiskPoolQueryResponse_t* obj);

/**
 * Cast into Response
 */
struct Crh_Response_t* Cquery_DiskPoolQueryResponse_getResponse(struct Cquery_DiskPoolQueryResponse_t* obj);

/**
 * Dynamic cast from Response
 */
struct Cquery_DiskPoolQueryResponse_t* Cquery_DiskPoolQueryResponse_fromResponse(struct Crh_Response_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cquery_DiskPoolQueryResponse_getIObject(struct Cquery_DiskPoolQueryResponse_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cquery_DiskPoolQueryResponse_t* Cquery_DiskPoolQueryResponse_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cquery_DiskPoolQueryResponse_print(struct Cquery_DiskPoolQueryResponse_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cquery_DiskPoolQueryResponse_TYPE(int* ret);

/*********************************************/
/* Implementation of Response abstract class */
/*********************************************/

/**
 * Get the value of errorCode
 * The error code in case of error
 */
int Cquery_DiskPoolQueryResponse_errorCode(struct Cquery_DiskPoolQueryResponse_t* instance, unsigned int* var);

/**
 * Set the value of errorCode
 * The error code in case of error
 */
int Cquery_DiskPoolQueryResponse_setErrorCode(struct Cquery_DiskPoolQueryResponse_t* instance, unsigned int new_var);

/**
 * Get the value of errorMessage
 * The error message in case of error
 */
int Cquery_DiskPoolQueryResponse_errorMessage(struct Cquery_DiskPoolQueryResponse_t* instance, const char** var);

/**
 * Set the value of errorMessage
 * The error message in case of error
 */
int Cquery_DiskPoolQueryResponse_setErrorMessage(struct Cquery_DiskPoolQueryResponse_t* instance, const char* new_var);

/**
 * Get the value of reqAssociated
 */
int Cquery_DiskPoolQueryResponse_reqAssociated(struct Cquery_DiskPoolQueryResponse_t* instance, const char** var);

/**
 * Set the value of reqAssociated
 */
int Cquery_DiskPoolQueryResponse_setReqAssociated(struct Cquery_DiskPoolQueryResponse_t* instance, const char* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cquery_DiskPoolQueryResponse_type(struct Cquery_DiskPoolQueryResponse_t* instance,
                                      int* ret);

/**
 * virtual method to clone any object
 */
int Cquery_DiskPoolQueryResponse_clone(struct Cquery_DiskPoolQueryResponse_t* instance,
                                       struct C_IObject_t* ret);

/**
 * Get the value of diskPoolName
 * Name of the diskpool
 */
int Cquery_DiskPoolQueryResponse_diskPoolName(struct Cquery_DiskPoolQueryResponse_t* instance, const char** var);

/**
 * Set the value of diskPoolName
 * Name of the diskpool
 */
int Cquery_DiskPoolQueryResponse_setDiskPoolName(struct Cquery_DiskPoolQueryResponse_t* instance, const char* new_var);

/**
 * Get the value of freeSpace
 * The total free space in the diskPool
 */
int Cquery_DiskPoolQueryResponse_freeSpace(struct Cquery_DiskPoolQueryResponse_t* instance, u_signed64* var);

/**
 * Set the value of freeSpace
 * The total free space in the diskPool
 */
int Cquery_DiskPoolQueryResponse_setFreeSpace(struct Cquery_DiskPoolQueryResponse_t* instance, u_signed64 new_var);

/**
 * Get the value of totalSpace
 * The total space provided by the diskPool
 */
int Cquery_DiskPoolQueryResponse_totalSpace(struct Cquery_DiskPoolQueryResponse_t* instance, u_signed64* var);

/**
 * Set the value of totalSpace
 * The total space provided by the diskPool
 */
int Cquery_DiskPoolQueryResponse_setTotalSpace(struct Cquery_DiskPoolQueryResponse_t* instance, u_signed64 new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cquery_DiskPoolQueryResponse_id(struct Cquery_DiskPoolQueryResponse_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cquery_DiskPoolQueryResponse_setId(struct Cquery_DiskPoolQueryResponse_t* instance, u_signed64 new_var);

/**
 * Add a struct Cquery_DiskServerDescription_t* object to the diskServers list
 */
int Cquery_DiskPoolQueryResponse_addDiskServers(struct Cquery_DiskPoolQueryResponse_t* instance, struct Cquery_DiskServerDescription_t* obj);

/**
 * Remove a struct Cquery_DiskServerDescription_t* object from diskServers
 */
int Cquery_DiskPoolQueryResponse_removeDiskServers(struct Cquery_DiskPoolQueryResponse_t* instance, struct Cquery_DiskServerDescription_t* obj);

/**
 * Get the list of struct Cquery_DiskServerDescription_t* objects held by
 * diskServers. Note that the caller is responsible for the deletion of the
 * returned vector.
 */
int Cquery_DiskPoolQueryResponse_diskServers(struct Cquery_DiskPoolQueryResponse_t* instance, struct Cquery_DiskServerDescription_t*** var, int* len);

#endif /* CASTOR_QUERY_DISKPOOLQUERYRESPONSE_H */

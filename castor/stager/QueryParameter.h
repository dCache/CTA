/******************************************************************************
 *                      castor/stager/QueryParameter.h
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

#ifndef CASTOR_STAGER_QUERYPARAMETER_H
#define CASTOR_STAGER_QUERYPARAMETER_H

// Include Files and Forward declarations for the C world
#include "castor/stager/RequestQueryType.h"
#include "osdep.h"
struct C_IObject_t;
struct Cstager_QryRequest_t;
struct Cstager_QueryParameter_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class QueryParameter
// A parameter of a query request. Depending on its type, its value can contain a
// fileName, a requestId, a fileId, ...
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_QueryParameter_create(struct Cstager_QueryParameter_t** obj);

/**
 * Empty Destructor
 */
int Cstager_QueryParameter_delete(struct Cstager_QueryParameter_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_QueryParameter_getIObject(struct Cstager_QueryParameter_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_QueryParameter_t* Cstager_QueryParameter_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_QueryParameter_print(struct Cstager_QueryParameter_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_QueryParameter_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_QueryParameter_type(struct Cstager_QueryParameter_t* instance,
                                int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_QueryParameter_clone(struct Cstager_QueryParameter_t* instance,
                                 struct C_IObject_t* ret);

/**
 * Get the value of value
 * Value of this parameter. The meaning depends on the type.
 */
int Cstager_QueryParameter_value(struct Cstager_QueryParameter_t* instance, const char** var);

/**
 * Set the value of value
 * Value of this parameter. The meaning depends on the type.
 */
int Cstager_QueryParameter_setValue(struct Cstager_QueryParameter_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_QueryParameter_id(struct Cstager_QueryParameter_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_QueryParameter_setId(struct Cstager_QueryParameter_t* instance, u_signed64 new_var);

/**
 * Get the value of query
 */
int Cstager_QueryParameter_query(struct Cstager_QueryParameter_t* instance, struct Cstager_QryRequest_t** var);

/**
 * Set the value of query
 */
int Cstager_QueryParameter_setQuery(struct Cstager_QueryParameter_t* instance, struct Cstager_QryRequest_t* new_var);

/**
 * Get the value of queryType
 */
int Cstager_QueryParameter_queryType(struct Cstager_QueryParameter_t* instance, enum Cstager_RequestQueryType_t* var);

/**
 * Set the value of queryType
 */
int Cstager_QueryParameter_setQueryType(struct Cstager_QueryParameter_t* instance, enum Cstager_RequestQueryType_t new_var);

#endif // CASTOR_STAGER_QUERYPARAMETER_H

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/infoPolicy/PolicyObj.h
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

#ifndef CASTOR_INFOPOLICY_POLICYOBJ_H
#define CASTOR_INFOPOLICY_POLICYOBJ_H

/* Include Files and Forward declarations for the C world */
#include "osdep.h"
struct C_IObject_t;
struct CinfoPolicy_CnsInfoPolicy_t;
struct CinfoPolicy_DbInfoPolicy_t;
struct CinfoPolicy_PolicyObj_t;

/* --------------------------------------------------------------------------
  This defines a C interface to the following class
// class PolicyObj
  -------------------------------------------------------------------------- */

/**
 * Empty Constructor
 */
int CinfoPolicy_PolicyObj_create(struct CinfoPolicy_PolicyObj_t** obj);

/**
 * Empty Destructor
 */
int CinfoPolicy_PolicyObj_delete(struct CinfoPolicy_PolicyObj_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* CinfoPolicy_PolicyObj_getIObject(struct CinfoPolicy_PolicyObj_t* obj);

/**
 * Dynamic cast from IObject
 */
struct CinfoPolicy_PolicyObj_t* CinfoPolicy_PolicyObj_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int CinfoPolicy_PolicyObj_print(struct CinfoPolicy_PolicyObj_t* instance);

/**
 * Gets the type of this kind of objects
 */
int CinfoPolicy_PolicyObj_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int CinfoPolicy_PolicyObj_type(struct CinfoPolicy_PolicyObj_t* instance,
                               int* ret);

/**
 * virtual method to clone any object
 */
int CinfoPolicy_PolicyObj_clone(struct CinfoPolicy_PolicyObj_t* instance,
                                struct C_IObject_t* ret);

/**
 * Get the value of policyName
 */
int CinfoPolicy_PolicyObj_policyName(struct CinfoPolicy_PolicyObj_t* instance, const char** var);

/**
 * Set the value of policyName
 */
int CinfoPolicy_PolicyObj_setPolicyName(struct CinfoPolicy_PolicyObj_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int CinfoPolicy_PolicyObj_svcClassName(struct CinfoPolicy_PolicyObj_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int CinfoPolicy_PolicyObj_setSvcClassName(struct CinfoPolicy_PolicyObj_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int CinfoPolicy_PolicyObj_id(struct CinfoPolicy_PolicyObj_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int CinfoPolicy_PolicyObj_setId(struct CinfoPolicy_PolicyObj_t* instance, u_signed64 new_var);

/**
 * Add a struct CinfoPolicy_DbInfoPolicy_t* object to the dbInfoPolicy list
 */
int CinfoPolicy_PolicyObj_addDbInfoPolicy(struct CinfoPolicy_PolicyObj_t* instance, struct CinfoPolicy_DbInfoPolicy_t* obj);

/**
 * Remove a struct CinfoPolicy_DbInfoPolicy_t* object from dbInfoPolicy
 */
int CinfoPolicy_PolicyObj_removeDbInfoPolicy(struct CinfoPolicy_PolicyObj_t* instance, struct CinfoPolicy_DbInfoPolicy_t* obj);

/**
 * Get the list of struct CinfoPolicy_DbInfoPolicy_t* objects held by dbInfoPolicy.
 * Note that the caller is responsible for the deletion of the returned vector.
 */
int CinfoPolicy_PolicyObj_dbInfoPolicy(struct CinfoPolicy_PolicyObj_t* instance, struct CinfoPolicy_DbInfoPolicy_t*** var, int* len);

/**
 * Add a struct CinfoPolicy_CnsInfoPolicy_t* object to the cnsInfoPolicy list
 */
int CinfoPolicy_PolicyObj_addCnsInfoPolicy(struct CinfoPolicy_PolicyObj_t* instance, struct CinfoPolicy_CnsInfoPolicy_t* obj);

/**
 * Remove a struct CinfoPolicy_CnsInfoPolicy_t* object from cnsInfoPolicy
 */
int CinfoPolicy_PolicyObj_removeCnsInfoPolicy(struct CinfoPolicy_PolicyObj_t* instance, struct CinfoPolicy_CnsInfoPolicy_t* obj);

/**
 * Get the list of struct CinfoPolicy_CnsInfoPolicy_t* objects held by
 * cnsInfoPolicy. Note that the caller is responsible for the deletion of the
 * returned vector.
 */
int CinfoPolicy_PolicyObj_cnsInfoPolicy(struct CinfoPolicy_PolicyObj_t* instance, struct CinfoPolicy_CnsInfoPolicy_t*** var, int* len);

#endif /* CASTOR_INFOPOLICY_POLICYOBJ_H */

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/ClientIdentification.h
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

#ifndef CASTOR_STAGER_CLIENTIDENTIFICATION_H
#define CASTOR_STAGER_CLIENTIDENTIFICATION_H

// Include Files and Forward declarations for the C world
struct C_IObject_t;
struct C_int_t;
struct C_u_signed64_t;
struct Cstager_ClientIdentification_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class ClientIdentification
// An object to store all client related informations. In future this should be
// replaced by an abstract class to support also other identification methods, like
// Kerberos. 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_ClientIdentification_create(struct Cstager_ClientIdentification_t** obj);

/**
 * Empty Destructor
 */
int Cstager_ClientIdentification_delete(struct Cstager_ClientIdentification_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_ClientIdentification_getIObject(struct Cstager_ClientIdentification_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_ClientIdentification_t* Cstager_ClientIdentification_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_ClientIdentification_print(struct Cstager_ClientIdentification_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_ClientIdentification_TYPE(struct C_int_t* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_ClientIdentification_type(struct Cstager_ClientIdentification_t* instance,
                                      struct C_int_t* ret);

/**
 * virtual method to clone any object
 */
int Cstager_ClientIdentification_clone(struct Cstager_ClientIdentification_t* instance,
                                       struct C_IObject_t* ret);

/**
 * Get the value of machine
 */
int Cstager_ClientIdentification_machine(struct Cstager_ClientIdentification_t* instance, const char** var);

/**
 * Set the value of machine
 */
int Cstager_ClientIdentification_setMachine(struct Cstager_ClientIdentification_t* instance, const char* new_var);

/**
 * Get the value of userName
 */
int Cstager_ClientIdentification_userName(struct Cstager_ClientIdentification_t* instance, const char** var);

/**
 * Set the value of userName
 */
int Cstager_ClientIdentification_setUserName(struct Cstager_ClientIdentification_t* instance, const char* new_var);

/**
 * Get the value of port
 */
int Cstager_ClientIdentification_port(struct Cstager_ClientIdentification_t* instance, struct C_int_t* var);

/**
 * Set the value of port
 */
int Cstager_ClientIdentification_setPort(struct Cstager_ClientIdentification_t* instance, struct C_int_t new_var);

/**
 * Get the value of euid
 */
int Cstager_ClientIdentification_euid(struct Cstager_ClientIdentification_t* instance, struct C_int_t* var);

/**
 * Set the value of euid
 */
int Cstager_ClientIdentification_setEuid(struct Cstager_ClientIdentification_t* instance, struct C_int_t new_var);

/**
 * Get the value of egid
 */
int Cstager_ClientIdentification_egid(struct Cstager_ClientIdentification_t* instance, struct C_int_t* var);

/**
 * Set the value of egid
 */
int Cstager_ClientIdentification_setEgid(struct Cstager_ClientIdentification_t* instance, struct C_int_t new_var);

/**
 * Get the value of magic
 * Specifies the protocol, which is using the client
 */
int Cstager_ClientIdentification_magic(struct Cstager_ClientIdentification_t* instance, struct C_int_t* var);

/**
 * Set the value of magic
 * Specifies the protocol, which is using the client
 */
int Cstager_ClientIdentification_setMagic(struct Cstager_ClientIdentification_t* instance, struct C_int_t new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_ClientIdentification_id(struct Cstager_ClientIdentification_t* instance, struct C_u_signed64_t* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_ClientIdentification_setId(struct Cstager_ClientIdentification_t* instance, struct C_u_signed64_t new_var);

#endif // CASTOR_STAGER_CLIENTIDENTIFICATION_H

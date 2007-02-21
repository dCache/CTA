/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/GCFileList.h
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

#ifndef CASTOR_STAGER_GCFILELIST_H
#define CASTOR_STAGER_GCFILELIST_H

/* Include Files and Forward declarations for the C world */
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_GCFileList_t;
struct Cstager_GCFile_t;
struct Cstager_Request_t;
struct Cstager_SvcClass_t;

/* --------------------------------------------------------------------------
  This defines a C interface to the following class
// class GCFileList
  -------------------------------------------------------------------------- */

/**
 * Empty Constructor
 */
int Cstager_GCFileList_create(struct Cstager_GCFileList_t** obj);

/**
 * Empty Destructor
 */
int Cstager_GCFileList_delete(struct Cstager_GCFileList_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_GCFileList_getRequest(struct Cstager_GCFileList_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_GCFileList_t* Cstager_GCFileList_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_GCFileList_getIObject(struct Cstager_GCFileList_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_GCFileList_t* Cstager_GCFileList_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_GCFileList_print(struct Cstager_GCFileList_t* instance);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_GCFileList_flags(struct Cstager_GCFileList_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_GCFileList_setFlags(struct Cstager_GCFileList_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_GCFileList_userName(struct Cstager_GCFileList_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_GCFileList_setUserName(struct Cstager_GCFileList_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_GCFileList_euid(struct Cstager_GCFileList_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_GCFileList_setEuid(struct Cstager_GCFileList_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_GCFileList_egid(struct Cstager_GCFileList_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_GCFileList_setEgid(struct Cstager_GCFileList_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_GCFileList_mask(struct Cstager_GCFileList_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_GCFileList_setMask(struct Cstager_GCFileList_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_GCFileList_pid(struct Cstager_GCFileList_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_GCFileList_setPid(struct Cstager_GCFileList_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_GCFileList_machine(struct Cstager_GCFileList_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_GCFileList_setMachine(struct Cstager_GCFileList_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_GCFileList_svcClassName(struct Cstager_GCFileList_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_GCFileList_setSvcClassName(struct Cstager_GCFileList_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_GCFileList_userTag(struct Cstager_GCFileList_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 * select requests.
 */
int Cstager_GCFileList_setUserTag(struct Cstager_GCFileList_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_GCFileList_reqId(struct Cstager_GCFileList_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_GCFileList_setReqId(struct Cstager_GCFileList_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_GCFileList_creationTime(struct Cstager_GCFileList_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_GCFileList_setCreationTime(struct Cstager_GCFileList_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_GCFileList_lastModificationTime(struct Cstager_GCFileList_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_GCFileList_setLastModificationTime(struct Cstager_GCFileList_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_GCFileList_svcClass(struct Cstager_GCFileList_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_GCFileList_setSvcClass(struct Cstager_GCFileList_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_GCFileList_client(struct Cstager_GCFileList_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_GCFileList_setClient(struct Cstager_GCFileList_t* instance, struct C_IClient_t* new_var);

/**
 * Add a struct Cstager_GCFile_t* object to the files list
 */
int Cstager_GCFileList_addFiles(struct Cstager_GCFileList_t* instance, struct Cstager_GCFile_t* obj);

/**
 * Remove a struct Cstager_GCFile_t* object from files
 */
int Cstager_GCFileList_removeFiles(struct Cstager_GCFileList_t* instance, struct Cstager_GCFile_t* obj);

/**
 * Get the list of struct Cstager_GCFile_t* objects held by files. Note that the
 * caller is responsible for the deletion of the returned vector.
 */
int Cstager_GCFileList_files(struct Cstager_GCFileList_t* instance, struct Cstager_GCFile_t*** var, int* len);

#endif /* CASTOR_STAGER_GCFILELIST_H */

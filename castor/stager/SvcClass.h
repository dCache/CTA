/******************************************************************************
 *                      castor/stager/SvcClass.h
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

#ifndef CASTOR_STAGER_SVCCLASS_H
#define CASTOR_STAGER_SVCCLASS_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Cstager_DiskPool_t;
struct Cstager_SvcClass_t;
struct Cstager_TapePool_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class SvcClass
// A service, as seen by the user. A SvcClass is a container of resources and may be
// given as parameter of the request.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_SvcClass_create(struct Cstager_SvcClass_t** obj);

/**
 * Empty Destructor
 */
int Cstager_SvcClass_delete(struct Cstager_SvcClass_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_SvcClass_getIObject(struct Cstager_SvcClass_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_SvcClass_t* Cstager_SvcClass_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_SvcClass_print(struct Cstager_SvcClass_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_SvcClass_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_SvcClass_type(struct Cstager_SvcClass_t* instance,
                          int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_SvcClass_clone(struct Cstager_SvcClass_t* instance,
                           struct C_IObject_t* ret);

/**
 * Get the value of nbDrives
 * Number of drives to use for this service class. This is the default number, but
 * it could be that occasionnally more drives are used, if a resource is shared with
 * another service class using more drives
 */
int Cstager_SvcClass_nbDrives(struct Cstager_SvcClass_t* instance, unsigned int* var);

/**
 * Set the value of nbDrives
 * Number of drives to use for this service class. This is the default number, but
 * it could be that occasionnally more drives are used, if a resource is shared with
 * another service class using more drives
 */
int Cstager_SvcClass_setNbDrives(struct Cstager_SvcClass_t* instance, unsigned int new_var);

/**
 * Get the value of name
 * the name of this SvcClass
 */
int Cstager_SvcClass_name(struct Cstager_SvcClass_t* instance, const char** var);

/**
 * Set the value of name
 * the name of this SvcClass
 */
int Cstager_SvcClass_setName(struct Cstager_SvcClass_t* instance, const char* new_var);

/**
 * Get the value of defaultFileSize
 * Default size used for space allocation in the case of a stage put with no size
 * explicitely given (ie size given was 0)
 */
int Cstager_SvcClass_defaultFileSize(struct Cstager_SvcClass_t* instance, u_signed64* var);

/**
 * Set the value of defaultFileSize
 * Default size used for space allocation in the case of a stage put with no size
 * explicitely given (ie size given was 0)
 */
int Cstager_SvcClass_setDefaultFileSize(struct Cstager_SvcClass_t* instance, u_signed64 new_var);

/**
 * Get the value of maxReplicaNb
 * Maximum number of replicas for a file created with this service class. If 0, the
 * replication is ruled by the replication policy. If there is none, then 0 means
 * infinity.
 */
int Cstager_SvcClass_maxReplicaNb(struct Cstager_SvcClass_t* instance, int* var);

/**
 * Set the value of maxReplicaNb
 * Maximum number of replicas for a file created with this service class. If 0, the
 * replication is ruled by the replication policy. If there is none, then 0 means
 * infinity.
 */
int Cstager_SvcClass_setMaxReplicaNb(struct Cstager_SvcClass_t* instance, int new_var);

/**
 * Get the value of replicationPolicy
 * Policy ruling the replication of files in the diskpools.
 */
int Cstager_SvcClass_replicationPolicy(struct Cstager_SvcClass_t* instance, const char** var);

/**
 * Set the value of replicationPolicy
 * Policy ruling the replication of files in the diskpools.
 */
int Cstager_SvcClass_setReplicationPolicy(struct Cstager_SvcClass_t* instance, const char* new_var);

/**
 * Get the value of gcPolicy
 * Policy ruling the garbage collection of files
 */
int Cstager_SvcClass_gcPolicy(struct Cstager_SvcClass_t* instance, const char** var);

/**
 * Set the value of gcPolicy
 * Policy ruling the garbage collection of files
 */
int Cstager_SvcClass_setGcPolicy(struct Cstager_SvcClass_t* instance, const char* new_var);

/**
 * Get the value of migratorPolicy
 * Policy ruling the migration of files.
 */
int Cstager_SvcClass_migratorPolicy(struct Cstager_SvcClass_t* instance, const char** var);

/**
 * Set the value of migratorPolicy
 * Policy ruling the migration of files.
 */
int Cstager_SvcClass_setMigratorPolicy(struct Cstager_SvcClass_t* instance, const char* new_var);

/**
 * Get the value of recallerPolicy
 * Policy ruling the recall of files.
 */
int Cstager_SvcClass_recallerPolicy(struct Cstager_SvcClass_t* instance, const char** var);

/**
 * Set the value of recallerPolicy
 * Policy ruling the recall of files.
 */
int Cstager_SvcClass_setRecallerPolicy(struct Cstager_SvcClass_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_SvcClass_id(struct Cstager_SvcClass_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_SvcClass_setId(struct Cstager_SvcClass_t* instance, u_signed64 new_var);

/**
 * Add a struct Cstager_TapePool_t* object to the tapePools list
 */
int Cstager_SvcClass_addTapePools(struct Cstager_SvcClass_t* instance, struct Cstager_TapePool_t* obj);

/**
 * Remove a struct Cstager_TapePool_t* object from tapePools
 */
int Cstager_SvcClass_removeTapePools(struct Cstager_SvcClass_t* instance, struct Cstager_TapePool_t* obj);

/**
 * Get the list of struct Cstager_TapePool_t* objects held by tapePools. Note that
 * the caller is responsible for the deletion of the returned vector.
 */
int Cstager_SvcClass_tapePools(struct Cstager_SvcClass_t* instance, struct Cstager_TapePool_t*** var, int* len);

/**
 * Add a struct Cstager_DiskPool_t* object to the diskPools list
 */
int Cstager_SvcClass_addDiskPools(struct Cstager_SvcClass_t* instance, struct Cstager_DiskPool_t* obj);

/**
 * Remove a struct Cstager_DiskPool_t* object from diskPools
 */
int Cstager_SvcClass_removeDiskPools(struct Cstager_SvcClass_t* instance, struct Cstager_DiskPool_t* obj);

/**
 * Get the list of struct Cstager_DiskPool_t* objects held by diskPools. Note that
 * the caller is responsible for the deletion of the returned vector.
 */
int Cstager_SvcClass_diskPools(struct Cstager_SvcClass_t* instance, struct Cstager_DiskPool_t*** var, int* len);

#endif // CASTOR_STAGER_SVCCLASS_H

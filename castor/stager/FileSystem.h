/******************************************************************************
 *                      castor/stager/FileSystem.h
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

#ifndef CASTOR_STAGER_FILESYSTEM_H
#define CASTOR_STAGER_FILESYSTEM_H

// Include Files and Forward declarations for the C world
#include "castor/stager/FileSystemStatusCodes.h"
#include "osdep.h"
struct C_IObject_t;
struct Cstager_DiskCopy_t;
struct Cstager_DiskPool_t;
struct Cstager_DiskServer_t;
struct Cstager_FileSystem_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class FileSystem
// A file system used in a disk pool
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_FileSystem_create(struct Cstager_FileSystem_t** obj);

/**
 * Empty Destructor
 */
int Cstager_FileSystem_delete(struct Cstager_FileSystem_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_FileSystem_getIObject(struct Cstager_FileSystem_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_FileSystem_t* Cstager_FileSystem_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_FileSystem_print(struct Cstager_FileSystem_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_FileSystem_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_FileSystem_type(struct Cstager_FileSystem_t* instance,
                            int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_FileSystem_clone(struct Cstager_FileSystem_t* instance,
                             struct C_IObject_t* ret);

/**
 * Get the value of free
 * Free space on the filesystem
 */
int Cstager_FileSystem_free(struct Cstager_FileSystem_t* instance, u_signed64* var);

/**
 * Set the value of free
 * Free space on the filesystem
 */
int Cstager_FileSystem_setFree(struct Cstager_FileSystem_t* instance, u_signed64 new_var);

/**
 * Get the value of weight
 * Weight of the filesystem, as computed by the expert system
 */
int Cstager_FileSystem_weight(struct Cstager_FileSystem_t* instance, float* var);

/**
 * Set the value of weight
 * Weight of the filesystem, as computed by the expert system
 */
int Cstager_FileSystem_setWeight(struct Cstager_FileSystem_t* instance, float new_var);

/**
 * Get the value of fsDeviation
 */
int Cstager_FileSystem_fsDeviation(struct Cstager_FileSystem_t* instance, float* var);

/**
 * Set the value of fsDeviation
 */
int Cstager_FileSystem_setFsDeviation(struct Cstager_FileSystem_t* instance, float new_var);

/**
 * Get the value of mountPoint
 */
int Cstager_FileSystem_mountPoint(struct Cstager_FileSystem_t* instance, const char** var);

/**
 * Set the value of mountPoint
 */
int Cstager_FileSystem_setMountPoint(struct Cstager_FileSystem_t* instance, const char* new_var);

/**
 * Get the value of deltaWeight
 * Estimation of the modifications of the weight since the last update.
 */
int Cstager_FileSystem_deltaWeight(struct Cstager_FileSystem_t* instance, float* var);

/**
 * Set the value of deltaWeight
 * Estimation of the modifications of the weight since the last update.
 */
int Cstager_FileSystem_setDeltaWeight(struct Cstager_FileSystem_t* instance, float new_var);

/**
 * Get the value of deltaFree
 * Estimation of the modifications of the free space since the last update.
 */
int Cstager_FileSystem_deltaFree(struct Cstager_FileSystem_t* instance, int* var);

/**
 * Set the value of deltaFree
 * Estimation of the modifications of the free space since the last update.
 */
int Cstager_FileSystem_setDeltaFree(struct Cstager_FileSystem_t* instance, int new_var);

/**
 * Get the value of reservedSpace
 * Space reserved on the filesystem but not yet used and thus not taken into account
 * in free and deltaFree
 */
int Cstager_FileSystem_reservedSpace(struct Cstager_FileSystem_t* instance, int* var);

/**
 * Set the value of reservedSpace
 * Space reserved on the filesystem but not yet used and thus not taken into account
 * in free and deltaFree
 */
int Cstager_FileSystem_setReservedSpace(struct Cstager_FileSystem_t* instance, int new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_FileSystem_id(struct Cstager_FileSystem_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_FileSystem_setId(struct Cstager_FileSystem_t* instance, u_signed64 new_var);

/**
 * Get the value of diskPool
 */
int Cstager_FileSystem_diskPool(struct Cstager_FileSystem_t* instance, struct Cstager_DiskPool_t** var);

/**
 * Set the value of diskPool
 */
int Cstager_FileSystem_setDiskPool(struct Cstager_FileSystem_t* instance, struct Cstager_DiskPool_t* new_var);

/**
 * Add a struct Cstager_DiskCopy_t* object to the copies list
 */
int Cstager_FileSystem_addCopies(struct Cstager_FileSystem_t* instance, struct Cstager_DiskCopy_t* obj);

/**
 * Remove a struct Cstager_DiskCopy_t* object from copies
 */
int Cstager_FileSystem_removeCopies(struct Cstager_FileSystem_t* instance, struct Cstager_DiskCopy_t* obj);

/**
 * Get the list of struct Cstager_DiskCopy_t* objects held by copies. Note that the
 * caller is responsible for the deletion of the returned vector.
 */
int Cstager_FileSystem_copies(struct Cstager_FileSystem_t* instance, struct Cstager_DiskCopy_t*** var, int* len);

/**
 * Get the value of diskserver
 */
int Cstager_FileSystem_diskserver(struct Cstager_FileSystem_t* instance, struct Cstager_DiskServer_t** var);

/**
 * Set the value of diskserver
 */
int Cstager_FileSystem_setDiskserver(struct Cstager_FileSystem_t* instance, struct Cstager_DiskServer_t* new_var);

/**
 * Get the value of status
 */
int Cstager_FileSystem_status(struct Cstager_FileSystem_t* instance, enum Cstager_FileSystemStatusCodes_t* var);

/**
 * Set the value of status
 */
int Cstager_FileSystem_setStatus(struct Cstager_FileSystem_t* instance, enum Cstager_FileSystemStatusCodes_t new_var);

#endif // CASTOR_STAGER_FILESYSTEM_H

/******************************************************************************
 *                      castor/stager/DiskCopyInfo.h
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

#ifndef CASTOR_STAGER_DISKCOPYINFO_H
#define CASTOR_STAGER_DISKCOPYINFO_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Cstager_DiskCopyInfo_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class DiskCopyInfo
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_DiskCopyInfo_create(struct Cstager_DiskCopyInfo_t** obj);

/**
 * Empty Destructor
 */
int Cstager_DiskCopyInfo_delete(struct Cstager_DiskCopyInfo_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_DiskCopyInfo_getIObject(struct Cstager_DiskCopyInfo_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_DiskCopyInfo_t* Cstager_DiskCopyInfo_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_DiskCopyInfo_print(struct Cstager_DiskCopyInfo_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_DiskCopyInfo_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_DiskCopyInfo_type(struct Cstager_DiskCopyInfo_t* instance,
                              int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_DiskCopyInfo_clone(struct Cstager_DiskCopyInfo_t* instance,
                               struct C_IObject_t* ret);

/**
 * Get the value of DiskCopyPath
 * Path of the DiskCopy described
 */
int Cstager_DiskCopyInfo_DiskCopyPath(struct Cstager_DiskCopyInfo_t* instance, const char** var);

/**
 * Set the value of DiskCopyPath
 * Path of the DiskCopy described
 */
int Cstager_DiskCopyInfo_setDiskCopyPath(struct Cstager_DiskCopyInfo_t* instance, const char* new_var);

/**
 * Get the value of size
 * Size of the underlying CastorFile
 */
int Cstager_DiskCopyInfo_size(struct Cstager_DiskCopyInfo_t* instance, u_signed64* var);

/**
 * Set the value of size
 * Size of the underlying CastorFile
 */
int Cstager_DiskCopyInfo_setSize(struct Cstager_DiskCopyInfo_t* instance, u_signed64 new_var);

/**
 * Get the value of diskCopyStatus
 * Status of the DiskCopy described
 */
int Cstager_DiskCopyInfo_diskCopyStatus(struct Cstager_DiskCopyInfo_t* instance, int* var);

/**
 * Set the value of diskCopyStatus
 * Status of the DiskCopy described
 */
int Cstager_DiskCopyInfo_setDiskCopyStatus(struct Cstager_DiskCopyInfo_t* instance, int new_var);

/**
 * Get the value of tapeCopyStatus
 * Status of the tapeCopy(ies?) associated to the underlying CastorFile
 */
int Cstager_DiskCopyInfo_tapeCopyStatus(struct Cstager_DiskCopyInfo_t* instance, int* var);

/**
 * Set the value of tapeCopyStatus
 * Status of the tapeCopy(ies?) associated to the underlying CastorFile
 */
int Cstager_DiskCopyInfo_setTapeCopyStatus(struct Cstager_DiskCopyInfo_t* instance, int new_var);

/**
 * Get the value of segmentStatus
 * Status of the segment(s?) associated to the underlying CastorFile
 */
int Cstager_DiskCopyInfo_segmentStatus(struct Cstager_DiskCopyInfo_t* instance, int* var);

/**
 * Set the value of segmentStatus
 * Status of the segment(s?) associated to the underlying CastorFile
 */
int Cstager_DiskCopyInfo_setSegmentStatus(struct Cstager_DiskCopyInfo_t* instance, int new_var);

/**
 * Get the value of DiskCopyId
 * Id of the DiskCopy Described
 */
int Cstager_DiskCopyInfo_DiskCopyId(struct Cstager_DiskCopyInfo_t* instance, u_signed64* var);

/**
 * Set the value of DiskCopyId
 * Id of the DiskCopy Described
 */
int Cstager_DiskCopyInfo_setDiskCopyId(struct Cstager_DiskCopyInfo_t* instance, u_signed64 new_var);

/**
 * Get the value of fileId
 * fileId of the underlying castorfile
 */
int Cstager_DiskCopyInfo_fileId(struct Cstager_DiskCopyInfo_t* instance, u_signed64* var);

/**
 * Set the value of fileId
 * fileId of the underlying castorfile
 */
int Cstager_DiskCopyInfo_setFileId(struct Cstager_DiskCopyInfo_t* instance, u_signed64 new_var);

/**
 * Get the value of nsHost
 * name server host of the underlying castorfile
 */
int Cstager_DiskCopyInfo_nsHost(struct Cstager_DiskCopyInfo_t* instance, const char** var);

/**
 * Set the value of nsHost
 * name server host of the underlying castorfile
 */
int Cstager_DiskCopyInfo_setNsHost(struct Cstager_DiskCopyInfo_t* instance, const char* new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_DiskCopyInfo_id(struct Cstager_DiskCopyInfo_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_DiskCopyInfo_setId(struct Cstager_DiskCopyInfo_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_DISKCOPYINFO_H

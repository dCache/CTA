/******************************************************************************
 *                      castor/stager/TapeCopyForMigration.h
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

#ifndef CASTOR_STAGER_TAPECOPYFORMIGRATION_H
#define CASTOR_STAGER_TAPECOPYFORMIGRATION_H

// Include Files and Forward declarations for the C world
#include "castor/stager/TapeCopyStatusCodes.h"
#include "osdep.h"
struct C_IObject_t;
struct Cstager_CastorFile_t;
struct Cstager_Segment_t;
struct Cstager_Stream_t;
struct Cstager_TapeCopyForMigration_t;
struct Cstager_TapeCopy_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class TapeCopyForMigration
// This class is a wrapper around a Tape Copy that represents a TapCopy ready for
// Migration. It thus has information about the physical file to be copied.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_TapeCopyForMigration_create(struct Cstager_TapeCopyForMigration_t** obj);

/**
 * Empty Destructor
 */
int Cstager_TapeCopyForMigration_delete(struct Cstager_TapeCopyForMigration_t* obj);

/**
 * Cast into TapeCopy
 */
struct Cstager_TapeCopy_t* Cstager_TapeCopyForMigration_getTapeCopy(struct Cstager_TapeCopyForMigration_t* obj);

/**
 * Dynamic cast from TapeCopy
 */
struct Cstager_TapeCopyForMigration_t* Cstager_TapeCopyForMigration_fromTapeCopy(struct Cstager_TapeCopy_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_TapeCopyForMigration_getIObject(struct Cstager_TapeCopyForMigration_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_TapeCopyForMigration_t* Cstager_TapeCopyForMigration_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_TapeCopyForMigration_print(struct Cstager_TapeCopyForMigration_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_TapeCopyForMigration_TYPE(int* ret);

/**
 * Gets the type of the object
 */
int Cstager_TapeCopyForMigration_type(struct Cstager_TapeCopyForMigration_t* instance,
                                      int* ret);

/************************************/
/* Implementation of TapeCopy class */
/************************************/

/**
 * Get the value of copyNb
 * The copy number allows to identify the different copies on tape of a single file
 */
int Cstager_TapeCopyForMigration_copyNb(struct Cstager_TapeCopyForMigration_t* instance, unsigned int* var);

/**
 * Set the value of copyNb
 * The copy number allows to identify the different copies on tape of a single file
 */
int Cstager_TapeCopyForMigration_setCopyNb(struct Cstager_TapeCopyForMigration_t* instance, unsigned int new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_TapeCopyForMigration_id(struct Cstager_TapeCopyForMigration_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_TapeCopyForMigration_setId(struct Cstager_TapeCopyForMigration_t* instance, u_signed64 new_var);

/**
 * Add a struct Cstager_Stream_t* object to the stream list
 */
int Cstager_TapeCopyForMigration_addStream(struct Cstager_TapeCopyForMigration_t* instance, struct Cstager_Stream_t* obj);

/**
 * Remove a struct Cstager_Stream_t* object from stream
 */
int Cstager_TapeCopyForMigration_removeStream(struct Cstager_TapeCopyForMigration_t* instance, struct Cstager_Stream_t* obj);

/**
 * Get the list of struct Cstager_Stream_t* objects held by stream
 */
int Cstager_TapeCopyForMigration_stream(struct Cstager_TapeCopyForMigration_t* instance, struct Cstager_Stream_t*** var, int* len);

/**
 * Add a struct Cstager_Segment_t* object to the segments list
 */
int Cstager_TapeCopyForMigration_addSegments(struct Cstager_TapeCopyForMigration_t* instance, struct Cstager_Segment_t* obj);

/**
 * Remove a struct Cstager_Segment_t* object from segments
 */
int Cstager_TapeCopyForMigration_removeSegments(struct Cstager_TapeCopyForMigration_t* instance, struct Cstager_Segment_t* obj);

/**
 * Get the list of struct Cstager_Segment_t* objects held by segments
 */
int Cstager_TapeCopyForMigration_segments(struct Cstager_TapeCopyForMigration_t* instance, struct Cstager_Segment_t*** var, int* len);

/**
 * Get the value of castorFile
 */
int Cstager_TapeCopyForMigration_castorFile(struct Cstager_TapeCopyForMigration_t* instance, struct Cstager_CastorFile_t** var);

/**
 * Set the value of castorFile
 */
int Cstager_TapeCopyForMigration_setCastorFile(struct Cstager_TapeCopyForMigration_t* instance, struct Cstager_CastorFile_t* new_var);

/**
 * Get the value of status
 */
int Cstager_TapeCopyForMigration_status(struct Cstager_TapeCopyForMigration_t* instance, enum Cstager_TapeCopyStatusCodes_t* var);

/**
 * Set the value of status
 */
int Cstager_TapeCopyForMigration_setStatus(struct Cstager_TapeCopyForMigration_t* instance, enum Cstager_TapeCopyStatusCodes_t new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Sets the id of the object
 */
int Cstager_TapeCopyForMigration_setId(struct Cstager_TapeCopyForMigration_t* instance,
                                       u_signed64 id);

/**
 * gets the id of the object
 */
int Cstager_TapeCopyForMigration_id(struct Cstager_TapeCopyForMigration_t* instance,
                                    u_signed64* ret);

/**
 * virtual method to clone any object
 */
int Cstager_TapeCopyForMigration_clone(struct Cstager_TapeCopyForMigration_t* instance,
                                       struct C_IObject_t* ret);

/**
 * Get the value of diskServer
 * The disk server on which the file to be migrated resides
 */
int Cstager_TapeCopyForMigration_diskServer(struct Cstager_TapeCopyForMigration_t* instance, const char** var);

/**
 * Set the value of diskServer
 * The disk server on which the file to be migrated resides
 */
int Cstager_TapeCopyForMigration_setDiskServer(struct Cstager_TapeCopyForMigration_t* instance, const char* new_var);

/**
 * Get the value of mountPoint
 */
int Cstager_TapeCopyForMigration_mountPoint(struct Cstager_TapeCopyForMigration_t* instance, const char** var);

/**
 * Set the value of mountPoint
 */
int Cstager_TapeCopyForMigration_setMountPoint(struct Cstager_TapeCopyForMigration_t* instance, const char* new_var);

#endif // CASTOR_STAGER_TAPECOPYFORMIGRATION_H

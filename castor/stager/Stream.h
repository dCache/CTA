/******************************************************************************
 *                      castor/stager/Stream.h
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

#ifndef CASTOR_STAGER_STREAM_H
#define CASTOR_STAGER_STREAM_H

// Include Files and Forward declarations for the C world
#include "castor/stager/StreamStatusCodes.h"
#include "osdep.h"
struct C_IObject_t;
struct Cstager_Stream_t;
struct Cstager_TapeCopy_t;
struct Cstager_TapePool_t;
struct Cstager_Tape_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class Stream
// A logical stream for the writing of DiskCopies on Tapes
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_Stream_create(struct Cstager_Stream_t** obj);

/**
 * Empty Destructor
 */
int Cstager_Stream_delete(struct Cstager_Stream_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_Stream_getIObject(struct Cstager_Stream_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_Stream_t* Cstager_Stream_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_Stream_print(struct Cstager_Stream_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_Stream_TYPE(int* ret);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_Stream_type(struct Cstager_Stream_t* instance,
                        int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_Stream_clone(struct Cstager_Stream_t* instance,
                         struct C_IObject_t* ret);

/**
 * Get the value of initialSizeToTransfer
 * Initial data volume to be migrated (needed by vmgr_gettape())
 */
int Cstager_Stream_initialSizeToTransfer(struct Cstager_Stream_t* instance, u_signed64* var);

/**
 * Set the value of initialSizeToTransfer
 * Initial data volume to be migrated (needed by vmgr_gettape())
 */
int Cstager_Stream_setInitialSizeToTransfer(struct Cstager_Stream_t* instance, u_signed64 new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_Stream_id(struct Cstager_Stream_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_Stream_setId(struct Cstager_Stream_t* instance, u_signed64 new_var);

/**
 * Add a struct Cstager_TapeCopy_t* object to the tapeCopy list
 */
int Cstager_Stream_addTapeCopy(struct Cstager_Stream_t* instance, struct Cstager_TapeCopy_t* obj);

/**
 * Remove a struct Cstager_TapeCopy_t* object from tapeCopy
 */
int Cstager_Stream_removeTapeCopy(struct Cstager_Stream_t* instance, struct Cstager_TapeCopy_t* obj);

/**
 * Get the list of struct Cstager_TapeCopy_t* objects held by tapeCopy. Note that
 * the caller is responsible for the deletion of the returned vector.
 */
int Cstager_Stream_tapeCopy(struct Cstager_Stream_t* instance, struct Cstager_TapeCopy_t*** var, int* len);

/**
 * Get the value of tape
 */
int Cstager_Stream_tape(struct Cstager_Stream_t* instance, struct Cstager_Tape_t** var);

/**
 * Set the value of tape
 */
int Cstager_Stream_setTape(struct Cstager_Stream_t* instance, struct Cstager_Tape_t* new_var);

/**
 * Get the value of tapePool
 */
int Cstager_Stream_tapePool(struct Cstager_Stream_t* instance, struct Cstager_TapePool_t** var);

/**
 * Set the value of tapePool
 */
int Cstager_Stream_setTapePool(struct Cstager_Stream_t* instance, struct Cstager_TapePool_t* new_var);

/**
 * Get the value of status
 */
int Cstager_Stream_status(struct Cstager_Stream_t* instance, enum Cstager_StreamStatusCodes_t* var);

/**
 * Set the value of status
 */
int Cstager_Stream_setStatus(struct Cstager_Stream_t* instance, enum Cstager_StreamStatusCodes_t new_var);

#endif // CASTOR_STAGER_STREAM_H

/******************************************************************************
 *                      castor/stager/Segment.h
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

#ifndef CASTOR_STAGER_SEGMENT_H
#define CASTOR_STAGER_SEGMENT_H

// Include Files and Forward declarations for the C world
#include "castor/stager/SegmentStatusCodes.h"
#include "osdep.h"
struct C_IObject_t;
struct Cstager_Segment_t;
struct Cstager_TapeCopy_t;
struct Cstager_Tape_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class Segment
// Segment Information  The Segment object contains all tape file (segment)
// information required to be stored in the stager request catalog. The remaining
// tape file parameters such as record format and blocksize are either defined by
// defaults or taken from the tape label.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_Segment_create(struct Cstager_Segment_t** obj);

/**
 * Empty Destructor
 */
int Cstager_Segment_delete(struct Cstager_Segment_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_Segment_getIObject(struct Cstager_Segment_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_Segment_t* Cstager_Segment_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_Segment_print(struct Cstager_Segment_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_Segment_TYPE(int* ret);

/********************************************/
/* Implementation of IObject abstract class */
/********************************************/
/**
 * Sets the id of the object
 */
int Cstager_Segment_setId(struct Cstager_Segment_t* instance,
                          u_signed64 id);

/**
 * gets the id of the object
 */
int Cstager_Segment_id(struct Cstager_Segment_t* instance,
                       u_signed64* ret);

/**
 * Gets the type of the object
 */
int Cstager_Segment_type(struct Cstager_Segment_t* instance,
                         int* ret);

/**
 * Get the value of blockid
 */
int Cstager_Segment_blockid(struct Cstager_Segment_t* instance, const unsigned char** var);

/**
 * Set the value of blockid
 */
int Cstager_Segment_setBlockid(struct Cstager_Segment_t* instance, const unsigned char* new_var);

/**
 * Get the value of fseq
 * Tape media file sequence number (if no locate)
 */
int Cstager_Segment_fseq(struct Cstager_Segment_t* instance, int* var);

/**
 * Set the value of fseq
 * Tape media file sequence number (if no locate)
 */
int Cstager_Segment_setFseq(struct Cstager_Segment_t* instance, int new_var);

/**
 * Get the value of offset
 * Start offset in the disk file (if segmented file)
 */
int Cstager_Segment_offset(struct Cstager_Segment_t* instance, u_signed64* var);

/**
 * Set the value of offset
 * Start offset in the disk file (if segmented file)
 */
int Cstager_Segment_setOffset(struct Cstager_Segment_t* instance, u_signed64 new_var);

/**
 * Get the value of bytes_in
 * Size of the tape file (segment) to be copied
 */
int Cstager_Segment_bytes_in(struct Cstager_Segment_t* instance, u_signed64* var);

/**
 * Set the value of bytes_in
 * Size of the tape file (segment) to be copied
 */
int Cstager_Segment_setBytes_in(struct Cstager_Segment_t* instance, u_signed64 new_var);

/**
 * Get the value of bytes_out
 */
int Cstager_Segment_bytes_out(struct Cstager_Segment_t* instance, u_signed64* var);

/**
 * Set the value of bytes_out
 */
int Cstager_Segment_setBytes_out(struct Cstager_Segment_t* instance, u_signed64 new_var);

/**
 * Get the value of host_bytes
 */
int Cstager_Segment_host_bytes(struct Cstager_Segment_t* instance, u_signed64* var);

/**
 * Set the value of host_bytes
 */
int Cstager_Segment_setHost_bytes(struct Cstager_Segment_t* instance, u_signed64 new_var);

/**
 * Get the value of segmCksumAlgorithm
 */
int Cstager_Segment_segmCksumAlgorithm(struct Cstager_Segment_t* instance, const char** var);

/**
 * Set the value of segmCksumAlgorithm
 */
int Cstager_Segment_setSegmCksumAlgorithm(struct Cstager_Segment_t* instance, const char* new_var);

/**
 * Get the value of segmCksum
 */
int Cstager_Segment_segmCksum(struct Cstager_Segment_t* instance, unsigned long* var);

/**
 * Set the value of segmCksum
 */
int Cstager_Segment_setSegmCksum(struct Cstager_Segment_t* instance, unsigned long new_var);

/**
 * Get the value of errMsgTxt
 */
int Cstager_Segment_errMsgTxt(struct Cstager_Segment_t* instance, const char** var);

/**
 * Set the value of errMsgTxt
 */
int Cstager_Segment_setErrMsgTxt(struct Cstager_Segment_t* instance, const char* new_var);

/**
 * Get the value of errorCode
 * RTCOPY serrno if status == SEGMENT_FAILED
 */
int Cstager_Segment_errorCode(struct Cstager_Segment_t* instance, int* var);

/**
 * Set the value of errorCode
 * RTCOPY serrno if status == SEGMENT_FAILED
 */
int Cstager_Segment_setErrorCode(struct Cstager_Segment_t* instance, int new_var);

/**
 * Get the value of severity
 */
int Cstager_Segment_severity(struct Cstager_Segment_t* instance, int* var);

/**
 * Set the value of severity
 */
int Cstager_Segment_setSeverity(struct Cstager_Segment_t* instance, int new_var);

/**
 * Get the value of tape
 */
int Cstager_Segment_tape(struct Cstager_Segment_t* instance, struct Cstager_Tape_t** var);

/**
 * Set the value of tape
 */
int Cstager_Segment_setTape(struct Cstager_Segment_t* instance, struct Cstager_Tape_t* new_var);

/**
 * Get the value of copy
 */
int Cstager_Segment_copy(struct Cstager_Segment_t* instance, struct Cstager_TapeCopy_t** var);

/**
 * Set the value of copy
 */
int Cstager_Segment_setCopy(struct Cstager_Segment_t* instance, struct Cstager_TapeCopy_t* new_var);

/**
 * Get the value of status
 */
int Cstager_Segment_status(struct Cstager_Segment_t* instance, enum Cstager_SegmentStatusCodes_t* var);

/**
 * Set the value of status
 */
int Cstager_Segment_setStatus(struct Cstager_Segment_t* instance, enum Cstager_SegmentStatusCodes_t new_var);

#endif // CASTOR_STAGER_SEGMENT_H

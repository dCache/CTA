/******************************************************************************
 *                      castor/stager/StageGetRequest.h
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

#ifndef CASTOR_STAGER_STAGEGETREQUEST_H
#define CASTOR_STAGER_STAGEGETREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct Cstager_FileRequest_t;
struct Cstager_StageGetRequest_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StageGetRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StageGetRequest_create(struct Cstager_StageGetRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StageGetRequest_delete(struct Cstager_StageGetRequest_t* obj);

/**
 * Cast into FileRequest
 */
struct Cstager_FileRequest_t* Cstager_StageGetRequest_getFileRequest(struct Cstager_StageGetRequest_t* obj);

/**
 * Dynamic cast from FileRequest
 */
struct Cstager_StageGetRequest_t* Cstager_StageGetRequest_fromFileRequest(struct Cstager_FileRequest_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StageGetRequest_print(struct Cstager_StageGetRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StageGetRequest_TYPE(int* ret);

/********************************************/
/* Implementation of IObject abstract class */
/********************************************/
/**
 * Sets the id of the object
 */
int Cstager_StageGetRequest_setId(struct Cstager_StageGetRequest_t* instance,
                                  u_signed64 id);

/**
 * gets the id of the object
 */
int Cstager_StageGetRequest_id(struct Cstager_StageGetRequest_t* instance,
                               u_signed64* ret);

/**
 * Gets the type of the object
 */
int Cstager_StageGetRequest_type(struct Cstager_StageGetRequest_t* instance,
                                 int* ret);

#endif // CASTOR_STAGER_STAGEGETREQUEST_H

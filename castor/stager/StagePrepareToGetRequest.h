/******************************************************************************
 *                      castor/stager/StagePrepareToGetRequest.h
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

#ifndef CASTOR_STAGER_STAGEPREPARETOGETREQUEST_H
#define CASTOR_STAGER_STAGEPREPARETOGETREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct Cstager_Request_t;
struct Cstager_StagePrepareToGetRequest_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StagePrepareToGetRequest
// 
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StagePrepareToGetRequest_create(struct Cstager_StagePrepareToGetRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StagePrepareToGetRequest_delete(struct Cstager_StagePrepareToGetRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_StagePrepareToGetRequest_getRequest(struct Cstager_StagePrepareToGetRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_StagePrepareToGetRequest_t* Cstager_StagePrepareToGetRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StagePrepareToGetRequest_print(struct Cstager_StagePrepareToGetRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StagePrepareToGetRequest_TYPE(int* ret);

/********************************************/
/* Implementation of IObject abstract class */
/********************************************/
/**
 * Sets the id of the object
 */
int Cstager_StagePrepareToGetRequest_setId(struct Cstager_StagePrepareToGetRequest_t* instance,
                                           u_signed64 id);

/**
 * gets the id of the object
 */
int Cstager_StagePrepareToGetRequest_id(struct Cstager_StagePrepareToGetRequest_t* instance,
                                        u_signed64* ret);

/**
 * Gets the type of the object
 */
int Cstager_StagePrepareToGetRequest_type(struct Cstager_StagePrepareToGetRequest_t* instance,
                                          int* ret);

#endif // CASTOR_STAGER_STAGEPREPARETOGETREQUEST_H

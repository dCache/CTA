/******************************************************************************
 *                      castor/IObject.h
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
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

#ifndef CASTOR_IOBJECT_H
#define CASTOR_IOBJECT_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class IObject
// Base class for all objects Implements the id methods
//------------------------------------------------------------------------------

/**
 * Empty Destructor
 */
int C_IObject_delete(struct C_IObject_t* obj);

/**
 * Sets the id of the object
 */
int C_IObject_setId(struct C_IObject_t* instance,
                    u_signed64 id);

/**
 * gets the id of the object
 */
int C_IObject_id(struct C_IObject_t* instance,
                 u_signed64* ret);

/**
 * Gets the type of the object
 */
int C_IObject_type(struct C_IObject_t* instance,
                   int* ret);

/**
 * Outputs this object in a human readable format
 */
int C_IObject_print(struct C_IObject_t* instance);

#endif // CASTOR_IOBJECT_H

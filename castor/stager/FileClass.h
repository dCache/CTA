/******************************************************************************
 *                      castor/stager/FileClass.h
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

#ifndef CASTOR_STAGER_FILECLASS_H
#define CASTOR_STAGER_FILECLASS_H

// Include Files and Forward declarations for the C world
struct C_IObject_t;
struct Cstager_FileClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class FileClass
// The FileClass of a file defines several attributes like the number of copies of
// the file or its tapepool
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_FileClass_create(struct Cstager_FileClass_t** obj);

/**
 * Empty Destructor
 */
int Cstager_FileClass_delete(struct Cstager_FileClass_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_FileClass_getIObject(struct Cstager_FileClass_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_FileClass_t* Cstager_FileClass_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_FileClass_print(struct Cstager_FileClass_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_FileClass_TYPE(int* ret);

/********************************************/
/* Implementation of IObject abstract class */
/********************************************/
/**
 * Sets the id of the object
 */
int Cstager_FileClass_setId(struct Cstager_FileClass_t* instance,
                            unsigned long id);

/**
 * gets the id of the object
 */
int Cstager_FileClass_id(struct Cstager_FileClass_t* instance,
                         unsigned long* ret);

/**
 * Gets the type of the object
 */
int Cstager_FileClass_type(struct Cstager_FileClass_t* instance,
                           int* ret);

/**
 * Get the value of name
 * The name of the FileClass
 */
int Cstager_FileClass_name(struct Cstager_FileClass_t* instance, const char** var);

/**
 * Set the value of name
 * The name of the FileClass
 */
int Cstager_FileClass_setName(struct Cstager_FileClass_t* instance, const char* new_var);

/**
 * Get the value of minFileSize
 * The minimum size of a file in this FileClass
 */
int Cstager_FileClass_minFileSize(struct Cstager_FileClass_t* instance, unsigned int* var);

/**
 * Set the value of minFileSize
 * The minimum size of a file in this FileClass
 */
int Cstager_FileClass_setMinFileSize(struct Cstager_FileClass_t* instance, unsigned int new_var);

/**
 * Get the value of maxFileSize
 * The maximum size of a file in this FileClass
 */
int Cstager_FileClass_maxFileSize(struct Cstager_FileClass_t* instance, unsigned int* var);

/**
 * Set the value of maxFileSize
 * The maximum size of a file in this FileClass
 */
int Cstager_FileClass_setMaxFileSize(struct Cstager_FileClass_t* instance, unsigned int new_var);

/**
 * Get the value of nbCopies
 * The number of copies on tape for a file of this FileClass
 */
int Cstager_FileClass_nbCopies(struct Cstager_FileClass_t* instance, unsigned int* var);

/**
 * Set the value of nbCopies
 * The number of copies on tape for a file of this FileClass
 */
int Cstager_FileClass_setNbCopies(struct Cstager_FileClass_t* instance, unsigned int new_var);

#endif // CASTOR_STAGER_FILECLASS_H

/******************************************************************************
 *                      castor/stager/GCLocalFileCInt.cpp
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

// Include Files
#include "castor/IObject.hpp"
#include "castor/stager/GCLocalFile.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_create
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_create(castor::stager::GCLocalFile** obj) {
    *obj = new castor::stager::GCLocalFile();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_delete
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_delete(castor::stager::GCLocalFile* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_GCLocalFile_getIObject(castor::stager::GCLocalFile* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::GCLocalFile* Cstager_GCLocalFile_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::GCLocalFile*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_print
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_print(castor::stager::GCLocalFile* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_TYPE
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_TYPE(int* ret) {
    *ret = castor::stager::GCLocalFile::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_type
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_type(castor::stager::GCLocalFile* instance,
                               int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_clone
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_clone(castor::stager::GCLocalFile* instance,
                                castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_fileName
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_fileName(castor::stager::GCLocalFile* instance, const char** var) {
    *var = instance->fileName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_setFileName
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_setFileName(castor::stager::GCLocalFile* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setFileName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_diskCopyId
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_diskCopyId(castor::stager::GCLocalFile* instance, u_signed64* var) {
    *var = instance->diskCopyId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_setDiskCopyId
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_setDiskCopyId(castor::stager::GCLocalFile* instance, u_signed64 new_var) {
    instance->setDiskCopyId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_id
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_id(castor::stager::GCLocalFile* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GCLocalFile_setId
  //----------------------------------------------------------------------------
  int Cstager_GCLocalFile_setId(castor::stager::GCLocalFile* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/CastorFileCInt.cpp
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
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/FileClass.hpp"
#include "castor/stager/SvcClass.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "osdep.h"
#include <stdlib.h>
#include <string.h>
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_create
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_create(castor::stager::CastorFile** obj) {
    *obj = new castor::stager::CastorFile();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_CastorFile_delete
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_delete(castor::stager::CastorFile* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_CastorFile_getIObject(castor::stager::CastorFile* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::CastorFile* Cstager_CastorFile_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::CastorFile*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_print
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_print(castor::stager::CastorFile* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_TYPE
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_TYPE(int* ret) {
    *ret = castor::stager::CastorFile::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_type
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_type(castor::stager::CastorFile* instance,
                              int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_clone
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_clone(castor::stager::CastorFile* instance,
                               castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_fileId
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_fileId(castor::stager::CastorFile* instance, u_signed64* var) {
    *var = instance->fileId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setFileId
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setFileId(castor::stager::CastorFile* instance, u_signed64 new_var) {
    instance->setFileId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_nsHost
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_nsHost(castor::stager::CastorFile* instance, const char** var) {
    *var = instance->nsHost().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setNsHost
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setNsHost(castor::stager::CastorFile* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setNsHost(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_fileSize
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_fileSize(castor::stager::CastorFile* instance, u_signed64* var) {
    *var = instance->fileSize();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setFileSize
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setFileSize(castor::stager::CastorFile* instance, u_signed64 new_var) {
    instance->setFileSize(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_creationTime
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_creationTime(castor::stager::CastorFile* instance, u_signed64* var) {
    *var = instance->creationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setCreationTime
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setCreationTime(castor::stager::CastorFile* instance, u_signed64 new_var) {
    instance->setCreationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_lastAccessTime
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_lastAccessTime(castor::stager::CastorFile* instance, u_signed64* var) {
    *var = instance->lastAccessTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setLastAccessTime
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setLastAccessTime(castor::stager::CastorFile* instance, u_signed64 new_var) {
    instance->setLastAccessTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_lastKnownFileName
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_lastKnownFileName(castor::stager::CastorFile* instance, const char** var) {
    *var = instance->lastKnownFileName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setLastKnownFileName
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setLastKnownFileName(castor::stager::CastorFile* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setLastKnownFileName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_lastUpdateTime
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_lastUpdateTime(castor::stager::CastorFile* instance, u_signed64* var) {
    *var = instance->lastUpdateTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setLastUpdateTime
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setLastUpdateTime(castor::stager::CastorFile* instance, u_signed64 new_var) {
    instance->setLastUpdateTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_id
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_id(castor::stager::CastorFile* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setId
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setId(castor::stager::CastorFile* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_svcClass
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_svcClass(castor::stager::CastorFile* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setSvcClass(castor::stager::CastorFile* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_fileClass
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_fileClass(castor::stager::CastorFile* instance, castor::stager::FileClass** var) {
    *var = instance->fileClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_setFileClass
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_setFileClass(castor::stager::CastorFile* instance, castor::stager::FileClass* new_var) {
    instance->setFileClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_addDiskCopies
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_addDiskCopies(castor::stager::CastorFile* instance, castor::stager::DiskCopy* obj) {
    instance->addDiskCopies(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_removeDiskCopies
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_removeDiskCopies(castor::stager::CastorFile* instance, castor::stager::DiskCopy* obj) {
    instance->removeDiskCopies(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_diskCopies
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_diskCopies(castor::stager::CastorFile* instance, castor::stager::DiskCopy*** var, int* len) {
    std::vector<castor::stager::DiskCopy*>& result = instance->diskCopies();
    *len = result.size();
    *var = (castor::stager::DiskCopy**) malloc((*len) * sizeof(castor::stager::DiskCopy*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_addTapeCopies
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_addTapeCopies(castor::stager::CastorFile* instance, castor::stager::TapeCopy* obj) {
    instance->addTapeCopies(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_removeTapeCopies
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_removeTapeCopies(castor::stager::CastorFile* instance, castor::stager::TapeCopy* obj) {
    instance->removeTapeCopies(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_CastorFile_tapeCopies
  //----------------------------------------------------------------------------
  int Cstager_CastorFile_tapeCopies(castor::stager::CastorFile* instance, castor::stager::TapeCopy*** var, int* len) {
    std::vector<castor::stager::TapeCopy*>& result = instance->tapeCopies();
    *len = result.size();
    *var = (castor::stager::TapeCopy**) malloc((*len) * sizeof(castor::stager::TapeCopy*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

} // End of extern "C"

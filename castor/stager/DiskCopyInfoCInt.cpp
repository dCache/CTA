/******************************************************************************
 *                      castor/stager/DiskCopyInfoCInt.cpp
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
#include "castor/stager/DiskCopyInfo.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_create
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_create(castor::stager::DiskCopyInfo** obj) {
    *obj = new castor::stager::DiskCopyInfo();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_delete
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_delete(castor::stager::DiskCopyInfo* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_DiskCopyInfo_getIObject(castor::stager::DiskCopyInfo* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::DiskCopyInfo* Cstager_DiskCopyInfo_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::DiskCopyInfo*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_print
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_print(castor::stager::DiskCopyInfo* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_TYPE
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_TYPE(int* ret) {
    *ret = castor::stager::DiskCopyInfo::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_type
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_type(castor::stager::DiskCopyInfo* instance,
                                int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_clone
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_clone(castor::stager::DiskCopyInfo* instance,
                                 castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_DiskCopyPath
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_DiskCopyPath(castor::stager::DiskCopyInfo* instance, const char** var) {
    *var = instance->DiskCopyPath().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setDiskCopyPath
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setDiskCopyPath(castor::stager::DiskCopyInfo* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setDiskCopyPath(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_size
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_size(castor::stager::DiskCopyInfo* instance, u_signed64* var) {
    *var = instance->size();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setSize
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setSize(castor::stager::DiskCopyInfo* instance, u_signed64 new_var) {
    instance->setSize(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_diskCopyStatus
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_diskCopyStatus(castor::stager::DiskCopyInfo* instance, int* var) {
    *var = instance->diskCopyStatus();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setDiskCopyStatus
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setDiskCopyStatus(castor::stager::DiskCopyInfo* instance, int new_var) {
    instance->setDiskCopyStatus(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_tapeCopyStatus
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_tapeCopyStatus(castor::stager::DiskCopyInfo* instance, int* var) {
    *var = instance->tapeCopyStatus();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setTapeCopyStatus
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setTapeCopyStatus(castor::stager::DiskCopyInfo* instance, int new_var) {
    instance->setTapeCopyStatus(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_segmentStatus
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_segmentStatus(castor::stager::DiskCopyInfo* instance, int* var) {
    *var = instance->segmentStatus();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setSegmentStatus
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setSegmentStatus(castor::stager::DiskCopyInfo* instance, int new_var) {
    instance->setSegmentStatus(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_DiskCopyId
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_DiskCopyId(castor::stager::DiskCopyInfo* instance, u_signed64* var) {
    *var = instance->DiskCopyId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setDiskCopyId
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setDiskCopyId(castor::stager::DiskCopyInfo* instance, u_signed64 new_var) {
    instance->setDiskCopyId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_fileId
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_fileId(castor::stager::DiskCopyInfo* instance, u_signed64* var) {
    *var = instance->fileId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setFileId
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setFileId(castor::stager::DiskCopyInfo* instance, u_signed64 new_var) {
    instance->setFileId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_nsHost
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_nsHost(castor::stager::DiskCopyInfo* instance, const char** var) {
    *var = instance->nsHost().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setNsHost
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setNsHost(castor::stager::DiskCopyInfo* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setNsHost(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_id
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_id(castor::stager::DiskCopyInfo* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskCopyInfo_setId
  //----------------------------------------------------------------------------
  int Cstager_DiskCopyInfo_setId(castor::stager::DiskCopyInfo* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

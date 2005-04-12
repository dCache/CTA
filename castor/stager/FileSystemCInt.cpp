/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/FileSystemCInt.cpp
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
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/DiskPool.hpp"
#include "castor/stager/DiskServer.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/stager/FileSystemStatusCodes.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_create
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_create(castor::stager::FileSystem** obj) {
    *obj = new castor::stager::FileSystem();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_FileSystem_delete
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_delete(castor::stager::FileSystem* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_FileSystem_getIObject(castor::stager::FileSystem* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::FileSystem* Cstager_FileSystem_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::FileSystem*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_print
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_print(castor::stager::FileSystem* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_TYPE
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_TYPE(int* ret) {
    *ret = castor::stager::FileSystem::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_type
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_type(castor::stager::FileSystem* instance,
                              int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_clone
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_clone(castor::stager::FileSystem* instance,
                               castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_free
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_free(castor::stager::FileSystem* instance, u_signed64* var) {
    *var = instance->free();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setFree
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setFree(castor::stager::FileSystem* instance, u_signed64 new_var) {
    instance->setFree(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_weight
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_weight(castor::stager::FileSystem* instance, float* var) {
    *var = instance->weight();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setWeight
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setWeight(castor::stager::FileSystem* instance, float new_var) {
    instance->setWeight(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_fsDeviation
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_fsDeviation(castor::stager::FileSystem* instance, float* var) {
    *var = instance->fsDeviation();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setFsDeviation
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setFsDeviation(castor::stager::FileSystem* instance, float new_var) {
    instance->setFsDeviation(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_mountPoint
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_mountPoint(castor::stager::FileSystem* instance, const char** var) {
    *var = instance->mountPoint().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setMountPoint
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setMountPoint(castor::stager::FileSystem* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMountPoint(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_deltaWeight
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_deltaWeight(castor::stager::FileSystem* instance, float* var) {
    *var = instance->deltaWeight();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setDeltaWeight
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setDeltaWeight(castor::stager::FileSystem* instance, float new_var) {
    instance->setDeltaWeight(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_deltaFree
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_deltaFree(castor::stager::FileSystem* instance, int* var) {
    *var = instance->deltaFree();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setDeltaFree
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setDeltaFree(castor::stager::FileSystem* instance, int new_var) {
    instance->setDeltaFree(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_reservedSpace
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_reservedSpace(castor::stager::FileSystem* instance, int* var) {
    *var = instance->reservedSpace();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setReservedSpace
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setReservedSpace(castor::stager::FileSystem* instance, int new_var) {
    instance->setReservedSpace(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_id
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_id(castor::stager::FileSystem* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setId
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setId(castor::stager::FileSystem* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_diskPool
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_diskPool(castor::stager::FileSystem* instance, castor::stager::DiskPool** var) {
    *var = instance->diskPool();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setDiskPool
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setDiskPool(castor::stager::FileSystem* instance, castor::stager::DiskPool* new_var) {
    instance->setDiskPool(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_addCopies
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_addCopies(castor::stager::FileSystem* instance, castor::stager::DiskCopy* obj) {
    instance->addCopies(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_removeCopies
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_removeCopies(castor::stager::FileSystem* instance, castor::stager::DiskCopy* obj) {
    instance->removeCopies(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_copies
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_copies(castor::stager::FileSystem* instance, castor::stager::DiskCopy*** var, int* len) {
    std::vector<castor::stager::DiskCopy*>& result = instance->copies();
    *len = result.size();
    *var = (castor::stager::DiskCopy**) malloc((*len) * sizeof(castor::stager::DiskCopy*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_diskserver
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_diskserver(castor::stager::FileSystem* instance, castor::stager::DiskServer** var) {
    *var = instance->diskserver();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setDiskserver
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setDiskserver(castor::stager::FileSystem* instance, castor::stager::DiskServer* new_var) {
    instance->setDiskserver(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_status
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_status(castor::stager::FileSystem* instance, castor::stager::FileSystemStatusCodes* var) {
    *var = instance->status();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileSystem_setStatus
  //----------------------------------------------------------------------------
  int Cstager_FileSystem_setStatus(castor::stager::FileSystem* instance, castor::stager::FileSystemStatusCodes new_var) {
    instance->setStatus(new_var);
    return 0;
  }

} // End of extern "C"

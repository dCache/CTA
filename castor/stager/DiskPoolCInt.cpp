/******************************************************************************
 *                      castor/stager/DiskPoolCInt.cpp
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
#include "castor/stager/DiskPool.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_create
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_create(castor::stager::DiskPool** obj) {
    *obj = new castor::stager::DiskPool();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_DiskPool_delete
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_delete(castor::stager::DiskPool* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_DiskPool_getIObject(castor::stager::DiskPool* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::DiskPool* Cstager_DiskPool_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::DiskPool*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_print
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_print(castor::stager::DiskPool* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_TYPE
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_TYPE(int* ret) {
    *ret = castor::stager::DiskPool::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_setId
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_setId(castor::stager::DiskPool* instance,
                             u_signed64 id) {
    instance->setId(id);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_id
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_id(castor::stager::DiskPool* instance,
                          u_signed64* ret) {
    *ret = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_type
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_type(castor::stager::DiskPool* instance,
                            int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_name
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_name(castor::stager::DiskPool* instance, const char** var) {
    *var = instance->name().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_setName
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_setName(castor::stager::DiskPool* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_addFileSystems
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_addFileSystems(castor::stager::DiskPool* instance, castor::stager::FileSystem* obj) {
    instance->addFileSystems(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_removeFileSystems
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_removeFileSystems(castor::stager::DiskPool* instance, castor::stager::FileSystem* obj) {
    instance->removeFileSystems(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_fileSystems
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_fileSystems(castor::stager::DiskPool* instance, castor::stager::FileSystem*** var, int* len) {
    std::vector<castor::stager::FileSystem*>& result = instance->fileSystems();
    *len = result.size();
    *var = (castor::stager::FileSystem**) malloc((*len) * sizeof(castor::stager::FileSystem*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_addSvcClasses
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_addSvcClasses(castor::stager::DiskPool* instance, castor::stager::SvcClass* obj) {
    instance->addSvcClasses(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_removeSvcClasses
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_removeSvcClasses(castor::stager::DiskPool* instance, castor::stager::SvcClass* obj) {
    instance->removeSvcClasses(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_DiskPool_svcClasses
  //----------------------------------------------------------------------------
  int Cstager_DiskPool_svcClasses(castor::stager::DiskPool* instance, castor::stager::SvcClass*** var, int* len) {
    std::vector<castor::stager::SvcClass*>& result = instance->svcClasses();
    *len = result.size();
    *var = (castor::stager::SvcClass**) malloc((*len) * sizeof(castor::stager::SvcClass*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

} // End of extern "C"

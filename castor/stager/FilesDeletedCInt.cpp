/******************************************************************************
 *                      castor/stager/FilesDeletedCInt.cpp
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
#include "castor/IClient.hpp"
#include "castor/IObject.hpp"
#include "castor/stager/FilesDeleted.hpp"
#include "castor/stager/GCRemovedFile.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_create
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_create(castor::stager::FilesDeleted** obj) {
    *obj = new castor::stager::FilesDeleted();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_delete
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_delete(castor::stager::FilesDeleted* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_FilesDeleted_getRequest(castor::stager::FilesDeleted* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::FilesDeleted* Cstager_FilesDeleted_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::FilesDeleted*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_FilesDeleted_getIObject(castor::stager::FilesDeleted* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::FilesDeleted* Cstager_FilesDeleted_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::FilesDeleted*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_print
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_print(castor::stager::FilesDeleted* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_TYPE
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_TYPE(int* ret) {
    *ret = castor::stager::FilesDeleted::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_flags
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_flags(castor::stager::FilesDeleted* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setFlags
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setFlags(castor::stager::FilesDeleted* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_userName
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_userName(castor::stager::FilesDeleted* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setUserName
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setUserName(castor::stager::FilesDeleted* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_euid
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_euid(castor::stager::FilesDeleted* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setEuid
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setEuid(castor::stager::FilesDeleted* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_egid
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_egid(castor::stager::FilesDeleted* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setEgid
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setEgid(castor::stager::FilesDeleted* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_mask
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_mask(castor::stager::FilesDeleted* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setMask
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setMask(castor::stager::FilesDeleted* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_pid
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_pid(castor::stager::FilesDeleted* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setPid
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setPid(castor::stager::FilesDeleted* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_machine
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_machine(castor::stager::FilesDeleted* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setMachine
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setMachine(castor::stager::FilesDeleted* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_svcClassName(castor::stager::FilesDeleted* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setSvcClassName(castor::stager::FilesDeleted* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_userTag
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_userTag(castor::stager::FilesDeleted* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setUserTag(castor::stager::FilesDeleted* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_reqId
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_reqId(castor::stager::FilesDeleted* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setReqId
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setReqId(castor::stager::FilesDeleted* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_creationTime
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_creationTime(castor::stager::FilesDeleted* instance, u_signed64* var) {
    *var = instance->creationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setCreationTime
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setCreationTime(castor::stager::FilesDeleted* instance, u_signed64 new_var) {
    instance->setCreationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_lastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_lastModificationTime(castor::stager::FilesDeleted* instance, u_signed64* var) {
    *var = instance->lastModificationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setLastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setLastModificationTime(castor::stager::FilesDeleted* instance, u_signed64 new_var) {
    instance->setLastModificationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_svcClass
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_svcClass(castor::stager::FilesDeleted* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setSvcClass(castor::stager::FilesDeleted* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_client
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_client(castor::stager::FilesDeleted* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setClient
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setClient(castor::stager::FilesDeleted* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_type
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_type(castor::stager::FilesDeleted* instance,
                                int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_clone
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_clone(castor::stager::FilesDeleted* instance,
                                 castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_id
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_id(castor::stager::FilesDeleted* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_setId
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_setId(castor::stager::FilesDeleted* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_addFiles
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_addFiles(castor::stager::FilesDeleted* instance, castor::stager::GCRemovedFile* obj) {
    instance->addFiles(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_removeFiles
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_removeFiles(castor::stager::FilesDeleted* instance, castor::stager::GCRemovedFile* obj) {
    instance->removeFiles(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FilesDeleted_files
  //----------------------------------------------------------------------------
  int Cstager_FilesDeleted_files(castor::stager::FilesDeleted* instance, castor::stager::GCRemovedFile*** var, int* len) {
    std::vector<castor::stager::GCRemovedFile*>& result = instance->files();
    *len = result.size();
    *var = (castor::stager::GCRemovedFile**) malloc((*len) * sizeof(castor::stager::GCRemovedFile*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

} // End of extern "C"

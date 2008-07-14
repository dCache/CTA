/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/SetFileGCWeightCInt.cpp
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
#include "castor/stager/FileRequest.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/SetFileGCWeight.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_create
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_create(castor::stager::SetFileGCWeight** obj) {
    *obj = new castor::stager::SetFileGCWeight();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_delete
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_delete(castor::stager::SetFileGCWeight* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_getFileRequest
  //----------------------------------------------------------------------------
  castor::stager::FileRequest* Cstager_SetFileGCWeight_getFileRequest(castor::stager::SetFileGCWeight* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_fromFileRequest
  //----------------------------------------------------------------------------
  castor::stager::SetFileGCWeight* Cstager_SetFileGCWeight_fromFileRequest(castor::stager::FileRequest* obj) {
    return dynamic_cast<castor::stager::SetFileGCWeight*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_SetFileGCWeight_getRequest(castor::stager::SetFileGCWeight* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::SetFileGCWeight* Cstager_SetFileGCWeight_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::SetFileGCWeight*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_SetFileGCWeight_getIObject(castor::stager::SetFileGCWeight* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::SetFileGCWeight* Cstager_SetFileGCWeight_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::SetFileGCWeight*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_print
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_print(castor::stager::SetFileGCWeight* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_TYPE
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_TYPE(int* ret) {
    *ret = castor::stager::SetFileGCWeight::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_addSubRequests
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_addSubRequests(castor::stager::SetFileGCWeight* instance, castor::stager::SubRequest* obj) {
    instance->addSubRequests(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_removeSubRequests
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_removeSubRequests(castor::stager::SetFileGCWeight* instance, castor::stager::SubRequest* obj) {
    instance->removeSubRequests(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_subRequests
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_subRequests(castor::stager::SetFileGCWeight* instance, castor::stager::SubRequest*** var, int* len) {
    std::vector<castor::stager::SubRequest*>& result = instance->subRequests();
    *len = result.size();
    *var = (castor::stager::SubRequest**) malloc((*len) * sizeof(castor::stager::SubRequest*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_flags
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_flags(castor::stager::SetFileGCWeight* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setFlags
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setFlags(castor::stager::SetFileGCWeight* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_userName
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_userName(castor::stager::SetFileGCWeight* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setUserName
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setUserName(castor::stager::SetFileGCWeight* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_euid
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_euid(castor::stager::SetFileGCWeight* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setEuid
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setEuid(castor::stager::SetFileGCWeight* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_egid
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_egid(castor::stager::SetFileGCWeight* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setEgid
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setEgid(castor::stager::SetFileGCWeight* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_mask
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_mask(castor::stager::SetFileGCWeight* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setMask
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setMask(castor::stager::SetFileGCWeight* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_pid
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_pid(castor::stager::SetFileGCWeight* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setPid
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setPid(castor::stager::SetFileGCWeight* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_machine
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_machine(castor::stager::SetFileGCWeight* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setMachine
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setMachine(castor::stager::SetFileGCWeight* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_svcClassName(castor::stager::SetFileGCWeight* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setSvcClassName(castor::stager::SetFileGCWeight* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_userTag
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_userTag(castor::stager::SetFileGCWeight* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setUserTag(castor::stager::SetFileGCWeight* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_reqId
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_reqId(castor::stager::SetFileGCWeight* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setReqId
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setReqId(castor::stager::SetFileGCWeight* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_creationTime
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_creationTime(castor::stager::SetFileGCWeight* instance, u_signed64* var) {
    *var = instance->creationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setCreationTime
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setCreationTime(castor::stager::SetFileGCWeight* instance, u_signed64 new_var) {
    instance->setCreationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_lastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_lastModificationTime(castor::stager::SetFileGCWeight* instance, u_signed64* var) {
    *var = instance->lastModificationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setLastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setLastModificationTime(castor::stager::SetFileGCWeight* instance, u_signed64 new_var) {
    instance->setLastModificationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_svcClass
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_svcClass(castor::stager::SetFileGCWeight* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setSvcClass(castor::stager::SetFileGCWeight* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_client
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_client(castor::stager::SetFileGCWeight* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setClient
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setClient(castor::stager::SetFileGCWeight* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_type
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_type(castor::stager::SetFileGCWeight* instance,
                                   int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_clone
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_clone(castor::stager::SetFileGCWeight* instance,
                                    castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_weight
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_weight(castor::stager::SetFileGCWeight* instance, double* var) {
    *var = instance->weight();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setWeight
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setWeight(castor::stager::SetFileGCWeight* instance, double new_var) {
    instance->setWeight(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_id
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_id(castor::stager::SetFileGCWeight* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_SetFileGCWeight_setId
  //----------------------------------------------------------------------------
  int Cstager_SetFileGCWeight_setId(castor::stager::SetFileGCWeight* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

/******************************************************************************
 *                      castor/stager/StageRmRequestCInt.cpp
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
#include "castor/stager/StageRmRequest.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_create
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_create(castor::stager::StageRmRequest** obj) {
    *obj = new castor::stager::StageRmRequest();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_delete
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_delete(castor::stager::StageRmRequest* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_getFileRequest
  //----------------------------------------------------------------------------
  castor::stager::FileRequest* Cstager_StageRmRequest_getFileRequest(castor::stager::StageRmRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_fromFileRequest
  //----------------------------------------------------------------------------
  castor::stager::StageRmRequest* Cstager_StageRmRequest_fromFileRequest(castor::stager::FileRequest* obj) {
    return dynamic_cast<castor::stager::StageRmRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_StageRmRequest_getRequest(castor::stager::StageRmRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::StageRmRequest* Cstager_StageRmRequest_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::StageRmRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_StageRmRequest_getIObject(castor::stager::StageRmRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::StageRmRequest* Cstager_StageRmRequest_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::StageRmRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_print
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_print(castor::stager::StageRmRequest* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_TYPE
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_TYPE(int* ret) {
    *ret = castor::stager::StageRmRequest::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_addSubRequests
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_addSubRequests(castor::stager::StageRmRequest* instance, castor::stager::SubRequest* obj) {
    instance->addSubRequests(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_removeSubRequests
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_removeSubRequests(castor::stager::StageRmRequest* instance, castor::stager::SubRequest* obj) {
    instance->removeSubRequests(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_subRequests
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_subRequests(castor::stager::StageRmRequest* instance, castor::stager::SubRequest*** var, int* len) {
    std::vector<castor::stager::SubRequest*> result = instance->subRequests();
    *len = result.size();
    *var = (castor::stager::SubRequest**) malloc((*len) * sizeof(castor::stager::SubRequest*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_flags
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_flags(castor::stager::StageRmRequest* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setFlags
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setFlags(castor::stager::StageRmRequest* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_userName
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_userName(castor::stager::StageRmRequest* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setUserName
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setUserName(castor::stager::StageRmRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_euid
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_euid(castor::stager::StageRmRequest* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setEuid
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setEuid(castor::stager::StageRmRequest* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_egid
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_egid(castor::stager::StageRmRequest* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setEgid
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setEgid(castor::stager::StageRmRequest* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_mask
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_mask(castor::stager::StageRmRequest* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setMask
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setMask(castor::stager::StageRmRequest* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_pid
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_pid(castor::stager::StageRmRequest* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setPid
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setPid(castor::stager::StageRmRequest* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_machine
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_machine(castor::stager::StageRmRequest* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setMachine
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setMachine(castor::stager::StageRmRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_svcClassName(castor::stager::StageRmRequest* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setSvcClassName(castor::stager::StageRmRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_userTag
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_userTag(castor::stager::StageRmRequest* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setUserTag(castor::stager::StageRmRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_reqId
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_reqId(castor::stager::StageRmRequest* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setReqId
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setReqId(castor::stager::StageRmRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_svcClass
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_svcClass(castor::stager::StageRmRequest* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setSvcClass(castor::stager::StageRmRequest* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_client
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_client(castor::stager::StageRmRequest* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setClient
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setClient(castor::stager::StageRmRequest* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_setId
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_setId(castor::stager::StageRmRequest* instance,
                                   u_signed64 id) {
    instance->setId(id);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_id
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_id(castor::stager::StageRmRequest* instance,
                                u_signed64* ret) {
    *ret = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageRmRequest_type
  //----------------------------------------------------------------------------
  int Cstager_StageRmRequest_type(castor::stager::StageRmRequest* instance,
                                  int* ret) {
    *ret = instance->type();
    return 0;
  }

} // End of extern "C"

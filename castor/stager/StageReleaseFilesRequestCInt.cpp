/******************************************************************************
 *                      castor/stager/StageReleaseFilesRequestCInt.cpp
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
#include "castor/stager/StageReleaseFilesRequest.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_create
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_create(castor::stager::StageReleaseFilesRequest** obj) {
    *obj = new castor::stager::StageReleaseFilesRequest();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_delete
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_delete(castor::stager::StageReleaseFilesRequest* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_getFileRequest
  //----------------------------------------------------------------------------
  castor::stager::FileRequest* Cstager_StageReleaseFilesRequest_getFileRequest(castor::stager::StageReleaseFilesRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_fromFileRequest
  //----------------------------------------------------------------------------
  castor::stager::StageReleaseFilesRequest* Cstager_StageReleaseFilesRequest_fromFileRequest(castor::stager::FileRequest* obj) {
    return dynamic_cast<castor::stager::StageReleaseFilesRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_StageReleaseFilesRequest_getRequest(castor::stager::StageReleaseFilesRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::StageReleaseFilesRequest* Cstager_StageReleaseFilesRequest_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::StageReleaseFilesRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_StageReleaseFilesRequest_getIObject(castor::stager::StageReleaseFilesRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::StageReleaseFilesRequest* Cstager_StageReleaseFilesRequest_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::StageReleaseFilesRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_print
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_print(castor::stager::StageReleaseFilesRequest* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_TYPE
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_TYPE(int* ret) {
    *ret = castor::stager::StageReleaseFilesRequest::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_addSubRequests
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_addSubRequests(castor::stager::StageReleaseFilesRequest* instance, castor::stager::SubRequest* obj) {
    instance->addSubRequests(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_removeSubRequests
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_removeSubRequests(castor::stager::StageReleaseFilesRequest* instance, castor::stager::SubRequest* obj) {
    instance->removeSubRequests(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_subRequests
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_subRequests(castor::stager::StageReleaseFilesRequest* instance, castor::stager::SubRequest*** var, int* len) {
    std::vector<castor::stager::SubRequest*>& result = instance->subRequests();
    *len = result.size();
    *var = (castor::stager::SubRequest**) malloc((*len) * sizeof(castor::stager::SubRequest*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_flags
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_flags(castor::stager::StageReleaseFilesRequest* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setFlags
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setFlags(castor::stager::StageReleaseFilesRequest* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_userName
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_userName(castor::stager::StageReleaseFilesRequest* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setUserName
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setUserName(castor::stager::StageReleaseFilesRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_euid
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_euid(castor::stager::StageReleaseFilesRequest* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setEuid
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setEuid(castor::stager::StageReleaseFilesRequest* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_egid
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_egid(castor::stager::StageReleaseFilesRequest* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setEgid
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setEgid(castor::stager::StageReleaseFilesRequest* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_mask
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_mask(castor::stager::StageReleaseFilesRequest* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setMask
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setMask(castor::stager::StageReleaseFilesRequest* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_pid
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_pid(castor::stager::StageReleaseFilesRequest* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setPid
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setPid(castor::stager::StageReleaseFilesRequest* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_machine
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_machine(castor::stager::StageReleaseFilesRequest* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setMachine
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setMachine(castor::stager::StageReleaseFilesRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_svcClassName(castor::stager::StageReleaseFilesRequest* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setSvcClassName(castor::stager::StageReleaseFilesRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_userTag
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_userTag(castor::stager::StageReleaseFilesRequest* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setUserTag(castor::stager::StageReleaseFilesRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_reqId
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_reqId(castor::stager::StageReleaseFilesRequest* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setReqId
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setReqId(castor::stager::StageReleaseFilesRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_svcClass
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_svcClass(castor::stager::StageReleaseFilesRequest* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setSvcClass(castor::stager::StageReleaseFilesRequest* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_client
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_client(castor::stager::StageReleaseFilesRequest* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setClient
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setClient(castor::stager::StageReleaseFilesRequest* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_setId
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_setId(castor::stager::StageReleaseFilesRequest* instance,
                                             u_signed64 id) {
    instance->setId(id);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_id
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_id(castor::stager::StageReleaseFilesRequest* instance,
                                          u_signed64* ret) {
    *ret = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_type
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_type(castor::stager::StageReleaseFilesRequest* instance,
                                            int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StageReleaseFilesRequest_clone
  //----------------------------------------------------------------------------
  int Cstager_StageReleaseFilesRequest_clone(castor::stager::StageReleaseFilesRequest* instance,
                                             castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

} // End of extern "C"

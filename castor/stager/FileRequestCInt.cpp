/******************************************************************************
 *                      castor/stager/FileRequestCInt.cpp
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
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_delete
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_delete(castor::stager::FileRequest* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_FileRequest_getRequest(castor::stager::FileRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::FileRequest* Cstager_FileRequest_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::FileRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_FileRequest_getIObject(castor::stager::FileRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::FileRequest* Cstager_FileRequest_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::FileRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_print
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_print(castor::stager::FileRequest* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_flags
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_flags(castor::stager::FileRequest* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setFlags
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setFlags(castor::stager::FileRequest* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_userName
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_userName(castor::stager::FileRequest* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setUserName
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setUserName(castor::stager::FileRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_euid
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_euid(castor::stager::FileRequest* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setEuid
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setEuid(castor::stager::FileRequest* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_egid
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_egid(castor::stager::FileRequest* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setEgid
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setEgid(castor::stager::FileRequest* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_mask
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_mask(castor::stager::FileRequest* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setMask
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setMask(castor::stager::FileRequest* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_pid
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_pid(castor::stager::FileRequest* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setPid
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setPid(castor::stager::FileRequest* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_machine
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_machine(castor::stager::FileRequest* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setMachine
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setMachine(castor::stager::FileRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_svcClassName(castor::stager::FileRequest* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setSvcClassName(castor::stager::FileRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_userTag
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_userTag(castor::stager::FileRequest* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setUserTag(castor::stager::FileRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_reqId
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_reqId(castor::stager::FileRequest* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setReqId
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setReqId(castor::stager::FileRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_svcClass
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_svcClass(castor::stager::FileRequest* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setSvcClass(castor::stager::FileRequest* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_client
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_client(castor::stager::FileRequest* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_setClient
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_setClient(castor::stager::FileRequest* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_addSubRequests
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_addSubRequests(castor::stager::FileRequest* instance, castor::stager::SubRequest* obj) {
    instance->addSubRequests(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_removeSubRequests
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_removeSubRequests(castor::stager::FileRequest* instance, castor::stager::SubRequest* obj) {
    instance->removeSubRequests(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_FileRequest_subRequests
  //----------------------------------------------------------------------------
  int Cstager_FileRequest_subRequests(castor::stager::FileRequest* instance, castor::stager::SubRequest*** var, int* len) {
    std::vector<castor::stager::SubRequest*>& result = instance->subRequests();
    *len = result.size();
    *var = (castor::stager::SubRequest**) malloc((*len) * sizeof(castor::stager::SubRequest*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

} // End of extern "C"

/******************************************************************************
 *                      castor/stager/StagePutNextRequestCInt.cpp
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
#include "castor/stager/ReqIdRequest.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/StagePutNextRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_create
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_create(castor::stager::StagePutNextRequest** obj) {
    *obj = new castor::stager::StagePutNextRequest();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_delete
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_delete(castor::stager::StagePutNextRequest* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_getReqIdRequest
  //----------------------------------------------------------------------------
  castor::stager::ReqIdRequest* Cstager_StagePutNextRequest_getReqIdRequest(castor::stager::StagePutNextRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_fromReqIdRequest
  //----------------------------------------------------------------------------
  castor::stager::StagePutNextRequest* Cstager_StagePutNextRequest_fromReqIdRequest(castor::stager::ReqIdRequest* obj) {
    return dynamic_cast<castor::stager::StagePutNextRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_StagePutNextRequest_getRequest(castor::stager::StagePutNextRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::StagePutNextRequest* Cstager_StagePutNextRequest_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::StagePutNextRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_StagePutNextRequest_getIObject(castor::stager::StagePutNextRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::StagePutNextRequest* Cstager_StagePutNextRequest_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::StagePutNextRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_print
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_print(castor::stager::StagePutNextRequest* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_TYPE
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_TYPE(int* ret) {
    *ret = castor::stager::StagePutNextRequest::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_parent
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_parent(castor::stager::StagePutNextRequest* instance, castor::stager::FileRequest** var) {
    *var = instance->parent();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setParent
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setParent(castor::stager::StagePutNextRequest* instance, castor::stager::FileRequest* new_var) {
    instance->setParent(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_flags
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_flags(castor::stager::StagePutNextRequest* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setFlags
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setFlags(castor::stager::StagePutNextRequest* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_userName
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_userName(castor::stager::StagePutNextRequest* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setUserName
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setUserName(castor::stager::StagePutNextRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_euid
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_euid(castor::stager::StagePutNextRequest* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setEuid
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setEuid(castor::stager::StagePutNextRequest* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_egid
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_egid(castor::stager::StagePutNextRequest* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setEgid
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setEgid(castor::stager::StagePutNextRequest* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_mask
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_mask(castor::stager::StagePutNextRequest* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setMask
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setMask(castor::stager::StagePutNextRequest* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_pid
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_pid(castor::stager::StagePutNextRequest* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setPid
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setPid(castor::stager::StagePutNextRequest* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_machine
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_machine(castor::stager::StagePutNextRequest* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setMachine
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setMachine(castor::stager::StagePutNextRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_svcClassName(castor::stager::StagePutNextRequest* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setSvcClassName(castor::stager::StagePutNextRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_userTag
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_userTag(castor::stager::StagePutNextRequest* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setUserTag(castor::stager::StagePutNextRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_reqId
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_reqId(castor::stager::StagePutNextRequest* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setReqId
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setReqId(castor::stager::StagePutNextRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_svcClass
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_svcClass(castor::stager::StagePutNextRequest* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setSvcClass(castor::stager::StagePutNextRequest* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_client
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_client(castor::stager::StagePutNextRequest* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setClient
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setClient(castor::stager::StagePutNextRequest* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_type
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_type(castor::stager::StagePutNextRequest* instance,
                                       int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_clone
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_clone(castor::stager::StagePutNextRequest* instance,
                                        castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_id
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_id(castor::stager::StagePutNextRequest* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StagePutNextRequest_setId
  //----------------------------------------------------------------------------
  int Cstager_StagePutNextRequest_setId(castor::stager::StagePutNextRequest* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

/******************************************************************************
 *                      castor/stager/MoverCloseRequestCInt.cpp
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
#include "castor/stager/MoverCloseRequest.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_create
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_create(castor::stager::MoverCloseRequest** obj) {
    *obj = new castor::stager::MoverCloseRequest();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_delete
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_delete(castor::stager::MoverCloseRequest* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_MoverCloseRequest_getRequest(castor::stager::MoverCloseRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::MoverCloseRequest* Cstager_MoverCloseRequest_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::MoverCloseRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_MoverCloseRequest_getIObject(castor::stager::MoverCloseRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::MoverCloseRequest* Cstager_MoverCloseRequest_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::MoverCloseRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_print
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_print(castor::stager::MoverCloseRequest* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_TYPE
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_TYPE(int* ret) {
    *ret = castor::stager::MoverCloseRequest::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_flags
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_flags(castor::stager::MoverCloseRequest* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setFlags
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setFlags(castor::stager::MoverCloseRequest* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_userName
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_userName(castor::stager::MoverCloseRequest* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setUserName
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setUserName(castor::stager::MoverCloseRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_euid
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_euid(castor::stager::MoverCloseRequest* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setEuid
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setEuid(castor::stager::MoverCloseRequest* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_egid
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_egid(castor::stager::MoverCloseRequest* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setEgid
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setEgid(castor::stager::MoverCloseRequest* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_mask
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_mask(castor::stager::MoverCloseRequest* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setMask
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setMask(castor::stager::MoverCloseRequest* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_pid
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_pid(castor::stager::MoverCloseRequest* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setPid
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setPid(castor::stager::MoverCloseRequest* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_machine
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_machine(castor::stager::MoverCloseRequest* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setMachine
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setMachine(castor::stager::MoverCloseRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_svcClassName(castor::stager::MoverCloseRequest* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setSvcClassName(castor::stager::MoverCloseRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_userTag
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_userTag(castor::stager::MoverCloseRequest* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setUserTag(castor::stager::MoverCloseRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_reqId
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_reqId(castor::stager::MoverCloseRequest* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setReqId
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setReqId(castor::stager::MoverCloseRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_svcClass
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_svcClass(castor::stager::MoverCloseRequest* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setSvcClass(castor::stager::MoverCloseRequest* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_client
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_client(castor::stager::MoverCloseRequest* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setClient
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setClient(castor::stager::MoverCloseRequest* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_type
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_type(castor::stager::MoverCloseRequest* instance,
                                     int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_clone
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_clone(castor::stager::MoverCloseRequest* instance,
                                      castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_subReqId
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_subReqId(castor::stager::MoverCloseRequest* instance, u_signed64* var) {
    *var = instance->subReqId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setSubReqId
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setSubReqId(castor::stager::MoverCloseRequest* instance, u_signed64 new_var) {
    instance->setSubReqId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_id
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_id(castor::stager::MoverCloseRequest* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_MoverCloseRequest_setId
  //----------------------------------------------------------------------------
  int Cstager_MoverCloseRequest_setId(castor::stager::MoverCloseRequest* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

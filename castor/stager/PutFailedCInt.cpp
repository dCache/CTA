/******************************************************************************
 *                      castor/stager/PutFailedCInt.cpp
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
#include "castor/stager/PutFailed.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_create
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_create(castor::stager::PutFailed** obj) {
    *obj = new castor::stager::PutFailed();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_PutFailed_delete
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_delete(castor::stager::PutFailed* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_PutFailed_getRequest(castor::stager::PutFailed* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::PutFailed* Cstager_PutFailed_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::PutFailed*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_PutFailed_getIObject(castor::stager::PutFailed* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::PutFailed* Cstager_PutFailed_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::PutFailed*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_print
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_print(castor::stager::PutFailed* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_TYPE
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_TYPE(int* ret) {
    *ret = castor::stager::PutFailed::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_flags
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_flags(castor::stager::PutFailed* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setFlags
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setFlags(castor::stager::PutFailed* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_userName
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_userName(castor::stager::PutFailed* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setUserName
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setUserName(castor::stager::PutFailed* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_euid
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_euid(castor::stager::PutFailed* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setEuid
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setEuid(castor::stager::PutFailed* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_egid
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_egid(castor::stager::PutFailed* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setEgid
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setEgid(castor::stager::PutFailed* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_mask
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_mask(castor::stager::PutFailed* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setMask
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setMask(castor::stager::PutFailed* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_pid
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_pid(castor::stager::PutFailed* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setPid
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setPid(castor::stager::PutFailed* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_machine
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_machine(castor::stager::PutFailed* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setMachine
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setMachine(castor::stager::PutFailed* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_svcClassName(castor::stager::PutFailed* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setSvcClassName(castor::stager::PutFailed* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_userTag
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_userTag(castor::stager::PutFailed* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setUserTag(castor::stager::PutFailed* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_reqId
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_reqId(castor::stager::PutFailed* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setReqId
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setReqId(castor::stager::PutFailed* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_creationTime
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_creationTime(castor::stager::PutFailed* instance, u_signed64* var) {
    *var = instance->creationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setCreationTime
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setCreationTime(castor::stager::PutFailed* instance, u_signed64 new_var) {
    instance->setCreationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_lastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_lastModificationTime(castor::stager::PutFailed* instance, u_signed64* var) {
    *var = instance->lastModificationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setLastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setLastModificationTime(castor::stager::PutFailed* instance, u_signed64 new_var) {
    instance->setLastModificationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_svcClass
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_svcClass(castor::stager::PutFailed* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setSvcClass(castor::stager::PutFailed* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_client
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_client(castor::stager::PutFailed* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setClient
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setClient(castor::stager::PutFailed* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_type
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_type(castor::stager::PutFailed* instance,
                             int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_clone
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_clone(castor::stager::PutFailed* instance,
                              castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_subReqId
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_subReqId(castor::stager::PutFailed* instance, u_signed64* var) {
    *var = instance->subReqId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setSubReqId
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setSubReqId(castor::stager::PutFailed* instance, u_signed64 new_var) {
    instance->setSubReqId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_id
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_id(castor::stager::PutFailed* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_PutFailed_setId
  //----------------------------------------------------------------------------
  int Cstager_PutFailed_setId(castor::stager::PutFailed* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

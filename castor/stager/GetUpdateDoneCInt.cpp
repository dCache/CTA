/******************************************************************************
 *                      castor/stager/GetUpdateDoneCInt.cpp
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
#include "castor/stager/GetUpdateDone.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_create
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_create(castor::stager::GetUpdateDone** obj) {
    *obj = new castor::stager::GetUpdateDone();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_delete
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_delete(castor::stager::GetUpdateDone* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_GetUpdateDone_getRequest(castor::stager::GetUpdateDone* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::GetUpdateDone* Cstager_GetUpdateDone_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::GetUpdateDone*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_GetUpdateDone_getIObject(castor::stager::GetUpdateDone* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::GetUpdateDone* Cstager_GetUpdateDone_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::GetUpdateDone*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_print
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_print(castor::stager::GetUpdateDone* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_TYPE
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_TYPE(int* ret) {
    *ret = castor::stager::GetUpdateDone::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_flags
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_flags(castor::stager::GetUpdateDone* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setFlags
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setFlags(castor::stager::GetUpdateDone* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_userName
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_userName(castor::stager::GetUpdateDone* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setUserName
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setUserName(castor::stager::GetUpdateDone* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_euid
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_euid(castor::stager::GetUpdateDone* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setEuid
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setEuid(castor::stager::GetUpdateDone* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_egid
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_egid(castor::stager::GetUpdateDone* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setEgid
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setEgid(castor::stager::GetUpdateDone* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_mask
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_mask(castor::stager::GetUpdateDone* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setMask
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setMask(castor::stager::GetUpdateDone* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_pid
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_pid(castor::stager::GetUpdateDone* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setPid
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setPid(castor::stager::GetUpdateDone* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_machine
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_machine(castor::stager::GetUpdateDone* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setMachine
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setMachine(castor::stager::GetUpdateDone* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_svcClassName(castor::stager::GetUpdateDone* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setSvcClassName(castor::stager::GetUpdateDone* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_userTag
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_userTag(castor::stager::GetUpdateDone* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setUserTag(castor::stager::GetUpdateDone* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_reqId
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_reqId(castor::stager::GetUpdateDone* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setReqId
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setReqId(castor::stager::GetUpdateDone* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_creationTime
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_creationTime(castor::stager::GetUpdateDone* instance, u_signed64* var) {
    *var = instance->creationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setCreationTime
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setCreationTime(castor::stager::GetUpdateDone* instance, u_signed64 new_var) {
    instance->setCreationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_lastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_lastModificationTime(castor::stager::GetUpdateDone* instance, u_signed64* var) {
    *var = instance->lastModificationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setLastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setLastModificationTime(castor::stager::GetUpdateDone* instance, u_signed64 new_var) {
    instance->setLastModificationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_svcClass
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_svcClass(castor::stager::GetUpdateDone* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setSvcClass(castor::stager::GetUpdateDone* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_client
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_client(castor::stager::GetUpdateDone* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setClient
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setClient(castor::stager::GetUpdateDone* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_type
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_type(castor::stager::GetUpdateDone* instance,
                                 int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_clone
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_clone(castor::stager::GetUpdateDone* instance,
                                  castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_subReqId
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_subReqId(castor::stager::GetUpdateDone* instance, u_signed64* var) {
    *var = instance->subReqId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setSubReqId
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setSubReqId(castor::stager::GetUpdateDone* instance, u_signed64 new_var) {
    instance->setSubReqId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_id
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_id(castor::stager::GetUpdateDone* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_GetUpdateDone_setId
  //----------------------------------------------------------------------------
  int Cstager_GetUpdateDone_setId(castor::stager::GetUpdateDone* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

/******************************************************************************
 *                      castor/stager/Disk2DiskCopyDoneRequestCInt.cpp
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
#include "castor/stager/Disk2DiskCopyDoneRequest.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_create
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_create(castor::stager::Disk2DiskCopyDoneRequest** obj) {
    *obj = new castor::stager::Disk2DiskCopyDoneRequest();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_delete
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_delete(castor::stager::Disk2DiskCopyDoneRequest* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_Disk2DiskCopyDoneRequest_getRequest(castor::stager::Disk2DiskCopyDoneRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::Disk2DiskCopyDoneRequest* Cstager_Disk2DiskCopyDoneRequest_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::Disk2DiskCopyDoneRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_Disk2DiskCopyDoneRequest_getIObject(castor::stager::Disk2DiskCopyDoneRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::Disk2DiskCopyDoneRequest* Cstager_Disk2DiskCopyDoneRequest_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::Disk2DiskCopyDoneRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_print
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_print(castor::stager::Disk2DiskCopyDoneRequest* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_TYPE
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_TYPE(int* ret) {
    *ret = castor::stager::Disk2DiskCopyDoneRequest::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_flags
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_flags(castor::stager::Disk2DiskCopyDoneRequest* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setFlags
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setFlags(castor::stager::Disk2DiskCopyDoneRequest* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_userName
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_userName(castor::stager::Disk2DiskCopyDoneRequest* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setUserName
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setUserName(castor::stager::Disk2DiskCopyDoneRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_euid
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_euid(castor::stager::Disk2DiskCopyDoneRequest* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setEuid
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setEuid(castor::stager::Disk2DiskCopyDoneRequest* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_egid
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_egid(castor::stager::Disk2DiskCopyDoneRequest* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setEgid
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setEgid(castor::stager::Disk2DiskCopyDoneRequest* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_mask
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_mask(castor::stager::Disk2DiskCopyDoneRequest* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setMask
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setMask(castor::stager::Disk2DiskCopyDoneRequest* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_pid
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_pid(castor::stager::Disk2DiskCopyDoneRequest* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setPid
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setPid(castor::stager::Disk2DiskCopyDoneRequest* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_machine
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_machine(castor::stager::Disk2DiskCopyDoneRequest* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setMachine
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setMachine(castor::stager::Disk2DiskCopyDoneRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_svcClassName(castor::stager::Disk2DiskCopyDoneRequest* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setSvcClassName(castor::stager::Disk2DiskCopyDoneRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_userTag
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_userTag(castor::stager::Disk2DiskCopyDoneRequest* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setUserTag(castor::stager::Disk2DiskCopyDoneRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_reqId
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_reqId(castor::stager::Disk2DiskCopyDoneRequest* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setReqId
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setReqId(castor::stager::Disk2DiskCopyDoneRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_svcClass
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_svcClass(castor::stager::Disk2DiskCopyDoneRequest* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setSvcClass(castor::stager::Disk2DiskCopyDoneRequest* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_client
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_client(castor::stager::Disk2DiskCopyDoneRequest* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setClient
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setClient(castor::stager::Disk2DiskCopyDoneRequest* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_type
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_type(castor::stager::Disk2DiskCopyDoneRequest* instance,
                                            int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_clone
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_clone(castor::stager::Disk2DiskCopyDoneRequest* instance,
                                             castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_diskCopyId
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_diskCopyId(castor::stager::Disk2DiskCopyDoneRequest* instance, u_signed64* var) {
    *var = instance->diskCopyId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setDiskCopyId
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setDiskCopyId(castor::stager::Disk2DiskCopyDoneRequest* instance, u_signed64 new_var) {
    instance->setDiskCopyId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_status
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_status(castor::stager::Disk2DiskCopyDoneRequest* instance, int* var) {
    *var = instance->status();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setStatus
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setStatus(castor::stager::Disk2DiskCopyDoneRequest* instance, int new_var) {
    instance->setStatus(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_id
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_id(castor::stager::Disk2DiskCopyDoneRequest* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_Disk2DiskCopyDoneRequest_setId
  //----------------------------------------------------------------------------
  int Cstager_Disk2DiskCopyDoneRequest_setId(castor::stager::Disk2DiskCopyDoneRequest* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

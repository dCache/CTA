/******************************************************************************
 *                      castor/stager/StartRequestCInt.cpp
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
#include "castor/stager/Request.hpp"
#include "castor/stager/StartRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_delete
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_delete(castor::stager::StartRequest* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_getRequest
  //----------------------------------------------------------------------------
  castor::stager::Request* Cstager_StartRequest_getRequest(castor::stager::StartRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_fromRequest
  //----------------------------------------------------------------------------
  castor::stager::StartRequest* Cstager_StartRequest_fromRequest(castor::stager::Request* obj) {
    return dynamic_cast<castor::stager::StartRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_StartRequest_getIObject(castor::stager::StartRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::StartRequest* Cstager_StartRequest_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::StartRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_print
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_print(castor::stager::StartRequest* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_flags
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_flags(castor::stager::StartRequest* instance, u_signed64* var) {
    *var = instance->flags();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setFlags
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setFlags(castor::stager::StartRequest* instance, u_signed64 new_var) {
    instance->setFlags(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_userName
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_userName(castor::stager::StartRequest* instance, const char** var) {
    *var = instance->userName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setUserName
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setUserName(castor::stager::StartRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_euid
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_euid(castor::stager::StartRequest* instance, unsigned long* var) {
    *var = instance->euid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setEuid
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setEuid(castor::stager::StartRequest* instance, unsigned long new_var) {
    instance->setEuid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_egid
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_egid(castor::stager::StartRequest* instance, unsigned long* var) {
    *var = instance->egid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setEgid
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setEgid(castor::stager::StartRequest* instance, unsigned long new_var) {
    instance->setEgid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_mask
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_mask(castor::stager::StartRequest* instance, unsigned long* var) {
    *var = instance->mask();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setMask
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setMask(castor::stager::StartRequest* instance, unsigned long new_var) {
    instance->setMask(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_pid
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_pid(castor::stager::StartRequest* instance, unsigned long* var) {
    *var = instance->pid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setPid
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setPid(castor::stager::StartRequest* instance, unsigned long new_var) {
    instance->setPid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_machine
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_machine(castor::stager::StartRequest* instance, const char** var) {
    *var = instance->machine().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setMachine
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setMachine(castor::stager::StartRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMachine(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_svcClassName
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_svcClassName(castor::stager::StartRequest* instance, const char** var) {
    *var = instance->svcClassName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setSvcClassName
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setSvcClassName(castor::stager::StartRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setSvcClassName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_userTag
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_userTag(castor::stager::StartRequest* instance, const char** var) {
    *var = instance->userTag().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setUserTag
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setUserTag(castor::stager::StartRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setUserTag(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_reqId
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_reqId(castor::stager::StartRequest* instance, const char** var) {
    *var = instance->reqId().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setReqId
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setReqId(castor::stager::StartRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqId(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_creationTime
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_creationTime(castor::stager::StartRequest* instance, u_signed64* var) {
    *var = instance->creationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setCreationTime
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setCreationTime(castor::stager::StartRequest* instance, u_signed64 new_var) {
    instance->setCreationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_lastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_lastModificationTime(castor::stager::StartRequest* instance, u_signed64* var) {
    *var = instance->lastModificationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setLastModificationTime
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setLastModificationTime(castor::stager::StartRequest* instance, u_signed64 new_var) {
    instance->setLastModificationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_svcClass
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_svcClass(castor::stager::StartRequest* instance, castor::stager::SvcClass** var) {
    *var = instance->svcClass();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setSvcClass
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setSvcClass(castor::stager::StartRequest* instance, castor::stager::SvcClass* new_var) {
    instance->setSvcClass(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_client
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_client(castor::stager::StartRequest* instance, castor::IClient** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setClient
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setClient(castor::stager::StartRequest* instance, castor::IClient* new_var) {
    instance->setClient(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_subreqId
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_subreqId(castor::stager::StartRequest* instance, u_signed64* var) {
    *var = instance->subreqId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setSubreqId
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setSubreqId(castor::stager::StartRequest* instance, u_signed64 new_var) {
    instance->setSubreqId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_diskServer
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_diskServer(castor::stager::StartRequest* instance, const char** var) {
    *var = instance->diskServer().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setDiskServer
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setDiskServer(castor::stager::StartRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setDiskServer(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_fileSystem
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_fileSystem(castor::stager::StartRequest* instance, const char** var) {
    *var = instance->fileSystem().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_StartRequest_setFileSystem
  //----------------------------------------------------------------------------
  int Cstager_StartRequest_setFileSystem(castor::stager::StartRequest* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setFileSystem(snew_var);
    return 0;
  }

} // End of extern "C"

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeDriveCInt.cpp
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
#include "castor/IObject.hpp"
#include "castor/stager/ClientIdentification.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/vdqm/ExtendedDeviceGroup.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDriveStatusCodes.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_create
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_create(castor::vdqm::TapeDrive** obj) {
    *obj = new castor::vdqm::TapeDrive();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_delete
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_delete(castor::vdqm::TapeDrive* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cvdqm_TapeDrive_getIObject(castor::vdqm::TapeDrive* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_fromIObject
  //----------------------------------------------------------------------------
  castor::vdqm::TapeDrive* Cvdqm_TapeDrive_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::vdqm::TapeDrive*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_print
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_print(castor::vdqm::TapeDrive* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_TYPE
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_TYPE(int* ret) {
    *ret = castor::vdqm::TapeDrive::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_type
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_type(castor::vdqm::TapeDrive* instance,
                           int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_clone
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_clone(castor::vdqm::TapeDrive* instance,
                            castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_jobID
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_jobID(castor::vdqm::TapeDrive* instance, int* var) {
    *var = instance->jobID();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setJobID
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setJobID(castor::vdqm::TapeDrive* instance, int new_var) {
    instance->setJobID(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_modificationTime
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_modificationTime(castor::vdqm::TapeDrive* instance, int* var) {
    *var = instance->modificationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setModificationTime
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setModificationTime(castor::vdqm::TapeDrive* instance, int new_var) {
    instance->setModificationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_resettime
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_resettime(castor::vdqm::TapeDrive* instance, int* var) {
    *var = instance->resettime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setResettime
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setResettime(castor::vdqm::TapeDrive* instance, int new_var) {
    instance->setResettime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_usecount
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_usecount(castor::vdqm::TapeDrive* instance, int* var) {
    *var = instance->usecount();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setUsecount
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setUsecount(castor::vdqm::TapeDrive* instance, int new_var) {
    instance->setUsecount(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_errcount
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_errcount(castor::vdqm::TapeDrive* instance, int* var) {
    *var = instance->errcount();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setErrcount
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setErrcount(castor::vdqm::TapeDrive* instance, int new_var) {
    instance->setErrcount(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_transferredMB
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_transferredMB(castor::vdqm::TapeDrive* instance, int* var) {
    *var = instance->transferredMB();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setTransferredMB
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setTransferredMB(castor::vdqm::TapeDrive* instance, int new_var) {
    instance->setTransferredMB(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_totalMB
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_totalMB(castor::vdqm::TapeDrive* instance, u_signed64* var) {
    *var = instance->totalMB();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setTotalMB
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setTotalMB(castor::vdqm::TapeDrive* instance, u_signed64 new_var) {
    instance->setTotalMB(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_dedicate
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_dedicate(castor::vdqm::TapeDrive* instance, const char** var) {
    *var = instance->dedicate().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setDedicate
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setDedicate(castor::vdqm::TapeDrive* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setDedicate(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_newDedicate
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_newDedicate(castor::vdqm::TapeDrive* instance, const char** var) {
    *var = instance->newDedicate().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNewDedicate
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNewDedicate(castor::vdqm::TapeDrive* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setNewDedicate(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_is_uid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_is_uid(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->is_uid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setIs_uid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setIs_uid(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setIs_uid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_is_gid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_is_gid(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->is_gid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setIs_gid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setIs_gid(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setIs_gid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_is_name
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_is_name(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->is_name();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setIs_name
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setIs_name(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setIs_name(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_uid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_uid(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_uid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_uid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_uid(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_uid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_gid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_gid(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_gid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_gid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_gid(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_gid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_name
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_name(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_name();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_name
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_name(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_name(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_host
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_host(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_host();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_host
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_host(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_host(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_vid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_vid(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_vid();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_vid
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_vid(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_vid(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_mode
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_mode(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_mode();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_mode
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_mode(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_mode(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_date
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_date(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_date();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_date
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_date(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_date(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_time
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_time(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_time();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_time
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_time(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_time(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_no_age
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_no_age(castor::vdqm::TapeDrive* instance, short* var) {
    *var = instance->no_age();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setNo_age
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setNo_age(castor::vdqm::TapeDrive* instance, short new_var) {
    instance->setNo_age(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_driveName
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_driveName(castor::vdqm::TapeDrive* instance, const char** var) {
    *var = instance->driveName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setDriveName
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setDriveName(castor::vdqm::TapeDrive* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setDriveName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_id
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_id(castor::vdqm::TapeDrive* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setId
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setId(castor::vdqm::TapeDrive* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_tape
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_tape(castor::vdqm::TapeDrive* instance, castor::stager::Tape** var) {
    *var = instance->tape();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setTape
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setTape(castor::vdqm::TapeDrive* instance, castor::stager::Tape* new_var) {
    instance->setTape(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_addExtDevGrp
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_addExtDevGrp(castor::vdqm::TapeDrive* instance, castor::vdqm::ExtendedDeviceGroup* obj) {
    instance->addExtDevGrp(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_removeExtDevGrp
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_removeExtDevGrp(castor::vdqm::TapeDrive* instance, castor::vdqm::ExtendedDeviceGroup* obj) {
    instance->removeExtDevGrp(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_extDevGrp
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_extDevGrp(castor::vdqm::TapeDrive* instance, castor::vdqm::ExtendedDeviceGroup*** var, int* len) {
    std::vector<castor::vdqm::ExtendedDeviceGroup*>& result = instance->extDevGrp();
    *len = result.size();
    *var = (castor::vdqm::ExtendedDeviceGroup**) malloc((*len) * sizeof(castor::vdqm::ExtendedDeviceGroup*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_runningTapeReq
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_runningTapeReq(castor::vdqm::TapeDrive* instance, castor::vdqm::TapeRequest** var) {
    *var = instance->runningTapeReq();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setRunningTapeReq
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setRunningTapeReq(castor::vdqm::TapeDrive* instance, castor::vdqm::TapeRequest* new_var) {
    instance->setRunningTapeReq(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_status
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_status(castor::vdqm::TapeDrive* instance, castor::vdqm::TapeDriveStatusCodes* var) {
    *var = instance->status();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setStatus
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setStatus(castor::vdqm::TapeDrive* instance, castor::vdqm::TapeDriveStatusCodes new_var) {
    instance->setStatus(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_tapeServer
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_tapeServer(castor::vdqm::TapeDrive* instance, castor::vdqm::TapeServer** var) {
    *var = instance->tapeServer();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setTapeServer
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setTapeServer(castor::vdqm::TapeDrive* instance, castor::vdqm::TapeServer* new_var) {
    instance->setTapeServer(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_client
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_client(castor::vdqm::TapeDrive* instance, castor::stager::ClientIdentification** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeDrive_setClient
  //----------------------------------------------------------------------------
  int Cvdqm_TapeDrive_setClient(castor::vdqm::TapeDrive* instance, castor::stager::ClientIdentification* new_var) {
    instance->setClient(new_var);
    return 0;
  }

} // End of extern "C"

/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeServerCInt.cpp
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
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "castor/vdqm/TapeServerStatusCodes.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_create
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_create(castor::vdqm::TapeServer** obj) {
    *obj = new castor::vdqm::TapeServer();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_delete
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_delete(castor::vdqm::TapeServer* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cvdqm_TapeServer_getIObject(castor::vdqm::TapeServer* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_fromIObject
  //----------------------------------------------------------------------------
  castor::vdqm::TapeServer* Cvdqm_TapeServer_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::vdqm::TapeServer*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_print
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_print(castor::vdqm::TapeServer* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_TYPE
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_TYPE(int* ret) {
    *ret = castor::vdqm::TapeServer::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_type
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_type(castor::vdqm::TapeServer* instance,
                            int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_clone
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_clone(castor::vdqm::TapeServer* instance,
                             castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_serverName
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_serverName(castor::vdqm::TapeServer* instance, const char** var) {
    *var = instance->serverName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_setServerName
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_setServerName(castor::vdqm::TapeServer* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setServerName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_id
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_id(castor::vdqm::TapeServer* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_setId
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_setId(castor::vdqm::TapeServer* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_addTapeDrives
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_addTapeDrives(castor::vdqm::TapeServer* instance, castor::vdqm::TapeDrive* obj) {
    instance->addTapeDrives(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_removeTapeDrives
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_removeTapeDrives(castor::vdqm::TapeServer* instance, castor::vdqm::TapeDrive* obj) {
    instance->removeTapeDrives(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_tapeDrives
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_tapeDrives(castor::vdqm::TapeServer* instance, castor::vdqm::TapeDrive*** var, int* len) {
    std::vector<castor::vdqm::TapeDrive*>& result = instance->tapeDrives();
    *len = result.size();
    *var = (castor::vdqm::TapeDrive**) malloc((*len) * sizeof(castor::vdqm::TapeDrive*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_actingMode
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_actingMode(castor::vdqm::TapeServer* instance, castor::vdqm::TapeServerStatusCodes* var) {
    *var = instance->actingMode();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeServer_setActingMode
  //----------------------------------------------------------------------------
  int Cvdqm_TapeServer_setActingMode(castor::vdqm::TapeServer* instance, castor::vdqm::TapeServerStatusCodes new_var) {
    instance->setActingMode(new_var);
    return 0;
  }

} // End of extern "C"

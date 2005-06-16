/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeRequestCInt.cpp
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
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_create
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_create(castor::vdqm::TapeRequest** obj) {
    *obj = new castor::vdqm::TapeRequest();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_delete
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_delete(castor::vdqm::TapeRequest* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cvdqm_TapeRequest_getIObject(castor::vdqm::TapeRequest* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_fromIObject
  //----------------------------------------------------------------------------
  castor::vdqm::TapeRequest* Cvdqm_TapeRequest_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::vdqm::TapeRequest*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_print
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_print(castor::vdqm::TapeRequest* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_TYPE
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_TYPE(int* ret) {
    *ret = castor::vdqm::TapeRequest::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_type
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_type(castor::vdqm::TapeRequest* instance,
                             int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_clone
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_clone(castor::vdqm::TapeRequest* instance,
                              castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_priority
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_priority(castor::vdqm::TapeRequest* instance, int* var) {
    *var = instance->priority();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_setPriority
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_setPriority(castor::vdqm::TapeRequest* instance, int new_var) {
    instance->setPriority(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_creationTime
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_creationTime(castor::vdqm::TapeRequest* instance, int* var) {
    *var = instance->creationTime();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_setCreationTime
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_setCreationTime(castor::vdqm::TapeRequest* instance, int new_var) {
    instance->setCreationTime(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_id
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_id(castor::vdqm::TapeRequest* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_setId
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_setId(castor::vdqm::TapeRequest* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_tape
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_tape(castor::vdqm::TapeRequest* instance, castor::stager::Tape** var) {
    *var = instance->tape();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_setTape
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_setTape(castor::vdqm::TapeRequest* instance, castor::stager::Tape* new_var) {
    instance->setTape(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_reqExtDevGrp
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_reqExtDevGrp(castor::vdqm::TapeRequest* instance, castor::vdqm::ExtendedDeviceGroup** var) {
    *var = instance->reqExtDevGrp();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_setReqExtDevGrp
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_setReqExtDevGrp(castor::vdqm::TapeRequest* instance, castor::vdqm::ExtendedDeviceGroup* new_var) {
    instance->setReqExtDevGrp(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_requestedSrv
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_requestedSrv(castor::vdqm::TapeRequest* instance, castor::vdqm::TapeServer** var) {
    *var = instance->requestedSrv();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_setRequestedSrv
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_setRequestedSrv(castor::vdqm::TapeRequest* instance, castor::vdqm::TapeServer* new_var) {
    instance->setRequestedSrv(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_tapeDrive
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_tapeDrive(castor::vdqm::TapeRequest* instance, castor::vdqm::TapeDrive** var) {
    *var = instance->tapeDrive();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_setTapeDrive
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_setTapeDrive(castor::vdqm::TapeRequest* instance, castor::vdqm::TapeDrive* new_var) {
    instance->setTapeDrive(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_client
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_client(castor::vdqm::TapeRequest* instance, castor::stager::ClientIdentification** var) {
    *var = instance->client();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_TapeRequest_setClient
  //----------------------------------------------------------------------------
  int Cvdqm_TapeRequest_setClient(castor::vdqm::TapeRequest* instance, castor::stager::ClientIdentification* new_var) {
    instance->setClient(new_var);
    return 0;
  }

} // End of extern "C"

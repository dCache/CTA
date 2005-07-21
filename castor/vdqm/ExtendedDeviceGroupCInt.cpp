/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/ExtendedDeviceGroupCInt.cpp
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
#include "castor/vdqm/ExtendedDeviceGroup.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_create
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_create(castor::vdqm::ExtendedDeviceGroup** obj) {
    *obj = new castor::vdqm::ExtendedDeviceGroup();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_delete
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_delete(castor::vdqm::ExtendedDeviceGroup* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cvdqm_ExtendedDeviceGroup_getIObject(castor::vdqm::ExtendedDeviceGroup* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_fromIObject
  //----------------------------------------------------------------------------
  castor::vdqm::ExtendedDeviceGroup* Cvdqm_ExtendedDeviceGroup_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::vdqm::ExtendedDeviceGroup*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_print
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_print(castor::vdqm::ExtendedDeviceGroup* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_TYPE
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_TYPE(int* ret) {
    *ret = castor::vdqm::ExtendedDeviceGroup::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_type
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_type(castor::vdqm::ExtendedDeviceGroup* instance,
                                     int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_clone
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_clone(castor::vdqm::ExtendedDeviceGroup* instance,
                                      castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_dgName
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_dgName(castor::vdqm::ExtendedDeviceGroup* instance, const char** var) {
    *var = instance->dgName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_setDgName
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_setDgName(castor::vdqm::ExtendedDeviceGroup* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setDgName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_accessMode
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_accessMode(castor::vdqm::ExtendedDeviceGroup* instance, int* var) {
    *var = instance->accessMode();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_setAccessMode
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_setAccessMode(castor::vdqm::ExtendedDeviceGroup* instance, int new_var) {
    instance->setAccessMode(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_density
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_density(castor::vdqm::ExtendedDeviceGroup* instance, const char** var) {
    *var = instance->density().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_setDensity
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_setDensity(castor::vdqm::ExtendedDeviceGroup* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setDensity(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_tapeModel
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_tapeModel(castor::vdqm::ExtendedDeviceGroup* instance, const char** var) {
    *var = instance->tapeModel().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_setTapeModel
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_setTapeModel(castor::vdqm::ExtendedDeviceGroup* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setTapeModel(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_id
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_id(castor::vdqm::ExtendedDeviceGroup* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cvdqm_ExtendedDeviceGroup_setId
  //----------------------------------------------------------------------------
  int Cvdqm_ExtendedDeviceGroup_setId(castor::vdqm::ExtendedDeviceGroup* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

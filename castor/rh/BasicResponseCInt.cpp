/******************************************************************************
 *                      castor/rh/BasicResponseCInt.cpp
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
#include "castor/rh/BasicResponse.hpp"
#include "castor/rh/Response.hpp"
#include "osdep.h"

extern "C" {

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_create
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_create(castor::rh::BasicResponse** obj) {
    *obj = new castor::rh::BasicResponse();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Crh_BasicResponse_delete
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_delete(castor::rh::BasicResponse* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_getResponse
  //----------------------------------------------------------------------------
  castor::rh::Response* Crh_BasicResponse_getResponse(castor::rh::BasicResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_fromResponse
  //----------------------------------------------------------------------------
  castor::rh::BasicResponse* Crh_BasicResponse_fromResponse(castor::rh::Response* obj) {
    return dynamic_cast<castor::rh::BasicResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Crh_BasicResponse_getIObject(castor::rh::BasicResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_fromIObject
  //----------------------------------------------------------------------------
  castor::rh::BasicResponse* Crh_BasicResponse_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::rh::BasicResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_print
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_print(castor::rh::BasicResponse* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_TYPE
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_TYPE(int* ret) {
    *ret = castor::rh::BasicResponse::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_errorCode
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_errorCode(castor::rh::BasicResponse* instance, unsigned int* var) {
    *var = instance->errorCode();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_setErrorCode
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_setErrorCode(castor::rh::BasicResponse* instance, unsigned int new_var) {
    instance->setErrorCode(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_errorMessage
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_errorMessage(castor::rh::BasicResponse* instance, const char** var) {
    *var = instance->errorMessage().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_setErrorMessage
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_setErrorMessage(castor::rh::BasicResponse* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setErrorMessage(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_type
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_type(castor::rh::BasicResponse* instance,
                             int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_clone
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_clone(castor::rh::BasicResponse* instance,
                              castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_id
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_id(castor::rh::BasicResponse* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_BasicResponse_setId
  //----------------------------------------------------------------------------
  int Crh_BasicResponse_setId(castor::rh::BasicResponse* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

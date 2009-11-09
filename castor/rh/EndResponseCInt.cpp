/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/rh/EndResponseCInt.cpp
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
#include "castor/rh/EndResponse.hpp"
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <string.h>

extern "C" {

  //----------------------------------------------------------------------------
  // Crh_EndResponse_create
  //----------------------------------------------------------------------------
  int Crh_EndResponse_create(castor::rh::EndResponse** obj) {
    *obj = new castor::rh::EndResponse();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Crh_EndResponse_delete
  //----------------------------------------------------------------------------
  int Crh_EndResponse_delete(castor::rh::EndResponse* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_getResponse
  //----------------------------------------------------------------------------
  castor::rh::Response* Crh_EndResponse_getResponse(castor::rh::EndResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_fromResponse
  //----------------------------------------------------------------------------
  castor::rh::EndResponse* Crh_EndResponse_fromResponse(castor::rh::Response* obj) {
    return dynamic_cast<castor::rh::EndResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Crh_EndResponse_getIObject(castor::rh::EndResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_fromIObject
  //----------------------------------------------------------------------------
  castor::rh::EndResponse* Crh_EndResponse_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::rh::EndResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_print
  //----------------------------------------------------------------------------
  int Crh_EndResponse_print(castor::rh::EndResponse* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_TYPE
  //----------------------------------------------------------------------------
  int Crh_EndResponse_TYPE(int* ret) {
    *ret = castor::rh::EndResponse::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_errorCode
  //----------------------------------------------------------------------------
  int Crh_EndResponse_errorCode(castor::rh::EndResponse* instance, unsigned int* var) {
    *var = instance->errorCode();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_setErrorCode
  //----------------------------------------------------------------------------
  int Crh_EndResponse_setErrorCode(castor::rh::EndResponse* instance, unsigned int new_var) {
    instance->setErrorCode(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_errorMessage
  //----------------------------------------------------------------------------
  int Crh_EndResponse_errorMessage(castor::rh::EndResponse* instance, const char** var) {
    *var = instance->errorMessage().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_setErrorMessage
  //----------------------------------------------------------------------------
  int Crh_EndResponse_setErrorMessage(castor::rh::EndResponse* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setErrorMessage(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_reqAssociated
  //----------------------------------------------------------------------------
  int Crh_EndResponse_reqAssociated(castor::rh::EndResponse* instance, const char** var) {
    *var = instance->reqAssociated().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_setReqAssociated
  //----------------------------------------------------------------------------
  int Crh_EndResponse_setReqAssociated(castor::rh::EndResponse* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqAssociated(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_type
  //----------------------------------------------------------------------------
  int Crh_EndResponse_type(castor::rh::EndResponse* instance,
                           int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_clone
  //----------------------------------------------------------------------------
  int Crh_EndResponse_clone(castor::rh::EndResponse* instance,
                            castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_id
  //----------------------------------------------------------------------------
  int Crh_EndResponse_id(castor::rh::EndResponse* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_EndResponse_setId
  //----------------------------------------------------------------------------
  int Crh_EndResponse_setId(castor::rh::EndResponse* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

} // End of extern "C"

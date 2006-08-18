/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/rh/GCFilesResponseCInt.cpp
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
#include "castor/rh/GCFilesResponse.hpp"
#include "castor/rh/Response.hpp"
#include "castor/stager/GCLocalFile.hpp"
#include "osdep.h"
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_create
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_create(castor::rh::GCFilesResponse** obj) {
    *obj = new castor::rh::GCFilesResponse();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_delete
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_delete(castor::rh::GCFilesResponse* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_getResponse
  //----------------------------------------------------------------------------
  castor::rh::Response* Crh_GCFilesResponse_getResponse(castor::rh::GCFilesResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_fromResponse
  //----------------------------------------------------------------------------
  castor::rh::GCFilesResponse* Crh_GCFilesResponse_fromResponse(castor::rh::Response* obj) {
    return dynamic_cast<castor::rh::GCFilesResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Crh_GCFilesResponse_getIObject(castor::rh::GCFilesResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_fromIObject
  //----------------------------------------------------------------------------
  castor::rh::GCFilesResponse* Crh_GCFilesResponse_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::rh::GCFilesResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_print
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_print(castor::rh::GCFilesResponse* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_TYPE
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_TYPE(int* ret) {
    *ret = castor::rh::GCFilesResponse::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_errorCode
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_errorCode(castor::rh::GCFilesResponse* instance, unsigned int* var) {
    *var = instance->errorCode();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_setErrorCode
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_setErrorCode(castor::rh::GCFilesResponse* instance, unsigned int new_var) {
    instance->setErrorCode(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_errorMessage
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_errorMessage(castor::rh::GCFilesResponse* instance, const char** var) {
    *var = instance->errorMessage().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_setErrorMessage
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_setErrorMessage(castor::rh::GCFilesResponse* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setErrorMessage(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_reqAssociated
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_reqAssociated(castor::rh::GCFilesResponse* instance, const char** var) {
    *var = instance->reqAssociated().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_setReqAssociated
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_setReqAssociated(castor::rh::GCFilesResponse* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setReqAssociated(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_type
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_type(castor::rh::GCFilesResponse* instance,
                               int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_clone
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_clone(castor::rh::GCFilesResponse* instance,
                                castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_id
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_id(castor::rh::GCFilesResponse* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_setId
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_setId(castor::rh::GCFilesResponse* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_addFiles
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_addFiles(castor::rh::GCFilesResponse* instance, castor::stager::GCLocalFile* obj) {
    instance->addFiles(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_removeFiles
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_removeFiles(castor::rh::GCFilesResponse* instance, castor::stager::GCLocalFile* obj) {
    instance->removeFiles(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_GCFilesResponse_files
  //----------------------------------------------------------------------------
  int Crh_GCFilesResponse_files(castor::rh::GCFilesResponse* instance, castor::stager::GCLocalFile*** var, int* len) {
    std::vector<castor::stager::GCLocalFile*>& result = instance->files();
    *len = result.size();
    *var = (castor::stager::GCLocalFile**) malloc((*len) * sizeof(castor::stager::GCLocalFile*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

} // End of extern "C"

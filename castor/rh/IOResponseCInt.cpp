/******************************************************************************
 *                      castor/rh/IOResponseCInt.cpp
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
#include "castor/rh/FileResponse.hpp"
#include "castor/rh/IOResponse.hpp"
#include "castor/rh/Response.hpp"

extern "C" {

  //----------------------------------------------------------------------------
  // Crh_IOResponse_create
  //----------------------------------------------------------------------------
  int Crh_IOResponse_create(castor::rh::IOResponse** obj) {
    *obj = new castor::rh::IOResponse();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Crh_IOResponse_delete
  //----------------------------------------------------------------------------
  int Crh_IOResponse_delete(castor::rh::IOResponse* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_getFileResponse
  //----------------------------------------------------------------------------
  castor::rh::FileResponse* Crh_IOResponse_getFileResponse(castor::rh::IOResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_fromFileResponse
  //----------------------------------------------------------------------------
  castor::rh::IOResponse* Crh_IOResponse_fromFileResponse(castor::rh::FileResponse* obj) {
    return dynamic_cast<castor::rh::IOResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_getResponse
  //----------------------------------------------------------------------------
  castor::rh::Response* Crh_IOResponse_getResponse(castor::rh::IOResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_fromResponse
  //----------------------------------------------------------------------------
  castor::rh::IOResponse* Crh_IOResponse_fromResponse(castor::rh::Response* obj) {
    return dynamic_cast<castor::rh::IOResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Crh_IOResponse_getIObject(castor::rh::IOResponse* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_fromIObject
  //----------------------------------------------------------------------------
  castor::rh::IOResponse* Crh_IOResponse_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::rh::IOResponse*>(obj);
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_print
  //----------------------------------------------------------------------------
  int Crh_IOResponse_print(castor::rh::IOResponse* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_TYPE
  //----------------------------------------------------------------------------
  int Crh_IOResponse_TYPE(int* ret) {
    *ret = castor::rh::IOResponse::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_type
  //----------------------------------------------------------------------------
  int Crh_IOResponse_type(castor::rh::IOResponse* instance,
                          int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_fileName
  //----------------------------------------------------------------------------
  int Crh_IOResponse_fileName(castor::rh::IOResponse* instance, const char** var) {
    *var = instance->fileName().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_setFileName
  //----------------------------------------------------------------------------
  int Crh_IOResponse_setFileName(castor::rh::IOResponse* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setFileName(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_server
  //----------------------------------------------------------------------------
  int Crh_IOResponse_server(castor::rh::IOResponse* instance, const char** var) {
    *var = instance->server().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_setServer
  //----------------------------------------------------------------------------
  int Crh_IOResponse_setServer(castor::rh::IOResponse* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setServer(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_port
  //----------------------------------------------------------------------------
  int Crh_IOResponse_port(castor::rh::IOResponse* instance, int* var) {
    *var = instance->port();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_setPort
  //----------------------------------------------------------------------------
  int Crh_IOResponse_setPort(castor::rh::IOResponse* instance, int new_var) {
    instance->setPort(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_protocol
  //----------------------------------------------------------------------------
  int Crh_IOResponse_protocol(castor::rh::IOResponse* instance, const char** var) {
    *var = instance->protocol().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Crh_IOResponse_setProtocol
  //----------------------------------------------------------------------------
  int Crh_IOResponse_setProtocol(castor::rh::IOResponse* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setProtocol(snew_var);
    return 0;
  }

} // End of extern "C"

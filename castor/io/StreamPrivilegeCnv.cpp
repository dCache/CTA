/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamPrivilegeCnv.cpp
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
#include "StreamPrivilegeCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/bwlist/ListPrivilegesResponse.hpp"
#include "castor/bwlist/Privilege.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamBaseCnv.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamPrivilegeCnv>* s_factoryStreamPrivilegeCnv =
  new castor::CnvFactory<castor::io::StreamPrivilegeCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamPrivilegeCnv::StreamPrivilegeCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamPrivilegeCnv::~StreamPrivilegeCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamPrivilegeCnv::ObjType() {
  return castor::bwlist::Privilege::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamPrivilegeCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamPrivilegeCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool,
                                               unsigned int)
  throw (castor::exception::Exception) {
  castor::bwlist::Privilege* obj = 
    dynamic_cast<castor::bwlist::Privilege*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->serviceClass();
  ad->stream() << obj->euid();
  ad->stream() << obj->egid();
  ad->stream() << obj->requestType();
  ad->stream() << obj->granted();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamPrivilegeCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::bwlist::Privilege* object = new castor::bwlist::Privilege();
  // Now retrieve and set members
  std::string serviceClass;
  ad->stream() >> serviceClass;
  object->setServiceClass(serviceClass);
  int euid;
  ad->stream() >> euid;
  object->setEuid(euid);
  int egid;
  ad->stream() >> egid;
  object->setEgid(egid);
  int requestType;
  ad->stream() >> requestType;
  object->setRequestType(requestType);
  bool granted;
  ad->stream() >> granted;
  object->setGranted(granted);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamPrivilegeCnv::marshalObject(castor::IObject* object,
                                                   castor::io::StreamAddress* address,
                                                   castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::bwlist::Privilege* obj = 
    dynamic_cast<castor::bwlist::Privilege*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->response(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamPrivilegeCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                 castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::bwlist::Privilege* obj = 
    dynamic_cast<castor::bwlist::Privilege*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objResponse = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setResponse(dynamic_cast<castor::bwlist::ListPrivilegesResponse*>(objResponse));
  return object;
}


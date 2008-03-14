/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamQueryParameterCnv.cpp
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
#include "StreamQueryParameterCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamBaseCnv.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/stager/QryRequest.hpp"
#include "castor/stager/QueryParameter.hpp"
#include "castor/stager/RequestQueryType.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamQueryParameterCnv>* s_factoryStreamQueryParameterCnv =
  new castor::CnvFactory<castor::io::StreamQueryParameterCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamQueryParameterCnv::StreamQueryParameterCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamQueryParameterCnv::~StreamQueryParameterCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamQueryParameterCnv::ObjType() {
  return castor::stager::QueryParameter::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamQueryParameterCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamQueryParameterCnv::createRep(castor::IAddress* address,
                                                    castor::IObject* object,
                                                    bool endTransaction,
                                                    unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::QueryParameter* obj = 
    dynamic_cast<castor::stager::QueryParameter*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->value();
  ad->stream() << obj->id();
  ad->stream() << obj->queryType();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamQueryParameterCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::QueryParameter* object = new castor::stager::QueryParameter();
  // Now retrieve and set members
  std::string value;
  ad->stream() >> value;
  object->setValue(value);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int queryType;
  ad->stream() >> queryType;
  object->setQueryType((castor::stager::RequestQueryType)queryType);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamQueryParameterCnv::marshalObject(castor::IObject* object,
                                                        castor::io::StreamAddress* address,
                                                        castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::QueryParameter* obj = 
    dynamic_cast<castor::stager::QueryParameter*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->query(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamQueryParameterCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                      castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::QueryParameter* obj = 
    dynamic_cast<castor::stager::QueryParameter*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objQuery = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setQuery(dynamic_cast<castor::stager::QryRequest*>(objQuery));
  return object;
}


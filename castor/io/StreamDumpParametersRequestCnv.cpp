/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamDumpParametersRequestCnv.cpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamDumpParametersRequestCnv.hpp"
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
#include "castor/tape/tapegateway/DumpParametersRequest.hpp"
#include "osdep.h"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamDumpParametersRequestCnv>* s_factoryStreamDumpParametersRequestCnv =
  new castor::CnvFactory<castor::io::StreamDumpParametersRequestCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamDumpParametersRequestCnv::StreamDumpParametersRequestCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamDumpParametersRequestCnv::~StreamDumpParametersRequestCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamDumpParametersRequestCnv::ObjType() {
  return castor::tape::tapegateway::DumpParametersRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamDumpParametersRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamDumpParametersRequestCnv::createRep(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           bool,
                                                           unsigned int)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::DumpParametersRequest* obj = 
    dynamic_cast<castor::tape::tapegateway::DumpParametersRequest*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->mountTransactionId();
  ad->stream() << obj->aggregatorTransactionId();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDumpParametersRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::DumpParametersRequest* object = new castor::tape::tapegateway::DumpParametersRequest();
  // Now retrieve and set members
  u_signed64 mountTransactionId;
  ad->stream() >> mountTransactionId;
  object->setMountTransactionId(mountTransactionId);
  u_signed64 aggregatorTransactionId;
  ad->stream() >> aggregatorTransactionId;
  object->setAggregatorTransactionId(aggregatorTransactionId);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamDumpParametersRequestCnv::marshalObject(castor::IObject* object,
                                                               castor::io::StreamAddress* address,
                                                               castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::DumpParametersRequest* obj = 
    dynamic_cast<castor::tape::tapegateway::DumpParametersRequest*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDumpParametersRequestCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                             castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


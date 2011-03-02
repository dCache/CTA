/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamRepackAckCnv.cpp
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
#include "StreamRepackAckCnv.hpp"
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
#include "castor/repack/RepackAck.hpp"
#include "castor/repack/RepackCommandCode.hpp"
#include "castor/repack/RepackResponse.hpp"
#include "osdep.h"
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamRepackAckCnv>* s_factoryStreamRepackAckCnv =
  new castor::CnvFactory<castor::io::StreamRepackAckCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamRepackAckCnv::StreamRepackAckCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamRepackAckCnv::~StreamRepackAckCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamRepackAckCnv::ObjType() {
  return castor::repack::RepackAck::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamRepackAckCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamRepackAckCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool,
                                               unsigned int)
  throw (castor::exception::Exception) {
  castor::repack::RepackAck* obj = 
    dynamic_cast<castor::repack::RepackAck*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->id();
  ad->stream() << obj->command();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamRepackAckCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::repack::RepackAck* object = new castor::repack::RepackAck();
  // Now retrieve and set members
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int command;
  ad->stream() >> command;
  object->setCommand((castor::repack::RepackCommandCode)command);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamRepackAckCnv::marshalObject(castor::IObject* object,
                                                   castor::io::StreamAddress* address,
                                                   castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::repack::RepackAck* obj = 
    dynamic_cast<castor::repack::RepackAck*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->repackresponse().size();
    for (std::vector<castor::repack::RepackResponse*>::iterator it = obj->repackresponse().begin();
         it != obj->repackresponse().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamRepackAckCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                 castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::repack::RepackAck* obj = 
    dynamic_cast<castor::repack::RepackAck*>(object);
  unsigned int repackresponseNb;
  ad.stream() >> repackresponseNb;
  for (unsigned int i = 0; i < repackresponseNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objRepackresponse = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addRepackresponse(dynamic_cast<castor::repack::RepackResponse*>(objRepackresponse));
  }
  return object;
}


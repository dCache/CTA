/******************************************************************************
 *                      castor/io/StreamTapePoolCnv.cpp
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
#include "StreamTapePoolCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvFactory.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/stager/Stream.hpp"
#include "castor/stager/SvcClass.hpp"
#include "castor/stager/TapePool.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamTapePoolCnv> s_factoryStreamTapePoolCnv;
const castor::ICnvFactory& StreamTapePoolCnvFactory = 
  s_factoryStreamTapePoolCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamTapePoolCnv::StreamTapePoolCnv(castor::ICnvSvc* cnvSvc) :
  StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamTapePoolCnv::~StreamTapePoolCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapePoolCnv::ObjType() {
  return castor::stager::TapePool::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapePoolCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamTapePoolCnv::createRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              bool autocommit,
                                              unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::TapePool* obj = 
    dynamic_cast<castor::stager::TapePool*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->name();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapePoolCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::TapePool* object = new castor::stager::TapePool();
  // Now retrieve and set members
  std::string name;
  ad->stream() >> name;
  object->setName(name);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamTapePoolCnv::marshalObject(castor::IObject* object,
                                                  castor::io::StreamAddress* address,
                                                  castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::TapePool* obj = 
    dynamic_cast<castor::stager::TapePool*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->svcClasses().size();
    for (std::vector<castor::stager::SvcClass*>::iterator it = obj->svcClasses().begin();
         it != obj->svcClasses().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    address->stream() << obj->streams().size();
    for (std::vector<castor::stager::Stream*>::iterator it = obj->streams().begin();
         it != obj->streams().end();
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
castor::IObject* castor::io::StreamTapePoolCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::TapePool* obj = 
    dynamic_cast<castor::stager::TapePool*>(object);
  unsigned int svcClassesNb;
  ad.stream() >> svcClassesNb;
  for (unsigned int i = 0; i < svcClassesNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    IObject* objSvcClasses = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addSvcClasses(dynamic_cast<castor::stager::SvcClass*>(objSvcClasses));
  }
  unsigned int streamsNb;
  ad.stream() >> streamsNb;
  for (unsigned int i = 0; i < streamsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    IObject* objStreams = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addStreams(dynamic_cast<castor::stager::Stream*>(objStreams));
  }
  return object;
}


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamTapeRequestCnv.cpp
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
#include "StreamTapeRequestCnv.hpp"
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
#include "castor/stager/ClientIdentification.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/vdqm/DeviceGroupName.hpp"
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "osdep.h"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamTapeRequestCnv>* s_factoryStreamTapeRequestCnv =
  new castor::CnvFactory<castor::io::StreamTapeRequestCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamTapeRequestCnv::StreamTapeRequestCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamTapeRequestCnv::~StreamTapeRequestCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeRequestCnv::ObjType() {
  return castor::vdqm::TapeRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamTapeRequestCnv::createRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit,
                                                 unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeRequest* obj = 
    dynamic_cast<castor::vdqm::TapeRequest*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->priority();
  ad->stream() << obj->modificationTime();
  ad->stream() << obj->creationTime();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::vdqm::TapeRequest* object = new castor::vdqm::TapeRequest();
  // Now retrieve and set members
  int priority;
  ad->stream() >> priority;
  object->setPriority(priority);
  u_signed64 modificationTime;
  ad->stream() >> modificationTime;
  object->setModificationTime(modificationTime);
  u_signed64 creationTime;
  ad->stream() >> creationTime;
  object->setCreationTime(creationTime);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamTapeRequestCnv::marshalObject(castor::IObject* object,
                                                     castor::io::StreamAddress* address,
                                                     castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeRequest* obj = 
    dynamic_cast<castor::vdqm::TapeRequest*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->tape(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->tapeAccessSpecification(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->requestedSrv(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->tapeDrive(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->deviceGroupName(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->client(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeRequestCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                   castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::vdqm::TapeRequest* obj = 
    dynamic_cast<castor::vdqm::TapeRequest*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objTape = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setTape(dynamic_cast<castor::stager::Tape*>(objTape));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objTapeAccessSpecification = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setTapeAccessSpecification(dynamic_cast<castor::vdqm::TapeAccessSpecification*>(objTapeAccessSpecification));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objRequestedSrv = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setRequestedSrv(dynamic_cast<castor::vdqm::TapeServer*>(objRequestedSrv));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objTapeDrive = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setTapeDrive(dynamic_cast<castor::vdqm::TapeDrive*>(objTapeDrive));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objDeviceGroupName = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setDeviceGroupName(dynamic_cast<castor::vdqm::DeviceGroupName*>(objDeviceGroupName));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objClient = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setClient(dynamic_cast<castor::stager::ClientIdentification*>(objClient));
  return object;
}


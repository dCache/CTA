/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamErrorHistoryCnv.cpp
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
#include "StreamErrorHistoryCnv.hpp"
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
#include "castor/vdqm/ErrorHistory.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/VdqmTape.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamErrorHistoryCnv>* s_factoryStreamErrorHistoryCnv =
  new castor::CnvFactory<castor::io::StreamErrorHistoryCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamErrorHistoryCnv::StreamErrorHistoryCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamErrorHistoryCnv::~StreamErrorHistoryCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamErrorHistoryCnv::ObjType() {
  return castor::vdqm::ErrorHistory::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamErrorHistoryCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamErrorHistoryCnv::createRep(castor::IAddress* address,
                                                  castor::IObject* object,
                                                  bool endTransaction,
                                                  unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::ErrorHistory* obj = 
    dynamic_cast<castor::vdqm::ErrorHistory*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->errorMessage();
  ad->stream() << obj->timeStamp();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamErrorHistoryCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::vdqm::ErrorHistory* object = new castor::vdqm::ErrorHistory();
  // Now retrieve and set members
  std::string errorMessage;
  ad->stream() >> errorMessage;
  object->setErrorMessage(errorMessage);
  u_signed64 timeStamp;
  ad->stream() >> timeStamp;
  object->setTimeStamp(timeStamp);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamErrorHistoryCnv::marshalObject(castor::IObject* object,
                                                      castor::io::StreamAddress* address,
                                                      castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::vdqm::ErrorHistory* obj = 
    dynamic_cast<castor::vdqm::ErrorHistory*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->tapeDrive(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->tape(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamErrorHistoryCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                    castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::vdqm::ErrorHistory* obj = 
    dynamic_cast<castor::vdqm::ErrorHistory*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objTapeDrive = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setTapeDrive(dynamic_cast<castor::vdqm::TapeDrive*>(objTapeDrive));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objTape = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setTape(dynamic_cast<castor::vdqm::VdqmTape*>(objTape));
  return object;
}


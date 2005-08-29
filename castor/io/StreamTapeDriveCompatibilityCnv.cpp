/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamTapeDriveCompatibilityCnv.cpp
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
#include "StreamTapeDriveCompatibilityCnv.hpp"
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
#include "castor/io/StreamBaseCnv.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDriveCompatibility.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamTapeDriveCompatibilityCnv> s_factoryStreamTapeDriveCompatibilityCnv;
const castor::ICnvFactory& StreamTapeDriveCompatibilityCnvFactory = 
  s_factoryStreamTapeDriveCompatibilityCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamTapeDriveCompatibilityCnv::StreamTapeDriveCompatibilityCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamTapeDriveCompatibilityCnv::~StreamTapeDriveCompatibilityCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeDriveCompatibilityCnv::ObjType() {
  return castor::vdqm::TapeDriveCompatibility::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeDriveCompatibilityCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamTapeDriveCompatibilityCnv::createRep(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            bool autocommit,
                                                            unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->tapeDriveModel();
  ad->stream() << obj->priorityLevel();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeDriveCompatibilityCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::vdqm::TapeDriveCompatibility* object = new castor::vdqm::TapeDriveCompatibility();
  // Now retrieve and set members
  std::string tapeDriveModel;
  ad->stream() >> tapeDriveModel;
  object->setTapeDriveModel(tapeDriveModel);
  int priorityLevel;
  ad->stream() >> priorityLevel;
  object->setPriorityLevel(priorityLevel);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamTapeDriveCompatibilityCnv::marshalObject(castor::IObject* object,
                                                                castor::io::StreamAddress* address,
                                                                castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->().size();
    for (std::vector<castor::vdqm::TapeDrive*>::iterator it = obj->().begin();
         it != obj->().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    cnvSvc()->marshalObject(obj->tapeAccessSpecification(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeDriveCompatibilityCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                              castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  unsigned int Nb;
  ad.stream() >> Nb;
  for (unsigned int i = 0; i < Nb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* obj
/******************************************************************************
 *                      castor/io/StreamScheduleSubReqResponseCnv.cpp
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
 * @(#)$RCSfile: StreamScheduleSubReqResponseCnv.cpp,v $ $Revision: 1.1 $ $Release$ $Date: 2004/11/24 11:52:23 $ $Author: sponcec3 $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamScheduleSubReqResponseCnv.hpp"
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
#include "castor/rh/ScheduleSubReqResponse.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamScheduleSubReqResponseCnv> s_factoryStreamScheduleSubReqResponseCnv;
const castor::ICnvFactory& StreamScheduleSubReqResponseCnvFactory = 
  s_factoryStreamScheduleSubReqResponseCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamScheduleSubReqResponseCnv::StreamScheduleSubReqResponseCnv(castor::ICnvSvc* cnvSvc) :
  StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamScheduleSubReqResponseCnv::~StreamScheduleSubReqResponseCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamScheduleSubReqResponseCnv::ObjType() {
  return castor::rh::ScheduleSubReqResponse::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamScheduleSubReqResponseCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamScheduleSubReqResponseCnv::createRep(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            bool autocommit,
                                                            unsigned int type)
  throw (castor::exception::Exception) {
  castor::rh::ScheduleSubReqResponse* obj = 
    dynamic_cast<castor::rh::ScheduleSubReqResponse*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->errorCode();
  ad->stream() << obj->errorMessage();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamScheduleSubReqResponseCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::rh::ScheduleSubReqResponse* object = new castor::rh::ScheduleSubReqResponse();
  // Now retrieve and set members
  unsigned int errorCode;
  ad->stream() >> errorCode;
  object->setErrorCode(errorCode);
  std::string errorMessage;
  ad->stream() >> errorMessage;
  object->setErrorMessage(errorMessage);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamScheduleSubReqResponseCnv::marshalObject(castor::IObject* object,
                                                                castor::io::StreamAddress* address,
                                                                castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::rh::ScheduleSubReqResponse* obj = 
    dynamic_cast<castor::rh::ScheduleSubReqResponse*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->diskCopy(), address, alreadyDone);
    address->stream() << obj->sources().size();
    for (std::vector<castor::stager::DiskCopy*>::iterator it = obj->sources().begin();
         it != obj->sources().end();
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
castor::IObject* castor::io::StreamScheduleSubReqResponseCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                              castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::rh::ScheduleSubReqResponse* obj = 
    dynamic_cast<castor::rh::ScheduleSubReqResponse*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objDiskCopy = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setDiskCopy(dynamic_cast<castor::stager::DiskCopy*>(objDiskCopy));
  unsigned int sourcesNb;
  ad.stream() >> sourcesNb;
  for (unsigned int i = 0; i < sourcesNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    IObject* objSources = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addSources(dynamic_cast<castor::stager::DiskCopy*>(objSources));
  }
  return object;
}


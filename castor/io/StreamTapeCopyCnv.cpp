/******************************************************************************
 *                      castor/io/StreamTapeCopyCnv.cpp
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
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamTapeCopyCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IConverter.hpp"
#include "castor/IFactory.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/Segment.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "castor/stager/TapeCopyStatusCodes.hpp"
#include "osdep.h"
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamTapeCopyCnv> s_factoryStreamTapeCopyCnv;
const castor::IFactory<castor::IConverter>& StreamTapeCopyCnvFactory = 
  s_factoryStreamTapeCopyCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamTapeCopyCnv::StreamTapeCopyCnv() :
  StreamBaseCnv() {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamTapeCopyCnv::~StreamTapeCopyCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeCopyCnv::ObjType() {
  return castor::stager::TapeCopy::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeCopyCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamTapeCopyCnv::createRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::TapeCopy* obj = 
    dynamic_cast<castor::stager::TapeCopy*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->id();
  ad->stream() << obj->status();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeCopyCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::TapeCopy* object = new castor::stager::TapeCopy();
  // Now retrieve and set members
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int status;
  ad->stream() >> status;
  object->setStatus((castor::stager::TapeCopyStatusCodes)status);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamTapeCopyCnv::marshalObject(castor::IObject* object,
                                                  castor::io::StreamAddress* address,
                                                  castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::TapeCopy* obj = 
    dynamic_cast<castor::stager::TapeCopy*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << 0;
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    cnvSvc()->createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->segments().size();
    for (std::vector<castor::stager::Segment*>::iterator it = obj->segments().begin();
         it != obj->segments().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    cnvSvc()->marshalObject(obj->castorFile(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeCopyCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = cnvSvc()->createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::TapeCopy* obj = 
    dynamic_cast<castor::stager::TapeCopy*>(object);
  unsigned int segmentsNb;
  ad.stream() >> segmentsNb;
  for (unsigned int i = 0; i < segmentsNb; i++) {
    IObject* objSegments = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addSegments(dynamic_cast<castor::stager::Segment*>(objSegments));
  }
  IObject* objCastorFile = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setCastorFile(dynamic_cast<castor::stager::CastorFile*>(objCastorFile));
}


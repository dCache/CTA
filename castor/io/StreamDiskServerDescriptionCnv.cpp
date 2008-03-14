/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamDiskServerDescriptionCnv.cpp
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
#include "StreamDiskServerDescriptionCnv.hpp"
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
#include "castor/query/DiskPoolQueryResponse.hpp"
#include "castor/query/DiskServerDescription.hpp"
#include "castor/query/FileSystemDescription.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamDiskServerDescriptionCnv>* s_factoryStreamDiskServerDescriptionCnv =
  new castor::CnvFactory<castor::io::StreamDiskServerDescriptionCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamDiskServerDescriptionCnv::StreamDiskServerDescriptionCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamDiskServerDescriptionCnv::~StreamDiskServerDescriptionCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamDiskServerDescriptionCnv::ObjType() {
  return castor::query::DiskServerDescription::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamDiskServerDescriptionCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamDiskServerDescriptionCnv::createRep(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           bool endTransaction,
                                                           unsigned int type)
  throw (castor::exception::Exception) {
  castor::query::DiskServerDescription* obj = 
    dynamic_cast<castor::query::DiskServerDescription*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->name();
  ad->stream() << obj->status();
  ad->stream() << obj->freeSpace();
  ad->stream() << obj->totalSpace();
  ad->stream() << obj->reservedSpace();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDiskServerDescriptionCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::query::DiskServerDescription* object = new castor::query::DiskServerDescription();
  // Now retrieve and set members
  std::string name;
  ad->stream() >> name;
  object->setName(name);
  int status;
  ad->stream() >> status;
  object->setStatus(status);
  u_signed64 freeSpace;
  ad->stream() >> freeSpace;
  object->setFreeSpace(freeSpace);
  u_signed64 totalSpace;
  ad->stream() >> totalSpace;
  object->setTotalSpace(totalSpace);
  u_signed64 reservedSpace;
  ad->stream() >> reservedSpace;
  object->setReservedSpace(reservedSpace);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamDiskServerDescriptionCnv::marshalObject(castor::IObject* object,
                                                               castor::io::StreamAddress* address,
                                                               castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::query::DiskServerDescription* obj = 
    dynamic_cast<castor::query::DiskServerDescription*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->query(), address, alreadyDone);
    address->stream() << obj->fileSystems().size();
    for (std::vector<castor::query::FileSystemDescription*>::iterator it = obj->fileSystems().begin();
         it != obj->fileSystems().end();
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
castor::IObject* castor::io::StreamDiskServerDescriptionCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                             castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::query::DiskServerDescription* obj = 
    dynamic_cast<castor::query::DiskServerDescription*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objQuery = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setQuery(dynamic_cast<castor::query::DiskPoolQueryResponse*>(objQuery));
  unsigned int fileSystemsNb;
  ad.stream() >> fileSystemsNb;
  for (unsigned int i = 0; i < fileSystemsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objFileSystems = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addFileSystems(dynamic_cast<castor::query::FileSystemDescription*>(objFileSystems));
  }
  return object;
}


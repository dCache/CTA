/******************************************************************************
 *                      castor/io/StreamFileSystemCnv.cpp
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
#include "StreamFileSystemCnv.hpp"
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
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/DiskPool.hpp"
#include "castor/stager/DiskServer.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/stager/FileSystemStatusCodes.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamFileSystemCnv> s_factoryStreamFileSystemCnv;
const castor::ICnvFactory& StreamFileSystemCnvFactory = 
  s_factoryStreamFileSystemCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamFileSystemCnv::StreamFileSystemCnv(castor::ICnvSvc* cnvSvc) :
  StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamFileSystemCnv::~StreamFileSystemCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamFileSystemCnv::ObjType() {
  return castor::stager::FileSystem::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamFileSystemCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamFileSystemCnv::createRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                bool autocommit,
                                                unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::FileSystem* obj = 
    dynamic_cast<castor::stager::FileSystem*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->free();
  ad->stream() << obj->weight();
  ad->stream() << obj->fsDeviation();
  ad->stream() << obj->mountPoint();
  ad->stream() << obj->id();
  ad->stream() << obj->status();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileSystemCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::FileSystem* object = new castor::stager::FileSystem();
  // Now retrieve and set members
  u_signed64 free;
  ad->stream() >> free;
  object->setFree(free);
  float weight;
  ad->stream() >> weight;
  object->setWeight(weight);
  float fsDeviation;
  ad->stream() >> fsDeviation;
  object->setFsDeviation(fsDeviation);
  std::string mountPoint;
  ad->stream() >> mountPoint;
  object->setMountPoint(mountPoint);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int status;
  ad->stream() >> status;
  object->setStatus((castor::stager::FileSystemStatusCodes)status);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamFileSystemCnv::marshalObject(castor::IObject* object,
                                                    castor::io::StreamAddress* address,
                                                    castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::FileSystem* obj = 
    dynamic_cast<castor::stager::FileSystem*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->diskPool(), address, alreadyDone);
    address->stream() << obj->copies().size();
    for (std::vector<castor::stager::DiskCopy*>::iterator it = obj->copies().begin();
         it != obj->copies().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    cnvSvc()->marshalObject(obj->diskserver(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileSystemCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                  castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::FileSystem* obj = 
    dynamic_cast<castor::stager::FileSystem*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objDiskPool = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setDiskPool(dynamic_cast<castor::stager::DiskPool*>(objDiskPool));
  unsigned int copiesNb;
  ad.stream() >> copiesNb;
  for (unsigned int i = 0; i < copiesNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    IObject* objCopies = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addCopies(dynamic_cast<castor::stager::DiskCopy*>(objCopies));
  }
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objDiskserver = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setDiskserver(dynamic_cast<castor::stager::DiskServer*>(objDiskserver));
  return object;
}


/******************************************************************************
 *                      castor/io/StreamDiskCopyCnv.cpp
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
#include "StreamDiskCopyCnv.hpp"
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
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/DiskCopyStatusCodes.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/stager/SubRequest.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamDiskCopyCnv> s_factoryStreamDiskCopyCnv;
const castor::ICnvFactory& StreamDiskCopyCnvFactory = 
  s_factoryStreamDiskCopyCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamDiskCopyCnv::StreamDiskCopyCnv(castor::ICnvSvc* cnvSvc) :
  StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamDiskCopyCnv::~StreamDiskCopyCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamDiskCopyCnv::ObjType() {
  return castor::stager::DiskCopy::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamDiskCopyCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamDiskCopyCnv::createRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              bool autocommit,
                                              unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->path();
  ad->stream() << obj->diskcopyId();
  ad->stream() << obj->gcWeight();
  ad->stream() << obj->id();
  ad->stream() << obj->status();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDiskCopyCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::DiskCopy* object = new castor::stager::DiskCopy();
  // Now retrieve and set members
  std::string path;
  ad->stream() >> path;
  object->setPath(path);
  std::string diskcopyId;
  ad->stream() >> diskcopyId;
  object->setDiskcopyId(diskcopyId);
  float gcWeight;
  ad->stream() >> gcWeight;
  object->setGcWeight(gcWeight);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int status;
  ad->stream() >> status;
  object->setStatus((castor::stager::DiskCopyStatusCodes)status);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamDiskCopyCnv::marshalObject(castor::IObject* object,
                                                  castor::io::StreamAddress* address,
                                                  castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->subRequests().size();
    for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
         it != obj->subRequests().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    cnvSvc()->marshalObject(obj->fileSystem(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->castorFile(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDiskCopyCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
  unsigned int subRequestsNb;
  ad.stream() >> subRequestsNb;
  for (unsigned int i = 0; i < subRequestsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    IObject* objSubRequests = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addSubRequests(dynamic_cast<castor::stager::SubRequest*>(objSubRequests));
  }
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objFileSystem = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setFileSystem(dynamic_cast<castor::stager::FileSystem*>(objFileSystem));
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objCastorFile = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setCastorFile(dynamic_cast<castor::stager::CastorFile*>(objCastorFile));
  return object;
}


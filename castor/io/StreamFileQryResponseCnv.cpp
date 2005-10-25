/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamFileQryResponseCnv.cpp
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
#include "StreamFileQryResponseCnv.hpp"
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
#include "castor/rh/FileQryResponse.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamFileQryResponseCnv> s_factoryStreamFileQryResponseCnv;
const castor::ICnvFactory& StreamFileQryResponseCnvFactory = 
  s_factoryStreamFileQryResponseCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamFileQryResponseCnv::StreamFileQryResponseCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamFileQryResponseCnv::~StreamFileQryResponseCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamFileQryResponseCnv::ObjType() {
  return castor::rh::FileQryResponse::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamFileQryResponseCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamFileQryResponseCnv::createRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     bool autocommit,
                                                     unsigned int type)
  throw (castor::exception::Exception) {
  castor::rh::FileQryResponse* obj = 
    dynamic_cast<castor::rh::FileQryResponse*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->errorCode();
  ad->stream() << obj->errorMessage();
  ad->stream() << obj->fileName();
  ad->stream() << obj->castorFileName();
  ad->stream() << obj->fileId();
  ad->stream() << obj->status();
  ad->stream() << obj->size();
  ad->stream() << obj->poolName();
  ad->stream() << obj->creationTime();
  ad->stream() << obj->accessTime();
  ad->stream() << obj->nbAccesses();
  ad->stream() << obj->diskServer();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileQryResponseCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::rh::FileQryResponse* object = new castor::rh::FileQryResponse();
  // Now retrieve and set members
  unsigned int errorCode;
  ad->stream() >> errorCode;
  object->setErrorCode(errorCode);
  std::string errorMessage;
  ad->stream() >> errorMessage;
  object->setErrorMessage(errorMessage);
  std::string fileName;
  ad->stream() >> fileName;
  object->setFileName(fileName);
  std::string castorFileName;
  ad->stream() >> castorFileName;
  object->setCastorFileName(castorFileName);
  u_signed64 fileId;
  ad->stream() >> fileId;
  object->setFileId(fileId);
  unsigned int status;
  ad->stream() >> status;
  object->setStatus(status);
  u_signed64 size;
  ad->stream() >> size;
  object->setSize(size);
  std::string poolName;
  ad->stream() >> poolName;
  object->setPoolName(poolName);
  u_signed64 creationTime;
  ad->stream() >> creationTime;
  object->setCreationTime(creationTime);
  u_signed64 accessTime;
  ad->stream() >> accessTime;
  object->setAccessTime(accessTime);
  unsigned int nbAccesses;
  ad->stream() >> nbAccesses;
  object->setNbAccesses(nbAccesses);
  std::string diskServer;
  ad->stream() >> diskServer;
  object->setDiskServer(diskServer);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamFileQryResponseCnv::marshalObject(castor::IObject* object,
                                                         castor::io::StreamAddress* address,
                                                         castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::rh::FileQryResponse* obj = 
    dynamic_cast<castor::rh::FileQryResponse*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileQryResponseCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                       castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


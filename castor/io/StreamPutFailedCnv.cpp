/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamPutFailedCnv.cpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamPutFailedCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamBaseCnv.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/stager/PutFailed.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <rfcntl.h>
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamPutFailedCnv>
  s_factoryStreamPutFailedCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamPutFailedCnv::StreamPutFailedCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamPutFailedCnv::~StreamPutFailedCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamPutFailedCnv::ObjType() {
  return castor::stager::PutFailed::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamPutFailedCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamPutFailedCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool,
                                               unsigned int)
  throw (castor::exception::Exception) {
  castor::stager::PutFailed* obj = 
    dynamic_cast<castor::stager::PutFailed*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << htolopnflg(obj->flags());
  ad->stream() << obj->userName();
  ad->stream() << obj->euid();
  ad->stream() << obj->egid();
  ad->stream() << obj->mask();
  ad->stream() << obj->pid();
  ad->stream() << obj->machine();
  ad->stream() << obj->svcClassName();
  ad->stream() << obj->userTag();
  ad->stream() << obj->reqId();
  ad->stream() << obj->creationTime();
  ad->stream() << obj->lastModificationTime();
  ad->stream() << obj->subReqId();
  ad->stream() << obj->fileId();
  ad->stream() << obj->nsHost();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamPutFailedCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::PutFailed* object = new castor::stager::PutFailed();
  // Now retrieve and set members
  u_signed64 flags;
  ad->stream() >> flags;
  flags = ltohopnflg(flags);
  object->setFlags(flags);
  std::string userName;
  ad->stream() >> userName;
  object->setUserName(userName);
  unsigned long euid;
  ad->stream() >> euid;
  object->setEuid(euid);
  unsigned long egid;
  ad->stream() >> egid;
  object->setEgid(egid);
  unsigned long mask;
  ad->stream() >> mask;
  object->setMask(mask);
  unsigned long pid;
  ad->stream() >> pid;
  object->setPid(pid);
  std::string machine;
  ad->stream() >> machine;
  object->setMachine(machine);
  std::string svcClassName;
  ad->stream() >> svcClassName;
  object->setSvcClassName(svcClassName);
  std::string userTag;
  ad->stream() >> userTag;
  object->setUserTag(userTag);
  std::string reqId;
  ad->stream() >> reqId;
  object->setReqId(reqId);
  u_signed64 creationTime;
  ad->stream() >> creationTime;
  object->setCreationTime(creationTime);
  u_signed64 lastModificationTime;
  ad->stream() >> lastModificationTime;
  object->setLastModificationTime(lastModificationTime);
  u_signed64 subReqId;
  ad->stream() >> subReqId;
  object->setSubReqId(subReqId);
  u_signed64 fileId;
  ad->stream() >> fileId;
  object->setFileId(fileId);
  std::string nsHost;
  ad->stream() >> nsHost;
  object->setNsHost(nsHost);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamPutFailedCnv::marshalObject(castor::IObject* object,
                                                   castor::io::StreamAddress* address,
                                                   castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::PutFailed* obj = 
    dynamic_cast<castor::stager::PutFailed*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->svcClass(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->client(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamPutFailedCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                 castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::PutFailed* obj = 
    dynamic_cast<castor::stager::PutFailed*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objSvcClass = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setSvcClass(dynamic_cast<castor::stager::SvcClass*>(objSvcClass));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objClient = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setClient(dynamic_cast<castor::IClient*>(objClient));
  return object;
}


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamDiskPoolQueryCnv.cpp
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
#include "StreamDiskPoolQueryCnv.hpp"
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
#include "castor/query/DiskPoolQuery.hpp"
#include "castor/query/DiskPoolQueryType.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <rfcntl.h>
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamDiskPoolQueryCnv>
  s_factoryStreamDiskPoolQueryCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamDiskPoolQueryCnv::StreamDiskPoolQueryCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamDiskPoolQueryCnv::~StreamDiskPoolQueryCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamDiskPoolQueryCnv::ObjType() {
  return castor::query::DiskPoolQuery::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamDiskPoolQueryCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamDiskPoolQueryCnv::createRep(castor::IAddress* address,
                                                   castor::IObject* object,
                                                   bool,
                                                   unsigned int)
   {
  castor::query::DiskPoolQuery* obj = 
    dynamic_cast<castor::query::DiskPoolQuery*>(object);
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
  ad->stream() << obj->diskPoolName();
  ad->stream() << obj->id();
  ad->stream() << obj->queryType();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDiskPoolQueryCnv::createObj(castor::IAddress* address)
   {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::query::DiskPoolQuery* object = new castor::query::DiskPoolQuery();
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
  std::string diskPoolName;
  ad->stream() >> diskPoolName;
  object->setDiskPoolName(diskPoolName);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int queryType;
  ad->stream() >> queryType;
  object->setQueryType((castor::query::DiskPoolQueryType)queryType);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamDiskPoolQueryCnv::marshalObject(castor::IObject* object,
                                                       castor::io::StreamAddress* address,
                                                       castor::ObjectSet& alreadyDone)
   {
  castor::query::DiskPoolQuery* obj = 
    dynamic_cast<castor::query::DiskPoolQuery*>(object);
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
castor::IObject* castor::io::StreamDiskPoolQueryCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                     castor::ObjectCatalog& newlyCreated)
   {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::query::DiskPoolQuery* obj = 
    dynamic_cast<castor::query::DiskPoolQuery*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objSvcClass = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setSvcClass(dynamic_cast<castor::stager::SvcClass*>(objSvcClass));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objClient = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setClient(dynamic_cast<castor::IClient*>(objClient));
  return object;
}


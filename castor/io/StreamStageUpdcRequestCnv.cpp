/******************************************************************************
 *                      castor/io/StreamStageUpdcRequestCnv.cpp
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
 * @(#)$RCSfile: StreamStageUpdcRequestCnv.cpp,v $ $Revision: 1.6 $ $Release$ $Date: 2004/10/11 16:44:39 $ $Author: sponcec3 $
 *
 * 
 *
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamStageUpdcRequestCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/IConverter.hpp"
#include "castor/IFactory.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/stager/ReqId.hpp"
#include "castor/stager/StageUpdcRequest.hpp"
#include "castor/stager/SubRequest.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamStageUpdcRequestCnv> s_factoryStreamStageUpdcRequestCnv;
const castor::IFactory<castor::IConverter>& StreamStageUpdcRequestCnvFactory = 
  s_factoryStreamStageUpdcRequestCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamStageUpdcRequestCnv::StreamStageUpdcRequestCnv() :
  StreamBaseCnv() {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamStageUpdcRequestCnv::~StreamStageUpdcRequestCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamStageUpdcRequestCnv::ObjType() {
  return castor::stager::StageUpdcRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamStageUpdcRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamStageUpdcRequestCnv::createRep(castor::IAddress* address,
                                                      castor::IObject* object,
                                                      bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageUpdcRequest* obj = 
    dynamic_cast<castor::stager::StageUpdcRequest*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->flags();
  ad->stream() << obj->userName();
  ad->stream() << obj->euid();
  ad->stream() << obj->egid();
  ad->stream() << obj->mask();
  ad->stream() << obj->pid();
  ad->stream() << obj->machine();
  ad->stream() << obj->svcClassName();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamStageUpdcRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::StageUpdcRequest* object = new castor::stager::StageUpdcRequest();
  // Now retrieve and set members
  u_signed64 flags;
  ad->stream() >> flags;
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
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamStageUpdcRequestCnv::marshalObject(castor::IObject* object,
                                                          castor::io::StreamAddress* address,
                                                          castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::StageUpdcRequest* obj = 
    dynamic_cast<castor::stager::StageUpdcRequest*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << 0;
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    cnvSvc()->createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->reqids().size();
    for (std::vector<castor::stager::ReqId*>::iterator it = obj->reqids().begin();
         it != obj->reqids().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    address->stream() << obj->subRequests().size();
    for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
         it != obj->subRequests().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    cnvSvc()->marshalObject(obj->client(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamStageUpdcRequestCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                        castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = cnvSvc()->createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::StageUpdcRequest* obj = 
    dynamic_cast<castor::stager::StageUpdcRequest*>(object);
  unsigned int reqidsNb;
  ad.stream() >> reqidsNb;
  for (unsigned int i = 0; i < reqidsNb; i++) {
    IObject* objReqids = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addReqids(dynamic_cast<castor::stager::ReqId*>(objReqids));
  }
  unsigned int subRequestsNb;
  ad.stream() >> subRequestsNb;
  for (unsigned int i = 0; i < subRequestsNb; i++) {
    IObject* objSubRequests = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addSubRequests(dynamic_cast<castor::stager::SubRequest*>(objSubRequests));
  }
  IObject* objClient = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setClient(dynamic_cast<castor::IClient*>(objClient));
}


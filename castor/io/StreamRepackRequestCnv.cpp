/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamRepackRequestCnv.cpp
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
#include "StreamRepackRequestCnv.hpp"
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
#include "castor/repack/RepackRequest.hpp"
#include "castor/repack/RepackSubRequest.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamRepackRequestCnv>* s_factoryStreamRepackRequestCnv =
  new castor::CnvFactory<castor::io::StreamRepackRequestCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamRepackRequestCnv::StreamRepackRequestCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamRepackRequestCnv::~StreamRepackRequestCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamRepackRequestCnv::ObjType() {
  return castor::repack::RepackRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamRepackRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamRepackRequestCnv::createRep(castor::IAddress* address,
                                                   castor::IObject* object,
                                                   bool autocommit,
                                                   unsigned int type)
  throw (castor::exception::Exception) {
  castor::repack::RepackRequest* obj = 
    dynamic_cast<castor::repack::RepackRequest*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->machine();
  ad->stream() << obj->userName();
  ad->stream() << obj->creationTime();
  ad->stream() << obj->pool();
  ad->stream() << obj->pid();
  ad->stream() << obj->svcclass();
  ad->stream() << obj->command();
  ad->stream() << obj->stager();
  ad->stream() << obj->uid();
  ad->stream() << obj->gid();
  ad->stream() << obj->retryMax();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamRepackRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::repack::RepackRequest* object = new castor::repack::RepackRequest();
  // Now retrieve and set members
  std::string machine;
  ad->stream() >> machine;
  object->setMachine(machine);
  std::string userName;
  ad->stream() >> userName;
  object->setUserName(userName);
  u_signed64 creationTime;
  ad->stream() >> creationTime;
  object->setCreationTime(creationTime);
  std::string pool;
  ad->stream() >> pool;
  object->setPool(pool);
  u_signed64 pid;
  ad->stream() >> pid;
  object->setPid(pid);
  std::string svcclass;
  ad->stream() >> svcclass;
  object->setSvcclass(svcclass);
  int command;
  ad->stream() >> command;
  object->setCommand(command);
  std::string stager;
  ad->stream() >> stager;
  object->setStager(stager);
  int uid;
  ad->stream() >> uid;
  object->setUid(uid);
  int gid;
  ad->stream() >> gid;
  object->setGid(gid);
  int retryMax;
  ad->stream() >> retryMax;
  object->setRetryMax(retryMax);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamRepackRequestCnv::marshalObject(castor::IObject* object,
                                                       castor::io::StreamAddress* address,
                                                       castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::repack::RepackRequest* obj = 
    dynamic_cast<castor::repack::RepackRequest*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->repacksubrequest().size();
    for (std::vector<castor::repack::RepackSubRequest*>::iterator it = obj->repacksubrequest().begin();
         it != obj->repacksubrequest().end();
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
castor::IObject* castor::io::StreamRepackRequestCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                     castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::repack::RepackRequest* obj = 
    dynamic_cast<castor::repack::RepackRequest*>(object);
  unsigned int repacksubrequestNb;
  ad.stream() >> repacksubrequestNb;
  for (unsigned int i = 0; i < repacksubrequestNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objRepacksubrequest = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addRepacksubrequest(dynamic_cast<castor::repack::RepackSubRequest*>(objRepacksubrequest));
  }
  return object;
}


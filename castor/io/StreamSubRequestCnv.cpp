/******************************************************************************
 *                      castor/io/StreamSubRequestCnv.cpp
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
#include "StreamSubRequestCnv.hpp"
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
#include "castor/stager/FileRequest.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamSubRequestCnv> s_factoryStreamSubRequestCnv;
const castor::ICnvFactory& StreamSubRequestCnvFactory = 
  s_factoryStreamSubRequestCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamSubRequestCnv::StreamSubRequestCnv(castor::ICnvSvc* cnvSvc) :
  StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamSubRequestCnv::~StreamSubRequestCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamSubRequestCnv::ObjType() {
  return castor::stager::SubRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamSubRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamSubRequestCnv::createRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                bool autocommit,
                                                unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::SubRequest* obj = 
    dynamic_cast<castor::stager::SubRequest*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->retryCounter();
  ad->stream() << obj->fileName();
  ad->stream() << obj->protocol();
  ad->stream() << obj->xsize();
  ad->stream() << obj->priority();
  ad->stream() << obj->subreqId();
  ad->stream() << obj->flags();
  ad->stream() << obj->modeBits();
  ad->stream() << obj->creationTime();
  ad->stream() << obj->lastModificationTime();
  ad->stream() << obj->id();
  ad->stream() << obj->status();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamSubRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::SubRequest* object = new castor::stager::SubRequest();
  // Now retrieve and set members
  unsigned int retryCounter;
  ad->stream() >> retryCounter;
  object->setRetryCounter(retryCounter);
  std::string fileName;
  ad->stream() >> fileName;
  object->setFileName(fileName);
  std::string protocol;
  ad->stream() >> protocol;
  object->setProtocol(protocol);
  u_signed64 xsize;
  ad->stream() >> xsize;
  object->setXsize(xsize);
  unsigned int priority;
  ad->stream() >> priority;
  object->setPriority(priority);
  std::string subreqId;
  ad->stream() >> subreqId;
  object->setSubreqId(subreqId);
  int flags;
  ad->stream() >> flags;
  object->setFlags(flags);
  int modeBits;
  ad->stream() >> modeBits;
  object->setModeBits(modeBits);
  u_signed64 creationTime;
  ad->stream() >> creationTime;
  object->setCreationTime(creationTime);
  u_signed64 lastModificationTime;
  ad->stream() >> lastModificationTime;
  object->setLastModificationTime(lastModificationTime);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int status;
  ad->stream() >> status;
  object->setStatus((castor::stager::SubRequestStatusCodes)status);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamSubRequestCnv::marshalObject(castor::IObject* object,
                                                    castor::io::StreamAddress* address,
                                                    castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::SubRequest* obj = 
    dynamic_cast<castor::stager::SubRequest*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->diskcopy(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->castorFile(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->parent(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->request(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamSubRequestCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                  castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::SubRequest* obj = 
    dynamic_cast<castor::stager::SubRequest*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objDiskcopy = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setDiskcopy(dynamic_cast<castor::stager::DiskCopy*>(objDiskcopy));
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objCastorFile = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setCastorFile(dynamic_cast<castor::stager::CastorFile*>(objCastorFile));
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objParent = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setParent(dynamic_cast<castor::stager::SubRequest*>(objParent));
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objRequest = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setRequest(dynamic_cast<castor::stager::FileRequest*>(objRequest));
  return object;
}


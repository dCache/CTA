/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
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
#include "StreamFilesToMigrateListRequestCnv.hpp"
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
#include "castor/tape/tapegateway/FilesToMigrateListRequest.hpp"
#include "osdep.h"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamFilesToMigrateListRequestCnv>
  s_factoryStreamFilesToMigrateListRequestCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamFilesToMigrateListRequestCnv::StreamFilesToMigrateListRequestCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamFilesToMigrateListRequestCnv::~StreamFilesToMigrateListRequestCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamFilesToMigrateListRequestCnv::ObjType() {
  return castor::tape::tapegateway::FilesToMigrateListRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamFilesToMigrateListRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamFilesToMigrateListRequestCnv::createRep(castor::IAddress* address,
                                                               castor::IObject* object,
                                                               bool,
                                                               unsigned int)
   {
  castor::tape::tapegateway::FilesToMigrateListRequest* obj = 
    dynamic_cast<castor::tape::tapegateway::FilesToMigrateListRequest*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->maxFiles();
  ad->stream() << obj->maxBytes();
  ad->stream() << obj->id();
  ad->stream() << obj->mountTransactionId();
  ad->stream() << obj->aggregatorTransactionId();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFilesToMigrateListRequestCnv::createObj(castor::IAddress* address)
   {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::FilesToMigrateListRequest* object = new castor::tape::tapegateway::FilesToMigrateListRequest();
  // Now retrieve and set members
  u_signed64 maxFiles;
  ad->stream() >> maxFiles;
  object->setMaxFiles(maxFiles);
  u_signed64 maxBytes;
  ad->stream() >> maxBytes;
  object->setMaxBytes(maxBytes);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  u_signed64 mountTransactionId;
  ad->stream() >> mountTransactionId;
  object->setMountTransactionId(mountTransactionId);
  u_signed64 aggregatorTransactionId;
  ad->stream() >> aggregatorTransactionId;
  object->setAggregatorTransactionId(aggregatorTransactionId);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamFilesToMigrateListRequestCnv::marshalObject(castor::IObject* object,
                                                                   castor::io::StreamAddress* address,
                                                                   castor::ObjectSet& alreadyDone)
   {
  castor::tape::tapegateway::FilesToMigrateListRequest* obj = 
    dynamic_cast<castor::tape::tapegateway::FilesToMigrateListRequest*>(object);
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
castor::IObject* castor::io::StreamFilesToMigrateListRequestCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                                 castor::ObjectCatalog& newlyCreated)
   {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


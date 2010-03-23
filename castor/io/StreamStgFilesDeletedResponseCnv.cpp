/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamStgFilesDeletedResponseCnv.cpp
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
#include "StreamStgFilesDeletedResponseCnv.hpp"
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
#include "castor/stager/GCFile.hpp"
#include "castor/stager/StgFilesDeletedResponse.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamStgFilesDeletedResponseCnv>* s_factoryStreamStgFilesDeletedResponseCnv =
  new castor::CnvFactory<castor::io::StreamStgFilesDeletedResponseCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamStgFilesDeletedResponseCnv::StreamStgFilesDeletedResponseCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamStgFilesDeletedResponseCnv::~StreamStgFilesDeletedResponseCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamStgFilesDeletedResponseCnv::ObjType() {
  return castor::stager::StgFilesDeletedResponse::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamStgFilesDeletedResponseCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamStgFilesDeletedResponseCnv::createRep(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             bool,
                                                             unsigned int)
  throw (castor::exception::Exception) {
  castor::stager::StgFilesDeletedResponse* obj = 
    dynamic_cast<castor::stager::StgFilesDeletedResponse*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->errorCode();
  ad->stream() << obj->errorMessage();
  ad->stream() << obj->reqAssociated();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamStgFilesDeletedResponseCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::StgFilesDeletedResponse* object = new castor::stager::StgFilesDeletedResponse();
  // Now retrieve and set members
  unsigned int errorCode;
  ad->stream() >> errorCode;
  object->setErrorCode(errorCode);
  std::string errorMessage;
  ad->stream() >> errorMessage;
  object->setErrorMessage(errorMessage);
  std::string reqAssociated;
  ad->stream() >> reqAssociated;
  object->setReqAssociated(reqAssociated);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamStgFilesDeletedResponseCnv::marshalObject(castor::IObject* object,
                                                                 castor::io::StreamAddress* address,
                                                                 castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::StgFilesDeletedResponse* obj = 
    dynamic_cast<castor::stager::StgFilesDeletedResponse*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->orphanFileIds().size();
    for (std::vector<castor::stager::GCFile*>::iterator it = obj->orphanFileIds().begin();
         it != obj->orphanFileIds().end();
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
castor::IObject* castor::io::StreamStgFilesDeletedResponseCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                               castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::StgFilesDeletedResponse* obj = 
    dynamic_cast<castor::stager::StgFilesDeletedResponse*>(object);
  unsigned int orphanFileIdsNb;
  ad.stream() >> orphanFileIdsNb;
  for (unsigned int i = 0; i < orphanFileIdsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objOrphanFileIds = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addOrphanFileIds(dynamic_cast<castor::stager::GCFile*>(objOrphanFileIds));
  }
  return object;
}


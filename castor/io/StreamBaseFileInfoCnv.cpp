/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamBaseFileInfoCnv.cpp
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
#include "StreamBaseFileInfoCnv.hpp"
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
#include "castor/tape/tapegateway/BaseFileInfo.hpp"
#include "castor/tape/tapegateway/PositionCommandCode.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamBaseFileInfoCnv>* s_factoryStreamBaseFileInfoCnv =
  new castor::CnvFactory<castor::io::StreamBaseFileInfoCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamBaseFileInfoCnv::StreamBaseFileInfoCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamBaseFileInfoCnv::~StreamBaseFileInfoCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamBaseFileInfoCnv::ObjType() {
  return castor::tape::tapegateway::BaseFileInfo::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamBaseFileInfoCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamBaseFileInfoCnv::createRep(castor::IAddress* address,
                                                  castor::IObject* object,
                                                  bool endTransaction,
                                                  unsigned int type)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::BaseFileInfo* obj = 
    dynamic_cast<castor::tape::tapegateway::BaseFileInfo*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->transactionId();
  ad->stream() << obj->path();
  ad->stream() << obj->nshost();
  ad->stream() << obj->fileid();
  ad->stream() << obj->fseq();
  ad->stream() << obj->blockId();
  ad->stream() << obj->id();
  ad->stream() << obj->positionCommandCode();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamBaseFileInfoCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::BaseFileInfo* object = new castor::tape::tapegateway::BaseFileInfo();
  // Now retrieve and set members
  u_signed64 transactionId;
  ad->stream() >> transactionId;
  object->setTransactionId(transactionId);
  std::string path;
  ad->stream() >> path;
  object->setPath(path);
  std::string nshost;
  ad->stream() >> nshost;
  object->setNshost(nshost);
  u_signed64 fileid;
  ad->stream() >> fileid;
  object->setFileid(fileid);
  int fseq;
  ad->stream() >> fseq;
  object->setFseq(fseq);
  u_signed64 blockId;
  ad->stream() >> blockId;
  object->setBlockId(blockId);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int positionCommandCode;
  ad->stream() >> positionCommandCode;
  object->setPositionCommandCode((castor::tape::tapegateway::PositionCommandCode)positionCommandCode);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamBaseFileInfoCnv::marshalObject(castor::IObject* object,
                                                      castor::io::StreamAddress* address,
                                                      castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::BaseFileInfo* obj = 
    dynamic_cast<castor::tape::tapegateway::BaseFileInfo*>(object);
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
castor::IObject* castor::io::StreamBaseFileInfoCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                    castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


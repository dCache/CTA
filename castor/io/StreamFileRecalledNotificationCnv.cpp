/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamFileRecalledNotificationCnv.cpp
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
#include "StreamFileRecalledNotificationCnv.hpp"
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
#include "castor/tape/tapegateway/FileRecalledNotification.hpp"
#include "castor/tape/tapegateway/PositionCommandCode.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamFileRecalledNotificationCnv>* s_factoryStreamFileRecalledNotificationCnv =
  new castor::CnvFactory<castor::io::StreamFileRecalledNotificationCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamFileRecalledNotificationCnv::StreamFileRecalledNotificationCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamFileRecalledNotificationCnv::~StreamFileRecalledNotificationCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamFileRecalledNotificationCnv::ObjType() {
  return castor::tape::tapegateway::FileRecalledNotification::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamFileRecalledNotificationCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamFileRecalledNotificationCnv::createRep(castor::IAddress* address,
                                                              castor::IObject* object,
                                                              bool,
                                                              unsigned int)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::FileRecalledNotification* obj = 
    dynamic_cast<castor::tape::tapegateway::FileRecalledNotification*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->nshost();
  ad->stream() << obj->fileid();
  ad->stream() << obj->fseq();
  ad->stream() << obj->mountTransactionId();
  ad->stream() << obj->aggregatorTransactionId();
  ad->stream() << obj->path();
  ad->stream() << obj->checksumName();
  ad->stream() << obj->checksum();
  ad->stream() << obj->fileSize();
  ad->stream() << obj->id();
  ad->stream() << obj->positionCommandCode();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileRecalledNotificationCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::FileRecalledNotification* object = new castor::tape::tapegateway::FileRecalledNotification();
  // Now retrieve and set members
  std::string nshost;
  ad->stream() >> nshost;
  object->setNshost(nshost);
  u_signed64 fileid;
  ad->stream() >> fileid;
  object->setFileid(fileid);
  int fseq;
  ad->stream() >> fseq;
  object->setFseq(fseq);
  u_signed64 mountTransactionId;
  ad->stream() >> mountTransactionId;
  object->setMountTransactionId(mountTransactionId);
  u_signed64 aggregatorTransactionId;
  ad->stream() >> aggregatorTransactionId;
  object->setAggregatorTransactionId(aggregatorTransactionId);
  std::string path;
  ad->stream() >> path;
  object->setPath(path);
  std::string checksumName;
  ad->stream() >> checksumName;
  object->setChecksumName(checksumName);
  u_signed64 checksum;
  ad->stream() >> checksum;
  object->setChecksum(checksum);
  u_signed64 fileSize;
  ad->stream() >> fileSize;
  object->setFileSize(fileSize);
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
void castor::io::StreamFileRecalledNotificationCnv::marshalObject(castor::IObject* object,
                                                                  castor::io::StreamAddress* address,
                                                                  castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::FileRecalledNotification* obj = 
    dynamic_cast<castor::tape::tapegateway::FileRecalledNotification*>(object);
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
castor::IObject* castor::io::StreamFileRecalledNotificationCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                                castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


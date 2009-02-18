/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamTapeFileNsAttributeCnv.cpp
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
 * @(#)$RCSfile: StreamTapeFileNsAttributeCnv.cpp,v $ $Revision: 1.11 $ $Release$ $Date: 2009/02/18 13:19:53 $ $Author: gtaur $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamTapeFileNsAttributeCnv.hpp"
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
#include "castor/tape/tapegateway/TapeFileNsAttribute.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamTapeFileNsAttributeCnv>* s_factoryStreamTapeFileNsAttributeCnv =
  new castor::CnvFactory<castor::io::StreamTapeFileNsAttributeCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamTapeFileNsAttributeCnv::StreamTapeFileNsAttributeCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamTapeFileNsAttributeCnv::~StreamTapeFileNsAttributeCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeFileNsAttributeCnv::ObjType() {
  return castor::tape::tapegateway::TapeFileNsAttribute::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeFileNsAttributeCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamTapeFileNsAttributeCnv::createRep(castor::IAddress* address,
                                                         castor::IObject* object,
                                                         bool endTransaction,
                                                         unsigned int type)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::TapeFileNsAttribute* obj = 
    dynamic_cast<castor::tape::tapegateway::TapeFileNsAttribute*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->copyNo();
  ad->stream() << obj->fsec();
  ad->stream() << obj->compression();
  ad->stream() << obj->side();
  ad->stream() << obj->checksumName();
  ad->stream() << obj->checksum();
  ad->stream() << obj->vid();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeFileNsAttributeCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::TapeFileNsAttribute* object = new castor::tape::tapegateway::TapeFileNsAttribute();
  // Now retrieve and set members
  int copyNo;
  ad->stream() >> copyNo;
  object->setCopyNo(copyNo);
  int fsec;
  ad->stream() >> fsec;
  object->setFsec(fsec);
  int compression;
  ad->stream() >> compression;
  object->setCompression(compression);
  int side;
  ad->stream() >> side;
  object->setSide(side);
  std::string checksumName;
  ad->stream() >> checksumName;
  object->setChecksumName(checksumName);
  u_signed64 checksum;
  ad->stream() >> checksum;
  object->setChecksum(checksum);
  std::string vid;
  ad->stream() >> vid;
  object->setVid(vid);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamTapeFileNsAttributeCnv::marshalObject(castor::IObject* object,
                                                             castor::io::StreamAddress* address,
                                                             castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::TapeFileNsAttribute* obj = 
    dynamic_cast<castor::tape::tapegateway::TapeFileNsAttribute*>(object);
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
castor::IObject* castor::io::StreamTapeFileNsAttributeCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                           castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


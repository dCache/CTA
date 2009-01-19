/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamSegmentCnv.cpp
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
#include "StreamSegmentCnv.hpp"
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
#include "castor/stager/Segment.hpp"
#include "castor/stager/SegmentStatusCodes.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamSegmentCnv>* s_factoryStreamSegmentCnv =
  new castor::CnvFactory<castor::io::StreamSegmentCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamSegmentCnv::StreamSegmentCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamSegmentCnv::~StreamSegmentCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamSegmentCnv::ObjType() {
  return castor::stager::Segment::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamSegmentCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamSegmentCnv::createRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             bool endTransaction,
                                             unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::Segment* obj = 
    dynamic_cast<castor::stager::Segment*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->fseq();
  ad->stream() << obj->offset();
  ad->stream() << obj->bytes_in();
  ad->stream() << obj->bytes_out();
  ad->stream() << obj->host_bytes();
  ad->stream() << obj->segmCksumAlgorithm();
  ad->stream() << obj->segmCksum();
  ad->stream() << obj->errMsgTxt();
  ad->stream() << obj->errorCode();
  ad->stream() << obj->severity();
  ad->stream() << obj->blockId0();
  ad->stream() << obj->blockId1();
  ad->stream() << obj->blockId2();
  ad->stream() << obj->blockId3();
  ad->stream() << obj->creationTime();
  ad->stream() << obj->priority();
  ad->stream() << obj->id();
  ad->stream() << obj->status();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamSegmentCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::Segment* object = new castor::stager::Segment();
  // Now retrieve and set members
  int fseq;
  ad->stream() >> fseq;
  object->setFseq(fseq);
  u_signed64 offset;
  ad->stream() >> offset;
  object->setOffset(offset);
  u_signed64 bytes_in;
  ad->stream() >> bytes_in;
  object->setBytes_in(bytes_in);
  u_signed64 bytes_out;
  ad->stream() >> bytes_out;
  object->setBytes_out(bytes_out);
  u_signed64 host_bytes;
  ad->stream() >> host_bytes;
  object->setHost_bytes(host_bytes);
  std::string segmCksumAlgorithm;
  ad->stream() >> segmCksumAlgorithm;
  object->setSegmCksumAlgorithm(segmCksumAlgorithm);
  unsigned long segmCksum;
  ad->stream() >> segmCksum;
  object->setSegmCksum(segmCksum);
  std::string errMsgTxt;
  ad->stream() >> errMsgTxt;
  object->setErrMsgTxt(errMsgTxt);
  int errorCode;
  ad->stream() >> errorCode;
  object->setErrorCode(errorCode);
  int severity;
  ad->stream() >> severity;
  object->setSeverity(severity);
  unsigned char blockId0;
  ad->stream() >> blockId0;
  object->setBlockId0(blockId0);
  unsigned char blockId1;
  ad->stream() >> blockId1;
  object->setBlockId1(blockId1);
  unsigned char blockId2;
  ad->stream() >> blockId2;
  object->setBlockId2(blockId2);
  unsigned char blockId3;
  ad->stream() >> blockId3;
  object->setBlockId3(blockId3);
  u_signed64 creationTime;
  ad->stream() >> creationTime;
  object->setCreationTime(creationTime);
  u_signed64 priority;
  ad->stream() >> priority;
  object->setPriority(priority);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int status;
  ad->stream() >> status;
  object->setStatus((castor::stager::SegmentStatusCodes)status);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamSegmentCnv::marshalObject(castor::IObject* object,
                                                 castor::io::StreamAddress* address,
                                                 castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::Segment* obj = 
    dynamic_cast<castor::stager::Segment*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->copy(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->tape(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamSegmentCnv::unmarshalObject(castor::io::biniostream& stream,
                                                               castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::Segment* obj = 
    dynamic_cast<castor::stager::Segment*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objCopy = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setCopy(dynamic_cast<castor::stager::TapeCopy*>(objCopy));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objTape = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setTape(dynamic_cast<castor::stager::Tape*>(objTape));
  return object;
}


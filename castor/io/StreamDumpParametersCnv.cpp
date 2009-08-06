/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamDumpParametersCnv.cpp
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
#include "StreamDumpParametersCnv.hpp"
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
#include "castor/tape/tapegateway/DumpParameters.hpp"
#include "osdep.h"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamDumpParametersCnv>* s_factoryStreamDumpParametersCnv =
  new castor::CnvFactory<castor::io::StreamDumpParametersCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamDumpParametersCnv::StreamDumpParametersCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamDumpParametersCnv::~StreamDumpParametersCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamDumpParametersCnv::ObjType() {
  return castor::tape::tapegateway::DumpParameters::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamDumpParametersCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamDumpParametersCnv::createRep(castor::IAddress* address,
                                                    castor::IObject* object,
                                                    bool endTransaction,
                                                    unsigned int type)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::DumpParameters* obj = 
    dynamic_cast<castor::tape::tapegateway::DumpParameters*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->mountTransactionId();
  ad->stream() << obj->tapeMaxBytes();
  ad->stream() << obj->tapeBlockSize();
  ad->stream() << obj->tapeConverter();
  ad->stream() << obj->tapeErrAction();
  ad->stream() << obj->tapeStartFile();
  ad->stream() << obj->tapeMaxFile();
  ad->stream() << obj->tapeFromBlock();
  ad->stream() << obj->tapeToBlock();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDumpParametersCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::DumpParameters* object = new castor::tape::tapegateway::DumpParameters();
  // Now retrieve and set members
  u_signed64 mountTransactionId;
  ad->stream() >> mountTransactionId;
  object->setMountTransactionId(mountTransactionId);
  int tapeMaxBytes;
  ad->stream() >> tapeMaxBytes;
  object->setTapeMaxBytes(tapeMaxBytes);
  int tapeBlockSize;
  ad->stream() >> tapeBlockSize;
  object->setTapeBlockSize(tapeBlockSize);
  int tapeConverter;
  ad->stream() >> tapeConverter;
  object->setTapeConverter(tapeConverter);
  int tapeErrAction;
  ad->stream() >> tapeErrAction;
  object->setTapeErrAction(tapeErrAction);
  int tapeStartFile;
  ad->stream() >> tapeStartFile;
  object->setTapeStartFile(tapeStartFile);
  int tapeMaxFile;
  ad->stream() >> tapeMaxFile;
  object->setTapeMaxFile(tapeMaxFile);
  int tapeFromBlock;
  ad->stream() >> tapeFromBlock;
  object->setTapeFromBlock(tapeFromBlock);
  int tapeToBlock;
  ad->stream() >> tapeToBlock;
  object->setTapeToBlock(tapeToBlock);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamDumpParametersCnv::marshalObject(castor::IObject* object,
                                                        castor::io::StreamAddress* address,
                                                        castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::DumpParameters* obj = 
    dynamic_cast<castor::tape::tapegateway::DumpParameters*>(object);
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
castor::IObject* castor::io::StreamDumpParametersCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                      castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamRepackCommandCodeCnv.cpp
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
#include "StreamRepackCommandCodeCnv.hpp"
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
#include "castor/repack/RepackAck.hpp"
#include "castor/repack/RepackCommandCode.hpp"
#include "castor/repack/RepackRequest.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamRepackCommandCodeCnv>* s_factoryStreamRepackCommandCodeCnv =
  new castor::CnvFactory<castor::io::StreamRepackCommandCodeCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamRepackCommandCodeCnv::StreamRepackCommandCodeCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamRepackCommandCodeCnv::~StreamRepackCommandCodeCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamRepackCommandCodeCnv::ObjType() {
  return castor::repack::RepackCommandCode::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamRepackCommandCodeCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamRepackCommandCodeCnv::createRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool,
                                                       unsigned int)
  throw (castor::exception::Exception) {
  castor::repack::RepackCommandCode* obj = 
    dynamic_cast<castor::repack::RepackCommandCode*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->REPACK();
  ad->stream() << obj->RESTART();
  ad->stream() << obj->REMOVE();
  ad->stream() << obj->GET_STATUS();
  ad->stream() << obj->GET_STATUS_ALL();
  ad->stream() << obj->ARCHIVE();
  ad->stream() << obj->ARCHIVE_ALL();
  ad->stream() << obj->GET_NS_STATUS();
  ad->stream() << obj->GET_ERROR();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamRepackCommandCodeCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::repack::RepackCommandCode* object = new castor::repack::RepackCommandCode();
  // Now retrieve and set members
  int REPACK;
  ad->stream() >> REPACK;
  object->setREPACK(REPACK);
  int RESTART;
  ad->stream() >> RESTART;
  object->setRESTART(RESTART);
  int REMOVE;
  ad->stream() >> REMOVE;
  object->setREMOVE(REMOVE);
  int GET_STATUS;
  ad->stream() >> GET_STATUS;
  object->setGET_STATUS(GET_STATUS);
  int GET_STATUS_ALL;
  ad->stream() >> GET_STATUS_ALL;
  object->setGET_STATUS_ALL(GET_STATUS_ALL);
  int ARCHIVE;
  ad->stream() >> ARCHIVE;
  object->setARCHIVE(ARCHIVE);
  int ARCHIVE_ALL;
  ad->stream() >> ARCHIVE_ALL;
  object->setARCHIVE_ALL(ARCHIVE_ALL);
  int GET_NS_STATUS;
  ad->stream() >> GET_NS_STATUS;
  object->setGET_NS_STATUS(GET_NS_STATUS);
  int GET_ERROR;
  ad->stream() >> GET_ERROR;
  object->setGET_ERROR(GET_ERROR);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamRepackCommandCodeCnv::marshalObject(castor::IObject* object,
                                                           castor::io::StreamAddress* address,
                                                           castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::repack::RepackCommandCode* obj = 
    dynamic_cast<castor::repack::RepackCommandCode*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->command(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->command(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamRepackCommandCodeCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                         castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::repack::RepackCommandCode* obj = 
    dynamic_cast<castor::repack::RepackCommandCode*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objCommand = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setCommand(dynamic_cast<castor::repack::RepackAck*>(objCommand));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objCommand = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setCommand(dynamic_cast<castor::repack::RepackRequest*>(objCommand));
  return object;
}


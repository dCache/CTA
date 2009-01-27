/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamNsFileInformationCnv.cpp
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
 * @(#)$RCSfile: StreamNsFileInformationCnv.cpp,v $ $Revision: 1.3 $ $Release$ $Date: 2009/01/27 16:07:41 $ $Author: gtaur $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamNsFileInformationCnv.hpp"
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
#include "castor/tape/tapegateway/NsFileInformation.hpp"
#include "castor/tape/tapegateway/TapeFileNsAttribute.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamNsFileInformationCnv>* s_factoryStreamNsFileInformationCnv =
  new castor::CnvFactory<castor::io::StreamNsFileInformationCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamNsFileInformationCnv::StreamNsFileInformationCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamNsFileInformationCnv::~StreamNsFileInformationCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamNsFileInformationCnv::ObjType() {
  return castor::tape::tapegateway::NsFileInformation::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamNsFileInformationCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamNsFileInformationCnv::createRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool endTransaction,
                                                       unsigned int type)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::NsFileInformation* obj = 
    dynamic_cast<castor::tape::tapegateway::NsFileInformation*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->nshost();
  ad->stream() << obj->fileid();
  ad->stream() << obj->fileSize();
  ad->stream() << obj->lastModificationTime();
  ad->stream() << obj->lastKnownFileName();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamNsFileInformationCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::NsFileInformation* object = new castor::tape::tapegateway::NsFileInformation();
  // Now retrieve and set members
  std::string nshost;
  ad->stream() >> nshost;
  object->setNshost(nshost);
  u_signed64 fileid;
  ad->stream() >> fileid;
  object->setFileid(fileid);
  u_signed64 fileSize;
  ad->stream() >> fileSize;
  object->setFileSize(fileSize);
  u_signed64 lastModificationTime;
  ad->stream() >> lastModificationTime;
  object->setLastModificationTime(lastModificationTime);
  std::string lastKnownFileName;
  ad->stream() >> lastKnownFileName;
  object->setLastKnownFileName(lastKnownFileName);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamNsFileInformationCnv::marshalObject(castor::IObject* object,
                                                           castor::io::StreamAddress* address,
                                                           castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::NsFileInformation* obj = 
    dynamic_cast<castor::tape::tapegateway::NsFileInformation*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->tapeFileNsAttribute(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamNsFileInformationCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                         castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::tape::tapegateway::NsFileInformation* obj = 
    dynamic_cast<castor::tape::tapegateway::NsFileInformation*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objTapeFileNsAttribute = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setTapeFileNsAttribute(dynamic_cast<castor::tape::tapegateway::TapeFileNsAttribute*>(objTapeFileNsAttribute));
  return object;
}


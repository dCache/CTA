/******************************************************************************
 *                      castor/io/StreamCastorFileCnv.cpp
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
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamCastorFileCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IConverter.hpp"
#include "castor/IFactory.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/FileClass.hpp"
#include "castor/stager/SvcClass.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamCastorFileCnv> s_factoryStreamCastorFileCnv;
const castor::IFactory<castor::IConverter>& StreamCastorFileCnvFactory = 
  s_factoryStreamCastorFileCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamCastorFileCnv::StreamCastorFileCnv() :
  StreamBaseCnv() {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamCastorFileCnv::~StreamCastorFileCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamCastorFileCnv::ObjType() {
  return castor::stager::CastorFile::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamCastorFileCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamCastorFileCnv::createRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                bool autocommit,
                                                unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::CastorFile* obj = 
    dynamic_cast<castor::stager::CastorFile*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->fileId();
  ad->stream() << obj->nsHost();
  ad->stream() << obj->fileSize();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamCastorFileCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::CastorFile* object = new castor::stager::CastorFile();
  // Now retrieve and set members
  u_signed64 fileId;
  ad->stream() >> fileId;
  object->setFileId(fileId);
  std::string nsHost;
  ad->stream() >> nsHost;
  object->setNsHost(nsHost);
  u_signed64 fileSize;
  ad->stream() >> fileSize;
  object->setFileSize(fileSize);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamCastorFileCnv::marshalObject(castor::IObject* object,
                                                    castor::io::StreamAddress* address,
                                                    castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::CastorFile* obj = 
    dynamic_cast<castor::stager::CastorFile*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << 0;
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->svcClass(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->fileClass(), address, alreadyDone);
    address->stream() << obj->diskCopies().size();
    for (std::vector<castor::stager::DiskCopy*>::iterator it = obj->diskCopies().begin();
         it != obj->diskCopies().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    address->stream() << obj->tapeCopies().size();
    for (std::vector<castor::stager::TapeCopy*>::iterator it = obj->tapeCopies().begin();
         it != obj->tapeCopies().end();
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
castor::IObject* castor::io::StreamCastorFileCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                  castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = cnvSvc()->createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::CastorFile* obj = 
    dynamic_cast<castor::stager::CastorFile*>(object);
  IObject* objSvcClass = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setSvcClass(dynamic_cast<castor::stager::SvcClass*>(objSvcClass));
  IObject* objFileClass = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setFileClass(dynamic_cast<castor::stager::FileClass*>(objFileClass));
  unsigned int diskCopiesNb;
  ad.stream() >> diskCopiesNb;
  for (unsigned int i = 0; i < diskCopiesNb; i++) {
    IObject* objDiskCopies = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addDiskCopies(dynamic_cast<castor::stager::DiskCopy*>(objDiskCopies));
  }
  unsigned int tapeCopiesNb;
  ad.stream() >> tapeCopiesNb;
  for (unsigned int i = 0; i < tapeCopiesNb; i++) {
    IObject* objTapeCopies = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addTapeCopies(dynamic_cast<castor::stager::TapeCopy*>(objTapeCopies));
  }
return object;
}


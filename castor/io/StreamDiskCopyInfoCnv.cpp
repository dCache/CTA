/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamDiskCopyInfoCnv.cpp
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
#include "StreamDiskCopyInfoCnv.hpp"
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
#include "castor/stager/DiskCopyInfo.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamDiskCopyInfoCnv>* s_factoryStreamDiskCopyInfoCnv =
  new castor::CnvFactory<castor::io::StreamDiskCopyInfoCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamDiskCopyInfoCnv::StreamDiskCopyInfoCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamDiskCopyInfoCnv::~StreamDiskCopyInfoCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamDiskCopyInfoCnv::ObjType() {
  return castor::stager::DiskCopyInfo::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamDiskCopyInfoCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamDiskCopyInfoCnv::createRep(castor::IAddress* address,
                                                  castor::IObject* object,
                                                  bool,
                                                  unsigned int)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopyInfo* obj = 
    dynamic_cast<castor::stager::DiskCopyInfo*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->diskCopyPath();
  ad->stream() << obj->size();
  ad->stream() << obj->diskCopyStatus();
  ad->stream() << obj->tapeCopyStatus();
  ad->stream() << obj->segmentStatus();
  ad->stream() << obj->diskCopyId();
  ad->stream() << obj->fileId();
  ad->stream() << obj->nsHost();
  ad->stream() << obj->diskServer();
  ad->stream() << obj->mountPoint();
  ad->stream() << obj->nbAccesses();
  ad->stream() << obj->lastKnownFileName();
  ad->stream() << obj->svcClass();
  ad->stream() << obj->csumType();
  ad->stream() << obj->csumValue();
  ad->stream() << obj->creationTime();
  ad->stream() << obj->lastAccessTime();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDiskCopyInfoCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::DiskCopyInfo* object = new castor::stager::DiskCopyInfo();
  // Now retrieve and set members
  std::string diskCopyPath;
  ad->stream() >> diskCopyPath;
  object->setDiskCopyPath(diskCopyPath);
  u_signed64 size;
  ad->stream() >> size;
  object->setSize(size);
  int diskCopyStatus;
  ad->stream() >> diskCopyStatus;
  object->setDiskCopyStatus(diskCopyStatus);
  int tapeCopyStatus;
  ad->stream() >> tapeCopyStatus;
  object->setTapeCopyStatus(tapeCopyStatus);
  int segmentStatus;
  ad->stream() >> segmentStatus;
  object->setSegmentStatus(segmentStatus);
  u_signed64 diskCopyId;
  ad->stream() >> diskCopyId;
  object->setDiskCopyId(diskCopyId);
  u_signed64 fileId;
  ad->stream() >> fileId;
  object->setFileId(fileId);
  std::string nsHost;
  ad->stream() >> nsHost;
  object->setNsHost(nsHost);
  std::string diskServer;
  ad->stream() >> diskServer;
  object->setDiskServer(diskServer);
  std::string mountPoint;
  ad->stream() >> mountPoint;
  object->setMountPoint(mountPoint);
  unsigned int nbAccesses;
  ad->stream() >> nbAccesses;
  object->setNbAccesses(nbAccesses);
  std::string lastKnownFileName;
  ad->stream() >> lastKnownFileName;
  object->setLastKnownFileName(lastKnownFileName);
  std::string svcClass;
  ad->stream() >> svcClass;
  object->setSvcClass(svcClass);
  std::string csumType;
  ad->stream() >> csumType;
  object->setCsumType(csumType);
  std::string csumValue;
  ad->stream() >> csumValue;
  object->setCsumValue(csumValue);
  u_signed64 creationTime;
  ad->stream() >> creationTime;
  object->setCreationTime(creationTime);
  u_signed64 lastAccessTime;
  ad->stream() >> lastAccessTime;
  object->setLastAccessTime(lastAccessTime);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamDiskCopyInfoCnv::marshalObject(castor::IObject* object,
                                                      castor::io::StreamAddress* address,
                                                      castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopyInfo* obj = 
    dynamic_cast<castor::stager::DiskCopyInfo*>(object);
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
castor::IObject* castor::io::StreamDiskCopyInfoCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                    castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


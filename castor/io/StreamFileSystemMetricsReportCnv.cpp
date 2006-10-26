/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamFileSystemMetricsReportCnv.cpp
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
#include "StreamFileSystemMetricsReportCnv.hpp"
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
#include "castor/monitoring/DiskServerMetricsReport.hpp"
#include "castor/monitoring/FileSystemMetricsReport.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamFileSystemMetricsReportCnv>* s_factoryStreamFileSystemMetricsReportCnv =
  new castor::CnvFactory<castor::io::StreamFileSystemMetricsReportCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamFileSystemMetricsReportCnv::StreamFileSystemMetricsReportCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamFileSystemMetricsReportCnv::~StreamFileSystemMetricsReportCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamFileSystemMetricsReportCnv::ObjType() {
  return castor::monitoring::FileSystemMetricsReport::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamFileSystemMetricsReportCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamFileSystemMetricsReportCnv::createRep(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             bool autocommit,
                                                             unsigned int type)
  throw (castor::exception::Exception) {
  castor::monitoring::FileSystemMetricsReport* obj = 
    dynamic_cast<castor::monitoring::FileSystemMetricsReport*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->writeRate();
  ad->stream() << obj->mountPoint();
  ad->stream() << obj->readRate();
  ad->stream() << obj->readStreams();
  ad->stream() << obj->writeStreams();
  ad->stream() << obj->readWriteStreams();
  ad->stream() << obj->freeSpace();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileSystemMetricsReportCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::monitoring::FileSystemMetricsReport* object = new castor::monitoring::FileSystemMetricsReport();
  // Now retrieve and set members
  u_signed64 writeRate;
  ad->stream() >> writeRate;
  object->setWriteRate(writeRate);
  std::string mountPoint;
  ad->stream() >> mountPoint;
  object->setMountPoint(mountPoint);
  u_signed64 readRate;
  ad->stream() >> readRate;
  object->setReadRate(readRate);
  unsigned int readStreams;
  ad->stream() >> readStreams;
  object->setReadStreams(readStreams);
  unsigned int writeStreams;
  ad->stream() >> writeStreams;
  object->setWriteStreams(writeStreams);
  unsigned int readWriteStreams;
  ad->stream() >> readWriteStreams;
  object->setReadWriteStreams(readWriteStreams);
  u_signed64 freeSpace;
  ad->stream() >> freeSpace;
  object->setFreeSpace(freeSpace);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamFileSystemMetricsReportCnv::marshalObject(castor::IObject* object,
                                                                 castor::io::StreamAddress* address,
                                                                 castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::monitoring::FileSystemMetricsReport* obj = 
    dynamic_cast<castor::monitoring::FileSystemMetricsReport*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->diskServer(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileSystemMetricsReportCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                               castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::monitoring::FileSystemMetricsReport* obj = 
    dynamic_cast<castor::monitoring::FileSystemMetricsReport*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objDiskServer = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setDiskServer(dynamic_cast<castor::monitoring::DiskServerMetricsReport*>(objDiskServer));
  return object;
}


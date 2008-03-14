/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamDiskServerMetricsReportCnv.cpp
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
#include "StreamDiskServerMetricsReportCnv.hpp"
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
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamDiskServerMetricsReportCnv>* s_factoryStreamDiskServerMetricsReportCnv =
  new castor::CnvFactory<castor::io::StreamDiskServerMetricsReportCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamDiskServerMetricsReportCnv::StreamDiskServerMetricsReportCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamDiskServerMetricsReportCnv::~StreamDiskServerMetricsReportCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamDiskServerMetricsReportCnv::ObjType() {
  return castor::monitoring::DiskServerMetricsReport::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamDiskServerMetricsReportCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamDiskServerMetricsReportCnv::createRep(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             bool endTransaction,
                                                             unsigned int type)
  throw (castor::exception::Exception) {
  castor::monitoring::DiskServerMetricsReport* obj = 
    dynamic_cast<castor::monitoring::DiskServerMetricsReport*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->name();
  ad->stream() << obj->freeRam();
  ad->stream() << obj->freeMemory();
  ad->stream() << obj->freeSwap();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDiskServerMetricsReportCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::monitoring::DiskServerMetricsReport* object = new castor::monitoring::DiskServerMetricsReport();
  // Now retrieve and set members
  std::string name;
  ad->stream() >> name;
  object->setName(name);
  u_signed64 freeRam;
  ad->stream() >> freeRam;
  object->setFreeRam(freeRam);
  u_signed64 freeMemory;
  ad->stream() >> freeMemory;
  object->setFreeMemory(freeMemory);
  u_signed64 freeSwap;
  ad->stream() >> freeSwap;
  object->setFreeSwap(freeSwap);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamDiskServerMetricsReportCnv::marshalObject(castor::IObject* object,
                                                                 castor::io::StreamAddress* address,
                                                                 castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::monitoring::DiskServerMetricsReport* obj = 
    dynamic_cast<castor::monitoring::DiskServerMetricsReport*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->fileSystemMetricsReports().size();
    for (std::vector<castor::monitoring::FileSystemMetricsReport*>::iterator it = obj->fileSystemMetricsReports().begin();
         it != obj->fileSystemMetricsReports().end();
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
castor::IObject* castor::io::StreamDiskServerMetricsReportCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                               castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::monitoring::DiskServerMetricsReport* obj = 
    dynamic_cast<castor::monitoring::DiskServerMetricsReport*>(object);
  unsigned int fileSystemMetricsReportsNb;
  ad.stream() >> fileSystemMetricsReportsNb;
  for (unsigned int i = 0; i < fileSystemMetricsReportsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objFileSystemMetricsReports = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addFileSystemMetricsReports(dynamic_cast<castor::monitoring::FileSystemMetricsReport*>(objFileSystemMetricsReports));
  }
  return object;
}


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamDiskServerStateReportCnv.cpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamDiskServerStateReportCnv.hpp"
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
#include "castor/monitoring/AdminStatusCodes.hpp"
#include "castor/monitoring/DiskServerStateReport.hpp"
#include "castor/monitoring/FileSystemStateReport.hpp"
#include "castor/stager/DiskServerStatusCode.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamDiskServerStateReportCnv>* s_factoryStreamDiskServerStateReportCnv =
  new castor::CnvFactory<castor::io::StreamDiskServerStateReportCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamDiskServerStateReportCnv::StreamDiskServerStateReportCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamDiskServerStateReportCnv::~StreamDiskServerStateReportCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamDiskServerStateReportCnv::ObjType() {
  return castor::monitoring::DiskServerStateReport::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamDiskServerStateReportCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamDiskServerStateReportCnv::createRep(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           bool,
                                                           unsigned int)
  throw (castor::exception::Exception) {
  castor::monitoring::DiskServerStateReport* obj = 
    dynamic_cast<castor::monitoring::DiskServerStateReport*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->name();
  ad->stream() << obj->ram();
  ad->stream() << obj->memory();
  ad->stream() << obj->swap();
  ad->stream() << obj->id();
  ad->stream() << obj->status();
  ad->stream() << obj->adminStatus();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamDiskServerStateReportCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::monitoring::DiskServerStateReport* object = new castor::monitoring::DiskServerStateReport();
  // Now retrieve and set members
  std::string name;
  ad->stream() >> name;
  object->setName(name);
  u_signed64 ram;
  ad->stream() >> ram;
  object->setRam(ram);
  u_signed64 memory;
  ad->stream() >> memory;
  object->setMemory(memory);
  u_signed64 swap;
  ad->stream() >> swap;
  object->setSwap(swap);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int status;
  ad->stream() >> status;
  object->setStatus((castor::stager::DiskServerStatusCode)status);
  int adminStatus;
  ad->stream() >> adminStatus;
  object->setAdminStatus((castor::monitoring::AdminStatusCodes)adminStatus);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamDiskServerStateReportCnv::marshalObject(castor::IObject* object,
                                                               castor::io::StreamAddress* address,
                                                               castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::monitoring::DiskServerStateReport* obj = 
    dynamic_cast<castor::monitoring::DiskServerStateReport*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->FileSystemStatesReports().size();
    for (std::vector<castor::monitoring::FileSystemStateReport*>::iterator it = obj->FileSystemStatesReports().begin();
         it != obj->FileSystemStatesReports().end();
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
castor::IObject* castor::io::StreamDiskServerStateReportCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                             castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::monitoring::DiskServerStateReport* obj = 
    dynamic_cast<castor::monitoring::DiskServerStateReport*>(object);
  unsigned int FileSystemStatesReportsNb;
  ad.stream() >> FileSystemStatesReportsNb;
  for (unsigned int i = 0; i < FileSystemStatesReportsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objFileSystemStatesReports = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addFileSystemStatesReports(dynamic_cast<castor::monitoring::FileSystemStateReport*>(objFileSystemStatesReports));
  }
  return object;
}


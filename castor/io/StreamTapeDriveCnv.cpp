/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamTapeDriveCnv.cpp
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
#include "StreamTapeDriveCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvFactory.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/stager/ClientIdentification.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/vdqm/ErrorHistory.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDrive2ExtDevGroup.hpp"
#include "castor/vdqm/TapeDriveDedication.hpp"
#include "castor/vdqm/TapeDriveStatusCodes.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamTapeDriveCnv> s_factoryStreamTapeDriveCnv;
const castor::ICnvFactory& StreamTapeDriveCnvFactory = 
  s_factoryStreamTapeDriveCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamTapeDriveCnv::StreamTapeDriveCnv(castor::ICnvSvc* cnvSvc) :
  StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamTapeDriveCnv::~StreamTapeDriveCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeDriveCnv::ObjType() {
  return castor::vdqm::TapeDrive::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapeDriveCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamTapeDriveCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit,
                                               unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDrive* obj = 
    dynamic_cast<castor::vdqm::TapeDrive*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->jobID();
  ad->stream() << obj->modificationTime();
  ad->stream() << obj->resettime();
  ad->stream() << obj->usecount();
  ad->stream() << obj->errcount();
  ad->stream() << obj->transferredMB();
  ad->stream() << obj->totalMB();
  ad->stream() << obj->driveName();
  ad->stream() << obj->tapeAccessMode();
  ad->stream() << obj->id();
  ad->stream() << obj->status();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeDriveCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::vdqm::TapeDrive* object = new castor::vdqm::TapeDrive();
  // Now retrieve and set members
  int jobID;
  ad->stream() >> jobID;
  object->setJobID(jobID);
  int modificationTime;
  ad->stream() >> modificationTime;
  object->setModificationTime(modificationTime);
  int resettime;
  ad->stream() >> resettime;
  object->setResettime(resettime);
  int usecount;
  ad->stream() >> usecount;
  object->setUsecount(usecount);
  int errcount;
  ad->stream() >> errcount;
  object->setErrcount(errcount);
  int transferredMB;
  ad->stream() >> transferredMB;
  object->setTransferredMB(transferredMB);
  u_signed64 totalMB;
  ad->stream() >> totalMB;
  object->setTotalMB(totalMB);
  std::string driveName;
  ad->stream() >> driveName;
  object->setDriveName(driveName);
  int tapeAccessMode;
  ad->stream() >> tapeAccessMode;
  object->setTapeAccessMode(tapeAccessMode);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  int status;
  ad->stream() >> status;
  object->setStatus((castor::vdqm::TapeDriveStatusCodes)status);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamTapeDriveCnv::marshalObject(castor::IObject* object,
                                                   castor::io::StreamAddress* address,
                                                   castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDrive* obj = 
    dynamic_cast<castor::vdqm::TapeDrive*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    cnvSvc()->marshalObject(obj->tape(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->runningTapeReq(), address, alreadyDone);
    address->stream() << obj->errorHistory().size();
    for (std::vector<castor::vdqm::ErrorHistory*>::iterator it = obj->errorHistory().begin();
         it != obj->errorHistory().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    address->stream() << obj->tapeDriveDedication().size();
    for (std::vector<castor::vdqm::TapeDriveDedication*>::iterator it = obj->tapeDriveDedication().begin();
         it != obj->tapeDriveDedication().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    address->stream() << obj->().size();
    for (std::vector<castor::vdqm::TapeDrive2ExtDevGroup*>::iterator it = obj->().begin();
         it != obj->().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    cnvSvc()->marshalObject(obj->tapeServer(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->client(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapeDriveCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                 castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::vdqm::TapeDrive* obj = 
    dynamic_cast<castor::vdqm::TapeDrive*>(object);
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objTape = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setTape(dynamic_cast<castor::stager::Tape*>(objTape));
  ad.setObjType(castor::OBJ_INVALID);
  IObject* objRunningTapeReq = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setRunningTapeReq(dynamic_cast<castor::vdqm::TapeRequest*>(objRunningTapeReq));
  unsigned int errorHistoryNb;
  ad.stream() >> errorHistoryNb;
  for (unsigned int i = 0; i < errorHistoryNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    IObject* objErrorHistory = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addErrorHistory(dynamic_cast<castor::vdqm::ErrorHistory*>(objErrorHistory));
  }
  unsigned int tapeDriveDedicationNb;
  ad.stream() >> tapeDriveDedicationNb;
  for (unsigned int i = 0; i < tapeDriveDedicationNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    IObject* objTapeDriveDedication = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addTapeDriveDedication(dynamic_cast<castor::vdqm::TapeDriveDedication*>(objTapeDriveDedication));
  }
  unsigned int Nb;
  ad.stream() >> Nb;
  for (unsigned int i = 0; i < Nb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    IObject* obj
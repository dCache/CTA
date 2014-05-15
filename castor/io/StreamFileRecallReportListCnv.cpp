/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamFileRecallReportListCnv.cpp
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
#include "StreamFileRecallReportListCnv.hpp"
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
#include "castor/tape/tapegateway/FileErrorReportStruct.hpp"
#include "castor/tape/tapegateway/FileRecallReportList.hpp"
#include "castor/tape/tapegateway/FileRecalledNotificationStruct.hpp"
#include "osdep.h"
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamFileRecallReportListCnv>
  s_factoryStreamFileRecallReportListCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamFileRecallReportListCnv::StreamFileRecallReportListCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamFileRecallReportListCnv::~StreamFileRecallReportListCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamFileRecallReportListCnv::ObjType() {
  return castor::tape::tapegateway::FileRecallReportList::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamFileRecallReportListCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamFileRecallReportListCnv::createRep(castor::IAddress* address,
                                                          castor::IObject* object,
                                                          bool,
                                                          unsigned int)
   {
  castor::tape::tapegateway::FileRecallReportList* obj = 
    dynamic_cast<castor::tape::tapegateway::FileRecallReportList*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->mountTransactionId();
  ad->stream() << obj->aggregatorTransactionId();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileRecallReportListCnv::createObj(castor::IAddress* address)
   {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::FileRecallReportList* object = new castor::tape::tapegateway::FileRecallReportList();
  // Now retrieve and set members
  u_signed64 mountTransactionId;
  ad->stream() >> mountTransactionId;
  object->setMountTransactionId(mountTransactionId);
  u_signed64 aggregatorTransactionId;
  ad->stream() >> aggregatorTransactionId;
  object->setAggregatorTransactionId(aggregatorTransactionId);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamFileRecallReportListCnv::marshalObject(castor::IObject* object,
                                                              castor::io::StreamAddress* address,
                                                              castor::ObjectSet& alreadyDone)
   {
  castor::tape::tapegateway::FileRecallReportList* obj = 
    dynamic_cast<castor::tape::tapegateway::FileRecallReportList*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->successfulRecalls().size();
    for (std::vector<castor::tape::tapegateway::FileRecalledNotificationStruct*>::iterator it = obj->successfulRecalls().begin();
         it != obj->successfulRecalls().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    address->stream() << obj->failedRecalls().size();
    for (std::vector<castor::tape::tapegateway::FileErrorReportStruct*>::iterator it = obj->failedRecalls().begin();
         it != obj->failedRecalls().end();
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
castor::IObject* castor::io::StreamFileRecallReportListCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                            castor::ObjectCatalog& newlyCreated)
   {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::tape::tapegateway::FileRecallReportList* obj = 
    dynamic_cast<castor::tape::tapegateway::FileRecallReportList*>(object);
  unsigned int successfulRecallsNb;
  ad.stream() >> successfulRecallsNb;
  for (unsigned int i = 0; i < successfulRecallsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objSuccessfulRecalls = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addSuccessfulRecalls(dynamic_cast<castor::tape::tapegateway::FileRecalledNotificationStruct*>(objSuccessfulRecalls));
  }
  unsigned int failedRecallsNb;
  ad.stream() >> failedRecallsNb;
  for (unsigned int i = 0; i < failedRecallsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objFailedRecalls = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addFailedRecalls(dynamic_cast<castor::tape::tapegateway::FileErrorReportStruct*>(objFailedRecalls));
  }
  return object;
}


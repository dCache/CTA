/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamFileErrorReportCnv.cpp
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
#include "StreamFileErrorReportCnv.hpp"
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
#include "castor/tape/tapegateway/FileErrorReport.hpp"
#include "castor/tape/tapegateway/PositionCommandCode.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamFileErrorReportCnv>* s_factoryStreamFileErrorReportCnv =
  new castor::CnvFactory<castor::io::StreamFileErrorReportCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamFileErrorReportCnv::StreamFileErrorReportCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamFileErrorReportCnv::~StreamFileErrorReportCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamFileErrorReportCnv::ObjType() {
  return castor::tape::tapegateway::FileErrorReport::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamFileErrorReportCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamFileErrorReportCnv::createRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     bool endTransaction,
                                                     unsigned int type)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::FileErrorReport* obj = 
    dynamic_cast<castor::tape::tapegateway::FileErrorReport*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->fileTransactionId();
  ad->stream() << obj->nshost();
  ad->stream() << obj->fileid();
  ad->stream() << obj->fseq();
  ad->stream() << obj->mountTransactionId();
  ad->stream() << obj->errorCode();
  ad->stream() << obj->errorMessage();
  ad->stream() << obj->positionCommandCode();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamFileErrorReportCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::tape::tapegateway::FileErrorReport* object = new castor::tape::tapegateway::FileErrorReport();
  // Now retrieve and set members
  u_signed64 fileTransactionId;
  ad->stream() >> fileTransactionId;
  object->setFileTransactionId(fileTransactionId);
  std::string nshost;
  ad->stream() >> nshost;
  object->setNshost(nshost);
  u_signed64 fileid;
  ad->stream() >> fileid;
  object->setFileid(fileid);
  int fseq;
  ad->stream() >> fseq;
  object->setFseq(fseq);
  u_signed64 mountTransactionId;
  ad->stream() >> mountTransactionId;
  object->setMountTransactionId(mountTransactionId);
  int errorCode;
  ad->stream() >> errorCode;
  object->setErrorCode(errorCode);
  std::string errorMessage;
  ad->stream() >> errorMessage;
  object->setErrorMessage(errorMessage);
  int positionCommandCode;
  ad->stream() >> positionCommandCode;
  object->setPositionCommandCode((castor::tape::tapegateway::PositionCommandCode)positionCommandCode);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamFileErrorReportCnv::marshalObject(castor::IObject* object,
                                                         castor::io::StreamAddress* address,
                                                         castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::FileErrorReport* obj = 
    dynamic_cast<castor::tape::tapegateway::FileErrorReport*>(object);
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
castor::IObject* castor::io::StreamFileErrorReportCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                       castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


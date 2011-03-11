/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/io/StreamJobSubmissionRequestCnv.cpp
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
#include "StreamJobSubmissionRequestCnv.hpp"
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
#include "castor/jobmanager/JobSubmissionRequest.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamJobSubmissionRequestCnv>* s_factoryStreamJobSubmissionRequestCnv =
  new castor::CnvFactory<castor::io::StreamJobSubmissionRequestCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamJobSubmissionRequestCnv::StreamJobSubmissionRequestCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamJobSubmissionRequestCnv::~StreamJobSubmissionRequestCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamJobSubmissionRequestCnv::ObjType() {
  return castor::jobmanager::JobSubmissionRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::io::StreamJobSubmissionRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamJobSubmissionRequestCnv::createRep(castor::IAddress* address,
                                                          castor::IObject* object,
                                                          bool,
                                                          unsigned int)
  throw (castor::exception::Exception) {
  castor::jobmanager::JobSubmissionRequest* obj = 
    dynamic_cast<castor::jobmanager::JobSubmissionRequest*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->subReqId();
  ad->stream() << obj->protocol();
  ad->stream() << obj->xsize();
  ad->stream() << obj->requestedFileSystems();
  ad->stream() << obj->reqId();
  ad->stream() << obj->clientVersion();
  ad->stream() << obj->fileId();
  ad->stream() << obj->nsHost();
  ad->stream() << obj->svcClass();
  ad->stream() << obj->ipAddress();
  ad->stream() << obj->port();
  ad->stream() << obj->requestType();
  ad->stream() << obj->username();
  ad->stream() << obj->euid();
  ad->stream() << obj->egid();
  ad->stream() << obj->selectTime();
  ad->stream() << obj->submitStartTime();
  ad->stream() << obj->clientType();
  ad->stream() << obj->openFlags();
  ad->stream() << obj->sourceDiskCopyId();
  ad->stream() << obj->askedHosts();
  ad->stream() << obj->numAskedHosts();
  ad->stream() << obj->destDiskCopyId();
  ad->stream() << obj->clientSecure();
  ad->stream() << obj->sourceSvcClass();
  ad->stream() << obj->requestCreationTime();
  ad->stream() << obj->defaultFileSize();
  ad->stream() << obj->excludedHosts();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamJobSubmissionRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::jobmanager::JobSubmissionRequest* object = new castor::jobmanager::JobSubmissionRequest();
  // Now retrieve and set members
  std::string subReqId;
  ad->stream() >> subReqId;
  object->setSubReqId(subReqId);
  std::string protocol;
  ad->stream() >> protocol;
  object->setProtocol(protocol);
  u_signed64 xsize;
  ad->stream() >> xsize;
  object->setXsize(xsize);
  std::string requestedFileSystems;
  ad->stream() >> requestedFileSystems;
  object->setRequestedFileSystems(requestedFileSystems);
  std::string reqId;
  ad->stream() >> reqId;
  object->setReqId(reqId);
  u_signed64 clientVersion;
  ad->stream() >> clientVersion;
  object->setClientVersion(clientVersion);
  u_signed64 fileId;
  ad->stream() >> fileId;
  object->setFileId(fileId);
  std::string nsHost;
  ad->stream() >> nsHost;
  object->setNsHost(nsHost);
  std::string svcClass;
  ad->stream() >> svcClass;
  object->setSvcClass(svcClass);
  unsigned long ipAddress;
  ad->stream() >> ipAddress;
  object->setIpAddress(ipAddress);
  unsigned short port;
  ad->stream() >> port;
  object->setPort(port);
  int requestType;
  ad->stream() >> requestType;
  object->setRequestType(requestType);
  std::string username;
  ad->stream() >> username;
  object->setUsername(username);
  unsigned long euid;
  ad->stream() >> euid;
  object->setEuid(euid);
  unsigned long egid;
  ad->stream() >> egid;
  object->setEgid(egid);
  u_signed64 selectTime;
  ad->stream() >> selectTime;
  object->setSelectTime(selectTime);
  u_signed64 submitStartTime;
  ad->stream() >> submitStartTime;
  object->setSubmitStartTime(submitStartTime);
  u_signed64 clientType;
  ad->stream() >> clientType;
  object->setClientType(clientType);
  std::string openFlags;
  ad->stream() >> openFlags;
  object->setOpenFlags(openFlags);
  u_signed64 sourceDiskCopyId;
  ad->stream() >> sourceDiskCopyId;
  object->setSourceDiskCopyId(sourceDiskCopyId);
  std::string askedHosts;
  ad->stream() >> askedHosts;
  object->setAskedHosts(askedHosts);
  u_signed64 numAskedHosts;
  ad->stream() >> numAskedHosts;
  object->setNumAskedHosts(numAskedHosts);
  u_signed64 destDiskCopyId;
  ad->stream() >> destDiskCopyId;
  object->setDestDiskCopyId(destDiskCopyId);
  int clientSecure;
  ad->stream() >> clientSecure;
  object->setClientSecure(clientSecure);
  std::string sourceSvcClass;
  ad->stream() >> sourceSvcClass;
  object->setSourceSvcClass(sourceSvcClass);
  u_signed64 requestCreationTime;
  ad->stream() >> requestCreationTime;
  object->setRequestCreationTime(requestCreationTime);
  u_signed64 defaultFileSize;
  ad->stream() >> defaultFileSize;
  object->setDefaultFileSize(defaultFileSize);
  std::string excludedHosts;
  ad->stream() >> excludedHosts;
  object->setExcludedHosts(excludedHosts);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamJobSubmissionRequestCnv::marshalObject(castor::IObject* object,
                                                              castor::io::StreamAddress* address,
                                                              castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::jobmanager::JobSubmissionRequest* obj = 
    dynamic_cast<castor::jobmanager::JobSubmissionRequest*>(object);
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
castor::IObject* castor::io::StreamJobSubmissionRequestCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                            castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  return object;
}


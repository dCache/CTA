/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "StreamStagePutRequestCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/io/StreamBaseCnv.hpp"
#include "castor/io/StreamCnvSvc.hpp"
#include "castor/stager/StagePutRequest.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamStagePutRequestCnv>* s_factoryStreamStagePutRequestCnv =
  new castor::CnvFactory<castor::io::StreamStagePutRequestCnv>();

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamStagePutRequestCnv::StreamStagePutRequestCnv(castor::ICnvSvc* cnvSvc) :
 StreamBaseCnv(cnvSvc) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamStagePutRequestCnv::~StreamStagePutRequestCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamStagePutRequestCnv::ObjType() {
  return castor::stager::StagePutRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamStagePutRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamStagePutRequestCnv::createRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     bool autocommit,
                                                     unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::StagePutRequest* obj = 
    dynamic_cast<castor::stager::StagePutRequest*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->flags();
  ad->stream() << obj->userName();
  ad->stream() << obj->euid();
  ad->stream() << obj->egid();
  ad->stream() << obj->mask();
  ad->stream() << obj->pid();
  ad->stream() << obj->machine();
  ad->stream() << obj->svcClassName();
  ad->stream() << obj->userTag();
  ad->stream() << obj->reqId();
  ad->stream() << obj->creationTime();
  ad->stream() << obj->lastModificationTime();
  ad->stream() << obj->id();
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamStagePutRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::StagePutRequest* object = new castor::stager::StagePutRequest();
  // Now retrieve and set members
  u_signed64 flags;
  ad->stream() >> flags;
  object->setFlags(flags);
  std::string userName;
  ad->stream() >> userName;
  object->setUserName(userName);
  unsigned long euid;
  ad->stream() >> euid;
  object->setEuid(euid);
  unsigned long egid;
  ad->stream() >> egid;
  object->setEgid(egid);
  unsigned long mask;
  ad->stream() >> mask;
  object->setMask(mask);
  unsigned long pid;
  ad->stream() >> pid;
  object->setPid(pid);
  std::string machine;
  ad->stream() >> machine;
  object->setMachine(machine);
  std::string svcClassName;
  ad->stream() >> svcClassName;
  object->setSvcClassName(svcClassName);
  std::string userTag;
  ad->stream() >> userTag;
  object->setUserTag(userTag);
  std::string reqId;
  ad->stream() >> reqId;
  object->setReqId(reqId);
  u_signed64 creationTime;
  ad->stream() >> creationTime;
  object->setCreationTime(creationTime);
  u_signed64 lastModificationTime;
  ad->stream() >> lastModificationTime;
  object->setLastModificationTime(lastModificationTime);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  return object;
}

//------------------------------------------------------------------------------
// marshalObject
//------------------------------------------------------------------------------
void castor::io::StreamStagePutRequestCnv::marshalObject(castor::IObject* object,
                                                         castor::io::StreamAddress* address,
                                                         castor::ObjectSet& alreadyDone)
  throw (castor::exception::Exception) {
  castor::stager::StagePutRequest* obj = 
    dynamic_cast<castor::stager::StagePutRequest*>(object);
  if (0 == obj) {
    // Case of a null pointer
    address->stream() << castor::OBJ_Ptr << ((unsigned int)0);
  } else if (alreadyDone.find(obj) == alreadyDone.end()) {
    // Case of a pointer to a non streamed object
    createRep(address, obj, true);
    // Mark object as done
    alreadyDone.insert(obj);
    address->stream() << obj->subRequests().size();
    for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
         it != obj->subRequests().end();
         it++) {
      cnvSvc()->marshalObject(*it, address, alreadyDone);
    }
    cnvSvc()->marshalObject(obj->svcClass(), address, alreadyDone);
    cnvSvc()->marshalObject(obj->client(), address, alreadyDone);
  } else {
    // case of a pointer to an already streamed object
    address->stream() << castor::OBJ_Ptr << alreadyDone[obj];
  }
}

//------------------------------------------------------------------------------
// unmarshalObject
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamStagePutRequestCnv::unmarshalObject(castor::io::biniostream& stream,
                                                                       castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  castor::io::StreamAddress ad(stream, "StreamCnvSvc", castor::SVC_STREAMCNV);
  castor::IObject* object = createObj(&ad);
  // Mark object as created
  newlyCreated.insert(object);
  // Fill object with associations
  castor::stager::StagePutRequest* obj = 
    dynamic_cast<castor::stager::StagePutRequest*>(object);
  unsigned int subRequestsNb;
  ad.stream() >> subRequestsNb;
  for (unsigned int i = 0; i < subRequestsNb; i++) {
    ad.setObjType(castor::OBJ_INVALID);
    castor::IObject* objSubRequests = cnvSvc()->unmarshalObject(ad, newlyCreated);
    obj->addSubRequests(dynamic_cast<castor::stager::SubRequest*>(objSubRequests));
  }
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objSvcClass = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setSvcClass(dynamic_cast<castor::stager::SvcClass*>(objSvcClass));
  ad.setObjType(castor::OBJ_INVALID);
  castor::IObject* objClient = cnvSvc()->unmarshalObject(ad, newlyCreated);
  obj->setClient(dynamic_cast<castor::IClient*>(objClient));
  return object;
}


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "DbStageFileQueryRequestCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/QueryParameter.hpp"
#include "castor/stager/StageFileQueryRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbStageFileQueryRequestCnv>* s_factoryDbStageFileQueryRequestCnv =
  new castor::CnvFactory<castor::db::cnv::DbStageFileQueryRequestCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_insertStatementString =
"INSERT INTO StageFileQueryRequest (flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, fileName, id, svcClass, client) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,ids_seq.nextval,:14,:15) RETURNING id INTO :16";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_deleteStatementString =
"DELETE FROM StageFileQueryRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_selectStatementString =
"SELECT flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, fileName, id, svcClass, client FROM StageFileQueryRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_updateStatementString =
"UPDATE StageFileQueryRequest SET flags = :1, userName = :2, euid = :3, egid = :4, mask = :5, pid = :6, machine = :7, svcClassName = :8, userTag = :9, reqId = :10, lastModificationTime = :11, fileName = :12 WHERE id = :13";

/// SQL statement for type storage
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL statement for request insertion into newRequests table
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_insertNewReqStatementString =
"INSERT INTO newRequests (id, type, creation) VALUES (:1, :2, SYSDATE)";

/// SQL select statement for member parameters
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_selectQueryParameterStatementString =
"SELECT id FROM QueryParameter WHERE query = :1 FOR UPDATE";

/// SQL delete statement for member parameters
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_deleteQueryParameterStatementString =
"UPDATE QueryParameter SET query = 0 WHERE id = :1";

/// SQL remote update statement for member parameters
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_remoteUpdateQueryParameterStatementString =
"UPDATE QueryParameter SET query = :1 WHERE id = :2";

/// SQL existence statement for member svcClass
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_checkSvcClassExistStatementString =
"SELECT id FROM SvcClass WHERE id = :1";

/// SQL update statement for member svcClass
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_updateSvcClassStatementString =
"UPDATE StageFileQueryRequest SET svcClass = :1 WHERE id = :2";

/// SQL update statement for member client
const std::string castor::db::cnv::DbStageFileQueryRequestCnv::s_updateIClientStatementString =
"UPDATE StageFileQueryRequest SET client = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbStageFileQueryRequestCnv::DbStageFileQueryRequestCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_insertNewReqStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectQueryParameterStatement(0),
  m_deleteQueryParameterStatement(0),
  m_remoteUpdateQueryParameterStatement(0),
  m_checkSvcClassExistStatement(0),
  m_updateSvcClassStatement(0),
  m_updateIClientStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbStageFileQueryRequestCnv::~DbStageFileQueryRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    delete m_insertStatement;
    delete m_deleteStatement;
    delete m_selectStatement;
    delete m_updateStatement;
    delete m_insertNewReqStatement;
    delete m_storeTypeStatement;
    delete m_deleteTypeStatement;
    delete m_deleteQueryParameterStatement;
    delete m_selectQueryParameterStatement;
    delete m_remoteUpdateQueryParameterStatement;
    delete m_checkSvcClassExistStatement;
    delete m_updateSvcClassStatement;
    delete m_updateIClientStatement;
  } catch (castor::exception::SQLError ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_insertNewReqStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectQueryParameterStatement = 0;
  m_deleteQueryParameterStatement = 0;
  m_remoteUpdateQueryParameterStatement = 0;
  m_checkSvcClassExistStatement = 0;
  m_updateSvcClassStatement = 0;
  m_updateIClientStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbStageFileQueryRequestCnv::ObjType() {
  return castor::stager::StageFileQueryRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbStageFileQueryRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::fillRep(castor::IAddress* address,
                                                          castor::IObject* object,
                                                          unsigned int type,
                                                          bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageFileQueryRequest* obj = 
    dynamic_cast<castor::stager::StageFileQueryRequest*>(object);
  try {
    switch (type) {
    case castor::OBJ_QueryParameter :
      fillRepQueryParameter(obj);
      break;
    case castor::OBJ_SvcClass :
      fillRepSvcClass(obj);
      break;
    case castor::OBJ_IClient :
      fillRepIClient(obj);
      break;
    default :
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "fillRep called for type " << type 
                      << " on object of type " << obj->type() 
                      << ". This is meaningless.";
      throw ex;
    }
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.getMessage().str() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepQueryParameter
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::fillRepQueryParameter(castor::stager::StageFileQueryRequest* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectQueryParameterStatement) {
    m_selectQueryParameterStatement = createStatement(s_selectQueryParameterStatementString);
  }
  // Get current database data
  std::set<int> parametersList;
  m_selectQueryParameterStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectQueryParameterStatement->executeQuery();
  while (rset->next()) {
    parametersList.insert(rset->getInt(1));
  }
  delete rset;
  // update parameters and create new ones
  for (std::vector<castor::stager::QueryParameter*>::iterator it = obj->parameters().begin();
       it != obj->parameters().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_QryRequest);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateQueryParameterStatement) {
        m_remoteUpdateQueryParameterStatement = createStatement(s_remoteUpdateQueryParameterStatementString);
      }
      // Update remote object
      m_remoteUpdateQueryParameterStatement->setInt64(1, obj->id());
      m_remoteUpdateQueryParameterStatement->setInt64(2, (*it)->id());
      m_remoteUpdateQueryParameterStatement->execute();
      std::set<int>::iterator item;
      if ((item = parametersList.find((*it)->id())) != parametersList.end()) {
        parametersList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = parametersList.begin();
       it != parametersList.end();
       it++) {
    if (0 == m_deleteQueryParameterStatement) {
      m_deleteQueryParameterStatement = createStatement(s_deleteQueryParameterStatementString);
    }
    m_deleteQueryParameterStatement->setInt64(1, *it);
    m_deleteQueryParameterStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepSvcClass
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::fillRepSvcClass(castor::stager::StageFileQueryRequest* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->svcClass()) {
    // Check checkSvcClassExist statement
    if (0 == m_checkSvcClassExistStatement) {
      m_checkSvcClassExistStatement = createStatement(s_checkSvcClassExistStatementString);
    }
    // retrieve the object from the database
    m_checkSvcClassExistStatement->setInt64(1, obj->svcClass()->id());
    castor::db::IDbResultSet *rset = m_checkSvcClassExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->svcClass(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateSvcClassStatement) {
    m_updateSvcClassStatement = createStatement(s_updateSvcClassStatementString);
  }
  // Update local object
  m_updateSvcClassStatement->setInt64(1, 0 == obj->svcClass() ? 0 : obj->svcClass()->id());
  m_updateSvcClassStatement->setInt64(2, obj->id());
  m_updateSvcClassStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepIClient
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::fillRepIClient(castor::stager::StageFileQueryRequest* obj)
  throw (castor::exception::Exception) {
  // Check update statement
  if (0 == m_updateIClientStatement) {
    m_updateIClientStatement = createStatement(s_updateIClientStatementString);
  }
  // Update local object
  m_updateIClientStatement->setInt64(1, 0 == obj->client() ? 0 : obj->client()->id());
  m_updateIClientStatement->setInt64(2, obj->id());
  m_updateIClientStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::fillObj(castor::IAddress* address,
                                                          castor::IObject* object,
                                                          unsigned int type,
                                                          bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageFileQueryRequest* obj = 
    dynamic_cast<castor::stager::StageFileQueryRequest*>(object);
  switch (type) {
  case castor::OBJ_QueryParameter :
    fillObjQueryParameter(obj);
    break;
  case castor::OBJ_SvcClass :
    fillObjSvcClass(obj);
    break;
  case castor::OBJ_IClient :
    fillObjIClient(obj);
    break;
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillObj called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
  if (autocommit) {
    cnvSvc()->commit();
  }
}
//------------------------------------------------------------------------------
// fillObjQueryParameter
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::fillObjQueryParameter(castor::stager::StageFileQueryRequest* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectQueryParameterStatement) {
    m_selectQueryParameterStatement = createStatement(s_selectQueryParameterStatementString);
  }
  // retrieve the object from the database
  std::set<int> parametersList;
  m_selectQueryParameterStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectQueryParameterStatement->executeQuery();
  while (rset->next()) {
    parametersList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::QueryParameter*> toBeDeleted;
  for (std::vector<castor::stager::QueryParameter*>::iterator it = obj->parameters().begin();
       it != obj->parameters().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = parametersList.find((*it)->id())) == parametersList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      parametersList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::QueryParameter*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeParameters(*it);
    (*it)->setQuery(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = parametersList.begin();
       it != parametersList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::QueryParameter* remoteObj = 
      dynamic_cast<castor::stager::QueryParameter*>(item);
    obj->addParameters(remoteObj);
    remoteObj->setQuery(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjSvcClass
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::fillObjSvcClass(castor::stager::StageFileQueryRequest* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
  if (!rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 svcClassId = rset->getInt64(15);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->svcClass() &&
      (0 == svcClassId ||
       obj->svcClass()->id() != svcClassId)) {
    obj->setSvcClass(0);
  }
  // Update object or create new one
  if (0 != svcClassId) {
    if (0 == obj->svcClass()) {
      obj->setSvcClass
        (dynamic_cast<castor::stager::SvcClass*>
         (cnvSvc()->getObjFromId(svcClassId)));
    } else {
      cnvSvc()->updateObj(obj->svcClass());
    }
  }
}

//------------------------------------------------------------------------------
// fillObjIClient
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::fillObjIClient(castor::stager::StageFileQueryRequest* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
  if (!rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 clientId = rset->getInt64(16);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->client() &&
      (0 == clientId ||
       obj->client()->id() != clientId)) {
    obj->setClient(0);
  }
  // Update object or create new one
  if (0 != clientId) {
    if (0 == obj->client()) {
      obj->setClient
        (dynamic_cast<castor::IClient*>
         (cnvSvc()->getObjFromId(clientId)));
    } else {
      cnvSvc()->updateObj(obj->client());
    }
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::createRep(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            bool autocommit,
                                                            unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::StageFileQueryRequest* obj = 
    dynamic_cast<castor::stager::StageFileQueryRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(16, castor::db::DBTYPE_INT64);
    }
    if (0 == m_insertNewReqStatement) {
      m_insertNewReqStatement = createStatement(s_insertNewReqStatementString);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt64(1, obj->flags());
    m_insertStatement->setString(2, obj->userName());
    m_insertStatement->setInt(3, obj->euid());
    m_insertStatement->setInt(4, obj->egid());
    m_insertStatement->setInt(5, obj->mask());
    m_insertStatement->setInt(6, obj->pid());
    m_insertStatement->setString(7, obj->machine());
    m_insertStatement->setString(8, obj->svcClassName());
    m_insertStatement->setString(9, obj->userTag());
    m_insertStatement->setString(10, obj->reqId());
    m_insertStatement->setInt(11, time(0));
    m_insertStatement->setInt(12, time(0));
    m_insertStatement->setString(13, obj->fileName());
    m_insertStatement->setInt64(14, (type == OBJ_SvcClass && obj->svcClass() != 0) ? obj->svcClass()->id() : 0);
    m_insertStatement->setInt64(15, (type == OBJ_IClient && obj->client() != 0) ? obj->client()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getInt64(16));
    m_storeTypeStatement->setInt64(1, obj->id());
    m_storeTypeStatement->setInt64(2, obj->type());
    m_storeTypeStatement->execute();
    m_insertNewReqStatement->setInt64(1, obj->id());
    m_insertNewReqStatement->setInt64(2, obj->type());
    m_insertNewReqStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  flags : " << obj->flags() << std::endl
                    << "  userName : " << obj->userName() << std::endl
                    << "  euid : " << obj->euid() << std::endl
                    << "  egid : " << obj->egid() << std::endl
                    << "  mask : " << obj->mask() << std::endl
                    << "  pid : " << obj->pid() << std::endl
                    << "  machine : " << obj->machine() << std::endl
                    << "  svcClassName : " << obj->svcClassName() << std::endl
                    << "  userTag : " << obj->userTag() << std::endl
                    << "  reqId : " << obj->reqId() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  lastModificationTime : " << obj->lastModificationTime() << std::endl
                    << "  fileName : " << obj->fileName() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  svcClass : " << obj->svcClass() << std::endl
                    << "  client : " << obj->client() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::updateRep(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageFileQueryRequest* obj = 
    dynamic_cast<castor::stager::StageFileQueryRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt64(1, obj->flags());
    m_updateStatement->setString(2, obj->userName());
    m_updateStatement->setInt(3, obj->euid());
    m_updateStatement->setInt(4, obj->egid());
    m_updateStatement->setInt(5, obj->mask());
    m_updateStatement->setInt(6, obj->pid());
    m_updateStatement->setString(7, obj->machine());
    m_updateStatement->setString(8, obj->svcClassName());
    m_updateStatement->setString(9, obj->userTag());
    m_updateStatement->setString(10, obj->reqId());
    m_updateStatement->setInt(11, time(0));
    m_updateStatement->setString(12, obj->fileName());
    m_updateStatement->setInt64(13, obj->id());
    m_updateStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::deleteRep(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageFileQueryRequest* obj = 
    dynamic_cast<castor::stager::StageFileQueryRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_deleteStatement) {
      m_deleteStatement = createStatement(s_deleteStatementString);
    }
    if (0 == m_deleteTypeStatement) {
      m_deleteTypeStatement = createStatement(s_deleteTypeStatementString);
    }
    // Now Delete the object
    m_deleteTypeStatement->setInt64(1, obj->id());
    m_deleteTypeStatement->execute();
    m_deleteStatement->setInt64(1, obj->id());
    m_deleteStatement->execute();
    for (std::vector<castor::stager::QueryParameter*>::iterator it = obj->parameters().begin();
         it != obj->parameters().end();
         it++) {
      cnvSvc()->deleteRep(0, *it, false);
    }
    if (obj->client() != 0) {
      cnvSvc()->deleteRep(0, obj->client(), false);
    }
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in delete request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_deleteStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::db::cnv::DbStageFileQueryRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::BaseAddress* ad = 
    dynamic_cast<castor::BaseAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setInt64(1, ad->target());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << ad->target();
      throw ex;
    }
    // create the new Object
    castor::stager::StageFileQueryRequest* object = new castor::stager::StageFileQueryRequest();
    // Now retrieve and set members
    object->setFlags(rset->getInt64(1));
    object->setUserName(rset->getString(2));
    object->setEuid(rset->getInt(3));
    object->setEgid(rset->getInt(4));
    object->setMask(rset->getInt(5));
    object->setPid(rset->getInt(6));
    object->setMachine(rset->getString(7));
    object->setSvcClassName(rset->getString(8));
    object->setUserTag(rset->getString(9));
    object->setReqId(rset->getString(10));
    object->setCreationTime(rset->getInt64(11));
    object->setLastModificationTime(rset->getInt64(12));
    object->setFileName(rset->getString(13));
    object->setId(rset->getInt64(14));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in select request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_selectStatementString << std::endl
                    << "and id was " << ad->target() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbStageFileQueryRequestCnv::updateObj(castor::IObject* obj)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setInt64(1, obj->id());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << obj->id();
      throw ex;
    }
    // Now retrieve and set members
    castor::stager::StageFileQueryRequest* object = 
      dynamic_cast<castor::stager::StageFileQueryRequest*>(obj);
    object->setFlags(rset->getInt64(1));
    object->setUserName(rset->getString(2));
    object->setEuid(rset->getInt(3));
    object->setEgid(rset->getInt(4));
    object->setMask(rset->getInt(5));
    object->setPid(rset->getInt(6));
    object->setMachine(rset->getString(7));
    object->setSvcClassName(rset->getString(8));
    object->setUserTag(rset->getString(9));
    object->setReqId(rset->getString(10));
    object->setCreationTime(rset->getInt64(11));
    object->setLastModificationTime(rset->getInt64(12));
    object->setFileName(rset->getString(13));
    object->setId(rset->getInt64(14));
    delete rset;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


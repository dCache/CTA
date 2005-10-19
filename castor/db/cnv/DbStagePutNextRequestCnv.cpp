/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbStagePutNextRequestCnv.cpp
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
#include "DbStagePutNextRequestCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/ICnvFactory.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/FileRequest.hpp"
#include "castor/stager/StagePutNextRequest.hpp"
#include "castor/stager/SvcClass.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbStagePutNextRequestCnv> s_factoryDbStagePutNextRequestCnv;
const castor::ICnvFactory& DbStagePutNextRequestCnvFactory = 
  s_factoryDbStagePutNextRequestCnv;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_insertStatementString =
"INSERT INTO StagePutNextRequest (parentUuid, flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, id, parent, svcClass, client) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,ids_seq.nextval,:14,:15,:16) RETURNING id INTO :17";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_deleteStatementString =
"DELETE FROM StagePutNextRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_selectStatementString =
"SELECT parentUuid, flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, id, parent, svcClass, client FROM StagePutNextRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_updateStatementString =
"UPDATE StagePutNextRequest SET parentUuid = :1, flags = :2, userName = :3, euid = :4, egid = :5, mask = :6, pid = :7, machine = :8, svcClassName = :9, userTag = :10, reqId = :11, lastModificationTime = :12 WHERE id = :13";

/// SQL statement for type storage
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL statement for request insertion into newRequests table
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_insertNewReqStatementString =
"INSERT INTO newRequests (id, type, creation) VALUES (:1, :2, SYSDATE)";

/// SQL update statement for member parent
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_updateFileRequestStatementString =
"UPDATE StagePutNextRequest SET parent = :1 WHERE id = :2";

/// SQL existence statement for member svcClass
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_checkSvcClassExistStatementString =
"SELECT id FROM SvcClass WHERE id = :1";

/// SQL update statement for member svcClass
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_updateSvcClassStatementString =
"UPDATE StagePutNextRequest SET svcClass = :1 WHERE id = :2";

/// SQL update statement for member client
const std::string castor::db::cnv::DbStagePutNextRequestCnv::s_updateIClientStatementString =
"UPDATE StagePutNextRequest SET client = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbStagePutNextRequestCnv::DbStagePutNextRequestCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_insertNewReqStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_updateFileRequestStatement(0),
  m_checkSvcClassExistStatement(0),
  m_updateSvcClassStatement(0),
  m_updateIClientStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbStagePutNextRequestCnv::~DbStagePutNextRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbStagePutNextRequestCnv::reset() throw() {
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
    delete m_updateFileRequestStatement;
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
  m_updateFileRequestStatement = 0;
  m_checkSvcClassExistStatement = 0;
  m_updateSvcClassStatement = 0;
  m_updateIClientStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbStagePutNextRequestCnv::ObjType() {
  return castor::stager::StagePutNextRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbStagePutNextRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStagePutNextRequestCnv::fillRep(castor::IAddress* address,
                                                        castor::IObject* object,
                                                        unsigned int type,
                                                        bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StagePutNextRequest* obj = 
    dynamic_cast<castor::stager::StagePutNextRequest*>(object);
  try {
    switch (type) {
    case castor::OBJ_FileRequest :
      fillRepFileRequest(obj);
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
// fillRepFileRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbStagePutNextRequestCnv::fillRepFileRequest(castor::stager::StagePutNextRequest* obj)
  throw (castor::exception::Exception) {
  // Check update statement
  if (0 == m_updateFileRequestStatement) {
    m_updateFileRequestStatement = createStatement(s_updateFileRequestStatementString);
  }
  // Update local object
  m_updateFileRequestStatement->setInt64(1, 0 == obj->parent() ? 0 : obj->parent()->id());
  m_updateFileRequestStatement->setInt64(2, obj->id());
  m_updateFileRequestStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepSvcClass
//------------------------------------------------------------------------------
void castor::db::cnv::DbStagePutNextRequestCnv::fillRepSvcClass(castor::stager::StagePutNextRequest* obj)
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
void castor::db::cnv::DbStagePutNextRequestCnv::fillRepIClient(castor::stager::StagePutNextRequest* obj)
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
void castor::db::cnv::DbStagePutNextRequestCnv::fillObj(castor::IAddress* address,
                                                        castor::IObject* object,
                                                        unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::StagePutNextRequest* obj = 
    dynamic_cast<castor::stager::StagePutNextRequest*>(object);
  switch (type) {
  case castor::OBJ_FileRequest :
    fillObjFileRequest(obj);
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
}

//------------------------------------------------------------------------------
// fillObjFileRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbStagePutNextRequestCnv::fillObjFileRequest(castor::stager::StagePutNextRequest* obj)
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
  u_signed64 parentId = rset->getInt64(15);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->parent() &&
      (0 == parentId ||
       obj->parent()->id() != parentId)) {
    obj->setParent(0);
  }
  // Update object or create new one
  if (0 != parentId) {
    if (0 == obj->parent()) {
      obj->setParent
        (dynamic_cast<castor::stager::FileRequest*>
         (cnvSvc()->getObjFromId(parentId)));
    } else {
      cnvSvc()->updateObj(obj->parent());
    }
  }
}

//------------------------------------------------------------------------------
// fillObjSvcClass
//------------------------------------------------------------------------------
void castor::db::cnv::DbStagePutNextRequestCnv::fillObjSvcClass(castor::stager::StagePutNextRequest* obj)
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
  u_signed64 svcClassId = rset->getInt64(16);
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
void castor::db::cnv::DbStagePutNextRequestCnv::fillObjIClient(castor::stager::StagePutNextRequest* obj)
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
  u_signed64 clientId = rset->getInt64(17);
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
void castor::db::cnv::DbStagePutNextRequestCnv::createRep(castor::IAddress* address,
                                                          castor::IObject* object,
                                                          bool autocommit,
                                                          unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::StagePutNextRequest* obj = 
    dynamic_cast<castor::stager::StagePutNextRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(17, castor::db::DBTYPE_INT64);
    }
    if (0 == m_insertNewReqStatement) {
      m_insertNewReqStatement = createStatement(s_insertNewReqStatementString);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->parentUuid());
    m_insertStatement->setInt64(2, obj->flags());
    m_insertStatement->setString(3, obj->userName());
    m_insertStatement->setInt(4, obj->euid());
    m_insertStatement->setInt(5, obj->egid());
    m_insertStatement->setInt(6, obj->mask());
    m_insertStatement->setInt(7, obj->pid());
    m_insertStatement->setString(8, obj->machine());
    m_insertStatement->setString(9, obj->svcClassName());
    m_insertStatement->setString(10, obj->userTag());
    m_insertStatement->setString(11, obj->reqId());
    m_insertStatement->setInt(12, time(0));
    m_insertStatement->setInt(13, time(0));
    m_insertStatement->setInt64(14, (type == OBJ_FileRequest && obj->parent() != 0) ? obj->parent()->id() : 0);
    m_insertStatement->setInt64(15, (type == OBJ_SvcClass && obj->svcClass() != 0) ? obj->svcClass()->id() : 0);
    m_insertStatement->setInt64(16, (type == OBJ_IClient && obj->client() != 0) ? obj->client()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getInt64(17));
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
                    << "  parentUuid : " << obj->parentUuid() << std::endl
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
                    << "  id : " << obj->id() << std::endl
                    << "  parent : " << obj->parent() << std::endl
                    << "  svcClass : " << obj->svcClass() << std::endl
                    << "  client : " << obj->client() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStagePutNextRequestCnv::updateRep(castor::IAddress* address,
                                                          castor::IObject* object,
                                                          bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StagePutNextRequest* obj = 
    dynamic_cast<castor::stager::StagePutNextRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->parentUuid());
    m_updateStatement->setInt64(2, obj->flags());
    m_updateStatement->setString(3, obj->userName());
    m_updateStatement->setInt(4, obj->euid());
    m_updateStatement->setInt(5, obj->egid());
    m_updateStatement->setInt(6, obj->mask());
    m_updateStatement->setInt(7, obj->pid());
    m_updateStatement->setString(8, obj->machine());
    m_updateStatement->setString(9, obj->svcClassName());
    m_updateStatement->setString(10, obj->userTag());
    m_updateStatement->setString(11, obj->reqId());
    m_updateStatement->setInt(12, time(0));
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
void castor::db::cnv::DbStagePutNextRequestCnv::deleteRep(castor::IAddress* address,
                                                          castor::IObject* object,
                                                          bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StagePutNextRequest* obj = 
    dynamic_cast<castor::stager::StagePutNextRequest*>(object);
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
castor::IObject* castor::db::cnv::DbStagePutNextRequestCnv::createObj(castor::IAddress* address)
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
    castor::stager::StagePutNextRequest* object = new castor::stager::StagePutNextRequest();
    // Now retrieve and set members
    object->setParentUuid(rset->getString(1));
    object->setFlags(rset->getInt64(2));
    object->setUserName(rset->getString(3));
    object->setEuid(rset->getInt(4));
    object->setEgid(rset->getInt(5));
    object->setMask(rset->getInt(6));
    object->setPid(rset->getInt(7));
    object->setMachine(rset->getString(8));
    object->setSvcClassName(rset->getString(9));
    object->setUserTag(rset->getString(10));
    object->setReqId(rset->getString(11));
    object->setCreationTime(rset->getInt64(12));
    object->setLastModificationTime(rset->getInt64(13));
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
void castor::db::cnv::DbStagePutNextRequestCnv::updateObj(castor::IObject* obj)
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
    castor::stager::StagePutNextRequest* object = 
      dynamic_cast<castor::stager::StagePutNextRequest*>(obj);
    object->setParentUuid(rset->getString(1));
    object->setFlags(rset->getInt64(2));
    object->setUserName(rset->getString(3));
    object->setEuid(rset->getInt(4));
    object->setEgid(rset->getInt(5));
    object->setMask(rset->getInt(6));
    object->setPid(rset->getInt(7));
    object->setMachine(rset->getString(8));
    object->setSvcClassName(rset->getString(9));
    object->setUserTag(rset->getString(10));
    object->setReqId(rset->getString(11));
    object->setCreationTime(rset->getInt64(12));
    object->setLastModificationTime(rset->getInt64(13));
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


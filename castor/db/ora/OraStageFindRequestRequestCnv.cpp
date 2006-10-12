/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/ora/OraStageFindRequestRequestCnv.cpp
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
#include "OraStageFindRequestRequestCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/QueryParameter.hpp"
#include "castor/stager/StageFindRequestRequest.hpp"
#include "castor/stager/SvcClass.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraStageFindRequestRequestCnv>* s_factoryOraStageFindRequestRequestCnv =
  new castor::CnvFactory<castor::db::ora::OraStageFindRequestRequestCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_insertStatementString =
"INSERT INTO StageFindRequestRequest (flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, id, svcClass, client) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,ids_seq.nextval,:13,:14) RETURNING id INTO :15";

/// SQL statement for request deletion
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_deleteStatementString =
"DELETE FROM StageFindRequestRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_selectStatementString =
"SELECT flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, id, svcClass, client FROM StageFindRequestRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_updateStatementString =
"UPDATE StageFindRequestRequest SET flags = :1, userName = :2, euid = :3, egid = :4, mask = :5, pid = :6, machine = :7, svcClassName = :8, userTag = :9, reqId = :10, lastModificationTime = :11 WHERE id = :12";

/// SQL statement for type storage
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL statement for request insertion into newRequests table
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_insertNewReqStatementString =
"INSERT INTO newRequests (id, type, creation) VALUES (:1, :2, SYSDATE)";

/// SQL select statement for member parameters
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_selectQueryParameterStatementString =
"SELECT id from QueryParameter WHERE query = :1 FOR UPDATE";

/// SQL delete statement for member parameters
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_deleteQueryParameterStatementString =
"UPDATE QueryParameter SET query = 0 WHERE id = :1";

/// SQL remote update statement for member parameters
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_remoteUpdateQueryParameterStatementString =
"UPDATE QueryParameter SET query = :1 WHERE id = :2";

/// SQL existence statement for member svcClass
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_checkSvcClassExistStatementString =
"SELECT id from SvcClass WHERE id = :1";

/// SQL update statement for member svcClass
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_updateSvcClassStatementString =
"UPDATE StageFindRequestRequest SET svcClass = :1 WHERE id = :2";

/// SQL update statement for member client
const std::string castor::db::ora::OraStageFindRequestRequestCnv::s_updateIClientStatementString =
"UPDATE StageFindRequestRequest SET client = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraStageFindRequestRequestCnv::OraStageFindRequestRequestCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
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
castor::db::ora::OraStageFindRequestRequestCnv::~OraStageFindRequestRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraStageFindRequestRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_insertNewReqStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_deleteQueryParameterStatement);
    deleteStatement(m_selectQueryParameterStatement);
    deleteStatement(m_remoteUpdateQueryParameterStatement);
    deleteStatement(m_checkSvcClassExistStatement);
    deleteStatement(m_updateSvcClassStatement);
    deleteStatement(m_updateIClientStatement);
  } catch (oracle::occi::SQLException e) {};
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
const unsigned int castor::db::ora::OraStageFindRequestRequestCnv::ObjType() {
  return castor::stager::StageFindRequestRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraStageFindRequestRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraStageFindRequestRequestCnv::fillRep(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             unsigned int type,
                                                             bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageFindRequestRequest* obj = 
    dynamic_cast<castor::stager::StageFindRequestRequest*>(object);
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
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex; 
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepQueryParameter
//------------------------------------------------------------------------------
void castor::db::ora::OraStageFindRequestRequestCnv::fillRepQueryParameter(castor::stager::StageFindRequestRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // check select statement
  if (0 == m_selectQueryParameterStatement) {
    m_selectQueryParameterStatement = createStatement(s_selectQueryParameterStatementString);
  }
  // Get current database data
  std::set<int> parametersList;
  m_selectQueryParameterStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectQueryParameterStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    parametersList.insert(rset->getInt(1));
  }
  m_selectQueryParameterStatement->closeResultSet(rset);
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
      m_remoteUpdateQueryParameterStatement->setDouble(1, obj->id());
      m_remoteUpdateQueryParameterStatement->setDouble(2, (*it)->id());
      m_remoteUpdateQueryParameterStatement->executeUpdate();
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
    m_deleteQueryParameterStatement->setDouble(1, *it);
    m_deleteQueryParameterStatement->executeUpdate();
  }
}

//------------------------------------------------------------------------------
// fillRepSvcClass
//------------------------------------------------------------------------------
void castor::db::ora::OraStageFindRequestRequestCnv::fillRepSvcClass(castor::stager::StageFindRequestRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->svcClass()) {
    // Check checkSvcClassExist statement
    if (0 == m_checkSvcClassExistStatement) {
      m_checkSvcClassExistStatement = createStatement(s_checkSvcClassExistStatementString);
    }
    // retrieve the object from the database
    m_checkSvcClassExistStatement->setDouble(1, obj->svcClass()->id());
    oracle::occi::ResultSet *rset = m_checkSvcClassExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->svcClass(), false);
    }
    // Close resultset
    m_checkSvcClassExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateSvcClassStatement) {
    m_updateSvcClassStatement = createStatement(s_updateSvcClassStatementString);
  }
  // Update local object
  m_updateSvcClassStatement->setDouble(1, 0 == obj->svcClass() ? 0 : obj->svcClass()->id());
  m_updateSvcClassStatement->setDouble(2, obj->id());
  m_updateSvcClassStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillRepIClient
//------------------------------------------------------------------------------
void castor::db::ora::OraStageFindRequestRequestCnv::fillRepIClient(castor::stager::StageFindRequestRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // Check update statement
  if (0 == m_updateIClientStatement) {
    m_updateIClientStatement = createStatement(s_updateIClientStatementString);
  }
  // Update local object
  m_updateIClientStatement->setDouble(1, 0 == obj->client() ? 0 : obj->client()->id());
  m_updateIClientStatement->setDouble(2, obj->id());
  m_updateIClientStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraStageFindRequestRequestCnv::fillObj(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             unsigned int type,
                                                             bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageFindRequestRequest* obj = 
    dynamic_cast<castor::stager::StageFindRequestRequest*>(object);
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
void castor::db::ora::OraStageFindRequestRequestCnv::fillObjQueryParameter(castor::stager::StageFindRequestRequest* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectQueryParameterStatement) {
    m_selectQueryParameterStatement = createStatement(s_selectQueryParameterStatementString);
  }
  // retrieve the object from the database
  std::set<int> parametersList;
  m_selectQueryParameterStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectQueryParameterStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    parametersList.insert(rset->getInt(1));
  }
  // Close ResultSet
  m_selectQueryParameterStatement->closeResultSet(rset);
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
void castor::db::ora::OraStageFindRequestRequestCnv::fillObjSvcClass(castor::stager::StageFindRequestRequest* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
  if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 svcClassId = (u_signed64)rset->getDouble(14);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
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
void castor::db::ora::OraStageFindRequestRequestCnv::fillObjIClient(castor::stager::StageFindRequestRequest* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
  if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 clientId = (u_signed64)rset->getDouble(15);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
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
void castor::db::ora::OraStageFindRequestRequestCnv::createRep(castor::IAddress* address,
                                                               castor::IObject* object,
                                                               bool autocommit,
                                                               unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::StageFindRequestRequest* obj = 
    dynamic_cast<castor::stager::StageFindRequestRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(15, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_insertNewReqStatement) {
      m_insertNewReqStatement = createStatement(s_insertNewReqStatementString);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setDouble(1, obj->flags());
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
    m_insertStatement->setDouble(13, (type == OBJ_SvcClass && obj->svcClass() != 0) ? obj->svcClass()->id() : 0);
    m_insertStatement->setDouble(14, (type == OBJ_IClient && obj->client() != 0) ? obj->client()->id() : 0);
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(15));
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    m_insertNewReqStatement->setDouble(1, obj->id());
    m_insertNewReqStatement->setInt(2, obj->type());
    m_insertNewReqStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (oracle::occi::SQLException e) {
      handleException(e);
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "Error in insert request :"
                      << std::endl << e.what() << std::endl
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
                      << "  id : " << obj->id() << std::endl
                      << "  svcClass : " << obj->svcClass() << std::endl
                      << "  client : " << obj->client() << std::endl;
      throw ex;
    }
}

  //----------------------------------------------------------------------------
  // updateRep
  //----------------------------------------------------------------------------
  void castor::db::ora::OraStageFindRequestRequestCnv::updateRep(castor::IAddress* address,
                                                                 castor::IObject* object,
                                                                 bool autocommit)
    throw (castor::exception::Exception) {
    castor::stager::StageFindRequestRequest* obj = 
      dynamic_cast<castor::stager::StageFindRequestRequest*>(object);
    // check whether something needs to be done
    if (0 == obj) return;
    try {
      // Check whether the statements are ok
      if (0 == m_updateStatement) {
        m_updateStatement = createStatement(s_updateStatementString);
      }
      // Update the current object
      m_updateStatement->setDouble(1, obj->flags());
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
      m_updateStatement->setDouble(12, obj->id());
      m_updateStatement->executeUpdate();
      if (autocommit) {
        cnvSvc()->commit();
      }
    } catch (oracle::occi::SQLException e) {
        handleException(e);
        castor::exception::InvalidArgument ex;
        ex.getMessage() << "Error in update request :"
                        << std::endl << e.what() << std::endl
                        << "Statement was :" << std::endl
                        << s_updateStatementString << std::endl
                        << "and id was " << obj->id() << std::endl;;
        throw ex;
      }
}

    //--------------------------------------------------------------------------
    // deleteRep
    //--------------------------------------------------------------------------
    void castor::db::ora::OraStageFindRequestRequestCnv::deleteRep(castor::IAddress* address,
                                                                   castor::IObject* object,
                                                                   bool autocommit)
      throw (castor::exception::Exception) {
      castor::stager::StageFindRequestRequest* obj = 
        dynamic_cast<castor::stager::StageFindRequestRequest*>(object);
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
        m_deleteTypeStatement->setDouble(1, obj->id());
        m_deleteTypeStatement->executeUpdate();
        m_deleteStatement->setDouble(1, obj->id());
        m_deleteStatement->executeUpdate();
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
      } catch (oracle::occi::SQLException e) {
          handleException(e);
          castor::exception::InvalidArgument ex;
          ex.getMessage() << "Error in delete request :"
                          << std::endl << e.what() << std::endl
                          << "Statement was :" << std::endl
                          << s_deleteStatementString << std::endl
                          << "and id was " << obj->id() << std::endl;;
          throw ex;
        }
      }

      //------------------------------------------------------------------------
      // createObj
      //------------------------------------------------------------------------
      castor::IObject* castor::db::ora::OraStageFindRequestRequestCnv::createObj(castor::IAddress* address)
        throw (castor::exception::Exception) {
        castor::BaseAddress* ad = 
          dynamic_cast<castor::BaseAddress*>(address);
        try {
          // Check whether the statement is ok
          if (0 == m_selectStatement) {
            m_selectStatement = createStatement(s_selectStatementString);
          }
          // retrieve the object from the database
          m_selectStatement->setDouble(1, ad->target());
          oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
          if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
            castor::exception::NoEntry ex;
            ex.getMessage() << "No object found for id :" << ad->target();
            throw ex;
          }
          // create the new Object
          castor::stager::StageFindRequestRequest* object = new castor::stager::StageFindRequestRequest();
          // Now retrieve and set members
          object->setFlags((u_signed64)rset->getDouble(1));
          object->setUserName(rset->getString(2));
          object->setEuid(rset->getInt(3));
          object->setEgid(rset->getInt(4));
          object->setMask(rset->getInt(5));
          object->setPid(rset->getInt(6));
          object->setMachine(rset->getString(7));
          object->setSvcClassName(rset->getString(8));
          object->setUserTag(rset->getString(9));
          object->setReqId(rset->getString(10));
          object->setCreationTime((u_signed64)rset->getDouble(11));
          object->setLastModificationTime((u_signed64)rset->getDouble(12));
          object->setId((u_signed64)rset->getDouble(13));
          m_selectStatement->closeResultSet(rset);
          return object;
        } catch (oracle::occi::SQLException e) {
            handleException(e);
            castor::exception::InvalidArgument ex;
            ex.getMessage() << "Error in select request :"
                            << std::endl << e.what() << std::endl
                            << "Statement was :" << std::endl
                            << s_selectStatementString << std::endl
                            << "and id was " << ad->target() << std::endl;;
            throw ex;
          }
        }

        //----------------------------------------------------------------------
        // updateObj
        //----------------------------------------------------------------------
        void castor::db::ora::OraStageFindRequestRequestCnv::updateObj(castor::IObject* obj)
          throw (castor::exception::Exception) {
          try {
            // Check whether the statement is ok
            if (0 == m_selectStatement) {
              m_selectStatement = createStatement(s_selectStatementString);
            }
            // retrieve the object from the database
            m_selectStatement->setDouble(1, obj->id());
            oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
            if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
              castor::exception::NoEntry ex;
              ex.getMessage() << "No object found for id :" << obj->id();
              throw ex;
            }
            // Now retrieve and set members
            castor::stager::StageFindRequestRequest* object = 
              dynamic_cast<castor::stager::StageFindRequestRequest*>(obj);
            object->setFlags((u_signed64)rset->getDouble(1));
            object->setUserName(rset->getString(2));
            object->setEuid(rset->getInt(3));
            object->setEgid(rset->getInt(4));
            object->setMask(rset->getInt(5));
            object->setPid(rset->getInt(6));
            object->setMachine(rset->getString(7));
            object->setSvcClassName(rset->getString(8));
            object->setUserTag(rset->getString(9));
            object->setReqId(rset->getString(10));
            object->setCreationTime((u_signed64)rset->getDouble(11));
            object->setLastModificationTime((u_signed64)rset->getDouble(12));
            object->setId((u_signed64)rset->getDouble(13));
            m_selectStatement->closeResultSet(rset);
          } catch (oracle::occi::SQLException e) {
              handleException(e);
              castor::exception::InvalidArgument ex;
              ex.getMessage() << "Error in update request :"
                              << std::endl << e.what() << std::endl
                              << "Statement was :" << std::endl
                              << s_updateStatementString << std::endl
                              << "and id was " << obj->id() << std::endl;;
              throw ex;
            }
}


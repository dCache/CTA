/******************************************************************************
 *                      castor/db/ora/OraStageOutRequestCnv.cpp
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
 * @(#)$RCSfile: OraStageOutRequestCnv.cpp,v $ $Revision: 1.4 $ $Release$ $Date: 2004/10/07 14:33:59 $ $Author: sponcec3 $
 *
 * 
 *
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

// Include Files
#include "OraStageOutRequestCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/IConverter.hpp"
#include "castor/IFactory.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/db/DbAddress.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/RequestStatusCodes.hpp"
#include "castor/stager/StageOutRequest.hpp"
#include "castor/stager/SubRequest.hpp"
#include <list>
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraStageOutRequestCnv> s_factoryOraStageOutRequestCnv;
const castor::IFactory<castor::IConverter>& OraStageOutRequestCnvFactory = 
  s_factoryOraStageOutRequestCnv;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraStageOutRequestCnv::s_insertStatementString =
"INSERT INTO rh_StageOutRequest (flags, userName, euid, egid, mask, pid, machine, projectName, openmode, id, client, status) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12)";

/// SQL statement for request deletion
const std::string castor::db::ora::OraStageOutRequestCnv::s_deleteStatementString =
"DELETE FROM rh_StageOutRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraStageOutRequestCnv::s_selectStatementString =
"SELECT flags, userName, euid, egid, mask, pid, machine, projectName, openmode, id, client, status FROM rh_StageOutRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraStageOutRequestCnv::s_updateStatementString =
"UPDATE rh_StageOutRequest SET flags = :1, userName = :2, euid = :3, egid = :4, mask = :5, pid = :6, machine = :7, projectName = :8, openmode = :9, client = :10, status = :11 WHERE id = :12";

/// SQL statement for type storage
const std::string castor::db::ora::OraStageOutRequestCnv::s_storeTypeStatementString =
"INSERT INTO rh_Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraStageOutRequestCnv::s_deleteTypeStatementString =
"DELETE FROM rh_Id2Type WHERE id = :1";

/// SQL statement for request status insertion
const std::string castor::db::ora::OraStageOutRequestCnv::s_insertStatusStatementString =
"INSERT INTO rh_requestsStatus (id, status, creation, lastChange) VALUES (:1, 'NEW', SYSDATE, SYSDATE)";

/// SQL statement for request status deletion
const std::string castor::db::ora::OraStageOutRequestCnv::s_deleteStatusStatementString =
"DELETE FROM rh_requestsStatus WHERE id = :1";

/// SQL select statement for member subRequests
const std::string castor::db::ora::OraStageOutRequestCnv::s_Request2SubRequestStatementString =
"SELECT Child from rh_Request2SubRequest WHERE Parent = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraStageOutRequestCnv::OraStageOutRequestCnv() :
  OraBaseCnv(),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_insertStatusStatement(0),
  m_deleteStatusStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_Request2SubRequestStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraStageOutRequestCnv::~OraStageOutRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraStageOutRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_insertStatusStatement);
    deleteStatement(m_deleteStatusStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_Request2SubRequestStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_insertStatusStatement = 0;
  m_deleteStatusStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_Request2SubRequestStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraStageOutRequestCnv::ObjType() {
  return castor::stager::StageOutRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraStageOutRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraStageOutRequestCnv::createRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       castor::ObjectSet& alreadyDone,
                                                       bool autocommit,
                                                       bool recursive)
  throw (castor::exception::Exception) {
  castor::stager::StageOutRequest* obj = 
    dynamic_cast<castor::stager::StageOutRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
    }
    if (0 == m_insertStatusStatement) {
      m_insertStatusStatement = createStatement(s_insertStatusStatementString);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Mark the current object as done
    alreadyDone.insert(obj);
    // Set ids of all objects
    int nids = obj->id() == 0 ? 1 : 0;
    // check which objects need to be saved/updated and keeps a list of them
    std::list<castor::IObject*> toBeSaved;
    std::list<castor::IObject*> toBeUpdated;
    if (recursive) {
      for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
           it != obj->subRequests().end();
           it++) {
        if (alreadyDone.find(*it) == alreadyDone.end()) {
          if (0 == (*it)->id()) {
            toBeSaved.push_back(*it);
            nids++;
          } else {
            toBeUpdated.push_back(*it);
          }
        }
      }
    }
    if (recursive) {
      if (alreadyDone.find(obj->client()) == alreadyDone.end() &&
          obj->client() != 0) {
        if (0 == obj->client()->id()) {
          toBeSaved.push_back(obj->client());
          nids++;
        } else {
          toBeUpdated.push_back(obj->client());
        }
      }
    }
    u_signed64 id = cnvSvc()->getIds(nids);
    if (0 == obj->id()) obj->setId(id++);
    for (std::list<castor::IObject*>::const_iterator it = toBeSaved.begin();
         it != toBeSaved.end();
         it++) {
      (*it)->setId(id++);
    }
    // Now Save the current object
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    m_insertStatusStatement->setDouble(1, obj->id());
    m_insertStatusStatement->executeUpdate();
    m_insertStatement->setDouble(1, obj->flags());
    m_insertStatement->setString(2, obj->userName());
    m_insertStatement->setInt(3, obj->euid());
    m_insertStatement->setInt(4, obj->egid());
    m_insertStatement->setInt(5, obj->mask());
    m_insertStatement->setInt(6, obj->pid());
    m_insertStatement->setString(7, obj->machine());
    m_insertStatement->setString(8, obj->projectName());
    m_insertStatement->setInt(9, obj->openmode());
    m_insertStatement->setDouble(10, obj->id());
    m_insertStatement->setDouble(11, obj->client() ? obj->client()->id() : 0);
    m_insertStatement->setDouble(12, (int)obj->status());
    m_insertStatement->executeUpdate();
    if (recursive) {
      // Save dependant objects that need it
      for (std::list<castor::IObject*>::iterator it = toBeSaved.begin();
           it != toBeSaved.end();
           it++) {
        cnvSvc()->createRep(0, *it, alreadyDone, false, true);
      }
      // Update dependant objects that need it
      for (std::list<castor::IObject*>::iterator it = toBeUpdated.begin();
           it != toBeUpdated.end();
           it++) {
        cnvSvc()->updateRep(0, *it, alreadyDone, false, true);
      }
    }
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
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
                    << "  projectName : " << obj->projectName() << std::endl
                    << "  openmode : " << obj->openmode() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  client : " << obj->client() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::ora::OraStageOutRequestCnv::updateRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       castor::ObjectSet& alreadyDone,
                                                       bool autocommit,
                                                       bool recursive)
  throw (castor::exception::Exception) {
  castor::stager::StageOutRequest* obj = 
    dynamic_cast<castor::stager::StageOutRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    if (0 == m_updateStatement) {
      castor::exception::Internal ex;
      ex.getMessage() << "Unable to create statement :" << std::endl
                      << s_updateStatementString;
      throw ex;
    }
    if (recursive) {
      if (0 == m_selectStatement) {
        m_selectStatement = createStatement(s_selectStatementString);
      }
      if (0 == m_selectStatement) {
        castor::exception::Internal ex;
        ex.getMessage() << "Unable to create statement :" << std::endl
                        << s_selectStatementString;
        throw ex;
      }
    }
    // Mark the current object as done
    alreadyDone.insert(obj);
    if (recursive) {
      // retrieve the object from the database
      m_selectStatement->setDouble(1, obj->id());
      oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
      if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
        castor::exception::NoEntry ex;
        ex.getMessage() << "No object found for id :" << obj->id();
        throw ex;
      }
      // Dealing with client
      {
        u_signed64 clientId = (unsigned long long)rset->getDouble(11);
        castor::db::DbAddress ad(clientId, " ", 0);
        if (0 != clientId &&
            0 != obj->client() &&
            obj->client()->id() != clientId) {
          cnvSvc()->deleteRepByAddress(&ad, false);
          clientId = 0;
        }
        if (clientId == 0) {
          if (0 != obj->client()) {
            if (alreadyDone.find(obj->client()) == alreadyDone.end()) {
              cnvSvc()->createRep(&ad, obj->client(), alreadyDone, false, true);
            }
          }
        } else {
          if (alreadyDone.find(obj->client()) == alreadyDone.end()) {
            cnvSvc()->updateRep(&ad, obj->client(), alreadyDone, false, recursive);
          }
        }
      }
      m_selectStatement->closeResultSet(rset);
    }
    // Now Update the current object
    m_updateStatement->setDouble(1, obj->flags());
    m_updateStatement->setString(2, obj->userName());
    m_updateStatement->setInt(3, obj->euid());
    m_updateStatement->setInt(4, obj->egid());
    m_updateStatement->setInt(5, obj->mask());
    m_updateStatement->setInt(6, obj->pid());
    m_updateStatement->setString(7, obj->machine());
    m_updateStatement->setString(8, obj->projectName());
    m_updateStatement->setInt(9, obj->openmode());
    m_updateStatement->setDouble(10, obj->client() ? obj->client()->id() : 0);
    m_updateStatement->setDouble(11, (int)obj->status());
    m_updateStatement->setDouble(12, obj->id());
    m_updateStatement->executeUpdate();
    if (recursive) {
      // Dealing with subRequests
      {
        if (0 == m_Request2SubRequestStatement) {
          m_Request2SubRequestStatement = createStatement(s_Request2SubRequestStatementString);
        }
        std::set<int> subRequestsList;
        m_Request2SubRequestStatement->setDouble(1, obj->id());
        oracle::occi::ResultSet *rset = m_Request2SubRequestStatement->executeQuery();
        while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
          subRequestsList.insert(rset->getInt(1));
        }
        m_Request2SubRequestStatement->closeResultSet(rset);
        for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
             it != obj->subRequests().end();
             it++) {
          std::set<int>::iterator item;
          if ((item = subRequestsList.find((*it)->id())) == subRequestsList.end()) {
            if (alreadyDone.find(*it) == alreadyDone.end()) {
              cnvSvc()->createRep(0, *it, alreadyDone, false, true);
            }
          } else {
            subRequestsList.erase(item);
            if (alreadyDone.find(*it) == alreadyDone.end()) {
              cnvSvc()->updateRep(0, *it, alreadyDone, false, recursive);
            }
          }
        }
        for (std::set<int>::iterator it = subRequestsList.begin();
             it != subRequestsList.end();
             it++) {
          castor::db::DbAddress ad(*it, " ", 0);
          cnvSvc()->deleteRepByAddress(&ad, false);
        }
      }
    }
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::ora::OraStageOutRequestCnv::deleteRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       castor::ObjectSet& alreadyDone,
                                                       bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageOutRequest* obj = 
    dynamic_cast<castor::stager::StageOutRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_deleteStatement) {
      m_deleteStatement = createStatement(s_deleteStatementString);
    }
    if (0 == m_deleteStatusStatement) {
      m_deleteStatusStatement = createStatement(s_deleteStatusStatementString);
    }
    if (0 == m_deleteTypeStatement) {
      m_deleteTypeStatement = createStatement(s_deleteTypeStatementString);
    }
    // Mark the current object as done
    alreadyDone.insert(obj);
    // Now Delete the object
    m_deleteTypeStatement->setDouble(1, obj->id());
    m_deleteTypeStatement->executeUpdate();
    m_deleteStatement->setDouble(1, obj->id());
    m_deleteStatement->executeUpdate();
    m_deleteStatusStatement->setDouble(1, obj->id());
    m_deleteStatusStatement->executeUpdate();
    for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
         it != obj->subRequests().end();
         it++) {
      if (alreadyDone.find(*it) == alreadyDone.end()) {
        cnvSvc()->deleteRep(0, *it, alreadyDone, false);
      }
    }
    if (alreadyDone.find(obj->client()) == alreadyDone.end() &&
        obj->client() != 0) {
      cnvSvc()->deleteRep(0, obj->client(), alreadyDone, false);
    }
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in delete request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_deleteStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::db::ora::OraStageOutRequestCnv::createObj(castor::IAddress* address,
                                                                   castor::ObjectCatalog& newlyCreated,
                                                                   bool recursive)
  throw (castor::exception::Exception) {
  castor::db::DbAddress* ad = 
    dynamic_cast<castor::db::DbAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    if (0 == m_selectStatement) {
      castor::exception::Internal ex;
      ex.getMessage() << "Unable to create statement :" << std::endl
                      << s_selectStatementString;
      throw ex;
    }
    // retrieve the object from the database
    m_selectStatement->setDouble(1, ad->id());
    oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << ad->id();
      throw ex;
    }
    // create the new Object
    castor::stager::StageOutRequest* object = new castor::stager::StageOutRequest();
    // Now retrieve and set members
    object->setFlags((unsigned long long)rset->getDouble(1));
    object->setUserName(rset->getString(2));
    object->setEuid(rset->getInt(3));
    object->setEgid(rset->getInt(4));
    object->setMask(rset->getInt(5));
    object->setPid(rset->getInt(6));
    object->setMachine(rset->getString(7));
    object->setProjectName(rset->getString(8));
    object->setOpenmode(rset->getInt(9));
    object->setId((unsigned long long)rset->getDouble(10));
    newlyCreated[object->id()] = object;
    object->setStatus((enum castor::stager::RequestStatusCodes)rset->getInt(11));
    if (recursive) {
      u_signed64 clientId = (unsigned long long)rset->getDouble(12);
      IObject* objClient = cnvSvc()->getObjFromId(clientId, newlyCreated);
      object->setClient(dynamic_cast<castor::IClient*>(objClient));
    }
    m_selectStatement->closeResultSet(rset);
    if (recursive) {
      // Get ids of objs to retrieve
      if (0 == m_Request2SubRequestStatement) {
        m_Request2SubRequestStatement = createStatement(s_Request2SubRequestStatementString);
      }
      m_Request2SubRequestStatement->setDouble(1, ad->id());
      rset = m_Request2SubRequestStatement->executeQuery();
      while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
        IObject* obj = cnvSvc()->getObjFromId(rset->getInt(1), newlyCreated);
        object->addSubRequests(dynamic_cast<castor::stager::SubRequest*>(obj));
      }
      m_Request2SubRequestStatement->closeResultSet(rset);
    }
    return object;
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in select request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_selectStatementString << std::endl
                    << "and id was " << ad->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::db::ora::OraStageOutRequestCnv::updateObj(castor::IObject* obj,
                                                       castor::ObjectCatalog& alreadyDone)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    if (0 == m_selectStatement) {
      castor::exception::Internal ex;
      ex.getMessage() << "Unable to create statement :" << std::endl
                      << s_selectStatementString;
      throw ex;
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
    castor::stager::StageOutRequest* object = 
      dynamic_cast<castor::stager::StageOutRequest*>(obj);
    object->setFlags((unsigned long long)rset->getDouble(1));
    object->setUserName(rset->getString(2));
    object->setEuid(rset->getInt(3));
    object->setEgid(rset->getInt(4));
    object->setMask(rset->getInt(5));
    object->setPid(rset->getInt(6));
    object->setMachine(rset->getString(7));
    object->setProjectName(rset->getString(8));
    object->setOpenmode(rset->getInt(9));
    object->setId((unsigned long long)rset->getDouble(10));
    alreadyDone[obj->id()] = obj;
    // Dealing with client
    u_signed64 clientId = (unsigned long long)rset->getDouble(11);
    if (0 != object->client() &&
        (0 == clientId ||
         object->client()->id() != clientId)) {
      delete object->client();
      object->setClient(0);
    }
    if (0 != clientId) {
      if (0 == object->client()) {
        object->setClient
          (dynamic_cast<castor::IClient*>
           (cnvSvc()->getObjFromId(clientId, alreadyDone)));
      } else if (object->client()->id() == clientId) {
        if (alreadyDone.find(object->client()->id()) == alreadyDone.end()) {
          cnvSvc()->updateObj(object->client(), alreadyDone);
        }
      }
    }
    object->setStatus((enum castor::stager::RequestStatusCodes)rset->getInt(12));
    m_selectStatement->closeResultSet(rset);
    // Deal with subRequests
    if (0 == m_Request2SubRequestStatement) {
      m_Request2SubRequestStatement = createStatement(s_Request2SubRequestStatementString);
    }
    std::set<int> subRequestsList;
    m_Request2SubRequestStatement->setDouble(1, obj->id());
    rset = m_Request2SubRequestStatement->executeQuery();
    while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
      subRequestsList.insert(rset->getInt(1));
    }
    m_Request2SubRequestStatement->closeResultSet(rset);
    {
      std::vector<castor::stager::SubRequest*> toBeDeleted;
      for (std::vector<castor::stager::SubRequest*>::iterator it = object->subRequests().begin();
           it != object->subRequests().end();
           it++) {
        std::set<int>::iterator item;
        if ((item = subRequestsList.find((*it)->id())) == subRequestsList.end()) {
          toBeDeleted.push_back(*it);
        } else {
          subRequestsList.erase(item);
          cnvSvc()->updateObj((*it), alreadyDone);
        }
      }
      for (std::vector<castor::stager::SubRequest*>::iterator it = toBeDeleted.begin();
           it != toBeDeleted.end();
           it++) {
        object->removeSubRequests(*it);
        delete (*it);
      }
    }
    for (std::set<int>::iterator it = subRequestsList.begin();
         it != subRequestsList.end();
         it++) {
      IObject* item = cnvSvc()->getObjFromId(*it, alreadyDone);
      object->addSubRequests(dynamic_cast<castor::stager::SubRequest*>(item));
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


/******************************************************************************
 *                      OraCnvSvc.cpp
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
 * @author Sebastien Ponce
 *****************************************************************************/

// Include Files
#include "castor/Constants.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/MsgSvc.hpp"
#include "castor/SvcFactory.hpp"
#include "castor/db/DbAddress.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include <iomanip>
#include <Cthread_api.h>

// Local Files
#include "OraCnvSvc.hpp"

// -----------------------------------------------------------------------
// External C function used for getting configuration from shift.conf file
// -----------------------------------------------------------------------
extern "C" {
  char* getconfent (const char *, char *, int);
}

// -----------------------------------------------------------------------
// Instantiation of a static factory class
// -----------------------------------------------------------------------
static castor::SvcFactory<castor::db::ora::OraCnvSvc> s_factoryOraCnvSvc;
const castor::IFactory<castor::IService>& OraCnvSvcFactory = s_factoryOraCnvSvc;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for type retrieval
const std::string castor::db::ora::OraCnvSvc::s_getTypeStatementString =
  "SELECT type FROM rh_Id2Type WHERE id = :1";

/// SQL statement for id retrieval
const std::string castor::db::ora::OraCnvSvc::s_getIdStatementString =
  "BEGIN UPDATE rh_indices SET value = value + :1 WHERE name='next_id' RETURNING value INTO :result; END;";

/// SQL statement for selecting next request to be processed
const std::string castor::db::ora::OraCnvSvc::s_getNRStatementString =
  "BEGIN UPDATE rh_requestsStatus SET status = 'RUNNING', lastChange = SYSDATE WHERE status = 'NEW' AND rownum <=1 RETURNING ID INTO :result; END;";

/// SQL statement for getting index of next request to be processed
const std::string castor::db::ora::OraCnvSvc::s_getNBRStatementString =
  "SELECT count(*) FROM rh_requestsStatus WHERE status = 'NEW'";

// -----------------------------------------------------------------------
// OraCnvSvc
// -----------------------------------------------------------------------
castor::db::ora::OraCnvSvc::OraCnvSvc(const std::string name) :
  BaseCnvSvc(name),
  m_user(""),
  m_passwd(""),
  m_dbName(""),
  m_environment(0),
  m_connection(0),
  m_getTypeStatement(0),
  m_getIdStatement(0),
  m_getNRStatement(0),
  m_getNBRStatement(0) {
  char* cuser = getconfent(name.c_str(), "user", 0);
  if (0 != cuser) m_user = std::string(cuser);
  char* cpasswd = getconfent(name.c_str(), "passwd", 0);
  if (0 != cpasswd) m_passwd = std::string(cpasswd);
  char* cdbName = getconfent(name.c_str(), "dbName", 0);
  if (0 != cdbName) m_dbName = std::string(cdbName);
}

// -----------------------------------------------------------------------
// ~OraCnvSvc
// -----------------------------------------------------------------------
castor::db::ora::OraCnvSvc::~OraCnvSvc() {
  if (0 != m_connection) {
    m_connection->terminateStatement(m_getTypeStatement);
    m_connection->terminateStatement(m_getIdStatement);
    m_connection->terminateStatement(m_getNRStatement);
    m_connection->terminateStatement(m_getNBRStatement);
    deleteConnection(m_connection);
  }
  if (0 != m_environment) {
    oracle::occi::Environment::terminateEnvironment(m_environment);
  }
}

// -----------------------------------------------------------------------
// id
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraCnvSvc::id() const {
  return ID();
}

// -----------------------------------------------------------------------
// ID
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraCnvSvc::ID() {
  return castor::SVC_ORACNV;
}

// -----------------------------------------------------------------------
// repType
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraCnvSvc::repType() const {
  return REPTYPE();
}

// -----------------------------------------------------------------------
// REPTYPE
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraCnvSvc::REPTYPE() {
  return castor::REP_ORACLE;
}

//------------------------------------------------------------------------------
// Utility function to get the current timestamp
//------------------------------------------------------------------------------
std::string castor::db::ora::OraCnvSvc::getTimestamp() {
  struct tm tmstruc, *tm;
  time_t current_time;
  (void) time (&current_time);
  (void) localtime_r (&current_time, &tmstruc);
  tm = &tmstruc;
  std::ostringstream buf;
  buf << std::setw(2) <<tm->tm_mon+1 << "/" << tm->tm_mday << " "
      << tm->tm_hour << ":" <<  tm->tm_min << ":" << tm->tm_sec
      << " " << Cthread_self() << " ";
  return buf.str();
}

// -----------------------------------------------------------------------
// getConnection
// -----------------------------------------------------------------------
oracle::occi::Connection* castor::db::ora::OraCnvSvc::getConnection()
  throw (oracle::occi::SQLException) {
  if (0 == m_environment) {
    m_environment = oracle::occi::Environment::createEnvironment
      (oracle::occi::Environment::THREADED_MUTEXED);
  }
  if (0 == m_connection) {
    m_connection =
      m_environment->createConnection(m_user, m_passwd, m_dbName);
    msgSvc()->stream() << getTimestamp()
                       << " Created new Oracle connection : "
                       << std::ios::hex << m_connection
                       << std::ios::dec << std::endl;
    //oracle::occi::Statement* stmt = m_connection->createStatement
    //  ("alter session set events '10046 trace name context forever, level 8'");
    //stmt->executeUpdate();
    //m_connection->terminateStatement(stmt);
    //m_connection->commit();
  }
  return m_connection;
}

// -----------------------------------------------------------------------
// deleteConnection
// -----------------------------------------------------------------------
void castor::db::ora::OraCnvSvc::deleteConnection
(oracle::occi::Connection* connection) throw() {
  if (0 != m_environment) {
    //oracle::occi::Statement* stmt = m_connection->createStatement
    //  ("alter session set events '10046 trace name context off'");
    //stmt->executeUpdate();
    //m_connection->terminateStatement(stmt);
    //m_connection->commit();
    m_environment->terminateConnection(connection);
  }
}

// -----------------------------------------------------------------------
// createObj
// -----------------------------------------------------------------------
castor::IObject* castor::db::ora::OraCnvSvc::createObj (castor::IAddress* address,
                                                        ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  // If the address has no type, find it out
  if (OBJ_INVALID == address->objType()) {
    castor::db::DbAddress* ad =
      dynamic_cast<castor::db::DbAddress*>(address);
    unsigned int type = getTypeFromId(ad->id());
    if (0 == type) return 0;
    ad->setObjType(type);
  }
  // call method of parent object
  return this->BaseCnvSvc::createObj(address, newlyCreated);
}

// -----------------------------------------------------------------------
// getIds
// -----------------------------------------------------------------------
const unsigned long
castor::db::ora::OraCnvSvc::getIds(const unsigned int nids)
  throw (oracle::occi::SQLException) {
  if (0 == nids) return 0;
  if (0 == m_getIdStatement) {
    try {
      m_getIdStatement =
        getConnection()->createStatement(s_getIdStatementString);
      m_getIdStatement->setAutoCommit(true);
      m_getIdStatement->registerOutParam(2, oracle::occi::OCCIINT, sizeof(int));
    } catch (oracle::occi::SQLException e) {
      m_getIdStatement = 0;
      castor::exception::Internal ex;
      ex.getMessage() << "Error in creating get Id statement."
                      << std::endl << e.what();
      throw ex;
    }
  }
  try {
    m_getIdStatement->setInt(1, nids);
    m_getIdStatement->executeUpdate();
    return m_getIdStatement->getInt(2) - nids;
  } catch (oracle::occi::SQLException e) {
    castor::exception::InvalidArgument ex; // XXX fix it depending on ORACLE error
    ex.getMessage() << "Error in getting next id."
                    << std::endl << e.what();
    throw ex;
  }
  return 0;
}

// -----------------------------------------------------------------------
// nextRequestAddress
// -----------------------------------------------------------------------
castor::IAddress* castor::db::ora::OraCnvSvc::nextRequestAddress()
  throw (castor::exception::Exception) {
  // Prepare statement
  if (0 == m_getNRStatement) {
    try {
      m_getNRStatement =
        getConnection()->createStatement(s_getNRStatementString);
      m_getNRStatement->setAutoCommit(true);
      m_getNRStatement->registerOutParam(1, oracle::occi::OCCIINT, sizeof(int));
    } catch (oracle::occi::SQLException e) {
      m_getNRStatement = 0;
      castor::exception::Internal ex;
      ex.getMessage() << "Error in creating next request statement."
                      << std::endl << e.what();
      throw ex;
    }
  }
  try {
    int nb = m_getNRStatement->executeUpdate();
    if (nb > 0) {
      return new DbAddress(m_getNRStatement->getInt(1), "OraCnvSvc", castor::SVC_ORACNV);  
    }
  } catch (oracle::occi::SQLException e) {
    castor::exception::InvalidArgument ex; // XXX fix it depending on ORACLE error
    ex.getMessage() << "Error in getting next request id."
                    << std::endl << e.what();
    throw ex;
  }
  return 0;
}

// -----------------------------------------------------------------------
// nbRequestsToProcess
// -----------------------------------------------------------------------
unsigned int castor::db::ora::OraCnvSvc::nbRequestsToProcess()
  throw (castor::exception::Exception) {
  try {
    // Prepare statements
    if (0 == m_getNBRStatement) {
      m_getNBRStatement =
        getConnection()->createStatement(s_getNBRStatementString);
    }
    // Get the id
    oracle::occi::ResultSet *rset = m_getNBRStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      return 0;
    } else {
      return rset->getInt(1);
    }
  } catch (oracle::occi::SQLException e) {
    castor::exception::InvalidArgument ex; // XXX fix it depending on ORACLE error
    ex.getMessage() << "Error in getting number of request to process."
                    << std::endl << e.what();
    try {
      getConnection()->rollback();
    } catch (oracle::occi::SQLException e2) {
      // XXX fix it depending on ORACLE error
      ex.getMessage() << std::endl
                      << "After that, the rollback also failed :"
                      << std::endl << e2.what();
    }
    throw ex;
  }
  // never reached
  return 0;
}

// -----------------------------------------------------------------------
// getTypeFromId
// -----------------------------------------------------------------------
const unsigned int
castor::db::ora::OraCnvSvc::getTypeFromId(const unsigned long id)
  throw (castor::exception::Exception) {
  // a null id has a null type
  if (0 == id) return 0;
  try {
    // Check whether the statement is ok
    if (0 == m_getTypeStatement) {
      m_getTypeStatement = getConnection()->createStatement();
      m_getTypeStatement->setSQL(s_getTypeStatementString);
    }
    // Execute it
    m_getTypeStatement->setInt(1, id);
    oracle::occi::ResultSet *rset = m_getTypeStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No type found for id : " << id;
      throw ex;
    }
    return rset->getInt(1);
  } catch (oracle::occi::SQLException e) {
    castor::exception::InvalidArgument ex; // XXX fix it depending on ORACLE error
    ex.getMessage() << "Error in getting type from id."
                    << std::endl << e.what();
    try {
      getConnection()->rollback();
    } catch (oracle::occi::SQLException e2) {
      // XXX fix it depending on ORACLE error
      ex.getMessage() << "After that, the rollback also failed :"
                      << std::endl << e2.what();
    }
    throw ex;
  }
  // never reached
  return OBJ_INVALID;
}

// -----------------------------------------------------------------------
// getObjFromId
// -----------------------------------------------------------------------
castor::IObject* castor::db::ora::OraCnvSvc::getObjFromId
(unsigned long id,
 castor::ObjectCatalog& newlyCreated)
  throw (castor::exception::Exception) {
  if (newlyCreated.find(id) != newlyCreated.end()) {
    return newlyCreated[id];
  } else {
    castor::db::DbAddress clientAd(id, "OraCnvSvc", repType());
    return createObj(&clientAd, newlyCreated);
  }
}

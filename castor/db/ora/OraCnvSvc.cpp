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
#include "castor/IConverter.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/SvcFactory.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/db/ora/OraBaseObj.hpp"
#include "castor/exception/BadVersion.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include <iomanip>

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
  "SELECT type FROM Id2Type WHERE id = :1";

/// SQL statement for selecting next request to be processed
const std::string castor::db::ora::OraCnvSvc::s_getNRStatementString =
  "BEGIN getNRStatement(:1); END;";

/// SQL statement for getting index of next request to be processed
const std::string castor::db::ora::OraCnvSvc::s_getNBRStatementString =
  "SELECT count(*) FROM requestsStatus WHERE status = 'NEW'";

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
  m_getNRStatement(0),
  m_getNBRStatement(0) {
  char* cuser = getconfent(name.c_str(), "user", 0);
  if (0 != cuser) m_user = std::string(cuser);
  char* cpasswd = getconfent(name.c_str(), "passwd", 0);
  if (0 != cpasswd) m_passwd = std::string(cpasswd);
  char* cdbName = getconfent(name.c_str(), "dbName", 0);
  if (0 != cdbName) m_dbName = std::string(cdbName);
  // Add alias for DiskCopyForRecall on DiskCopy
  addAlias(58, 5);
  // Add alias for TapeCopyForMigration on TapeCopy
  addAlias(59, 30);  
}

// -----------------------------------------------------------------------
// ~OraCnvSvc
// -----------------------------------------------------------------------
castor::db::ora::OraCnvSvc::~OraCnvSvc() throw() {
  dropConnection();
  if (0 != m_environment) {
    try {
      oracle::occi::Environment::terminateEnvironment(m_environment);
    } catch (...) { }
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

// -----------------------------------------------------------------------
// getConnection
// -----------------------------------------------------------------------
oracle::occi::Connection* castor::db::ora::OraCnvSvc::getConnection()
  throw (oracle::occi::SQLException,
         castor::exception::Exception) {
  // Quick answer if connection available
  if (0 != m_connection) return m_connection;
  // Else try to build one
  if (0 == m_environment) {
    m_environment = oracle::occi::Environment::createEnvironment
      (oracle::occi::Environment::THREADED_MUTEXED);
  }
  if (0 == m_connection) {
    m_connection =
      m_environment->createConnection(m_user, m_passwd, m_dbName);
    clog() << DEBUG << "Created new Oracle connection : "
           << m_connection << std::endl;
    std::string codeVersion = "2.0.0.0";
    std::string DBVersion = "";
    oracle::occi::Statement* stmt = 0;
    try {
      oracle::occi::Statement* stmt = m_connection->createStatement
        ("SELECT version FROM CastorVersion");
      oracle::occi::ResultSet *rset = stmt->executeQuery();
      if (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
        DBVersion = rset->getString(1);
      }   
      m_connection->terminateStatement(stmt);
      if (codeVersion != DBVersion) {
        castor::exception::BadVersion e;
        e.getMessage() << "Version mismatch between the database and the code : \""
                       << DBVersion << "\" versus \""
                       << codeVersion << "\"";
        throw e;
      }
    } catch (oracle::occi::SQLException ex) {
      // No CastorVersion table ?? This means bad version
      if (0 != stmt) m_connection->terminateStatement(stmt);
      castor::exception::BadVersion e;
      ex.getMessage() << "Not able to find the version of castor in the database"
                      << "\n Original error was " << e.what();
      throw ex;
    }
    
    // Uncomment this to unable tracing of the DB
    //stmt = m_connection->createStatement
    //  ("alter session set events '10046 trace name context forever, level 8'");
    //stmt->executeUpdate();
    //m_connection->terminateStatement(stmt);
    //m_connection->commit();
  }
  return m_connection;
}

// -----------------------------------------------------------------------
// dropConnection
// -----------------------------------------------------------------------
void castor::db::ora::OraCnvSvc::dropConnection () throw() {
  // make all registered converters aware
  for (std::set<castor::db::ora::OraBaseObj*>::const_iterator it =
         m_registeredCnvs.begin();
       it != m_registeredCnvs.end();
       it++) {
    (*it)->reset();
  }
  // drop the connection
  try {
    if (0 != m_connection) {
      if (0 != m_getTypeStatement)
        m_connection->terminateStatement(m_getTypeStatement);
      if (0 != m_getNRStatement)
        m_connection->terminateStatement(m_getNRStatement);
      if (0 != m_getNBRStatement)
          m_connection->terminateStatement(m_getNBRStatement);
      if (0 != m_environment) {
        //oracle::occi::Statement* stmt = m_connection->createStatement
        //  ("alter session set events '10046 trace name context off'");
        //stmt->executeUpdate();
        //m_connection->terminateStatement(stmt);
        //m_connection->commit();
        m_environment->terminateConnection(m_connection);
      }
    }
  } catch (oracle::occi::SQLException e) {};
  // reset all whatever the state is
  m_connection = 0;
  m_getTypeStatement = 0;
  m_getNRStatement = 0;
  m_getNBRStatement = 0;
}

// -----------------------------------------------------------------------
// createObj
// -----------------------------------------------------------------------
castor::IObject* castor::db::ora::OraCnvSvc::createObj (castor::IAddress* address)
  throw (castor::exception::Exception) {
  // If the address has no type, find it out
  if (OBJ_INVALID == address->objType()) {
    castor::BaseAddress* ad =
      dynamic_cast<castor::BaseAddress*>(address);
    unsigned int type = getTypeFromId(ad->target());
    if (0 == type) return 0;
    ad->setObjType(type);
  }
  // call method of parent object
  return this->BaseCnvSvc::createObj(address);
}

//------------------------------------------------------------------------------
// commit
//------------------------------------------------------------------------------
void castor::db::ora::OraCnvSvc::commit()
  throw (castor::exception::Exception) {
  try {
    if (0 != m_connection) {
      m_connection->commit();
    }
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error while commiting :"
                    << std::endl << e.what();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// rollback
//------------------------------------------------------------------------------
void castor::db::ora::OraCnvSvc::rollback()
  throw (castor::exception::Exception) {
  try {
    if (0 != m_connection) {
      m_connection->rollback();
    }
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error while rollbacking :"
                    << std::endl << e.what();
    throw ex;
  }
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
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        dropConnection();
      }
      m_getNRStatement = 0;
      castor::exception::Internal ex;
      ex.getMessage() << "Error in creating next request statement."
                      << std::endl << e.what();
      throw ex;
    }
  }
  try {
    //clog() << VERBOSE << "Trying to find new requests" << std::endl;
    int nb = m_getNRStatement->executeUpdate();
    if (nb > 0) {
      clog() << VERBOSE << "Found a new requests : "
             << m_getNRStatement->getInt(1) << std::endl;
      castor::BaseAddress* ret = new castor::BaseAddress();
      ret->setCnvSvcName("OraCnvSvc");
      ret->setCnvSvcType(castor::SVC_ORACNV);
      ret->setTarget(m_getNRStatement->getInt(1));
      return ret;
    }
  } catch (oracle::occi::SQLException e) {
    if (1403 == e.getErrorCode()) {
      //clog() << VERBOSE << "Found no requests." << std::endl;
      return 0;
    }
    if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
      // We've obviously lost the ORACLE connection here
      dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX fix it depending on ORACLE error
    ex.getMessage() << "Error in getting next request id."
                    << std::endl << e.what();
    throw ex;
  }
  //clog() << VERBOSE << "Found no new requests." << std::endl;
  return 0;
}

// -----------------------------------------------------------------------
// nbRequestsToProcess
// -----------------------------------------------------------------------
unsigned int castor::db::ora::OraCnvSvc::nbRequestsToProcess()
  throw (castor::exception::Exception) {
  oracle::occi::ResultSet *rset = 0;
  try {
    // Prepare statements
    if (0 == m_getNBRStatement) {
      m_getNBRStatement =
        getConnection()->createStatement(s_getNBRStatementString);
    }
    // Get the id
    rset = m_getNBRStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_getNBRStatement->closeResultSet(rset);
      return 0;
    } else {
      unsigned int res = rset->getInt(1);
      m_getNBRStatement->closeResultSet(rset);
      return res;
    }
  } catch (oracle::occi::SQLException e) {
    // let's try to close the result set
    try {
      if (0 != m_getNBRStatement && 0 != rset) {
        m_getNBRStatement->closeResultSet(rset);
      }
    } catch (oracle::occi::SQLException e2) {}
    try {
      getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        dropConnection();
      }
    } catch (oracle::occi::SQLException e2) {
      // rollback failed, let's drop the connection for security
      dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX fix it depending on ORACLE error
    ex.getMessage() << "Error in getting number of request to process."
                    << std::endl << e.what();
    throw ex;
  }
  // never reached
  return 0;
}

// -----------------------------------------------------------------------
// getTypeFromId
// -----------------------------------------------------------------------
const unsigned int
castor::db::ora::OraCnvSvc::getTypeFromId(const u_signed64 id)
  throw (castor::exception::Exception) {
  // a null id has a null type
  if (0 == id) return 0;
  oracle::occi::ResultSet *rset;
  try {
    // Check whether the statement is ok
    if (0 == m_getTypeStatement) {
      m_getTypeStatement = getConnection()->createStatement();
      m_getTypeStatement->setSQL(s_getTypeStatementString);
    }
    // Execute it
    m_getTypeStatement->setInt(1, id);
    rset = m_getTypeStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_getTypeStatement->closeResultSet(rset);
      castor::exception::NoEntry ex;
      ex.getMessage() << "No type found for id : " << id;
      throw ex;
    }
    const unsigned int res = rset->getInt(1);
    m_getTypeStatement->closeResultSet(rset);
    return res;
  } catch (oracle::occi::SQLException e) {
    try {
      m_getTypeStatement->closeResultSet(rset);
    } catch (oracle::occi::SQLException e2) {}
    try {
      getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        dropConnection();
      }
    } catch (oracle::occi::SQLException e2) {
      // rollback failed, let's drop the connection for security
      dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX fix it depending on ORACLE error
    ex.getMessage() << "Error in getting type from id."
                    << std::endl << e.what();
    throw ex;
  }
  // never reached
  return OBJ_INVALID;
}

// -----------------------------------------------------------------------
// getObjFromId
// -----------------------------------------------------------------------
castor::IObject* castor::db::ora::OraCnvSvc::getObjFromId
(u_signed64 id)
  throw (castor::exception::Exception) {
  castor::BaseAddress clientAd;
  clientAd.setTarget(id);
  clientAd.setCnvSvcName("OraCnvSvc");
  clientAd.setCnvSvcType(repType());
  return createObj(&clientAd);
}

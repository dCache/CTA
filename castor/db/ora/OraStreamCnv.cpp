/******************************************************************************
 *                      castor/db/ora/OraStreamCnv.cpp
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
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

// Include Files
#include "OraStreamCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/IAddress.hpp"
#include "castor/IConverter.hpp"
#include "castor/IFactory.hpp"
#include "castor/IObject.hpp"
#include "castor/db/DbAddress.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/Stream.hpp"
#include "castor/stager/StreamStatusCodes.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraStreamCnv> s_factoryOraStreamCnv;
const castor::IFactory<castor::IConverter>& OraStreamCnvFactory = 
  s_factoryOraStreamCnv;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraStreamCnv::s_insertStatementString =
"INSERT INTO rh_Stream (initialSizeToTransfer, id, status) VALUES (:1,:2,:3)";

/// SQL statement for request deletion
const std::string castor::db::ora::OraStreamCnv::s_deleteStatementString =
"DELETE FROM rh_Stream WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraStreamCnv::s_selectStatementString =
"SELECT initialSizeToTransfer, id, status FROM rh_Stream WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraStreamCnv::s_updateStatementString =
"UPDATE rh_Stream SET initialSizeToTransfer = :1, status = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::ora::OraStreamCnv::s_storeTypeStatementString =
"INSERT INTO rh_Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraStreamCnv::s_deleteTypeStatementString =
"DELETE FROM rh_Id2Type WHERE id = :1";

/// SQL insert statement for member status
const std::string castor::db::ora::OraStreamCnv::s_insertStream2StreamStatusCodesStatementString =
"INSERT INTO rh_Stream2StreamStatusCodes (Parent, Child) VALUES (:1, :2)";

/// SQL delete statement for member status
const std::string castor::db::ora::OraStreamCnv::s_deleteStream2StreamStatusCodesStatementString =
"DELETE FROM rh_Stream2StreamStatusCodes WHERE Parent = :1 AND Child = :2";

/// SQL select statement for member status
const std::string castor::db::ora::OraStreamCnv::s_Stream2StreamStatusCodesStatementString =
"SELECT Child from rh_Stream2StreamStatusCodes WHERE Parent = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraStreamCnv::OraStreamCnv() :
  OraBaseCnv(),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_insertStream2StreamStatusCodesStatement(0),
  m_deleteStream2StreamStatusCodesStatement(0),
  m_Stream2StreamStatusCodesStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraStreamCnv::~OraStreamCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraStreamCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_insertStream2StreamStatusCodesStatement);
    deleteStatement(m_Stream2StreamStatusCodesStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_insertStream2StreamStatusCodesStatement = 0;
  m_deleteStream2StreamStatusCodesStatement = 0;
  m_Stream2StreamStatusCodesStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraStreamCnv::ObjType() {
  return castor::stager::Stream::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraStreamCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraStreamCnv::fillRep(castor::IAddress* address,
                                            castor::IObject* object,
                                            unsigned int type,
                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
  switch (type) {
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillRep called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
  if (autocommit) {
    cnvSvc()->getConnection()->commit();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraStreamCnv::fillObj(castor::IAddress* address,
                                            castor::IObject* object,
                                            unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
  switch (type) {
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillObj called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraStreamCnv::createRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Get an id for the new object
    obj->setId(cnvSvc()->getIds(1));
    // Now Save the current object
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    m_insertStatement->setDouble(1, obj->initialSizeToTransfer());
    m_insertStatement->setDouble(2, obj->id());
    m_insertStatement->setDouble(3, (int)obj->status());
    m_insertStatement->executeUpdate();
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
                    << "  initialSizeToTransfer : " << obj->initialSizeToTransfer() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::ora::OraStreamCnv::updateRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setDouble(1, obj->initialSizeToTransfer());
    m_updateStatement->setDouble(2, (int)obj->status());
    m_updateStatement->setDouble(3, obj->id());
    m_updateStatement->executeUpdate();
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
void castor::db::ora::OraStreamCnv::deleteRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
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
castor::IObject* castor::db::ora::OraStreamCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::db::DbAddress* ad = 
    dynamic_cast<castor::db::DbAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
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
    castor::stager::Stream* object = new castor::stager::Stream();
    // Now retrieve and set members
    object->setInitialSizeToTransfer((unsigned long long)rset->getDouble(1));
    object->setId((unsigned long long)rset->getDouble(2));
    object->setStatus((enum castor::stager::StreamStatusCodes)rset->getInt(3));
    m_selectStatement->closeResultSet(rset);
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
void castor::db::ora::OraStreamCnv::updateObj(castor::IObject* obj)
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
    castor::stager::Stream* object = 
      dynamic_cast<castor::stager::Stream*>(obj);
    object->setInitialSizeToTransfer((unsigned long long)rset->getDouble(1));
    object->setId((unsigned long long)rset->getDouble(2));
    object->setStatus((enum castor::stager::StreamStatusCodes)rset->getInt(3));
    m_selectStatement->closeResultSet(rset);
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


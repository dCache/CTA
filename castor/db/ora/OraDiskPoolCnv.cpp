/******************************************************************************
 *                      castor/db/ora/OraDiskPoolCnv.cpp
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
#include "OraDiskPoolCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvFactory.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/DiskPool.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/stager/SvcClass.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraDiskPoolCnv> s_factoryOraDiskPoolCnv;
const castor::ICnvFactory& OraDiskPoolCnvFactory = 
  s_factoryOraDiskPoolCnv;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraDiskPoolCnv::s_insertStatementString =
"INSERT INTO DiskPool (name, id) VALUES (:1,:2)";

/// SQL statement for request deletion
const std::string castor::db::ora::OraDiskPoolCnv::s_deleteStatementString =
"DELETE FROM DiskPool WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraDiskPoolCnv::s_selectStatementString =
"SELECT name, id FROM DiskPool WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraDiskPoolCnv::s_updateStatementString =
"UPDATE DiskPool SET name = :1 WHERE id = :2";

/// SQL statement for type storage
const std::string castor::db::ora::OraDiskPoolCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraDiskPoolCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member fileSystems
const std::string castor::db::ora::OraDiskPoolCnv::s_selectFileSystemStatementString =
"SELECT id from FileSystem WHERE diskPool = :1";

/// SQL delete statement for member fileSystems
const std::string castor::db::ora::OraDiskPoolCnv::s_deleteFileSystemStatementString =
"UPDATE FileSystem SET diskPool = 0 WHERE id = :1";

/// SQL remote update statement for member fileSystems
const std::string castor::db::ora::OraDiskPoolCnv::s_remoteUpdateFileSystemStatementString =
"UPDATE FileSystem SET diskPool = :1 WHERE id = :2";

/// SQL insert statement for member svcClasses
const std::string castor::db::ora::OraDiskPoolCnv::s_insertSvcClassStatementString =
"INSERT INTO DiskPool2SvcClass (Parent, Child) VALUES (:1, :2)";

/// SQL delete statement for member svcClasses
const std::string castor::db::ora::OraDiskPoolCnv::s_deleteSvcClassStatementString =
"DELETE FROM DiskPool2SvcClass WHERE Parent = :1 AND Child = :2";

/// SQL select statement for member svcClasses
const std::string castor::db::ora::OraDiskPoolCnv::s_selectSvcClassStatementString =
"SELECT Child from DiskPool2SvcClass WHERE Parent = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraDiskPoolCnv::OraDiskPoolCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectFileSystemStatement(0),
  m_deleteFileSystemStatement(0),
  m_remoteUpdateFileSystemStatement(0),
  m_insertSvcClassStatement(0),
  m_deleteSvcClassStatement(0),
  m_selectSvcClassStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraDiskPoolCnv::~OraDiskPoolCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_deleteFileSystemStatement);
    deleteStatement(m_selectFileSystemStatement);
    deleteStatement(m_remoteUpdateFileSystemStatement);
    deleteStatement(m_insertSvcClassStatement);
    deleteStatement(m_deleteSvcClassStatement);
    deleteStatement(m_selectSvcClassStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectFileSystemStatement = 0;
  m_deleteFileSystemStatement = 0;
  m_remoteUpdateFileSystemStatement = 0;
  m_insertSvcClassStatement = 0;
  m_deleteSvcClassStatement = 0;
  m_selectSvcClassStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraDiskPoolCnv::ObjType() {
  return castor::stager::DiskPool::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraDiskPoolCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::fillRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              unsigned int type,
                                              bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::DiskPool* obj = 
    dynamic_cast<castor::stager::DiskPool*>(object);
  try {
    switch (type) {
    case castor::OBJ_FileSystem :
      fillRepFileSystem(obj);
      break;
    case castor::OBJ_SvcClass :
      fillRepSvcClass(obj);
      break;
    default :
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "fillRep called for type " << type 
                      << " on object of type " << obj->type() 
                      << ". This is meaningless.";
      throw ex;
    }
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepFileSystem
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::fillRepFileSystem(castor::stager::DiskPool* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // check select statement
  if (0 == m_selectFileSystemStatement) {
    m_selectFileSystemStatement = createStatement(s_selectFileSystemStatementString);
  }
  // Get current database data
  std::set<int> fileSystemsList;
  m_selectFileSystemStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectFileSystemStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    fileSystemsList.insert(rset->getInt(1));
  }
  m_selectFileSystemStatement->closeResultSet(rset);
  // update fileSystems and create new ones
  for (std::vector<castor::stager::FileSystem*>::iterator it = obj->fileSystems().begin();
       it != obj->fileSystems().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_DiskPool);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateFileSystemStatement) {
        m_remoteUpdateFileSystemStatement = createStatement(s_remoteUpdateFileSystemStatementString);
      }
      // Update remote object
      m_remoteUpdateFileSystemStatement->setDouble(1, obj->id());
      m_remoteUpdateFileSystemStatement->setDouble(2, (*it)->id());
      m_remoteUpdateFileSystemStatement->executeUpdate();
      std::set<int>::iterator item;
      if ((item = fileSystemsList.find((*it)->id())) != fileSystemsList.end()) {
        fileSystemsList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = fileSystemsList.begin();
       it != fileSystemsList.end();
       it++) {
    if (0 == m_deleteFileSystemStatement) {
      m_deleteFileSystemStatement = createStatement(s_deleteFileSystemStatementString);
    }
    m_deleteFileSystemStatement->setDouble(1, *it);
    m_deleteFileSystemStatement->executeUpdate();
  }
}

//------------------------------------------------------------------------------
// fillRepSvcClass
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::fillRepSvcClass(castor::stager::DiskPool* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // check select statement
  if (0 == m_selectSvcClassStatement) {
    m_selectSvcClassStatement = createStatement(s_selectSvcClassStatementString);
  }
  // Get current database data
  std::set<int> svcClassesList;
  m_selectSvcClassStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectSvcClassStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    svcClassesList.insert(rset->getInt(1));
  }
  m_selectSvcClassStatement->closeResultSet(rset);
  // update svcClasses and create new ones
  for (std::vector<castor::stager::SvcClass*>::iterator it = obj->svcClasses().begin();
       it != obj->svcClasses().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false);
    }
    std::set<int>::iterator item;
    if ((item = svcClassesList.find((*it)->id())) != svcClassesList.end()) {
      svcClassesList.erase(item);
    } else {
      if (0 == m_insertSvcClassStatement) {
        m_insertSvcClassStatement = createStatement(s_insertSvcClassStatementString);
      }
      m_insertSvcClassStatement->setDouble(1, obj->id());
      m_insertSvcClassStatement->setDouble(2, (*it)->id());
      m_insertSvcClassStatement->executeUpdate();
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = svcClassesList.begin();
       it != svcClassesList.end();
       it++) {
    if (0 == m_deleteSvcClassStatement) {
      m_deleteSvcClassStatement = createStatement(s_deleteSvcClassStatementString);
    }
    m_deleteSvcClassStatement->setDouble(1, obj->id());
    m_deleteSvcClassStatement->setDouble(2, *it);
    m_deleteSvcClassStatement->executeUpdate();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::fillObj(castor::IAddress* address,
                                              castor::IObject* object,
                                              unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::DiskPool* obj = 
    dynamic_cast<castor::stager::DiskPool*>(object);
  switch (type) {
  case castor::OBJ_FileSystem :
    fillObjFileSystem(obj);
    break;
  case castor::OBJ_SvcClass :
    fillObjSvcClass(obj);
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
// fillObjFileSystem
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::fillObjFileSystem(castor::stager::DiskPool* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectFileSystemStatement) {
    m_selectFileSystemStatement = createStatement(s_selectFileSystemStatementString);
  }
  // retrieve the object from the database
  std::set<int> fileSystemsList;
  m_selectFileSystemStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectFileSystemStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    fileSystemsList.insert(rset->getInt(1));
  }
  // Close ResultSet
  m_selectFileSystemStatement->closeResultSet(rset);
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::FileSystem*> toBeDeleted;
  for (std::vector<castor::stager::FileSystem*>::iterator it = obj->fileSystems().begin();
       it != obj->fileSystems().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = fileSystemsList.find((*it)->id())) == fileSystemsList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      fileSystemsList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::FileSystem*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeFileSystems(*it);
    (*it)->setDiskPool(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = fileSystemsList.begin();
       it != fileSystemsList.end();
       it++) {
    IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::FileSystem* remoteObj = 
      dynamic_cast<castor::stager::FileSystem*>(item);
    obj->addFileSystems(remoteObj);
    remoteObj->setDiskPool(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjSvcClass
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::fillObjSvcClass(castor::stager::DiskPool* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectSvcClassStatement) {
    m_selectSvcClassStatement = createStatement(s_selectSvcClassStatementString);
  }
  // retrieve the object from the database
  std::set<int> svcClassesList;
  m_selectSvcClassStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectSvcClassStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    svcClassesList.insert(rset->getInt(1));
  }
  // Close ResultSet
  m_selectSvcClassStatement->closeResultSet(rset);
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::SvcClass*> toBeDeleted;
  for (std::vector<castor::stager::SvcClass*>::iterator it = obj->svcClasses().begin();
       it != obj->svcClasses().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = svcClassesList.find((*it)->id())) == svcClassesList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      svcClassesList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::SvcClass*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeSvcClasses(*it);
    (*it)->removeDiskPools(obj);
  }
  // Create new objects
  for (std::set<int>::iterator it = svcClassesList.begin();
       it != svcClassesList.end();
       it++) {
    IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::SvcClass* remoteObj = 
      dynamic_cast<castor::stager::SvcClass*>(item);
    obj->addSvcClasses(remoteObj);
    remoteObj->addDiskPools(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::createRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                bool autocommit,
                                                unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::DiskPool* obj = 
    dynamic_cast<castor::stager::DiskPool*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
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
    m_insertStatement->setString(1, obj->name());
    m_insertStatement->setDouble(2, obj->id());
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
                    << "  name : " << obj->name() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskPoolCnv::updateRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::DiskPool* obj = 
    dynamic_cast<castor::stager::DiskPool*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->name());
    m_updateStatement->setDouble(2, obj->id());
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
void castor::db::ora::OraDiskPoolCnv::deleteRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::DiskPool* obj = 
    dynamic_cast<castor::stager::DiskPool*>(object);
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
castor::IObject* castor::db::ora::OraDiskPoolCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::BaseAddress* ad = 
    dynamic_cast<castor::BaseAddress*>(address);
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
    castor::stager::DiskPool* object = new castor::stager::DiskPool();
    // Now retrieve and set members
    object->setName(rset->getString(1));
    object->setId((u_signed64)rset->getDouble(2));
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
void castor::db::ora::OraDiskPoolCnv::updateObj(castor::IObject* obj)
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
    castor::stager::DiskPool* object = 
      dynamic_cast<castor::stager::DiskPool*>(obj);
    object->setName(rset->getString(1));
    object->setId((u_signed64)rset->getDouble(2));
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


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbSvcClassCnv.cpp
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
#include "DbSvcClassCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/DiskPool.hpp"
#include "castor/stager/SvcClass.hpp"
#include "castor/stager/TapePool.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbSvcClassCnv>* s_factoryDbSvcClassCnv =
  new castor::CnvFactory<castor::db::cnv::DbSvcClassCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbSvcClassCnv::s_insertStatementString =
"INSERT INTO SvcClass (nbDrives, name, defaultFileSize, maxReplicaNb, replicationPolicy, gcPolicy, migratorPolicy, recallerPolicy, id) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,ids_seq.nextval) RETURNING id INTO :9";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbSvcClassCnv::s_deleteStatementString =
"DELETE FROM SvcClass WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbSvcClassCnv::s_selectStatementString =
"SELECT nbDrives, name, defaultFileSize, maxReplicaNb, replicationPolicy, gcPolicy, migratorPolicy, recallerPolicy, id FROM SvcClass WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbSvcClassCnv::s_updateStatementString =
"UPDATE SvcClass SET nbDrives = :1, name = :2, defaultFileSize = :3, maxReplicaNb = :4, replicationPolicy = :5, gcPolicy = :6, migratorPolicy = :7, recallerPolicy = :8 WHERE id = :9";

/// SQL statement for type storage
const std::string castor::db::cnv::DbSvcClassCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbSvcClassCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL insert statement for member tapePools
const std::string castor::db::cnv::DbSvcClassCnv::s_insertTapePoolStatementString =
"INSERT INTO SvcClass2TapePool (Parent, Child) VALUES (:1, :2)";

/// SQL delete statement for member tapePools
const std::string castor::db::cnv::DbSvcClassCnv::s_deleteTapePoolStatementString =
"DELETE FROM SvcClass2TapePool WHERE Parent = :1 AND Child = :2";

/// SQL select statement for member tapePools
// The FOR UPDATE is needed in order to avoid deletion
// of a segment after listing and before update/remove
const std::string castor::db::cnv::DbSvcClassCnv::s_selectTapePoolStatementString =
"SELECT Child FROM SvcClass2TapePool WHERE Parent = :1 FOR UPDATE";

/// SQL insert statement for member diskPools
const std::string castor::db::cnv::DbSvcClassCnv::s_insertDiskPoolStatementString =
"INSERT INTO DiskPool2SvcClass (Child, Parent) VALUES (:1, :2)";

/// SQL delete statement for member diskPools
const std::string castor::db::cnv::DbSvcClassCnv::s_deleteDiskPoolStatementString =
"DELETE FROM DiskPool2SvcClass WHERE Child = :1 AND Parent = :2";

/// SQL select statement for member diskPools
// The FOR UPDATE is needed in order to avoid deletion
// of a segment after listing and before update/remove
const std::string castor::db::cnv::DbSvcClassCnv::s_selectDiskPoolStatementString =
"SELECT Parent FROM DiskPool2SvcClass WHERE Child = :1 FOR UPDATE";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbSvcClassCnv::DbSvcClassCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_insertTapePoolStatement(0),
  m_deleteTapePoolStatement(0),
  m_selectTapePoolStatement(0),
  m_insertDiskPoolStatement(0),
  m_deleteDiskPoolStatement(0),
  m_selectDiskPoolStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbSvcClassCnv::~DbSvcClassCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    delete m_insertStatement;
    delete m_deleteStatement;
    delete m_selectStatement;
    delete m_updateStatement;
    delete m_storeTypeStatement;
    delete m_deleteTypeStatement;
    delete m_insertTapePoolStatement;
    delete m_deleteTapePoolStatement;
    delete m_selectTapePoolStatement;
    delete m_insertDiskPoolStatement;
    delete m_deleteDiskPoolStatement;
    delete m_selectDiskPoolStatement;
  } catch (castor::exception::SQLError ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_insertTapePoolStatement = 0;
  m_deleteTapePoolStatement = 0;
  m_selectTapePoolStatement = 0;
  m_insertDiskPoolStatement = 0;
  m_deleteDiskPoolStatement = 0;
  m_selectDiskPoolStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbSvcClassCnv::ObjType() {
  return castor::stager::SvcClass::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbSvcClassCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::fillRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::SvcClass* obj = 
    dynamic_cast<castor::stager::SvcClass*>(object);
  try {
    switch (type) {
    case castor::OBJ_TapePool :
      fillRepTapePool(obj);
      break;
    case castor::OBJ_DiskPool :
      fillRepDiskPool(obj);
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
// fillRepTapePool
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::fillRepTapePool(castor::stager::SvcClass* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectTapePoolStatement) {
    m_selectTapePoolStatement = createStatement(s_selectTapePoolStatementString);
  }
  // Get current database data
  std::set<int> tapePoolsList;
  m_selectTapePoolStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapePoolStatement->executeQuery();
  while (rset->next()) {
    tapePoolsList.insert(rset->getInt(1));
  }
  delete rset;
  // update tapePools and create new ones
  for (std::vector<castor::stager::TapePool*>::iterator it = obj->tapePools().begin();
       it != obj->tapePools().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false);
    }
    std::set<int>::iterator item;
    if ((item = tapePoolsList.find((*it)->id())) != tapePoolsList.end()) {
      tapePoolsList.erase(item);
    } else {
      if (0 == m_insertTapePoolStatement) {
        m_insertTapePoolStatement = createStatement(s_insertTapePoolStatementString);
      }
      m_insertTapePoolStatement->setInt64(1, obj->id());
      m_insertTapePoolStatement->setInt64(2, (*it)->id());
      m_insertTapePoolStatement->execute();
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = tapePoolsList.begin();
       it != tapePoolsList.end();
       it++) {
    if (0 == m_deleteTapePoolStatement) {
      m_deleteTapePoolStatement = createStatement(s_deleteTapePoolStatementString);
    }
    m_deleteTapePoolStatement->setInt64(1, obj->id());
    m_deleteTapePoolStatement->setInt64(2, *it);
    m_deleteTapePoolStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepDiskPool
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::fillRepDiskPool(castor::stager::SvcClass* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectDiskPoolStatement) {
    m_selectDiskPoolStatement = createStatement(s_selectDiskPoolStatementString);
  }
  // Get current database data
  std::set<int> diskPoolsList;
  m_selectDiskPoolStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectDiskPoolStatement->executeQuery();
  while (rset->next()) {
    diskPoolsList.insert(rset->getInt(1));
  }
  delete rset;
  // update diskPools and create new ones
  for (std::vector<castor::stager::DiskPool*>::iterator it = obj->diskPools().begin();
       it != obj->diskPools().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false);
    }
    std::set<int>::iterator item;
    if ((item = diskPoolsList.find((*it)->id())) != diskPoolsList.end()) {
      diskPoolsList.erase(item);
    } else {
      if (0 == m_insertDiskPoolStatement) {
        m_insertDiskPoolStatement = createStatement(s_insertDiskPoolStatementString);
      }
      m_insertDiskPoolStatement->setInt64(1, obj->id());
      m_insertDiskPoolStatement->setInt64(2, (*it)->id());
      m_insertDiskPoolStatement->execute();
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = diskPoolsList.begin();
       it != diskPoolsList.end();
       it++) {
    if (0 == m_deleteDiskPoolStatement) {
      m_deleteDiskPoolStatement = createStatement(s_deleteDiskPoolStatementString);
    }
    m_deleteDiskPoolStatement->setInt64(1, obj->id());
    m_deleteDiskPoolStatement->setInt64(2, *it);
    m_deleteDiskPoolStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::fillObj(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::SvcClass* obj = 
    dynamic_cast<castor::stager::SvcClass*>(object);
  switch (type) {
  case castor::OBJ_TapePool :
    fillObjTapePool(obj);
    break;
  case castor::OBJ_DiskPool :
    fillObjDiskPool(obj);
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
// fillObjTapePool
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::fillObjTapePool(castor::stager::SvcClass* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectTapePoolStatement) {
    m_selectTapePoolStatement = createStatement(s_selectTapePoolStatementString);
  }
  // retrieve the object from the database
  std::set<int> tapePoolsList;
  m_selectTapePoolStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapePoolStatement->executeQuery();
  while (rset->next()) {
    tapePoolsList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::TapePool*> toBeDeleted;
  for (std::vector<castor::stager::TapePool*>::iterator it = obj->tapePools().begin();
       it != obj->tapePools().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = tapePoolsList.find((*it)->id())) == tapePoolsList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      tapePoolsList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::TapePool*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeTapePools(*it);
    (*it)->removeSvcClasses(obj);
  }
  // Create new objects
  for (std::set<int>::iterator it = tapePoolsList.begin();
       it != tapePoolsList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::TapePool* remoteObj = 
      dynamic_cast<castor::stager::TapePool*>(item);
    obj->addTapePools(remoteObj);
    remoteObj->addSvcClasses(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjDiskPool
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::fillObjDiskPool(castor::stager::SvcClass* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectDiskPoolStatement) {
    m_selectDiskPoolStatement = createStatement(s_selectDiskPoolStatementString);
  }
  // retrieve the object from the database
  std::set<int> diskPoolsList;
  m_selectDiskPoolStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectDiskPoolStatement->executeQuery();
  while (rset->next()) {
    diskPoolsList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::DiskPool*> toBeDeleted;
  for (std::vector<castor::stager::DiskPool*>::iterator it = obj->diskPools().begin();
       it != obj->diskPools().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = diskPoolsList.find((*it)->id())) == diskPoolsList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      diskPoolsList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::DiskPool*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeDiskPools(*it);
    (*it)->removeSvcClasses(obj);
  }
  // Create new objects
  for (std::set<int>::iterator it = diskPoolsList.begin();
       it != diskPoolsList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::DiskPool* remoteObj = 
      dynamic_cast<castor::stager::DiskPool*>(item);
    obj->addDiskPools(remoteObj);
    remoteObj->addSvcClasses(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit,
                                               unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::SvcClass* obj = 
    dynamic_cast<castor::stager::SvcClass*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(9, castor::db::DBTYPE_INT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->nbDrives());
    m_insertStatement->setString(2, obj->name());
    m_insertStatement->setInt64(3, obj->defaultFileSize());
    m_insertStatement->setInt(4, obj->maxReplicaNb());
    m_insertStatement->setString(5, obj->replicationPolicy());
    m_insertStatement->setString(6, obj->gcPolicy());
    m_insertStatement->setString(7, obj->migratorPolicy());
    m_insertStatement->setString(8, obj->recallerPolicy());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getInt64(9));
    m_storeTypeStatement->setInt64(1, obj->id());
    m_storeTypeStatement->setInt64(2, obj->type());
    m_storeTypeStatement->execute();
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
                    << "  nbDrives : " << obj->nbDrives() << std::endl
                    << "  name : " << obj->name() << std::endl
                    << "  defaultFileSize : " << obj->defaultFileSize() << std::endl
                    << "  maxReplicaNb : " << obj->maxReplicaNb() << std::endl
                    << "  replicationPolicy : " << obj->replicationPolicy() << std::endl
                    << "  gcPolicy : " << obj->gcPolicy() << std::endl
                    << "  migratorPolicy : " << obj->migratorPolicy() << std::endl
                    << "  recallerPolicy : " << obj->recallerPolicy() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbSvcClassCnv::updateRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::SvcClass* obj = 
    dynamic_cast<castor::stager::SvcClass*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->nbDrives());
    m_updateStatement->setString(2, obj->name());
    m_updateStatement->setInt64(3, obj->defaultFileSize());
    m_updateStatement->setInt(4, obj->maxReplicaNb());
    m_updateStatement->setString(5, obj->replicationPolicy());
    m_updateStatement->setString(6, obj->gcPolicy());
    m_updateStatement->setString(7, obj->migratorPolicy());
    m_updateStatement->setString(8, obj->recallerPolicy());
    m_updateStatement->setInt64(9, obj->id());
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
void castor::db::cnv::DbSvcClassCnv::deleteRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::SvcClass* obj = 
    dynamic_cast<castor::stager::SvcClass*>(object);
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
castor::IObject* castor::db::cnv::DbSvcClassCnv::createObj(castor::IAddress* address)
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
    castor::stager::SvcClass* object = new castor::stager::SvcClass();
    // Now retrieve and set members
    object->setNbDrives(rset->getInt(1));
    object->setName(rset->getString(2));
    object->setDefaultFileSize(rset->getInt64(3));
    object->setMaxReplicaNb(rset->getInt(4));
    object->setReplicationPolicy(rset->getString(5));
    object->setGcPolicy(rset->getString(6));
    object->setMigratorPolicy(rset->getString(7));
    object->setRecallerPolicy(rset->getString(8));
    object->setId(rset->getInt64(9));
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
void castor::db::cnv::DbSvcClassCnv::updateObj(castor::IObject* obj)
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
    castor::stager::SvcClass* object = 
      dynamic_cast<castor::stager::SvcClass*>(obj);
    object->setNbDrives(rset->getInt(1));
    object->setName(rset->getString(2));
    object->setDefaultFileSize(rset->getInt64(3));
    object->setMaxReplicaNb(rset->getInt(4));
    object->setReplicationPolicy(rset->getString(5));
    object->setGcPolicy(rset->getString(6));
    object->setMigratorPolicy(rset->getString(7));
    object->setRecallerPolicy(rset->getString(8));
    object->setId(rset->getInt64(9));
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


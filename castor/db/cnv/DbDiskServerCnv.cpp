/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbDiskServerCnv.cpp
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
#include "DbDiskServerCnv.hpp"
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
#include "castor/monitoring/AdminStatusCodes.hpp"
#include "castor/stager/DiskServer.hpp"
#include "castor/stager/DiskServerStatusCode.hpp"
#include "castor/stager/FileSystem.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbDiskServerCnv>* s_factoryDbDiskServerCnv =
  new castor::CnvFactory<castor::db::cnv::DbDiskServerCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbDiskServerCnv::s_insertStatementString =
"INSERT INTO DiskServer (name, readRate, writeRate, nbReadStreams, nbWriteStreams, nbReadWriteStreams, nbMigratorStreams, nbRecallerStreams, id, status, adminStatus) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,ids_seq.nextval,:9,:10) RETURNING id INTO :11";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbDiskServerCnv::s_deleteStatementString =
"DELETE FROM DiskServer WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbDiskServerCnv::s_selectStatementString =
"SELECT name, readRate, writeRate, nbReadStreams, nbWriteStreams, nbReadWriteStreams, nbMigratorStreams, nbRecallerStreams, id, status, adminStatus FROM DiskServer WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbDiskServerCnv::s_updateStatementString =
"UPDATE DiskServer SET name = :1, readRate = :2, writeRate = :3, nbReadStreams = :4, nbWriteStreams = :5, nbReadWriteStreams = :6, nbMigratorStreams = :7, nbRecallerStreams = :8, status = :9, adminStatus = :10 WHERE id = :11";

/// SQL statement for type storage
const std::string castor::db::cnv::DbDiskServerCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbDiskServerCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member fileSystems
const std::string castor::db::cnv::DbDiskServerCnv::s_selectFileSystemStatementString =
"SELECT id FROM FileSystem WHERE diskserver = :1 FOR UPDATE";

/// SQL delete statement for member fileSystems
const std::string castor::db::cnv::DbDiskServerCnv::s_deleteFileSystemStatementString =
"UPDATE FileSystem SET diskserver = 0 WHERE id = :1";

/// SQL remote update statement for member fileSystems
const std::string castor::db::cnv::DbDiskServerCnv::s_remoteUpdateFileSystemStatementString =
"UPDATE FileSystem SET diskserver = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbDiskServerCnv::DbDiskServerCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectFileSystemStatement(0),
  m_deleteFileSystemStatement(0),
  m_remoteUpdateFileSystemStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbDiskServerCnv::~DbDiskServerCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskServerCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_deleteFileSystemStatement) delete m_deleteFileSystemStatement;
    if(m_selectFileSystemStatement) delete m_selectFileSystemStatement;
    if(m_remoteUpdateFileSystemStatement) delete m_remoteUpdateFileSystemStatement;
  } catch (castor::exception::Exception ignored) {};
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
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbDiskServerCnv::ObjType() {
  return castor::stager::DiskServer::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbDiskServerCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskServerCnv::fillRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::DiskServer* obj = 
    dynamic_cast<castor::stager::DiskServer*>(object);
  try {
    switch (type) {
    case castor::OBJ_FileSystem :
      fillRepFileSystem(obj);
      break;
    default :
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "fillRep called for type " << type 
                      << " on object of type " << obj->type() 
                      << ". This is meaningless.";
      throw ex;
    }
    if (endTransaction) {
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
// fillRepFileSystem
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskServerCnv::fillRepFileSystem(castor::stager::DiskServer* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectFileSystemStatement) {
    m_selectFileSystemStatement = createStatement(s_selectFileSystemStatementString);
  }
  // Get current database data
  std::set<int> fileSystemsList;
  m_selectFileSystemStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectFileSystemStatement->executeQuery();
  while (rset->next()) {
    fileSystemsList.insert(rset->getInt(1));
  }
  delete rset;
  // update fileSystems and create new ones
  for (std::vector<castor::stager::FileSystem*>::iterator it = obj->fileSystems().begin();
       it != obj->fileSystems().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_DiskServer);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateFileSystemStatement) {
        m_remoteUpdateFileSystemStatement = createStatement(s_remoteUpdateFileSystemStatementString);
      }
      // Update remote object
      m_remoteUpdateFileSystemStatement->setUInt64(1, obj->id());
      m_remoteUpdateFileSystemStatement->setUInt64(2, (*it)->id());
      m_remoteUpdateFileSystemStatement->execute();
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
    m_deleteFileSystemStatement->setUInt64(1, *it);
    m_deleteFileSystemStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskServerCnv::fillObj(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::DiskServer* obj = 
    dynamic_cast<castor::stager::DiskServer*>(object);
  switch (type) {
  case castor::OBJ_FileSystem :
    fillObjFileSystem(obj);
    break;
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillObj called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
  if (endTransaction) {
    cnvSvc()->commit();
  }
}
//------------------------------------------------------------------------------
// fillObjFileSystem
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskServerCnv::fillObjFileSystem(castor::stager::DiskServer* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectFileSystemStatement) {
    m_selectFileSystemStatement = createStatement(s_selectFileSystemStatementString);
  }
  // retrieve the object from the database
  std::set<int> fileSystemsList;
  m_selectFileSystemStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectFileSystemStatement->executeQuery();
  while (rset->next()) {
    fileSystemsList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
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
    (*it)->setDiskserver(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = fileSystemsList.begin();
       it != fileSystemsList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::FileSystem* remoteObj = 
      dynamic_cast<castor::stager::FileSystem*>(item);
    obj->addFileSystems(remoteObj);
    remoteObj->setDiskserver(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskServerCnv::createRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool endTransaction,
                                                 unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::DiskServer* obj = 
    dynamic_cast<castor::stager::DiskServer*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(11, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->name());
    m_insertStatement->setUInt64(2, obj->readRate());
    m_insertStatement->setUInt64(3, obj->writeRate());
    m_insertStatement->setInt(4, obj->nbReadStreams());
    m_insertStatement->setInt(5, obj->nbWriteStreams());
    m_insertStatement->setInt(6, obj->nbReadWriteStreams());
    m_insertStatement->setInt(7, obj->nbMigratorStreams());
    m_insertStatement->setInt(8, obj->nbRecallerStreams());
    m_insertStatement->setInt(9, (int)obj->status());
    m_insertStatement->setInt(10, (int)obj->adminStatus());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(11));
    m_storeTypeStatement->setUInt64(1, obj->id());
    m_storeTypeStatement->setUInt64(2, obj->type());
    m_storeTypeStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { if (endTransaction) cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  name : " << obj->name() << std::endl
                    << "  readRate : " << obj->readRate() << std::endl
                    << "  writeRate : " << obj->writeRate() << std::endl
                    << "  nbReadStreams : " << obj->nbReadStreams() << std::endl
                    << "  nbWriteStreams : " << obj->nbWriteStreams() << std::endl
                    << "  nbReadWriteStreams : " << obj->nbReadWriteStreams() << std::endl
                    << "  nbMigratorStreams : " << obj->nbMigratorStreams() << std::endl
                    << "  nbRecallerStreams : " << obj->nbRecallerStreams() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  status : " << obj->status() << std::endl
                    << "  adminStatus : " << obj->adminStatus() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskServerCnv::updateRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::DiskServer* obj = 
    dynamic_cast<castor::stager::DiskServer*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->name());
    m_updateStatement->setUInt64(2, obj->readRate());
    m_updateStatement->setUInt64(3, obj->writeRate());
    m_updateStatement->setInt(4, obj->nbReadStreams());
    m_updateStatement->setInt(5, obj->nbWriteStreams());
    m_updateStatement->setInt(6, obj->nbReadWriteStreams());
    m_updateStatement->setInt(7, obj->nbMigratorStreams());
    m_updateStatement->setInt(8, obj->nbRecallerStreams());
    m_updateStatement->setInt(9, (int)obj->status());
    m_updateStatement->setInt(10, (int)obj->adminStatus());
    m_updateStatement->setUInt64(11, obj->id());
    m_updateStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { if (endTransaction) cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
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
void castor::db::cnv::DbDiskServerCnv::deleteRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::DiskServer* obj = 
    dynamic_cast<castor::stager::DiskServer*>(object);
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
    m_deleteTypeStatement->setUInt64(1, obj->id());
    m_deleteTypeStatement->execute();
    m_deleteStatement->setUInt64(1, obj->id());
    m_deleteStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { if (endTransaction) cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
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
castor::IObject* castor::db::cnv::DbDiskServerCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::BaseAddress* ad = 
    dynamic_cast<castor::BaseAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setUInt64(1, ad->target());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << ad->target();
      throw ex;
    }
    // create the new Object
    castor::stager::DiskServer* object = new castor::stager::DiskServer();
    // Now retrieve and set members
    object->setName(rset->getString(1));
    object->setReadRate(rset->getUInt64(2));
    object->setWriteRate(rset->getUInt64(3));
    object->setNbReadStreams(rset->getInt(4));
    object->setNbWriteStreams(rset->getInt(5));
    object->setNbReadWriteStreams(rset->getInt(6));
    object->setNbMigratorStreams(rset->getInt(7));
    object->setNbRecallerStreams(rset->getInt(8));
    object->setId(rset->getUInt64(9));
    object->setStatus((enum castor::stager::DiskServerStatusCode)rset->getInt(10));
    object->setAdminStatus((enum castor::monitoring::AdminStatusCodes)rset->getInt(11));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
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
void castor::db::cnv::DbDiskServerCnv::updateObj(castor::IObject* obj)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setUInt64(1, obj->id());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << obj->id();
      throw ex;
    }
    // Now retrieve and set members
    castor::stager::DiskServer* object = 
      dynamic_cast<castor::stager::DiskServer*>(obj);
    object->setName(rset->getString(1));
    object->setReadRate(rset->getUInt64(2));
    object->setWriteRate(rset->getUInt64(3));
    object->setNbReadStreams(rset->getInt(4));
    object->setNbWriteStreams(rset->getInt(5));
    object->setNbReadWriteStreams(rset->getInt(6));
    object->setNbMigratorStreams(rset->getInt(7));
    object->setNbRecallerStreams(rset->getInt(8));
    object->setId(rset->getUInt64(9));
    object->setStatus((enum castor::stager::DiskServerStatusCode)rset->getInt(10));
    object->setAdminStatus((enum castor::monitoring::AdminStatusCodes)rset->getInt(11));
    delete rset;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


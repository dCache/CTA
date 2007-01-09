/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbDiskCopyCnv.cpp
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
#include "DbDiskCopyCnv.hpp"
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
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/DiskCopyStatusCodes.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/stager/SubRequest.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbDiskCopyCnv>* s_factoryDbDiskCopyCnv =
  new castor::CnvFactory<castor::db::cnv::DbDiskCopyCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbDiskCopyCnv::s_insertStatementString =
"INSERT INTO DiskCopy (path, gcWeight, creationTime, id, fileSystem, castorFile, status) VALUES (:1,:2,:3,ids_seq.nextval,:4,:5,:6) RETURNING id INTO :7";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbDiskCopyCnv::s_deleteStatementString =
"DELETE FROM DiskCopy WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbDiskCopyCnv::s_selectStatementString =
"SELECT path, gcWeight, creationTime, id, fileSystem, castorFile, status FROM DiskCopy WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbDiskCopyCnv::s_updateStatementString =
"UPDATE DiskCopy SET path = :1, gcWeight = :2, status = :3 WHERE id = :4";

/// SQL statement for type storage
const std::string castor::db::cnv::DbDiskCopyCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbDiskCopyCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member subRequests
const std::string castor::db::cnv::DbDiskCopyCnv::s_selectSubRequestStatementString =
"SELECT id FROM SubRequest WHERE diskcopy = :1 FOR UPDATE";

/// SQL delete statement for member subRequests
const std::string castor::db::cnv::DbDiskCopyCnv::s_deleteSubRequestStatementString =
"UPDATE SubRequest SET diskcopy = 0 WHERE id = :1";

/// SQL remote update statement for member subRequests
const std::string castor::db::cnv::DbDiskCopyCnv::s_remoteUpdateSubRequestStatementString =
"UPDATE SubRequest SET diskcopy = :1 WHERE id = :2";

/// SQL existence statement for member fileSystem
const std::string castor::db::cnv::DbDiskCopyCnv::s_checkFileSystemExistStatementString =
"SELECT id FROM FileSystem WHERE id = :1";

/// SQL update statement for member fileSystem
const std::string castor::db::cnv::DbDiskCopyCnv::s_updateFileSystemStatementString =
"UPDATE DiskCopy SET fileSystem = :1 WHERE id = :2";

/// SQL existence statement for member castorFile
const std::string castor::db::cnv::DbDiskCopyCnv::s_checkCastorFileExistStatementString =
"SELECT id FROM CastorFile WHERE id = :1";

/// SQL update statement for member castorFile
const std::string castor::db::cnv::DbDiskCopyCnv::s_updateCastorFileStatementString =
"UPDATE DiskCopy SET castorFile = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbDiskCopyCnv::DbDiskCopyCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectSubRequestStatement(0),
  m_deleteSubRequestStatement(0),
  m_remoteUpdateSubRequestStatement(0),
  m_checkFileSystemExistStatement(0),
  m_updateFileSystemStatement(0),
  m_checkCastorFileExistStatement(0),
  m_updateCastorFileStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbDiskCopyCnv::~DbDiskCopyCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    delete m_insertStatement;
    delete m_deleteStatement;
    delete m_selectStatement;
    delete m_updateStatement;
    delete m_storeTypeStatement;
    delete m_deleteTypeStatement;
    delete m_deleteSubRequestStatement;
    delete m_selectSubRequestStatement;
    delete m_remoteUpdateSubRequestStatement;
    delete m_checkFileSystemExistStatement;
    delete m_updateFileSystemStatement;
    delete m_checkCastorFileExistStatement;
    delete m_updateCastorFileStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectSubRequestStatement = 0;
  m_deleteSubRequestStatement = 0;
  m_remoteUpdateSubRequestStatement = 0;
  m_checkFileSystemExistStatement = 0;
  m_updateFileSystemStatement = 0;
  m_checkCastorFileExistStatement = 0;
  m_updateCastorFileStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbDiskCopyCnv::ObjType() {
  return castor::stager::DiskCopy::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbDiskCopyCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
  try {
    switch (type) {
    case castor::OBJ_SubRequest :
      fillRepSubRequest(obj);
      break;
    case castor::OBJ_FileSystem :
      fillRepFileSystem(obj);
      break;
    case castor::OBJ_CastorFile :
      fillRepCastorFile(obj);
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
// fillRepSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillRepSubRequest(castor::stager::DiskCopy* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectSubRequestStatement) {
    m_selectSubRequestStatement = createStatement(s_selectSubRequestStatementString);
  }
  // Get current database data
  std::set<int> subRequestsList;
  m_selectSubRequestStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectSubRequestStatement->executeQuery();
  while (rset->next()) {
    subRequestsList.insert(rset->getInt(1));
  }
  delete rset;
  // update subRequests and create new ones
  for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
       it != obj->subRequests().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_DiskCopy);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateSubRequestStatement) {
        m_remoteUpdateSubRequestStatement = createStatement(s_remoteUpdateSubRequestStatementString);
      }
      // Update remote object
      m_remoteUpdateSubRequestStatement->setInt64(1, obj->id());
      m_remoteUpdateSubRequestStatement->setInt64(2, (*it)->id());
      m_remoteUpdateSubRequestStatement->execute();
      std::set<int>::iterator item;
      if ((item = subRequestsList.find((*it)->id())) != subRequestsList.end()) {
        subRequestsList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = subRequestsList.begin();
       it != subRequestsList.end();
       it++) {
    if (0 == m_deleteSubRequestStatement) {
      m_deleteSubRequestStatement = createStatement(s_deleteSubRequestStatementString);
    }
    m_deleteSubRequestStatement->setInt64(1, *it);
    m_deleteSubRequestStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepFileSystem
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillRepFileSystem(castor::stager::DiskCopy* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->fileSystem()) {
    // Check checkFileSystemExist statement
    if (0 == m_checkFileSystemExistStatement) {
      m_checkFileSystemExistStatement = createStatement(s_checkFileSystemExistStatementString);
    }
    // retrieve the object from the database
    m_checkFileSystemExistStatement->setInt64(1, obj->fileSystem()->id());
    castor::db::IDbResultSet *rset = m_checkFileSystemExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->fileSystem(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateFileSystemStatement) {
    m_updateFileSystemStatement = createStatement(s_updateFileSystemStatementString);
  }
  // Update local object
  m_updateFileSystemStatement->setInt64(1, 0 == obj->fileSystem() ? 0 : obj->fileSystem()->id());
  m_updateFileSystemStatement->setInt64(2, obj->id());
  m_updateFileSystemStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepCastorFile
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillRepCastorFile(castor::stager::DiskCopy* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->castorFile()) {
    // Check checkCastorFileExist statement
    if (0 == m_checkCastorFileExistStatement) {
      m_checkCastorFileExistStatement = createStatement(s_checkCastorFileExistStatementString);
    }
    // retrieve the object from the database
    m_checkCastorFileExistStatement->setInt64(1, obj->castorFile()->id());
    castor::db::IDbResultSet *rset = m_checkCastorFileExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->castorFile(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateCastorFileStatement) {
    m_updateCastorFileStatement = createStatement(s_updateCastorFileStatementString);
  }
  // Update local object
  m_updateCastorFileStatement->setInt64(1, 0 == obj->castorFile() ? 0 : obj->castorFile()->id());
  m_updateCastorFileStatement->setInt64(2, obj->id());
  m_updateCastorFileStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillObj(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
  switch (type) {
  case castor::OBJ_SubRequest :
    fillObjSubRequest(obj);
    break;
  case castor::OBJ_FileSystem :
    fillObjFileSystem(obj);
    break;
  case castor::OBJ_CastorFile :
    fillObjCastorFile(obj);
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
// fillObjSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillObjSubRequest(castor::stager::DiskCopy* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectSubRequestStatement) {
    m_selectSubRequestStatement = createStatement(s_selectSubRequestStatementString);
  }
  // retrieve the object from the database
  std::set<int> subRequestsList;
  m_selectSubRequestStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectSubRequestStatement->executeQuery();
  while (rset->next()) {
    subRequestsList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::SubRequest*> toBeDeleted;
  for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
       it != obj->subRequests().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = subRequestsList.find((*it)->id())) == subRequestsList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      subRequestsList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::SubRequest*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeSubRequests(*it);
    (*it)->setDiskcopy(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = subRequestsList.begin();
       it != subRequestsList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::SubRequest* remoteObj = 
      dynamic_cast<castor::stager::SubRequest*>(item);
    obj->addSubRequests(remoteObj);
    remoteObj->setDiskcopy(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjFileSystem
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillObjFileSystem(castor::stager::DiskCopy* obj)
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
  u_signed64 fileSystemId = rset->getInt64(5);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->fileSystem() &&
      (0 == fileSystemId ||
       obj->fileSystem()->id() != fileSystemId)) {
    obj->fileSystem()->removeCopies(obj);
    obj->setFileSystem(0);
  }
  // Update object or create new one
  if (0 != fileSystemId) {
    if (0 == obj->fileSystem()) {
      obj->setFileSystem
        (dynamic_cast<castor::stager::FileSystem*>
         (cnvSvc()->getObjFromId(fileSystemId)));
    } else {
      cnvSvc()->updateObj(obj->fileSystem());
    }
    obj->fileSystem()->addCopies(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjCastorFile
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillObjCastorFile(castor::stager::DiskCopy* obj)
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
  u_signed64 castorFileId = rset->getInt64(6);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->castorFile() &&
      (0 == castorFileId ||
       obj->castorFile()->id() != castorFileId)) {
    obj->castorFile()->removeDiskCopies(obj);
    obj->setCastorFile(0);
  }
  // Update object or create new one
  if (0 != castorFileId) {
    if (0 == obj->castorFile()) {
      obj->setCastorFile
        (dynamic_cast<castor::stager::CastorFile*>
         (cnvSvc()->getObjFromId(castorFileId)));
    } else {
      cnvSvc()->updateObj(obj->castorFile());
    }
    obj->castorFile()->addDiskCopies(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit,
                                               unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(7, castor::db::DBTYPE_INT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->path());
    m_insertStatement->setFloat(2, obj->gcWeight());
    m_insertStatement->setInt(3, time(0));
    m_insertStatement->setInt64(4, (type == OBJ_FileSystem && obj->fileSystem() != 0) ? obj->fileSystem()->id() : 0);
    m_insertStatement->setInt64(5, (type == OBJ_CastorFile && obj->castorFile() != 0) ? obj->castorFile()->id() : 0);
    m_insertStatement->setInt(6, (int)obj->status());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getInt64(7));
    m_storeTypeStatement->setInt64(1, obj->id());
    m_storeTypeStatement->setInt64(2, obj->type());
    m_storeTypeStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  path : " << obj->path() << std::endl
                    << "  gcWeight : " << obj->gcWeight() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  fileSystem : " << obj->fileSystem() << std::endl
                    << "  castorFile : " << obj->castorFile() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::updateRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->path());
    m_updateStatement->setFloat(2, obj->gcWeight());
    m_updateStatement->setInt(3, (int)obj->status());
    m_updateStatement->setInt64(4, obj->id());
    m_updateStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
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

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::deleteRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
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
castor::IObject* castor::db::cnv::DbDiskCopyCnv::createObj(castor::IAddress* address)
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
    castor::stager::DiskCopy* object = new castor::stager::DiskCopy();
    // Now retrieve and set members
    object->setPath(rset->getString(1));
    object->setGcWeight(rset->getFloat(2));
    object->setCreationTime(rset->getInt64(3));
    object->setId(rset->getInt64(4));
    object->setStatus((enum castor::stager::DiskCopyStatusCodes)rset->getInt(7));
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
void castor::db::cnv::DbDiskCopyCnv::updateObj(castor::IObject* obj)
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
    castor::stager::DiskCopy* object = 
      dynamic_cast<castor::stager::DiskCopy*>(obj);
    object->setPath(rset->getString(1));
    object->setGcWeight(rset->getFloat(2));
    object->setCreationTime(rset->getInt64(3));
    object->setId(rset->getInt64(4));
    object->setStatus((enum castor::stager::DiskCopyStatusCodes)rset->getInt(7));
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


/******************************************************************************
 *                      castor/db/ora/OraDiskCopyCnv.cpp
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
#include "OraDiskCopyCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/IAddress.hpp"
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
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/DiskCopyStatusCode.hpp"
#include "castor/stager/FileSystem.hpp"
#include <list>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraDiskCopyCnv> s_factoryOraDiskCopyCnv;
const castor::IFactory<castor::IConverter>& OraDiskCopyCnvFactory = 
  s_factoryOraDiskCopyCnv;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraDiskCopyCnv::s_insertStatementString =
"INSERT INTO rh_DiskCopy (path, id, fileSystem, castorFile, status) VALUES (:1,:2,:3,:4,:5)";

/// SQL statement for request deletion
const std::string castor::db::ora::OraDiskCopyCnv::s_deleteStatementString =
"DELETE FROM rh_DiskCopy WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraDiskCopyCnv::s_selectStatementString =
"SELECT path, id, fileSystem, castorFile, status FROM rh_DiskCopy WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraDiskCopyCnv::s_updateStatementString =
"UPDATE rh_DiskCopy SET path = :1, fileSystem = :2, castorFile = :3, status = :4 WHERE id = :5";

/// SQL statement for type storage
const std::string castor::db::ora::OraDiskCopyCnv::s_storeTypeStatementString =
"INSERT INTO rh_Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraDiskCopyCnv::s_deleteTypeStatementString =
"DELETE FROM rh_Id2Type WHERE id = :1";

/// SQL insert statement for member fileSystem
const std::string castor::db::ora::OraDiskCopyCnv::s_insertFileSystem2DiskCopyStatementString =
"INSERT INTO rh_FileSystem2DiskCopy (Parent, Child) VALUES (:1, :2)";

/// SQL delete statement for member fileSystem
const std::string castor::db::ora::OraDiskCopyCnv::s_deleteFileSystem2DiskCopyStatementString =
"DELETE FROM rh_FileSystem2DiskCopy WHERE Parent = :1 AND Child = :2";

/// SQL insert statement for member castorFile
const std::string castor::db::ora::OraDiskCopyCnv::s_insertCastorFile2DiskCopyStatementString =
"INSERT INTO rh_CastorFile2DiskCopy (Parent, Child) VALUES (:1, :2)";

/// SQL delete statement for member castorFile
const std::string castor::db::ora::OraDiskCopyCnv::s_deleteCastorFile2DiskCopyStatementString =
"DELETE FROM rh_CastorFile2DiskCopy WHERE Parent = :1 AND Child = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraDiskCopyCnv::OraDiskCopyCnv() :
  OraBaseCnv(),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_insertFileSystem2DiskCopyStatement(0),
  m_deleteFileSystem2DiskCopyStatement(0),
  m_insertCastorFile2DiskCopyStatement(0),
  m_deleteCastorFile2DiskCopyStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraDiskCopyCnv::~OraDiskCopyCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskCopyCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_insertFileSystem2DiskCopyStatement);
    deleteStatement(m_deleteFileSystem2DiskCopyStatement);
    deleteStatement(m_insertCastorFile2DiskCopyStatement);
    deleteStatement(m_deleteCastorFile2DiskCopyStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_insertFileSystem2DiskCopyStatement = 0;
  m_deleteFileSystem2DiskCopyStatement = 0;
  m_insertCastorFile2DiskCopyStatement = 0;
  m_deleteCastorFile2DiskCopyStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraDiskCopyCnv::ObjType() {
  return castor::stager::DiskCopy::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraDiskCopyCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraDiskCopyCnv::createRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                castor::ObjectSet& alreadyDone,
                                                bool autocommit,
                                                bool recursive)
  throw (castor::exception::Exception) {
  castor::stager::DiskCopy* obj = 
    dynamic_cast<castor::stager::DiskCopy*>(object);
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
    // Mark the current object as done
    alreadyDone.insert(obj);
    // Set ids of all objects
    int nids = obj->id() == 0 ? 1 : 0;
    // check which objects need to be saved/updated and keeps a list of them
    std::list<castor::IObject*> toBeSaved;
    std::list<castor::IObject*> toBeUpdated;
    if (recursive) {
      if (alreadyDone.find(obj->fileSystem()) == alreadyDone.end() &&
          obj->fileSystem() != 0) {
        if (0 == obj->fileSystem()->id()) {
          toBeSaved.push_back(obj->fileSystem());
          nids++;
        } else {
          toBeUpdated.push_back(obj->fileSystem());
        }
      }
    }
    if (recursive) {
      if (alreadyDone.find(obj->castorFile()) == alreadyDone.end() &&
          obj->castorFile() != 0) {
        if (0 == obj->castorFile()->id()) {
          toBeSaved.push_back(obj->castorFile());
          nids++;
        } else {
          toBeUpdated.push_back(obj->castorFile());
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
    m_insertStatement->setString(1, obj->path());
    m_insertStatement->setDouble(2, obj->id());
    m_insertStatement->setDouble(3, obj->fileSystem() ? obj->fileSystem()->id() : 0);
    m_insertStatement->setDouble(4, obj->castorFile() ? obj->castorFile()->id() : 0);
    m_insertStatement->setDouble(5, (int)obj->status());
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
    // Deal with fileSystem
    if (0 != obj->fileSystem()) {
      if (0 == m_insertFileSystem2DiskCopyStatement) {
        m_insertFileSystem2DiskCopyStatement = createStatement(s_insertFileSystem2DiskCopyStatementString);
      }
      m_insertFileSystem2DiskCopyStatement->setDouble(1, obj->fileSystem()->id());
      m_insertFileSystem2DiskCopyStatement->setDouble(2, obj->id());
      m_insertFileSystem2DiskCopyStatement->executeUpdate();
    }
    // Deal with castorFile
    if (0 != obj->castorFile()) {
      if (0 == m_insertCastorFile2DiskCopyStatement) {
        m_insertCastorFile2DiskCopyStatement = createStatement(s_insertCastorFile2DiskCopyStatementString);
      }
      m_insertCastorFile2DiskCopyStatement->setDouble(1, obj->castorFile()->id());
      m_insertCastorFile2DiskCopyStatement->setDouble(2, obj->id());
      m_insertCastorFile2DiskCopyStatement->executeUpdate();
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
                    << "  path : " << obj->path() << std::endl
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
void castor::db::ora::OraDiskCopyCnv::updateRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                castor::ObjectSet& alreadyDone,
                                                bool autocommit,
                                                bool recursive)
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
      // Dealing with fileSystem
      {
        u_signed64 fileSystemId = (unsigned long long)rset->getDouble(3);
        castor::db::DbAddress ad(fileSystemId, " ", 0);
        if (0 != fileSystemId &&
            0 != obj->fileSystem() &&
            obj->fileSystem()->id() != fileSystemId) {
          cnvSvc()->deleteRepByAddress(&ad, false);
          fileSystemId = 0;
          if (0 == m_deleteFileSystem2DiskCopyStatement) {
            m_deleteFileSystem2DiskCopyStatement = createStatement(s_deleteFileSystem2DiskCopyStatementString);
          }
          m_deleteFileSystem2DiskCopyStatement->setDouble(1, obj->fileSystem()->id());
          m_deleteFileSystem2DiskCopyStatement->setDouble(2, obj->id());
          m_deleteFileSystem2DiskCopyStatement->executeUpdate();
        }
        if (fileSystemId == 0) {
          if (0 != obj->fileSystem()) {
            if (alreadyDone.find(obj->fileSystem()) == alreadyDone.end()) {
              cnvSvc()->createRep(&ad, obj->fileSystem(), alreadyDone, false, true);
              if (0 == m_insertFileSystem2DiskCopyStatement) {
                m_insertFileSystem2DiskCopyStatement = createStatement(s_insertFileSystem2DiskCopyStatementString);
              }
              m_insertFileSystem2DiskCopyStatement->setDouble(1, obj->fileSystem()->id());
              m_insertFileSystem2DiskCopyStatement->setDouble(2, obj->id());
              m_insertFileSystem2DiskCopyStatement->executeUpdate();
            }
          }
        } else {
          if (alreadyDone.find(obj->fileSystem()) == alreadyDone.end()) {
            cnvSvc()->updateRep(&ad, obj->fileSystem(), alreadyDone, false, recursive);
          }
        }
      }
      // Dealing with castorFile
      {
        u_signed64 castorFileId = (unsigned long long)rset->getDouble(4);
        castor::db::DbAddress ad(castorFileId, " ", 0);
        if (0 != castorFileId &&
            0 != obj->castorFile() &&
            obj->castorFile()->id() != castorFileId) {
          cnvSvc()->deleteRepByAddress(&ad, false);
          castorFileId = 0;
          if (0 == m_deleteCastorFile2DiskCopyStatement) {
            m_deleteCastorFile2DiskCopyStatement = createStatement(s_deleteCastorFile2DiskCopyStatementString);
          }
          m_deleteCastorFile2DiskCopyStatement->setDouble(1, obj->castorFile()->id());
          m_deleteCastorFile2DiskCopyStatement->setDouble(2, obj->id());
          m_deleteCastorFile2DiskCopyStatement->executeUpdate();
        }
        if (castorFileId == 0) {
          if (0 != obj->castorFile()) {
            if (alreadyDone.find(obj->castorFile()) == alreadyDone.end()) {
              cnvSvc()->createRep(&ad, obj->castorFile(), alreadyDone, false, true);
              if (0 == m_insertCastorFile2DiskCopyStatement) {
                m_insertCastorFile2DiskCopyStatement = createStatement(s_insertCastorFile2DiskCopyStatementString);
              }
              m_insertCastorFile2DiskCopyStatement->setDouble(1, obj->castorFile()->id());
              m_insertCastorFile2DiskCopyStatement->setDouble(2, obj->id());
              m_insertCastorFile2DiskCopyStatement->executeUpdate();
            }
          }
        } else {
          if (alreadyDone.find(obj->castorFile()) == alreadyDone.end()) {
            cnvSvc()->updateRep(&ad, obj->castorFile(), alreadyDone, false, recursive);
          }
        }
      }
      m_selectStatement->closeResultSet(rset);
    }
    // Now Update the current object
    m_updateStatement->setString(1, obj->path());
    m_updateStatement->setDouble(2, obj->fileSystem() ? obj->fileSystem()->id() : 0);
    m_updateStatement->setDouble(3, obj->castorFile() ? obj->castorFile()->id() : 0);
    m_updateStatement->setDouble(4, (int)obj->status());
    m_updateStatement->setDouble(5, obj->id());
    m_updateStatement->executeUpdate();
    if (recursive) {
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
void castor::db::ora::OraDiskCopyCnv::deleteRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                castor::ObjectSet& alreadyDone,
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
    // Mark the current object as done
    alreadyDone.insert(obj);
    // Now Delete the object
    m_deleteTypeStatement->setDouble(1, obj->id());
    m_deleteTypeStatement->executeUpdate();
    m_deleteStatement->setDouble(1, obj->id());
    m_deleteStatement->executeUpdate();
    // Delete link to fileSystem object
    if (0 != obj->fileSystem()) {
      // Check whether the statement is ok
      if (0 == m_deleteFileSystem2DiskCopyStatement) {
        m_deleteFileSystem2DiskCopyStatement = createStatement(s_deleteFileSystem2DiskCopyStatementString);
      }
      // Delete links to objects
      m_deleteFileSystem2DiskCopyStatement->setDouble(1, obj->fileSystem()->id());
      m_deleteFileSystem2DiskCopyStatement->setDouble(2, obj->id());
      m_deleteFileSystem2DiskCopyStatement->executeUpdate();
    }
    // Delete link to castorFile object
    if (0 != obj->castorFile()) {
      // Check whether the statement is ok
      if (0 == m_deleteCastorFile2DiskCopyStatement) {
        m_deleteCastorFile2DiskCopyStatement = createStatement(s_deleteCastorFile2DiskCopyStatementString);
      }
      // Delete links to objects
      m_deleteCastorFile2DiskCopyStatement->setDouble(1, obj->castorFile()->id());
      m_deleteCastorFile2DiskCopyStatement->setDouble(2, obj->id());
      m_deleteCastorFile2DiskCopyStatement->executeUpdate();
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
castor::IObject* castor::db::ora::OraDiskCopyCnv::createObj(castor::IAddress* address,
                                                            castor::ObjectCatalog& newlyCreated)
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
    castor::stager::DiskCopy* object = new castor::stager::DiskCopy();
    // Now retrieve and set members
    object->setPath(rset->getString(1));
    object->setId((unsigned long long)rset->getDouble(2));
    newlyCreated[object->id()] = object;
    u_signed64 fileSystemId = (unsigned long long)rset->getDouble(3);
    IObject* objFileSystem = cnvSvc()->getObjFromId(fileSystemId, newlyCreated);
    object->setFileSystem(dynamic_cast<castor::stager::FileSystem*>(objFileSystem));
    u_signed64 castorFileId = (unsigned long long)rset->getDouble(4);
    IObject* objCastorFile = cnvSvc()->getObjFromId(castorFileId, newlyCreated);
    object->setCastorFile(dynamic_cast<castor::stager::CastorFile*>(objCastorFile));
    object->setStatus((enum castor::stager::DiskCopyStatusCode)rset->getInt(5));
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
void castor::db::ora::OraDiskCopyCnv::updateObj(castor::IObject* obj,
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
    castor::stager::DiskCopy* object = 
      dynamic_cast<castor::stager::DiskCopy*>(obj);
    object->setPath(rset->getString(1));
    object->setId((unsigned long long)rset->getDouble(2));
    alreadyDone[obj->id()] = obj;
    // Dealing with fileSystem
    u_signed64 fileSystemId = (unsigned long long)rset->getDouble(3);
    if (0 != object->fileSystem() &&
        (0 == fileSystemId ||
         object->fileSystem()->id() != fileSystemId)) {
      delete object->fileSystem();
      object->setFileSystem(0);
    }
    if (0 != fileSystemId) {
      if (0 == object->fileSystem()) {
        object->setFileSystem
          (dynamic_cast<castor::stager::FileSystem*>
           (cnvSvc()->getObjFromId(fileSystemId, alreadyDone)));
      } else if (object->fileSystem()->id() == fileSystemId) {
        if (alreadyDone.find(object->fileSystem()->id()) == alreadyDone.end()) {
          cnvSvc()->updateObj(object->fileSystem(), alreadyDone);
        }
      }
    }
    // Dealing with castorFile
    u_signed64 castorFileId = (unsigned long long)rset->getDouble(4);
    if (0 != object->castorFile() &&
        (0 == castorFileId ||
         object->castorFile()->id() != castorFileId)) {
      delete object->castorFile();
      object->setCastorFile(0);
    }
    if (0 != castorFileId) {
      if (0 == object->castorFile()) {
        object->setCastorFile
          (dynamic_cast<castor::stager::CastorFile*>
           (cnvSvc()->getObjFromId(castorFileId, alreadyDone)));
      } else if (object->castorFile()->id() == castorFileId) {
        if (alreadyDone.find(object->castorFile()->id()) == alreadyDone.end()) {
          cnvSvc()->updateObj(object->castorFile(), alreadyDone);
        }
      }
    }
    object->setStatus((enum castor::stager::DiskCopyStatusCode)rset->getInt(5));
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


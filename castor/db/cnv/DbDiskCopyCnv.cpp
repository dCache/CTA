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
#include "castor/VectorAddress.hpp"
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
"INSERT INTO DiskCopy (path, gcWeight, creationTime, lastAccessTime, id, fileSystem, castorFile, status) VALUES (:1,:2,:3,NULL,ids_seq.nextval,:4,:5,:6) RETURNING id INTO :7";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbDiskCopyCnv::s_deleteStatementString =
"DELETE FROM DiskCopy WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbDiskCopyCnv::s_selectStatementString =
"SELECT path, gcWeight, creationTime, lastAccessTime, id, fileSystem, castorFile, status FROM DiskCopy WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbDiskCopyCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE RecordType IS RECORD (path VARCHAR2(2048), gcWeight NUMBER, creationTime INTEGER, lastAccessTime INTEGER, id INTEGER, fileSystem INTEGER, castorFile INTEGER, status INTEGER); \
   TYPE CurType IS REF CURSOR RETURN RecordType; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT path, gcWeight, creationTime, lastAccessTime, id, fileSystem, castorFile, status \
                     FROM DiskCopy t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

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
  m_bulkSelectStatement(0),
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
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_bulkSelectStatement) delete m_bulkSelectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_deleteSubRequestStatement) delete m_deleteSubRequestStatement;
    if(m_selectSubRequestStatement) delete m_selectSubRequestStatement;
    if(m_remoteUpdateSubRequestStatement) delete m_remoteUpdateSubRequestStatement;
    if(m_checkFileSystemExistStatement) delete m_checkFileSystemExistStatement;
    if(m_updateFileSystemStatement) delete m_updateFileSystemStatement;
    if(m_checkCastorFileExistStatement) delete m_checkCastorFileExistStatement;
    if(m_updateCastorFileStatement) delete m_updateCastorFileStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_bulkSelectStatement = 0;
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
                                             bool endTransaction)
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
// fillRepSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillRepSubRequest(castor::stager::DiskCopy* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectSubRequestStatement) {
    m_selectSubRequestStatement = createStatement(s_selectSubRequestStatementString);
  }
  // Get current database data
  std::set<u_signed64> subRequestsList;
  m_selectSubRequestStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectSubRequestStatement->executeQuery();
  while (rset->next()) {
    subRequestsList.insert(rset->getUInt64(1));
  }
  delete rset;
  // update subRequests and create new ones
  std::vector<castor::IObject*> toBeCreated;
  for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
       it != obj->subRequests().end();
       it++) {
    if (0 == (*it)->id()) {
      toBeCreated.push_back(*it);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateSubRequestStatement) {
        m_remoteUpdateSubRequestStatement = createStatement(s_remoteUpdateSubRequestStatementString);
      }
      // Update remote object
      m_remoteUpdateSubRequestStatement->setUInt64(1, obj->id());
      m_remoteUpdateSubRequestStatement->setUInt64(2, (*it)->id());
      m_remoteUpdateSubRequestStatement->execute();
      std::set<u_signed64>::iterator item;
      if ((item = subRequestsList.find((*it)->id())) != subRequestsList.end()) {
        subRequestsList.erase(item);
      }
    }
  }
  // create new objects
  cnvSvc()->bulkCreateRep(0, toBeCreated, false, OBJ_DiskCopy);
  // Delete old links
  for (std::set<u_signed64>::iterator it = subRequestsList.begin();
       it != subRequestsList.end();
       it++) {
    if (0 == m_deleteSubRequestStatement) {
      m_deleteSubRequestStatement = createStatement(s_deleteSubRequestStatementString);
    }
    m_deleteSubRequestStatement->setUInt64(1, *it);
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
    m_checkFileSystemExistStatement->setUInt64(1, obj->fileSystem()->id());
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
  m_updateFileSystemStatement->setUInt64(1, 0 == obj->fileSystem() ? 0 : obj->fileSystem()->id());
  m_updateFileSystemStatement->setUInt64(2, obj->id());
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
    m_checkCastorFileExistStatement->setUInt64(1, obj->castorFile()->id());
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
  m_updateCastorFileStatement->setUInt64(1, 0 == obj->castorFile() ? 0 : obj->castorFile()->id());
  m_updateCastorFileStatement->setUInt64(2, obj->id());
  m_updateCastorFileStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::fillObj(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
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
  if (endTransaction) {
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
  std::vector<u_signed64> subRequestsList;
  m_selectSubRequestStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectSubRequestStatement->executeQuery();
  while (rset->next()) {
    subRequestsList.push_back(rset->getUInt64(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::SubRequest*> toBeDeleted;
  for (std::vector<castor::stager::SubRequest*>::iterator it = obj->subRequests().begin();
       it != obj->subRequests().end();
       it++) {
    std::vector<u_signed64>::iterator item =
      std::find(subRequestsList.begin(), subRequestsList.end(), (*it)->id());
    if (item == subRequestsList.end()) {
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
  std::vector<castor::IObject*> newSubRequests =
    cnvSvc()->getObjsFromIds(subRequestsList, OBJ_SubRequest);
  for (std::vector<castor::IObject*>::iterator it = newSubRequests.begin();
       it != newSubRequests.end();
       it++) {
    castor::stager::SubRequest* remoteObj = 
      dynamic_cast<castor::stager::SubRequest*>(*it);
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
  m_selectStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
  if (!rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 fileSystemId = rset->getInt64(6);
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
  m_selectStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
  if (!rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 castorFileId = rset->getInt64(7);
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
                                               bool endTransaction,
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
      m_insertStatement->registerOutParam(7, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->path());
    m_insertStatement->setDouble(2, obj->gcWeight());
    m_insertStatement->setInt(3, time(0));
    m_insertStatement->setUInt64(4, (type == OBJ_FileSystem && obj->fileSystem() != 0) ? obj->fileSystem()->id() : 0);
    m_insertStatement->setUInt64(5, (type == OBJ_CastorFile && obj->castorFile() != 0) ? obj->castorFile()->id() : 0);
    m_insertStatement->setInt(6, (int)obj->status());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(7));
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
                    << "Statement was : " << std::endl
                    << s_insertStatementString << std::endl
                    << " and parameters' values were :" << std::endl
                    << "  path : " << obj->path() << std::endl
                    << "  gcWeight : " << obj->gcWeight() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  lastAccessTime : " << obj->lastAccessTime() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  fileSystem : " << obj->fileSystem() << std::endl
                    << "  castorFile : " << obj->castorFile() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::bulkCreateRep(castor::IAddress* address,
                                                   std::vector<castor::IObject*> &objects,
                                                   bool endTransaction,
                                                   unsigned int type)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::stager::DiskCopy*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::stager::DiskCopy*>(objects[i]));
  }
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(7, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for path
    unsigned int pathMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->path().length()+1 > pathMaxLen)
        pathMaxLen = objs[i]->path().length()+1;
    }
    char* pathBuffer = (char*) calloc(nb, pathMaxLen);
    unsigned short* pathBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      strncpy(pathBuffer+(i*pathMaxLen), objs[i]->path().c_str(), pathMaxLen);
      pathBufLens[i] = objs[i]->path().length()+1; // + 1 for the trailing \0
    }
    m_insertStatement->setDataBuffer
      (1, pathBuffer, castor::db::DBTYPE_STRING, pathMaxLen, pathBufLens);
    // build the buffers for gcWeight
    double* gcWeightBuffer = (double*) malloc(nb * sizeof(double));
    unsigned short* gcWeightBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      gcWeightBuffer[i] = objs[i]->gcWeight();
      gcWeightBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (2, gcWeightBuffer, castor::db::DBTYPE_DOUBLE, sizeof(gcWeightBuffer[0]), gcWeightBufLens);
    // build the buffers for creationTime
    double* creationTimeBuffer = (double*) malloc(nb * sizeof(double));
    unsigned short* creationTimeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      creationTimeBuffer[i] = time(0);
      creationTimeBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (3, creationTimeBuffer, castor::db::DBTYPE_UINT64, sizeof(creationTimeBuffer[0]), creationTimeBufLens);
    // build the buffers for fileSystem
    double* fileSystemBuffer = (double*) malloc(nb * sizeof(double));
    unsigned short* fileSystemBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      fileSystemBuffer[i] = (type == OBJ_FileSystem && objs[i]->fileSystem() != 0) ? objs[i]->fileSystem()->id() : 0;
      fileSystemBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (4, fileSystemBuffer, castor::db::DBTYPE_UINT64, sizeof(fileSystemBuffer[0]), fileSystemBufLens);
    // build the buffers for castorFile
    double* castorFileBuffer = (double*) malloc(nb * sizeof(double));
    unsigned short* castorFileBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      castorFileBuffer[i] = (type == OBJ_CastorFile && objs[i]->castorFile() != 0) ? objs[i]->castorFile()->id() : 0;
      castorFileBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (5, castorFileBuffer, castor::db::DBTYPE_UINT64, sizeof(castorFileBuffer[0]), castorFileBufLens);
    // build the buffers for status
    int* statusBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* statusBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      statusBuffer[i] = objs[i]->status();
      statusBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (6, statusBuffer, castor::db::DBTYPE_INT, sizeof(statusBuffer[0]), statusBufLens);
    // build the buffers for returned ids
    double* idBuffer = (double*) calloc(nb, sizeof(double));
    unsigned short* idBufLens = (unsigned short*) calloc(nb, sizeof(unsigned short));
    m_insertStatement->setDataBuffer
      (7, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
    m_insertStatement->execute(nb);
    for (int i = 0; i < nb; i++) {
      objects[i]->setId((u_signed64)idBuffer[i]);
    }
    // release the buffers for path
    free(pathBuffer);
    free(pathBufLens);
    // release the buffers for gcWeight
    free(gcWeightBuffer);
    free(gcWeightBufLens);
    // release the buffers for creationTime
    free(creationTimeBuffer);
    free(creationTimeBufLens);
    // release the buffers for fileSystem
    free(fileSystemBuffer);
    free(fileSystemBufLens);
    // release the buffers for castorFile
    free(castorFileBuffer);
    free(castorFileBufLens);
    // release the buffers for status
    free(statusBuffer);
    free(statusBufLens);
    // reuse idBuffer for bulk insertion into Id2Type
    m_storeTypeStatement->setDataBuffer
      (1, idBuffer, castor::db::DBTYPE_UINT64, sizeof(idBuffer[0]), idBufLens);
    // build the buffers for type
    int* typeBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* typeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      typeBuffer[i] = objs[i]->type();
      typeBufLens[i] = sizeof(int);
    }
    m_storeTypeStatement->setDataBuffer
      (2, typeBuffer, castor::db::DBTYPE_INT, sizeof(typeBuffer[0]), typeBufLens);
    m_storeTypeStatement->execute(nb);
    // release the buffers for type
    free(typeBuffer);
    free(typeBufLens);
    // release the buffers for returned ids
    free(idBuffer);
    free(idBufLens);
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { if (endTransaction) cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in bulkInsert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << " was called in bulk with "
                    << nb << " items." << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::updateRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool endTransaction)
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
    m_updateStatement->setDouble(2, obj->gcWeight());
    m_updateStatement->setInt(3, (int)obj->status());
    m_updateStatement->setUInt64(4, obj->id());
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
                    << "Statement was : " << std::endl
                    << s_updateStatementString << std::endl
                    << " and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbDiskCopyCnv::deleteRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool endTransaction)
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
                    << "Statement was : " << std::endl
                    << s_deleteStatementString << std::endl
                    << " and id was " << obj->id() << std::endl;;
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
    m_selectStatement->setUInt64(1, ad->target());
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
    object->setGcWeight(rset->getDouble(2));
    object->setCreationTime(rset->getUInt64(3));
    object->setLastAccessTime(rset->getUInt64(4));
    object->setId(rset->getUInt64(5));
    object->setStatus((enum castor::stager::DiskCopyStatusCodes)rset->getInt(8));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in select request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was : " << std::endl
                    << s_selectStatementString << std::endl
                    << " and id was " << ad->target() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateObj
//------------------------------------------------------------------------------
std::vector<castor::IObject*>
castor::db::cnv::DbDiskCopyCnv::bulkCreateObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  // Prepare result
  std::vector<castor::IObject*> res;
  // check whether something needs to be done
  castor::VectorAddress* ad = 
    dynamic_cast<castor::VectorAddress*>(address);
  int nb = ad->target().size();
  if (0 == nb) return res;
  try {
    // Check whether the statement is ok
    if (0 == m_bulkSelectStatement) {
      m_bulkSelectStatement = createStatement(s_bulkSelectStatementString);
      m_bulkSelectStatement->registerOutParam(2, castor::db::DBTYPE_CURSOR);
    }
    // set the buffer for input ids
    m_bulkSelectStatement->setDataBufferUInt64Array(1, ad->target());
    // Execute statement
    m_bulkSelectStatement->execute();
    // get the result, that is a cursor on the selected rows
    castor::db::IDbResultSet *rset =
      m_bulkSelectStatement->getCursor(2);
    // loop and create the new objects
    bool status = rset->next();
    while (status) {
      // create the new Object
      castor::stager::DiskCopy* object = new castor::stager::DiskCopy();
      // Now retrieve and set members
      object->setPath(rset->getString(1));
      object->setGcWeight(rset->getDouble(2));
      object->setCreationTime(rset->getUInt64(3));
      object->setLastAccessTime(rset->getUInt64(4));
      object->setId(rset->getUInt64(5));
      object->setStatus((enum castor::stager::DiskCopyStatusCodes)rset->getInt(8));
      // store object in results and loop;
      res.push_back(object);
      status = rset->next();
    }
    delete rset;
    return res;
  } catch (castor::exception::SQLError e) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in bulkSelect request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << " was called in bulk with "
                    << nb << " items." << std::endl;
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
    m_selectStatement->setUInt64(1, obj->id());
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
    object->setGcWeight(rset->getDouble(2));
    object->setCreationTime(rset->getUInt64(3));
    object->setLastAccessTime(rset->getUInt64(4));
    object->setId(rset->getUInt64(5));
    object->setStatus((enum castor::stager::DiskCopyStatusCodes)rset->getInt(8));
    delete rset;
  } catch (castor::exception::SQLError e) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was : " << std::endl
                    << s_updateStatementString << std::endl
                    << " and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


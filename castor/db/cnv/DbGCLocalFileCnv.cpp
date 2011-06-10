/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbGCLocalFileCnv.cpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "DbGCLocalFileCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/VectorAddress.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/exception/OutOfMemory.hpp"
#include "castor/stager/GCLocalFile.hpp"
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbGCLocalFileCnv>* s_factoryDbGCLocalFileCnv =
  new castor::CnvFactory<castor::db::cnv::DbGCLocalFileCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbGCLocalFileCnv::s_insertStatementString =
"INSERT INTO GCLocalFile (fileName, diskCopyId, fileId, nsHost, lastAccessTime, nbAccesses, gcWeight, gcTriggeredBy, svcClassName, id) VALUES (:1,:2,:3,:4,NULL,0,:5,:6,:7,ids_seq.nextval) RETURNING id INTO :8";

/// SQL statement for request bulk insertion
const std::string castor::db::cnv::DbGCLocalFileCnv::s_bulkInsertStatementString =
"INSERT /* bulk */ INTO GCLocalFile (fileName, diskCopyId, fileId, nsHost, lastAccessTime, nbAccesses, gcWeight, gcTriggeredBy, svcClassName, id) VALUES (:1,:2,:3,:4,NULL,0,:5,:6,:7,ids_seq.nextval) RETURNING id INTO :8";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbGCLocalFileCnv::s_deleteStatementString =
"DELETE FROM GCLocalFile WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbGCLocalFileCnv::s_selectStatementString =
"SELECT fileName, diskCopyId, fileId, nsHost, lastAccessTime, nbAccesses, gcWeight, gcTriggeredBy, svcClassName, id FROM GCLocalFile WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbGCLocalFileCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE RecordType IS RECORD (fileName VARCHAR2(2048), diskCopyId INTEGER, fileId INTEGER, nsHost VARCHAR2(2048), lastAccessTime INTEGER, nbAccesses NUMBER, gcWeight NUMBER, gcTriggeredBy VARCHAR2(2048), svcClassName VARCHAR2(2048), id INTEGER); \
   TYPE CurType IS REF CURSOR RETURN RecordType; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT fileName, diskCopyId, fileId, nsHost, lastAccessTime, nbAccesses, gcWeight, gcTriggeredBy, svcClassName, id \
                     FROM GCLocalFile t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbGCLocalFileCnv::s_updateStatementString =
"UPDATE GCLocalFile SET fileName = :1, diskCopyId = :2, fileId = :3, nsHost = :4, gcWeight = :5, gcTriggeredBy = :6, svcClassName = :7 WHERE id = :8";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbGCLocalFileCnv::DbGCLocalFileCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_bulkInsertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbGCLocalFileCnv::~DbGCLocalFileCnv() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_bulkInsertStatement) delete m_bulkInsertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_bulkSelectStatement) delete m_bulkSelectStatement;
    if(m_updateStatement) delete m_updateStatement;
  } catch (castor::exception::Exception& ignored) {};
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbGCLocalFileCnv::ObjType() {
  return castor::stager::GCLocalFile::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbGCLocalFileCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbGCLocalFileCnv::fillRep(castor::IAddress*,
                                                castor::IObject* object,
                                                unsigned int type,
                                                bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::GCLocalFile* obj = 
    dynamic_cast<castor::stager::GCLocalFile*>(object);
  try {
    switch (type) {
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
  } catch (castor::exception::SQLError& e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.getMessage().str() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbGCLocalFileCnv::fillObj(castor::IAddress*,
                                                castor::IObject* object,
                                                unsigned int type,
                                                bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::GCLocalFile* obj = 
    dynamic_cast<castor::stager::GCLocalFile*>(object);
  switch (type) {
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
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbGCLocalFileCnv::createRep(castor::IAddress*,
                                                  castor::IObject* object,
                                                  bool endTransaction,
                                                  unsigned int)
  throw (castor::exception::Exception) {
  castor::stager::GCLocalFile* obj = 
    dynamic_cast<castor::stager::GCLocalFile*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(8, castor::db::DBTYPE_UINT64);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->fileName());
    m_insertStatement->setUInt64(2, obj->diskCopyId());
    m_insertStatement->setUInt64(3, obj->fileId());
    m_insertStatement->setString(4, obj->nsHost());
    m_insertStatement->setInt(5, obj->nbAccesses());
    m_insertStatement->setDouble(6, obj->gcWeight());
    m_insertStatement->setString(7, obj->gcTriggeredBy());
    m_insertStatement->setString(8, obj->svcClassName());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(9));
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError& e) {
    // Always try to rollback
    try {
      if (endTransaction) cnvSvc()->rollback();
    } catch (castor::exception::Exception& ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was : " << std::endl
                    << s_insertStatementString << std::endl
                    << " and parameters' values were :" << std::endl
                    << "  fileName : " << obj->fileName() << std::endl
                    << "  diskCopyId : " << obj->diskCopyId() << std::endl
                    << "  fileId : " << obj->fileId() << std::endl
                    << "  nsHost : " << obj->nsHost() << std::endl
                    << "  lastAccessTime : " << obj->lastAccessTime() << std::endl
                    << "  nbAccesses : " << obj->nbAccesses() << std::endl
                    << "  gcWeight : " << obj->gcWeight() << std::endl
                    << "  gcTriggeredBy : " << obj->gcTriggeredBy() << std::endl
                    << "  svcClassName : " << obj->svcClassName() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbGCLocalFileCnv::bulkCreateRep(castor::IAddress*,
                                                      std::vector<castor::IObject*> &objects,
                                                      bool endTransaction,
                                                      unsigned int)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::stager::GCLocalFile*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::stager::GCLocalFile*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_bulkInsertStatement) {
      m_bulkInsertStatement = createStatement(s_bulkInsertStatementString);
      m_bulkInsertStatement->registerOutParam(8, castor::db::DBTYPE_UINT64);
    }
    // build the buffers for fileName
    unsigned int fileNameMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->fileName().length()+1 > fileNameMaxLen)
        fileNameMaxLen = objs[i]->fileName().length()+1;
    }
    char* fileNameBuffer = (char*) calloc(nb, fileNameMaxLen);
    if (fileNameBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(fileNameBuffer);
    unsigned short* fileNameBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (fileNameBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(fileNameBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(fileNameBuffer+(i*fileNameMaxLen), objs[i]->fileName().c_str(), fileNameMaxLen);
      fileNameBufLens[i] = objs[i]->fileName().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (1, fileNameBuffer, castor::db::DBTYPE_STRING, fileNameMaxLen, fileNameBufLens);
    // build the buffers for diskCopyId
    double* diskCopyIdBuffer = (double*) malloc(nb * sizeof(double));
    if (diskCopyIdBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(diskCopyIdBuffer);
    unsigned short* diskCopyIdBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (diskCopyIdBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(diskCopyIdBufLens);
    for (int i = 0; i < nb; i++) {
      diskCopyIdBuffer[i] = objs[i]->diskCopyId();
      diskCopyIdBufLens[i] = sizeof(double);
    }
    m_bulkInsertStatement->setDataBuffer
      (2, diskCopyIdBuffer, castor::db::DBTYPE_UINT64, sizeof(diskCopyIdBuffer[0]), diskCopyIdBufLens);
    // build the buffers for fileId
    double* fileIdBuffer = (double*) malloc(nb * sizeof(double));
    if (fileIdBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(fileIdBuffer);
    unsigned short* fileIdBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (fileIdBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(fileIdBufLens);
    for (int i = 0; i < nb; i++) {
      fileIdBuffer[i] = objs[i]->fileId();
      fileIdBufLens[i] = sizeof(double);
    }
    m_bulkInsertStatement->setDataBuffer
      (3, fileIdBuffer, castor::db::DBTYPE_UINT64, sizeof(fileIdBuffer[0]), fileIdBufLens);
    // build the buffers for nsHost
    unsigned int nsHostMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->nsHost().length()+1 > nsHostMaxLen)
        nsHostMaxLen = objs[i]->nsHost().length()+1;
    }
    char* nsHostBuffer = (char*) calloc(nb, nsHostMaxLen);
    if (nsHostBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(nsHostBuffer);
    unsigned short* nsHostBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (nsHostBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(nsHostBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(nsHostBuffer+(i*nsHostMaxLen), objs[i]->nsHost().c_str(), nsHostMaxLen);
      nsHostBufLens[i] = objs[i]->nsHost().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (4, nsHostBuffer, castor::db::DBTYPE_STRING, nsHostMaxLen, nsHostBufLens);
    // build the buffers for gcWeight
    double* gcWeightBuffer = (double*) malloc(nb * sizeof(double));
    if (gcWeightBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(gcWeightBuffer);
    unsigned short* gcWeightBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (gcWeightBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(gcWeightBufLens);
    for (int i = 0; i < nb; i++) {
      gcWeightBuffer[i] = objs[i]->gcWeight();
      gcWeightBufLens[i] = sizeof(double);
    }
    m_bulkInsertStatement->setDataBuffer
      (5, gcWeightBuffer, castor::db::DBTYPE_DOUBLE, sizeof(gcWeightBuffer[0]), gcWeightBufLens);
    // build the buffers for gcTriggeredBy
    unsigned int gcTriggeredByMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->gcTriggeredBy().length()+1 > gcTriggeredByMaxLen)
        gcTriggeredByMaxLen = objs[i]->gcTriggeredBy().length()+1;
    }
    char* gcTriggeredByBuffer = (char*) calloc(nb, gcTriggeredByMaxLen);
    if (gcTriggeredByBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(gcTriggeredByBuffer);
    unsigned short* gcTriggeredByBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (gcTriggeredByBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(gcTriggeredByBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(gcTriggeredByBuffer+(i*gcTriggeredByMaxLen), objs[i]->gcTriggeredBy().c_str(), gcTriggeredByMaxLen);
      gcTriggeredByBufLens[i] = objs[i]->gcTriggeredBy().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (6, gcTriggeredByBuffer, castor::db::DBTYPE_STRING, gcTriggeredByMaxLen, gcTriggeredByBufLens);
    // build the buffers for svcClassName
    unsigned int svcClassNameMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->svcClassName().length()+1 > svcClassNameMaxLen)
        svcClassNameMaxLen = objs[i]->svcClassName().length()+1;
    }
    char* svcClassNameBuffer = (char*) calloc(nb, svcClassNameMaxLen);
    if (svcClassNameBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(svcClassNameBuffer);
    unsigned short* svcClassNameBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (svcClassNameBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(svcClassNameBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(svcClassNameBuffer+(i*svcClassNameMaxLen), objs[i]->svcClassName().c_str(), svcClassNameMaxLen);
      svcClassNameBufLens[i] = objs[i]->svcClassName().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (7, svcClassNameBuffer, castor::db::DBTYPE_STRING, svcClassNameMaxLen, svcClassNameBufLens);
    // build the buffers for returned ids
    double* idBuffer = (double*) calloc(nb, sizeof(double));
    if (idBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(idBuffer);
    unsigned short* idBufLens = (unsigned short*) calloc(nb, sizeof(unsigned short));
    if (idBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(idBufLens);
    m_bulkInsertStatement->setDataBuffer
      (8, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
    m_bulkInsertStatement->execute(nb);
    for (int i = 0; i < nb; i++) {
      objects[i]->setId((u_signed64)idBuffer[i]);
    }
    // release the buffers
    for (unsigned int i = 0; i < allocMem.size(); i++) {
      free(allocMem[i]);
    }
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError& e) {
    // release the buffers
    for (unsigned int i = 0; i < allocMem.size(); i++) {
      free(allocMem[i]);
    }
    // Always try to rollback
    try {
      if (endTransaction) cnvSvc()->rollback();
    } catch (castor::exception::Exception& ignored) {}
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
void castor::db::cnv::DbGCLocalFileCnv::updateRep(castor::IAddress*,
                                                  castor::IObject* object,
                                                  bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::GCLocalFile* obj = 
    dynamic_cast<castor::stager::GCLocalFile*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->fileName());
    m_updateStatement->setUInt64(2, obj->diskCopyId());
    m_updateStatement->setUInt64(3, obj->fileId());
    m_updateStatement->setString(4, obj->nsHost());
    m_updateStatement->setDouble(5, obj->gcWeight());
    m_updateStatement->setString(6, obj->gcTriggeredBy());
    m_updateStatement->setString(7, obj->svcClassName());
    m_updateStatement->setUInt64(8, obj->id());
    m_updateStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError& e) {
    // Always try to rollback
    try {
      if (endTransaction) cnvSvc()->rollback();
    } catch (castor::exception::Exception& ignored) {}
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
void castor::db::cnv::DbGCLocalFileCnv::deleteRep(castor::IAddress*,
                                                  castor::IObject* object,
                                                  bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::GCLocalFile* obj = 
    dynamic_cast<castor::stager::GCLocalFile*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_deleteStatement) {
      m_deleteStatement = createStatement(s_deleteStatementString);
    }
    // Now Delete the object
    m_deleteStatement->setUInt64(1, obj->id());
    m_deleteStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError& e) {
    // Always try to rollback
    try {
      if (endTransaction) cnvSvc()->rollback();
    } catch (castor::exception::Exception& ignored) {}
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
castor::IObject* castor::db::cnv::DbGCLocalFileCnv::createObj(castor::IAddress* address)
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
    castor::stager::GCLocalFile* object = new castor::stager::GCLocalFile();
    // Now retrieve and set members
    object->setFileName(rset->getString(1));
    object->setDiskCopyId(rset->getUInt64(2));
    object->setFileId(rset->getUInt64(3));
    object->setNsHost(rset->getString(4));
    object->setLastAccessTime(rset->getUInt64(5));
    object->setNbAccesses(rset->getInt(6));
    object->setGcWeight(rset->getDouble(7));
    object->setGcTriggeredBy(rset->getString(8));
    object->setSvcClassName(rset->getString(9));
    object->setId(rset->getUInt64(10));
    delete rset;
    return object;
  } catch (castor::exception::SQLError& e) {
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
castor::db::cnv::DbGCLocalFileCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::stager::GCLocalFile* object = new castor::stager::GCLocalFile();
      // Now retrieve and set members
      object->setFileName(rset->getString(1));
      object->setDiskCopyId(rset->getUInt64(2));
      object->setFileId(rset->getUInt64(3));
      object->setNsHost(rset->getString(4));
      object->setLastAccessTime(rset->getUInt64(5));
      object->setNbAccesses(rset->getInt(6));
      object->setGcWeight(rset->getDouble(7));
      object->setGcTriggeredBy(rset->getString(8));
      object->setSvcClassName(rset->getString(9));
      object->setId(rset->getUInt64(10));
      // store object in results and loop;
      res.push_back(object);
      status = rset->next();
    }
    delete rset;
    return res;
  } catch (castor::exception::SQLError& e) {
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
void castor::db::cnv::DbGCLocalFileCnv::updateObj(castor::IObject* obj)
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
    castor::stager::GCLocalFile* object = 
      dynamic_cast<castor::stager::GCLocalFile*>(obj);
    object->setFileName(rset->getString(1));
    object->setDiskCopyId(rset->getUInt64(2));
    object->setFileId(rset->getUInt64(3));
    object->setNsHost(rset->getString(4));
    object->setLastAccessTime(rset->getUInt64(5));
    object->setNbAccesses(rset->getInt(6));
    object->setGcWeight(rset->getDouble(7));
    object->setGcTriggeredBy(rset->getString(8));
    object->setSvcClassName(rset->getString(9));
    object->setId(rset->getUInt64(10));
    delete rset;
  } catch (castor::exception::SQLError& e) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was : " << std::endl
                    << s_updateStatementString << std::endl
                    << " and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


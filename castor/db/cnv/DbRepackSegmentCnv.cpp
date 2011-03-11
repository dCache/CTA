/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbRepackSegmentCnv.cpp
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
#include "DbRepackSegmentCnv.hpp"
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
#include "castor/exception/OutOfMemory.hpp"
#include "castor/repack/RepackSegment.hpp"
#include "castor/repack/RepackSubRequest.hpp"
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbRepackSegmentCnv>* s_factoryDbRepackSegmentCnv =
  new castor::CnvFactory<castor::db::cnv::DbRepackSegmentCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbRepackSegmentCnv::s_insertStatementString =
"INSERT INTO RepackSegment (fileid, segsize, compression, filesec, copyno, blockid, fileseq, errorCode, errorMessage, id, repacksubrequest) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,ids_seq.nextval,:10) RETURNING id INTO :11";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbRepackSegmentCnv::s_deleteStatementString =
"DELETE FROM RepackSegment WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbRepackSegmentCnv::s_selectStatementString =
"SELECT fileid, segsize, compression, filesec, copyno, blockid, fileseq, errorCode, errorMessage, id, repacksubrequest FROM RepackSegment WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbRepackSegmentCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE CurType IS REF CURSOR RETURN RepackSegment%ROWTYPE; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT fileid, segsize, compression, filesec, copyno, blockid, fileseq, errorCode, errorMessage, id, repacksubrequest \
                     FROM RepackSegment t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbRepackSegmentCnv::s_updateStatementString =
"UPDATE RepackSegment SET fileid = :1, segsize = :2, compression = :3, filesec = :4, copyno = :5, blockid = :6, fileseq = :7, errorCode = :8, errorMessage = :9 WHERE id = :10";

/// SQL statement for type storage
const std::string castor::db::cnv::DbRepackSegmentCnv::s_storeTypeStatementString =
"INSERT /* RepackSegment class */ INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbRepackSegmentCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member repacksubrequest
const std::string castor::db::cnv::DbRepackSegmentCnv::s_checkRepackSubRequestExistStatementString =
"SELECT id FROM RepackSubRequest WHERE id = :1";

/// SQL update statement for member repacksubrequest
const std::string castor::db::cnv::DbRepackSegmentCnv::s_updateRepackSubRequestStatementString =
"UPDATE RepackSegment SET repacksubrequest = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbRepackSegmentCnv::DbRepackSegmentCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkRepackSubRequestExistStatement(0),
  m_updateRepackSubRequestStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbRepackSegmentCnv::~DbRepackSegmentCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSegmentCnv::reset() throw() {
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
    if(m_checkRepackSubRequestExistStatement) delete m_checkRepackSubRequestExistStatement;
    if(m_updateRepackSubRequestStatement) delete m_updateRepackSubRequestStatement;
  } catch (castor::exception::Exception& ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_bulkSelectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkRepackSubRequestExistStatement = 0;
  m_updateRepackSubRequestStatement = 0;
  // Call upper level reset
  this->DbBaseCnv::reset();
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbRepackSegmentCnv::ObjType() {
  return castor::repack::RepackSegment::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbRepackSegmentCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSegmentCnv::fillRep(castor::IAddress*,
                                                  castor::IObject* object,
                                                  unsigned int type,
                                                  bool endTransaction)
  throw (castor::exception::Exception) {
  castor::repack::RepackSegment* obj = 
    dynamic_cast<castor::repack::RepackSegment*>(object);
  try {
    switch (type) {
    case castor::OBJ_RepackSubRequest :
      fillRepRepackSubRequest(obj);
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
  } catch (castor::exception::SQLError& e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.getMessage().str() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepRepackSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSegmentCnv::fillRepRepackSubRequest(castor::repack::RepackSegment* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->repacksubrequest()) {
    // Check checkRepackSubRequestExist statement
    if (0 == m_checkRepackSubRequestExistStatement) {
      m_checkRepackSubRequestExistStatement = createStatement(s_checkRepackSubRequestExistStatementString);
    }
    // retrieve the object from the database
    m_checkRepackSubRequestExistStatement->setUInt64(1, obj->repacksubrequest()->id());
    castor::db::IDbResultSet *rset = m_checkRepackSubRequestExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->repacksubrequest(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateRepackSubRequestStatement) {
    m_updateRepackSubRequestStatement = createStatement(s_updateRepackSubRequestStatementString);
  }
  // Update local object
  m_updateRepackSubRequestStatement->setUInt64(1, 0 == obj->repacksubrequest() ? 0 : obj->repacksubrequest()->id());
  m_updateRepackSubRequestStatement->setUInt64(2, obj->id());
  m_updateRepackSubRequestStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSegmentCnv::fillObj(castor::IAddress*,
                                                  castor::IObject* object,
                                                  unsigned int type,
                                                  bool endTransaction)
  throw (castor::exception::Exception) {
  castor::repack::RepackSegment* obj = 
    dynamic_cast<castor::repack::RepackSegment*>(object);
  switch (type) {
  case castor::OBJ_RepackSubRequest :
    fillObjRepackSubRequest(obj);
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
// fillObjRepackSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSegmentCnv::fillObjRepackSubRequest(castor::repack::RepackSegment* obj)
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
  u_signed64 repacksubrequestId = rset->getInt64(11);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->repacksubrequest() &&
      (0 == repacksubrequestId ||
       obj->repacksubrequest()->id() != repacksubrequestId)) {
    obj->repacksubrequest()->removeRepacksegment(obj);
    obj->setRepacksubrequest(0);
  }
  // Update object or create new one
  if (0 != repacksubrequestId) {
    if (0 == obj->repacksubrequest()) {
      obj->setRepacksubrequest
        (dynamic_cast<castor::repack::RepackSubRequest*>
         (cnvSvc()->getObjFromId(repacksubrequestId)));
    } else {
      cnvSvc()->updateObj(obj->repacksubrequest());
    }
    obj->repacksubrequest()->addRepacksegment(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSegmentCnv::createRep(castor::IAddress*,
                                                    castor::IObject* object,
                                                    bool endTransaction,
                                                    unsigned int type)
  throw (castor::exception::Exception) {
  castor::repack::RepackSegment* obj = 
    dynamic_cast<castor::repack::RepackSegment*>(object);
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
    m_insertStatement->setUInt64(1, obj->fileid());
    m_insertStatement->setUInt64(2, obj->segsize());
    m_insertStatement->setInt(3, obj->compression());
    m_insertStatement->setInt(4, obj->filesec());
    m_insertStatement->setInt(5, obj->copyno());
    m_insertStatement->setUInt64(6, obj->blockid());
    m_insertStatement->setUInt64(7, obj->fileseq());
    m_insertStatement->setInt(8, obj->errorCode());
    m_insertStatement->setString(9, obj->errorMessage());
    m_insertStatement->setUInt64(10, (type == OBJ_RepackSubRequest && obj->repacksubrequest() != 0) ? obj->repacksubrequest()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(11));
    m_storeTypeStatement->setUInt64(1, obj->id());
    m_storeTypeStatement->setUInt64(2, obj->type());
    m_storeTypeStatement->execute();
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
                    << "  fileid : " << obj->fileid() << std::endl
                    << "  segsize : " << obj->segsize() << std::endl
                    << "  compression : " << obj->compression() << std::endl
                    << "  filesec : " << obj->filesec() << std::endl
                    << "  copyno : " << obj->copyno() << std::endl
                    << "  blockid : " << obj->blockid() << std::endl
                    << "  fileseq : " << obj->fileseq() << std::endl
                    << "  errorCode : " << obj->errorCode() << std::endl
                    << "  errorMessage : " << obj->errorMessage() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  repacksubrequest : " << (obj->repacksubrequest() ? obj->repacksubrequest()->id() : 0) << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSegmentCnv::bulkCreateRep(castor::IAddress*,
                                                        std::vector<castor::IObject*> &objects,
                                                        bool endTransaction,
                                                        unsigned int type)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::repack::RepackSegment*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::repack::RepackSegment*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(11, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for fileid
    double* fileidBuffer = (double*) malloc(nb * sizeof(double));
    if (fileidBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(fileidBuffer);
    unsigned short* fileidBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (fileidBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(fileidBufLens);
    for (int i = 0; i < nb; i++) {
      fileidBuffer[i] = objs[i]->fileid();
      fileidBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (1, fileidBuffer, castor::db::DBTYPE_UINT64, sizeof(fileidBuffer[0]), fileidBufLens);
    // build the buffers for segsize
    double* segsizeBuffer = (double*) malloc(nb * sizeof(double));
    if (segsizeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(segsizeBuffer);
    unsigned short* segsizeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (segsizeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(segsizeBufLens);
    for (int i = 0; i < nb; i++) {
      segsizeBuffer[i] = objs[i]->segsize();
      segsizeBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (2, segsizeBuffer, castor::db::DBTYPE_UINT64, sizeof(segsizeBuffer[0]), segsizeBufLens);
    // build the buffers for compression
    int* compressionBuffer = (int*) malloc(nb * sizeof(int));
    if (compressionBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(compressionBuffer);
    unsigned short* compressionBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (compressionBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(compressionBufLens);
    for (int i = 0; i < nb; i++) {
      compressionBuffer[i] = objs[i]->compression();
      compressionBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (3, compressionBuffer, castor::db::DBTYPE_INT, sizeof(compressionBuffer[0]), compressionBufLens);
    // build the buffers for filesec
    int* filesecBuffer = (int*) malloc(nb * sizeof(int));
    if (filesecBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(filesecBuffer);
    unsigned short* filesecBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (filesecBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(filesecBufLens);
    for (int i = 0; i < nb; i++) {
      filesecBuffer[i] = objs[i]->filesec();
      filesecBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (4, filesecBuffer, castor::db::DBTYPE_INT, sizeof(filesecBuffer[0]), filesecBufLens);
    // build the buffers for copyno
    int* copynoBuffer = (int*) malloc(nb * sizeof(int));
    if (copynoBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(copynoBuffer);
    unsigned short* copynoBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (copynoBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(copynoBufLens);
    for (int i = 0; i < nb; i++) {
      copynoBuffer[i] = objs[i]->copyno();
      copynoBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (5, copynoBuffer, castor::db::DBTYPE_INT, sizeof(copynoBuffer[0]), copynoBufLens);
    // build the buffers for blockid
    double* blockidBuffer = (double*) malloc(nb * sizeof(double));
    if (blockidBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(blockidBuffer);
    unsigned short* blockidBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (blockidBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(blockidBufLens);
    for (int i = 0; i < nb; i++) {
      blockidBuffer[i] = objs[i]->blockid();
      blockidBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (6, blockidBuffer, castor::db::DBTYPE_UINT64, sizeof(blockidBuffer[0]), blockidBufLens);
    // build the buffers for fileseq
    double* fileseqBuffer = (double*) malloc(nb * sizeof(double));
    if (fileseqBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(fileseqBuffer);
    unsigned short* fileseqBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (fileseqBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(fileseqBufLens);
    for (int i = 0; i < nb; i++) {
      fileseqBuffer[i] = objs[i]->fileseq();
      fileseqBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (7, fileseqBuffer, castor::db::DBTYPE_UINT64, sizeof(fileseqBuffer[0]), fileseqBufLens);
    // build the buffers for errorCode
    int* errorCodeBuffer = (int*) malloc(nb * sizeof(int));
    if (errorCodeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(errorCodeBuffer);
    unsigned short* errorCodeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (errorCodeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(errorCodeBufLens);
    for (int i = 0; i < nb; i++) {
      errorCodeBuffer[i] = objs[i]->errorCode();
      errorCodeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (8, errorCodeBuffer, castor::db::DBTYPE_INT, sizeof(errorCodeBuffer[0]), errorCodeBufLens);
    // build the buffers for errorMessage
    unsigned int errorMessageMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->errorMessage().length()+1 > errorMessageMaxLen)
        errorMessageMaxLen = objs[i]->errorMessage().length()+1;
    }
    char* errorMessageBuffer = (char*) calloc(nb, errorMessageMaxLen);
    if (errorMessageBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(errorMessageBuffer);
    unsigned short* errorMessageBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (errorMessageBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(errorMessageBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(errorMessageBuffer+(i*errorMessageMaxLen), objs[i]->errorMessage().c_str(), errorMessageMaxLen);
      errorMessageBufLens[i] = objs[i]->errorMessage().length()+1; // + 1 for the trailing \0
    }
    m_insertStatement->setDataBuffer
      (9, errorMessageBuffer, castor::db::DBTYPE_STRING, errorMessageMaxLen, errorMessageBufLens);
    // build the buffers for repacksubrequest
    double* repacksubrequestBuffer = (double*) malloc(nb * sizeof(double));
    if (repacksubrequestBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(repacksubrequestBuffer);
    unsigned short* repacksubrequestBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (repacksubrequestBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(repacksubrequestBufLens);
    for (int i = 0; i < nb; i++) {
      repacksubrequestBuffer[i] = (type == OBJ_RepackSubRequest && objs[i]->repacksubrequest() != 0) ? objs[i]->repacksubrequest()->id() : 0;
      repacksubrequestBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (10, repacksubrequestBuffer, castor::db::DBTYPE_UINT64, sizeof(repacksubrequestBuffer[0]), repacksubrequestBufLens);
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
    m_insertStatement->setDataBuffer
      (11, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
    m_insertStatement->execute(nb);
    for (int i = 0; i < nb; i++) {
      objects[i]->setId((u_signed64)idBuffer[i]);
    }
    // reuse idBuffer for bulk insertion into Id2Type
    m_storeTypeStatement->setDataBuffer
      (1, idBuffer, castor::db::DBTYPE_UINT64, sizeof(idBuffer[0]), idBufLens);
    // build the buffers for type
    int* typeBuffer = (int*) malloc(nb * sizeof(int));
    if (typeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(typeBuffer);
    unsigned short* typeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (typeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(typeBufLens);
    for (int i = 0; i < nb; i++) {
      typeBuffer[i] = objs[i]->type();
      typeBufLens[i] = sizeof(int);
    }
    m_storeTypeStatement->setDataBuffer
      (2, typeBuffer, castor::db::DBTYPE_INT, sizeof(typeBuffer[0]), typeBufLens);
    m_storeTypeStatement->execute(nb);
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
void castor::db::cnv::DbRepackSegmentCnv::updateRep(castor::IAddress*,
                                                    castor::IObject* object,
                                                    bool endTransaction)
  throw (castor::exception::Exception) {
  castor::repack::RepackSegment* obj = 
    dynamic_cast<castor::repack::RepackSegment*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setUInt64(1, obj->fileid());
    m_updateStatement->setUInt64(2, obj->segsize());
    m_updateStatement->setInt(3, obj->compression());
    m_updateStatement->setInt(4, obj->filesec());
    m_updateStatement->setInt(5, obj->copyno());
    m_updateStatement->setUInt64(6, obj->blockid());
    m_updateStatement->setUInt64(7, obj->fileseq());
    m_updateStatement->setInt(8, obj->errorCode());
    m_updateStatement->setString(9, obj->errorMessage());
    m_updateStatement->setUInt64(10, obj->id());
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
void castor::db::cnv::DbRepackSegmentCnv::deleteRep(castor::IAddress*,
                                                    castor::IObject* object,
                                                    bool endTransaction)
  throw (castor::exception::Exception) {
  castor::repack::RepackSegment* obj = 
    dynamic_cast<castor::repack::RepackSegment*>(object);
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
castor::IObject* castor::db::cnv::DbRepackSegmentCnv::createObj(castor::IAddress* address)
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
    castor::repack::RepackSegment* object = new castor::repack::RepackSegment();
    // Now retrieve and set members
    object->setFileid(rset->getUInt64(1));
    object->setSegsize(rset->getUInt64(2));
    object->setCompression(rset->getInt(3));
    object->setFilesec(rset->getInt(4));
    object->setCopyno(rset->getInt(5));
    object->setBlockid(rset->getUInt64(6));
    object->setFileseq(rset->getUInt64(7));
    object->setErrorCode(rset->getInt(8));
    object->setErrorMessage(rset->getString(9));
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
castor::db::cnv::DbRepackSegmentCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::repack::RepackSegment* object = new castor::repack::RepackSegment();
      // Now retrieve and set members
      object->setFileid(rset->getUInt64(1));
      object->setSegsize(rset->getUInt64(2));
      object->setCompression(rset->getInt(3));
      object->setFilesec(rset->getInt(4));
      object->setCopyno(rset->getInt(5));
      object->setBlockid(rset->getUInt64(6));
      object->setFileseq(rset->getUInt64(7));
      object->setErrorCode(rset->getInt(8));
      object->setErrorMessage(rset->getString(9));
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
void castor::db::cnv::DbRepackSegmentCnv::updateObj(castor::IObject* obj)
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
    castor::repack::RepackSegment* object = 
      dynamic_cast<castor::repack::RepackSegment*>(obj);
    object->setFileid(rset->getUInt64(1));
    object->setSegsize(rset->getUInt64(2));
    object->setCompression(rset->getInt(3));
    object->setFilesec(rset->getInt(4));
    object->setCopyno(rset->getInt(5));
    object->setBlockid(rset->getUInt64(6));
    object->setFileseq(rset->getUInt64(7));
    object->setErrorCode(rset->getInt(8));
    object->setErrorMessage(rset->getString(9));
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


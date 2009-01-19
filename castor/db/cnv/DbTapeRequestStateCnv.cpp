/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbTapeRequestStateCnv.cpp
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
#include "DbTapeRequestStateCnv.hpp"
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
#include "castor/stager/Stream.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/tape/tapegateway/TapeRequestState.hpp"
#include "castor/tape/tapegateway/TapeRequestStateCode.hpp"
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbTapeRequestStateCnv>* s_factoryDbTapeRequestStateCnv =
  new castor::CnvFactory<castor::db::cnv::DbTapeRequestStateCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_insertStatementString =
"INSERT INTO TapeRequestState (accessMode, startTime, lastVdqmPingTime, vdqmVolReqId, id, streamMigration, tapeRecall, status) VALUES (:1,:2,:3,:4,ids_seq.nextval,:5,:6,:7) RETURNING id INTO :8";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_deleteStatementString =
"DELETE FROM TapeRequestState WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_selectStatementString =
"SELECT accessMode, startTime, lastVdqmPingTime, vdqmVolReqId, id, streamMigration, tapeRecall, status FROM TapeRequestState WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE CurType IS REF CURSOR RETURN TapeRequestState%ROWTYPE; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT accessMode, startTime, lastVdqmPingTime, vdqmVolReqId, id, streamMigration, tapeRecall, status \
                     FROM TapeRequestState t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_updateStatementString =
"UPDATE TapeRequestState SET accessMode = :1, startTime = :2, lastVdqmPingTime = :3, vdqmVolReqId = :4, status = :5 WHERE id = :6";

/// SQL statement for type storage
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member streamMigration
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_checkStreamExistStatementString =
"SELECT id FROM Stream WHERE id = :1";

/// SQL update statement for member streamMigration
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_updateStreamStatementString =
"UPDATE TapeRequestState SET streamMigration = :1 WHERE id = :2";

/// SQL existence statement for member tapeRecall
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_checkTapeExistStatementString =
"SELECT id FROM Tape WHERE id = :1";

/// SQL update statement for member tapeRecall
const std::string castor::db::cnv::DbTapeRequestStateCnv::s_updateTapeStatementString =
"UPDATE TapeRequestState SET tapeRecall = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeRequestStateCnv::DbTapeRequestStateCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkStreamExistStatement(0),
  m_updateStreamStatement(0),
  m_checkTapeExistStatement(0),
  m_updateTapeStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeRequestStateCnv::~DbTapeRequestStateCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::reset() throw() {
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
    if(m_checkStreamExistStatement) delete m_checkStreamExistStatement;
    if(m_updateStreamStatement) delete m_updateStreamStatement;
    if(m_checkTapeExistStatement) delete m_checkTapeExistStatement;
    if(m_updateTapeStatement) delete m_updateTapeStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_bulkSelectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkStreamExistStatement = 0;
  m_updateStreamStatement = 0;
  m_checkTapeExistStatement = 0;
  m_updateTapeStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeRequestStateCnv::ObjType() {
  return castor::tape::tapegateway::TapeRequestState::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeRequestStateCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::fillRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     unsigned int type,
                                                     bool endTransaction)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::TapeRequestState* obj = 
    dynamic_cast<castor::tape::tapegateway::TapeRequestState*>(object);
  try {
    switch (type) {
    case castor::OBJ_Stream :
      fillRepStream(obj);
      break;
    case castor::OBJ_Tape :
      fillRepTape(obj);
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
// fillRepStream
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::fillRepStream(castor::tape::tapegateway::TapeRequestState* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->streamMigration()) {
    // Check checkStreamExist statement
    if (0 == m_checkStreamExistStatement) {
      m_checkStreamExistStatement = createStatement(s_checkStreamExistStatementString);
    }
    // retrieve the object from the database
    m_checkStreamExistStatement->setUInt64(1, obj->streamMigration()->id());
    castor::db::IDbResultSet *rset = m_checkStreamExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->streamMigration(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateStreamStatement) {
    m_updateStreamStatement = createStatement(s_updateStreamStatementString);
  }
  // Update local object
  m_updateStreamStatement->setUInt64(1, 0 == obj->streamMigration() ? 0 : obj->streamMigration()->id());
  m_updateStreamStatement->setUInt64(2, obj->id());
  m_updateStreamStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepTape
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::fillRepTape(castor::tape::tapegateway::TapeRequestState* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->tapeRecall()) {
    // Check checkTapeExist statement
    if (0 == m_checkTapeExistStatement) {
      m_checkTapeExistStatement = createStatement(s_checkTapeExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeExistStatement->setUInt64(1, obj->tapeRecall()->id());
    castor::db::IDbResultSet *rset = m_checkTapeExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->tapeRecall(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateTapeStatement) {
    m_updateTapeStatement = createStatement(s_updateTapeStatementString);
  }
  // Update local object
  m_updateTapeStatement->setUInt64(1, 0 == obj->tapeRecall() ? 0 : obj->tapeRecall()->id());
  m_updateTapeStatement->setUInt64(2, obj->id());
  m_updateTapeStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::fillObj(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     unsigned int type,
                                                     bool endTransaction)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::TapeRequestState* obj = 
    dynamic_cast<castor::tape::tapegateway::TapeRequestState*>(object);
  switch (type) {
  case castor::OBJ_Stream :
    fillObjStream(obj);
    break;
  case castor::OBJ_Tape :
    fillObjTape(obj);
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
// fillObjStream
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::fillObjStream(castor::tape::tapegateway::TapeRequestState* obj)
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
  u_signed64 streamMigrationId = rset->getInt64(6);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->streamMigration() &&
      (0 == streamMigrationId ||
       obj->streamMigration()->id() != streamMigrationId)) {
    obj->setStreamMigration(0);
  }
  // Update object or create new one
  if (0 != streamMigrationId) {
    if (0 == obj->streamMigration()) {
      obj->setStreamMigration
        (dynamic_cast<castor::stager::Stream*>
         (cnvSvc()->getObjFromId(streamMigrationId)));
    } else {
      cnvSvc()->updateObj(obj->streamMigration());
    }
  }
}

//------------------------------------------------------------------------------
// fillObjTape
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::fillObjTape(castor::tape::tapegateway::TapeRequestState* obj)
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
  u_signed64 tapeRecallId = rset->getInt64(7);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->tapeRecall() &&
      (0 == tapeRecallId ||
       obj->tapeRecall()->id() != tapeRecallId)) {
    obj->setTapeRecall(0);
  }
  // Update object or create new one
  if (0 != tapeRecallId) {
    if (0 == obj->tapeRecall()) {
      obj->setTapeRecall
        (dynamic_cast<castor::stager::Tape*>
         (cnvSvc()->getObjFromId(tapeRecallId)));
    } else {
      cnvSvc()->updateObj(obj->tapeRecall());
    }
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::createRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool endTransaction,
                                                       unsigned int type)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::TapeRequestState* obj = 
    dynamic_cast<castor::tape::tapegateway::TapeRequestState*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(8, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->accessMode());
    m_insertStatement->setUInt64(2, obj->startTime());
    m_insertStatement->setUInt64(3, obj->lastVdqmPingTime());
    m_insertStatement->setInt(4, obj->vdqmVolReqId());
    m_insertStatement->setUInt64(5, (type == OBJ_Stream && obj->streamMigration() != 0) ? obj->streamMigration()->id() : 0);
    m_insertStatement->setUInt64(6, (type == OBJ_Tape && obj->tapeRecall() != 0) ? obj->tapeRecall()->id() : 0);
    m_insertStatement->setInt(7, (int)obj->status());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(8));
    m_storeTypeStatement->setUInt64(1, obj->id());
    m_storeTypeStatement->setUInt64(2, obj->type());
    m_storeTypeStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try {
      if (endTransaction) cnvSvc()->rollback();
    } catch (castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was : " << std::endl
                    << s_insertStatementString << std::endl
                    << " and parameters' values were :" << std::endl
                    << "  accessMode : " << obj->accessMode() << std::endl
                    << "  startTime : " << obj->startTime() << std::endl
                    << "  lastVdqmPingTime : " << obj->lastVdqmPingTime() << std::endl
                    << "  vdqmVolReqId : " << obj->vdqmVolReqId() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  streamMigration : " << obj->streamMigration() << std::endl
                    << "  tapeRecall : " << obj->tapeRecall() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeRequestStateCnv::bulkCreateRep(castor::IAddress* address,
                                                           std::vector<castor::IObject*> &objects,
                                                           bool endTransaction,
                                                           unsigned int type)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::tape::tapegateway::TapeRequestState*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::tape::tapegateway::TapeRequestState*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(8, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for accessMode
    int* accessModeBuffer = (int*) malloc(nb * sizeof(int));
    if (accessModeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(accessModeBuffer);
    unsigned short* accessModeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (accessModeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(accessModeBufLens);
    for (int i = 0; i < nb; i++) {
      accessModeBuffer[i] = objs[i]->accessMode();
      accessModeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (1, accessModeBuffer, castor::db::DBTYPE_INT, sizeof(accessModeBuffer[0]), accessModeBufLens);
    // build the buffers for startTime
    double* startTimeBuffer = (double*) malloc(nb * sizeof(double));
    if (startTimeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(startTimeBuffer);
    unsigned short* startTimeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (startTimeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(startTimeBufLens);
    for (int i = 0; i < nb; i++) {
      startTimeBuffer[i] = objs[i]->startTime();
      startTimeBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (2, startTimeBuffer, castor::db::DBTYPE_UINT64, sizeof(startTimeBuffer[0]), startTimeBufLens);
    // build the buffers for lastVdqmPingTime
    double* lastVdqmPingTimeBuffer = (double*) malloc(nb * sizeof(double));
    if (lastVdqmPingTimeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(lastVdqmPingTimeBuffer);
    unsigned short* lastVdqmPingTimeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (lastVdqmPingTimeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(lastVdqmPingTimeBufLens);
    for (int i = 0; i < nb; i++) {
      lastVdqmPingTimeBuffer[i] = objs[i]->lastVdqmPingTime();
      lastVdqmPingTimeBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (3, lastVdqmPingTimeBuffer, castor::db::DBTYPE_UINT64, sizeof(lastVdqmPingTimeBuffer[0]), lastVdqmPingTimeBufLens);
    // build the buffers for vdqmVolReqId
    int* vdqmVolReqIdBuffer = (int*) malloc(nb * sizeof(int));
    if (vdqmVolReqIdBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(vdqmVolReqIdBuffer);
    unsigned short* vdqmVolReqIdBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (vdqmVolReqIdBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(vdqmVolReqIdBufLens);
    for (int i = 0; i < nb; i++) {
      vdqmVolReqIdBuffer[i] = objs[i]->vdqmVolReqId();
      vdqmVolReqIdBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (4, vdqmVolReqIdBuffer, castor::db::DBTYPE_INT, sizeof(vdqmVolReqIdBuffer[0]), vdqmVolReqIdBufLens);
    // build the buffers for streamMigration
    double* streamMigrationBuffer = (double*) malloc(nb * sizeof(double));
    if (streamMigrationBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(streamMigrationBuffer);
    unsigned short* streamMigrationBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (streamMigrationBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(streamMigrationBufLens);
    for (int i = 0; i < nb; i++) {
      streamMigrationBuffer[i] = (type == OBJ_Stream && objs[i]->streamMigration() != 0) ? objs[i]->streamMigration()->id() : 0;
      streamMigrationBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (5, streamMigrationBuffer, castor::db::DBTYPE_UINT64, sizeof(streamMigrationBuffer[0]), streamMigrationBufLens);
    // build the buffers for tapeRecall
    double* tapeRecallBuffer = (double*) malloc(nb * sizeof(double));
    if (tapeRecallBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tapeRecallBuffer);
    unsigned short* tapeRecallBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (tapeRecallBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tapeRecallBufLens);
    for (int i = 0; i < nb; i++) {
      tapeRecallBuffer[i] = (type == OBJ_Tape && objs[i]->tapeRecall() != 0) ? objs[i]->tapeRecall()->id() : 0;
      tapeRecallBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (6, tapeRecallBuffer, castor::db::DBTYPE_UINT64, sizeof(tapeRecallBuffer[0]), tapeRecallBufLens);
    // build the buffers for status
    int* statusBuffer = (int*) malloc(nb * sizeof(int));
    if (statusBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(statusBuffer);
    unsigned short* statusBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (statusBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(statusBufLens);
    for (int i = 0; i < nb; i++) {
      statusBuffer[i] = objs[i]->status();
      statusBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (7, statusBuffer, castor::db::DBTYPE_INT, sizeof(statusBuffer[0]), statusBufLens);
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
      (8, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
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
  } catch (castor::exception::SQLError e) {
    // release the buffers
    for (unsigned int i = 0; i < allocMem.size(); i++) {
      free(allocMem[i]);
    }
    // Always try to rollback
    try {
      if (endTransaction) cnvSvc()->rollback();
    } catch (castor::exception::Exception ignored) {}
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
void castor::db::cnv::DbTapeRequestStateCnv::updateRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool endTransaction)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::TapeRequestState* obj = 
    dynamic_cast<castor::tape::tapegateway::TapeRequestState*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->accessMode());
    m_updateStatement->setUInt64(2, obj->startTime());
    m_updateStatement->setUInt64(3, obj->lastVdqmPingTime());
    m_updateStatement->setInt(4, obj->vdqmVolReqId());
    m_updateStatement->setInt(5, (int)obj->status());
    m_updateStatement->setUInt64(6, obj->id());
    m_updateStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try {
      if (endTransaction) cnvSvc()->rollback();
    } catch (castor::exception::Exception ignored) {}
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
void castor::db::cnv::DbTapeRequestStateCnv::deleteRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool endTransaction)
  throw (castor::exception::Exception) {
  castor::tape::tapegateway::TapeRequestState* obj = 
    dynamic_cast<castor::tape::tapegateway::TapeRequestState*>(object);
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
    try {
      if (endTransaction) cnvSvc()->rollback();
    } catch (castor::exception::Exception ignored) {}
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
castor::IObject* castor::db::cnv::DbTapeRequestStateCnv::createObj(castor::IAddress* address)
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
    castor::tape::tapegateway::TapeRequestState* object = new castor::tape::tapegateway::TapeRequestState();
    // Now retrieve and set members
    object->setAccessMode(rset->getInt(1));
    object->setStartTime(rset->getUInt64(2));
    object->setLastVdqmPingTime(rset->getUInt64(3));
    object->setVdqmVolReqId(rset->getInt(4));
    object->setId(rset->getUInt64(5));
    object->setStatus((enum castor::tape::tapegateway::TapeRequestStateCode)rset->getInt(8));
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
castor::db::cnv::DbTapeRequestStateCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::tape::tapegateway::TapeRequestState* object = new castor::tape::tapegateway::TapeRequestState();
      // Now retrieve and set members
      object->setAccessMode(rset->getInt(1));
      object->setStartTime(rset->getUInt64(2));
      object->setLastVdqmPingTime(rset->getUInt64(3));
      object->setVdqmVolReqId(rset->getInt(4));
      object->setId(rset->getUInt64(5));
      object->setStatus((enum castor::tape::tapegateway::TapeRequestStateCode)rset->getInt(8));
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
void castor::db::cnv::DbTapeRequestStateCnv::updateObj(castor::IObject* obj)
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
    castor::tape::tapegateway::TapeRequestState* object = 
      dynamic_cast<castor::tape::tapegateway::TapeRequestState*>(obj);
    object->setAccessMode(rset->getInt(1));
    object->setStartTime(rset->getUInt64(2));
    object->setLastVdqmPingTime(rset->getUInt64(3));
    object->setVdqmVolReqId(rset->getInt(4));
    object->setId(rset->getUInt64(5));
    object->setStatus((enum castor::tape::tapegateway::TapeRequestStateCode)rset->getInt(8));
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


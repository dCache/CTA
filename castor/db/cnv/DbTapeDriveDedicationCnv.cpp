/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
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
#include "DbTapeDriveDedicationCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/VectorAddress.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/exception/OutOfMemory.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDriveDedication.hpp"
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbTapeDriveDedicationCnv>* s_factoryDbTapeDriveDedicationCnv =
  new castor::CnvFactory<castor::db::cnv::DbTapeDriveDedicationCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbTapeDriveDedicationCnv::s_insertStatementString =
"INSERT INTO TapeDriveDedication (clientHost, euid, egid, vid, accessMode, startTime, endTime, reason, id, tapeDrive) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,ids_seq.nextval,:9) RETURNING id INTO :10";

/// SQL statement for request bulk insertion
const std::string castor::db::cnv::DbTapeDriveDedicationCnv::s_bulkInsertStatementString =
"INSERT /* bulk */ INTO TapeDriveDedication (clientHost, euid, egid, vid, accessMode, startTime, endTime, reason, id, tapeDrive) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,ids_seq.nextval,:9) RETURNING id INTO :10";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbTapeDriveDedicationCnv::s_deleteStatementString =
"DELETE FROM TapeDriveDedication WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbTapeDriveDedicationCnv::s_selectStatementString =
"SELECT clientHost, euid, egid, vid, accessMode, startTime, endTime, reason, id, tapeDrive FROM TapeDriveDedication WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbTapeDriveDedicationCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE RecordType IS RECORD (clientHost VARCHAR2(2048), euid NUMBER, egid NUMBER, vid VARCHAR2(2048), accessMode NUMBER, startTime INTEGER, endTime INTEGER, reason VARCHAR2(2048), id INTEGER, tapeDrive INTEGER); \
   TYPE CurType IS REF CURSOR RETURN RecordType; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT clientHost, euid, egid, vid, accessMode, startTime, endTime, reason, id, tapeDrive \
                     FROM TapeDriveDedication t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbTapeDriveDedicationCnv::s_updateStatementString =
"UPDATE TapeDriveDedication SET clientHost = :1, euid = :2, egid = :3, vid = :4, accessMode = :5, startTime = :6, endTime = :7, reason = :8 WHERE id = :9";

/// SQL existence statement for member tapeDrive
const std::string castor::db::cnv::DbTapeDriveDedicationCnv::s_checkTapeDriveExistStatementString =
"SELECT id FROM TapeDrive WHERE id = :1";

/// SQL update statement for member tapeDrive
const std::string castor::db::cnv::DbTapeDriveDedicationCnv::s_updateTapeDriveStatementString =
"UPDATE TapeDriveDedication SET tapeDrive = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeDriveDedicationCnv::DbTapeDriveDedicationCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_bulkInsertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0),
  m_checkTapeDriveExistStatement(0),
  m_updateTapeDriveStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeDriveDedicationCnv::~DbTapeDriveDedicationCnv() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_bulkInsertStatement) delete m_bulkInsertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_bulkSelectStatement) delete m_bulkSelectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_checkTapeDriveExistStatement) delete m_checkTapeDriveExistStatement;
    if(m_updateTapeDriveStatement) delete m_updateTapeDriveStatement;
  } catch (castor::exception::Exception& ignored) {};
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbTapeDriveDedicationCnv::ObjType() {
  return castor::vdqm::TapeDriveDedication::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbTapeDriveDedicationCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveDedicationCnv::fillRep(castor::IAddress*,
                                                        castor::IObject* object,
                                                        unsigned int type,
                                                        bool endTransaction)
   {
  castor::vdqm::TapeDriveDedication* obj = 
    dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
  try {
    switch (type) {
    case castor::OBJ_TapeDrive :
      fillRepTapeDrive(obj);
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
    castor::exception::Exception ex;
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.getMessage().str() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepTapeDrive
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveDedicationCnv::fillRepTapeDrive(castor::vdqm::TapeDriveDedication* obj)
   {
  if (0 != obj->tapeDrive()) {
    // Check checkTapeDriveExist statement
    if (0 == m_checkTapeDriveExistStatement) {
      m_checkTapeDriveExistStatement = createStatement(s_checkTapeDriveExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeDriveExistStatement->setUInt64(1, obj->tapeDrive()->id());
    castor::db::IDbResultSet *rset = m_checkTapeDriveExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->tapeDrive(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateTapeDriveStatement) {
    m_updateTapeDriveStatement = createStatement(s_updateTapeDriveStatementString);
  }
  // Update local object
  m_updateTapeDriveStatement->setUInt64(1, 0 == obj->tapeDrive() ? 0 : obj->tapeDrive()->id());
  m_updateTapeDriveStatement->setUInt64(2, obj->id());
  m_updateTapeDriveStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveDedicationCnv::fillObj(castor::IAddress*,
                                                        castor::IObject* object,
                                                        unsigned int type,
                                                        bool endTransaction)
   {
  castor::vdqm::TapeDriveDedication* obj = 
    dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
  switch (type) {
  case castor::OBJ_TapeDrive :
    fillObjTapeDrive(obj);
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
// fillObjTapeDrive
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveDedicationCnv::fillObjTapeDrive(castor::vdqm::TapeDriveDedication* obj)
   {
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
  u_signed64 tapeDriveId = rset->getInt64(10);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->tapeDrive() &&
      (0 == tapeDriveId ||
       obj->tapeDrive()->id() != tapeDriveId)) {
    obj->tapeDrive()->removeTapeDriveDedication(obj);
    obj->setTapeDrive(0);
  }
  // Update object or create new one
  if (0 != tapeDriveId) {
    if (0 == obj->tapeDrive()) {
      obj->setTapeDrive
        (dynamic_cast<castor::vdqm::TapeDrive*>
         (cnvSvc()->getObjFromId(tapeDriveId, OBJ_TapeDrive)));
    } else {
      cnvSvc()->updateObj(obj->tapeDrive());
    }
    obj->tapeDrive()->addTapeDriveDedication(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveDedicationCnv::createRep(castor::IAddress*,
                                                          castor::IObject* object,
                                                          bool endTransaction,
                                                          unsigned int type)
   {
  castor::vdqm::TapeDriveDedication* obj = 
    dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(10, castor::db::DBTYPE_UINT64);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->clientHost());
    m_insertStatement->setInt(2, obj->euid());
    m_insertStatement->setInt(3, obj->egid());
    m_insertStatement->setString(4, obj->vid());
    m_insertStatement->setInt(5, obj->accessMode());
    m_insertStatement->setUInt64(6, obj->startTime());
    m_insertStatement->setUInt64(7, obj->endTime());
    m_insertStatement->setString(8, obj->reason());
    m_insertStatement->setUInt64(9, (type == OBJ_TapeDrive && obj->tapeDrive() != 0) ? obj->tapeDrive()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(10));
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
                    << "  clientHost : " << obj->clientHost() << std::endl
                    << "  euid : " << obj->euid() << std::endl
                    << "  egid : " << obj->egid() << std::endl
                    << "  vid : " << obj->vid() << std::endl
                    << "  accessMode : " << obj->accessMode() << std::endl
                    << "  startTime : " << obj->startTime() << std::endl
                    << "  endTime : " << obj->endTime() << std::endl
                    << "  reason : " << obj->reason() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  tapeDrive : " << (obj->tapeDrive() ? obj->tapeDrive()->id() : 0) << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveDedicationCnv::bulkCreateRep(castor::IAddress*,
                                                              std::vector<castor::IObject*> &objects,
                                                              bool endTransaction,
                                                              unsigned int type)
   {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::vdqm::TapeDriveDedication*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::vdqm::TapeDriveDedication*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_bulkInsertStatement) {
      m_bulkInsertStatement = createStatement(s_bulkInsertStatementString);
      m_bulkInsertStatement->registerOutParam(10, castor::db::DBTYPE_UINT64);
    }
    // build the buffers for clientHost
    unsigned int clientHostMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->clientHost().length()+1 > clientHostMaxLen)
        clientHostMaxLen = objs[i]->clientHost().length()+1;
    }
    char* clientHostBuffer = (char*) calloc(nb, clientHostMaxLen);
    if (clientHostBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(clientHostBuffer);
    unsigned short* clientHostBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (clientHostBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(clientHostBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(clientHostBuffer+(i*clientHostMaxLen), objs[i]->clientHost().c_str(), clientHostMaxLen);
      clientHostBufLens[i] = objs[i]->clientHost().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (1, clientHostBuffer, castor::db::DBTYPE_STRING, clientHostMaxLen, clientHostBufLens);
    // build the buffers for euid
    int* euidBuffer = (int*) malloc(nb * sizeof(int));
    if (euidBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(euidBuffer);
    unsigned short* euidBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (euidBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(euidBufLens);
    for (int i = 0; i < nb; i++) {
      euidBuffer[i] = objs[i]->euid();
      euidBufLens[i] = sizeof(int);
    }
    m_bulkInsertStatement->setDataBuffer
      (2, euidBuffer, castor::db::DBTYPE_INT, sizeof(euidBuffer[0]), euidBufLens);
    // build the buffers for egid
    int* egidBuffer = (int*) malloc(nb * sizeof(int));
    if (egidBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(egidBuffer);
    unsigned short* egidBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (egidBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(egidBufLens);
    for (int i = 0; i < nb; i++) {
      egidBuffer[i] = objs[i]->egid();
      egidBufLens[i] = sizeof(int);
    }
    m_bulkInsertStatement->setDataBuffer
      (3, egidBuffer, castor::db::DBTYPE_INT, sizeof(egidBuffer[0]), egidBufLens);
    // build the buffers for vid
    unsigned int vidMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->vid().length()+1 > vidMaxLen)
        vidMaxLen = objs[i]->vid().length()+1;
    }
    char* vidBuffer = (char*) calloc(nb, vidMaxLen);
    if (vidBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(vidBuffer);
    unsigned short* vidBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (vidBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(vidBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(vidBuffer+(i*vidMaxLen), objs[i]->vid().c_str(), vidMaxLen);
      vidBufLens[i] = objs[i]->vid().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (4, vidBuffer, castor::db::DBTYPE_STRING, vidMaxLen, vidBufLens);
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
    m_bulkInsertStatement->setDataBuffer
      (5, accessModeBuffer, castor::db::DBTYPE_INT, sizeof(accessModeBuffer[0]), accessModeBufLens);
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
    m_bulkInsertStatement->setDataBuffer
      (6, startTimeBuffer, castor::db::DBTYPE_UINT64, sizeof(startTimeBuffer[0]), startTimeBufLens);
    // build the buffers for endTime
    double* endTimeBuffer = (double*) malloc(nb * sizeof(double));
    if (endTimeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(endTimeBuffer);
    unsigned short* endTimeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (endTimeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(endTimeBufLens);
    for (int i = 0; i < nb; i++) {
      endTimeBuffer[i] = objs[i]->endTime();
      endTimeBufLens[i] = sizeof(double);
    }
    m_bulkInsertStatement->setDataBuffer
      (7, endTimeBuffer, castor::db::DBTYPE_UINT64, sizeof(endTimeBuffer[0]), endTimeBufLens);
    // build the buffers for reason
    unsigned int reasonMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->reason().length()+1 > reasonMaxLen)
        reasonMaxLen = objs[i]->reason().length()+1;
    }
    char* reasonBuffer = (char*) calloc(nb, reasonMaxLen);
    if (reasonBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(reasonBuffer);
    unsigned short* reasonBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (reasonBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(reasonBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(reasonBuffer+(i*reasonMaxLen), objs[i]->reason().c_str(), reasonMaxLen);
      reasonBufLens[i] = objs[i]->reason().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (8, reasonBuffer, castor::db::DBTYPE_STRING, reasonMaxLen, reasonBufLens);
    // build the buffers for tapeDrive
    double* tapeDriveBuffer = (double*) malloc(nb * sizeof(double));
    if (tapeDriveBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tapeDriveBuffer);
    unsigned short* tapeDriveBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (tapeDriveBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tapeDriveBufLens);
    for (int i = 0; i < nb; i++) {
      tapeDriveBuffer[i] = (type == OBJ_TapeDrive && objs[i]->tapeDrive() != 0) ? objs[i]->tapeDrive()->id() : 0;
      tapeDriveBufLens[i] = sizeof(double);
    }
    m_bulkInsertStatement->setDataBuffer
      (9, tapeDriveBuffer, castor::db::DBTYPE_UINT64, sizeof(tapeDriveBuffer[0]), tapeDriveBufLens);
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
      (10, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
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
void castor::db::cnv::DbTapeDriveDedicationCnv::updateRep(castor::IAddress*,
                                                          castor::IObject* object,
                                                          bool endTransaction)
   {
  castor::vdqm::TapeDriveDedication* obj = 
    dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->clientHost());
    m_updateStatement->setInt(2, obj->euid());
    m_updateStatement->setInt(3, obj->egid());
    m_updateStatement->setString(4, obj->vid());
    m_updateStatement->setInt(5, obj->accessMode());
    m_updateStatement->setUInt64(6, obj->startTime());
    m_updateStatement->setUInt64(7, obj->endTime());
    m_updateStatement->setString(8, obj->reason());
    m_updateStatement->setUInt64(9, obj->id());
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
void castor::db::cnv::DbTapeDriveDedicationCnv::deleteRep(castor::IAddress*,
                                                          castor::IObject* object,
                                                          bool endTransaction)
   {
  castor::vdqm::TapeDriveDedication* obj = 
    dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
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
castor::IObject* castor::db::cnv::DbTapeDriveDedicationCnv::createObj(castor::IAddress* address)
   {
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
    castor::vdqm::TapeDriveDedication* object = new castor::vdqm::TapeDriveDedication();
    // Now retrieve and set members
    object->setClientHost(rset->getString(1));
    object->setEuid(rset->getInt(2));
    object->setEgid(rset->getInt(3));
    object->setVid(rset->getString(4));
    object->setAccessMode(rset->getInt(5));
    object->setStartTime(rset->getUInt64(6));
    object->setEndTime(rset->getUInt64(7));
    object->setReason(rset->getString(8));
    object->setId(rset->getUInt64(9));
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
castor::db::cnv::DbTapeDriveDedicationCnv::bulkCreateObj(castor::IAddress* address)
   {
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
      castor::vdqm::TapeDriveDedication* object = new castor::vdqm::TapeDriveDedication();
      // Now retrieve and set members
      object->setClientHost(rset->getString(1));
      object->setEuid(rset->getInt(2));
      object->setEgid(rset->getInt(3));
      object->setVid(rset->getString(4));
      object->setAccessMode(rset->getInt(5));
      object->setStartTime(rset->getUInt64(6));
      object->setEndTime(rset->getUInt64(7));
      object->setReason(rset->getString(8));
      object->setId(rset->getUInt64(9));
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
void castor::db::cnv::DbTapeDriveDedicationCnv::updateObj(castor::IObject* obj)
   {
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
    castor::vdqm::TapeDriveDedication* object = 
      dynamic_cast<castor::vdqm::TapeDriveDedication*>(obj);
    object->setClientHost(rset->getString(1));
    object->setEuid(rset->getInt(2));
    object->setEgid(rset->getInt(3));
    object->setVid(rset->getString(4));
    object->setAccessMode(rset->getInt(5));
    object->setStartTime(rset->getUInt64(6));
    object->setEndTime(rset->getUInt64(7));
    object->setReason(rset->getString(8));
    object->setId(rset->getUInt64(9));
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


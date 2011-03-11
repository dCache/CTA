/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbVolumePriorityCnv.cpp
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
#include "DbVolumePriorityCnv.hpp"
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
#include "castor/vdqm/VolumePriority.hpp"
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbVolumePriorityCnv>* s_factoryDbVolumePriorityCnv =
  new castor::CnvFactory<castor::db::cnv::DbVolumePriorityCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbVolumePriorityCnv::s_insertStatementString =
"INSERT INTO VolumePriority (priority, clientUID, clientGID, clientHost, vid, tpMode, lifespanType, creationTime, modificationTime, id) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,ids_seq.nextval) RETURNING id INTO :10";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbVolumePriorityCnv::s_deleteStatementString =
"DELETE FROM VolumePriority WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbVolumePriorityCnv::s_selectStatementString =
"SELECT priority, clientUID, clientGID, clientHost, vid, tpMode, lifespanType, creationTime, modificationTime, id FROM VolumePriority WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbVolumePriorityCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE CurType IS REF CURSOR RETURN VolumePriority%ROWTYPE; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT priority, clientUID, clientGID, clientHost, vid, tpMode, lifespanType, creationTime, modificationTime, id \
                     FROM VolumePriority t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbVolumePriorityCnv::s_updateStatementString =
"UPDATE VolumePriority SET priority = :1, clientUID = :2, clientGID = :3, clientHost = :4, vid = :5, tpMode = :6, lifespanType = :7, modificationTime = :8 WHERE id = :9";

/// SQL statement for type storage
const std::string castor::db::cnv::DbVolumePriorityCnv::s_storeTypeStatementString =
"INSERT /* VolumePriority class */ INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbVolumePriorityCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbVolumePriorityCnv::DbVolumePriorityCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbVolumePriorityCnv::~DbVolumePriorityCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbVolumePriorityCnv::reset() throw() {
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
  } catch (castor::exception::Exception& ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_bulkSelectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  // Call upper level reset
  this->DbBaseCnv::reset();
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbVolumePriorityCnv::ObjType() {
  return castor::vdqm::VolumePriority::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbVolumePriorityCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbVolumePriorityCnv::fillRep(castor::IAddress*,
                                                   castor::IObject* object,
                                                   unsigned int type,
                                                   bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VolumePriority* obj = 
    dynamic_cast<castor::vdqm::VolumePriority*>(object);
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
void castor::db::cnv::DbVolumePriorityCnv::fillObj(castor::IAddress*,
                                                   castor::IObject* object,
                                                   unsigned int type,
                                                   bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VolumePriority* obj = 
    dynamic_cast<castor::vdqm::VolumePriority*>(object);
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
void castor::db::cnv::DbVolumePriorityCnv::createRep(castor::IAddress*,
                                                     castor::IObject* object,
                                                     bool endTransaction,
                                                     unsigned int)
  throw (castor::exception::Exception) {
  castor::vdqm::VolumePriority* obj = 
    dynamic_cast<castor::vdqm::VolumePriority*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(10, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->priority());
    m_insertStatement->setInt(2, obj->clientUID());
    m_insertStatement->setInt(3, obj->clientGID());
    m_insertStatement->setString(4, obj->clientHost());
    m_insertStatement->setString(5, obj->vid());
    m_insertStatement->setInt(6, obj->tpMode());
    m_insertStatement->setInt(7, obj->lifespanType());
    m_insertStatement->setInt(8, time(0));
    m_insertStatement->setUInt64(9, obj->modificationTime());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(10));
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
                    << "  priority : " << obj->priority() << std::endl
                    << "  clientUID : " << obj->clientUID() << std::endl
                    << "  clientGID : " << obj->clientGID() << std::endl
                    << "  clientHost : " << obj->clientHost() << std::endl
                    << "  vid : " << obj->vid() << std::endl
                    << "  tpMode : " << obj->tpMode() << std::endl
                    << "  lifespanType : " << obj->lifespanType() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  modificationTime : " << obj->modificationTime() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbVolumePriorityCnv::bulkCreateRep(castor::IAddress*,
                                                         std::vector<castor::IObject*> &objects,
                                                         bool endTransaction,
                                                         unsigned int)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::vdqm::VolumePriority*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::vdqm::VolumePriority*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(10, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for priority
    int* priorityBuffer = (int*) malloc(nb * sizeof(int));
    if (priorityBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(priorityBuffer);
    unsigned short* priorityBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (priorityBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(priorityBufLens);
    for (int i = 0; i < nb; i++) {
      priorityBuffer[i] = objs[i]->priority();
      priorityBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (1, priorityBuffer, castor::db::DBTYPE_INT, sizeof(priorityBuffer[0]), priorityBufLens);
    // build the buffers for clientUID
    int* clientUIDBuffer = (int*) malloc(nb * sizeof(int));
    if (clientUIDBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(clientUIDBuffer);
    unsigned short* clientUIDBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (clientUIDBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(clientUIDBufLens);
    for (int i = 0; i < nb; i++) {
      clientUIDBuffer[i] = objs[i]->clientUID();
      clientUIDBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (2, clientUIDBuffer, castor::db::DBTYPE_INT, sizeof(clientUIDBuffer[0]), clientUIDBufLens);
    // build the buffers for clientGID
    int* clientGIDBuffer = (int*) malloc(nb * sizeof(int));
    if (clientGIDBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(clientGIDBuffer);
    unsigned short* clientGIDBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (clientGIDBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(clientGIDBufLens);
    for (int i = 0; i < nb; i++) {
      clientGIDBuffer[i] = objs[i]->clientGID();
      clientGIDBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (3, clientGIDBuffer, castor::db::DBTYPE_INT, sizeof(clientGIDBuffer[0]), clientGIDBufLens);
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
    m_insertStatement->setDataBuffer
      (4, clientHostBuffer, castor::db::DBTYPE_STRING, clientHostMaxLen, clientHostBufLens);
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
    m_insertStatement->setDataBuffer
      (5, vidBuffer, castor::db::DBTYPE_STRING, vidMaxLen, vidBufLens);
    // build the buffers for tpMode
    int* tpModeBuffer = (int*) malloc(nb * sizeof(int));
    if (tpModeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tpModeBuffer);
    unsigned short* tpModeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (tpModeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tpModeBufLens);
    for (int i = 0; i < nb; i++) {
      tpModeBuffer[i] = objs[i]->tpMode();
      tpModeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (6, tpModeBuffer, castor::db::DBTYPE_INT, sizeof(tpModeBuffer[0]), tpModeBufLens);
    // build the buffers for lifespanType
    int* lifespanTypeBuffer = (int*) malloc(nb * sizeof(int));
    if (lifespanTypeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(lifespanTypeBuffer);
    unsigned short* lifespanTypeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (lifespanTypeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(lifespanTypeBufLens);
    for (int i = 0; i < nb; i++) {
      lifespanTypeBuffer[i] = objs[i]->lifespanType();
      lifespanTypeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (7, lifespanTypeBuffer, castor::db::DBTYPE_INT, sizeof(lifespanTypeBuffer[0]), lifespanTypeBufLens);
    // build the buffers for creationTime
    double* creationTimeBuffer = (double*) malloc(nb * sizeof(double));
    if (creationTimeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(creationTimeBuffer);
    unsigned short* creationTimeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (creationTimeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(creationTimeBufLens);
    for (int i = 0; i < nb; i++) {
      creationTimeBuffer[i] = time(0);
      creationTimeBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (8, creationTimeBuffer, castor::db::DBTYPE_UINT64, sizeof(creationTimeBuffer[0]), creationTimeBufLens);
    // build the buffers for modificationTime
    double* modificationTimeBuffer = (double*) malloc(nb * sizeof(double));
    if (modificationTimeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(modificationTimeBuffer);
    unsigned short* modificationTimeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (modificationTimeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(modificationTimeBufLens);
    for (int i = 0; i < nb; i++) {
      modificationTimeBuffer[i] = objs[i]->modificationTime();
      modificationTimeBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (9, modificationTimeBuffer, castor::db::DBTYPE_UINT64, sizeof(modificationTimeBuffer[0]), modificationTimeBufLens);
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
      (10, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
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
void castor::db::cnv::DbVolumePriorityCnv::updateRep(castor::IAddress*,
                                                     castor::IObject* object,
                                                     bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VolumePriority* obj = 
    dynamic_cast<castor::vdqm::VolumePriority*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->priority());
    m_updateStatement->setInt(2, obj->clientUID());
    m_updateStatement->setInt(3, obj->clientGID());
    m_updateStatement->setString(4, obj->clientHost());
    m_updateStatement->setString(5, obj->vid());
    m_updateStatement->setInt(6, obj->tpMode());
    m_updateStatement->setInt(7, obj->lifespanType());
    m_updateStatement->setUInt64(8, obj->modificationTime());
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
void castor::db::cnv::DbVolumePriorityCnv::deleteRep(castor::IAddress*,
                                                     castor::IObject* object,
                                                     bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VolumePriority* obj = 
    dynamic_cast<castor::vdqm::VolumePriority*>(object);
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
castor::IObject* castor::db::cnv::DbVolumePriorityCnv::createObj(castor::IAddress* address)
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
    castor::vdqm::VolumePriority* object = new castor::vdqm::VolumePriority();
    // Now retrieve and set members
    object->setPriority(rset->getInt(1));
    object->setClientUID(rset->getInt(2));
    object->setClientGID(rset->getInt(3));
    object->setClientHost(rset->getString(4));
    object->setVid(rset->getString(5));
    object->setTpMode(rset->getInt(6));
    object->setLifespanType(rset->getInt(7));
    object->setCreationTime(rset->getUInt64(8));
    object->setModificationTime(rset->getUInt64(9));
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
castor::db::cnv::DbVolumePriorityCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::vdqm::VolumePriority* object = new castor::vdqm::VolumePriority();
      // Now retrieve and set members
      object->setPriority(rset->getInt(1));
      object->setClientUID(rset->getInt(2));
      object->setClientGID(rset->getInt(3));
      object->setClientHost(rset->getString(4));
      object->setVid(rset->getString(5));
      object->setTpMode(rset->getInt(6));
      object->setLifespanType(rset->getInt(7));
      object->setCreationTime(rset->getUInt64(8));
      object->setModificationTime(rset->getUInt64(9));
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
void castor::db::cnv::DbVolumePriorityCnv::updateObj(castor::IObject* obj)
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
    castor::vdqm::VolumePriority* object = 
      dynamic_cast<castor::vdqm::VolumePriority*>(obj);
    object->setPriority(rset->getInt(1));
    object->setClientUID(rset->getInt(2));
    object->setClientGID(rset->getInt(3));
    object->setClientHost(rset->getString(4));
    object->setVid(rset->getString(5));
    object->setTpMode(rset->getInt(6));
    object->setLifespanType(rset->getInt(7));
    object->setCreationTime(rset->getUInt64(8));
    object->setModificationTime(rset->getUInt64(9));
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


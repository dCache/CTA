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
#include "DbTapeDriveCompatibilityCnv.hpp"
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
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeDriveCompatibility.hpp"
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbTapeDriveCompatibilityCnv>* s_factoryDbTapeDriveCompatibilityCnv =
  new castor::CnvFactory<castor::db::cnv::DbTapeDriveCompatibilityCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_insertStatementString =
"INSERT INTO TapeDriveCompatibility (tapeDriveModel, priorityLevel, id, tapeAccessSpecification) VALUES (:1,:2,ids_seq.nextval,:3) RETURNING id INTO :4";

/// SQL statement for request bulk insertion
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_bulkInsertStatementString =
"INSERT /* bulk */ INTO TapeDriveCompatibility (tapeDriveModel, priorityLevel, id, tapeAccessSpecification) VALUES (:1,:2,ids_seq.nextval,:3) RETURNING id INTO :4";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_deleteStatementString =
"DELETE FROM TapeDriveCompatibility WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_selectStatementString =
"SELECT tapeDriveModel, priorityLevel, id, tapeAccessSpecification FROM TapeDriveCompatibility WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE RecordType IS RECORD (tapeDriveModel VARCHAR2(2048), priorityLevel NUMBER, id INTEGER, tapeAccessSpecification INTEGER); \
   TYPE CurType IS REF CURSOR RETURN RecordType; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT tapeDriveModel, priorityLevel, id, tapeAccessSpecification \
                     FROM TapeDriveCompatibility t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_updateStatementString =
"UPDATE TapeDriveCompatibility SET tapeDriveModel = :1, priorityLevel = :2 WHERE id = :3";

/// SQL existence statement for member tapeAccessSpecification
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_checkTapeAccessSpecificationExistStatementString =
"SELECT id FROM TapeAccessSpecification WHERE id = :1";

/// SQL update statement for member tapeAccessSpecification
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_updateTapeAccessSpecificationStatementString =
"UPDATE TapeDriveCompatibility SET tapeAccessSpecification = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeDriveCompatibilityCnv::DbTapeDriveCompatibilityCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_bulkInsertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0),
  m_checkTapeAccessSpecificationExistStatement(0),
  m_updateTapeAccessSpecificationStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeDriveCompatibilityCnv::~DbTapeDriveCompatibilityCnv() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_bulkInsertStatement) delete m_bulkInsertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_bulkSelectStatement) delete m_bulkSelectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_checkTapeAccessSpecificationExistStatement) delete m_checkTapeAccessSpecificationExistStatement;
    if(m_updateTapeAccessSpecificationStatement) delete m_updateTapeAccessSpecificationStatement;
  } catch (castor::exception::Exception& ignored) {};
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbTapeDriveCompatibilityCnv::ObjType() {
  return castor::vdqm::TapeDriveCompatibility::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbTapeDriveCompatibilityCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::fillRep(castor::IAddress*,
                                                           castor::IObject* object,
                                                           unsigned int type,
                                                           bool endTransaction)
   {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  try {
    switch (type) {
    case castor::OBJ_TapeAccessSpecification :
      fillRepTapeAccessSpecification(obj);
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
// fillRepTapeAccessSpecification
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::fillRepTapeAccessSpecification(castor::vdqm::TapeDriveCompatibility* obj)
   {
  if (0 != obj->tapeAccessSpecification()) {
    // Check checkTapeAccessSpecificationExist statement
    if (0 == m_checkTapeAccessSpecificationExistStatement) {
      m_checkTapeAccessSpecificationExistStatement = createStatement(s_checkTapeAccessSpecificationExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeAccessSpecificationExistStatement->setUInt64(1, obj->tapeAccessSpecification()->id());
    castor::db::IDbResultSet *rset = m_checkTapeAccessSpecificationExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->tapeAccessSpecification(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateTapeAccessSpecificationStatement) {
    m_updateTapeAccessSpecificationStatement = createStatement(s_updateTapeAccessSpecificationStatementString);
  }
  // Update local object
  m_updateTapeAccessSpecificationStatement->setUInt64(1, 0 == obj->tapeAccessSpecification() ? 0 : obj->tapeAccessSpecification()->id());
  m_updateTapeAccessSpecificationStatement->setUInt64(2, obj->id());
  m_updateTapeAccessSpecificationStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::fillObj(castor::IAddress*,
                                                           castor::IObject* object,
                                                           unsigned int type,
                                                           bool endTransaction)
   {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  switch (type) {
  case castor::OBJ_TapeAccessSpecification :
    fillObjTapeAccessSpecification(obj);
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
// fillObjTapeAccessSpecification
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::fillObjTapeAccessSpecification(castor::vdqm::TapeDriveCompatibility* obj)
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
  u_signed64 tapeAccessSpecificationId = rset->getInt64(4);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->tapeAccessSpecification() &&
      (0 == tapeAccessSpecificationId ||
       obj->tapeAccessSpecification()->id() != tapeAccessSpecificationId)) {
    obj->setTapeAccessSpecification(0);
  }
  // Update object or create new one
  if (0 != tapeAccessSpecificationId) {
    if (0 == obj->tapeAccessSpecification()) {
      obj->setTapeAccessSpecification
        (dynamic_cast<castor::vdqm::TapeAccessSpecification*>
         (cnvSvc()->getObjFromId(tapeAccessSpecificationId, OBJ_TapeAccessSpecification)));
    } else {
      cnvSvc()->updateObj(obj->tapeAccessSpecification());
    }
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::createRep(castor::IAddress*,
                                                             castor::IObject* object,
                                                             bool endTransaction,
                                                             unsigned int type)
   {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(4, castor::db::DBTYPE_UINT64);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->tapeDriveModel());
    m_insertStatement->setInt(2, obj->priorityLevel());
    m_insertStatement->setUInt64(3, (type == OBJ_TapeAccessSpecification && obj->tapeAccessSpecification() != 0) ? obj->tapeAccessSpecification()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(4));
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
                    << "  tapeDriveModel : " << obj->tapeDriveModel() << std::endl
                    << "  priorityLevel : " << obj->priorityLevel() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  tapeAccessSpecification : " << (obj->tapeAccessSpecification() ? obj->tapeAccessSpecification()->id() : 0) << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::bulkCreateRep(castor::IAddress*,
                                                                 std::vector<castor::IObject*> &objects,
                                                                 bool endTransaction,
                                                                 unsigned int type)
   {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::vdqm::TapeDriveCompatibility*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_bulkInsertStatement) {
      m_bulkInsertStatement = createStatement(s_bulkInsertStatementString);
      m_bulkInsertStatement->registerOutParam(4, castor::db::DBTYPE_UINT64);
    }
    // build the buffers for tapeDriveModel
    unsigned int tapeDriveModelMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->tapeDriveModel().length()+1 > tapeDriveModelMaxLen)
        tapeDriveModelMaxLen = objs[i]->tapeDriveModel().length()+1;
    }
    char* tapeDriveModelBuffer = (char*) calloc(nb, tapeDriveModelMaxLen);
    if (tapeDriveModelBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tapeDriveModelBuffer);
    unsigned short* tapeDriveModelBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (tapeDriveModelBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tapeDriveModelBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(tapeDriveModelBuffer+(i*tapeDriveModelMaxLen), objs[i]->tapeDriveModel().c_str(), tapeDriveModelMaxLen);
      tapeDriveModelBufLens[i] = objs[i]->tapeDriveModel().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (1, tapeDriveModelBuffer, castor::db::DBTYPE_STRING, tapeDriveModelMaxLen, tapeDriveModelBufLens);
    // build the buffers for priorityLevel
    int* priorityLevelBuffer = (int*) malloc(nb * sizeof(int));
    if (priorityLevelBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(priorityLevelBuffer);
    unsigned short* priorityLevelBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (priorityLevelBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(priorityLevelBufLens);
    for (int i = 0; i < nb; i++) {
      priorityLevelBuffer[i] = objs[i]->priorityLevel();
      priorityLevelBufLens[i] = sizeof(int);
    }
    m_bulkInsertStatement->setDataBuffer
      (2, priorityLevelBuffer, castor::db::DBTYPE_INT, sizeof(priorityLevelBuffer[0]), priorityLevelBufLens);
    // build the buffers for tapeAccessSpecification
    double* tapeAccessSpecificationBuffer = (double*) malloc(nb * sizeof(double));
    if (tapeAccessSpecificationBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tapeAccessSpecificationBuffer);
    unsigned short* tapeAccessSpecificationBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (tapeAccessSpecificationBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(tapeAccessSpecificationBufLens);
    for (int i = 0; i < nb; i++) {
      tapeAccessSpecificationBuffer[i] = (type == OBJ_TapeAccessSpecification && objs[i]->tapeAccessSpecification() != 0) ? objs[i]->tapeAccessSpecification()->id() : 0;
      tapeAccessSpecificationBufLens[i] = sizeof(double);
    }
    m_bulkInsertStatement->setDataBuffer
      (3, tapeAccessSpecificationBuffer, castor::db::DBTYPE_UINT64, sizeof(tapeAccessSpecificationBuffer[0]), tapeAccessSpecificationBufLens);
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
      (4, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
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
void castor::db::cnv::DbTapeDriveCompatibilityCnv::updateRep(castor::IAddress*,
                                                             castor::IObject* object,
                                                             bool endTransaction)
   {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->tapeDriveModel());
    m_updateStatement->setInt(2, obj->priorityLevel());
    m_updateStatement->setUInt64(3, obj->id());
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
void castor::db::cnv::DbTapeDriveCompatibilityCnv::deleteRep(castor::IAddress*,
                                                             castor::IObject* object,
                                                             bool endTransaction)
   {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
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
castor::IObject* castor::db::cnv::DbTapeDriveCompatibilityCnv::createObj(castor::IAddress* address)
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
    castor::vdqm::TapeDriveCompatibility* object = new castor::vdqm::TapeDriveCompatibility();
    // Now retrieve and set members
    object->setTapeDriveModel(rset->getString(1));
    object->setPriorityLevel(rset->getInt(2));
    object->setId(rset->getUInt64(3));
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
castor::db::cnv::DbTapeDriveCompatibilityCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::vdqm::TapeDriveCompatibility* object = new castor::vdqm::TapeDriveCompatibility();
      // Now retrieve and set members
      object->setTapeDriveModel(rset->getString(1));
      object->setPriorityLevel(rset->getInt(2));
      object->setId(rset->getUInt64(3));
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
void castor::db::cnv::DbTapeDriveCompatibilityCnv::updateObj(castor::IObject* obj)
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
    castor::vdqm::TapeDriveCompatibility* object = 
      dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(obj);
    object->setTapeDriveModel(rset->getString(1));
    object->setPriorityLevel(rset->getInt(2));
    object->setId(rset->getUInt64(3));
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


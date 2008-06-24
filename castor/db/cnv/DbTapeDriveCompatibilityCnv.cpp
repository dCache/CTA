/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbTapeDriveCompatibilityCnv.cpp
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
#include "DbTapeDriveCompatibilityCnv.hpp"
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
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeDriveCompatibility.hpp"
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

/// SQL statement for request deletion
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_deleteStatementString =
"DELETE FROM TapeDriveCompatibility WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_selectStatementString =
"SELECT tapeDriveModel, priorityLevel, id, tapeAccessSpecification FROM TapeDriveCompatibility WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_updateStatementString =
"UPDATE TapeDriveCompatibility SET tapeDriveModel = :1, priorityLevel = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbTapeDriveCompatibilityCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

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
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkTapeAccessSpecificationExistStatement(0),
  m_updateTapeAccessSpecificationStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeDriveCompatibilityCnv::~DbTapeDriveCompatibilityCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_checkTapeAccessSpecificationExistStatement) delete m_checkTapeAccessSpecificationExistStatement;
    if(m_updateTapeAccessSpecificationStatement) delete m_updateTapeAccessSpecificationStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkTapeAccessSpecificationExistStatement = 0;
  m_updateTapeAccessSpecificationStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeDriveCompatibilityCnv::ObjType() {
  return castor::vdqm::TapeDriveCompatibility::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeDriveCompatibilityCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::fillRep(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           unsigned int type,
                                                           bool endTransaction)
  throw (castor::exception::Exception) {
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
  } catch (castor::exception::SQLError e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.getMessage().str() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepTapeAccessSpecification
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::fillRepTapeAccessSpecification(castor::vdqm::TapeDriveCompatibility* obj)
  throw (castor::exception::Exception) {
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
void castor::db::cnv::DbTapeDriveCompatibilityCnv::fillObj(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           unsigned int type,
                                                           bool endTransaction)
  throw (castor::exception::Exception) {
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
         (cnvSvc()->getObjFromId(tapeAccessSpecificationId)));
    } else {
      cnvSvc()->updateObj(obj->tapeAccessSpecification());
    }
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::createRep(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             bool endTransaction,
                                                             unsigned int type)
  throw (castor::exception::Exception) {
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
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->tapeDriveModel());
    m_insertStatement->setInt(2, obj->priorityLevel());
    m_insertStatement->setUInt64(3, (type == OBJ_TapeAccessSpecification && obj->tapeAccessSpecification() != 0) ? obj->tapeAccessSpecification()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(4));
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
                    << "  tapeDriveModel : " << obj->tapeDriveModel() << std::endl
                    << "  priorityLevel : " << obj->priorityLevel() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  tapeAccessSpecification : " << obj->tapeAccessSpecification() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::bulkCreateRep(castor::IAddress* address,
                                                                 std::vector<castor::IObject*> &objects,
                                                                 bool endTransaction,
                                                                 unsigned int type)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::vdqm::TapeDriveCompatibility*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(objects[i]));
  }
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(4, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for tapeDriveModel
    unsigned int tapeDriveModelMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->tapeDriveModel().length()+1 > tapeDriveModelMaxLen)
        tapeDriveModelMaxLen = objs[i]->tapeDriveModel().length()+1;
    }
    char* tapeDriveModelBuffer = (char*) calloc(nb, tapeDriveModelMaxLen);
    unsigned short* tapeDriveModelBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      strncpy(tapeDriveModelBuffer+(i*tapeDriveModelMaxLen), objs[i]->tapeDriveModel().c_str(), tapeDriveModelMaxLen);
      tapeDriveModelBufLens[i] = objs[i]->tapeDriveModel().length()+1; // + 1 for the trailing \0
    }
    m_insertStatement->setDataBuffer
      (1, tapeDriveModelBuffer, DBTYPE_STRING, tapeDriveModelMaxLen, tapeDriveModelBufLens);
    // build the buffers for priorityLevel
    int* priorityLevelBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* priorityLevelBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      priorityLevelBuffer[i] = objs[i]->priorityLevel();
      priorityLevelBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (2, priorityLevelBuffer, DBTYPE_INT, sizeof(priorityLevelBuffer[0]), priorityLevelBufLens);
    // build the buffers for tapeAccessSpecification
    double* tapeAccessSpecificationBuffer = (double*) malloc(nb * sizeof(double));
    unsigned short* tapeAccessSpecificationBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      tapeAccessSpecificationBuffer[i] = (type == OBJ_TapeAccessSpecification && objs[i]->tapeAccessSpecification() != 0) ? objs[i]->tapeAccessSpecification()->id() : 0;
      tapeAccessSpecificationBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (3, tapeAccessSpecificationBuffer, DBTYPE_UINT64, sizeof(tapeAccessSpecificationBuffer[0]), tapeAccessSpecificationBufLens);
    // build the buffers for returned ids
    double* idBuffer = (double*) calloc(nb, sizeof(double));
    unsigned short* idBufLens = (unsigned short*) calloc(nb, sizeof(unsigned short));
    m_insertStatement->setDataBuffer
      (4, idBuffer, DBTYPE_UINT64, sizeof(double), idBufLens);
    m_insertStatement->execute(nb);
    for (int i = 0; i < nb; i++) {
      objects[i]->setId((u_signed64)idBuffer[i]);
    }
    // release the buffers for tapeDriveModel
    free(tapeDriveModelBuffer);
    free(tapeDriveModelBufLens);
    // release the buffers for priorityLevel
    free(priorityLevelBuffer);
    free(priorityLevelBufLens);
    // release the buffers for tapeAccessSpecification
    free(tapeAccessSpecificationBuffer);
    free(tapeAccessSpecificationBufLens);
    // reuse idBuffer for bulk insertion into Id2Type
    m_storeTypeStatement->setDataBuffer
      (1, idBuffer, DBTYPE_UINT64, sizeof(idBuffer[0]), idBufLens);
    // build the buffers for type
    int* typeBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* typeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      typeBuffer[i] = objs[i]->type();
      typeBufLens[i] = sizeof(int);
    }
    m_storeTypeStatement->setDataBuffer
      (2, typeBuffer, DBTYPE_INT, sizeof(typeBuffer[0]), typeBufLens);
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
void castor::db::cnv::DbTapeDriveCompatibilityCnv::updateRep(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             bool endTransaction)
  throw (castor::exception::Exception) {
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
void castor::db::cnv::DbTapeDriveCompatibilityCnv::deleteRep(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
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
castor::IObject* castor::db::cnv::DbTapeDriveCompatibilityCnv::createObj(castor::IAddress* address)
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
    castor::vdqm::TapeDriveCompatibility* object = new castor::vdqm::TapeDriveCompatibility();
    // Now retrieve and set members
    object->setTapeDriveModel(rset->getString(1));
    object->setPriorityLevel(rset->getInt(2));
    object->setId(rset->getUInt64(3));
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
// updateObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeDriveCompatibilityCnv::updateObj(castor::IObject* obj)
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
    castor::vdqm::TapeDriveCompatibility* object = 
      dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(obj);
    object->setTapeDriveModel(rset->getString(1));
    object->setPriorityLevel(rset->getInt(2));
    object->setId(rset->getUInt64(3));
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


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbVdqmTapeCnv.cpp
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
#include "DbVdqmTapeCnv.hpp"
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
#include "castor/vdqm/VdqmTape.hpp"
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbVdqmTapeCnv>* s_factoryDbVdqmTapeCnv =
  new castor::CnvFactory<castor::db::cnv::DbVdqmTapeCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbVdqmTapeCnv::s_insertStatementString =
"INSERT INTO VdqmTape (vid, id) VALUES (:1,ids_seq.nextval) RETURNING id INTO :2";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbVdqmTapeCnv::s_deleteStatementString =
"DELETE FROM VdqmTape WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbVdqmTapeCnv::s_selectStatementString =
"SELECT vid, id FROM VdqmTape WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbVdqmTapeCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE RecordType IS RECORD (vid VARCHAR2(2048), id INTEGER); \
   TYPE CurType IS REF CURSOR RETURN RecordType; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT vid, id \
                     FROM VdqmTape t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbVdqmTapeCnv::s_updateStatementString =
"UPDATE VdqmTape SET vid = :1 WHERE id = :2";

/// SQL statement for type storage
const std::string castor::db::cnv::DbVdqmTapeCnv::s_storeTypeStatementString =
"INSERT /* VdqmTape class */ INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbVdqmTapeCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbVdqmTapeCnv::DbVdqmTapeCnv(castor::ICnvSvc* cnvSvc) :
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
castor::db::cnv::DbVdqmTapeCnv::~DbVdqmTapeCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::reset() throw() {
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
unsigned int castor::db::cnv::DbVdqmTapeCnv::ObjType() {
  return castor::vdqm::VdqmTape::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbVdqmTapeCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::fillRep(castor::IAddress*,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VdqmTape* obj = 
    dynamic_cast<castor::vdqm::VdqmTape*>(object);
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
void castor::db::cnv::DbVdqmTapeCnv::fillObj(castor::IAddress*,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VdqmTape* obj = 
    dynamic_cast<castor::vdqm::VdqmTape*>(object);
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
void castor::db::cnv::DbVdqmTapeCnv::createRep(castor::IAddress*,
                                               castor::IObject* object,
                                               bool endTransaction,
                                               unsigned int)
  throw (castor::exception::Exception) {
  castor::vdqm::VdqmTape* obj = 
    dynamic_cast<castor::vdqm::VdqmTape*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(2, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->vid());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(2));
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
                    << "  vid : " << obj->vid() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::bulkCreateRep(castor::IAddress*,
                                                   std::vector<castor::IObject*> &objects,
                                                   bool endTransaction,
                                                   unsigned int)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::vdqm::VdqmTape*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::vdqm::VdqmTape*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(2, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
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
      (1, vidBuffer, castor::db::DBTYPE_STRING, vidMaxLen, vidBufLens);
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
      (2, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
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
void castor::db::cnv::DbVdqmTapeCnv::updateRep(castor::IAddress*,
                                               castor::IObject* object,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VdqmTape* obj = 
    dynamic_cast<castor::vdqm::VdqmTape*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->vid());
    m_updateStatement->setUInt64(2, obj->id());
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
void castor::db::cnv::DbVdqmTapeCnv::deleteRep(castor::IAddress*,
                                               castor::IObject* object,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VdqmTape* obj = 
    dynamic_cast<castor::vdqm::VdqmTape*>(object);
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
castor::IObject* castor::db::cnv::DbVdqmTapeCnv::createObj(castor::IAddress* address)
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
    castor::vdqm::VdqmTape* object = new castor::vdqm::VdqmTape();
    // Now retrieve and set members
    object->setVid(rset->getString(1));
    object->setId(rset->getUInt64(2));
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
castor::db::cnv::DbVdqmTapeCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::vdqm::VdqmTape* object = new castor::vdqm::VdqmTape();
      // Now retrieve and set members
      object->setVid(rset->getString(1));
      object->setId(rset->getUInt64(2));
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
void castor::db::cnv::DbVdqmTapeCnv::updateObj(castor::IObject* obj)
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
    castor::vdqm::VdqmTape* object = 
      dynamic_cast<castor::vdqm::VdqmTape*>(obj);
    object->setVid(rset->getString(1));
    object->setId(rset->getUInt64(2));
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


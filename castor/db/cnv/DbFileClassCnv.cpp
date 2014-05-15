/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbFileClassCnv.cpp
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
#include "DbFileClassCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/VectorAddress.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/exception/OutOfMemory.hpp"
#include "castor/stager/FileClass.hpp"
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbFileClassCnv>* s_factoryDbFileClassCnv =
  new castor::CnvFactory<castor::db::cnv::DbFileClassCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbFileClassCnv::s_insertStatementString =
"INSERT INTO FileClass (name, nbCopies, classId, id) VALUES (:1,:2,:3,ids_seq.nextval) RETURNING id INTO :4";

/// SQL statement for request bulk insertion
const std::string castor::db::cnv::DbFileClassCnv::s_bulkInsertStatementString =
"INSERT /* bulk */ INTO FileClass (name, nbCopies, classId, id) VALUES (:1,:2,:3,ids_seq.nextval) RETURNING id INTO :4";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbFileClassCnv::s_deleteStatementString =
"DELETE FROM FileClass WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbFileClassCnv::s_selectStatementString =
"SELECT name, nbCopies, classId, id FROM FileClass WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbFileClassCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE RecordType IS RECORD (name VARCHAR2(2048), nbCopies NUMBER, classId INTEGER, id INTEGER); \
   TYPE CurType IS REF CURSOR RETURN RecordType; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT name, nbCopies, classId, id \
                     FROM FileClass t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbFileClassCnv::s_updateStatementString =
"UPDATE FileClass SET name = :1, nbCopies = :2, classId = :3 WHERE id = :4";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbFileClassCnv::DbFileClassCnv(castor::ICnvSvc* cnvSvc) :
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
castor::db::cnv::DbFileClassCnv::~DbFileClassCnv() throw() {
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
unsigned int castor::db::cnv::DbFileClassCnv::ObjType() {
  return castor::stager::FileClass::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbFileClassCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbFileClassCnv::fillRep(castor::IAddress*,
                                              castor::IObject* object,
                                              unsigned int type,
                                              bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::FileClass* obj = 
    dynamic_cast<castor::stager::FileClass*>(object);
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
    castor::exception::Exception ex;
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.getMessage().str() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbFileClassCnv::fillObj(castor::IAddress*,
                                              castor::IObject* object,
                                              unsigned int type,
                                              bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::FileClass* obj = 
    dynamic_cast<castor::stager::FileClass*>(object);
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
void castor::db::cnv::DbFileClassCnv::createRep(castor::IAddress*,
                                                castor::IObject* object,
                                                bool endTransaction,
                                                unsigned int)
  throw (castor::exception::Exception) {
  castor::stager::FileClass* obj = 
    dynamic_cast<castor::stager::FileClass*>(object);
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
    m_insertStatement->setString(1, obj->name());
    m_insertStatement->setInt(2, obj->nbCopies());
    m_insertStatement->setUInt64(3, obj->classId());
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
                    << "  name : " << obj->name() << std::endl
                    << "  nbCopies : " << obj->nbCopies() << std::endl
                    << "  classId : " << obj->classId() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbFileClassCnv::bulkCreateRep(castor::IAddress*,
                                                    std::vector<castor::IObject*> &objects,
                                                    bool endTransaction,
                                                    unsigned int)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::stager::FileClass*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::stager::FileClass*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_bulkInsertStatement) {
      m_bulkInsertStatement = createStatement(s_bulkInsertStatementString);
      m_bulkInsertStatement->registerOutParam(4, castor::db::DBTYPE_UINT64);
    }
    // build the buffers for name
    unsigned int nameMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->name().length()+1 > nameMaxLen)
        nameMaxLen = objs[i]->name().length()+1;
    }
    char* nameBuffer = (char*) calloc(nb, nameMaxLen);
    if (nameBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(nameBuffer);
    unsigned short* nameBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (nameBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(nameBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(nameBuffer+(i*nameMaxLen), objs[i]->name().c_str(), nameMaxLen);
      nameBufLens[i] = objs[i]->name().length()+1; // + 1 for the trailing \0
    }
    m_bulkInsertStatement->setDataBuffer
      (1, nameBuffer, castor::db::DBTYPE_STRING, nameMaxLen, nameBufLens);
    // build the buffers for nbCopies
    int* nbCopiesBuffer = (int*) malloc(nb * sizeof(int));
    if (nbCopiesBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(nbCopiesBuffer);
    unsigned short* nbCopiesBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (nbCopiesBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(nbCopiesBufLens);
    for (int i = 0; i < nb; i++) {
      nbCopiesBuffer[i] = objs[i]->nbCopies();
      nbCopiesBufLens[i] = sizeof(int);
    }
    m_bulkInsertStatement->setDataBuffer
      (2, nbCopiesBuffer, castor::db::DBTYPE_INT, sizeof(nbCopiesBuffer[0]), nbCopiesBufLens);
    // build the buffers for classId
    double* classIdBuffer = (double*) malloc(nb * sizeof(double));
    if (classIdBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(classIdBuffer);
    unsigned short* classIdBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (classIdBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(classIdBufLens);
    for (int i = 0; i < nb; i++) {
      classIdBuffer[i] = objs[i]->classId();
      classIdBufLens[i] = sizeof(double);
    }
    m_bulkInsertStatement->setDataBuffer
      (3, classIdBuffer, castor::db::DBTYPE_UINT64, sizeof(classIdBuffer[0]), classIdBufLens);
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
void castor::db::cnv::DbFileClassCnv::updateRep(castor::IAddress*,
                                                castor::IObject* object,
                                                bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::FileClass* obj = 
    dynamic_cast<castor::stager::FileClass*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->name());
    m_updateStatement->setInt(2, obj->nbCopies());
    m_updateStatement->setUInt64(3, obj->classId());
    m_updateStatement->setUInt64(4, obj->id());
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
void castor::db::cnv::DbFileClassCnv::deleteRep(castor::IAddress*,
                                                castor::IObject* object,
                                                bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::FileClass* obj = 
    dynamic_cast<castor::stager::FileClass*>(object);
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
castor::IObject* castor::db::cnv::DbFileClassCnv::createObj(castor::IAddress* address)
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
    castor::stager::FileClass* object = new castor::stager::FileClass();
    // Now retrieve and set members
    object->setName(rset->getString(1));
    object->setNbCopies(rset->getInt(2));
    object->setClassId(rset->getUInt64(3));
    object->setId(rset->getUInt64(4));
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
castor::db::cnv::DbFileClassCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::stager::FileClass* object = new castor::stager::FileClass();
      // Now retrieve and set members
      object->setName(rset->getString(1));
      object->setNbCopies(rset->getInt(2));
      object->setClassId(rset->getUInt64(3));
      object->setId(rset->getUInt64(4));
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
void castor::db::cnv::DbFileClassCnv::updateObj(castor::IObject* obj)
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
    castor::stager::FileClass* object = 
      dynamic_cast<castor::stager::FileClass*>(obj);
    object->setName(rset->getString(1));
    object->setNbCopies(rset->getInt(2));
    object->setClassId(rset->getUInt64(3));
    object->setId(rset->getUInt64(4));
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


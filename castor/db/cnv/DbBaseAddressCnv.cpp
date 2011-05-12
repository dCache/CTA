/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbBaseAddressCnv.cpp
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
#include "DbBaseAddressCnv.hpp"
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
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbBaseAddressCnv>* s_factoryDbBaseAddressCnv =
  new castor::CnvFactory<castor::db::cnv::DbBaseAddressCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbBaseAddressCnv::s_insertStatementString =
"INSERT INTO BaseAddress (objType, cnvSvcName, cnvSvcType, target, id) VALUES (:1,:2,:3,:4,ids_seq.nextval) RETURNING id INTO :5";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbBaseAddressCnv::s_deleteStatementString =
"DELETE FROM BaseAddress WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbBaseAddressCnv::s_selectStatementString =
"SELECT objType, cnvSvcName, cnvSvcType, target, id FROM BaseAddress WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbBaseAddressCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE RecordType IS RECORD (objType NUMBER, cnvSvcName VARCHAR2(2048), cnvSvcType NUMBER, target INTEGER, id INTEGER); \
   TYPE CurType IS REF CURSOR RETURN RecordType; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT objType, cnvSvcName, cnvSvcType, target, id \
                     FROM BaseAddress t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbBaseAddressCnv::s_updateStatementString =
"UPDATE BaseAddress SET objType = :1, cnvSvcName = :2, cnvSvcType = :3, target = :4 WHERE id = :5";

/// SQL statement for type storage
const std::string castor::db::cnv::DbBaseAddressCnv::s_storeTypeStatementString =
"INSERT /* BaseAddress class */ INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbBaseAddressCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbBaseAddressCnv::DbBaseAddressCnv(castor::ICnvSvc* cnvSvc) :
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
castor::db::cnv::DbBaseAddressCnv::~DbBaseAddressCnv() throw() {
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
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbBaseAddressCnv::ObjType() {
  return castor::BaseAddress::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbBaseAddressCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbBaseAddressCnv::fillRep(castor::IAddress*,
                                                castor::IObject* object,
                                                unsigned int type,
                                                bool endTransaction)
  throw (castor::exception::Exception) {
  castor::BaseAddress* obj = 
    dynamic_cast<castor::BaseAddress*>(object);
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
void castor::db::cnv::DbBaseAddressCnv::fillObj(castor::IAddress*,
                                                castor::IObject* object,
                                                unsigned int type,
                                                bool endTransaction)
  throw (castor::exception::Exception) {
  castor::BaseAddress* obj = 
    dynamic_cast<castor::BaseAddress*>(object);
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
void castor::db::cnv::DbBaseAddressCnv::createRep(castor::IAddress*,
                                                  castor::IObject* object,
                                                  bool endTransaction,
                                                  unsigned int)
  throw (castor::exception::Exception) {
  castor::BaseAddress* obj = 
    dynamic_cast<castor::BaseAddress*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(5, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->objType());
    m_insertStatement->setString(2, obj->cnvSvcName());
    m_insertStatement->setInt(3, obj->cnvSvcType());
    m_insertStatement->setUInt64(4, obj->target());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(5));
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
                    << "  objType : " << obj->objType() << std::endl
                    << "  cnvSvcName : " << obj->cnvSvcName() << std::endl
                    << "  cnvSvcType : " << obj->cnvSvcType() << std::endl
                    << "  target : " << obj->target() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbBaseAddressCnv::bulkCreateRep(castor::IAddress*,
                                                      std::vector<castor::IObject*> &objects,
                                                      bool endTransaction,
                                                      unsigned int)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::BaseAddress*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::BaseAddress*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(5, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for objType
    int* objTypeBuffer = (int*) malloc(nb * sizeof(int));
    if (objTypeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(objTypeBuffer);
    unsigned short* objTypeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (objTypeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(objTypeBufLens);
    for (int i = 0; i < nb; i++) {
      objTypeBuffer[i] = objs[i]->objType();
      objTypeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (1, objTypeBuffer, castor::db::DBTYPE_INT, sizeof(objTypeBuffer[0]), objTypeBufLens);
    // build the buffers for cnvSvcName
    unsigned int cnvSvcNameMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->cnvSvcName().length()+1 > cnvSvcNameMaxLen)
        cnvSvcNameMaxLen = objs[i]->cnvSvcName().length()+1;
    }
    char* cnvSvcNameBuffer = (char*) calloc(nb, cnvSvcNameMaxLen);
    if (cnvSvcNameBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(cnvSvcNameBuffer);
    unsigned short* cnvSvcNameBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (cnvSvcNameBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(cnvSvcNameBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(cnvSvcNameBuffer+(i*cnvSvcNameMaxLen), objs[i]->cnvSvcName().c_str(), cnvSvcNameMaxLen);
      cnvSvcNameBufLens[i] = objs[i]->cnvSvcName().length()+1; // + 1 for the trailing \0
    }
    m_insertStatement->setDataBuffer
      (2, cnvSvcNameBuffer, castor::db::DBTYPE_STRING, cnvSvcNameMaxLen, cnvSvcNameBufLens);
    // build the buffers for cnvSvcType
    int* cnvSvcTypeBuffer = (int*) malloc(nb * sizeof(int));
    if (cnvSvcTypeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(cnvSvcTypeBuffer);
    unsigned short* cnvSvcTypeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (cnvSvcTypeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(cnvSvcTypeBufLens);
    for (int i = 0; i < nb; i++) {
      cnvSvcTypeBuffer[i] = objs[i]->cnvSvcType();
      cnvSvcTypeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (3, cnvSvcTypeBuffer, castor::db::DBTYPE_INT, sizeof(cnvSvcTypeBuffer[0]), cnvSvcTypeBufLens);
    // build the buffers for target
    double* targetBuffer = (double*) malloc(nb * sizeof(double));
    if (targetBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(targetBuffer);
    unsigned short* targetBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (targetBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(targetBufLens);
    for (int i = 0; i < nb; i++) {
      targetBuffer[i] = objs[i]->target();
      targetBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (4, targetBuffer, castor::db::DBTYPE_UINT64, sizeof(targetBuffer[0]), targetBufLens);
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
      (5, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
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
void castor::db::cnv::DbBaseAddressCnv::updateRep(castor::IAddress*,
                                                  castor::IObject* object,
                                                  bool endTransaction)
  throw (castor::exception::Exception) {
  castor::BaseAddress* obj = 
    dynamic_cast<castor::BaseAddress*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->objType());
    m_updateStatement->setString(2, obj->cnvSvcName());
    m_updateStatement->setInt(3, obj->cnvSvcType());
    m_updateStatement->setUInt64(4, obj->target());
    m_updateStatement->setUInt64(5, obj->id());
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
void castor::db::cnv::DbBaseAddressCnv::deleteRep(castor::IAddress*,
                                                  castor::IObject* object,
                                                  bool endTransaction)
  throw (castor::exception::Exception) {
  castor::BaseAddress* obj = 
    dynamic_cast<castor::BaseAddress*>(object);
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
castor::IObject* castor::db::cnv::DbBaseAddressCnv::createObj(castor::IAddress* address)
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
    castor::BaseAddress* object = new castor::BaseAddress();
    // Now retrieve and set members
    object->setObjType(rset->getInt(1));
    object->setCnvSvcName(rset->getString(2));
    object->setCnvSvcType(rset->getInt(3));
    object->setTarget(rset->getUInt64(4));
    object->setId(rset->getUInt64(5));
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
castor::db::cnv::DbBaseAddressCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::BaseAddress* object = new castor::BaseAddress();
      // Now retrieve and set members
      object->setObjType(rset->getInt(1));
      object->setCnvSvcName(rset->getString(2));
      object->setCnvSvcType(rset->getInt(3));
      object->setTarget(rset->getUInt64(4));
      object->setId(rset->getUInt64(5));
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
void castor::db::cnv::DbBaseAddressCnv::updateObj(castor::IObject* obj)
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
    castor::BaseAddress* object = 
      dynamic_cast<castor::BaseAddress*>(obj);
    object->setObjType(rset->getInt(1));
    object->setCnvSvcName(rset->getString(2));
    object->setCnvSvcType(rset->getInt(3));
    object->setTarget(rset->getUInt64(4));
    object->setId(rset->getUInt64(5));
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


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbNsFileIdCnv.cpp
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
#include "DbNsFileIdCnv.hpp"
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
#include "castor/stager/NsFileId.hpp"
#include "castor/stager/StageAbortRequest.hpp"
#include <stdlib.h>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbNsFileIdCnv>* s_factoryDbNsFileIdCnv =
  new castor::CnvFactory<castor::db::cnv::DbNsFileIdCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbNsFileIdCnv::s_insertStatementString =
"INSERT INTO NsFileId (fileid, nsHost, id, request) VALUES (:1,:2,ids_seq.nextval,:3) RETURNING id INTO :4";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbNsFileIdCnv::s_deleteStatementString =
"DELETE FROM NsFileId WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbNsFileIdCnv::s_selectStatementString =
"SELECT fileid, nsHost, id, request FROM NsFileId WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbNsFileIdCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE CurType IS REF CURSOR RETURN NsFileId%ROWTYPE; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT fileid, nsHost, id, request \
                     FROM NsFileId t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbNsFileIdCnv::s_updateStatementString =
"UPDATE NsFileId SET fileid = :1, nsHost = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::cnv::DbNsFileIdCnv::s_storeTypeStatementString =
"INSERT /* NsFileId class */ INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbNsFileIdCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member request
const std::string castor::db::cnv::DbNsFileIdCnv::s_checkStageAbortRequestExistStatementString =
"SELECT id FROM StageAbortRequest WHERE id = :1";

/// SQL update statement for member request
const std::string castor::db::cnv::DbNsFileIdCnv::s_updateStageAbortRequestStatementString =
"UPDATE NsFileId SET request = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbNsFileIdCnv::DbNsFileIdCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkStageAbortRequestExistStatement(0),
  m_updateStageAbortRequestStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbNsFileIdCnv::~DbNsFileIdCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFileIdCnv::reset() throw() {
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
    if(m_checkStageAbortRequestExistStatement) delete m_checkStageAbortRequestExistStatement;
    if(m_updateStageAbortRequestStatement) delete m_updateStageAbortRequestStatement;
  } catch (castor::exception::Exception& ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_bulkSelectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkStageAbortRequestExistStatement = 0;
  m_updateStageAbortRequestStatement = 0;
  // Call upper level reset
  this->DbBaseCnv::reset();
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbNsFileIdCnv::ObjType() {
  return castor::stager::NsFileId::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
unsigned int castor::db::cnv::DbNsFileIdCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFileIdCnv::fillRep(castor::IAddress*,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::NsFileId* obj = 
    dynamic_cast<castor::stager::NsFileId*>(object);
  try {
    switch (type) {
    case castor::OBJ_StageAbortRequest :
      fillRepStageAbortRequest(obj);
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
// fillRepStageAbortRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFileIdCnv::fillRepStageAbortRequest(castor::stager::NsFileId* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->request()) {
    // Check checkStageAbortRequestExist statement
    if (0 == m_checkStageAbortRequestExistStatement) {
      m_checkStageAbortRequestExistStatement = createStatement(s_checkStageAbortRequestExistStatementString);
    }
    // retrieve the object from the database
    m_checkStageAbortRequestExistStatement->setUInt64(1, obj->request()->id());
    castor::db::IDbResultSet *rset = m_checkStageAbortRequestExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->request(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateStageAbortRequestStatement) {
    m_updateStageAbortRequestStatement = createStatement(s_updateStageAbortRequestStatementString);
  }
  // Update local object
  m_updateStageAbortRequestStatement->setUInt64(1, 0 == obj->request() ? 0 : obj->request()->id());
  m_updateStageAbortRequestStatement->setUInt64(2, obj->id());
  m_updateStageAbortRequestStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFileIdCnv::fillObj(castor::IAddress*,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::NsFileId* obj = 
    dynamic_cast<castor::stager::NsFileId*>(object);
  switch (type) {
  case castor::OBJ_StageAbortRequest :
    fillObjStageAbortRequest(obj);
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
// fillObjStageAbortRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFileIdCnv::fillObjStageAbortRequest(castor::stager::NsFileId* obj)
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
  u_signed64 requestId = rset->getInt64(4);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->request() &&
      (0 == requestId ||
       obj->request()->id() != requestId)) {
    obj->request()->removeFiles(obj);
    obj->setRequest(0);
  }
  // Update object or create new one
  if (0 != requestId) {
    if (0 == obj->request()) {
      obj->setRequest
        (dynamic_cast<castor::stager::StageAbortRequest*>
         (cnvSvc()->getObjFromId(requestId)));
    } else {
      cnvSvc()->updateObj(obj->request());
    }
    obj->request()->addFiles(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFileIdCnv::createRep(castor::IAddress*,
                                               castor::IObject* object,
                                               bool endTransaction,
                                               unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::NsFileId* obj = 
    dynamic_cast<castor::stager::NsFileId*>(object);
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
    m_insertStatement->setUInt64(1, obj->fileid());
    m_insertStatement->setString(2, obj->nsHost());
    m_insertStatement->setUInt64(3, (type == OBJ_StageAbortRequest && obj->request() != 0) ? obj->request()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(4));
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
                    << "  nsHost : " << obj->nsHost() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  request : " << (obj->request() ? obj->request()->id() : 0) << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFileIdCnv::bulkCreateRep(castor::IAddress*,
                                                   std::vector<castor::IObject*> &objects,
                                                   bool endTransaction,
                                                   unsigned int type)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::stager::NsFileId*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::stager::NsFileId*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(4, castor::db::DBTYPE_UINT64);
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
    m_insertStatement->setDataBuffer
      (2, nsHostBuffer, castor::db::DBTYPE_STRING, nsHostMaxLen, nsHostBufLens);
    // build the buffers for request
    double* requestBuffer = (double*) malloc(nb * sizeof(double));
    if (requestBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(requestBuffer);
    unsigned short* requestBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (requestBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(requestBufLens);
    for (int i = 0; i < nb; i++) {
      requestBuffer[i] = (type == OBJ_StageAbortRequest && objs[i]->request() != 0) ? objs[i]->request()->id() : 0;
      requestBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (3, requestBuffer, castor::db::DBTYPE_UINT64, sizeof(requestBuffer[0]), requestBufLens);
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
      (4, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
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
void castor::db::cnv::DbNsFileIdCnv::updateRep(castor::IAddress*,
                                               castor::IObject* object,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::NsFileId* obj = 
    dynamic_cast<castor::stager::NsFileId*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setUInt64(1, obj->fileid());
    m_updateStatement->setString(2, obj->nsHost());
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
void castor::db::cnv::DbNsFileIdCnv::deleteRep(castor::IAddress*,
                                               castor::IObject* object,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::NsFileId* obj = 
    dynamic_cast<castor::stager::NsFileId*>(object);
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
castor::IObject* castor::db::cnv::DbNsFileIdCnv::createObj(castor::IAddress* address)
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
    castor::stager::NsFileId* object = new castor::stager::NsFileId();
    // Now retrieve and set members
    object->setFileid(rset->getUInt64(1));
    object->setNsHost(rset->getString(2));
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
castor::db::cnv::DbNsFileIdCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::stager::NsFileId* object = new castor::stager::NsFileId();
      // Now retrieve and set members
      object->setFileid(rset->getUInt64(1));
      object->setNsHost(rset->getString(2));
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
void castor::db::cnv::DbNsFileIdCnv::updateObj(castor::IObject* obj)
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
    castor::stager::NsFileId* object = 
      dynamic_cast<castor::stager::NsFileId*>(obj);
    object->setFileid(rset->getUInt64(1));
    object->setNsHost(rset->getString(2));
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


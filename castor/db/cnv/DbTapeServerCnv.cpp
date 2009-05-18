/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbTapeServerCnv.cpp
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
#include "DbTapeServerCnv.hpp"
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
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "castor/vdqm/TapeServerStatusCodes.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbTapeServerCnv>* s_factoryDbTapeServerCnv =
  new castor::CnvFactory<castor::db::cnv::DbTapeServerCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbTapeServerCnv::s_insertStatementString =
"INSERT INTO TapeServer (serverName, id, actingMode) VALUES (:1,ids_seq.nextval,:2) RETURNING id INTO :3";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbTapeServerCnv::s_deleteStatementString =
"DELETE FROM TapeServer WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbTapeServerCnv::s_selectStatementString =
"SELECT serverName, id, actingMode FROM TapeServer WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbTapeServerCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE CurType IS REF CURSOR RETURN TapeServer%ROWTYPE; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT serverName, id, actingMode \
                     FROM TapeServer t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbTapeServerCnv::s_updateStatementString =
"UPDATE TapeServer SET serverName = :1, actingMode = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::cnv::DbTapeServerCnv::s_storeTypeStatementString =
"INSERT /* TapeServer class */ INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbTapeServerCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member tapeDrives
const std::string castor::db::cnv::DbTapeServerCnv::s_selectTapeDriveStatementString =
"SELECT id FROM TapeDrive WHERE tapeServer = :1 FOR UPDATE";

/// SQL delete statement for member tapeDrives
const std::string castor::db::cnv::DbTapeServerCnv::s_deleteTapeDriveStatementString =
"UPDATE TapeDrive SET tapeServer = 0 WHERE id = :1";

/// SQL remote update statement for member tapeDrives
const std::string castor::db::cnv::DbTapeServerCnv::s_remoteUpdateTapeDriveStatementString =
"UPDATE TapeDrive SET tapeServer = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeServerCnv::DbTapeServerCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectTapeDriveStatement(0),
  m_deleteTapeDriveStatement(0),
  m_remoteUpdateTapeDriveStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeServerCnv::~DbTapeServerCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::reset() throw() {
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
    if(m_deleteTapeDriveStatement) delete m_deleteTapeDriveStatement;
    if(m_selectTapeDriveStatement) delete m_selectTapeDriveStatement;
    if(m_remoteUpdateTapeDriveStatement) delete m_remoteUpdateTapeDriveStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_bulkSelectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectTapeDriveStatement = 0;
  m_deleteTapeDriveStatement = 0;
  m_remoteUpdateTapeDriveStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeServerCnv::ObjType() {
  return castor::vdqm::TapeServer::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeServerCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::fillRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeServer* obj = 
    dynamic_cast<castor::vdqm::TapeServer*>(object);
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
  } catch (castor::exception::SQLError e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.getMessage().str() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepTapeDrive
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::fillRepTapeDrive(castor::vdqm::TapeServer* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectTapeDriveStatement) {
    m_selectTapeDriveStatement = createStatement(s_selectTapeDriveStatementString);
  }
  // Get current database data
  std::set<u_signed64> tapeDrivesList;
  m_selectTapeDriveStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapeDriveStatement->executeQuery();
  while (rset->next()) {
    tapeDrivesList.insert(rset->getUInt64(1));
  }
  delete rset;
  // update tapeDrives and create new ones
  std::vector<castor::IObject*> toBeCreated;
  for (std::vector<castor::vdqm::TapeDrive*>::iterator it = obj->tapeDrives().begin();
       it != obj->tapeDrives().end();
       it++) {
    if (0 == (*it)->id()) {
      toBeCreated.push_back(*it);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateTapeDriveStatement) {
        m_remoteUpdateTapeDriveStatement = createStatement(s_remoteUpdateTapeDriveStatementString);
      }
      // Update remote object
      m_remoteUpdateTapeDriveStatement->setUInt64(1, obj->id());
      m_remoteUpdateTapeDriveStatement->setUInt64(2, (*it)->id());
      m_remoteUpdateTapeDriveStatement->execute();
      std::set<u_signed64>::iterator item;
      if ((item = tapeDrivesList.find((*it)->id())) != tapeDrivesList.end()) {
        tapeDrivesList.erase(item);
      }
    }
  }
  // create new objects
  cnvSvc()->bulkCreateRep(0, toBeCreated, false, OBJ_TapeServer);
  // Delete old links
  for (std::set<u_signed64>::iterator it = tapeDrivesList.begin();
       it != tapeDrivesList.end();
       it++) {
    if (0 == m_deleteTapeDriveStatement) {
      m_deleteTapeDriveStatement = createStatement(s_deleteTapeDriveStatementString);
    }
    m_deleteTapeDriveStatement->setUInt64(1, *it);
    m_deleteTapeDriveStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::fillObj(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeServer* obj = 
    dynamic_cast<castor::vdqm::TapeServer*>(object);
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
void castor::db::cnv::DbTapeServerCnv::fillObjTapeDrive(castor::vdqm::TapeServer* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectTapeDriveStatement) {
    m_selectTapeDriveStatement = createStatement(s_selectTapeDriveStatementString);
  }
  // retrieve the object from the database
  std::vector<u_signed64> tapeDrivesList;
  m_selectTapeDriveStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapeDriveStatement->executeQuery();
  while (rset->next()) {
    tapeDrivesList.push_back(rset->getUInt64(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::vdqm::TapeDrive*> toBeDeleted;
  for (std::vector<castor::vdqm::TapeDrive*>::iterator it = obj->tapeDrives().begin();
       it != obj->tapeDrives().end();
       it++) {
    std::vector<u_signed64>::iterator item =
      std::find(tapeDrivesList.begin(), tapeDrivesList.end(), (*it)->id());
    if (item == tapeDrivesList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      tapeDrivesList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::vdqm::TapeDrive*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeTapeDrives(*it);
    (*it)->setTapeServer(0);
  }
  // Create new objects
  std::vector<castor::IObject*> newTapeDrives =
    cnvSvc()->getObjsFromIds(tapeDrivesList, OBJ_TapeDrive);
  for (std::vector<castor::IObject*>::iterator it = newTapeDrives.begin();
       it != newTapeDrives.end();
       it++) {
    castor::vdqm::TapeDrive* remoteObj = 
      dynamic_cast<castor::vdqm::TapeDrive*>(*it);
    obj->addTapeDrives(remoteObj);
    remoteObj->setTapeServer(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::createRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool endTransaction,
                                                 unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeServer* obj = 
    dynamic_cast<castor::vdqm::TapeServer*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(3, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->serverName());
    m_insertStatement->setInt(2, (int)obj->actingMode());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(3));
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
                    << "  serverName : " << obj->serverName() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  actingMode : " << obj->actingMode() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::bulkCreateRep(castor::IAddress* address,
                                                     std::vector<castor::IObject*> &objects,
                                                     bool endTransaction,
                                                     unsigned int type)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::vdqm::TapeServer*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::vdqm::TapeServer*>(objects[i]));
  }
  std::vector<void *> allocMem;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(3, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for serverName
    unsigned int serverNameMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->serverName().length()+1 > serverNameMaxLen)
        serverNameMaxLen = objs[i]->serverName().length()+1;
    }
    char* serverNameBuffer = (char*) calloc(nb, serverNameMaxLen);
    if (serverNameBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(serverNameBuffer);
    unsigned short* serverNameBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (serverNameBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(serverNameBufLens);
    for (int i = 0; i < nb; i++) {
      strncpy(serverNameBuffer+(i*serverNameMaxLen), objs[i]->serverName().c_str(), serverNameMaxLen);
      serverNameBufLens[i] = objs[i]->serverName().length()+1; // + 1 for the trailing \0
    }
    m_insertStatement->setDataBuffer
      (1, serverNameBuffer, castor::db::DBTYPE_STRING, serverNameMaxLen, serverNameBufLens);
    // build the buffers for actingMode
    int* actingModeBuffer = (int*) malloc(nb * sizeof(int));
    if (actingModeBuffer == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(actingModeBuffer);
    unsigned short* actingModeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    if (actingModeBufLens == 0) {
      castor::exception::OutOfMemory e;
      throw e;
    }
    allocMem.push_back(actingModeBufLens);
    for (int i = 0; i < nb; i++) {
      actingModeBuffer[i] = objs[i]->actingMode();
      actingModeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (2, actingModeBuffer, castor::db::DBTYPE_INT, sizeof(actingModeBuffer[0]), actingModeBufLens);
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
      (3, idBuffer, castor::db::DBTYPE_UINT64, sizeof(double), idBufLens);
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
void castor::db::cnv::DbTapeServerCnv::updateRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeServer* obj = 
    dynamic_cast<castor::vdqm::TapeServer*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->serverName());
    m_updateStatement->setInt(2, (int)obj->actingMode());
    m_updateStatement->setUInt64(3, obj->id());
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
void castor::db::cnv::DbTapeServerCnv::deleteRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeServer* obj = 
    dynamic_cast<castor::vdqm::TapeServer*>(object);
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
    for (std::vector<castor::vdqm::TapeDrive*>::iterator it = obj->tapeDrives().begin();
         it != obj->tapeDrives().end();
         it++) {
      cnvSvc()->deleteRep(0, *it, false);
    }
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
castor::IObject* castor::db::cnv::DbTapeServerCnv::createObj(castor::IAddress* address)
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
    castor::vdqm::TapeServer* object = new castor::vdqm::TapeServer();
    // Now retrieve and set members
    object->setServerName(rset->getString(1));
    object->setId(rset->getUInt64(2));
    object->setActingMode((enum castor::vdqm::TapeServerStatusCodes)rset->getInt(3));
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
castor::db::cnv::DbTapeServerCnv::bulkCreateObj(castor::IAddress* address)
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
      castor::vdqm::TapeServer* object = new castor::vdqm::TapeServer();
      // Now retrieve and set members
      object->setServerName(rset->getString(1));
      object->setId(rset->getUInt64(2));
      object->setActingMode((enum castor::vdqm::TapeServerStatusCodes)rset->getInt(3));
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
void castor::db::cnv::DbTapeServerCnv::updateObj(castor::IObject* obj)
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
    castor::vdqm::TapeServer* object = 
      dynamic_cast<castor::vdqm::TapeServer*>(obj);
    object->setServerName(rset->getString(1));
    object->setId(rset->getUInt64(2));
    object->setActingMode((enum castor::vdqm::TapeServerStatusCodes)rset->getInt(3));
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


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbStreamCnv.cpp
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
#include "DbStreamCnv.hpp"
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
#include "castor/stager/Stream.hpp"
#include "castor/stager/StreamStatusCodes.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "castor/stager/TapePool.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbStreamCnv>* s_factoryDbStreamCnv =
  new castor::CnvFactory<castor::db::cnv::DbStreamCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbStreamCnv::s_insertStatementString =
"INSERT INTO Stream (initialSizeToTransfer, id, tape, tapePool, status) VALUES (:1,ids_seq.nextval,:2,:3,:4) RETURNING id INTO :5";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbStreamCnv::s_deleteStatementString =
"DELETE FROM Stream WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbStreamCnv::s_selectStatementString =
"SELECT initialSizeToTransfer, id, tape, tapePool, status FROM Stream WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbStreamCnv::s_updateStatementString =
"UPDATE Stream SET initialSizeToTransfer = :1, status = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::cnv::DbStreamCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbStreamCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL insert statement for member tapeCopy
const std::string castor::db::cnv::DbStreamCnv::s_insertTapeCopyStatementString =
"INSERT INTO Stream2TapeCopy (Parent, Child) VALUES (:1, :2)";

/// SQL delete statement for member tapeCopy
const std::string castor::db::cnv::DbStreamCnv::s_deleteTapeCopyStatementString =
"DELETE FROM Stream2TapeCopy WHERE Parent = :1 AND Child = :2";

/// SQL select statement for member tapeCopy
// The FOR UPDATE is needed in order to avoid deletion
// of a segment after listing and before update/remove
const std::string castor::db::cnv::DbStreamCnv::s_selectTapeCopyStatementString =
"SELECT Child FROM Stream2TapeCopy WHERE Parent = :1 FOR UPDATE";

/// SQL select statement for member tape
const std::string castor::db::cnv::DbStreamCnv::s_selectTapeStatementString =
"SELECT id FROM Tape WHERE stream = :1 FOR UPDATE";

/// SQL delete statement for member tape
const std::string castor::db::cnv::DbStreamCnv::s_deleteTapeStatementString =
"UPDATE Tape SET stream = 0 WHERE id = :1";

/// SQL remote update statement for member tape
const std::string castor::db::cnv::DbStreamCnv::s_remoteUpdateTapeStatementString =
"UPDATE Tape SET stream = :1 WHERE id = :2";

/// SQL existence statement for member tape
const std::string castor::db::cnv::DbStreamCnv::s_checkTapeExistStatementString =
"SELECT id FROM Tape WHERE id = :1";

/// SQL update statement for member tape
const std::string castor::db::cnv::DbStreamCnv::s_updateTapeStatementString =
"UPDATE Stream SET tape = :1 WHERE id = :2";

/// SQL existence statement for member tapePool
const std::string castor::db::cnv::DbStreamCnv::s_checkTapePoolExistStatementString =
"SELECT id FROM TapePool WHERE id = :1";

/// SQL update statement for member tapePool
const std::string castor::db::cnv::DbStreamCnv::s_updateTapePoolStatementString =
"UPDATE Stream SET tapePool = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbStreamCnv::DbStreamCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_insertTapeCopyStatement(0),
  m_deleteTapeCopyStatement(0),
  m_selectTapeCopyStatement(0),
  m_selectTapeStatement(0),
  m_deleteTapeStatement(0),
  m_remoteUpdateTapeStatement(0),
  m_checkTapeExistStatement(0),
  m_updateTapeStatement(0),
  m_checkTapePoolExistStatement(0),
  m_updateTapePoolStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbStreamCnv::~DbStreamCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_insertTapeCopyStatement) delete m_insertTapeCopyStatement;
    if(m_deleteTapeCopyStatement) delete m_deleteTapeCopyStatement;
    if(m_selectTapeCopyStatement) delete m_selectTapeCopyStatement;
    if(m_deleteTapeStatement) delete m_deleteTapeStatement;
    if(m_selectTapeStatement) delete m_selectTapeStatement;
    if(m_remoteUpdateTapeStatement) delete m_remoteUpdateTapeStatement;
    if(m_checkTapeExistStatement) delete m_checkTapeExistStatement;
    if(m_updateTapeStatement) delete m_updateTapeStatement;
    if(m_checkTapePoolExistStatement) delete m_checkTapePoolExistStatement;
    if(m_updateTapePoolStatement) delete m_updateTapePoolStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_insertTapeCopyStatement = 0;
  m_deleteTapeCopyStatement = 0;
  m_selectTapeCopyStatement = 0;
  m_selectTapeStatement = 0;
  m_deleteTapeStatement = 0;
  m_remoteUpdateTapeStatement = 0;
  m_checkTapeExistStatement = 0;
  m_updateTapeStatement = 0;
  m_checkTapePoolExistStatement = 0;
  m_updateTapePoolStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbStreamCnv::ObjType() {
  return castor::stager::Stream::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbStreamCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::fillRep(castor::IAddress* address,
                                           castor::IObject* object,
                                           unsigned int type,
                                           bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
  try {
    switch (type) {
    case castor::OBJ_TapeCopy :
      fillRepTapeCopy(obj);
      break;
    case castor::OBJ_Tape :
      fillRepTape(obj);
      break;
    case castor::OBJ_TapePool :
      fillRepTapePool(obj);
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
// fillRepTapeCopy
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::fillRepTapeCopy(castor::stager::Stream* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectTapeCopyStatement) {
    m_selectTapeCopyStatement = createStatement(s_selectTapeCopyStatementString);
  }
  // Get current database data
  std::set<int> tapeCopyList;
  m_selectTapeCopyStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapeCopyStatement->executeQuery();
  while (rset->next()) {
    tapeCopyList.insert(rset->getInt(1));
  }
  delete rset;
  // update tapeCopy and create new ones
  for (std::vector<castor::stager::TapeCopy*>::iterator it = obj->tapeCopy().begin();
       it != obj->tapeCopy().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false);
    }
    std::set<int>::iterator item;
    if ((item = tapeCopyList.find((*it)->id())) != tapeCopyList.end()) {
      tapeCopyList.erase(item);
    } else {
      if (0 == m_insertTapeCopyStatement) {
        m_insertTapeCopyStatement = createStatement(s_insertTapeCopyStatementString);
      }
      m_insertTapeCopyStatement->setUInt64(1, obj->id());
      m_insertTapeCopyStatement->setUInt64(2, (*it)->id());
      m_insertTapeCopyStatement->execute();
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = tapeCopyList.begin();
       it != tapeCopyList.end();
       it++) {
    if (0 == m_deleteTapeCopyStatement) {
      m_deleteTapeCopyStatement = createStatement(s_deleteTapeCopyStatementString);
    }
    m_deleteTapeCopyStatement->setUInt64(1, obj->id());
    m_deleteTapeCopyStatement->setUInt64(2, *it);
    m_deleteTapeCopyStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepTape
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::fillRepTape(castor::stager::Stream* obj)
  throw (castor::exception::Exception) {
  // Check selectTape statement
  if (0 == m_selectTapeStatement) {
    m_selectTapeStatement = createStatement(s_selectTapeStatementString);
  }
  // retrieve the object from the database
  m_selectTapeStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapeStatement->executeQuery();
  if (rset->next()) {
    u_signed64 tapeId = rset->getInt64(1);
    if (0 != tapeId &&
        (0 == obj->tape() ||
         obj->tape()->id() != tapeId)) {
      if (0 == m_deleteTapeStatement) {
        m_deleteTapeStatement = createStatement(s_deleteTapeStatementString);
      }
      m_deleteTapeStatement->setUInt64(1, tapeId);
      m_deleteTapeStatement->execute();
    }
  }
  // Close resultset
  delete rset;
  if (0 != obj->tape()) {
    // Check checkTapeExist statement
    if (0 == m_checkTapeExistStatement) {
      m_checkTapeExistStatement = createStatement(s_checkTapeExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeExistStatement->setUInt64(1, obj->tape()->id());
    castor::db::IDbResultSet *rset = m_checkTapeExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->tape(), false, OBJ_Stream);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateTapeStatement) {
        m_remoteUpdateTapeStatement = createStatement(s_remoteUpdateTapeStatementString);
      }
      // Update remote object
      m_remoteUpdateTapeStatement->setUInt64(1, obj->id());
      m_remoteUpdateTapeStatement->setUInt64(2, obj->tape()->id());
      m_remoteUpdateTapeStatement->execute();
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateTapeStatement) {
    m_updateTapeStatement = createStatement(s_updateTapeStatementString);
  }
  // Update local object
  m_updateTapeStatement->setUInt64(1, 0 == obj->tape() ? 0 : obj->tape()->id());
  m_updateTapeStatement->setUInt64(2, obj->id());
  m_updateTapeStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepTapePool
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::fillRepTapePool(castor::stager::Stream* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->tapePool()) {
    // Check checkTapePoolExist statement
    if (0 == m_checkTapePoolExistStatement) {
      m_checkTapePoolExistStatement = createStatement(s_checkTapePoolExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapePoolExistStatement->setUInt64(1, obj->tapePool()->id());
    castor::db::IDbResultSet *rset = m_checkTapePoolExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->tapePool(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateTapePoolStatement) {
    m_updateTapePoolStatement = createStatement(s_updateTapePoolStatementString);
  }
  // Update local object
  m_updateTapePoolStatement->setUInt64(1, 0 == obj->tapePool() ? 0 : obj->tapePool()->id());
  m_updateTapePoolStatement->setUInt64(2, obj->id());
  m_updateTapePoolStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::fillObj(castor::IAddress* address,
                                           castor::IObject* object,
                                           unsigned int type,
                                           bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
  switch (type) {
  case castor::OBJ_TapeCopy :
    fillObjTapeCopy(obj);
    break;
  case castor::OBJ_Tape :
    fillObjTape(obj);
    break;
  case castor::OBJ_TapePool :
    fillObjTapePool(obj);
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
// fillObjTapeCopy
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::fillObjTapeCopy(castor::stager::Stream* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectTapeCopyStatement) {
    m_selectTapeCopyStatement = createStatement(s_selectTapeCopyStatementString);
  }
  // retrieve the object from the database
  std::set<int> tapeCopyList;
  m_selectTapeCopyStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapeCopyStatement->executeQuery();
  while (rset->next()) {
    tapeCopyList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::TapeCopy*> toBeDeleted;
  for (std::vector<castor::stager::TapeCopy*>::iterator it = obj->tapeCopy().begin();
       it != obj->tapeCopy().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = tapeCopyList.find((*it)->id())) == tapeCopyList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      tapeCopyList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::TapeCopy*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeTapeCopy(*it);
    (*it)->removeStream(obj);
  }
  // Create new objects
  for (std::set<int>::iterator it = tapeCopyList.begin();
       it != tapeCopyList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::TapeCopy* remoteObj = 
      dynamic_cast<castor::stager::TapeCopy*>(item);
    obj->addTapeCopy(remoteObj);
    remoteObj->addStream(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjTape
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::fillObjTape(castor::stager::Stream* obj)
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
  u_signed64 tapeId = rset->getInt64(3);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->tape() &&
      (0 == tapeId ||
       obj->tape()->id() != tapeId)) {
    obj->tape()->setStream(0);
    obj->setTape(0);
  }
  // Update object or create new one
  if (0 != tapeId) {
    if (0 == obj->tape()) {
      obj->setTape
        (dynamic_cast<castor::stager::Tape*>
         (cnvSvc()->getObjFromId(tapeId)));
    } else {
      cnvSvc()->updateObj(obj->tape());
    }
    obj->tape()->setStream(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjTapePool
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::fillObjTapePool(castor::stager::Stream* obj)
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
  u_signed64 tapePoolId = rset->getInt64(4);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->tapePool() &&
      (0 == tapePoolId ||
       obj->tapePool()->id() != tapePoolId)) {
    obj->tapePool()->removeStreams(obj);
    obj->setTapePool(0);
  }
  // Update object or create new one
  if (0 != tapePoolId) {
    if (0 == obj->tapePool()) {
      obj->setTapePool
        (dynamic_cast<castor::stager::TapePool*>
         (cnvSvc()->getObjFromId(tapePoolId)));
    } else {
      cnvSvc()->updateObj(obj->tapePool());
    }
    obj->tapePool()->addStreams(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::createRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             bool endTransaction,
                                             unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
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
    m_insertStatement->setUInt64(1, obj->initialSizeToTransfer());
    m_insertStatement->setUInt64(2, (type == OBJ_Tape && obj->tape() != 0) ? obj->tape()->id() : 0);
    m_insertStatement->setUInt64(3, (type == OBJ_TapePool && obj->tapePool() != 0) ? obj->tapePool()->id() : 0);
    m_insertStatement->setInt(4, (int)obj->status());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(5));
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
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  initialSizeToTransfer : " << obj->initialSizeToTransfer() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  tape : " << obj->tape() << std::endl
                    << "  tapePool : " << obj->tapePool() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::updateRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setUInt64(1, obj->initialSizeToTransfer());
    m_updateStatement->setInt(2, (int)obj->status());
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
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::deleteRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::Stream* obj = 
    dynamic_cast<castor::stager::Stream*>(object);
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
                    << "Statement was :" << std::endl
                    << s_deleteStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::db::cnv::DbStreamCnv::createObj(castor::IAddress* address)
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
    castor::stager::Stream* object = new castor::stager::Stream();
    // Now retrieve and set members
    object->setInitialSizeToTransfer(rset->getUInt64(1));
    object->setId(rset->getUInt64(2));
    object->setStatus((enum castor::stager::StreamStatusCodes)rset->getInt(5));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in select request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_selectStatementString << std::endl
                    << "and id was " << ad->target() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbStreamCnv::updateObj(castor::IObject* obj)
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
    castor::stager::Stream* object = 
      dynamic_cast<castor::stager::Stream*>(obj);
    object->setInitialSizeToTransfer(rset->getUInt64(1));
    object->setId(rset->getUInt64(2));
    object->setStatus((enum castor::stager::StreamStatusCodes)rset->getInt(5));
    delete rset;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


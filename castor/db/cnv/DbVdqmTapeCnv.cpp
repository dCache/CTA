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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "DbVdqmTapeCnv.hpp"
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
#include "castor/vdqm/ErrorHistory.hpp"
#include "castor/vdqm/TapeStatusCodes.hpp"
#include "castor/vdqm/VdqmTape.hpp"
#include <set>
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
"INSERT INTO VdqmTape (vid, side, tpmode, errMsgTxt, errorCode, severity, vwAddress, id, status) VALUES (:1,:2,:3,:4,:5,:6,:7,ids_seq.nextval,:8) RETURNING id INTO :9";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbVdqmTapeCnv::s_deleteStatementString =
"DELETE FROM VdqmTape WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbVdqmTapeCnv::s_selectStatementString =
"SELECT vid, side, tpmode, errMsgTxt, errorCode, severity, vwAddress, id, status FROM VdqmTape WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbVdqmTapeCnv::s_updateStatementString =
"UPDATE VdqmTape SET vid = :1, side = :2, tpmode = :3, errMsgTxt = :4, errorCode = :5, severity = :6, vwAddress = :7, status = :8 WHERE id = :9";

/// SQL statement for type storage
const std::string castor::db::cnv::DbVdqmTapeCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbVdqmTapeCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member errorHistory
const std::string castor::db::cnv::DbVdqmTapeCnv::s_selectErrorHistoryStatementString =
"SELECT id FROM ErrorHistory WHERE tape = :1 FOR UPDATE";

/// SQL delete statement for member errorHistory
const std::string castor::db::cnv::DbVdqmTapeCnv::s_deleteErrorHistoryStatementString =
"UPDATE ErrorHistory SET tape = 0 WHERE id = :1";

/// SQL remote update statement for member errorHistory
const std::string castor::db::cnv::DbVdqmTapeCnv::s_remoteUpdateErrorHistoryStatementString =
"UPDATE ErrorHistory SET tape = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbVdqmTapeCnv::DbVdqmTapeCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectErrorHistoryStatement(0),
  m_deleteErrorHistoryStatement(0),
  m_remoteUpdateErrorHistoryStatement(0) {}

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
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_deleteErrorHistoryStatement) delete m_deleteErrorHistoryStatement;
    if(m_selectErrorHistoryStatement) delete m_selectErrorHistoryStatement;
    if(m_remoteUpdateErrorHistoryStatement) delete m_remoteUpdateErrorHistoryStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectErrorHistoryStatement = 0;
  m_deleteErrorHistoryStatement = 0;
  m_remoteUpdateErrorHistoryStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbVdqmTapeCnv::ObjType() {
  return castor::vdqm::VdqmTape::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbVdqmTapeCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::fillRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VdqmTape* obj = 
    dynamic_cast<castor::vdqm::VdqmTape*>(object);
  try {
    switch (type) {
    case castor::OBJ_ErrorHistory :
      fillRepErrorHistory(obj);
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
// fillRepErrorHistory
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::fillRepErrorHistory(castor::vdqm::VdqmTape* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectErrorHistoryStatement) {
    m_selectErrorHistoryStatement = createStatement(s_selectErrorHistoryStatementString);
  }
  // Get current database data
  std::set<int> errorHistoryList;
  m_selectErrorHistoryStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectErrorHistoryStatement->executeQuery();
  while (rset->next()) {
    errorHistoryList.insert(rset->getInt(1));
  }
  delete rset;
  // update errorHistory and create new ones
  for (std::vector<castor::vdqm::ErrorHistory*>::iterator it = obj->errorHistory().begin();
       it != obj->errorHistory().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_VdqmTape);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateErrorHistoryStatement) {
        m_remoteUpdateErrorHistoryStatement = createStatement(s_remoteUpdateErrorHistoryStatementString);
      }
      // Update remote object
      m_remoteUpdateErrorHistoryStatement->setUInt64(1, obj->id());
      m_remoteUpdateErrorHistoryStatement->setUInt64(2, (*it)->id());
      m_remoteUpdateErrorHistoryStatement->execute();
      std::set<int>::iterator item;
      if ((item = errorHistoryList.find((*it)->id())) != errorHistoryList.end()) {
        errorHistoryList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = errorHistoryList.begin();
       it != errorHistoryList.end();
       it++) {
    if (0 == m_deleteErrorHistoryStatement) {
      m_deleteErrorHistoryStatement = createStatement(s_deleteErrorHistoryStatementString);
    }
    m_deleteErrorHistoryStatement->setUInt64(1, *it);
    m_deleteErrorHistoryStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::fillObj(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::vdqm::VdqmTape* obj = 
    dynamic_cast<castor::vdqm::VdqmTape*>(object);
  switch (type) {
  case castor::OBJ_ErrorHistory :
    fillObjErrorHistory(obj);
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
// fillObjErrorHistory
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::fillObjErrorHistory(castor::vdqm::VdqmTape* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectErrorHistoryStatement) {
    m_selectErrorHistoryStatement = createStatement(s_selectErrorHistoryStatementString);
  }
  // retrieve the object from the database
  std::set<int> errorHistoryList;
  m_selectErrorHistoryStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectErrorHistoryStatement->executeQuery();
  while (rset->next()) {
    errorHistoryList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::vdqm::ErrorHistory*> toBeDeleted;
  for (std::vector<castor::vdqm::ErrorHistory*>::iterator it = obj->errorHistory().begin();
       it != obj->errorHistory().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = errorHistoryList.find((*it)->id())) == errorHistoryList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      errorHistoryList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::vdqm::ErrorHistory*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeErrorHistory(*it);
    (*it)->setTape(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = errorHistoryList.begin();
       it != errorHistoryList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::vdqm::ErrorHistory* remoteObj = 
      dynamic_cast<castor::vdqm::ErrorHistory*>(item);
    obj->addErrorHistory(remoteObj);
    remoteObj->setTape(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool endTransaction,
                                               unsigned int type)
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
      m_insertStatement->registerOutParam(9, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->vid());
    m_insertStatement->setInt(2, obj->side());
    m_insertStatement->setInt(3, obj->tpmode());
    m_insertStatement->setString(4, obj->errMsgTxt());
    m_insertStatement->setInt(5, obj->errorCode());
    m_insertStatement->setInt(6, obj->severity());
    m_insertStatement->setString(7, obj->vwAddress());
    m_insertStatement->setInt(8, (int)obj->status());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(9));
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
                    << "  vid : " << obj->vid() << std::endl
                    << "  side : " << obj->side() << std::endl
                    << "  tpmode : " << obj->tpmode() << std::endl
                    << "  errMsgTxt : " << obj->errMsgTxt() << std::endl
                    << "  errorCode : " << obj->errorCode() << std::endl
                    << "  severity : " << obj->severity() << std::endl
                    << "  vwAddress : " << obj->vwAddress() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbVdqmTapeCnv::updateRep(castor::IAddress* address,
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
    m_updateStatement->setInt(2, obj->side());
    m_updateStatement->setInt(3, obj->tpmode());
    m_updateStatement->setString(4, obj->errMsgTxt());
    m_updateStatement->setInt(5, obj->errorCode());
    m_updateStatement->setInt(6, obj->severity());
    m_updateStatement->setString(7, obj->vwAddress());
    m_updateStatement->setInt(8, (int)obj->status());
    m_updateStatement->setUInt64(9, obj->id());
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
void castor::db::cnv::DbVdqmTapeCnv::deleteRep(castor::IAddress* address,
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
    object->setSide(rset->getInt(2));
    object->setTpmode(rset->getInt(3));
    object->setErrMsgTxt(rset->getString(4));
    object->setErrorCode(rset->getInt(5));
    object->setSeverity(rset->getInt(6));
    object->setVwAddress(rset->getString(7));
    object->setId(rset->getUInt64(8));
    object->setStatus((enum castor::vdqm::TapeStatusCodes)rset->getInt(9));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
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
    object->setSide(rset->getInt(2));
    object->setTpmode(rset->getInt(3));
    object->setErrMsgTxt(rset->getString(4));
    object->setErrorCode(rset->getInt(5));
    object->setSeverity(rset->getInt(6));
    object->setVwAddress(rset->getString(7));
    object->setId(rset->getUInt64(8));
    object->setStatus((enum castor::vdqm::TapeStatusCodes)rset->getInt(9));
    delete rset;
  } catch (castor::exception::SQLError e) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


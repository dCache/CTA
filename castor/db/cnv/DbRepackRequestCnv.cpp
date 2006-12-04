/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbRepackRequestCnv.cpp
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
#include "DbRepackRequestCnv.hpp"
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
#include "castor/repack/RepackRequest.hpp"
#include "castor/repack/RepackSubRequest.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbRepackRequestCnv>* s_factoryDbRepackRequestCnv =
  new castor::CnvFactory<castor::db::cnv::DbRepackRequestCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbRepackRequestCnv::s_insertStatementString =
"INSERT INTO RepackRequest (machine, userName, creationTime, serviceclass, pid, command, pool, stager, groupid, userid, id) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,ids_seq.nextval) RETURNING id INTO :11";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbRepackRequestCnv::s_deleteStatementString =
"DELETE FROM RepackRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbRepackRequestCnv::s_selectStatementString =
"SELECT machine, userName, creationTime, serviceclass, pid, command, pool, stager, groupid, userid, id FROM RepackRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbRepackRequestCnv::s_updateStatementString =
"UPDATE RepackRequest SET machine = :1, userName = :2, serviceclass = :3, pid = :4, command = :5, pool = :6, stager = :7, groupid = :8, userid = :9 WHERE id = :10";

/// SQL statement for type storage
const std::string castor::db::cnv::DbRepackRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbRepackRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member subRequest
const std::string castor::db::cnv::DbRepackRequestCnv::s_selectRepackSubRequestStatementString =
"SELECT id FROM RepackSubRequest WHERE requestID = :1 FOR UPDATE";

/// SQL delete statement for member subRequest
const std::string castor::db::cnv::DbRepackRequestCnv::s_deleteRepackSubRequestStatementString =
"UPDATE RepackSubRequest SET requestID = 0 WHERE id = :1";

/// SQL remote update statement for member subRequest
const std::string castor::db::cnv::DbRepackRequestCnv::s_remoteUpdateRepackSubRequestStatementString =
"UPDATE RepackSubRequest SET requestID = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbRepackRequestCnv::DbRepackRequestCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectRepackSubRequestStatement(0),
  m_deleteRepackSubRequestStatement(0),
  m_remoteUpdateRepackSubRequestStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbRepackRequestCnv::~DbRepackRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    delete m_insertStatement;
    delete m_deleteStatement;
    delete m_selectStatement;
    delete m_updateStatement;
    delete m_storeTypeStatement;
    delete m_deleteTypeStatement;
    delete m_deleteRepackSubRequestStatement;
    delete m_selectRepackSubRequestStatement;
    delete m_remoteUpdateRepackSubRequestStatement;
  } catch (castor::exception::SQLError ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectRepackSubRequestStatement = 0;
  m_deleteRepackSubRequestStatement = 0;
  m_remoteUpdateRepackSubRequestStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbRepackRequestCnv::ObjType() {
  return castor::repack::RepackRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbRepackRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::fillRep(castor::IAddress* address,
                                                  castor::IObject* object,
                                                  unsigned int type,
                                                  bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackRequest* obj = 
    dynamic_cast<castor::repack::RepackRequest*>(object);
  try {
    switch (type) {
    case castor::OBJ_RepackSubRequest :
      fillRepRepackSubRequest(obj);
      break;
    default :
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "fillRep called for type " << type 
                      << " on object of type " << obj->type() 
                      << ". This is meaningless.";
      throw ex;
    }
    if (autocommit) {
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
// fillRepRepackSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::fillRepRepackSubRequest(castor::repack::RepackRequest* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectRepackSubRequestStatement) {
    m_selectRepackSubRequestStatement = createStatement(s_selectRepackSubRequestStatementString);
  }
  // Get current database data
  std::set<int> subRequestList;
  m_selectRepackSubRequestStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectRepackSubRequestStatement->executeQuery();
  while (rset->next()) {
    subRequestList.insert(rset->getInt(1));
  }
  delete rset;
  // update subRequest and create new ones
  for (std::vector<castor::repack::RepackSubRequest*>::iterator it = obj->subRequest().begin();
       it != obj->subRequest().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_RepackRequest);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateRepackSubRequestStatement) {
        m_remoteUpdateRepackSubRequestStatement = createStatement(s_remoteUpdateRepackSubRequestStatementString);
      }
      // Update remote object
      m_remoteUpdateRepackSubRequestStatement->setInt64(1, obj->id());
      m_remoteUpdateRepackSubRequestStatement->setInt64(2, (*it)->id());
      m_remoteUpdateRepackSubRequestStatement->execute();
      std::set<int>::iterator item;
      if ((item = subRequestList.find((*it)->id())) != subRequestList.end()) {
        subRequestList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = subRequestList.begin();
       it != subRequestList.end();
       it++) {
    if (0 == m_deleteRepackSubRequestStatement) {
      m_deleteRepackSubRequestStatement = createStatement(s_deleteRepackSubRequestStatementString);
    }
    m_deleteRepackSubRequestStatement->setInt64(1, *it);
    m_deleteRepackSubRequestStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::fillObj(castor::IAddress* address,
                                                  castor::IObject* object,
                                                  unsigned int type,
                                                  bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackRequest* obj = 
    dynamic_cast<castor::repack::RepackRequest*>(object);
  switch (type) {
  case castor::OBJ_RepackSubRequest :
    fillObjRepackSubRequest(obj);
    break;
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillObj called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
  if (autocommit) {
    cnvSvc()->commit();
  }
}
//------------------------------------------------------------------------------
// fillObjRepackSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::fillObjRepackSubRequest(castor::repack::RepackRequest* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectRepackSubRequestStatement) {
    m_selectRepackSubRequestStatement = createStatement(s_selectRepackSubRequestStatementString);
  }
  // retrieve the object from the database
  std::set<int> subRequestList;
  m_selectRepackSubRequestStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectRepackSubRequestStatement->executeQuery();
  while (rset->next()) {
    subRequestList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::repack::RepackSubRequest*> toBeDeleted;
  for (std::vector<castor::repack::RepackSubRequest*>::iterator it = obj->subRequest().begin();
       it != obj->subRequest().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = subRequestList.find((*it)->id())) == subRequestList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      subRequestList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::repack::RepackSubRequest*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeSubRequest(*it);
    (*it)->setRequestID(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = subRequestList.begin();
       it != subRequestList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::repack::RepackSubRequest* remoteObj = 
      dynamic_cast<castor::repack::RepackSubRequest*>(item);
    obj->addSubRequest(remoteObj);
    remoteObj->setRequestID(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::createRep(castor::IAddress* address,
                                                    castor::IObject* object,
                                                    bool autocommit,
                                                    unsigned int type)
  throw (castor::exception::Exception) {
  castor::repack::RepackRequest* obj = 
    dynamic_cast<castor::repack::RepackRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(11, castor::db::DBTYPE_INT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->machine());
    m_insertStatement->setString(2, obj->userName());
    m_insertStatement->setInt64(3, time(0));
    m_insertStatement->setString(4, obj->serviceclass());
    m_insertStatement->setInt64(5, obj->pid());
    m_insertStatement->setInt(6, obj->command());
    m_insertStatement->setString(7, obj->pool());
    m_insertStatement->setString(8, obj->stager());
    m_insertStatement->setInt64(9, obj->groupid());
    m_insertStatement->setInt64(10, obj->userid());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getInt64(11));
    m_storeTypeStatement->setInt64(1, obj->id());
    m_storeTypeStatement->setInt64(2, obj->type());
    m_storeTypeStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  machine : " << obj->machine() << std::endl
                    << "  userName : " << obj->userName() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  serviceclass : " << obj->serviceclass() << std::endl
                    << "  pid : " << obj->pid() << std::endl
                    << "  command : " << obj->command() << std::endl
                    << "  pool : " << obj->pool() << std::endl
                    << "  stager : " << obj->stager() << std::endl
                    << "  groupid : " << obj->groupid() << std::endl
                    << "  userid : " << obj->userid() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::updateRep(castor::IAddress* address,
                                                    castor::IObject* object,
                                                    bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackRequest* obj = 
    dynamic_cast<castor::repack::RepackRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->machine());
    m_updateStatement->setString(2, obj->userName());
    m_updateStatement->setString(3, obj->serviceclass());
    m_updateStatement->setInt64(4, obj->pid());
    m_updateStatement->setInt(5, obj->command());
    m_updateStatement->setString(6, obj->pool());
    m_updateStatement->setString(7, obj->stager());
    m_updateStatement->setInt64(8, obj->groupid());
    m_updateStatement->setInt64(9, obj->userid());
    m_updateStatement->setInt64(10, obj->id());
    m_updateStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
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
void castor::db::cnv::DbRepackRequestCnv::deleteRep(castor::IAddress* address,
                                                    castor::IObject* object,
                                                    bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackRequest* obj = 
    dynamic_cast<castor::repack::RepackRequest*>(object);
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
    m_deleteTypeStatement->setInt64(1, obj->id());
    m_deleteTypeStatement->execute();
    m_deleteStatement->setInt64(1, obj->id());
    m_deleteStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
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
castor::IObject* castor::db::cnv::DbRepackRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::BaseAddress* ad = 
    dynamic_cast<castor::BaseAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setInt64(1, ad->target());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << ad->target();
      throw ex;
    }
    // create the new Object
    castor::repack::RepackRequest* object = new castor::repack::RepackRequest();
    // Now retrieve and set members
    object->setMachine(rset->getString(1));
    object->setUserName(rset->getString(2));
    object->setCreationTime(rset->getInt64(3));
    object->setServiceclass(rset->getString(4));
    object->setPid(rset->getInt64(5));
    object->setCommand(rset->getInt(6));
    object->setPool(rset->getString(7));
    object->setStager(rset->getString(8));
    object->setGroupid(rset->getInt64(9));
    object->setUserid(rset->getInt64(10));
    object->setId(rset->getInt64(11));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
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
void castor::db::cnv::DbRepackRequestCnv::updateObj(castor::IObject* obj)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setInt64(1, obj->id());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << obj->id();
      throw ex;
    }
    // Now retrieve and set members
    castor::repack::RepackRequest* object = 
      dynamic_cast<castor::repack::RepackRequest*>(obj);
    object->setMachine(rset->getString(1));
    object->setUserName(rset->getString(2));
    object->setCreationTime(rset->getInt64(3));
    object->setServiceclass(rset->getString(4));
    object->setPid(rset->getInt64(5));
    object->setCommand(rset->getInt(6));
    object->setPool(rset->getString(7));
    object->setStager(rset->getString(8));
    object->setGroupid(rset->getInt64(9));
    object->setUserid(rset->getInt64(10));
    object->setId(rset->getInt64(11));
    delete rset;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


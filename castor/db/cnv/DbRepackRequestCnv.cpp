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
#include "castor/repack/RepackAck.hpp"
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
"INSERT INTO RepackRequest (machine, userName, creationTime, pool, pid, svcclass, command, stager, userId, groupId, retryMax, id, repackack) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,ids_seq.nextval,:12) RETURNING id INTO :13";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbRepackRequestCnv::s_deleteStatementString =
"DELETE FROM RepackRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbRepackRequestCnv::s_selectStatementString =
"SELECT machine, userName, creationTime, pool, pid, svcclass, command, stager, userId, groupId, retryMax, id, repackack FROM RepackRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbRepackRequestCnv::s_updateStatementString =
"UPDATE RepackRequest SET machine = :1, userName = :2, pool = :3, pid = :4, svcclass = :5, command = :6, stager = :7, userId = :8, groupId = :9, retryMax = :10 WHERE id = :11";

/// SQL statement for type storage
const std::string castor::db::cnv::DbRepackRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbRepackRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member repacksubrequest
const std::string castor::db::cnv::DbRepackRequestCnv::s_selectRepackSubRequestStatementString =
"SELECT id FROM RepackSubRequest WHERE repackrequest = :1 FOR UPDATE";

/// SQL delete statement for member repacksubrequest
const std::string castor::db::cnv::DbRepackRequestCnv::s_deleteRepackSubRequestStatementString =
"UPDATE RepackSubRequest SET repackrequest = 0 WHERE id = :1";

/// SQL remote update statement for member repacksubrequest
const std::string castor::db::cnv::DbRepackRequestCnv::s_remoteUpdateRepackSubRequestStatementString =
"UPDATE RepackSubRequest SET repackrequest = :1 WHERE id = :2";

/// SQL existence statement for member repackack
const std::string castor::db::cnv::DbRepackRequestCnv::s_checkRepackAckExistStatementString =
"SELECT id FROM RepackAck WHERE id = :1";

/// SQL update statement for member repackack
const std::string castor::db::cnv::DbRepackRequestCnv::s_updateRepackAckStatementString =
"UPDATE RepackRequest SET repackack = :1 WHERE id = :2";

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
  m_remoteUpdateRepackSubRequestStatement(0),
  m_checkRepackAckExistStatement(0),
  m_updateRepackAckStatement(0) {}

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
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_deleteRepackSubRequestStatement) delete m_deleteRepackSubRequestStatement;
    if(m_selectRepackSubRequestStatement) delete m_selectRepackSubRequestStatement;
    if(m_remoteUpdateRepackSubRequestStatement) delete m_remoteUpdateRepackSubRequestStatement;
    if(m_checkRepackAckExistStatement) delete m_checkRepackAckExistStatement;
    if(m_updateRepackAckStatement) delete m_updateRepackAckStatement;
  } catch (castor::exception::Exception ignored) {};
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
  m_checkRepackAckExistStatement = 0;
  m_updateRepackAckStatement = 0;
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
    case castor::OBJ_RepackAck :
      fillRepRepackAck(obj);
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
  std::set<int> repacksubrequestList;
  m_selectRepackSubRequestStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectRepackSubRequestStatement->executeQuery();
  while (rset->next()) {
    repacksubrequestList.insert(rset->getInt(1));
  }
  delete rset;
  // update repacksubrequest and create new ones
  for (std::vector<castor::repack::RepackSubRequest*>::iterator it = obj->repacksubrequest().begin();
       it != obj->repacksubrequest().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_RepackRequest);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateRepackSubRequestStatement) {
        m_remoteUpdateRepackSubRequestStatement = createStatement(s_remoteUpdateRepackSubRequestStatementString);
      }
      // Update remote object
      m_remoteUpdateRepackSubRequestStatement->setUInt64(1, obj->id());
      m_remoteUpdateRepackSubRequestStatement->setUInt64(2, (*it)->id());
      m_remoteUpdateRepackSubRequestStatement->execute();
      std::set<int>::iterator item;
      if ((item = repacksubrequestList.find((*it)->id())) != repacksubrequestList.end()) {
        repacksubrequestList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = repacksubrequestList.begin();
       it != repacksubrequestList.end();
       it++) {
    if (0 == m_deleteRepackSubRequestStatement) {
      m_deleteRepackSubRequestStatement = createStatement(s_deleteRepackSubRequestStatementString);
    }
    m_deleteRepackSubRequestStatement->setUInt64(1, *it);
    m_deleteRepackSubRequestStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepRepackAck
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::fillRepRepackAck(castor::repack::RepackRequest* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->repackack()) {
    // Check checkRepackAckExist statement
    if (0 == m_checkRepackAckExistStatement) {
      m_checkRepackAckExistStatement = createStatement(s_checkRepackAckExistStatementString);
    }
    // retrieve the object from the database
    m_checkRepackAckExistStatement->setUInt64(1, obj->repackack()->id());
    castor::db::IDbResultSet *rset = m_checkRepackAckExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->repackack(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateRepackAckStatement) {
    m_updateRepackAckStatement = createStatement(s_updateRepackAckStatementString);
  }
  // Update local object
  m_updateRepackAckStatement->setUInt64(1, 0 == obj->repackack() ? 0 : obj->repackack()->id());
  m_updateRepackAckStatement->setUInt64(2, obj->id());
  m_updateRepackAckStatement->execute();
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
  case castor::OBJ_RepackAck :
    fillObjRepackAck(obj);
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
  std::set<int> repacksubrequestList;
  m_selectRepackSubRequestStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectRepackSubRequestStatement->executeQuery();
  while (rset->next()) {
    repacksubrequestList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::repack::RepackSubRequest*> toBeDeleted;
  for (std::vector<castor::repack::RepackSubRequest*>::iterator it = obj->repacksubrequest().begin();
       it != obj->repacksubrequest().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = repacksubrequestList.find((*it)->id())) == repacksubrequestList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      repacksubrequestList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::repack::RepackSubRequest*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeRepacksubrequest(*it);
    (*it)->setRepackrequest(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = repacksubrequestList.begin();
       it != repacksubrequestList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::repack::RepackSubRequest* remoteObj = 
      dynamic_cast<castor::repack::RepackSubRequest*>(item);
    obj->addRepacksubrequest(remoteObj);
    remoteObj->setRepackrequest(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjRepackAck
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackRequestCnv::fillObjRepackAck(castor::repack::RepackRequest* obj)
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
  u_signed64 repackackId = rset->getInt64(13);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->repackack() &&
      (0 == repackackId ||
       obj->repackack()->id() != repackackId)) {
    obj->repackack()->removeRepackrequest(obj);
    obj->setRepackack(0);
  }
  // Update object or create new one
  if (0 != repackackId) {
    if (0 == obj->repackack()) {
      obj->setRepackack
        (dynamic_cast<castor::repack::RepackAck*>
         (cnvSvc()->getObjFromId(repackackId)));
    } else {
      cnvSvc()->updateObj(obj->repackack());
    }
    obj->repackack()->addRepackrequest(obj);
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
      m_insertStatement->registerOutParam(13, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->machine());
    m_insertStatement->setString(2, obj->userName());
    m_insertStatement->setInt(3, time(0));
    m_insertStatement->setString(4, obj->pool());
    m_insertStatement->setUInt64(5, obj->pid());
    m_insertStatement->setString(6, obj->svcclass());
    m_insertStatement->setInt(7, obj->command());
    m_insertStatement->setString(8, obj->stager());
    m_insertStatement->setInt(9, obj->userId());
    m_insertStatement->setInt(10, obj->groupId());
    m_insertStatement->setInt(11, obj->retryMax());
    m_insertStatement->setUInt64(12, (type == OBJ_RepackAck && obj->repackack() != 0) ? obj->repackack()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(13));
    m_storeTypeStatement->setUInt64(1, obj->id());
    m_storeTypeStatement->setUInt64(2, obj->type());
    m_storeTypeStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  machine : " << obj->machine() << std::endl
                    << "  userName : " << obj->userName() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  pool : " << obj->pool() << std::endl
                    << "  pid : " << obj->pid() << std::endl
                    << "  svcclass : " << obj->svcclass() << std::endl
                    << "  command : " << obj->command() << std::endl
                    << "  stager : " << obj->stager() << std::endl
                    << "  userId : " << obj->userId() << std::endl
                    << "  groupId : " << obj->groupId() << std::endl
                    << "  retryMax : " << obj->retryMax() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  repackack : " << obj->repackack() << std::endl;
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
    m_updateStatement->setString(3, obj->pool());
    m_updateStatement->setUInt64(4, obj->pid());
    m_updateStatement->setString(5, obj->svcclass());
    m_updateStatement->setInt(6, obj->command());
    m_updateStatement->setString(7, obj->stager());
    m_updateStatement->setInt(8, obj->userId());
    m_updateStatement->setInt(9, obj->groupId());
    m_updateStatement->setInt(10, obj->retryMax());
    m_updateStatement->setUInt64(11, obj->id());
    m_updateStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
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
    m_deleteTypeStatement->setUInt64(1, obj->id());
    m_deleteTypeStatement->execute();
    m_deleteStatement->setUInt64(1, obj->id());
    m_deleteStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
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
    m_selectStatement->setUInt64(1, ad->target());
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
    object->setCreationTime(rset->getUInt64(3));
    object->setPool(rset->getString(4));
    object->setPid(rset->getUInt64(5));
    object->setSvcclass(rset->getString(6));
    object->setCommand(rset->getInt(7));
    object->setStager(rset->getString(8));
    object->setUserId(rset->getInt(9));
    object->setGroupId(rset->getInt(10));
    object->setRetryMax(rset->getInt(11));
    object->setId(rset->getUInt64(12));
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
void castor::db::cnv::DbRepackRequestCnv::updateObj(castor::IObject* obj)
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
    castor::repack::RepackRequest* object = 
      dynamic_cast<castor::repack::RepackRequest*>(obj);
    object->setMachine(rset->getString(1));
    object->setUserName(rset->getString(2));
    object->setCreationTime(rset->getUInt64(3));
    object->setPool(rset->getString(4));
    object->setPid(rset->getUInt64(5));
    object->setSvcclass(rset->getString(6));
    object->setCommand(rset->getInt(7));
    object->setStager(rset->getString(8));
    object->setUserId(rset->getInt(9));
    object->setGroupId(rset->getInt(10));
    object->setRetryMax(rset->getInt(11));
    object->setId(rset->getUInt64(12));
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


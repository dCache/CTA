/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbBWUserCnv.cpp
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
#include "DbBWUserCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/bwlist/BWUser.hpp"
#include "castor/bwlist/ChangePrivilege.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbBWUserCnv>* s_factoryDbBWUserCnv =
  new castor::CnvFactory<castor::db::cnv::DbBWUserCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbBWUserCnv::s_insertStatementString =
"INSERT INTO BWUser (uid, gid, id, request) VALUES (:1,:2,ids_seq.nextval,:3) RETURNING id INTO :4";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbBWUserCnv::s_deleteStatementString =
"DELETE FROM BWUser WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbBWUserCnv::s_selectStatementString =
"SELECT uid, gid, id, request FROM BWUser WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbBWUserCnv::s_updateStatementString =
"UPDATE BWUser SET uid = :1, gid = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::cnv::DbBWUserCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbBWUserCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member request
const std::string castor::db::cnv::DbBWUserCnv::s_checkChangePrivilegeExistStatementString =
"SELECT id FROM ChangePrivilege WHERE id = :1";

/// SQL update statement for member request
const std::string castor::db::cnv::DbBWUserCnv::s_updateChangePrivilegeStatementString =
"UPDATE BWUser SET request = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbBWUserCnv::DbBWUserCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkChangePrivilegeExistStatement(0),
  m_updateChangePrivilegeStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbBWUserCnv::~DbBWUserCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbBWUserCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_checkChangePrivilegeExistStatement) delete m_checkChangePrivilegeExistStatement;
    if(m_updateChangePrivilegeStatement) delete m_updateChangePrivilegeStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkChangePrivilegeExistStatement = 0;
  m_updateChangePrivilegeStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbBWUserCnv::ObjType() {
  return castor::bwlist::BWUser::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbBWUserCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbBWUserCnv::fillRep(castor::IAddress* address,
                                           castor::IObject* object,
                                           unsigned int type,
                                           bool endTransaction)
  throw (castor::exception::Exception) {
  castor::bwlist::BWUser* obj = 
    dynamic_cast<castor::bwlist::BWUser*>(object);
  try {
    switch (type) {
    case castor::OBJ_ChangePrivilege :
      fillRepChangePrivilege(obj);
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
// fillRepChangePrivilege
//------------------------------------------------------------------------------
void castor::db::cnv::DbBWUserCnv::fillRepChangePrivilege(castor::bwlist::BWUser* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->request()) {
    // Check checkChangePrivilegeExist statement
    if (0 == m_checkChangePrivilegeExistStatement) {
      m_checkChangePrivilegeExistStatement = createStatement(s_checkChangePrivilegeExistStatementString);
    }
    // retrieve the object from the database
    m_checkChangePrivilegeExistStatement->setUInt64(1, obj->request()->id());
    castor::db::IDbResultSet *rset = m_checkChangePrivilegeExistStatement->executeQuery();
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
  if (0 == m_updateChangePrivilegeStatement) {
    m_updateChangePrivilegeStatement = createStatement(s_updateChangePrivilegeStatementString);
  }
  // Update local object
  m_updateChangePrivilegeStatement->setUInt64(1, 0 == obj->request() ? 0 : obj->request()->id());
  m_updateChangePrivilegeStatement->setUInt64(2, obj->id());
  m_updateChangePrivilegeStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbBWUserCnv::fillObj(castor::IAddress* address,
                                           castor::IObject* object,
                                           unsigned int type,
                                           bool endTransaction)
  throw (castor::exception::Exception) {
  castor::bwlist::BWUser* obj = 
    dynamic_cast<castor::bwlist::BWUser*>(object);
  switch (type) {
  case castor::OBJ_ChangePrivilege :
    fillObjChangePrivilege(obj);
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
// fillObjChangePrivilege
//------------------------------------------------------------------------------
void castor::db::cnv::DbBWUserCnv::fillObjChangePrivilege(castor::bwlist::BWUser* obj)
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
    obj->request()->removeUsers(obj);
    obj->setRequest(0);
  }
  // Update object or create new one
  if (0 != requestId) {
    if (0 == obj->request()) {
      obj->setRequest
        (dynamic_cast<castor::bwlist::ChangePrivilege*>
         (cnvSvc()->getObjFromId(requestId)));
    } else {
      cnvSvc()->updateObj(obj->request());
    }
    obj->request()->addUsers(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbBWUserCnv::createRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             bool endTransaction,
                                             unsigned int type)
  throw (castor::exception::Exception) {
  castor::bwlist::BWUser* obj = 
    dynamic_cast<castor::bwlist::BWUser*>(object);
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
    m_insertStatement->setInt(1, obj->uid());
    m_insertStatement->setInt(2, obj->gid());
    m_insertStatement->setUInt64(3, (type == OBJ_ChangePrivilege && obj->request() != 0) ? obj->request()->id() : 0);
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
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  uid : " << obj->uid() << std::endl
                    << "  gid : " << obj->gid() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  request : " << obj->request() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbBWUserCnv::updateRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::bwlist::BWUser* obj = 
    dynamic_cast<castor::bwlist::BWUser*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->uid());
    m_updateStatement->setInt(2, obj->gid());
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
void castor::db::cnv::DbBWUserCnv::deleteRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::bwlist::BWUser* obj = 
    dynamic_cast<castor::bwlist::BWUser*>(object);
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
    if (obj->request() != 0) {
      cnvSvc()->deleteRep(0, obj->request(), false);
    }
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
castor::IObject* castor::db::cnv::DbBWUserCnv::createObj(castor::IAddress* address)
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
    castor::bwlist::BWUser* object = new castor::bwlist::BWUser();
    // Now retrieve and set members
    object->setUid(rset->getInt(1));
    object->setGid(rset->getInt(2));
    object->setId(rset->getUInt64(3));
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
void castor::db::cnv::DbBWUserCnv::updateObj(castor::IObject* obj)
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
    castor::bwlist::BWUser* object = 
      dynamic_cast<castor::bwlist::BWUser*>(obj);
    object->setUid(rset->getInt(1));
    object->setGid(rset->getInt(2));
    object->setId(rset->getUInt64(3));
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


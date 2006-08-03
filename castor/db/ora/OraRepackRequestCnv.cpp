/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/ora/OraRepackRequestCnv.cpp
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
#include "OraRepackRequestCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
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
static castor::CnvFactory<castor::db::ora::OraRepackRequestCnv>* s_factoryOraRepackRequestCnv =
  new castor::CnvFactory<castor::db::ora::OraRepackRequestCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraRepackRequestCnv::s_insertStatementString =
"INSERT INTO RepackRequest (machine, userName, creationTime, serviceclass, pid, command, id) VALUES (:1,:2,:3,:4,:5,:6,ids_seq.nextval) RETURNING id INTO :7";

/// SQL statement for request deletion
const std::string castor::db::ora::OraRepackRequestCnv::s_deleteStatementString =
"DELETE FROM RepackRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraRepackRequestCnv::s_selectStatementString =
"SELECT machine, userName, creationTime, serviceclass, pid, command, id FROM RepackRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraRepackRequestCnv::s_updateStatementString =
"UPDATE RepackRequest SET machine = :1, userName = :2, serviceclass = :3, pid = :4, command = :5 WHERE id = :6";

/// SQL statement for type storage
const std::string castor::db::ora::OraRepackRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraRepackRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member subRequest
const std::string castor::db::ora::OraRepackRequestCnv::s_selectRepackSubRequestStatementString =
"SELECT id from RepackSubRequest WHERE requestID = :1 FOR UPDATE";

/// SQL delete statement for member subRequest
const std::string castor::db::ora::OraRepackRequestCnv::s_deleteRepackSubRequestStatementString =
"UPDATE RepackSubRequest SET requestID = 0 WHERE id = :1";

/// SQL remote update statement for member subRequest
const std::string castor::db::ora::OraRepackRequestCnv::s_remoteUpdateRepackSubRequestStatementString =
"UPDATE RepackSubRequest SET requestID = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraRepackRequestCnv::OraRepackRequestCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
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
castor::db::ora::OraRepackRequestCnv::~OraRepackRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_deleteRepackSubRequestStatement);
    deleteStatement(m_selectRepackSubRequestStatement);
    deleteStatement(m_remoteUpdateRepackSubRequestStatement);
  } catch (oracle::occi::SQLException e) {};
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
const unsigned int castor::db::ora::OraRepackRequestCnv::ObjType() {
  return castor::repack::RepackRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraRepackRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackRequestCnv::fillRep(castor::IAddress* address,
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
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex; 
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepRepackSubRequest
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackRequestCnv::fillRepRepackSubRequest(castor::repack::RepackRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // check select statement
  if (0 == m_selectRepackSubRequestStatement) {
    m_selectRepackSubRequestStatement = createStatement(s_selectRepackSubRequestStatementString);
  }
  // Get current database data
  std::set<int> subRequestList;
  m_selectRepackSubRequestStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectRepackSubRequestStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    subRequestList.insert(rset->getInt(1));
  }
  m_selectRepackSubRequestStatement->closeResultSet(rset);
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
      m_remoteUpdateRepackSubRequestStatement->setDouble(1, obj->id());
      m_remoteUpdateRepackSubRequestStatement->setDouble(2, (*it)->id());
      m_remoteUpdateRepackSubRequestStatement->executeUpdate();
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
    m_deleteRepackSubRequestStatement->setDouble(1, *it);
    m_deleteRepackSubRequestStatement->executeUpdate();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackRequestCnv::fillObj(castor::IAddress* address,
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
void castor::db::ora::OraRepackRequestCnv::fillObjRepackSubRequest(castor::repack::RepackRequest* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectRepackSubRequestStatement) {
    m_selectRepackSubRequestStatement = createStatement(s_selectRepackSubRequestStatementString);
  }
  // retrieve the object from the database
  std::set<int> subRequestList;
  m_selectRepackSubRequestStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectRepackSubRequestStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    subRequestList.insert(rset->getInt(1));
  }
  // Close ResultSet
  m_selectRepackSubRequestStatement->closeResultSet(rset);
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
void castor::db::ora::OraRepackRequestCnv::createRep(castor::IAddress* address,
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
      m_insertStatement->registerOutParam(7, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->machine());
    m_insertStatement->setString(2, obj->userName());
    m_insertStatement->setInt(3, time(0));
    m_insertStatement->setString(4, obj->serviceclass());
    m_insertStatement->setDouble(5, obj->pid());
    m_insertStatement->setInt(6, obj->command());
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(7));
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (oracle::occi::SQLException e) {
      handleException(e);
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "Error in insert request :"
                      << std::endl << e.what() << std::endl
                      << "Statement was :" << std::endl
                      << s_insertStatementString << std::endl
                      << "and parameters' values were :" << std::endl
                      << "  machine : " << obj->machine() << std::endl
                      << "  userName : " << obj->userName() << std::endl
                      << "  creationTime : " << obj->creationTime() << std::endl
                      << "  serviceclass : " << obj->serviceclass() << std::endl
                      << "  pid : " << obj->pid() << std::endl
                      << "  command : " << obj->command() << std::endl
                      << "  id : " << obj->id() << std::endl;
      throw ex;
    }
}

  //----------------------------------------------------------------------------
  // updateRep
  //----------------------------------------------------------------------------
  void castor::db::ora::OraRepackRequestCnv::updateRep(castor::IAddress* address,
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
      m_updateStatement->setDouble(4, obj->pid());
      m_updateStatement->setInt(5, obj->command());
      m_updateStatement->setDouble(6, obj->id());
      m_updateStatement->executeUpdate();
      if (autocommit) {
        cnvSvc()->commit();
      }
    } catch (oracle::occi::SQLException e) {
        handleException(e);
        castor::exception::InvalidArgument ex;
        ex.getMessage() << "Error in update request :"
                        << std::endl << e.what() << std::endl
                        << "Statement was :" << std::endl
                        << s_updateStatementString << std::endl
                        << "and id was " << obj->id() << std::endl;;
        throw ex;
      }
}

    //--------------------------------------------------------------------------
    // deleteRep
    //--------------------------------------------------------------------------
    void castor::db::ora::OraRepackRequestCnv::deleteRep(castor::IAddress* address,
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
        m_deleteTypeStatement->setDouble(1, obj->id());
        m_deleteTypeStatement->executeUpdate();
        m_deleteStatement->setDouble(1, obj->id());
        m_deleteStatement->executeUpdate();
        if (autocommit) {
          cnvSvc()->commit();
        }
      } catch (oracle::occi::SQLException e) {
          handleException(e);
          castor::exception::InvalidArgument ex;
          ex.getMessage() << "Error in delete request :"
                          << std::endl << e.what() << std::endl
                          << "Statement was :" << std::endl
                          << s_deleteStatementString << std::endl
                          << "and id was " << obj->id() << std::endl;;
          throw ex;
        }
}

      //------------------------------------------------------------------------
      // createObj
      //------------------------------------------------------------------------
      castor::IObject* castor::db::ora::OraRepackRequestCnv::createObj(castor::IAddress* address)
        throw (castor::exception::Exception) {
        castor::BaseAddress* ad = 
          dynamic_cast<castor::BaseAddress*>(address);
        try {
          // Check whether the statement is ok
          if (0 == m_selectStatement) {
            m_selectStatement = createStatement(s_selectStatementString);
          }
          // retrieve the object from the database
          m_selectStatement->setDouble(1, ad->target());
          oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
          if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
            castor::exception::NoEntry ex;
            ex.getMessage() << "No object found for id :" << ad->target();
            throw ex;
          }
          // create the new Object
          castor::repack::RepackRequest* object = new castor::repack::RepackRequest();
          // Now retrieve and set members
          object->setMachine(rset->getString(1));
          object->setUserName(rset->getString(2));
          object->setCreationTime(rset->getInt(3));
          object->setServiceclass(rset->getString(4));
          object->setPid((u_signed64)rset->getDouble(5));
          object->setCommand(rset->getInt(6));
          object->setId((u_signed64)rset->getDouble(7));
          m_selectStatement->closeResultSet(rset);
          return object;
        } catch (oracle::occi::SQLException e) {
            handleException(e);
            castor::exception::InvalidArgument ex;
            ex.getMessage() << "Error in select request :"
                            << std::endl << e.what() << std::endl
                            << "Statement was :" << std::endl
                            << s_selectStatementString << std::endl
                            << "and id was " << ad->target() << std::endl;;
            throw ex;
          }
        }

        //----------------------------------------------------------------------
        // updateObj
        //----------------------------------------------------------------------
        void castor::db::ora::OraRepackRequestCnv::updateObj(castor::IObject* obj)
          throw (castor::exception::Exception) {
          try {
            // Check whether the statement is ok
            if (0 == m_selectStatement) {
              m_selectStatement = createStatement(s_selectStatementString);
            }
            // retrieve the object from the database
            m_selectStatement->setDouble(1, obj->id());
            oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
            if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
              castor::exception::NoEntry ex;
              ex.getMessage() << "No object found for id :" << obj->id();
              throw ex;
            }
            // Now retrieve and set members
            castor::repack::RepackRequest* object = 
              dynamic_cast<castor::repack::RepackRequest*>(obj);
            object->setMachine(rset->getString(1));
            object->setUserName(rset->getString(2));
            object->setCreationTime(rset->getInt(3));
            object->setServiceclass(rset->getString(4));
            object->setPid((u_signed64)rset->getDouble(5));
            object->setCommand(rset->getInt(6));
            object->setId((u_signed64)rset->getDouble(7));
            m_selectStatement->closeResultSet(rset);
          } catch (oracle::occi::SQLException e) {
              handleException(e);
              castor::exception::InvalidArgument ex;
              ex.getMessage() << "Error in update request :"
                              << std::endl << e.what() << std::endl
                              << "Statement was :" << std::endl
                              << s_updateStatementString << std::endl
                              << "and id was " << obj->id() << std::endl;;
              throw ex;
            }
}


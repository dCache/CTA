/******************************************************************************
 *                      castor/db/ora/OraTapeCnv.cpp
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
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

// Include Files
#include "OraTapeCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IConverter.hpp"
#include "castor/IFactory.hpp"
#include "castor/IObject.hpp"
#include "castor/db/DbAddress.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/Segment.hpp"
#include "castor/stager/Stream.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/stager/TapeStatusCodes.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraTapeCnv> s_factoryOraTapeCnv;
const castor::IFactory<castor::IConverter>& OraTapeCnvFactory = 
  s_factoryOraTapeCnv;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraTapeCnv::s_insertStatementString =
"INSERT INTO rh_Tape (vid, side, tpmode, errMsgTxt, errorCode, severity, vwAddress, id, stream, status) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10)";

/// SQL statement for request deletion
const std::string castor::db::ora::OraTapeCnv::s_deleteStatementString =
"DELETE FROM rh_Tape WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraTapeCnv::s_selectStatementString =
"SELECT vid, side, tpmode, errMsgTxt, errorCode, severity, vwAddress, id, stream, status FROM rh_Tape WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraTapeCnv::s_updateStatementString =
"UPDATE rh_Tape SET vid = :1, side = :2, tpmode = :3, errMsgTxt = :4, errorCode = :5, severity = :6, vwAddress = :7, status = :8 WHERE id = :9";

/// SQL statement for type storage
const std::string castor::db::ora::OraTapeCnv::s_storeTypeStatementString =
"INSERT INTO rh_Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraTapeCnv::s_deleteTypeStatementString =
"DELETE FROM rh_Id2Type WHERE id = :1";

/// SQL update statement for member stream
const std::string castor::db::ora::OraTapeCnv::s_updateStreamStatementString =
"UPDATE rh_Tape SET stream = : 1 WHERE id = :2";

/// SQL select statement for member segments
const std::string castor::db::ora::OraTapeCnv::s_selectSegmentStatementString =
"SELECT id from rh_Segment WHERE tape = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraTapeCnv::OraTapeCnv() :
  OraBaseCnv(),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_updateStreamStatement(0),
  m_selectSegmentStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraTapeCnv::~OraTapeCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_updateStreamStatement);
    deleteStatement(m_selectSegmentStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_updateStreamStatement = 0;
  m_selectSegmentStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeCnv::ObjType() {
  return castor::stager::Tape::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::fillRep(castor::IAddress* address,
                                          castor::IObject* object,
                                          unsigned int type,
                                          bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
  switch (type) {
  case castor::OBJ_Stream :
    fillRepStream(obj);
    break;
  case castor::OBJ_Segment :
    fillRepSegment(obj);
    break;
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillRep called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
  if (autocommit) {
    cnvSvc()->getConnection()->commit();
  }
}

//------------------------------------------------------------------------------
// fillRepStream
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::fillRepStream(castor::stager::Tape* obj)
  throw (castor::exception::Exception) {
  // Check select statement
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
  u_signed64 streamId = (u_signed64)rset->getDouble(9);
  // Close resultset
  m_selectStatement->closeResultSet(rset);
  castor::db::DbAddress ad(streamId, " ", 0);
  // Check whether old object should be deleted
  if (0 != streamId &&
      0 != obj->stream() &&
      obj->stream()->id() != streamId) {
    cnvSvc()->deleteRepByAddress(&ad, false);
    streamId = 0;
  }
  // Update remote object or create new one
  if (streamId == 0) {
    if (0 != obj->stream()) {
      cnvSvc()->createRep(&ad, obj->stream(), false, OBJ_Tape);
    }
  } else {
    cnvSvc()->updateRep(&ad, obj->stream(), false);
  }
  // Check update statement
  if (0 == m_updateStreamStatement) {
    m_updateStreamStatement = createStatement(s_updateStreamStatementString);
  }
  // Update local object
  m_updateStreamStatement->setDouble(1, 0 == obj->stream() ? 0 : obj->stream()->id());
  m_updateStreamStatement->setDouble(2, obj->id());
  m_updateStreamStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillRepSegment
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::fillRepSegment(castor::stager::Tape* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectSegmentStatement) {
    m_selectSegmentStatement = createStatement(s_selectSegmentStatementString);
  }
  // Get current database data
  std::set<int> segmentsList;
  m_selectSegmentStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectSegmentStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    segmentsList.insert(rset->getInt(1));
  }
  m_selectSegmentStatement->closeResultSet(rset);
  // update segments and create new ones
  for (std::vector<castor::stager::Segment*>::iterator it = obj->segments().begin();
       it != obj->segments().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = segmentsList.find((*it)->id())) == segmentsList.end()) {
      cnvSvc()->createRep(0, *it, false, OBJ_Tape);
    } else {
      segmentsList.erase(item);
      cnvSvc()->updateRep(0, *it, false);
    }
  }
  // Delete old data
  for (std::set<int>::iterator it = segmentsList.begin();
       it != segmentsList.end();
       it++) {
    castor::db::DbAddress ad(*it, " ", 0);
    cnvSvc()->deleteRepByAddress(&ad, false);
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::fillObj(castor::IAddress* address,
                                          castor::IObject* object,
                                          unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
  switch (type) {
  case castor::OBJ_Stream :
    fillObjStream(obj);
    break;
  case castor::OBJ_Segment :
    fillObjSegment(obj);
    break;
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillObj called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillObjStream
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::fillObjStream(castor::stager::Tape* obj)
  throw (castor::exception::Exception) {
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
  u_signed64 streamId = (u_signed64)rset->getDouble(9);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->stream() &&
      (0 == streamId ||
       obj->stream()->id() != streamId)) {
    delete obj->stream();
    obj->setStream(0);
  }
  // Update object or create new one
  if (0 != streamId) {
    if (0 == obj->stream()) {
      obj->setStream
        (dynamic_cast<castor::stager::Stream*>
         (cnvSvc()->getObjFromId(streamId)));
    } else if (obj->stream()->id() == streamId) {
      cnvSvc()->updateObj(obj->stream());
    }
  }
}

//------------------------------------------------------------------------------
// fillObjSegment
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::fillObjSegment(castor::stager::Tape* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectSegmentStatement) {
    m_selectSegmentStatement = createStatement(s_selectSegmentStatementString);
  }
  // retrieve the object from the database
  std::set<int> segmentsList;
  m_selectSegmentStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectSegmentStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    segmentsList.insert(rset->getInt(1));
  }
  // Close ResultSet
  m_selectSegmentStatement->closeResultSet(rset);
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::Segment*> toBeDeleted;
  for (std::vector<castor::stager::Segment*>::iterator it = obj->segments().begin();
       it != obj->segments().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = segmentsList.find((*it)->id())) == segmentsList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      segmentsList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::Segment*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeSegments(*it);
    delete (*it);
  }
  // Create new objects
  for (std::set<int>::iterator it = segmentsList.begin();
       it != segmentsList.end();
       it++) {
    IObject* item = cnvSvc()->getObjFromId(*it);
    obj->addSegments(dynamic_cast<castor::stager::Segment*>(item));
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::createRep(castor::IAddress* address,
                                            castor::IObject* object,
                                            bool autocommit,
                                            unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Get an id for the new object
    obj->setId(cnvSvc()->getIds(1));
    // Now Save the current object
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    m_insertStatement->setString(1, obj->vid());
    m_insertStatement->setInt(2, obj->side());
    m_insertStatement->setInt(3, obj->tpmode());
    m_insertStatement->setString(4, obj->errMsgTxt());
    m_insertStatement->setInt(5, obj->errorCode());
    m_insertStatement->setInt(6, obj->severity());
    m_insertStatement->setString(7, obj->vwAddress());
    m_insertStatement->setDouble(8, obj->id());
    m_insertStatement->setDouble(9, (type == OBJ_Stream && obj->stream() != 0) ? obj->stream()->id() : 0);
    m_updateStatement->setInt(10, (int)obj->status());
    m_insertStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.what() << std::endl
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
                    << "  stream : " << obj->stream() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::updateRep(castor::IAddress* address,
                                            castor::IObject* object,
                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
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
    m_updateStatement->setDouble(9, obj->id());
    m_updateStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::deleteRep(castor::IAddress* address,
                                            castor::IObject* object,
                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
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
    for (std::vector<castor::stager::Segment*>::iterator it = obj->segments().begin();
         it != obj->segments().end();
         it++) {
      cnvSvc()->deleteRep(0, *it, false);
    }
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in delete request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_deleteStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::db::ora::OraTapeCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::db::DbAddress* ad = 
    dynamic_cast<castor::db::DbAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setDouble(1, ad->id());
    oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << ad->id();
      throw ex;
    }
    // create the new Object
    castor::stager::Tape* object = new castor::stager::Tape();
    // Now retrieve and set members
    object->setVid(rset->getString(1));
    object->setSide(rset->getInt(2));
    object->setTpmode(rset->getInt(3));
    object->setErrMsgTxt(rset->getString(4));
    object->setErrorCode(rset->getInt(5));
    object->setSeverity(rset->getInt(6));
    object->setVwAddress(rset->getString(7));
    object->setId((u_signed64)rset->getDouble(8));
    object->setStatus((enum castor::stager::TapeStatusCodes)rset->getInt(10));
    m_selectStatement->closeResultSet(rset);
    return object;
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in select request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_selectStatementString << std::endl
                    << "and id was " << ad->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeCnv::updateObj(castor::IObject* obj)
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
    castor::stager::Tape* object = 
      dynamic_cast<castor::stager::Tape*>(obj);
    object->setVid(rset->getString(1));
    object->setSide(rset->getInt(2));
    object->setTpmode(rset->getInt(3));
    object->setErrMsgTxt(rset->getString(4));
    object->setErrorCode(rset->getInt(5));
    object->setSeverity(rset->getInt(6));
    object->setVwAddress(rset->getString(7));
    object->setId((u_signed64)rset->getDouble(8));
    object->setStatus((enum castor::stager::TapeStatusCodes)rset->getInt(10));
    m_selectStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


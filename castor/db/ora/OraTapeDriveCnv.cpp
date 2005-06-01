/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/ora/OraTapeDriveCnv.cpp
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
#include "OraTapeDriveCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvFactory.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/vdqm/ExtendedDeviceGroup.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDriveStatusCodes.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraTapeDriveCnv> s_factoryOraTapeDriveCnv;
const castor::ICnvFactory& OraTapeDriveCnvFactory = 
  s_factoryOraTapeDriveCnv;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraTapeDriveCnv::s_insertStatementString =
"INSERT INTO TapeDrive (jobID, creationTime, resettime, usecount, errcount, transferredMB, totalMB, dedicate, newDedicate, is_uid, is_gid, is_name, no_uid, no_gid, no_name, no_host, no_vid, no_mode, no_date, no_time, no_age, euid, egid, name, id, tape, status, tapeServer) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,ids_seq.nextval,:25,:26,:27) RETURNING id INTO :28";

/// SQL statement for request deletion
const std::string castor::db::ora::OraTapeDriveCnv::s_deleteStatementString =
"DELETE FROM TapeDrive WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraTapeDriveCnv::s_selectStatementString =
"SELECT jobID, creationTime, resettime, usecount, errcount, transferredMB, totalMB, dedicate, newDedicate, is_uid, is_gid, is_name, no_uid, no_gid, no_name, no_host, no_vid, no_mode, no_date, no_time, no_age, euid, egid, name, id, tape, status, tapeServer FROM TapeDrive WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraTapeDriveCnv::s_updateStatementString =
"UPDATE TapeDrive SET jobID = :1, resettime = :2, usecount = :3, errcount = :4, transferredMB = :5, totalMB = :6, dedicate = :7, newDedicate = :8, is_uid = :9, is_gid = :10, is_name = :11, no_uid = :12, no_gid = :13, no_name = :14, no_host = :15, no_vid = :16, no_mode = :17, no_date = :18, no_time = :19, no_age = :20, euid = :21, egid = :22, name = :23, status = :24 WHERE id = :25";

/// SQL statement for type storage
const std::string castor::db::ora::OraTapeDriveCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraTapeDriveCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member tape
const std::string castor::db::ora::OraTapeDriveCnv::s_selectTapeStatementString =
"SELECT id from Tape WHERE  = :1 FOR UPDATE";

/// SQL delete statement for member tape
const std::string castor::db::ora::OraTapeDriveCnv::s_deleteTapeStatementString =
"UPDATE Tape SET  = 0 WHERE id = :1";

/// SQL remote update statement for member tape
const std::string castor::db::ora::OraTapeDriveCnv::s_remoteUpdateTapeStatementString =
"UPDATE Tape SET  = :1 WHERE id = :2";

/// SQL existence statement for member tape
const std::string castor::db::ora::OraTapeDriveCnv::s_checkTapeExistStatementString =
"SELECT id from Tape WHERE id = :1";

/// SQL update statement for member tape
const std::string castor::db::ora::OraTapeDriveCnv::s_updateTapeStatementString =
"UPDATE TapeDrive SET tape = :1 WHERE id = :2";

/// SQL insert statement for member extDevGrp
const std::string castor::db::ora::OraTapeDriveCnv::s_insertExtendedDeviceGroupStatementString =
"INSERT INTO TapeDrive2ExtendedDevic (Parent, Child) VALUES (:1, :2)";

/// SQL delete statement for member extDevGrp
const std::string castor::db::ora::OraTapeDriveCnv::s_deleteExtendedDeviceGroupStatementString =
"DELETE FROM TapeDrive2ExtendedDevic WHERE Parent = :1 AND Child = :2";

/// SQL select statement for member extDevGrp
// The FOR UPDATE is needed in order to avoid deletion
// of a segment after listing and before update/remove
const std::string castor::db::ora::OraTapeDriveCnv::s_selectExtendedDeviceGroupStatementString =
"SELECT Child FROM TapeDrive2ExtendedDevic WHERE Parent = :1 FOR UPDATE";

/// SQL existence statement for member tapeServer
const std::string castor::db::ora::OraTapeDriveCnv::s_checkTapeServerExistStatementString =
"SELECT id from TapeServer WHERE id = :1";

/// SQL update statement for member tapeServer
const std::string castor::db::ora::OraTapeDriveCnv::s_updateTapeServerStatementString =
"UPDATE TapeDrive SET tapeServer = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraTapeDriveCnv::OraTapeDriveCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectTapeStatement(0),
  m_deleteTapeStatement(0),
  m_remoteUpdateTapeStatement(0),
  m_checkTapeExistStatement(0),
  m_updateTapeStatement(0),
  m_insertExtendedDeviceGroupStatement(0),
  m_deleteExtendedDeviceGroupStatement(0),
  m_selectExtendedDeviceGroupStatement(0),
  m_checkTapeServerExistStatement(0),
  m_updateTapeServerStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraTapeDriveCnv::~OraTapeDriveCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_deleteTapeStatement);
    deleteStatement(m_selectTapeStatement);
    deleteStatement(m_remoteUpdateTapeStatement);
    deleteStatement(m_checkTapeExistStatement);
    deleteStatement(m_updateTapeStatement);
    deleteStatement(m_insertExtendedDeviceGroupStatement);
    deleteStatement(m_deleteExtendedDeviceGroupStatement);
    deleteStatement(m_selectExtendedDeviceGroupStatement);
    deleteStatement(m_checkTapeServerExistStatement);
    deleteStatement(m_updateTapeServerStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectTapeStatement = 0;
  m_deleteTapeStatement = 0;
  m_remoteUpdateTapeStatement = 0;
  m_checkTapeExistStatement = 0;
  m_updateTapeStatement = 0;
  m_insertExtendedDeviceGroupStatement = 0;
  m_deleteExtendedDeviceGroupStatement = 0;
  m_selectExtendedDeviceGroupStatement = 0;
  m_checkTapeServerExistStatement = 0;
  m_updateTapeServerStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeDriveCnv::ObjType() {
  return castor::vdqm::TapeDrive::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeDriveCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::fillRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDrive* obj = 
    dynamic_cast<castor::vdqm::TapeDrive*>(object);
  try {
    switch (type) {
    case castor::OBJ_Tape :
      fillRepTape(obj);
      break;
    case castor::OBJ_ExtendedDeviceGroup :
      fillRepExtendedDeviceGroup(obj);
      break;
    case castor::OBJ_TapeServer :
      fillRepTapeServer(obj);
      break;
    default :
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "fillRep called for type " << type 
                      << " on object of type " << obj->type() 
                      << ". This is meaningless.";
      throw ex;
    }
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepTape
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::fillRepTape(castor::vdqm::TapeDrive* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // Check selectTape statement
  if (0 == m_selectTapeStatement) {
    m_selectTapeStatement = createStatement(s_selectTapeStatementString);
  }
  // retrieve the object from the database
  m_selectTapeStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectTapeStatement->executeQuery();
  if (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    u_signed64 tapeId = (u_signed64)rset->getDouble(1);
    if (0 != tapeId &&
        (0 == obj->tape() ||
         obj->tape()->id() != tapeId)) {
      if (0 == m_deleteTapeStatement) {
        m_deleteTapeStatement = createStatement(s_deleteTapeStatementString);
      }
      m_deleteTapeStatement->setDouble(1, tapeId);
      m_deleteTapeStatement->executeUpdate();
    }
  }
  // Close resultset
  m_selectTapeStatement->closeResultSet(rset);
  if (0 != obj->tape()) {
    // Check checkTapeExist statement
    if (0 == m_checkTapeExistStatement) {
      m_checkTapeExistStatement = createStatement(s_checkTapeExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeExistStatement->setDouble(1, obj->tape()->id());
    oracle::occi::ResultSet *rset = m_checkTapeExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("OraCnvSvc");
      ad.setCnvSvcType(castor::SVC_ORACNV);
      cnvSvc()->createRep(&ad, obj->tape(), false, OBJ_TapeDrive);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateTapeStatement) {
        m_remoteUpdateTapeStatement = createStatement(s_remoteUpdateTapeStatementString);
      }
      // Update remote object
      m_remoteUpdateTapeStatement->setDouble(1, obj->id());
      m_remoteUpdateTapeStatement->setDouble(2, obj->tape()->id());
      m_remoteUpdateTapeStatement->executeUpdate();
    }
    // Close resultset
    m_checkTapeExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateTapeStatement) {
    m_updateTapeStatement = createStatement(s_updateTapeStatementString);
  }
  // Update local object
  m_updateTapeStatement->setDouble(1, 0 == obj->tape() ? 0 : obj->tape()->id());
  m_updateTapeStatement->setDouble(2, obj->id());
  m_updateTapeStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillRepExtendedDeviceGroup
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::fillRepExtendedDeviceGroup(castor::vdqm::TapeDrive* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // check select statement
  if (0 == m_selectExtendedDeviceGroupStatement) {
    m_selectExtendedDeviceGroupStatement = createStatement(s_selectExtendedDeviceGroupStatementString);
  }
  // Get current database data
  std::set<int> extDevGrpList;
  m_selectExtendedDeviceGroupStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectExtendedDeviceGroupStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    extDevGrpList.insert(rset->getInt(1));
  }
  m_selectExtendedDeviceGroupStatement->closeResultSet(rset);
  // update extDevGrp and create new ones
  for (std::vector<castor::vdqm::ExtendedDeviceGroup*>::iterator it = obj->extDevGrp().begin();
       it != obj->extDevGrp().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false);
    }
    std::set<int>::iterator item;
    if ((item = extDevGrpList.find((*it)->id())) != extDevGrpList.end()) {
      extDevGrpList.erase(item);
    } else {
      if (0 == m_insertExtendedDeviceGroupStatement) {
        m_insertExtendedDeviceGroupStatement = createStatement(s_insertExtendedDeviceGroupStatementString);
      }
      m_insertExtendedDeviceGroupStatement->setDouble(1, obj->id());
      m_insertExtendedDeviceGroupStatement->setDouble(2, (*it)->id());
      m_insertExtendedDeviceGroupStatement->executeUpdate();
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = extDevGrpList.begin();
       it != extDevGrpList.end();
       it++) {
    if (0 == m_deleteExtendedDeviceGroupStatement) {
      m_deleteExtendedDeviceGroupStatement = createStatement(s_deleteExtendedDeviceGroupStatementString);
    }
    m_deleteExtendedDeviceGroupStatement->setDouble(1, obj->id());
    m_deleteExtendedDeviceGroupStatement->setDouble(2, *it);
    m_deleteExtendedDeviceGroupStatement->executeUpdate();
  }
}

//------------------------------------------------------------------------------
// fillRepTapeServer
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::fillRepTapeServer(castor::vdqm::TapeDrive* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->tapeServer()) {
    // Check checkTapeServerExist statement
    if (0 == m_checkTapeServerExistStatement) {
      m_checkTapeServerExistStatement = createStatement(s_checkTapeServerExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeServerExistStatement->setDouble(1, obj->tapeServer()->id());
    oracle::occi::ResultSet *rset = m_checkTapeServerExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("OraCnvSvc");
      ad.setCnvSvcType(castor::SVC_ORACNV);
      cnvSvc()->createRep(&ad, obj->tapeServer(), false);
    }
    // Close resultset
    m_checkTapeServerExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateTapeServerStatement) {
    m_updateTapeServerStatement = createStatement(s_updateTapeServerStatementString);
  }
  // Update local object
  m_updateTapeServerStatement->setDouble(1, 0 == obj->tapeServer() ? 0 : obj->tapeServer()->id());
  m_updateTapeServerStatement->setDouble(2, obj->id());
  m_updateTapeServerStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::fillObj(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDrive* obj = 
    dynamic_cast<castor::vdqm::TapeDrive*>(object);
  switch (type) {
  case castor::OBJ_Tape :
    fillObjTape(obj);
    break;
  case castor::OBJ_ExtendedDeviceGroup :
    fillObjExtendedDeviceGroup(obj);
    break;
  case castor::OBJ_TapeServer :
    fillObjTapeServer(obj);
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
// fillObjTape
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::fillObjTape(castor::vdqm::TapeDrive* obj)
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
  u_signed64 tapeId = (u_signed64)rset->getDouble(26);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->tape() &&
      (0 == tapeId ||
       obj->tape()->id() != tapeId)) {
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
  }
}

//------------------------------------------------------------------------------
// fillObjExtendedDeviceGroup
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::fillObjExtendedDeviceGroup(castor::vdqm::TapeDrive* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectExtendedDeviceGroupStatement) {
    m_selectExtendedDeviceGroupStatement = createStatement(s_selectExtendedDeviceGroupStatementString);
  }
  // retrieve the object from the database
  std::set<int> extDevGrpList;
  m_selectExtendedDeviceGroupStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectExtendedDeviceGroupStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    extDevGrpList.insert(rset->getInt(1));
  }
  // Close ResultSet
  m_selectExtendedDeviceGroupStatement->closeResultSet(rset);
  // Update objects and mark old ones for deletion
  std::vector<castor::vdqm::ExtendedDeviceGroup*> toBeDeleted;
  for (std::vector<castor::vdqm::ExtendedDeviceGroup*>::iterator it = obj->extDevGrp().begin();
       it != obj->extDevGrp().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = extDevGrpList.find((*it)->id())) == extDevGrpList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      extDevGrpList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::vdqm::ExtendedDeviceGroup*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeExtDevGrp(*it);
  }
  // Create new objects
  for (std::set<int>::iterator it = extDevGrpList.begin();
       it != extDevGrpList.end();
       it++) {
    IObject* item = cnvSvc()->getObjFromId(*it);
    castor::vdqm::ExtendedDeviceGroup* remoteObj = 
      dynamic_cast<castor::vdqm::ExtendedDeviceGroup*>(item);
    obj->addExtDevGrp(remoteObj);
  }
}

//------------------------------------------------------------------------------
// fillObjTapeServer
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::fillObjTapeServer(castor::vdqm::TapeDrive* obj)
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
  u_signed64 tapeServerId = (u_signed64)rset->getDouble(28);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->tapeServer() &&
      (0 == tapeServerId ||
       obj->tapeServer()->id() != tapeServerId)) {
    obj->tapeServer()->removeTapeDrives(obj);
    obj->setTapeServer(0);
  }
  // Update object or create new one
  if (0 != tapeServerId) {
    if (0 == obj->tapeServer()) {
      obj->setTapeServer
        (dynamic_cast<castor::vdqm::TapeServer*>
         (cnvSvc()->getObjFromId(tapeServerId)));
    } else {
      cnvSvc()->updateObj(obj->tapeServer());
    }
    obj->tapeServer()->addTapeDrives(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::createRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit,
                                                 unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDrive* obj = 
    dynamic_cast<castor::vdqm::TapeDrive*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(28, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->jobID());
    m_insertStatement->setInt(2, time(0));
    m_insertStatement->setInt(3, obj->resettime());
    m_insertStatement->setInt(4, obj->usecount());
    m_insertStatement->setInt(5, obj->errcount());
    m_insertStatement->setInt(6, obj->transferredMB());
    m_insertStatement->setDouble(7, obj->totalMB());
    m_insertStatement->setString(8, obj->dedicate());
    m_insertStatement->setString(9, obj->newDedicate());
    m_insertStatement->setInt(10, obj->is_uid());
    m_insertStatement->setInt(11, obj->is_gid());
    m_insertStatement->setInt(12, obj->is_name());
    m_insertStatement->setInt(13, obj->no_uid());
    m_insertStatement->setInt(14, obj->no_gid());
    m_insertStatement->setInt(15, obj->no_name());
    m_insertStatement->setInt(16, obj->no_host());
    m_insertStatement->setInt(17, obj->no_vid());
    m_insertStatement->setInt(18, obj->no_mode());
    m_insertStatement->setInt(19, obj->no_date());
    m_insertStatement->setInt(20, obj->no_time());
    m_insertStatement->setInt(21, obj->no_age());
    m_insertStatement->setInt(22, obj->euid());
    m_insertStatement->setInt(23, obj->egid());
    m_insertStatement->setString(24, obj->name());
    m_insertStatement->setDouble(25, (type == OBJ_Tape && obj->tape() != 0) ? obj->tape()->id() : 0);
    m_insertStatement->setInt(26, (int)obj->status());
    m_insertStatement->setDouble(27, (type == OBJ_TapeServer && obj->tapeServer() != 0) ? obj->tapeServer()->id() : 0);
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(28));
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
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
                    << "  jobID : " << obj->jobID() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  resettime : " << obj->resettime() << std::endl
                    << "  usecount : " << obj->usecount() << std::endl
                    << "  errcount : " << obj->errcount() << std::endl
                    << "  transferredMB : " << obj->transferredMB() << std::endl
                    << "  totalMB : " << obj->totalMB() << std::endl
                    << "  dedicate : " << obj->dedicate() << std::endl
                    << "  newDedicate : " << obj->newDedicate() << std::endl
                    << "  is_uid : " << obj->is_uid() << std::endl
                    << "  is_gid : " << obj->is_gid() << std::endl
                    << "  is_name : " << obj->is_name() << std::endl
                    << "  no_uid : " << obj->no_uid() << std::endl
                    << "  no_gid : " << obj->no_gid() << std::endl
                    << "  no_name : " << obj->no_name() << std::endl
                    << "  no_host : " << obj->no_host() << std::endl
                    << "  no_vid : " << obj->no_vid() << std::endl
                    << "  no_mode : " << obj->no_mode() << std::endl
                    << "  no_date : " << obj->no_date() << std::endl
                    << "  no_time : " << obj->no_time() << std::endl
                    << "  no_age : " << obj->no_age() << std::endl
                    << "  euid : " << obj->euid() << std::endl
                    << "  egid : " << obj->egid() << std::endl
                    << "  name : " << obj->name() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  tape : " << obj->tape() << std::endl
                    << "  status : " << obj->status() << std::endl
                    << "  tapeServer : " << obj->tapeServer() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::updateRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDrive* obj = 
    dynamic_cast<castor::vdqm::TapeDrive*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->jobID());
    m_updateStatement->setInt(2, obj->resettime());
    m_updateStatement->setInt(3, obj->usecount());
    m_updateStatement->setInt(4, obj->errcount());
    m_updateStatement->setInt(5, obj->transferredMB());
    m_updateStatement->setDouble(6, obj->totalMB());
    m_updateStatement->setString(7, obj->dedicate());
    m_updateStatement->setString(8, obj->newDedicate());
    m_updateStatement->setInt(9, obj->is_uid());
    m_updateStatement->setInt(10, obj->is_gid());
    m_updateStatement->setInt(11, obj->is_name());
    m_updateStatement->setInt(12, obj->no_uid());
    m_updateStatement->setInt(13, obj->no_gid());
    m_updateStatement->setInt(14, obj->no_name());
    m_updateStatement->setInt(15, obj->no_host());
    m_updateStatement->setInt(16, obj->no_vid());
    m_updateStatement->setInt(17, obj->no_mode());
    m_updateStatement->setInt(18, obj->no_date());
    m_updateStatement->setInt(19, obj->no_time());
    m_updateStatement->setInt(20, obj->no_age());
    m_updateStatement->setInt(21, obj->euid());
    m_updateStatement->setInt(22, obj->egid());
    m_updateStatement->setString(23, obj->name());
    m_updateStatement->setInt(24, (int)obj->status());
    m_updateStatement->setDouble(25, obj->id());
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
void castor::db::ora::OraTapeDriveCnv::deleteRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDrive* obj = 
    dynamic_cast<castor::vdqm::TapeDrive*>(object);
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
castor::IObject* castor::db::ora::OraTapeDriveCnv::createObj(castor::IAddress* address)
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
    castor::vdqm::TapeDrive* object = new castor::vdqm::TapeDrive();
    // Now retrieve and set members
    object->setJobID(rset->getInt(1));
    object->setCreationTime(rset->getInt(2));
    object->setResettime(rset->getInt(3));
    object->setUsecount(rset->getInt(4));
    object->setErrcount(rset->getInt(5));
    object->setTransferredMB(rset->getInt(6));
    object->setTotalMB((u_signed64)rset->getDouble(7));
    object->setDedicate(rset->getString(8));
    object->setNewDedicate(rset->getString(9));
    object->setIs_uid(rset->getInt(10));
    object->setIs_gid(rset->getInt(11));
    object->setIs_name(rset->getInt(12));
    object->setNo_uid(rset->getInt(13));
    object->setNo_gid(rset->getInt(14));
    object->setNo_name(rset->getInt(15));
    object->setNo_host(rset->getInt(16));
    object->setNo_vid(rset->getInt(17));
    object->setNo_mode(rset->getInt(18));
    object->setNo_date(rset->getInt(19));
    object->setNo_time(rset->getInt(20));
    object->setNo_age(rset->getInt(21));
    object->setEuid(rset->getInt(22));
    object->setEgid(rset->getInt(23));
    object->setName(rset->getString(24));
    object->setId((u_signed64)rset->getDouble(25));
    object->setStatus((enum castor::vdqm::TapeDriveStatusCodes)rset->getInt(27));
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
                    << "and id was " << ad->target() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCnv::updateObj(castor::IObject* obj)
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
    castor::vdqm::TapeDrive* object = 
      dynamic_cast<castor::vdqm::TapeDrive*>(obj);
    object->setJobID(rset->getInt(1));
    object->setCreationTime(rset->getInt(2));
    object->setResettime(rset->getInt(3));
    object->setUsecount(rset->getInt(4));
    object->setErrcount(rset->getInt(5));
    object->setTransferredMB(rset->getInt(6));
    object->setTotalMB((u_signed64)rset->getDouble(7));
    object->setDedicate(rset->getString(8));
    object->setNewDedicate(rset->getString(9));
    object->setIs_uid(rset->getInt(10));
    object->setIs_gid(rset->getInt(11));
    object->setIs_name(rset->getInt(12));
    object->setNo_uid(rset->getInt(13));
    object->setNo_gid(rset->getInt(14));
    object->setNo_name(rset->getInt(15));
    object->setNo_host(rset->getInt(16));
    object->setNo_vid(rset->getInt(17));
    object->setNo_mode(rset->getInt(18));
    object->setNo_date(rset->getInt(19));
    object->setNo_time(rset->getInt(20));
    object->setNo_age(rset->getInt(21));
    object->setEuid(rset->getInt(22));
    object->setEgid(rset->getInt(23));
    object->setName(rset->getString(24));
    object->setId((u_signed64)rset->getDouble(25));
    object->setStatus((enum castor::vdqm::TapeDriveStatusCodes)rset->getInt(27));
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


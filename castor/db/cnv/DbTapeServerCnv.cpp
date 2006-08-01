/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "DbTapeServerCnv.hpp"
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

/// SQL statement for request update
const std::string castor::db::cnv::DbTapeServerCnv::s_updateStatementString =
"UPDATE TapeServer SET serverName = :1, actingMode = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::cnv::DbTapeServerCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

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
    delete m_insertStatement;
    delete m_deleteStatement;
    delete m_selectStatement;
    delete m_updateStatement;
    delete m_storeTypeStatement;
    delete m_deleteTypeStatement;
    delete m_deleteTapeDriveStatement;
    delete m_selectTapeDriveStatement;
    delete m_remoteUpdateTapeDriveStatement;
  } catch (castor::exception::SQLError ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
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
                                               bool autocommit)
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
// fillRepTapeDrive
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::fillRepTapeDrive(castor::vdqm::TapeServer* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectTapeDriveStatement) {
    m_selectTapeDriveStatement = createStatement(s_selectTapeDriveStatementString);
  }
  // Get current database data
  std::set<int> tapeDrivesList;
  m_selectTapeDriveStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapeDriveStatement->executeQuery();
  while (rset->next()) {
    tapeDrivesList.insert(rset->getInt(1));
  }
  delete rset;
  // update tapeDrives and create new ones
  for (std::vector<castor::vdqm::TapeDrive*>::iterator it = obj->tapeDrives().begin();
       it != obj->tapeDrives().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_TapeServer);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateTapeDriveStatement) {
        m_remoteUpdateTapeDriveStatement = createStatement(s_remoteUpdateTapeDriveStatementString);
      }
      // Update remote object
      m_remoteUpdateTapeDriveStatement->setInt64(1, obj->id());
      m_remoteUpdateTapeDriveStatement->setInt64(2, (*it)->id());
      m_remoteUpdateTapeDriveStatement->execute();
      std::set<int>::iterator item;
      if ((item = tapeDrivesList.find((*it)->id())) != tapeDrivesList.end()) {
        tapeDrivesList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = tapeDrivesList.begin();
       it != tapeDrivesList.end();
       it++) {
    if (0 == m_deleteTapeDriveStatement) {
      m_deleteTapeDriveStatement = createStatement(s_deleteTapeDriveStatementString);
    }
    m_deleteTapeDriveStatement->setInt64(1, *it);
    m_deleteTapeDriveStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::fillObj(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type,
                                               bool autocommit)
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
  if (autocommit) {
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
  std::set<int> tapeDrivesList;
  m_selectTapeDriveStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectTapeDriveStatement->executeQuery();
  while (rset->next()) {
    tapeDrivesList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::vdqm::TapeDrive*> toBeDeleted;
  for (std::vector<castor::vdqm::TapeDrive*>::iterator it = obj->tapeDrives().begin();
       it != obj->tapeDrives().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = tapeDrivesList.find((*it)->id())) == tapeDrivesList.end()) {
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
  for (std::set<int>::iterator it = tapeDrivesList.begin();
       it != tapeDrivesList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::vdqm::TapeDrive* remoteObj = 
      dynamic_cast<castor::vdqm::TapeDrive*>(item);
    obj->addTapeDrives(remoteObj);
    remoteObj->setTapeServer(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::createRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit,
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
      m_insertStatement->registerOutParam(3, castor::db::DBTYPE_INT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->serverName());
    m_insertStatement->setInt(2, (int)obj->actingMode());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getInt64(3));
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
                    << "  serverName : " << obj->serverName() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  actingMode : " << obj->actingMode() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeServerCnv::updateRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit)
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
    m_updateStatement->setInt64(3, obj->id());
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
void castor::db::cnv::DbTapeServerCnv::deleteRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit)
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
    m_deleteTypeStatement->setInt64(1, obj->id());
    m_deleteTypeStatement->execute();
    m_deleteStatement->setInt64(1, obj->id());
    m_deleteStatement->execute();
    for (std::vector<castor::vdqm::TapeDrive*>::iterator it = obj->tapeDrives().begin();
         it != obj->tapeDrives().end();
         it++) {
      cnvSvc()->deleteRep(0, *it, false);
    }
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
    m_selectStatement->setInt64(1, ad->target());
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
    object->setId(rset->getInt64(2));
    object->setActingMode((enum castor::vdqm::TapeServerStatusCodes)rset->getInt(3));
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
void castor::db::cnv::DbTapeServerCnv::updateObj(castor::IObject* obj)
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
    castor::vdqm::TapeServer* object = 
      dynamic_cast<castor::vdqm::TapeServer*>(obj);
    object->setServerName(rset->getString(1));
    object->setId(rset->getInt64(2));
    object->setActingMode((enum castor::vdqm::TapeServerStatusCodes)rset->getInt(3));
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


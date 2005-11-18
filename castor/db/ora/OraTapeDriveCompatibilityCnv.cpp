/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/ora/OraTapeDriveCompatibilityCnv.cpp
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
#include "OraTapeDriveCompatibilityCnv.hpp"
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
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeDriveCompatibility.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraTapeDriveCompatibilityCnv>* s_factoryOraTapeDriveCompatibilityCnv =
  new castor::CnvFactory<castor::db::ora::OraTapeDriveCompatibilityCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraTapeDriveCompatibilityCnv::s_insertStatementString =
"INSERT INTO TapeDriveCompatibility (tapeDriveModel, priorityLevel, id, tapeAccessSpecification) VALUES (:1,:2,ids_seq.nextval,:3) RETURNING id INTO :4";

/// SQL statement for request deletion
const std::string castor::db::ora::OraTapeDriveCompatibilityCnv::s_deleteStatementString =
"DELETE FROM TapeDriveCompatibility WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraTapeDriveCompatibilityCnv::s_selectStatementString =
"SELECT tapeDriveModel, priorityLevel, id, tapeAccessSpecification FROM TapeDriveCompatibility WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraTapeDriveCompatibilityCnv::s_updateStatementString =
"UPDATE TapeDriveCompatibility SET tapeDriveModel = :1, priorityLevel = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::ora::OraTapeDriveCompatibilityCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraTapeDriveCompatibilityCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member tapeAccessSpecification
const std::string castor::db::ora::OraTapeDriveCompatibilityCnv::s_checkTapeAccessSpecificationExistStatementString =
"SELECT id from TapeAccessSpecification WHERE id = :1";

/// SQL update statement for member tapeAccessSpecification
const std::string castor::db::ora::OraTapeDriveCompatibilityCnv::s_updateTapeAccessSpecificationStatementString =
"UPDATE TapeDriveCompatibility SET tapeAccessSpecification = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraTapeDriveCompatibilityCnv::OraTapeDriveCompatibilityCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkTapeAccessSpecificationExistStatement(0),
  m_updateTapeAccessSpecificationStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraTapeDriveCompatibilityCnv::~OraTapeDriveCompatibilityCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCompatibilityCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_checkTapeAccessSpecificationExistStatement);
    deleteStatement(m_updateTapeAccessSpecificationStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkTapeAccessSpecificationExistStatement = 0;
  m_updateTapeAccessSpecificationStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeDriveCompatibilityCnv::ObjType() {
  return castor::vdqm::TapeDriveCompatibility::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeDriveCompatibilityCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCompatibilityCnv::fillRep(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            unsigned int type,
                                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  try {
    switch (type) {
    case castor::OBJ_TapeAccessSpecification :
      fillRepTapeAccessSpecification(obj);
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
    castor::exception::Internal ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepTapeAccessSpecification
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCompatibilityCnv::fillRepTapeAccessSpecification(castor::vdqm::TapeDriveCompatibility* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->tapeAccessSpecification()) {
    // Check checkTapeAccessSpecificationExist statement
    if (0 == m_checkTapeAccessSpecificationExistStatement) {
      m_checkTapeAccessSpecificationExistStatement = createStatement(s_checkTapeAccessSpecificationExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeAccessSpecificationExistStatement->setDouble(1, obj->tapeAccessSpecification()->id());
    oracle::occi::ResultSet *rset = m_checkTapeAccessSpecificationExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->tapeAccessSpecification(), false);
    }
    // Close resultset
    m_checkTapeAccessSpecificationExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateTapeAccessSpecificationStatement) {
    m_updateTapeAccessSpecificationStatement = createStatement(s_updateTapeAccessSpecificationStatementString);
  }
  // Update local object
  m_updateTapeAccessSpecificationStatement->setDouble(1, 0 == obj->tapeAccessSpecification() ? 0 : obj->tapeAccessSpecification()->id());
  m_updateTapeAccessSpecificationStatement->setDouble(2, obj->id());
  m_updateTapeAccessSpecificationStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCompatibilityCnv::fillObj(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  switch (type) {
  case castor::OBJ_TapeAccessSpecification :
    fillObjTapeAccessSpecification(obj);
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
// fillObjTapeAccessSpecification
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCompatibilityCnv::fillObjTapeAccessSpecification(castor::vdqm::TapeDriveCompatibility* obj)
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
  u_signed64 tapeAccessSpecificationId = (u_signed64)rset->getDouble(4);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->tapeAccessSpecification() &&
      (0 == tapeAccessSpecificationId ||
       obj->tapeAccessSpecification()->id() != tapeAccessSpecificationId)) {
    obj->setTapeAccessSpecification(0);
  }
  // Update object or create new one
  if (0 != tapeAccessSpecificationId) {
    if (0 == obj->tapeAccessSpecification()) {
      obj->setTapeAccessSpecification
        (dynamic_cast<castor::vdqm::TapeAccessSpecification*>
         (cnvSvc()->getObjFromId(tapeAccessSpecificationId)));
    } else {
      cnvSvc()->updateObj(obj->tapeAccessSpecification());
    }
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCompatibilityCnv::createRep(castor::IAddress* address,
                                                              castor::IObject* object,
                                                              bool autocommit,
                                                              unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(4, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->tapeDriveModel());
    m_insertStatement->setInt(2, obj->priorityLevel());
    m_insertStatement->setDouble(3, (type == OBJ_TapeAccessSpecification && obj->tapeAccessSpecification() != 0) ? obj->tapeAccessSpecification()->id() : 0);
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(4));
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (castor::exception::Exception e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  tapeDriveModel : " << obj->tapeDriveModel() << std::endl
                    << "  priorityLevel : " << obj->priorityLevel() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  tapeAccessSpecification : " << obj->tapeAccessSpecification() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveCompatibilityCnv::updateRep(castor::IAddress* address,
                                                              castor::IObject* object,
                                                              bool autocommit)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->tapeDriveModel());
    m_updateStatement->setInt(2, obj->priorityLevel());
    m_updateStatement->setDouble(3, obj->id());
    m_updateStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (castor::exception::Exception e) {
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
void castor::db::ora::OraTapeDriveCompatibilityCnv::deleteRep(castor::IAddress* address,
                                                              castor::IObject* object,
                                                              bool autocommit)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveCompatibility* obj = 
    dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(object);
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
    try {
      // Always try to rollback
      cnvSvc()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (castor::exception::Exception e) {
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
castor::IObject* castor::db::ora::OraTapeDriveCompatibilityCnv::createObj(castor::IAddress* address)
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
    castor::vdqm::TapeDriveCompatibility* object = new castor::vdqm::TapeDriveCompatibility();
    // Now retrieve and set members
    object->setTapeDriveModel(rset->getString(1));
    object->setPriorityLevel(rset->getInt(2));
    object->setId((u_signed64)rset->getDouble(3));
    m_selectStatement->closeResultSet(rset);
    return object;
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (castor::exception::Exception e) {
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
void castor::db::ora::OraTapeDriveCompatibilityCnv::updateObj(castor::IObject* obj)
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
    castor::vdqm::TapeDriveCompatibility* object = 
      dynamic_cast<castor::vdqm::TapeDriveCompatibility*>(obj);
    object->setTapeDriveModel(rset->getString(1));
    object->setPriorityLevel(rset->getInt(2));
    object->setId((u_signed64)rset->getDouble(3));
    m_selectStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (castor::exception::Exception e) {
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


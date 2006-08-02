/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/ora/OraTapeDriveDedicationCnv.cpp
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
#include "OraTapeDriveDedicationCnv.hpp"
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
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDriveDedication.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraTapeDriveDedicationCnv>* s_factoryOraTapeDriveDedicationCnv =
  new castor::CnvFactory<castor::db::ora::OraTapeDriveDedicationCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraTapeDriveDedicationCnv::s_insertStatementString =
"INSERT INTO TapeDriveDedication (clientHost, euid, egid, vid, accessMode, startTime, endTime, reason, id, tapeDrive) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,ids_seq.nextval,:9) RETURNING id INTO :10";

/// SQL statement for request deletion
const std::string castor::db::ora::OraTapeDriveDedicationCnv::s_deleteStatementString =
"DELETE FROM TapeDriveDedication WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraTapeDriveDedicationCnv::s_selectStatementString =
"SELECT clientHost, euid, egid, vid, accessMode, startTime, endTime, reason, id, tapeDrive FROM TapeDriveDedication WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraTapeDriveDedicationCnv::s_updateStatementString =
"UPDATE TapeDriveDedication SET clientHost = :1, euid = :2, egid = :3, vid = :4, accessMode = :5, startTime = :6, endTime = :7, reason = :8 WHERE id = :9";

/// SQL statement for type storage
const std::string castor::db::ora::OraTapeDriveDedicationCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraTapeDriveDedicationCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member tapeDrive
const std::string castor::db::ora::OraTapeDriveDedicationCnv::s_checkTapeDriveExistStatementString =
"SELECT id from TapeDrive WHERE id = :1";

/// SQL update statement for member tapeDrive
const std::string castor::db::ora::OraTapeDriveDedicationCnv::s_updateTapeDriveStatementString =
"UPDATE TapeDriveDedication SET tapeDrive = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraTapeDriveDedicationCnv::OraTapeDriveDedicationCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkTapeDriveExistStatement(0),
  m_updateTapeDriveStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraTapeDriveDedicationCnv::~OraTapeDriveDedicationCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveDedicationCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_checkTapeDriveExistStatement);
    deleteStatement(m_updateTapeDriveStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkTapeDriveExistStatement = 0;
  m_updateTapeDriveStatement = 0;
  }

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeDriveDedicationCnv::ObjType() {
  return castor::vdqm::TapeDriveDedication::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeDriveDedicationCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveDedicationCnv::fillRep(castor::IAddress* address,
                                                         castor::IObject* object,
                                                         unsigned int type,
                                                         bool autocommit)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveDedication* obj = 
    dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
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
  } catch (oracle::occi::SQLException e) {
    cnvSvc()->handleException(e);
    castor::exception::Internal ex; 
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepTapeDrive
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveDedicationCnv::fillRepTapeDrive(castor::vdqm::TapeDriveDedication* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->tapeDrive()) {
    // Check checkTapeDriveExist statement
    if (0 == m_checkTapeDriveExistStatement) {
      m_checkTapeDriveExistStatement = createStatement(s_checkTapeDriveExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeDriveExistStatement->setDouble(1, obj->tapeDrive()->id());
    oracle::occi::ResultSet *rset = m_checkTapeDriveExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->tapeDrive(), false);
    }
    // Close resultset
    m_checkTapeDriveExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateTapeDriveStatement) {
    m_updateTapeDriveStatement = createStatement(s_updateTapeDriveStatementString);
  }
  // Update local object
  m_updateTapeDriveStatement->setDouble(1, 0 == obj->tapeDrive() ? 0 : obj->tapeDrive()->id());
  m_updateTapeDriveStatement->setDouble(2, obj->id());
  m_updateTapeDriveStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveDedicationCnv::fillObj(castor::IAddress* address,
                                                         castor::IObject* object,
                                                         unsigned int type,
                                                         bool autocommit)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveDedication* obj = 
    dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
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
void castor::db::ora::OraTapeDriveDedicationCnv::fillObjTapeDrive(castor::vdqm::TapeDriveDedication* obj)
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
  u_signed64 tapeDriveId = (u_signed64)rset->getDouble(10);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->tapeDrive() &&
      (0 == tapeDriveId ||
       obj->tapeDrive()->id() != tapeDriveId)) {
    obj->tapeDrive()->removeTapeDriveDedication(obj);
    obj->setTapeDrive(0);
  }
  // Update object or create new one
  if (0 != tapeDriveId) {
    if (0 == obj->tapeDrive()) {
      obj->setTapeDrive
        (dynamic_cast<castor::vdqm::TapeDrive*>
         (cnvSvc()->getObjFromId(tapeDriveId)));
    } else {
      cnvSvc()->updateObj(obj->tapeDrive());
    }
    obj->tapeDrive()->addTapeDriveDedication(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeDriveDedicationCnv::createRep(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           bool autocommit,
                                                           unsigned int type)
  throw (castor::exception::Exception) {
  castor::vdqm::TapeDriveDedication* obj = 
    dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(10, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->clientHost());
    m_insertStatement->setInt(2, obj->euid());
    m_insertStatement->setInt(3, obj->egid());
    m_insertStatement->setString(4, obj->vid());
    m_insertStatement->setInt(5, obj->accessMode());
    m_insertStatement->setDouble(6, obj->startTime());
    m_insertStatement->setDouble(7, obj->endTime());
    m_insertStatement->setString(8, obj->reason());
    m_insertStatement->setDouble(9, (type == OBJ_TapeDrive && obj->tapeDrive() != 0) ? obj->tapeDrive()->id() : 0);
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(10));
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (oracle::occi::SQLException e) {
      cnvSvc()->handleException(e);
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "Error in insert request :"
                      << std::endl << e.what() << std::endl
                      << "Statement was :" << std::endl
                      << s_insertStatementString << std::endl
                      << "and parameters' values were :" << std::endl
                      << "  clientHost : " << obj->clientHost() << std::endl
                      << "  euid : " << obj->euid() << std::endl
                      << "  egid : " << obj->egid() << std::endl
                      << "  vid : " << obj->vid() << std::endl
                      << "  accessMode : " << obj->accessMode() << std::endl
                      << "  startTime : " << obj->startTime() << std::endl
                      << "  endTime : " << obj->endTime() << std::endl
                      << "  reason : " << obj->reason() << std::endl
                      << "  id : " << obj->id() << std::endl
                      << "  tapeDrive : " << obj->tapeDrive() << std::endl;
      throw ex;
    }
}

  //----------------------------------------------------------------------------
  // updateRep
  //----------------------------------------------------------------------------
  void castor::db::ora::OraTapeDriveDedicationCnv::updateRep(castor::IAddress* address,
                                                             castor::IObject* object,
                                                             bool autocommit)
    throw (castor::exception::Exception) {
    castor::vdqm::TapeDriveDedication* obj = 
      dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
    // check whether something needs to be done
    if (0 == obj) return;
    try {
      // Check whether the statements are ok
      if (0 == m_updateStatement) {
        m_updateStatement = createStatement(s_updateStatementString);
      }
      // Update the current object
      m_updateStatement->setString(1, obj->clientHost());
      m_updateStatement->setInt(2, obj->euid());
      m_updateStatement->setInt(3, obj->egid());
      m_updateStatement->setString(4, obj->vid());
      m_updateStatement->setInt(5, obj->accessMode());
      m_updateStatement->setDouble(6, obj->startTime());
      m_updateStatement->setDouble(7, obj->endTime());
      m_updateStatement->setString(8, obj->reason());
      m_updateStatement->setDouble(9, obj->id());
      m_updateStatement->executeUpdate();
      if (autocommit) {
        cnvSvc()->commit();
      }
    } catch (oracle::occi::SQLException e) {
        cnvSvc()->handleException(e);
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
    void castor::db::ora::OraTapeDriveDedicationCnv::deleteRep(castor::IAddress* address,
                                                               castor::IObject* object,
                                                               bool autocommit)
      throw (castor::exception::Exception) {
      castor::vdqm::TapeDriveDedication* obj = 
        dynamic_cast<castor::vdqm::TapeDriveDedication*>(object);
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
          cnvSvc()->handleException(e);
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
      castor::IObject* castor::db::ora::OraTapeDriveDedicationCnv::createObj(castor::IAddress* address)
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
          castor::vdqm::TapeDriveDedication* object = new castor::vdqm::TapeDriveDedication();
          // Now retrieve and set members
          object->setClientHost(rset->getString(1));
          object->setEuid(rset->getInt(2));
          object->setEgid(rset->getInt(3));
          object->setVid(rset->getString(4));
          object->setAccessMode(rset->getInt(5));
          object->setStartTime((u_signed64)rset->getDouble(6));
          object->setEndTime((u_signed64)rset->getDouble(7));
          object->setReason(rset->getString(8));
          object->setId((u_signed64)rset->getDouble(9));
          m_selectStatement->closeResultSet(rset);
          return object;
        } catch (oracle::occi::SQLException e) {
            cnvSvc()->handleException(e);
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
        void castor::db::ora::OraTapeDriveDedicationCnv::updateObj(castor::IObject* obj)
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
            castor::vdqm::TapeDriveDedication* object = 
              dynamic_cast<castor::vdqm::TapeDriveDedication*>(obj);
            object->setClientHost(rset->getString(1));
            object->setEuid(rset->getInt(2));
            object->setEgid(rset->getInt(3));
            object->setVid(rset->getString(4));
            object->setAccessMode(rset->getInt(5));
            object->setStartTime((u_signed64)rset->getDouble(6));
            object->setEndTime((u_signed64)rset->getDouble(7));
            object->setReason(rset->getString(8));
            object->setId((u_signed64)rset->getDouble(9));
            m_selectStatement->closeResultSet(rset);
          } catch (oracle::occi::SQLException e) {
              cnvSvc()->handleException(e);
              castor::exception::InvalidArgument ex;
              ex.getMessage() << "Error in update request :"
                              << std::endl << e.what() << std::endl
                              << "Statement was :" << std::endl
                              << s_updateStatementString << std::endl
                              << "and id was " << obj->id() << std::endl;;
              throw ex;
            }
}


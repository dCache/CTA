/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbClientIdentificationCnv.cpp
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
#include "DbClientIdentificationCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/ClientIdentification.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbClientIdentificationCnv>* s_factoryDbClientIdentificationCnv =
  new castor::CnvFactory<castor::db::cnv::DbClientIdentificationCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbClientIdentificationCnv::s_insertStatementString =
"INSERT INTO ClientIdentification (machine, userName, port, euid, egid, magic, id) VALUES (:1,:2,:3,:4,:5,:6,ids_seq.nextval) RETURNING id INTO :7";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbClientIdentificationCnv::s_deleteStatementString =
"DELETE FROM ClientIdentification WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbClientIdentificationCnv::s_selectStatementString =
"SELECT machine, userName, port, euid, egid, magic, id FROM ClientIdentification WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbClientIdentificationCnv::s_updateStatementString =
"UPDATE ClientIdentification SET machine = :1, userName = :2, port = :3, euid = :4, egid = :5, magic = :6 WHERE id = :7";

/// SQL statement for type storage
const std::string castor::db::cnv::DbClientIdentificationCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbClientIdentificationCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbClientIdentificationCnv::DbClientIdentificationCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbClientIdentificationCnv::~DbClientIdentificationCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbClientIdentificationCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    delete m_insertStatement;
    delete m_deleteStatement;
    delete m_selectStatement;
    delete m_updateStatement;
    delete m_storeTypeStatement;
    delete m_deleteTypeStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbClientIdentificationCnv::ObjType() {
  return castor::stager::ClientIdentification::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbClientIdentificationCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbClientIdentificationCnv::fillRep(castor::IAddress* address,
                                                         castor::IObject* object,
                                                         unsigned int type,
                                                         bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::ClientIdentification* obj = 
    dynamic_cast<castor::stager::ClientIdentification*>(object);
  try {
    switch (type) {
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
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbClientIdentificationCnv::fillObj(castor::IAddress* address,
                                                         castor::IObject* object,
                                                         unsigned int type,
                                                         bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::ClientIdentification* obj = 
    dynamic_cast<castor::stager::ClientIdentification*>(object);
  switch (type) {
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
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbClientIdentificationCnv::createRep(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           bool autocommit,
                                                           unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::ClientIdentification* obj = 
    dynamic_cast<castor::stager::ClientIdentification*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(7, castor::db::DBTYPE_INT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->machine());
    m_insertStatement->setString(2, obj->userName());
    m_insertStatement->setInt(3, obj->port());
    m_insertStatement->setInt(4, obj->euid());
    m_insertStatement->setInt(5, obj->egid());
    m_insertStatement->setInt(6, obj->magic());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(7));
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
                    << "  port : " << obj->port() << std::endl
                    << "  euid : " << obj->euid() << std::endl
                    << "  egid : " << obj->egid() << std::endl
                    << "  magic : " << obj->magic() << std::endl
                    << "  id : " << obj->id() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbClientIdentificationCnv::updateRep(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::ClientIdentification* obj = 
    dynamic_cast<castor::stager::ClientIdentification*>(object);
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
    m_updateStatement->setInt(3, obj->port());
    m_updateStatement->setInt(4, obj->euid());
    m_updateStatement->setInt(5, obj->egid());
    m_updateStatement->setInt(6, obj->magic());
    m_updateStatement->setUInt64(7, obj->id());
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
void castor::db::cnv::DbClientIdentificationCnv::deleteRep(castor::IAddress* address,
                                                           castor::IObject* object,
                                                           bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::ClientIdentification* obj = 
    dynamic_cast<castor::stager::ClientIdentification*>(object);
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
castor::IObject* castor::db::cnv::DbClientIdentificationCnv::createObj(castor::IAddress* address)
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
    castor::stager::ClientIdentification* object = new castor::stager::ClientIdentification();
    // Now retrieve and set members
    object->setMachine(rset->getString(1));
    object->setUserName(rset->getString(2));
    object->setPort(rset->getInt(3));
    object->setEuid(rset->getInt(4));
    object->setEgid(rset->getInt(5));
    object->setMagic(rset->getInt(6));
    object->setId(rset->getUInt64(7));
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
void castor::db::cnv::DbClientIdentificationCnv::updateObj(castor::IObject* obj)
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
    castor::stager::ClientIdentification* object = 
      dynamic_cast<castor::stager::ClientIdentification*>(obj);
    object->setMachine(rset->getString(1));
    object->setUserName(rset->getString(2));
    object->setPort(rset->getInt(3));
    object->setEuid(rset->getInt(4));
    object->setEgid(rset->getInt(5));
    object->setMagic(rset->getInt(6));
    object->setId(rset->getUInt64(7));
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


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/ora/OraClientCnv.cpp
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
#include "OraClientCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/rh/Client.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraClientCnv>* s_factoryOraClientCnv =
  new castor::CnvFactory<castor::db::ora::OraClientCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraClientCnv::s_insertStatementString =
"INSERT INTO Client (ipAddress, port, id) VALUES (:1,:2,ids_seq.nextval) RETURNING id INTO :3";

/// SQL statement for request deletion
const std::string castor::db::ora::OraClientCnv::s_deleteStatementString =
"DELETE FROM Client WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraClientCnv::s_selectStatementString =
"SELECT ipAddress, port, id FROM Client WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraClientCnv::s_updateStatementString =
"UPDATE Client SET ipAddress = :1, port = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::ora::OraClientCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraClientCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraClientCnv::OraClientCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraClientCnv::~OraClientCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraClientCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
  } catch (oracle::occi::SQLException e) {};
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
const unsigned int castor::db::ora::OraClientCnv::ObjType() {
  return castor::rh::Client::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraClientCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraClientCnv::fillRep(castor::IAddress* address,
                                            castor::IObject* object,
                                            unsigned int type,
                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::rh::Client* obj = 
    dynamic_cast<castor::rh::Client*>(object);
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
  } catch (oracle::occi::SQLException e) {
    cnvSvc()->handleException(e);
    castor::exception::Internal ex; 
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraClientCnv::fillObj(castor::IAddress* address,
                                            castor::IObject* object,
                                            unsigned int type,
                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::rh::Client* obj = 
    dynamic_cast<castor::rh::Client*>(object);
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
void castor::db::ora::OraClientCnv::createRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              bool autocommit,
                                              unsigned int type)
  throw (castor::exception::Exception) {
  castor::rh::Client* obj = 
    dynamic_cast<castor::rh::Client*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(3, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->ipAddress());
    m_insertStatement->setInt(2, obj->port());
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(3));
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
                      << "  ipAddress : " << obj->ipAddress() << std::endl
                      << "  port : " << obj->port() << std::endl
                      << "  id : " << obj->id() << std::endl;
      throw ex;
    }
}

  //----------------------------------------------------------------------------
  // updateRep
  //----------------------------------------------------------------------------
  void castor::db::ora::OraClientCnv::updateRep(castor::IAddress* address,
                                                castor::IObject* object,
                                                bool autocommit)
    throw (castor::exception::Exception) {
    castor::rh::Client* obj = 
      dynamic_cast<castor::rh::Client*>(object);
    // check whether something needs to be done
    if (0 == obj) return;
    try {
      // Check whether the statements are ok
      if (0 == m_updateStatement) {
        m_updateStatement = createStatement(s_updateStatementString);
      }
      // Update the current object
      m_updateStatement->setInt(1, obj->ipAddress());
      m_updateStatement->setInt(2, obj->port());
      m_updateStatement->setDouble(3, obj->id());
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
    void castor::db::ora::OraClientCnv::deleteRep(castor::IAddress* address,
                                                  castor::IObject* object,
                                                  bool autocommit)
      throw (castor::exception::Exception) {
      castor::rh::Client* obj = 
        dynamic_cast<castor::rh::Client*>(object);
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
      castor::IObject* castor::db::ora::OraClientCnv::createObj(castor::IAddress* address)
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
          castor::rh::Client* object = new castor::rh::Client();
          // Now retrieve and set members
          object->setIpAddress(rset->getInt(1));
          object->setPort(rset->getInt(2));
          object->setId((u_signed64)rset->getDouble(3));
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
        void castor::db::ora::OraClientCnv::updateObj(castor::IObject* obj)
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
            castor::rh::Client* object = 
              dynamic_cast<castor::rh::Client*>(obj);
            object->setIpAddress(rset->getInt(1));
            object->setPort(rset->getInt(2));
            object->setId((u_signed64)rset->getDouble(3));
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


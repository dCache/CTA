/******************************************************************************
 *                      OraRHSvc.cpp
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
 * @(#)$RCSfile: OraRHSvc.cpp,v $ $Revision: 1.2 $ $Release$ $Date: 2007/08/21 14:16:46 $ $Author: sponcec3 $
 *
 * Implementation of the IRHSvc for Oracle
 *
 * @author Sebastien Ponce
 *****************************************************************************/

// Include Files
#include "castor/db/ora/OraRHSvc.hpp"
#include "castor/Constants.hpp"
#include "castor/SvcFactory.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/PermissionDenied.hpp"
#include <string>
#include "occi.h"

// -----------------------------------------------------------------------
// Instantiation of a static factory class
// -----------------------------------------------------------------------
static castor::SvcFactory<castor::db::ora::OraRHSvc>* s_factoryOraRHSvc =
  new castor::SvcFactory<castor::db::ora::OraRHSvc>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------

/// SQL statement for checkPermission
const std::string castor::db::ora::OraRHSvc::s_checkPermissionStatementString =
  "BEGIN checkPermission(:1, :2, :3, :4, :5); END;";

// -----------------------------------------------------------------------
// OraRHSvc
// -----------------------------------------------------------------------
castor::db::ora::OraRHSvc::OraRHSvc(const std::string name) :
  OraCommonSvc(name),
  m_checkPermissionStatement(0) {
}

// -----------------------------------------------------------------------
// ~OraRHSvc
// -----------------------------------------------------------------------
castor::db::ora::OraRHSvc::~OraRHSvc() throw() {
  reset();
}

// -----------------------------------------------------------------------
// id
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraRHSvc::id() const {
  return ID();
}

// -----------------------------------------------------------------------
// ID
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraRHSvc::ID() {
  return castor::SVC_ORARHSVC;
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraRHSvc::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  OraCommonSvc::reset();
  try {
    if (m_checkPermissionStatement) deleteStatement(m_checkPermissionStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_checkPermissionStatement = 0;
}

//------------------------------------------------------------------------------
// checkPermission
//------------------------------------------------------------------------------
void castor::db::ora::OraRHSvc::checkPermission
(const std::string svcClassName, unsigned long euid,
 unsigned long egid, int type)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_checkPermissionStatement) {
      m_checkPermissionStatement =
        createStatement(s_checkPermissionStatementString);
      m_checkPermissionStatement->registerOutParam
        (5, oracle::occi::OCCIINT);
    }
    // execute the statement and see whether we found something
    m_checkPermissionStatement->setString(1, svcClassName);
    m_checkPermissionStatement->setInt(2, euid);
    m_checkPermissionStatement->setInt(3, egid);
    m_checkPermissionStatement->setInt(4, type);
    unsigned int nb = m_checkPermissionStatement->executeUpdate();
    if (0 == nb) {
      castor::exception::Internal ex;
      ex.getMessage()
        << "checkPermission did not return any result.";
      throw ex;
    }
    // throw exception if access denied
    if (m_checkPermissionStatement->getInt(5) != 0) {
      castor::exception::PermissionDenied ex;
      ex.getMessage() << "You don't have right to make a request of type "
                      << castor::ObjectsIdStrings[type]
                      << " in service class '";
      if (0 == svcClassName.size()) {
        ex.getMessage() << "default";
      } else {
        ex.getMessage() << svcClassName;
      }
      ex.getMessage() << "'\n";
      throw ex;
    }
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in checkPermission."
      << std::endl << e.what();
    throw ex;
  }
}

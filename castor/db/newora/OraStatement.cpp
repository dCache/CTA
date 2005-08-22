/******************************************************************************
 *                      OraStatement.cpp
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
 * @(#)$RCSfile: OraStatement.cpp,v $ $Revision: 1.1 $ $Release$ $Date: 2005/08/22 16:32:34 $ $Author: itglp $
 *
 * 
 *
 * @author Giuseppe Lo Presti, giuseppe.lopresti@cern.ch
 *****************************************************************************/

#include "OraStatement.hpp"
#include "OraResultSet.hpp"

castor::db::ora::OraStatement::OraStatement(oracle::occi::Statement* stmt, castor::db::ora::OraCnvSvc* cnvSvc) :
  m_statement(stmt),
  m_cnvSvc(cnvSvc)
{
    m_statement->setAutoCommit(false);
}

castor::db::ora::OraStatement::~OraStatement() throw (castor::exception::Exception)
{
    m_cnvSvc->closeStatement(this);
}

void castor::db::ora::OraStatement::autoCommit()
{
    m_statement->setAutoCommit(true);
}
 
void castor::db::ora::OraStatement::setInt(int pos, int value)
{
	m_statement->setInt(pos, value);
}

void castor::db::ora::OraStatement::setInt64(int pos, u_signed64 value)
{
	m_statement->setDouble(pos, (double)value);
}

void castor::db::ora::OraStatement::setString(int pos, std::string value)
{
	m_statement->setString(pos, value);
}

void castor::db::ora::OraStatement::setFloat(int pos, float value)
{
	m_statement->setFloat(pos, value);
}

void castor::db::ora::OraStatement::setDouble(int pos, double value)
{
	m_statement->setDouble(pos, value);
}


void castor::db::ora::OraStatement::registerOutParam(int pos, int dbType)
{
    oracle::occi::Type oraType;
    if(dbType == castor::db::DBTYPE_INT)
        oraType = oracle::occi::OCCIINT;
    else if(dbType == castor::db::DBTYPE_INT64 || dbType == castor::db::DBTYPE_DOUBLE)
        oraType = oracle::occi::OCCIDOUBLE;
    else if(dbType == castor::db::DBTYPE_STRING)
        oraType = oracle::occi::OCCISTRING;
    else
        return;
    m_statement->registerOutParam(pos, oraType);
}

int castor::db::ora::OraStatement::getInt(int pos)
{
    return m_statement->getInt(pos);
}

u_signed64 castor::db::ora::OraStatement::getInt64(int pos)
{
    return (u_signed64)m_statement->getDouble(pos);
}

std::string castor::db::ora::OraStatement::getString(int pos)
{
    return m_statement->getString(pos);
}

float castor::db::ora::OraStatement::getFloat(int pos)
{
    return m_statement->getFloat(pos);
}

double castor::db::ora::OraStatement::getDouble(int pos)
{
    return m_statement->getDouble(pos);
}

    
castor::db::IDbResultSet* castor::db::ora::OraStatement::executeQuery()
throw (castor::exception::Exception) {
  try {
      return new castor::db::ora::OraResultSet(m_statement->executeQuery(), m_statement);
  } catch(oracle::occi::SQLException e) {
    try {
      m_cnvSvc->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviosly lost the Oracle connection here
        m_cnvSvc->dropConnection();
      }
    } catch (oracle::occi::SQLException bad) {
      // rollback failed, let's drop the connection for security
      m_cnvSvc->dropConnection();
    }
    castor::exception::SQLError ex;
    // there was an old XXX - fix depending on Oracle error code
    ex.getMessage() << "Database error, Oracle code: " << e.getErrorCode()
                    << std::endl << e.what();
    throw ex;
  }
}

int castor::db::ora::OraStatement::execute()
throw (castor::exception::Exception) {
  try {
      return m_statement->executeUpdate();
  } catch(oracle::occi::SQLException e) {
    try {
      m_cnvSvc->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviosly lost the Oracle connection here
        m_cnvSvc->dropConnection();
      }
    } catch (oracle::occi::SQLException bad) {
      // rollback failed, let's drop the connection for security
      m_cnvSvc->dropConnection();
    }
    castor::exception::SQLError ex;
    // there was an old XXX - fix depending on Oracle error code
    ex.getMessage() << "Database error, Oracle code: " << e.getErrorCode()
                    << std::endl << e.what();
    ex.setSQLErrorCode(e.getErrorCode());
    throw ex;
  }
}


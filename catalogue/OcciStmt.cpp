/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "catalogue/OcciConn.hpp"
#include "catalogue/OcciRset.hpp"
#include "catalogue/OcciStmt.hpp"
#include "common/exception/Exception.hpp"

#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
OcciStmt::OcciStmt(const std::string &sql, OcciConn &conn, oracle::occi::Statement *const stmt) :
  m_sql(sql),
  m_paramNameToIdx(sql),
  m_conn(conn),
  m_stmt(stmt) {
  if(NULL == stmt) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + sql +
      ": stmt is NULL");
  }
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OcciStmt::~OcciStmt() throw() {
  try {
    close(); // Idempotent close() method
  } catch (...) {
    // Destructor does not throw
  }
}

//------------------------------------------------------------------------------
// close
//------------------------------------------------------------------------------
void OcciStmt::close() {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (NULL != m_stmt) {
    m_conn->terminateStatement(m_stmt);
    m_stmt = NULL;
  }
}

//------------------------------------------------------------------------------
// getSql
//------------------------------------------------------------------------------
const std::string &OcciStmt::getSql() const {
  return m_sql;
}

//------------------------------------------------------------------------------
// bind
//------------------------------------------------------------------------------
void OcciStmt::bindUint64(const std::string &paramName, const uint64_t paramValue) {
  try {
    const unsigned paramIdx = m_paramNameToIdx.getIdx(paramName);
    m_stmt->setUInt(paramIdx, paramValue);
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + getSql() + ": " +
      ex.getMessage().str());
  } catch(std::exception &se) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + getSql() + ": " + se.what());
  }
}

//------------------------------------------------------------------------------
// bind
//------------------------------------------------------------------------------
void OcciStmt::bind(const std::string &paramName, const std::string &paramValue) {
  try {
    const unsigned paramIdx = m_paramNameToIdx.getIdx(paramName);
    m_stmt->setString(paramIdx, paramValue);
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + getSql() + ": " +
      ex.getMessage().str());
  } catch(std::exception &se) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + getSql() + ": " + se.what());
  }
}

//------------------------------------------------------------------------------
// executeQuery
//------------------------------------------------------------------------------
DbRset *OcciStmt::executeQuery() {
  using namespace oracle;

  try {
    return new OcciRset(*this, m_stmt->executeQuery());
  } catch(exception::Exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + getSql() + ": " +
      ne.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// executeNonQuery
//------------------------------------------------------------------------------
void OcciStmt::executeNonQuery() {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// get
//------------------------------------------------------------------------------
oracle::occi::Statement *OcciStmt::get() const {
  return m_stmt;
}

//------------------------------------------------------------------------------
// operator->
//------------------------------------------------------------------------------
oracle::occi::Statement *OcciStmt::operator->() const {
  return get();
}

} // namespace catalogue
} // namespace cta

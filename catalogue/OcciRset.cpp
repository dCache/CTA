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

#include "catalogue/OcciRset.hpp"
#include "catalogue/OcciStmt.hpp"
#include "common/exception/Exception.hpp"

#include <cstring>
#include <map>
#include <stdexcept>

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
OcciRset::OcciRset(OcciStmt &stmt, oracle::occi::ResultSet *const rset):
  m_stmt(stmt),
  m_rset(rset) {
  try {
    if (NULL == rset) {
      throw exception::Exception("rset is NULL");
    }
    populateColNameToIdxMap();
  } catch(exception::Exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + m_stmt.getSql() + ": " +
      ne.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// populateColNameToIdx
//------------------------------------------------------------------------------
void OcciRset::populateColNameToIdxMap() {
  using namespace oracle;

  try {
    const std::vector<occi::MetaData> columns = m_rset->getColumnListMetaData();
    for (unsigned int i = 0; i < columns.size(); i++) {
      // Column indices start at 1
      const unsigned int colIdx = i + 1;
      m_colNameToIdx.add(columns[i].getString(occi::MetaData::ATTR_NAME), colIdx);
    }
  } catch(exception::Exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.getMessage().str());
  } catch(std::exception &se) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + se.what());
  }
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OcciRset::~OcciRset() throw() {
  try {
    close(); // Idempotent close()
  } catch(...) {
    // Destructor does not throw
  }
}

//------------------------------------------------------------------------------
// getSql
//------------------------------------------------------------------------------
const std::string &OcciRset::getSql() const {
  return m_stmt.getSql();
}

//------------------------------------------------------------------------------
// next
//------------------------------------------------------------------------------
bool OcciRset::next() {
  using namespace oracle;

  try {
    const occi::ResultSet::Status status = m_rset->next();
    return occi::ResultSet::DATA_AVAILABLE == status;
  } catch(std::exception &se) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + m_stmt.getSql() + ": " +
      se.what());
  }
}

//------------------------------------------------------------------------------
// columnIsNull
//------------------------------------------------------------------------------
bool OcciRset::columnIsNull(const std::string &colName) const {
  try {
    const int colIdx = m_colNameToIdx.getIdx(colName);
    return m_rset->isNull(colIdx);
  } catch(exception::Exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.getMessage().str());
  } catch(std::exception &se) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + se.what());
  }
}

//------------------------------------------------------------------------------
// close
//------------------------------------------------------------------------------
void OcciRset::close() {
  std::lock_guard<std::mutex> lock(m_mutex);

  if(NULL != m_rset) {
    m_stmt->closeResultSet(m_rset);
    m_rset = NULL;
  }
}

//------------------------------------------------------------------------------
// get
//------------------------------------------------------------------------------
oracle::occi::ResultSet *OcciRset::get() const {
  return m_rset;
}

//------------------------------------------------------------------------------
// operator->
//------------------------------------------------------------------------------
oracle::occi::ResultSet *OcciRset::operator->() const {
  return get();
}

//------------------------------------------------------------------------------
// columnText
//------------------------------------------------------------------------------
std::string OcciRset::columnText(const std::string &colName) const {
  try {
    const int colIdx = m_colNameToIdx.getIdx(colName);
    return m_rset->getString(colIdx);
  } catch(exception::Exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + m_stmt.getSql() + ": " +
      ne.getMessage().str());
  } catch(std::exception &se) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + m_stmt.getSql() + ": " +
      se.what());
  }
}

//------------------------------------------------------------------------------
// columnUint64
//------------------------------------------------------------------------------
uint64_t OcciRset::columnUint64(const std::string &colName) const {
  try {
    std::lock_guard<std::mutex> lock(m_mutex);

    const int colIdx = m_colNameToIdx.getIdx(colName);
    return m_rset->getUInt(colIdx);
  } catch(exception::Exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + m_stmt.getSql() + ": " +
      ne.getMessage().str());
  } catch(std::exception &se) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + m_stmt.getSql() + ": " +
      se.what());
  }
}

} // namespace catalogue
} // namespace cta

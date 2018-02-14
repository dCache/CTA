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

#include "common/exception/Exception.hpp"
#include "common/exception/Errnum.hpp"
#include "rdbms/NullDbValue.hpp"
#include "rdbms/wrapper/Sqlite.hpp"
#include "rdbms/wrapper/SqliteRset.hpp"
#include "rdbms/wrapper/SqliteStmt.hpp"

#include <cstring>
#include <sstream>
#include <stdexcept>


namespace cta {
namespace rdbms {
namespace wrapper {

/**
 * A map from column name to column index and type.
 */
class ColNameToIdxAndType {
public:

  /**
   * Structure to store a column's index and type.  With SQLite 3 the type of a
   * column needs to be stored before any type conversion has taken place.  This
   * is because the result of calling the sqlite3_column_type() function is no
   * longer meaningful after such a conversion.
   */
  struct IdxAndType {
    /**
     * The index of the column.
     */
    int colIdx;

    /**
     * The type of the column as return by the sqlite3_column_type() function
     * before any type conversion has taken place.
     */
    int colType;

    /**
     * Constructor.  Set both member-variables to 0.
     */
    IdxAndType(): colIdx(0), colType(0) {
    }
  };

  /**
   * Adds the specified mapping from column name to column index and type.
   *
   * This method throws an exception if the specified column name is a
   * duplicate, in other words has already been added to the map.
   *
   * @param name The name of the column.
   * @param idxAndType The column index and type.
   */
  void add(const std::string &name, const IdxAndType &idxAndType) {
    if(m_nameToIdxAndType.end() != m_nameToIdxAndType.find(name)) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: " + name + " is a duplicate");
    }
    m_nameToIdxAndType[name] = idxAndType;
  }

  /**
   * Returns the index and type of the column with the specified name.
   *
   * This method throws an exception if the specified column name is not in the
   * map.
   *
   * @param name The name of the column.
   * @return The index and type of the column.
   */
  IdxAndType getIdxAndType(const std::string &name) const {
    auto it = m_nameToIdxAndType.find(name);
    if(m_nameToIdxAndType.end() == it) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: Unknown column name " + name);
    }
    return it->second;
  }

  /**
   * Alias for the getIdx() method.
   *
   * @return the index of the column with the specified name.
   */
  IdxAndType operator[](const std::string &name) const {
    return getIdxAndType(name);
  }

  /**
   * Returns true if this map is empty.
   *
   * @return True if this map is empty.
   */
  bool empty() const {
    return m_nameToIdxAndType.empty();
  }

private:

  /**
   * The underlying STL map from column name to column index.
   */
  std::map<std::string, IdxAndType> m_nameToIdxAndType;

}; // class SqliteRset::ColNameToIdx

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
SqliteRset::SqliteRset(SqliteStmt &stmt): m_stmt(stmt) {
}

//------------------------------------------------------------------------------
// destructor.
//------------------------------------------------------------------------------
SqliteRset::~SqliteRset() throw() {
}

//------------------------------------------------------------------------------
// getSql
//------------------------------------------------------------------------------
const std::string &SqliteRset::getSql() const {
  return m_stmt.getSql();
}

//------------------------------------------------------------------------------
// next
//------------------------------------------------------------------------------
bool SqliteRset::next() {
  try {
    const int stepRc = sqlite3_step(m_stmt.get());

    // Throw an exception if the call to sqlite3_step() failed
    if(SQLITE_DONE != stepRc && SQLITE_ROW != stepRc) {
      throw exception::Exception(Sqlite::rcToStr(stepRc));
    }

    if(SQLITE_ROW == stepRc) {
      clearAndPopulateColNameToIdxAndTypeMap();
    }

    return SQLITE_ROW == stepRc;
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed for SQL statement " + m_stmt.getSqlForException() +
      ": " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// clearAndPopulateColNameToIdxMap
//------------------------------------------------------------------------------
void SqliteRset::clearAndPopulateColNameToIdxAndTypeMap() {
  try {
    m_colNameToIdxAndType.clear();

    const int nbCols = sqlite3_column_count(m_stmt.get());
    for (int i = 0; i < nbCols; i++) {
      // Get the name of the column
      const char *const colName = sqlite3_column_name(m_stmt.get(), i);
      if (nullptr == colName) {
        std::ostringstream msg;
        msg << "Failed to get column name for column index " << i;
        throw exception::Exception(msg.str());
      }

      // Get the type of the column
      ColumnNameToIdxAndType::IdxAndType idxAndType;
      idxAndType.colIdx = i;
      idxAndType.colType = sqlite3_column_type(m_stmt.get(), i);

      // Add the mapping from column name to index and type
      m_colNameToIdxAndType.add(colName, idxAndType);
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// columnIsNull
//------------------------------------------------------------------------------
bool SqliteRset::columnIsNull(const std::string &colName) const {
  try {
    const ColumnNameToIdxAndType::IdxAndType idxAndType = m_colNameToIdxAndType.getIdxAndType(colName);
    return SQLITE_NULL == idxAndType.colType;
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// columnOptionalString
//------------------------------------------------------------------------------
optional<std::string> SqliteRset::columnOptionalString(const std::string &colName) const {
  try {
    const ColumnNameToIdxAndType::IdxAndType idxAndType = m_colNameToIdxAndType.getIdxAndType(colName);
    if(SQLITE_NULL == idxAndType.colType) {
      return nullopt;
    } else {
      const char *const colValue = (const char *)sqlite3_column_text(m_stmt.get(), idxAndType.colIdx);
      if(NULL == colValue) {
        exception::Exception ex;
        ex.getMessage() << __FUNCTION__ << " failed: sqlite3_column_text() returned NULL when"
          " m_colNameToIdxAndType map states otherwise: colName=" << colName << ",colIdx=" << idxAndType.colIdx;
        throw ex;
      }
      return optional<std::string>(colValue);
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// columnOptionalUint64
//------------------------------------------------------------------------------
optional<uint64_t> SqliteRset::columnOptionalUint64(const std::string &colName) const {
  try {
    const ColumnNameToIdxAndType::IdxAndType idxAndType = m_colNameToIdxAndType.getIdxAndType(colName);
    if(SQLITE_NULL == idxAndType.colType) {
      return nullopt;
    } else {
      return optional<uint64_t>(sqlite3_column_int64(m_stmt.get(), idxAndType.colIdx));
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

} // namespace wrapper
} // namespace rdbms
} // namespace cta
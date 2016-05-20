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

#pragma once

#include "catalogue/ColumnNameToIdxAndType.hpp"
#include "catalogue/DbRset.hpp"

#include <memory>
#include <stdint.h>
#include <sqlite3.h>

namespace cta {
namespace catalogue {

/**
 * Forward declaration.
 */
class SqliteStmt;

/**
 * The result set of an sql query.
 */
class SqliteRset: public DbRset {
public:

  /**
   * Constructor.
   *
   * @param stmt The prepared statement.
   */
  SqliteRset(SqliteStmt &stmt);

  /**
   * Destructor.
   */
  ~SqliteRset() throw();

  /**
   * Returns the SQL statement.
   *
   * @return The SQL statement.
   */
  virtual const std::string &getSql() const;

  /**
   * Attempts to get the next row of the result set.
   *
   * @return True if a row has been retrieved else false if there are no more
   * rows in the result set.
   */
  virtual bool next();

  /**
   * Returns true if the specified column contains a null value.
   *
   * @param colName The name of the column.
   * @return True if the specified column contains a null value.
   */
  virtual bool columnIsNull(const std::string &colName) const;

  /**
   * Returns the value of the specified column as a string.
   *
   * Please note that if the value of the column is NULL within the database
   * then an empty string shall be returned.  Use the columnIsNull() method to
   * determine whether not a column contains a NULL value.
   *
   * @param colName The name of the column.
   * @return The string value of the specified column.
   */
  virtual std::string columnText(const std::string &colName) const;

  /**
   * Returns the value of the specified column as an integer.
   *
   * @param colName The name of the column.
   * @return The value of the specified column.
   */
  virtual uint64_t columnUint64(const std::string &colName) const;

private:

  /**
   * The prepared statement.
   */
  SqliteStmt &m_stmt;

  /**
   * True if the next() method has not yet been called.
   */
  bool m_nextHasNotBeenCalled;

  /**
   * Map from column name to column index and type.
   */
  ColumnNameToIdxAndType m_colNameToIdxAndType;

  /**
   * Populates the map from column name to column index and type.
   */
  void populateColNameToIdxAndTypeMap();

}; // class SqlLiteRset

} // namespace catalogue
} // namespace cta

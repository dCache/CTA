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

#include "ColumnNameToIdx.hpp"
#include "Rset.hpp"

#include <memory>
#include <mutex>
#include <occi.h>

namespace cta {
namespace rdbms {

/**
 * Forward declaration to avoid a circular dependency between OcciRset and
 * OcciStmt.
 */
class OcciStmt;

/**
 * A convenience wrapper around an OCCI result set.
 */
class OcciRset: public Rset {
public:

  /**
   * Constructor.
   *
   * This constructor will throw an exception if the result set is a nullptr
   * pointer.
   *
   * @param stmt The OCCI statement.
   * @param rset The OCCI result set.
   */
  OcciRset(OcciStmt &stmt, oracle::occi::ResultSet *const rset);

  /**
   * Destructor.
   */
  virtual ~OcciRset() throw();

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
   * This method will return a nullptr column value as an optional with no value.
   *
   * @param colName The name of the column.
   * @return The string value of the specified column.
   */
  virtual optional<std::string> columnOptionalText(const std::string &colName) const;

  /**
   * Returns the value of the specified column as an integer.
   *
   * This method will return a nullptr column value as an optional with no value.
   *
   * @param colName The name of the column.
   * @return The value of the specified column.
   */
  virtual optional<uint64_t> columnOptionalUint64(const std::string &colName) const;

  /**
   * Idempotent close() method.  The destructor calls this method.
   */
  void close();

  /**
   * Returns the underlying OCCI result set.
   *
   * This method will always return a valid pointer.
   *
   * @return The underlying OCCI result set.
   */
  oracle::occi::ResultSet *get() const;

  /**
   * An alias for the get() method.
   *
   * @return The underlying OCCI result set.
   */
  oracle::occi::ResultSet *operator->() const;

private:

  /**
   * Mutex used to serialize access to this object.
   */
  mutable std::mutex m_mutex;

  /**
   * The OCCI statement.
   */
  OcciStmt &m_stmt;

  /**
   * The OCCI result set.
   */
  oracle::occi::ResultSet *m_rset;

  /**
   * Map from column name to column index.
   */
  ColumnNameToIdx m_colNameToIdx;

  /**
   * Populates the map from column name to column index.
   */
  void populateColNameToIdxMap();

}; // class OcciRset

} // namespace rdbms
} // namespace cta

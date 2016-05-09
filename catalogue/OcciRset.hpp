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

#include "catalogue/ColumnNameToIdx.hpp"

#include <memory>
#include <mutex>
#include <occi.h>

namespace cta {
namespace catalogue {

/**
 * Forward declaraion to avoid a circular dependency beween OcciRset and
 * OcciStmt.
 */
class OcciStmt;

/**
 * A convenience wrapper around an OCCI result set.
 */
class OcciRset {
public:

  /**
   * Constructor.
   *
   * This constructor will throw an exception if the result set is a NULL
   * pointer.
   *
   * @param stmt The OCCI statement.
   * @param rset The OCCI result set.
   */
  OcciRset(OcciStmt &stmt, oracle::occi::ResultSet *const rset);

  /**
   * Destructor.
   */
  ~OcciRset() throw();

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

  /**
   * Attempts to get the next row of the result set.
   *
   * @return True if a row has been retrieved else false if there are no more
   * rows in the result set.
   */
  bool next();

  /**
   * Returns the value of the specified column as a string.
   *
   * Please note that a C string is returned instead of an std::string so that
   * this method can be used by code compiled against the CXX11 ABI and by code
   * compiled against a newer ABI.
   *
   * @param colName The name of the column.
   * @return The string value of the specified column.  Please note that it is
   * the responsibility of the caller to free the memory associated with the
   * string using delete[] operator.
   */
  char *columnText(const char *const colName) const;

private:

  /**
   * Mutex used to serialize access to this object.
   */
  std::mutex m_mutex;

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

}; // class OcciRset

} // namespace catalogue
} // namespace cta

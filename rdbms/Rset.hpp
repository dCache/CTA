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

#include "common/optional.hpp"

#include <stdint.h>
#include <string>

namespace cta {
namespace rdbms {

/**
 * Forward declarartion.
 */
class RsetImpl;

/**
 * A wrapper around an object that iterators over a result set from the
 * execution of a database query.
 *
 * This wrapper permits the user of the rdbms::Stmt::executeQuery() method to
 * use different result set implementations whilst only using a result set type.
 */
class Rset {
public:
  /**
   * Constructor.
   */
  Rset();

  /**
   * Constructor.
   *
   * @param impl The object actually implementing this result set.
   */
  Rset(RsetImpl *const impl);

  /**
   * Deletion of copy constructor.
   */
  Rset(const Rset &) = delete;

  /**
   * Move constructor.
   *
   * @param other The other object to be moved.
   */
  Rset(Rset &&other);

  /**
   * Destructor.
   */
  ~Rset() throw();

  /**
   * Deletion of copy assignment.
   */
  Rset &operator=(const Rset &) = delete;

  /**
   * Move assignment.
   */
  Rset &operator=(Rset &&rhs);

  /**
   * Returns the SQL statement.
   *
   * @return The SQL statement.
   */
  const std::string &getSql() const;

  /**
   * Attempts to get the next row of the result set.
   *
   * @return True if a row has been retrieved else false if there are no more
   * rows in the result set.
   */
  bool next();

  /**
   * Returns true if the specified column contains a null value.
   *
   * @param colName The name of the column.
   * @return True if the specified column contains a null value.
   */
  bool columnIsNull(const std::string &colName) const;

  /**
   * Returns the value of the specified column as a string.
   *
   * This method will throw an exception if the value of the specified column
   * is nullptr.
   *
   * @param colName The name of the column.
   * @return The string value of the specified column.
   */
  std::string columnString(const std::string &colName) const;

  /**
   * Returns the value of the specified column as a string.
   *
   * This method will return a null column value as an optional with no value.
   *
   * @param colName The name of the column.
   * @return The string value of the specified column.
   */
  optional<std::string> columnOptionalString(const std::string &colName) const;

  /**
   * Returns the value of the specified column as an integer.
   *
   * This method will throw an exception if the value of the specified column
   * is nullptr.
   *
   * @param colName The name of the column.
   * @return The value of the specified column.
   */
  uint64_t columnUint64(const std::string &colName) const;

  /**
   * Returns the value of the specified column as a boolean.
   *
   * Please note that the underlying database column type is expected to be a
   * number where a non-zero value means true and a value of zero means false.
   *
   * This method will throw an exception if the value of the specified column
   * is nullptr.
   *
   * @param colName The name of the column.
   * @return The value of the specified column.
   */
  bool columnBool(const std::string &colName) const;

  /**
   * Returns the value of the specified column as an integer.
   *
   * This method will return a null column value as an optional with no value.
   *
   * @param colName The name of the column.
   * @return The value of the specified column.
   */
  optional<uint64_t> columnOptionalUint64(const std::string &colName) const;

  /**
   * Returns the value of the specified column as a boolean.
   *
   * Please note that the underlying database column type is expected to be a
   * number where a non-zero value means true and a value of zero means false.
   *
   * This method will return a null column value as an optional with no value.
   *
   * @param colName The name of the column.
   * @return The value of the specified column.
   */
  optional<bool> columnOptionalBool(const std::string &colName) const;

private:

  /**
   * The object actually implementing this result set.
   */
  RsetImpl *m_impl;

}; // class Rset

} // namespace rdbms
} // namespace cta

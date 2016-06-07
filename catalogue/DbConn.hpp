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

#include "catalogue/DbStmt.hpp"

namespace cta {
namespace catalogue {

/**
 * Abstract class that specifies the interface to a database connection.
 */
class DbConn {
public:

  /**
   * Destructor.
   */
  virtual ~DbConn() throw() = 0;

  /**
   * Idempotent close() method.  The destructor calls this method.
   */
  virtual void close() = 0;

  /**
   * Creates a prepared statement.
   *
   * @sql The SQL statement.
   * @return The prepared statement.
   */
  virtual DbStmt *createStmt(const std::string &sql) = 0;

  /**
   * Convenience function implemented in DbConn around DbConn::createStmt(),
   * DbStmt::executeNonQuery().
   *
   * @sql The SQL statement.
   */
  void executeNonQuery(const std::string &sql);

  /**
   * Commits the current transaction.
   */
  virtual void commit() = 0;

  /**
   * Rolls back the current transaction.
   */
  virtual void rollback() = 0;

}; // class DbConn

} // namespace catalogue
} // namespace cta

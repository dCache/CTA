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

#include "rdbms/ConnAndStmts.hpp"
#include "rdbms/Stmt.hpp"

#include <list>
#include <memory>

namespace cta {
namespace rdbms {

class ConnPool;

/**
 * A smart database connection that will automatically return the underlying
 * database connection to its parent connection pool when it goes out of scope.
 */
class Conn {
public:

  /**
   * Constructor.
   *
   * @param connAndStmts The database connection and its pool of prepared
   * statements.
   * @param pool The database connection pool to which the connection
   * should be returned.
   */
  Conn(std::unique_ptr<ConnAndStmts> connAndStmts, ConnPool *const pool);

  /**
   * Deletion of the copy constructor.
   */
  Conn(Conn &) = delete;

  /**
   * Move constructor.
   *
   * @param other The other object.
   */
  Conn(Conn &&other);

  /**
   * Destructor.
   *
   * Returns the database connection back to its pool.
   */
  ~Conn() noexcept;

  /**
   * Deletion of the copy assignment operator.
   */
  Conn &operator=(const Conn &) = delete;

  /**
   * Move assignment operator.
   *
   * @param rhs The object on the right-hand side of the operator.
   * @return This object.
   */
  Conn &operator=(Conn &&rhs);

  /**
   * Creates a prepared statement.
   *
   * @param sql The SQL statement.
   * @param autocommitMode The autocommit mode of the statement.
   * @return The prepared statement.
   */
  Stmt createStmt(const std::string &sql, const AutocommitMode autocommitMode);

  /**
   * Convenience method that parses the specified string of multiple SQL
   * statements and calls executeNonQuery() for each individual statement found.
   *
   * Please note that each statement should be a non-query terminated by a
   * semicolon and that each individual statement will be executed with
   * autocommit ON.
   *
   * @param sqlStmts The SQL statements to be executed.
   * @param autocommitMode The autocommit mode of the statement.
   */
  void executeNonQueries(const std::string &sqlStmts);

  /**
   * Convenience method that wraps Conn::createStmt() followed by
   * Stmt::executeNonQuery().
   *
   * @param sql The SQL statement.
   * @param autocommitMode The autocommit mode of the statement.
   */
  void executeNonQuery(const std::string &sql, const AutocommitMode autocommitMode);

  /**
   * Commits the current transaction.
   */
  void commit();

  /**
   * Rolls back the current transaction.
   */
  void rollback();

  /**
   * Returns the names of all the tables in the database schema in alphabetical
   * order.
   *
   * @return The names of all the tables in the database schema in alphabetical
   * order.
   */
  std::list<std::string> getTableNames();

  /**
   * Closes the underlying database connection.  This method should only be used
   * in extreme cases where the closure of the underlying database connection
   * needs to be forced.
   */
  void closeUnderlyingDatabaseConnection();

  /**
   * Returns true if this connection is open.
   */
  bool isOpen() const;

  /**
   * Returns the names of all the sequences in the database schema in
   * alphabetical order.
   *
   * If the underlying database technologies does not supported sequences then
   * this method simply returns an empty list.
   *
   * @return The names of all the sequences in the database schema in
   * alphabetical order.
   */
  std::list<std::string> getSequenceNames();

private:

  /**
   * The database connection and its pool of prepared statements.
   */
  std::unique_ptr<ConnAndStmts> m_connAndStmts;

  /**
   * The database connection pool to which the m_conn should be returned.
   */
  ConnPool *m_pool;

}; // class Conn

} // namespace rdbms
} // namespace cta

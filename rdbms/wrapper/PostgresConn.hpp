/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2019  CERN
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

#include "common/threading/RWLock.hpp"
#include "rdbms/wrapper/ConnWrapper.hpp"
#include "rdbms/wrapper/Postgres.hpp"

#include <list>
#include <memory>
#include <string>
#include <cstdint>

namespace cta {
namespace rdbms {
namespace wrapper {

class PostgresStmt;
class PostgresRset;

class PostgresConn: public ConnWrapper {
public:

  /**
   * The PostgresStmt and PostgresRset classes need to use the RWLock in this class and use various private methods.
   */
  friend PostgresStmt;
  friend PostgresRset;


  /**
   * Constructor.
   *
   * @param conninfo The conninfo string to pass to PQconnectdb. This is a postgres URI or a series of key=value pairs separated by white space.
   * 
   */
  PostgresConn(const std::string &conninfo);

  /**
   * Destructor.
   */
  ~PostgresConn() override;

  /**
   * Idempotent close() method.  The destructor calls this method.
   */
  void close() override;

  /**
   * Commits the current transaction.
   */
  void commit() override;

  /**
   * Creates a prepared statement.
   *
   * @param sql The SQL statement.
   * @return The prepared statement.
   */
  std::unique_ptr<StmtWrapper> createStmt(const std::string &sql) override;

  /**
   * Executes the sql string, without returning any result set.
   *
   * @param sql The SQL statement.
   */
  void executeNonQuery(const std::string &sql) override;

  /**
   * Returns the autocommit mode of the connection.
   *
   * @return The autocommit mode of the connection.
   */
  AutocommitMode getAutocommitMode() const noexcept override;

  /**
   * Returns the names of all the sequences in the database schema in
   * alphabetical order.
   *
   * @return The names of all the sequences in the database schema in
   * alphabetical order.
   */
  std::list<std::string> getSequenceNames() override;

  /**
   * Returns the names of all the tables in the database schema in alphabetical
   * order.
   *
   * @return The names of all the tables in the database schema in alphabetical
   * order.
   */
  std::list<std::string> getTableNames() override;

  /**
   * Returns true if this connection is open.
   */
  bool isOpen() const override;

  /**
   * Rolls back the current transaction.
   */
  void rollback() override;

  /**
   * Sets the autocommit mode of the connection.
   *
   * @param autocommitMode The autocommit mode of the connection.
   * @throw AutocommitModeNotSupported If the specified autocommit mode is not
   * supported.
   */
  void setAutocommitMode(const AutocommitMode autocommitMode) override;

private:

  /**
   * Closes the conneciton, freeing the underlying libpq conneciton.
   * Used by the public close() mehtod, without locking.
   */
  void closeAssumeLocked();

  /**
   * Deallocates the specified statement.
   *
   * @param stmt The name of the statement to be closed.
   */
  void deallocateStmt(const std::string &stmt);

  /**
   * Get the libpq postgres connection
   */
   PGconn* get() { return m_pgsqlConn; }

  /**
   * Getter for m_asyncInProgress
   *
   * @return m_asyncInProgress
   */
   bool isAsyncInProgress() const { return m_asyncInProgress; }

  /**
   * Indicates if this connection is open. This is an internal funciton,
   * used by the public isOpen() method, without locking.
   *
   * @return true if this connection is open.
   */
  bool isOpenAssumeLocked() const;

  /**
   * Returns the name to be used for the next statement on this connection
   *
   * @return Name for next statement
   */
   std::string nextStmtName();

  /**
   * Function for handling postgres notices.
   * This is a static member function.
   */
  static void noticeProcessor(void *arg, const char *message);

  /**
   * Setter for m_asyncInProgress
   *
   * @val Indicates if async command is ongoing.
   */
   void setAsyncInProgress(const bool val) { m_asyncInProgress=val; }

  /**
   * Conditionally throws a DB related exception if the result status is not the expected one
   *
   * @param res The PGresult
   * @parm requiredStatus The required status
   * @param prefix A prefix to place in the message if it is thrown
   */
  void throwDBIfNotStatus(const PGresult *res, const ExecStatusType requiredStatus, const std::string &prefix);

  /**
   * RW lock used to serialize access to the database connection or access to connection parameters.
   */
  mutable threading::RWLock m_lock;

  /**
   * represent postgres connection, used by libpq
   */
  PGconn* m_pgsqlConn;

  /**
   * indicate if we have sent a command that will send results or take data while
   * it is processed by the server.
   * New async (PQSend.. or a COPY) requests can not be started during this time, and
   * if new sync commands are sent they will interrupt communiction with the
   * ongoing async command.
   */
   bool m_asyncInProgress;

  /**
   * Counter for number of statements ever created.
   * Used to ensure unique statement naming on the conneciton.
   */
   uint64_t m_nStmts;

}; // class PostgresConn


} // namespace wrapper
} // namespace rdbms
} // namespace cta
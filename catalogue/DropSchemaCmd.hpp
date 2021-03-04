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

#include "catalogue/Catalogue.hpp"
#include "catalogue/CmdLineTool.hpp"
#include "rdbms/Conn.hpp"
#include "rdbms/Login.hpp"

namespace cta {
namespace catalogue {

/**
 * Command-line tool that drops the schema of the catalogue database.
 */
class DropSchemaCmd: public CmdLineTool {
public:

  /**
   * Constructor.
   *
   * @param inStream Standard input stream.
   * @param outStream Standard output stream.
   * @param errStream Standard error stream.
   */
  DropSchemaCmd(std::istream &inStream, std::ostream &outStream, std::ostream &errStream);

  /**
   * Destructor.
   */
  ~DropSchemaCmd() noexcept;

private:

  /**
   * An exception throwing version of main().
   *
   * @param argc The number of command-line arguments including the program name.
   * @param argv The command-line arguments.
   * @return The exit value of the program.
   */
  int exceptionThrowingMain(const int argc, char *const *const argv) override;

  /**
   * Prints the usage message of the command-line tool.
   *
   * @param os The output stream to which the usage message is to be printed.
   */
  void printUsage(std::ostream &os) override;

  /**
   * Asks the user to confirm that they want to drop the schema of the catalogue
   * database.
   *
   * @param dbLogin The database login.
   * @return True if the user confirmed.
   */
  bool userConfirmsDropOfSchema(const rdbms::Login &dbLogin);

  /**
   * Unconditionally drops the schema of the catalogue database associated with
   * the specified database connection.
   *
   * @param dbType The database type.
   * @param conn The database connection.
   */
  void dropCatalogueSchema(const rdbms::Login::DbType &dbType, rdbms::Conn &conn);

  /**
   * Drops the database tables with the specified names.
   *
   * @param conn The database connection.
   */
  void dropDatabaseTables(rdbms::Conn &conn);

  /**
   * Drops the database sequences with the specified names.
   *
   * @param conn The database connection.
   */
  void dropDatabaseSequences(rdbms::Conn &conn);
  
  /**
   * Checks if the IS_PRODUCTION bit is set on the CTA_CATALOGUE table
   * @return true if the IS_PRODUCTION bit is set, false otherwise
   */
  bool isProductionSet(rdbms::Conn & conn);
  
  /**
   * Checks if we can check the IS_PRODUCTION bit. This allows the backward-compatibility of 
   * this tool if we use it against a schema that does not have the IS_PRODUCTION bit.
   * @param conn the connection to the Catalogue database
   * @param dbType the type of the Catalogue database
   * @return true if the production bit is set, false otherwise
   */
  bool isProductionProtectionCheckable(rdbms::Conn & conn, const cta::rdbms::Login::DbType dbType);

}; // class DropSchemaCmd

} // namespace catalogue
} // namespace cta

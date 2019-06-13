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

#include "catalogue/CmdLineTool.hpp"
#include "rdbms/Conn.hpp"

namespace cta {
namespace catalogue {

/**
 * Command-line tool for verifying the catalogue schema.
 */
class VerifySchemaCmd: public CmdLineTool {
public:

  /**
   * Constructor.
   *
   * @param inStream Standard input stream.
   * @param outStream Standard output stream.
   * @param errStream Standard error stream.
   */
  VerifySchemaCmd(std::istream &inStream, std::ostream &outStream, std::ostream &errStream);

  /**
   * Destructor.
   */
  ~VerifySchemaCmd() noexcept;
  
  enum class VerifyStatus { OK, INFO, ERROR, UNKNOWN };

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
   * Returns true if the table with the specified name exists in the database
   * schema of the specified database connection.
   *
   * @param tableName The name of the database table.
   * @param conn The database connection.
   * @return True if the table exists.
   */
  bool tableExists(const std::string tableName, rdbms::Conn &conn) const;
  
  /**
   * TODO
   * @param 
   * @param 
   * @return 
   */
  VerifyStatus verifyTableNames(const std::list<std::string> &schemaTableNames, 
    const std::list<std::string> &dbTableNames) const;
  
  /**
   * TODO
   * @param 
   * @param 
   * @return 
   */
  VerifyStatus verifyIndexNames(const std::list<std::string> &schemaIndexNames, 
    const std::list<std::string> &dbIndexNames) const;

}; // class VerifySchemaCmd

} // namespace catalogue
} // namespace cta

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

  /**
   * An exception throwing version of main().
   *
   * @param argc The number of command-line arguments including the program name.
   * @param argv The command-line arguments.
   * @return The exit value of the program.
   */
  int exceptionThrowingMain(const int argc, char *const *const argv) override;

private:

  /**
   * Unconditionally drops the schema of the catalogue database associated with
   * the specified database login.
   *
   * @param dbLogin The database login.
   */
  void dropCatalogueSchema(const rdbms::Login &dbLogin);

}; // class DropSchemaCmd

} // namespace catalogue
} // namespace cta

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

#include "CmdLineTool.hpp"
#include "rdbms/Conn.hpp"
#include "rdbms/Login.hpp"

namespace cta {
namespace catalogue {
  
class SetProductionCmd: public CmdLineTool {
public:
  /**
   * Constructor.
   *
   * @param inStream Standard input stream.
   * @param outStream Standard output stream.
   * @param errStream Standard error stream.
   */
  SetProductionCmd(std::istream &inStream, std::ostream &outStream, std::ostream &errStream);
  ~SetProductionCmd() noexcept;
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
   * Returns true if the IS_PRODUCTION flag is settable, false otherwise
   * @param login the database login informations
   * @param conn the connection to the database
   * @return true if the IS_PRODUCTION flag is settable, false otherwise
   */
  bool isProductionSettable(const cta::rdbms::Login & login, cta::rdbms::Conn & conn);
  
  /**
   * Set the IS_PRODUCTION flag to true on the CTA Catalogue
   * @param conn the connection to the CTA Catalogue database
   */
  void setProductionFlag(cta::rdbms::Conn & conn);
};

}}
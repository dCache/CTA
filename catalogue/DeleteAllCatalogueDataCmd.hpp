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

namespace cta {
namespace catalogue {

/**
 * Command-line tool that deletes all the rows of all the tables in the
 * catalogue database except for the CTA_CATALOGUE table.
 */
class DeleteAllCatalogueDataCmd: public CmdLineTool {
public:

  /**
   * Constructor.
   *
   * @param inStream Standard input stream.
   * @param outStream Standard output stream.
   * @param errStream Standard error stream.
   */
  DeleteAllCatalogueDataCmd(std::istream &inStream, std::ostream &outStream, std::ostream &errStream);

  /**
   * Destructor.
   */
  ~DeleteAllCatalogueDataCmd() noexcept;

  /**
   * An exception throwing version of main().
   *
   * @param argc The number of command-line arguments including the program name.
   * @param argv The command-line arguments.
   * @return The exit value of the program.
   */
  int exceptionThrowingMain(const int argc, char *const *const argv);

private:

  /**
   * Deletes all of the rows of all of the tables in the specified catalogue
   * database except for the CTA_CATALOGUE table.
   *
   * @param catalogue The catalogue database.
   */
  void deleteAllRowsExceptForCTA_CATALOGUE(Catalogue &catalogue);

  /**
   * Deletes all admin users from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteAdminUsers(Catalogue &catalogue);

  /**
   * Deletes all admin hosts from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteAdminHosts(Catalogue &catalogue);

  /**
   * Deletes all archive routes from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteArchiveRoutes(Catalogue &catalogue);

  /**
   * Deletes all requester mount-rules from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteRequesterMountRules(Catalogue &catalogue);

  /**
   * Deletes all requester-group mount-rules from the specified catalogue
   * database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteRequesterGroupMountRules(Catalogue &catalogue);

  /**
   * Deletes all archive files and their associated tape files from the
   * specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteArchiveAndTapeFiles(Catalogue &catalogue);

  /**
   * Deletes all tapes from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteTapes(Catalogue &catalogue);

  /**
   * Deletes all storage classes from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteStorageClasses(Catalogue &catalogue);

  /**
   * Deletes all tape pools from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteTapePools(Catalogue &catalogue);

  /**
   * Deletes all logical libraries from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteLogicalLibrares(Catalogue &catalogue);

  /**
   * Deletes all mount policies from the specified catalogue database.
   *
   * @param catalogue The catalogue database.
   */
  void deleteMountPolicies(Catalogue &catalogue);

}; // class DeleteAllCatalogueDataCmd

} // namespace catalogue
} // namespace cta

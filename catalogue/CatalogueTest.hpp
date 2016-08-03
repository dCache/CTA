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

#include "catalogue/ArchiveFileRow.hpp"
#include "catalogue/CatalogueFactory.hpp"
#include "common/exception/Exception.hpp"
#include "rdbms/LoginFactory.hpp"

#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <set>

namespace unitTests {

class cta_catalogue_CatalogueTest : public ::testing::TestWithParam<cta::rdbms::LoginFactory*> {
public:

  cta_catalogue_CatalogueTest();

protected:

  std::unique_ptr<cta::catalogue::Catalogue> m_catalogue;
  const std::string m_bootstrapComment;
  cta::common::dataStructures::SecurityIdentity m_cliSI;
  cta::common::dataStructures::SecurityIdentity m_bootstrapAdminSI;
  cta::common::dataStructures::SecurityIdentity m_adminSI;

  virtual void SetUp();

  virtual void TearDown();

  /**
   * Creates a map from VID to tape given the specified list of tapes.
   *
   * @param listOfTapes The list of tapes from which the map shall be created.
   * @return The map from VID to tape.
   */
  std::map<std::string, cta::common::dataStructures::Tape> tapeListToMap(
    const std::list<cta::common::dataStructures::Tape> &listOfTapes);

  /**
   * Creates a map from archive file ID to archive file from the specified
   * iterator.
   *
   * @param itor Iterator over archive files.
   * @return Map from archive file ID to archive file.
   */
  std::map<uint64_t, cta::common::dataStructures::ArchiveFile> archiveFileItorToMap(
    cta::catalogue::ArchiveFileItor &itor);

  /**
   * Creates a map from admin username to admin user from the specified list of
   * admin users.
   *
   * @param itor Iterator over archive files.
   * @return Map from archive file ID to archive file.
   */
  std::map<std::string, cta::common::dataStructures::AdminUser> adminUserListToMap(
    const std::list<cta::common::dataStructures::AdminUser> &listOfAdminUsers);
}; // cta_catalogue_CatalogueTest

} // namespace unitTests

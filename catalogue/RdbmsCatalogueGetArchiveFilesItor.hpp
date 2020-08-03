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

#include "catalogue/ArchiveFileBuilder.hpp"
#include "catalogue/ArchiveFileItorImpl.hpp"
#include "catalogue/TapeFileSearchCriteria.hpp"
#include "common/dataStructures/ArchiveFile.hpp"
#include "common/log/Logger.hpp"
#include "rdbms/ConnPool.hpp"
#include "rdbms/Stmt.hpp"
#include "rdbms/Rset.hpp"

namespace cta {
namespace catalogue {

/**
 * RdbmsCatalogue::getArchiveFiles() implementation of ArchiveFileItorImpl.
 */
class RdbmsCatalogueGetArchiveFilesItor: public ArchiveFileItorImpl {
public:

  /**
   * Constructor.
   *
   * @param log Object representing the API to the CTA logging system.
   * @param conn The database connection.
   * @param searchCriteria The search criteria to be used when listing archive
   * files.
   */
  RdbmsCatalogueGetArchiveFilesItor(
    log::Logger &log,
    rdbms::Conn conn,
    const TapeFileSearchCriteria &searchCriteria,
    const std::string &tempDiskFxidsTableName);

  /**
   * Destructor.
   */
  ~RdbmsCatalogueGetArchiveFilesItor() override;

  /**
   * Returns true if a call to next would return another archive file.
   */
  bool hasMore() override;

  /**
   * Returns the next archive or throws an exception if there isn't one.
   */
  common::dataStructures::ArchiveFile next() override;

private:

  /**
   * Object representing the API to the CTA logging system.
   */
  log::Logger &m_log;

  /**
   * The search criteria to be used when listing archive files.
   */
  TapeFileSearchCriteria m_searchCriteria;

  /**
   * True if the result set is empty.
   */
  bool m_rsetIsEmpty;

  /**
   * True if hasMore() has been called and the corresponding call to next() has
   * not.
   *
   * This member-variable is used to prevent next() being called before
   * hasMore().
   */
  bool m_hasMoreHasBeenCalled;

  /**
   * The database connection.
   */
  rdbms::Conn m_conn;

  /**
   * The database statement.
   */
  rdbms::Stmt m_stmt;

  /**
   * The result set of archive files that is to be iterated over.
   */
  rdbms::Rset m_rset;

  /**
   * Releases the database resources.
   *
   * This method is idempotent.
   */
  void releaseDbResources() noexcept;

  /**
   * Builds ArchiveFile objects from a stream of tape files ordered by archive
   * ID and then copy number.
   */
  ArchiveFileBuilder<cta::common::dataStructures::ArchiveFile> m_archiveFileBuilder;
}; // class RdbmsCatalogueGetArchiveFilesItor

} // namespace catalogue
} // namespace cta

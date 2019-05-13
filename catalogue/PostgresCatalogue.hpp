/*
 * The CERN Tape Archive(CTA) project
 * Copyright(C) 2019  CERN
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

#include "catalogue/RdbmsCatalogue.hpp"
#include "rdbms/Conn.hpp"

namespace cta {
namespace catalogue {

/**
 * An Postgres based implementation of the CTA catalogue.
 */
class PostgresCatalogue: public RdbmsCatalogue {
public:

  /**
   * Constructor.
   *
   * @param log Object representing the API to the CTA logging system.
   * @param username The database username.
   * @param password The database password.
   * @param database The database name.
   * @param nbConns The maximum number of concurrent connections to the
   * underlying relational database for all operations accept listing archive
   * files which can be relatively long operations.
   * @param nbArchiveFileListingConns The maximum number of concurrent
   * connections to the underlying relational database for the sole purpose of
   * listing archive files.
   */
  PostgresCatalogue(
    log::Logger &log,
    const rdbms::Login &login,
    const uint64_t nbConns,
    const uint64_t nbArchiveFileListingConns);

  /**
   * Destructor.
   */
  ~PostgresCatalogue() override;

  /**
   * Deletes the specified archive file and its associated tape copies from the
   * catalogue.
   *
   * Please note that the name of the disk instance is specified in order to
   * prevent a disk instance deleting an archive file that belongs to another
   * disk instance.
   *
   * Please note that this method is idempotent.  If the file to be deleted does
   * not exist in the CTA catalogue then this method returns without error.
   *
   * @param instanceName The name of the instance from where the deletion request
   * originated
   * @param archiveFileId The unique identifier of the archive file.
   * @param lc The log context.
   * @return The metadata of the deleted archive file including the metadata of
   * the associated and also deleted tape copies.
   */
  void deleteArchiveFile(const std::string &instanceName, const uint64_t archiveFileId,
    log::LogContext &lc) override;

  /**
   * Notifies the catalogue that the specified files have been written to tape.
   *
   * @param events The tape file written events.
   */
  void filesWrittenToTape(const std::set<TapeItemWrittenPointer> &events) override;

  /**
   * Returns a unique archive ID that can be used by a new archive file within
   * the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return A unique archive ID that can be used by a new archive file within
   * the catalogue.
   */
  uint64_t getNextArchiveFileId(rdbms::Conn &conn) override;

  /**
   * Returns a unique storage class ID that can be used by a new storage class
   * within the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique archive ID that can be used by a new archive file within
   * within the catalogue.
   */
  uint64_t getNextStorageClassId(rdbms::Conn &conn) override;


private:

  /**
   * Start a database transaction and then create the temporary
   * tables TEMP_ARCHIVE_FILE_BATCH and TEMP_TAPE_FILE_BATCH.
   * Sets deferred mode for one of the db constraints to avoid
   * violations during concurrent bulk insert.
   *
   * @parm conn The database connection.
   */
  void beginCreateTemporarySetDeferred(rdbms::Conn &conn) const;

  /**
   * Selects the specified tape within the Tape table for update.
   *
   * @param conn The database connection.
   * @param vid The volume identifier of the tape.
   */
  common::dataStructures::Tape selectTapeForUpdate(rdbms::Conn &conn, const std::string &vid) const;

  /**
   * Batch inserts rows into the ARCHIVE_FILE table that correspond to the
   * specified TapeFileWritten events.
   *
   * This method has idempotent behaviour in the case where an ARCHIVE_FILE
   * already exists.  Such a situation will occur when a file has more than one
   * copy on tape.  The first tape copy will cause two successful inserts, one
   * into the ARCHIVE_FILE table and one into the  TAPE_FILE table.  The second
   * tape copy will try to do the same, but the insert into the ARCHIVE_FILE
   * table will fail or simply bounce as the row will already exists.  The
   * insert into the TABLE_FILE table will succeed because the two TAPE_FILE
   * rows will be unique.
   *
   * @param conn The database connection.
   * @param events The tape file written events.
   */
  void idempotentBatchInsertArchiveFiles(rdbms::Conn &conn, const std::set<TapeFileWritten> &events) const;

  /**
   * The size and checksum of a file.
   */
  struct FileSizeAndChecksum {
    uint64_t fileSize;
    std::string checksumType;
    std::string checksumValue;
  };

  /**
   * Returns the sizes and checksums of the specified archive files.
   *
   * @param conn The database connection.
   * @param events The tape file written events that identify the archive files.
   * @return A map from the identifier of each archive file to its size and checksum.
   */
  std::map<uint64_t, FileSizeAndChecksum> selectArchiveFileSizesAndChecksums(rdbms::Conn &conn,
    const std::set<TapeFileWritten> &events) const;

  /**
   * Batch inserts rows into the TAPE_FILE_BATCH temporary table that correspond
   * to the specified TapeFileWritten events.
   *
   * @param conn The database connection.
   * @param events The tape file written events.
   */
  void insertTapeFileBatchIntoTempTable(rdbms::Conn &conn, const std::set<TapeFileWritten> &events) const;

}; // class PostgresCatalogue

} // namespace catalogue
} // namespace cta
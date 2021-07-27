/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2015-2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "catalogue/RdbmsCatalogue.hpp"
#include "catalogue/InsertFileRecycleLog.hpp"
#include "rdbms/Conn.hpp"

namespace cta {
namespace catalogue {

/**
 * An Oracle based implementation of the CTA catalogue.
 */
class OracleCatalogue: public RdbmsCatalogue {
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
  OracleCatalogue(
    log::Logger       &log,
    const std::string &username,
    const std::string &password,
    const std::string &database,
    const uint64_t nbConns,
    const uint64_t nbArchiveFileListingConns);

  /**
   * Destructor.
   */
  ~OracleCatalogue() override;

  /**
   * Creates a temporary table from the list of disk file IDs provided in the search criteria.
   *
   * @param conn The database connection.
   * @param diskFileIds List of disk file IDs (fxid).
   * @return Name of the temporary table
   */
  std::string createAndPopulateTempTableFxid(rdbms::Conn &conn, const optional<std::vector<std::string>> &diskFileIds) const override;

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
   * Returns a unique logical library ID that can be used by a new logical
   * library within the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique logical library ID that can be used by a new logical
   * library storage class within the catalogue.
   */
  uint64_t getNextLogicalLibraryId(rdbms::Conn &conn) override;
  
  /**
   * Returns a unique virtual organization ID that can be used by a new Virtual Organization
   * within the catalogue.
   * 
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   * 
   * @param conn The database connection
   * @return a unique virtual organization ID that can be used by a new Virtual Organization
   * within the catalogue.
   */
  uint64_t getNextVirtualOrganizationId(rdbms::Conn &conn) override;

  /**
   * Returns a unique media type ID that can be used by a new media type within
   * the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique media type ID that can be used by a new media type
   * within the catalogue.
   */
  uint64_t getNextMediaTypeId(rdbms::Conn &conn) override;

  /**
   * Returns a unique storage class ID that can be used by a new storage class
   * within the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique storage class ID that can be used by a new storage class
   * within the catalogue.
   */
  uint64_t getNextStorageClassId(rdbms::Conn &conn) override;

  /**
   * Returns a unique tape pool ID that can be used by a new tape pool within
   * the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique tape pool ID that can be used by a new tape pool within
   * the catalogue.
   */
  uint64_t getNextTapePoolId(rdbms::Conn &conn) override;
  
  /**
   * Returns a unique file recycle log ID that can be used by a new entry of file recycle log within
   * the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique file recycle log ID that can be used by a new entry of file recycle log within
   * the catalogue.
   */
  uint64_t getNextFileRecyleLogId(rdbms::Conn & conn) override;
  

  /**
   * Notifies the catalogue that the specified files have been written to tape.
   *
   * @param events The tape file written events.
   */
  void filesWrittenToTape(const std::set<TapeItemWrittenPointer> &events) override;

  /**
   * !!!!!!!!!!!!!!!!!!! THIS METHOD SHOULD NOT BE USED !!!!!!!!!!!!!!!!!!!!!!!
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
  void DO_NOT_USE_deleteArchiveFile_DO_NOT_USE(const std::string &instanceName, const uint64_t archiveFileId,
    log::LogContext &lc) override;

private:

  /**
   * Selects the specified tape for update and returns its last FSeq.
   *
   * @param conn The database connection.
   * @param vid The volume identifier of the tape.
   * @return The last FSeq of the tape.
   */
  uint64_t selectTapeForUpdateAndGetLastFSeq(rdbms::Conn &conn, const std::string &vid);

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
  void idempotentBatchInsertArchiveFiles(rdbms::Conn &conn, const std::set<TapeFileWritten> &events);
  
  /**
   * In the case we insert a TAPE_FILE that already has a copy on the catalogue (same copyNb),
   * this TAPE_FILE will go to the FILE_RECYCLE_LOG table.
   * 
   * This case happens always during the repacking of a tape: the new TAPE_FILE created 
   * will replace the old one, the old one will then be moved to the FILE_RECYCLE_LOG table
   * 
   * @param conn The database connection.
   * @returns the list of inserted fileRecycleLog
   */
  std::list<cta::catalogue::InsertFileRecycleLog> insertOldCopiesOfFilesIfAnyOnFileRecycleLog(rdbms::Conn & conn);

   /**
   * Copy the archiveFile and the associated tape files from the ARCHIVE_FILE and TAPE_FILE tables to the FILE_RECYCLE_LOG table
   * and deletes the ARCHIVE_FILE and TAPE_FILE entries.
   * @param conn the database connection
   * @param request the request that contains the necessary informations to identify the archiveFile to copy to the FILE_RECYCLE_LOG table
   * @param lc the log context
   */
  void copyArchiveFileToFileRecyleLogAndDelete(rdbms::Conn & conn, const common::dataStructures::DeleteArchiveRequest &request, log::LogContext & lc) override;
  
   /**
   * Delete the TapeFiles and the ArchiveFile from the recycle-bin in one transaction
   * @param conn the database connection
   * @param archiveFileId the archiveFileId of the file to delete from the recycle-bin
   */
  void deleteTapeFilesAndArchiveFileFromRecycleBin(rdbms::Conn& conn, const uint64_t archiveFileId, log::LogContext& lc) override;
  
  /**
   * Copy the tape files from the TAPE_FILE tables to the FILE_RECYCLE_LOG table
   * and deletes the TAPE_FILE entry.
   * @param conn the database connection
   * @param file the file to be deleted
   * @param reason The reason for deleting the tape file copy
   * @param lc the log context
   */
  void copyTapeFileToFileRecyleLogAndDelete(rdbms::Conn & conn, const cta::common::dataStructures::ArchiveFile &file, 
                                            const std::string &reason, log::LogContext & lc) override;

  /**
   * The size and checksum of a file.
   */
  struct FileSizeAndChecksum {
    uint64_t fileSize;
    checksum::ChecksumBlob checksumBlob;
  };

  /**
   * Returns the sizes and checksums of the specified archive files.
   *
   * @param conn The database connection.
   * @param events The tape file written events that identify the archive files.
   * @return A map from the identifier of each archive file to its size and checksum.
   */
  std::map<uint64_t, FileSizeAndChecksum> selectArchiveFileSizesAndChecksums(rdbms::Conn &conn,
    const std::set<TapeFileWritten> &events);

}; // class OracleCatalogue

} // namespace catalogue
} // namespace cta

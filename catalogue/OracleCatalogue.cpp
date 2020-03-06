/*
 * The CERN Tape Archive(CTA) project
 * Copyright(C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "catalogue/ArchiveFileRow.hpp"
#include "catalogue/OracleCatalogue.hpp"
#include "catalogue/retryOnLostConnection.hpp"
#include "common/exception/Exception.hpp"
#include "common/exception/LostDatabaseConnection.hpp"
#include "common/exception/TapeFseqMismatch.hpp"
#include "common/exception/UserError.hpp"
#include "common/make_unique.hpp"
#include "common/threading/MutexLocker.hpp"
#include "common/Timer.hpp"
#include "common/utils/utils.hpp"
#include "rdbms/AutoRollback.hpp"
#include "rdbms/rdbms.hpp"
#include "rdbms/wrapper/OcciColumn.hpp"
#include "rdbms/wrapper/OcciStmt.hpp"
#include <algorithm>

namespace cta {
namespace catalogue {

namespace {
  /**
   * Structure used to assemble a batch of rows to insert into the TAPE_FILE
   * table.
   */
  struct TapeFileBatch {
    size_t nbRows;
    rdbms::wrapper::OcciColumn vid;
    rdbms::wrapper::OcciColumn fSeq;
    rdbms::wrapper::OcciColumn blockId;
    rdbms::wrapper::OcciColumn fileSize;
    rdbms::wrapper::OcciColumn copyNb;
    rdbms::wrapper::OcciColumn creationTime;
    rdbms::wrapper::OcciColumn archiveFileId;

    /**
     * Constructor.
     *
     * @param nbRowsValue  The Number of rows to be inserted.
     */
    TapeFileBatch(const size_t nbRowsValue):
      nbRows(nbRowsValue),
      vid("VID", nbRows),
      fSeq("FSEQ", nbRows),
      blockId("BLOCK_ID", nbRows),
      fileSize("LOGICAL_SIZE_IN_BYTES", nbRows),
      copyNb("COPY_NB", nbRows),
      creationTime("CREATION_TIME", nbRows),
      archiveFileId("ARCHIVE_FILE_ID", nbRows) {
    }
  }; // struct TapeFileBatch

  /**
   * Structure used to assemble a batch of rows to insert into the ARCHIVE_FILE
   * table.
   */
  struct ArchiveFileBatch {
    size_t nbRows;
    rdbms::wrapper::OcciColumn archiveFileId;
    rdbms::wrapper::OcciColumn diskInstance;
    rdbms::wrapper::OcciColumn diskFileId;
    rdbms::wrapper::OcciColumn diskFilePath;
    rdbms::wrapper::OcciColumn diskFileUser;
    rdbms::wrapper::OcciColumn diskFileGroup;
    rdbms::wrapper::OcciColumn size;
    rdbms::wrapper::OcciColumn checksumBlob;
    rdbms::wrapper::OcciColumn checksumAdler32;
    rdbms::wrapper::OcciColumn storageClassName;
    rdbms::wrapper::OcciColumn creationTime;
    rdbms::wrapper::OcciColumn reconciliationTime;

    /**
     * Constructor.
     *
     * @param nbRowsValue  The Number of rows to be inserted.
     */
    ArchiveFileBatch(const size_t nbRowsValue):
      nbRows(nbRowsValue),
      archiveFileId("ARCHIVE_FILE_ID", nbRows),
      diskInstance("DISK_INSTANCE_NAME", nbRows),
      diskFileId("DISK_FILE_ID", nbRows),
      diskFilePath("DISK_FILE_PATH", nbRows),
      diskFileUser("DISK_FILE_UID", nbRows),
      diskFileGroup("DISK_FILE_GID", nbRows),
      size("SIZE_IN_BYTES", nbRows),
      checksumBlob("CHECKSUM_BLOB", nbRows),
      checksumAdler32("CHECKSUM_ADLER32", nbRows),
      storageClassName("STORAGE_CLASS_NAME", nbRows),
      creationTime("CREATION_TIME", nbRows),
      reconciliationTime("RECONCILIATION_TIME", nbRows) {
    }
  }; // struct ArchiveFileBatch

  /**
   * Structure used to assemble a batch of rows to insert into the
   * TAPE_FILE_BATCH temporary table.
   */
  struct TempTapeFileBatch {
    size_t nbRows;
    rdbms::wrapper::OcciColumn archiveFileId;

    /**
     * Constructor.
     *
     * @param nbRowsValue  The Number of rows to be inserted.
     */
    TempTapeFileBatch(const size_t nbRowsValue):
      nbRows(nbRowsValue),
      archiveFileId("ARCHIVE_FILE_ID", nbRows) {
    }
  }; // struct TempTapeFileBatch
} // anonymous namespace

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
OracleCatalogue::OracleCatalogue(
  log::Logger &log,
  const std::string &username,
  const std::string &password,
  const std::string &database,
  const uint64_t nbConns,
  const uint64_t nbArchiveFileListingConns):
  RdbmsCatalogue(
    log,
    rdbms::Login(rdbms::Login::DBTYPE_ORACLE, username, password, database, "", 0),
    nbConns,
    nbArchiveFileListingConns) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OracleCatalogue::~OracleCatalogue() {
}

//------------------------------------------------------------------------------
// getNextArchiveFileId
//------------------------------------------------------------------------------
uint64_t OracleCatalogue::getNextArchiveFileId(rdbms::Conn &conn) {
  try {
    const char *const sql =
      "SELECT "
        "ARCHIVE_FILE_ID_SEQ.NEXTVAL AS ARCHIVE_FILE_ID "
      "FROM "
        "DUAL";
    auto stmt = conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    if (!rset.next()) {
      throw exception::Exception(std::string("Result set is unexpectedly empty"));
    }

    return rset.columnUint64("ARCHIVE_FILE_ID");
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// getNextLogicalLibraryId
//------------------------------------------------------------------------------
uint64_t OracleCatalogue::getNextLogicalLibraryId(rdbms::Conn &conn) {
  try {
    const char *const sql =
      "SELECT "
        "LOGICAL_LIBRARY_ID_SEQ.NEXTVAL AS LOGICAL_LIBRARY_ID "
      "FROM "
        "DUAL";
    auto stmt = conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    if (!rset.next()) {
      throw exception::Exception(std::string("Result set is unexpectedly empty"));
    }

    return rset.columnUint64("LOGICAL_LIBRARY_ID");
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// getNextVirtualOrganizationId
//------------------------------------------------------------------------------
uint64_t OracleCatalogue::getNextVirtualOrganizationId(rdbms::Conn &conn) {
  try {
    const char *const sql =
      "SELECT "
        "VIRTUAL_ORGANIZATION_ID_SEQ.NEXTVAL AS VIRTUAL_ORGANIZATION_ID "
      "FROM "
        "DUAL";
    auto stmt = conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    if (!rset.next()) {
      throw exception::Exception(std::string("Result set is unexpectedly empty"));
    }

    return rset.columnUint64("VIRTUAL_ORGANIZATION_ID");
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// getNextStorageClassId
//------------------------------------------------------------------------------
uint64_t OracleCatalogue::getNextStorageClassId(rdbms::Conn &conn) {
  try {
    const char *const sql =
      "SELECT "
        "STORAGE_CLASS_ID_SEQ.NEXTVAL AS STORAGE_CLASS_ID "
      "FROM "
        "DUAL";
    auto stmt = conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    if (!rset.next()) {
      throw exception::Exception(std::string("Result set is unexpectedly empty"));
    }

    return rset.columnUint64("STORAGE_CLASS_ID");
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// getNextTapePoolId
//------------------------------------------------------------------------------
uint64_t OracleCatalogue::getNextTapePoolId(rdbms::Conn &conn) {
  try {
    const char *const sql =
      "SELECT "
        "TAPE_POOL_ID_SEQ.NEXTVAL AS TAPE_POOL_ID "
      "FROM "
        "DUAL";
    auto stmt = conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    if (!rset.next()) {
      throw exception::Exception(std::string("Result set is unexpectedly empty"));
    }

    return rset.columnUint64("TAPE_POOL_ID");
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// selectTapeForUpdateAndGetLastFSeq
//------------------------------------------------------------------------------
uint64_t OracleCatalogue::selectTapeForUpdateAndGetLastFSeq(rdbms::Conn &conn,
  const std::string &vid) {
  try {
    const char *const sql =
      "SELECT "
        "LAST_FSEQ AS LAST_FSEQ "
      "FROM "
        "TAPE "
      "WHERE "
        "VID = :VID "
      "FOR UPDATE";
    auto stmt = conn.createStmt(sql);
    stmt.bindString(":VID", vid);
    auto rset = stmt.executeQuery();
    if (!rset.next()) {
      throw exception::Exception(std::string("The tape with VID " + vid + " does not exist"));
    }

    return rset.columnUint64("LAST_FSEQ");
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// filesWrittenToTape
//------------------------------------------------------------------------------
void OracleCatalogue::filesWrittenToTape(const std::set<TapeItemWrittenPointer> &events) {
  try {
    if (events.empty()) {
      return;
    }

    auto firstEventItor = events.begin();
    const auto &firstEvent = **firstEventItor;
    checkTapeItemWrittenFieldsAreSet(__FUNCTION__, firstEvent);
    const time_t now = time(nullptr);
    threading::MutexLocker locker(m_mutex);
    auto conn = m_connPool.getConn();
    rdbms::AutoRollback autoRollback(conn);

    conn.setAutocommitMode(rdbms::AutocommitMode::AUTOCOMMIT_OFF);

    const uint64_t lastFSeq = selectTapeForUpdateAndGetLastFSeq(conn, firstEvent.vid);
    uint64_t expectedFSeq = lastFSeq + 1;
    uint64_t totalLogicalBytesWritten = 0;

    uint32_t i = 0;
    // We have a mix of files and items. Only files will be recorded, but items
    // allow checking fSeq coherency.
    // determine the number of files
    size_t filesCount=std::count_if(events.cbegin(), events.cend(), 
        [](const TapeItemWrittenPointer &e) -> bool {return typeid(*e)==typeid(TapeFileWritten);});
    TapeFileBatch tapeFileBatch(filesCount);
    
    std::set<TapeFileWritten> fileEvents;

    for (const auto &eventP: events) {
      // Check for all item types.
      const auto &event = *eventP;
      checkTapeItemWrittenFieldsAreSet(__FUNCTION__, event);

      if (event.vid != firstEvent.vid) {
        throw exception::Exception(std::string("VID mismatch: expected=") + firstEvent.vid + " actual=" + event.vid);
      }
      
      if (expectedFSeq != event.fSeq) {
        exception::TapeFseqMismatch ex;
        ex.getMessage() << "FSeq mismatch for tape " << firstEvent.vid << ": expected=" << expectedFSeq << " actual=" <<
          event.fSeq;
        throw ex;
      }
      expectedFSeq++;
      
      try {
        // If this is a file (as opposed to a placeholder), do the full processing.
        const auto &fileEvent=dynamic_cast<const TapeFileWritten &>(event);

        checkTapeFileWrittenFieldsAreSet(__FUNCTION__, fileEvent);
        
        totalLogicalBytesWritten += fileEvent.size;

        // Store the length of each field and implicitly calculate the maximum field
        // length of each column 
        tapeFileBatch.vid.setFieldLenToValueLen(i, fileEvent.vid);
        tapeFileBatch.fSeq.setFieldLenToValueLen(i, fileEvent.fSeq);
        tapeFileBatch.blockId.setFieldLenToValueLen(i, fileEvent.blockId);
        tapeFileBatch.fileSize.setFieldLenToValueLen(i, fileEvent.size);
        tapeFileBatch.copyNb.setFieldLenToValueLen(i, fileEvent.copyNb);
        tapeFileBatch.creationTime.setFieldLenToValueLen(i, now);
        tapeFileBatch.archiveFileId.setFieldLenToValueLen(i, fileEvent.archiveFileId);
        
        fileEvents.insert(fileEvent);
        i++;
      } catch (std::bad_cast&) {}
    }

    // Store the value of each field
    i = 0;
    for (const auto &event: fileEvents) {
      tapeFileBatch.vid.setFieldValue(i, event.vid);
      tapeFileBatch.fSeq.setFieldValue(i, event.fSeq);
      tapeFileBatch.blockId.setFieldValue(i, event.blockId);
      tapeFileBatch.fileSize.setFieldValue(i, event.size);
      tapeFileBatch.copyNb.setFieldValue(i, event.copyNb);
      tapeFileBatch.creationTime.setFieldValue(i, now);
      tapeFileBatch.archiveFileId.setFieldValue(i, event.archiveFileId);
      i++;
    }

    // Update the tape because all the necessary information is now available
    auto lastEventItor = events.cend();
    lastEventItor--;
    const TapeItemWritten &lastEvent = **lastEventItor;
    updateTape(conn, lastEvent.vid, lastEvent.fSeq, totalLogicalBytesWritten, lastEvent.tapeDrive);

    // If we had only placeholders and no file recorded, we are done (but we still commit the update of the tape's fSeq).
    if (fileEvents.empty()) {
      conn.commit();
      return;
    }
    
    // Create the archive file entries, skipping those that already exist
    idempotentBatchInsertArchiveFiles(conn, fileEvents);

    {
      const char *const sql =
        "INSERT INTO TEMP_TAPE_FILE_INSERTION_BATCH(" "\n"
          "VID,"                                      "\n"
          "FSEQ,"                                     "\n"
          "BLOCK_ID,"                                 "\n"
          "LOGICAL_SIZE_IN_BYTES,"                    "\n"
          "COPY_NB,"                                  "\n"
          "CREATION_TIME,"                            "\n"
          "ARCHIVE_FILE_ID)"                          "\n"
        "VALUES("                                     "\n"
          ":VID,"                                     "\n"
          ":FSEQ,"                                    "\n"
          ":BLOCK_ID,"                                "\n"
          ":LOGICAL_SIZE_IN_BYTES,"                   "\n"
          ":COPY_NB,"                                 "\n"
          ":CREATION_TIME,"                           "\n"
          ":ARCHIVE_FILE_ID)"                         "\n";
      auto stmt = conn.createStmt(sql);
      rdbms::wrapper::OcciStmt &occiStmt = dynamic_cast<rdbms::wrapper::OcciStmt &>(stmt.getStmt());
      occiStmt.setColumn(tapeFileBatch.vid);
      occiStmt.setColumn(tapeFileBatch.fSeq);
      occiStmt.setColumn(tapeFileBatch.blockId);
      occiStmt.setColumn(tapeFileBatch.fileSize);
      occiStmt.setColumn(tapeFileBatch.copyNb);
      occiStmt.setColumn(tapeFileBatch.creationTime);
      occiStmt.setColumn(tapeFileBatch.archiveFileId);
      try {
        occiStmt->executeArrayUpdate(tapeFileBatch.nbRows);
      } catch(oracle::occi::SQLException &ex) {
        std::ostringstream msg;
        msg << std::string(__FUNCTION__) << " failed for SQL statement " << rdbms::getSqlForException(sql) << ": " <<
          ex.what();

        if(rdbms::wrapper::OcciStmt::connShouldBeClosed(ex)) {
          // Close the statement first and then the connection
          try {
            occiStmt.close();
          } catch(...) {
          }

          try {
            conn.closeUnderlyingStmtsAndConn();
          } catch(...) {
          }
          throw exception::LostDatabaseConnection(msg.str());
        }
        throw exception::Exception(msg.str());
      } catch(std::exception &se) {
        std::ostringstream msg;
        msg << std::string(__FUNCTION__) << " failed for SQL statement " << rdbms::getSqlForException(sql) << ": " <<
          se.what();

        throw exception::Exception(msg.str());
      }
    }

    // Verify that the archive file entries in the catalogue database agree with
    // the tape file written events
    const auto fileSizesAndChecksums = selectArchiveFileSizesAndChecksums(conn, fileEvents);
    for (const auto &event: fileEvents) {
      const auto fileSizeAndChecksumItor = fileSizesAndChecksums.find(event.archiveFileId);

      std::ostringstream fileContext;
      fileContext << "archiveFileId=" << event.archiveFileId << ", diskInstanceName=" << event.diskInstance <<
        ", diskFileId=" << event.diskFileId << ", diskFilePath=" << event.diskFilePath;

      // This should never happen
      if(fileSizesAndChecksums.end() == fileSizeAndChecksumItor) {
        exception::Exception ex;
        ex.getMessage() << __FUNCTION__ << ": Failed to find archive file entry in the catalogue: " << fileContext.str();
        throw ex;
      }

      const auto &fileSizeAndChecksum = fileSizeAndChecksumItor->second;

      if(fileSizeAndChecksum.fileSize != event.size) {
        catalogue::FileSizeMismatch ex;
        ex.getMessage() << __FUNCTION__ << ": File size mismatch: expected=" << fileSizeAndChecksum.fileSize <<
          ", actual=" << event.size << ": " << fileContext.str();
        throw ex;
      }

      fileSizeAndChecksum.checksumBlob.validate(event.checksumBlob);
    }

    {
      const char *const sql =
        "INSERT INTO TAPE_FILE (VID, FSEQ, BLOCK_ID, LOGICAL_SIZE_IN_BYTES,"              "\n"
           "COPY_NB, CREATION_TIME, ARCHIVE_FILE_ID)"                                     "\n"
        "SELECT VID, FSEQ, BLOCK_ID, LOGICAL_SIZE_IN_BYTES,"                              "\n"
           "COPY_NB, CREATION_TIME, ARCHIVE_FILE_ID FROM TEMP_TAPE_FILE_INSERTION_BATCH";
      auto stmt = conn.createStmt(sql);
      stmt.executeNonQuery();
    }

    {
      const char *const sql =
        "MERGE INTO"                                                                      "\n"
          "TAPE_FILE"                                                                     "\n"
        "USING("                                                                          "\n"
          "SELECT"                                                                        "\n"
            "ARCHIVE_FILE_ID,"                                                            "\n"
            "COPY_NB,"                                                                    "\n"
            "VID,"                                                                        "\n"
            // Using MAX(FSEQ) to cover the same tape copy being written more than
            // once.  The last one written supersedes the previous ones.
            "MAX(FSEQ) AS MAX_FSEQ"                                                       "\n"
          "FROM"                                                                          "\n"
            "TEMP_TAPE_FILE_INSERTION_BATCH"                                              "\n"
          "GROUP BY"                                                                      "\n"
            "ARCHIVE_FILE_ID, COPY_NB, VID"                                               "\n"
          ") TEMP"                                                                        "\n"
        "ON("                                                                             "\n"
          "TAPE_FILE.ARCHIVE_FILE_ID = TEMP.ARCHIVE_FILE_ID AND"                          "\n"
          "TAPE_FILE.COPY_NB = TEMP.COPY_NB)"                                             "\n"
        "WHEN MATCHED THEN"                                                               "\n"
          "UPDATE SET"                                                                    "\n"
            "TAPE_FILE.SUPERSEDED_BY_VID = TEMP.VID,"                                     "\n"
            "TAPE_FILE.SUPERSEDED_BY_FSEQ = TEMP.MAX_FSEQ"                                "\n"
          "WHERE"                                                                         "\n"
            "NOT(TAPE_FILE.VID = TEMP.VID AND TAPE_FILE.FSEQ = TEMP.MAX_FSEQ)";
      conn.setAutocommitMode(rdbms::AutocommitMode::AUTOCOMMIT_ON);
      auto stmt = conn.createStmt(sql);
      stmt.executeNonQuery();
    }
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// idempotentBatchInsertArchiveFiles
//------------------------------------------------------------------------------
void OracleCatalogue::idempotentBatchInsertArchiveFiles(rdbms::Conn &conn, const std::set<TapeFileWritten> &events) {
  try {
    ArchiveFileBatch archiveFileBatch(events.size());
    const time_t now = time(nullptr);
    std::vector<uint32_t> adler32(events.size());

    // Store the length of each field and implicitly calculate the maximum field length of each column 
    uint32_t i = 0;
    for (const auto &event: events) {
      // Keep transition ADLER32 checksum column up-to-date with the ChecksumBlob
      try {
        std::string adler32hex = checksum::ChecksumBlob::ByteArrayToHex(event.checksumBlob.at(checksum::ADLER32));
        adler32[i] = strtoul(adler32hex.c_str(), 0, 16);
      } catch(exception::ChecksumTypeMismatch &ex) {
        // No ADLER32 checksum exists in the checksumBlob
        adler32[i] = 0;
      }

      archiveFileBatch.archiveFileId.setFieldLenToValueLen(i, event.archiveFileId);
      archiveFileBatch.diskInstance.setFieldLenToValueLen(i, event.diskInstance);
      archiveFileBatch.diskFileId.setFieldLenToValueLen(i, event.diskFileId);
      archiveFileBatch.diskFilePath.setFieldLenToValueLen(i, event.diskFilePath);
      archiveFileBatch.diskFileUser.setFieldLenToValueLen(i, event.diskFileOwnerUid);
      archiveFileBatch.diskFileGroup.setFieldLenToValueLen(i, event.diskFileGid);
      archiveFileBatch.size.setFieldLenToValueLen(i, event.size);
      archiveFileBatch.checksumBlob.setFieldLen(i, 2 + event.checksumBlob.length());
      archiveFileBatch.checksumAdler32.setFieldLenToValueLen(i, adler32[i]);
      archiveFileBatch.storageClassName.setFieldLenToValueLen(i, event.storageClassName);
      archiveFileBatch.creationTime.setFieldLenToValueLen(i, now);
      archiveFileBatch.reconciliationTime.setFieldLenToValueLen(i, now);
      i++;
    }

    // Store the value of each field
    i = 0;
    for (const auto &event: events) {
      archiveFileBatch.archiveFileId.setFieldValue(i, event.archiveFileId);
      archiveFileBatch.diskInstance.setFieldValue(i, event.diskInstance);
      archiveFileBatch.diskFileId.setFieldValue(i, event.diskFileId);
      archiveFileBatch.diskFilePath.setFieldValue(i, event.diskFilePath);
      archiveFileBatch.diskFileUser.setFieldValue(i, event.diskFileOwnerUid);
      archiveFileBatch.diskFileGroup.setFieldValue(i, event.diskFileGid);
      archiveFileBatch.size.setFieldValue(i, event.size);
      archiveFileBatch.checksumBlob.setFieldValueToRaw(i, event.checksumBlob.serialize());
      archiveFileBatch.checksumAdler32.setFieldValue(i, adler32[i]);
      archiveFileBatch.storageClassName.setFieldValue(i, event.storageClassName);
      archiveFileBatch.creationTime.setFieldValue(i, now);
      archiveFileBatch.reconciliationTime.setFieldValue(i, now);
      i++;
    }

    const char *const sql =
      "INSERT INTO ARCHIVE_FILE("
        "ARCHIVE_FILE_ID,"
        "DISK_INSTANCE_NAME,"
        "DISK_FILE_ID,"
        "DISK_FILE_PATH,"
        "DISK_FILE_UID,"
        "DISK_FILE_GID,"
        "SIZE_IN_BYTES,"
        "CHECKSUM_BLOB,"
        "CHECKSUM_ADLER32,"
        "STORAGE_CLASS_ID,"
        "CREATION_TIME,"
        "RECONCILIATION_TIME)"
      "SELECT "
        ":ARCHIVE_FILE_ID,"
        ":DISK_INSTANCE_NAME,"
        ":DISK_FILE_ID,"
        ":DISK_FILE_PATH,"
        ":DISK_FILE_UID,"
        ":DISK_FILE_GID,"
        ":SIZE_IN_BYTES,"
        ":CHECKSUM_BLOB,"
        ":CHECKSUM_ADLER32,"
        "STORAGE_CLASS_ID,"
        ":CREATION_TIME,"
        ":RECONCILIATION_TIME "
      "FROM "
        "STORAGE_CLASS "
      "WHERE "
        "STORAGE_CLASS_NAME = :STORAGE_CLASS_NAME";
    auto stmt = conn.createStmt(sql);
    rdbms::wrapper::OcciStmt &occiStmt = dynamic_cast<rdbms::wrapper::OcciStmt &>(stmt.getStmt());
    occiStmt->setBatchErrorMode(true);

    occiStmt.setColumn(archiveFileBatch.archiveFileId);
    occiStmt.setColumn(archiveFileBatch.diskInstance);
    occiStmt.setColumn(archiveFileBatch.diskFileId);
    occiStmt.setColumn(archiveFileBatch.diskFilePath);
    occiStmt.setColumn(archiveFileBatch.diskFileUser);
    occiStmt.setColumn(archiveFileBatch.diskFileGroup);
    occiStmt.setColumn(archiveFileBatch.size);
    occiStmt.setColumn(archiveFileBatch.checksumBlob, oracle::occi::OCCI_SQLT_VBI);
    occiStmt.setColumn(archiveFileBatch.checksumAdler32);
    occiStmt.setColumn(archiveFileBatch.storageClassName);
    occiStmt.setColumn(archiveFileBatch.creationTime);
    occiStmt.setColumn(archiveFileBatch.reconciliationTime);

    try {
      occiStmt->executeArrayUpdate(archiveFileBatch.nbRows);
    } catch(oracle::occi::BatchSQLException &be) {
      const unsigned int nbFailedRows = be.getFailedRowCount();
      exception::Exception ex;
      ex.getMessage() << "Caught a BatchSQLException" << nbFailedRows;
      bool foundErrorOtherThanUniqueConstraint = false;
      for (unsigned int row = 0; row < nbFailedRows; row++ ) {
        oracle::occi::SQLException err = be.getException(row);
        const unsigned int rowIndex = be.getRowNum(row);
        const int errorCode = err.getErrorCode();

        // If the error is anything other than a unique constraint error
        if(1 != errorCode) {
          foundErrorOtherThanUniqueConstraint = true;
          ex.getMessage() << ": Row " << rowIndex << " generated ORA error " << errorCode;
        }
      }
      if (foundErrorOtherThanUniqueConstraint) {
        throw ex;
      }
    } catch(oracle::occi::SQLException &ex) {
      std::ostringstream msg;
      msg << std::string(__FUNCTION__) << " failed for SQL statement " << rdbms::getSqlForException(sql) << ": " <<
        ex.what();

      if(rdbms::wrapper::OcciStmt::connShouldBeClosed(ex)) {
        // Close the statement first and then the connection
        try {
          occiStmt.close();
        } catch(...) {
        }

        try {
          conn.closeUnderlyingStmtsAndConn();
        } catch(...) {
        }
        throw exception::LostDatabaseConnection(msg.str());
      }
      throw exception::Exception(msg.str());
    } catch(std::exception &se) {
      std::ostringstream msg;
      msg << std::string(__FUNCTION__) << " failed for SQL statement " << rdbms::getSqlForException(sql) << ": " <<
        se.what();

      throw exception::Exception(msg.str());
    }
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// selectArchiveFileSizeAndChecksum
//------------------------------------------------------------------------------
std::map<uint64_t, OracleCatalogue::FileSizeAndChecksum> OracleCatalogue::selectArchiveFileSizesAndChecksums(
  rdbms::Conn &conn, const std::set<TapeFileWritten> &events) {
  try {
    std::vector<oracle::occi::Number> archiveFileIdList(events.size());
    for (const auto &event: events) {
      archiveFileIdList.push_back(oracle::occi::Number(event.archiveFileId));
    }

    const char *const sql =
      "SELECT "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID AS ARCHIVE_FILE_ID,"
        "ARCHIVE_FILE.SIZE_IN_BYTES AS SIZE_IN_BYTES,"
        "ARCHIVE_FILE.CHECKSUM_BLOB AS CHECKSUM_BLOB,"
        "ARCHIVE_FILE.CHECKSUM_ADLER32 AS CHECKSUM_ADLER32 "
      "FROM "
        "ARCHIVE_FILE "
      "INNER JOIN TEMP_TAPE_FILE_INSERTION_BATCH ON "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID = TEMP_TAPE_FILE_INSERTION_BATCH.ARCHIVE_FILE_ID";
    auto stmt = conn.createStmt(sql);
    auto rset = stmt.executeQuery();

    std::map<uint64_t, FileSizeAndChecksum> fileSizesAndChecksums;
    while (rset.next()) {
      const uint64_t archiveFileId = rset.columnUint64("ARCHIVE_FILE_ID");

      if (fileSizesAndChecksums.end() != fileSizesAndChecksums.find(archiveFileId)) {
        exception::Exception ex;
        ex.getMessage() << __FUNCTION__ << " failed: "
          "Found duplicate archive file identifier in batch of files written to tape: archiveFileId=" << archiveFileId;
        throw ex;
      }
      FileSizeAndChecksum fileSizeAndChecksum;
      fileSizeAndChecksum.fileSize = rset.columnUint64("SIZE_IN_BYTES");
      fileSizeAndChecksum.checksumBlob.deserializeOrSetAdler32(rset.columnBlob("CHECKSUM_BLOB"), rset.columnUint64("CHECKSUM_ADLER32"));
      fileSizesAndChecksums[archiveFileId] = fileSizeAndChecksum;
    }

    return fileSizesAndChecksums;
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// deleteArchiveFile
//------------------------------------------------------------------------------
void OracleCatalogue::deleteArchiveFile(const std::string &diskInstanceName, const uint64_t archiveFileId,
  log::LogContext &lc) {
  try {
    const char *selectSql =
      "SELECT "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID AS ARCHIVE_FILE_ID,"
        "ARCHIVE_FILE.DISK_INSTANCE_NAME AS DISK_INSTANCE_NAME,"
        "ARCHIVE_FILE.DISK_FILE_ID AS DISK_FILE_ID,"
        "ARCHIVE_FILE.DISK_FILE_PATH AS DISK_FILE_PATH,"
        "ARCHIVE_FILE.DISK_FILE_UID AS DISK_FILE_UID,"
        "ARCHIVE_FILE.DISK_FILE_GID AS DISK_FILE_GID,"
        "ARCHIVE_FILE.SIZE_IN_BYTES AS SIZE_IN_BYTES,"
        "ARCHIVE_FILE.CHECKSUM_BLOB AS CHECKSUM_BLOB,"
        "ARCHIVE_FILE.CHECKSUM_ADLER32 AS CHECKSUM_ADLER32,"
        "STORAGE_CLASS.STORAGE_CLASS_NAME AS STORAGE_CLASS_NAME,"
        "ARCHIVE_FILE.CREATION_TIME AS ARCHIVE_FILE_CREATION_TIME,"
        "ARCHIVE_FILE.RECONCILIATION_TIME AS RECONCILIATION_TIME,"
        "TAPE_FILE.VID AS VID,"
        "TAPE_FILE.FSEQ AS FSEQ,"
        "TAPE_FILE.BLOCK_ID AS BLOCK_ID,"
        "TAPE_FILE.LOGICAL_SIZE_IN_BYTES AS LOGICAL_SIZE_IN_BYTES,"
        "TAPE_FILE.COPY_NB AS COPY_NB,"
        "TAPE_FILE.CREATION_TIME AS TAPE_FILE_CREATION_TIME,"
        "TAPE_FILE.SUPERSEDED_BY_VID AS SSBY_VID,"
        "TAPE_FILE.SUPERSEDED_BY_FSEQ AS SSBY_FSEQ "
      "FROM "
        "ARCHIVE_FILE "
      "INNER JOIN STORAGE_CLASS ON "
        "ARCHIVE_FILE.STORAGE_CLASS_ID = STORAGE_CLASS.STORAGE_CLASS_ID "
      "LEFT OUTER JOIN TAPE_FILE ON "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID = TAPE_FILE.ARCHIVE_FILE_ID "
      "WHERE "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID = :ARCHIVE_FILE_ID "
      "FOR UPDATE";
    utils::Timer t;

    auto conn = m_connPool.getConn();
    rdbms::AutoRollback autoRollback(conn);

    conn.setAutocommitMode(rdbms::AutocommitMode::AUTOCOMMIT_OFF);

    const auto getConnTime = t.secs(utils::Timer::resetCounter);
    auto selectStmt = conn.createStmt(selectSql);
    const auto createStmtTime = t.secs();
    selectStmt.bindUint64(":ARCHIVE_FILE_ID", archiveFileId);
    t.reset();
    rdbms::Rset selectRset = selectStmt.executeQuery();
    const auto selectFromArchiveFileTime = t.secs();
    std::unique_ptr<common::dataStructures::ArchiveFile> archiveFile;
    std::set<std::string> vidsToSetDirty;
    while(selectRset.next()) {
      if(nullptr == archiveFile.get()) {
        archiveFile = cta::make_unique<common::dataStructures::ArchiveFile>();

        archiveFile->archiveFileID = selectRset.columnUint64("ARCHIVE_FILE_ID");
        archiveFile->diskInstance = selectRset.columnString("DISK_INSTANCE_NAME");
        archiveFile->diskFileId = selectRset.columnString("DISK_FILE_ID");
        archiveFile->diskFileInfo.path = selectRset.columnString("DISK_FILE_PATH");
        archiveFile->diskFileInfo.owner_uid = selectRset.columnUint64("DISK_FILE_UID");
        archiveFile->diskFileInfo.gid = selectRset.columnUint64("DISK_FILE_GID");
        archiveFile->fileSize = selectRset.columnUint64("SIZE_IN_BYTES");
        archiveFile->checksumBlob.deserializeOrSetAdler32(selectRset.columnBlob("CHECKSUM_BLOB"), selectRset.columnUint64("CHECKSUM_ADLER32"));
        archiveFile->storageClass = selectRset.columnString("STORAGE_CLASS_NAME");
        archiveFile->creationTime = selectRset.columnUint64("ARCHIVE_FILE_CREATION_TIME");
        archiveFile->reconciliationTime = selectRset.columnUint64("RECONCILIATION_TIME");
      }

      // If there is a tape file
      if(!selectRset.columnIsNull("VID")) {
        // Add the tape file to the archive file's in-memory structure
        common::dataStructures::TapeFile tapeFile;
        tapeFile.vid = selectRset.columnString("VID");
        vidsToSetDirty.insert(tapeFile.vid);
        tapeFile.fSeq = selectRset.columnUint64("FSEQ");
        tapeFile.blockId = selectRset.columnUint64("BLOCK_ID");
        tapeFile.fileSize = selectRset.columnUint64("LOGICAL_SIZE_IN_BYTES");
        tapeFile.copyNb = selectRset.columnUint8("COPY_NB");
        tapeFile.creationTime = selectRset.columnUint64("TAPE_FILE_CREATION_TIME");
        tapeFile.checksumBlob = archiveFile->checksumBlob; // Duplicated for convenience
        if (!selectRset.columnIsNull("SSBY_VID")) {
          tapeFile.supersededByVid = selectRset.columnString("SSBY_VID");
          tapeFile.supersededByFSeq = selectRset.columnUint64("SSBY_FSEQ");
        }
        
        archiveFile->tapeFiles.push_back(tapeFile);
      }
    }

    if(nullptr == archiveFile.get()) {
      log::ScopedParamContainer spc(lc);
      spc.add("fileId", archiveFileId);
      lc.log(log::WARNING, "Ignoring request to delete archive file because it does not exist in the catalogue");
      return;
    }

    if(diskInstanceName != archiveFile->diskInstance) {
      log::ScopedParamContainer spc(lc);
      spc.add("fileId", std::to_string(archiveFile->archiveFileID))
         .add("diskInstance", archiveFile->diskInstance)
         .add("requestDiskInstance", diskInstanceName)
         .add("diskFileId", archiveFile->diskFileId)
         .add("diskFileInfo.path", archiveFile->diskFileInfo.path)
         .add("diskFileInfo.owner_uid", archiveFile->diskFileInfo.owner_uid)
         .add("diskFileInfo.gid", archiveFile->diskFileInfo.gid)
         .add("fileSize", std::to_string(archiveFile->fileSize))
         .add("checksumBlob", archiveFile->checksumBlob)
         .add("creationTime", std::to_string(archiveFile->creationTime))
         .add("reconciliationTime", std::to_string(archiveFile->reconciliationTime))
         .add("storageClass", archiveFile->storageClass)
         .add("getConnTime", getConnTime)
         .add("createStmtTime", createStmtTime)
         .add("selectFromArchiveFileTime", selectFromArchiveFileTime);
      for(auto it=archiveFile->tapeFiles.begin(); it!=archiveFile->tapeFiles.end(); it++) {
        std::stringstream tapeCopyLogStream;
        tapeCopyLogStream << "copy number: " << it->copyNb
          << " vid: " << it->vid
          << " fSeq: " << it->fSeq
          << " blockId: " << it->blockId
          << " creationTime: " << it->creationTime
          << " fileSize: " << it->fileSize
          << " checksumBlob: " << it->checksumBlob //this shouldn't be here: repeated field
          << " copyNb: " << it->copyNb //this shouldn't be here: repeated field
          << " supersededByVid: " << it->supersededByVid
          << " supersededByFSeq: " << it->supersededByFSeq;
        spc.add("TAPE FILE", tapeCopyLogStream.str());
      }
      lc.log(log::WARNING, "Failed to delete archive file because the disk instance of the request does not match that "
        "of the archived file");

      exception::UserError ue;
      ue.getMessage() << "Failed to delete archive file with ID " << archiveFileId << " because the disk instance of "
        "the request does not match that of the archived file: archiveFileId=" << archiveFileId << " path=" <<
        archiveFile->diskFileInfo.path << " requestDiskInstance=" << diskInstanceName << " archiveFileDiskInstance=" <<
        archiveFile->diskInstance;
      throw ue;
    }

    t.reset();
    {
      const char *const sql = "DELETE FROM TAPE_FILE WHERE ARCHIVE_FILE_ID = :ARCHIVE_FILE_ID";
      auto stmt = conn.createStmt(sql);
      stmt.bindUint64(":ARCHIVE_FILE_ID", archiveFileId);
      stmt.executeNonQuery();
    }
    
    //Set the tapes where the files have been deleted to dirty
    for(auto &vidToSetDirty: vidsToSetDirty){
      setTapeDirty(conn,vidToSetDirty);
    }
    
    const auto deleteFromTapeFileTime = t.secs(utils::Timer::resetCounter);

    {
      const char *const sql = "DELETE FROM ARCHIVE_FILE WHERE ARCHIVE_FILE_ID = :ARCHIVE_FILE_ID";
      auto stmt = conn.createStmt(sql);
      stmt.bindUint64(":ARCHIVE_FILE_ID", archiveFileId);
      stmt.executeNonQuery();
    }
    const auto deleteFromArchiveFileTime = t.secs(utils::Timer::resetCounter);

    conn.commit();
    const auto commitTime = t.secs();

    log::ScopedParamContainer spc(lc);
    spc.add("fileId", std::to_string(archiveFile->archiveFileID))
       .add("diskInstance", archiveFile->diskInstance)
       .add("diskFileId", archiveFile->diskFileId)
       .add("diskFileInfo.path", archiveFile->diskFileInfo.path)
       .add("diskFileInfo.owner_uid", archiveFile->diskFileInfo.owner_uid)
       .add("diskFileInfo.gid", archiveFile->diskFileInfo.gid)
       .add("fileSize", std::to_string(archiveFile->fileSize))
       .add("checksumBlob", archiveFile->checksumBlob)
       .add("creationTime", std::to_string(archiveFile->creationTime))
       .add("reconciliationTime", std::to_string(archiveFile->reconciliationTime))
       .add("storageClass", archiveFile->storageClass)
       .add("getConnTime", getConnTime)
       .add("createStmtTime", createStmtTime)
       .add("selectFromArchiveFileTime", selectFromArchiveFileTime)
       .add("deleteFromTapeFileTime", deleteFromTapeFileTime)
       .add("deleteFromArchiveFileTime", deleteFromArchiveFileTime)
       .add("commitTime", commitTime);
    for(auto it=archiveFile->tapeFiles.begin(); it!=archiveFile->tapeFiles.end(); it++) {
      std::stringstream tapeCopyLogStream;
      tapeCopyLogStream << "copy number: " << it->copyNb
        << " vid: " << it->vid
        << " fSeq: " << it->fSeq
        << " blockId: " << it->blockId
        << " creationTime: " << it->creationTime
        << " fileSize: " << it->fileSize
        << " checksumBlob: " << it->checksumBlob //this shouldn't be here: repeated field
        << " copyNb: " << it->copyNb //this shouldn't be here: repeated field
        << " supersededByVid: " << it->supersededByVid
        << " supersededByFSeq: " << it->supersededByFSeq;
      spc.add("TAPE FILE", tapeCopyLogStream.str());
    }
    lc.log(log::INFO, "Archive file deleted from CTA catalogue");
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    std::ostringstream msg;
    msg << __FUNCTION__ << ": diskInstanceName=" << diskInstanceName <<",archiveFileId=" <<
      archiveFileId << ": " << ex.getMessage().str();
    ex.getMessage().str(msg.str());
    throw;
  }
}

} // namespace catalogue
} // namespace cta

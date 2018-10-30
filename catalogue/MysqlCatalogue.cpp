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
#include "catalogue/ChecksumTypeMismatch.hpp"
#include "catalogue/ChecksumValueMismatch.hpp"
#include "catalogue/FileSizeMismatch.hpp"
#include "catalogue/MysqlCatalogueSchema.hpp"
#include "catalogue/MysqlCatalogue.hpp"
#include "common/exception/DatabaseConstraintError.hpp"
#include "common/exception/DatabasePrimaryKeyError.hpp"
#include "common/exception/Exception.hpp"
#include "common/exception/UserError.hpp"
#include "common/make_unique.hpp"
#include "common/threading/MutexLocker.hpp"
#include "common/Timer.hpp"
#include "common/utils/utils.hpp"
#include "rdbms/AutoRollback.hpp"

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
MysqlCatalogue::MysqlCatalogue(
  log::Logger &log,
  const rdbms::Login &login,
  const uint64_t nbConns,
  const uint64_t nbArchiveFileListingConns,
  const uint32_t maxTriesToConnect):
  RdbmsCatalogue(
    log,
    rdbms::Login(rdbms::Login::DBTYPE_MYSQL, 
                 login.username, login.password, login.database,
                 login.hostname, login.port),
    nbConns,
    nbArchiveFileListingConns,
    maxTriesToConnect) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
MysqlCatalogue::~MysqlCatalogue() {
}

//------------------------------------------------------------------------------
// getNextArchiveFileId
//------------------------------------------------------------------------------
uint64_t MysqlCatalogue::getNextArchiveFileId(rdbms::Conn &conn) {
  try {
    rdbms::AutoRollback autoRollback(conn);
    {
      const char *const sql =
        "UPDATE ARCHIVE_FILE_ID SET ID = LAST_INSERT_ID(ID + 1)";
      auto stmt = conn.createStmt(sql);
      stmt.executeNonQuery(rdbms::AutocommitMode::AUTOCOMMIT_OFF);
    }

    uint64_t archiveFileId = 0;
    {
      const char *const sql =
        "SELECT LAST_INSERT_ID() AS ID ";
      auto stmt = conn.createStmt(sql);
      auto rset = stmt.executeQuery(rdbms::AutocommitMode::AUTOCOMMIT_OFF);
      if(!rset.next()) {
        throw exception::Exception("ARCHIVE_FILE_ID table is empty");
      }
      archiveFileId = rset.columnUint64("ID");
      if(rset.next()) {
        throw exception::Exception("Found more than one ID counter in the ARCHIVE_FILE_ID table");
      }
    }
    conn.commit();

    return archiveFileId;
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
uint64_t MysqlCatalogue::getNextStorageClassId(rdbms::Conn &conn) {
  try {
    rdbms::AutoRollback autoRollback(conn);
    {
      const char *const sql =
        "UPDATE STORAGE_CLASS_ID SET ID = LAST_INSERT_ID(ID + 1)";
      auto stmt = conn.createStmt(sql);
      stmt.executeNonQuery(rdbms::AutocommitMode::AUTOCOMMIT_OFF);
    }

    uint64_t storageClassId = 0;
    {
      const char *const sql =
        "SELECT LAST_INSERT_ID() AS ID ";
      auto stmt = conn.createStmt(sql);
      auto rset = stmt.executeQuery(rdbms::AutocommitMode::AUTOCOMMIT_OFF);
      if(!rset.next()) {
        throw exception::Exception("STORAGE_CLASS_ID table is empty");
      }
      storageClassId = rset.columnUint64("ID");
      if(rset.next()) {
        throw exception::Exception("Found more than one ID counter in the STORAGE_CLASS_ID table");
      }
    }
    conn.commit();

    return storageClassId;
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// selectTapeForUpdate
//------------------------------------------------------------------------------
common::dataStructures::Tape MysqlCatalogue::selectTapeForUpdate(rdbms::Conn &conn, const std::string &vid) {
  try {
    const char *const sql =
      "SELECT "
      "VID AS VID,"
      "LOGICAL_LIBRARY_NAME AS LOGICAL_LIBRARY_NAME,"
      "TAPE_POOL_NAME AS TAPE_POOL_NAME,"
      "ENCRYPTION_KEY AS ENCRYPTION_KEY,"
      "CAPACITY_IN_BYTES AS CAPACITY_IN_BYTES,"
      "DATA_IN_BYTES AS DATA_IN_BYTES,"
      "LAST_FSEQ AS LAST_FSEQ,"
      "IS_DISABLED AS IS_DISABLED,"
      "IS_FULL AS IS_FULL,"
      "LBP_IS_ON AS LBP_IS_ON,"

      "LABEL_DRIVE AS LABEL_DRIVE,"
      "LABEL_TIME AS LABEL_TIME,"

      "LAST_READ_DRIVE AS LAST_READ_DRIVE,"
      "LAST_READ_TIME AS LAST_READ_TIME,"

      "LAST_WRITE_DRIVE AS LAST_WRITE_DRIVE,"
      "LAST_WRITE_TIME AS LAST_WRITE_TIME,"

      "USER_COMMENT AS USER_COMMENT,"

      "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
      "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
      "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

      "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
      "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
      "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
      "FROM "
      "TAPE "
      "WHERE "
      "VID = :VID "
      "FOR UPDATE";
    auto stmt = conn.createStmt(sql);
    stmt.bindString(":VID", vid);
    auto rset = stmt.executeQuery(rdbms::AutocommitMode::AUTOCOMMIT_OFF);
    if (!rset.next()) {
      throw exception::Exception(std::string("The tape with VID " + vid + " does not exist"));
    }

    common::dataStructures::Tape tape;

    tape.vid = rset.columnString("VID");
    tape.logicalLibraryName = rset.columnString("LOGICAL_LIBRARY_NAME");
    tape.tapePoolName = rset.columnString("TAPE_POOL_NAME");
    tape.encryptionKey = rset.columnOptionalString("ENCRYPTION_KEY");
    tape.capacityInBytes = rset.columnUint64("CAPACITY_IN_BYTES");
    tape.dataOnTapeInBytes = rset.columnUint64("DATA_IN_BYTES");
    tape.lastFSeq = rset.columnUint64("LAST_FSEQ");
    tape.disabled = rset.columnBool("IS_DISABLED");
    tape.full = rset.columnBool("IS_FULL");
    tape.lbp = rset.columnOptionalBool("LBP_IS_ON");

    tape.labelLog = getTapeLogFromRset(rset, "LABEL_DRIVE", "LABEL_TIME");
    tape.lastReadLog = getTapeLogFromRset(rset, "LAST_READ_DRIVE", "LAST_READ_TIME");
    tape.lastWriteLog = getTapeLogFromRset(rset, "LAST_WRITE_DRIVE", "LAST_WRITE_TIME");

    tape.comment = rset.columnString("USER_COMMENT");

    common::dataStructures::UserIdentity creatorUI;
    creatorUI.name = rset.columnString("CREATION_LOG_USER_NAME");

    common::dataStructures::EntryLog creationLog;
    creationLog.username = rset.columnString("CREATION_LOG_USER_NAME");
    creationLog.host = rset.columnString("CREATION_LOG_HOST_NAME");
    creationLog.time = rset.columnUint64("CREATION_LOG_TIME");

    tape.creationLog = creationLog;

    common::dataStructures::UserIdentity updaterUI;
    updaterUI.name = rset.columnString("LAST_UPDATE_USER_NAME");

    common::dataStructures::EntryLog updateLog;
    updateLog.username = rset.columnString("LAST_UPDATE_USER_NAME");
    updateLog.host = rset.columnString("LAST_UPDATE_HOST_NAME");
    updateLog.time = rset.columnUint64("LAST_UPDATE_TIME");

    tape.lastModificationLog = updateLog;

    return tape;
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
void MysqlCatalogue::filesWrittenToTape(const std::set<TapeItemWrittenPointer> &events) {
  try {
    if(events.empty()) {
      return;
    }

    auto firstEventItor = events.cbegin();
    const auto &firstEvent = **firstEventItor;;
    checkTapeItemWrittenFieldsAreSet(__FUNCTION__, firstEvent);

    auto conn = m_connPool.getConn();
    rdbms::AutoRollback autoRollback(conn);

    const auto tape = selectTapeForUpdate(conn, firstEvent.vid);
    uint64_t expectedFSeq = tape.lastFSeq + 1;
    uint64_t totalCompressedBytesWritten = 0;

    for(const auto &eventP: events) {
      const auto & event = *eventP;
      checkTapeItemWrittenFieldsAreSet(__FUNCTION__, event);

      if(event.vid != firstEvent.vid) {
        throw exception::Exception(std::string("VID mismatch: expected=") + firstEvent.vid + " actual=" + event.vid);
      }

      if(expectedFSeq != event.fSeq) {
        exception::Exception ex;
        ex.getMessage() << "FSeq mismatch for tape " << firstEvent.vid << ": expected=" << expectedFSeq << " actual=" <<
          firstEvent.fSeq;
        throw ex;
      }
      expectedFSeq++;
      
      
      try {
        // If this is a file (as opposed to a placeholder), do the full processing.
        const auto &fileEvent=dynamic_cast<const TapeFileWritten &>(event); 
        totalCompressedBytesWritten += fileEvent.compressedSize;
      } catch (std::bad_cast&) {}
    }

    auto lastEventItor = events.cend();
    lastEventItor--;
    const TapeItemWritten &lastEvent = **lastEventItor;
    updateTape(conn, rdbms::AutocommitMode::AUTOCOMMIT_ON, lastEvent.vid, lastEvent.fSeq, totalCompressedBytesWritten,
      lastEvent.tapeDrive);

    for(const auto &event : events) {
      try {
        // If this is a file (as opposed to a placeholder), do the full processing.
        const auto &fileEvent=dynamic_cast<const TapeFileWritten &>(*event); 
        fileWrittenToTape(rdbms::AutocommitMode::AUTOCOMMIT_ON, conn, fileEvent);
      } catch (std::bad_cast&) {}
    }
    conn.commit();
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

//------------------------------------------------------------------------------
// fileWrittenToTape
//------------------------------------------------------------------------------
void MysqlCatalogue::fileWrittenToTape(const rdbms::AutocommitMode autocommitMode, rdbms::Conn &conn,
  const TapeFileWritten &event) {
  try {
    checkTapeFileWrittenFieldsAreSet(__FUNCTION__, event);

    // Try to insert a row into the ARCHIVE_FILE table - it is normal this will
    // fail if another tape copy has already been written to tape
    try {
      ArchiveFileRow row;
      row.archiveFileId = event.archiveFileId;
      row.diskFileId = event.diskFileId;
      row.diskInstance = event.diskInstance;
      row.size = event.size;
      row.checksumType = event.checksumType;
      row.checksumValue = event.checksumValue;
      row.storageClassName = event.storageClassName;
      row.diskFilePath = event.diskFilePath;
      row.diskFileUser = event.diskFileUser;
      row.diskFileGroup = event.diskFileGroup;
      row.diskFileRecoveryBlob = event.diskFileRecoveryBlob;
      insertArchiveFile(conn, autocommitMode, row);
    } catch(exception::DatabasePrimaryKeyError &) {
      // Ignore this error
    } catch(...) {
      throw;
    }

    const time_t now = time(nullptr);
    const auto archiveFile = getArchiveFileByArchiveFileId(conn, event.archiveFileId);

    if(nullptr == archiveFile) {
      // This should never happen
      exception::Exception ex;
      ex.getMessage() << "Failed to find archive file: archiveFileId=" << event.archiveFileId;
      throw ex;
    }

    std::ostringstream fileContext;
    fileContext << "archiveFileId=" << event.archiveFileId << ", diskInstanceName=" << event.diskInstance <<
      ", diskFileId=" << event.diskFileId << ", diskFilePath=" << event.diskFilePath;

    if(archiveFile->fileSize != event.size) {
      catalogue::FileSizeMismatch ex;
      ex.getMessage() << "File size mismatch: expected=" << archiveFile->fileSize << ", actual=" << event.size << ": "
        << fileContext.str();
      throw ex;
    }

    if(archiveFile->checksumType != event.checksumType) {
      catalogue::ChecksumTypeMismatch ex;
      ex.getMessage() << "Checksum type mismatch: expected=" << archiveFile->checksumType << ", actual=" <<
        event.checksumType << ": " << fileContext.str();
      throw ex;
    }

    if(archiveFile->checksumValue != event.checksumValue) {
      catalogue::ChecksumValueMismatch ex;
      ex.getMessage() << "Checksum value mismatch: expected=" << archiveFile->checksumValue << ", actual=" <<
        event.checksumValue << ": " << fileContext.str();
      throw ex;
    }

    // Insert the tape file
    common::dataStructures::TapeFile tapeFile;
    tapeFile.vid            = event.vid;
    tapeFile.fSeq           = event.fSeq;
    tapeFile.blockId        = event.blockId;
    tapeFile.compressedSize = event.compressedSize;
    tapeFile.copyNb         = event.copyNb;
    tapeFile.creationTime   = now;
    insertTapeFile(conn, autocommitMode, tapeFile, event.archiveFileId);
  } catch(exception::UserError &) {
    throw;
  } catch(exception::Exception &ex) {
    ex.getMessage().str(std::string(__FUNCTION__) + ": " + ex.getMessage().str());
    throw;
  }
}

} // namespace catalogue
} // namespace cta

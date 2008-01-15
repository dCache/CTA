/******************************************************************************
 *                      OraTapeSvc.cpp
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * @(#)OraTapeSvc.cpp,v 1.20 $Release$ 2007/04/13 11:58:53 sponcec3
 *
 * Implementation of the ITapeSvc for Oracle
 *
 * @author Sebastien Ponce
 *****************************************************************************/

// Include Files
#include "castor/IAddress.hpp"
#include "castor/IObject.hpp"
#include "castor/IFactory.hpp"
#include "castor/SvcFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IClient.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/stager/Stream.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/Segment.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/DiskPool.hpp"
#include "castor/stager/SvcClass.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "castor/stager/TapePool.hpp"
#include "castor/stager/FileClass.hpp"
#include "castor/stager/DiskServer.hpp"
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/Files2Delete.hpp"
#include "castor/stager/FilesDeleted.hpp"
#include "castor/stager/FilesDeletionFailed.hpp"
#include "castor/stager/GetUpdateDone.hpp"
#include "castor/stager/GetUpdateFailed.hpp"
#include "castor/stager/PutFailed.hpp"
#include "castor/stager/GCLocalFile.hpp"
#include "castor/stager/GCFile.hpp"
#include "castor/stager/DiskCopyForRecall.hpp"
#include "castor/stager/TapeCopyForMigration.hpp"
#include "castor/db/ora/OraTapeSvc.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Busy.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/exception/NotSupported.hpp"
#include "castor/stager/TapeStatusCodes.hpp"
#include "castor/stager/TapeCopyStatusCodes.hpp"
#include "castor/stager/StreamStatusCodes.hpp"
#include "castor/stager/SegmentStatusCodes.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/DiskCopyStatusCodes.hpp"
#include "castor/BaseAddress.hpp"
#include "occi.h"
#include <Cuuid.h>
#include <string>
#include <sstream>
#include <vector>
#include <Cns_api.h>
#include <vmgr_api.h>
#include <Ctape_api.h>
#include <serrno.h>
#include <fcntl.h>

#define NS_SEGMENT_NOTOK (' ')

// -----------------------------------------------------------------------
// Instantiation of a static factory class
// -----------------------------------------------------------------------
static castor::SvcFactory<castor::db::ora::OraTapeSvc>* s_factoryOraTapeSvc =
  new castor::SvcFactory<castor::db::ora::OraTapeSvc>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for tapesToDo
const std::string castor::db::ora::OraTapeSvc::s_tapesToDoStatementString =
  "SELECT id FROM Tape WHERE status = :1";

/// SQL statement for streamsToDo
const std::string castor::db::ora::OraTapeSvc::s_streamsToDoStatementString =
  "BEGIN streamsToDo(:1); END;";

/// SQL statement for anyTapeCopyForStream
const std::string castor::db::ora::OraTapeSvc::s_anyTapeCopyForStreamStatementString =
  "BEGIN anyTapeCopyForStream(:1, :2); END;";

/// SQL statement for bestTapeCopyForStream
const std::string castor::db::ora::OraTapeSvc::s_bestTapeCopyForStreamStatementString =
  "BEGIN bestTapeCopyForStream(:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11); END;";

/// SQL statement for streamsForTapePool
const std::string castor::db::ora::OraTapeSvc::s_streamsForTapePoolStatementString =
  "SELECT id from Stream WHERE tapePool = :1 FOR UPDATE";

/// SQL statement for bestFileSystemForSegment
const std::string castor::db::ora::OraTapeSvc::s_bestFileSystemForSegmentStatementString =
  "BEGIN bestFileSystemForSegment(:1, :2, :3, :4, :5, :6); END;";

/// SQL statement for fileRecalled
const std::string castor::db::ora::OraTapeSvc::s_fileRecalledStatementString =
  "BEGIN fileRecalled(:1); END;";

/// SQL statement for fileRecallFailed
const std::string castor::db::ora::OraTapeSvc::s_fileRecallFailedStatementString =
  "BEGIN fileRecallFailed(:1); END;";

/// SQL statement for selectTapeCopiesForMigration
const std::string castor::db::ora::OraTapeSvc::s_selectTapeCopiesForMigrationStatementString =
  "BEGIN selectTapeCopiesForMigration(:1,:2); END;";

/// SQL statement for resetStream
const std::string castor::db::ora::OraTapeSvc::s_resetStreamStatementString =
  "BEGIN resetStream(:1); END;";

/// SQL statement for segmentsForTape
const std::string castor::db::ora::OraTapeSvc::s_segmentsForTapeStatementString =
  "BEGIN segmentsForTape(:1, :2); END;";

/// SQL statement for anySegmentsForTape
const std::string castor::db::ora::OraTapeSvc::s_anySegmentsForTapeStatementString =
  "BEGIN anySegmentsForTape(:1, :2); END;";

/// SQL statement for failedSegments
const std::string castor::db::ora::OraTapeSvc::s_failedSegmentsStatementString =
  "BEGIN failedSegments(:1); END;";

/// SQL statement for checkFileForRepack
const std::string castor::db::ora::OraTapeSvc::s_checkFileForRepackStatementString = 
  "BEGIN checkFileForRepack(:1, :2); END;";

/// SQL statement for getNumFilesByStream
const std::string castor::db::ora::OraTapeSvc::s_getNumFilesByStreamStatementString = 
  "BEGIN getNumFilesByStream(:1,:2); END;";


// -----------------------------------------------------------------------
// OraTapeSvc
// -----------------------------------------------------------------------
castor::db::ora::OraTapeSvc::OraTapeSvc(const std::string name) :
  BaseTapeSvc(),  
  OraCommonSvc(name), 
  m_tapesToDoStatement(0),
  m_streamsToDoStatement(0),
  m_anyTapeCopyForStreamStatement(0),
  m_bestTapeCopyForStreamStatement(0),
  m_streamsForTapePoolStatement(0),
  m_bestFileSystemForSegmentStatement(0),
  m_segmentsForTapeStatement(0),
  m_anySegmentsForTapeStatement(0),
  m_fileRecalledStatement(0),
  m_fileRecallFailedStatement(0),
  m_selectTapeCopiesForMigrationStatement(0),
  m_resetStreamStatement(0),
  m_failedSegmentsStatement(0),
  m_checkFileForRepackStatement(0),
  m_getNumFilesByStreamStatement(0){
}

// -----------------------------------------------------------------------
// ~OraTapeSvc
// -----------------------------------------------------------------------
castor::db::ora::OraTapeSvc::~OraTapeSvc() throw() {
  reset();
}

// -----------------------------------------------------------------------
// id
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeSvc::id() const {
  return ID();
}

// -----------------------------------------------------------------------
// ID
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraTapeSvc::ID() {
  return castor::SVC_ORATAPESVC;
}

//-----------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraTapeSvc::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  OraCommonSvc::reset();
  try {
    if (m_tapesToDoStatement) deleteStatement(m_tapesToDoStatement);
    if (m_streamsToDoStatement) deleteStatement(m_streamsToDoStatement);
    if (m_anyTapeCopyForStreamStatement) deleteStatement(m_anyTapeCopyForStreamStatement);
    if (m_bestTapeCopyForStreamStatement) deleteStatement(m_bestTapeCopyForStreamStatement);
    if (m_streamsForTapePoolStatement) deleteStatement(m_streamsForTapePoolStatement);
    if (m_bestFileSystemForSegmentStatement) deleteStatement(m_bestFileSystemForSegmentStatement);
    if (m_fileRecalledStatement) deleteStatement(m_fileRecalledStatement);
    if (m_fileRecallFailedStatement) deleteStatement(m_fileRecallFailedStatement);
    if (m_selectTapeCopiesForMigrationStatement) deleteStatement(m_selectTapeCopiesForMigrationStatement);
    if (m_resetStreamStatement) deleteStatement(m_resetStreamStatement);
    if (m_segmentsForTapeStatement) deleteStatement(m_segmentsForTapeStatement);
    if (m_anySegmentsForTapeStatement) deleteStatement(m_anySegmentsForTapeStatement);
    if (m_failedSegmentsStatement) deleteStatement(m_failedSegmentsStatement);
    if (m_checkFileForRepackStatement) deleteStatement(m_checkFileForRepackStatement);
    if (m_getNumFilesByStreamStatement) deleteStatement(m_getNumFilesByStreamStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_tapesToDoStatement = 0;
  m_streamsToDoStatement = 0;
  m_anyTapeCopyForStreamStatement = 0;
  m_bestTapeCopyForStreamStatement = 0;
  m_streamsForTapePoolStatement = 0;
  m_bestFileSystemForSegmentStatement = 0;
  m_fileRecalledStatement = 0;
  m_fileRecallFailedStatement = 0;
  m_selectTapeCopiesForMigrationStatement = 0;
  m_resetStreamStatement = 0;
  m_segmentsForTapeStatement = 0;
  m_anySegmentsForTapeStatement = 0;
  m_failedSegmentsStatement = 0;
  m_checkFileForRepackStatement = 0;
  m_getNumFilesByStreamStatement = 0;
}

// -----------------------------------------------------------------------
// anySegmentsForTape
// -----------------------------------------------------------------------
int castor::db::ora::OraTapeSvc::anySegmentsForTape
(castor::stager::Tape* tape)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_anySegmentsForTapeStatement) {
      m_anySegmentsForTapeStatement =
        createStatement(s_anySegmentsForTapeStatementString);
      m_anySegmentsForTapeStatement->registerOutParam
        (2, oracle::occi::OCCIINT);
      m_anySegmentsForTapeStatement->setAutoCommit(true);
    }
    // execute the statement and see whether we found something
    m_anySegmentsForTapeStatement->setDouble(1, tape->id());
    (void)m_anySegmentsForTapeStatement->executeUpdate();
    return m_anySegmentsForTapeStatement->getInt(2);
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in anySegmentsForTape."
      << std::endl << e.what();
    throw ex;
  }
}

// -----------------------------------------------------------------------
// segmentsForTape
// -----------------------------------------------------------------------
std::vector<castor::stager::Segment*>
castor::db::ora::OraTapeSvc::segmentsForTape
(castor::stager::Tape* tape)
  throw (castor::exception::Exception) {
  std::vector<castor::stager::Segment*> result;
  try {
    // Check whether the statements are ok
    if (0 == m_segmentsForTapeStatement) {
      m_segmentsForTapeStatement =
        createStatement(s_segmentsForTapeStatementString);
      m_segmentsForTapeStatement->registerOutParam
        (2, oracle::occi::OCCICURSOR);
      m_segmentsForTapeStatement->setAutoCommit(true);
    }
    // execute the statement and see whether we found something
    m_segmentsForTapeStatement->setDouble(1, tape->id());
    unsigned int nb = m_segmentsForTapeStatement->executeUpdate();
    if (0 == nb) {
      rollback();
      castor::exception::Internal ex;
      ex.getMessage()
        << "segmentsForTape : Unable to get segments.";
      throw ex;
    }
    oracle::occi::ResultSet *rs =
      m_segmentsForTapeStatement->getCursor(2);
    // Run through the cursor
    oracle::occi::ResultSet::Status status = rs->next();
    while(status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      castor::stager::Segment* item =
        new castor::stager::Segment();
      item->setFseq(rs->getInt(1));
      item->setOffset((u_signed64)rs->getDouble(2));
      item->setBytes_in((u_signed64)rs->getDouble(3));
      item->setBytes_out((u_signed64)rs->getDouble(4));
      item->setHost_bytes((u_signed64)rs->getDouble(5));
      item->setSegmCksumAlgorithm(rs->getString(6));
      item->setSegmCksum(rs->getInt(7));
      item->setErrMsgTxt(rs->getString(8));
      item->setErrorCode(rs->getInt(9));
      item->setSeverity(rs->getInt(10));
      item->setBlockId0(rs->getInt(11));
      item->setBlockId1(rs->getInt(12));
      item->setBlockId2(rs->getInt(13));
      item->setBlockId3(rs->getInt(14));
      item->setCreationTime((u_signed64)rs->getDouble(15));
      item->setId((u_signed64)rs->getDouble(16));
      item->setStatus((enum castor::stager::SegmentStatusCodes)rs->getInt(19));
      result.push_back(item);
      status = rs->next();
    }
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in segmentsForTape."
      << std::endl << e.what();
    throw ex;
  }
  return result;
}

// -----------------------------------------------------------------------
// bestFileSystemForSegment
// -----------------------------------------------------------------------
castor::stager::DiskCopyForRecall*
castor::db::ora::OraTapeSvc::bestFileSystemForSegment
(castor::stager::Segment *segment)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_bestFileSystemForSegmentStatement) {
      m_bestFileSystemForSegmentStatement =
        createStatement(s_bestFileSystemForSegmentStatementString);
      m_bestFileSystemForSegmentStatement->registerOutParam
        (2, oracle::occi::OCCISTRING, 2048);
      m_bestFileSystemForSegmentStatement->registerOutParam
        (3, oracle::occi::OCCISTRING, 2048);
      m_bestFileSystemForSegmentStatement->registerOutParam
        (4, oracle::occi::OCCISTRING, 2048);
      m_bestFileSystemForSegmentStatement->registerOutParam
        (5, oracle::occi::OCCIDOUBLE);
    }
    // execute the statement and see whether we found something
    m_bestFileSystemForSegmentStatement->setDouble(1, segment->id());
    m_bestFileSystemForSegmentStatement->setInt(6, 1);
    unsigned int nb =
      m_bestFileSystemForSegmentStatement->executeUpdate();
    if (nb == 0) {
      return 0;
    }
    // Create result
    castor::stager::DiskCopyForRecall* result =
      new castor::stager::DiskCopyForRecall();
    result->setDiskServer(m_bestFileSystemForSegmentStatement->getString(2));
    result->setMountPoint(m_bestFileSystemForSegmentStatement->getString(3));
    result->setPath(m_bestFileSystemForSegmentStatement->getString(4));
    result->setId
      ((u_signed64)m_bestFileSystemForSegmentStatement->getDouble(5));
    // Fill result for CastorFile
    castor::BaseAddress ad;
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    cnvSvc()->fillObj(&ad, result, OBJ_CastorFile);
    // commit
    cnvSvc()->commit();
    // Make rmMaster aware of the new stream that will be created
    sendStreamReport(result->diskServer(),
		     result->mountPoint(),
		     castor::monitoring::STREAMDIRECTION_WRITE,
		     true);
    // return
    return result;
  } catch (oracle::occi::SQLException e) {
    if (1403 == e.getErrorCode()) {
      // No data found error, this is ok
      return 0;
    }
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in bestFileSystemForSegment."
      << std::endl << e.what();
    throw ex;
  }
}

// -----------------------------------------------------------------------
// anyTapeCopyForStream
// -----------------------------------------------------------------------
bool castor::db::ora::OraTapeSvc::anyTapeCopyForStream
(castor::stager::Stream* searchItem)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_anyTapeCopyForStreamStatement) {
      m_anyTapeCopyForStreamStatement =
        createStatement(s_anyTapeCopyForStreamStatementString);
      m_anyTapeCopyForStreamStatement->registerOutParam
        (2, oracle::occi::OCCIINT);
    }
    m_anyTapeCopyForStreamStatement->setInt(1, searchItem->id());
    m_anyTapeCopyForStreamStatement->executeUpdate();
    bool result =
      1 == m_anyTapeCopyForStreamStatement->getInt(2);
    if (result) {
      searchItem->setStatus(castor::stager::STREAM_WAITMOUNT);
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->updateRep(&ad, searchItem, true);
    }
    return result;
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in anyTapeCopyForStream."
      << std::endl << e.what();
    throw ex;
  }
}

// -----------------------------------------------------------------------
// bestTapeCopyForStream
// -----------------------------------------------------------------------
castor::stager::TapeCopyForMigration*
castor::db::ora::OraTapeSvc::bestTapeCopyForStream
(castor::stager::Stream* searchItem)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_bestTapeCopyForStreamStatement) {
      m_bestTapeCopyForStreamStatement =
        createStatement(s_bestTapeCopyForStreamStatementString);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (2, oracle::occi::OCCISTRING, 2048);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (3, oracle::occi::OCCISTRING, 2048);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (4, oracle::occi::OCCISTRING, 2048);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (5, oracle::occi::OCCIDOUBLE);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (6, oracle::occi::OCCIDOUBLE);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (7, oracle::occi::OCCIDOUBLE);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (8, oracle::occi::OCCISTRING, 2048);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (9, oracle::occi::OCCIDOUBLE);
      m_bestTapeCopyForStreamStatement->registerOutParam
        (10, oracle::occi::OCCIDOUBLE);
      m_bestTapeCopyForStreamStatement->setAutoCommit(true);
    }
    // execute the statement and see whether we found something
    m_bestTapeCopyForStreamStatement->setDouble(1, searchItem->id());
    m_bestTapeCopyForStreamStatement->setInt(11, 1);
    unsigned int nb =
      m_bestTapeCopyForStreamStatement->executeUpdate();
    if (nb == 0) {
      rollback();
      castor::exception::NoEntry e;
      e.getMessage() << "No TapeCopy found";
      throw e;
    }
    // Create result
    castor::stager::TapeCopyForMigration* result =
      new castor::stager::TapeCopyForMigration();
    result->setDiskServer(m_bestTapeCopyForStreamStatement->getString(2));
    result->setMountPoint(m_bestTapeCopyForStreamStatement->getString(3));
    castor::stager::DiskCopy* diskCopy =
      new castor::stager::DiskCopy();
    diskCopy->setPath(m_bestTapeCopyForStreamStatement->getString(4));
    diskCopy->setId((u_signed64)m_bestTapeCopyForStreamStatement->getDouble(5));
    castor::stager::CastorFile* castorFile =
      new castor::stager::CastorFile();
    castorFile->setId
      ((u_signed64)m_bestTapeCopyForStreamStatement->getDouble(6));
    castorFile->setFileId
      ((u_signed64)m_bestTapeCopyForStreamStatement->getDouble(7));
    castorFile->setNsHost(m_bestTapeCopyForStreamStatement->getString(8));
    castorFile->setFileSize
      ((u_signed64)m_bestTapeCopyForStreamStatement->getDouble(9));
    result->setId((u_signed64)m_bestTapeCopyForStreamStatement->getDouble(10));
    diskCopy->setCastorFile(castorFile);
    castorFile->addDiskCopies(diskCopy);
    result->setCastorFile(castorFile);
    castorFile->addTapeCopies(result);
    // Fill result for TapeCopy, Segments and Tape
    cnvSvc()->updateObj(result);
    castor::BaseAddress ad;
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    cnvSvc()->fillObj(&ad, result, OBJ_Segment);
    for (std::vector<castor::stager::Segment*>::iterator it =
           result->segments().begin();
         it != result->segments().end();
         it++) {
      cnvSvc()->fillObj(&ad, *it, OBJ_Tape);
    }
    // Make rmMaster aware of the stream that will be createdis now gone
    sendStreamReport(result->diskServer(),
		     result->mountPoint(),
		     castor::monitoring::STREAMDIRECTION_READ,
		     true);
    // return
    return result;
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in bestTapeCopyForStream."
      << std::endl << e.what();
    throw ex;
  }
}

// -----------------------------------------------------------------------
// streamsForTapePool
// -----------------------------------------------------------------------
void castor::db::ora::OraTapeSvc::streamsForTapePool
(castor::stager::TapePool* tapePool)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_streamsForTapePoolStatement) {
      m_streamsForTapePoolStatement =
        createStatement(s_streamsForTapePoolStatementString);
    }
    // retrieve the object from the database
    std::set<int> streamsList;
    m_streamsForTapePoolStatement->setDouble(1, tapePool->id());
    oracle::occi::ResultSet *rset =
      m_streamsForTapePoolStatement->executeQuery();
    while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
      streamsList.insert(rset->getInt(1));
    }
    // Close ResultSet
    m_streamsForTapePoolStatement->closeResultSet(rset);
    // Update objects and mark old ones for deletion
    std::vector<castor::stager::Stream*> toBeDeleted;
    for (std::vector<castor::stager::Stream*>::iterator it =
           tapePool->streams().begin();
         it != tapePool->streams().end();
         it++) {
      std::set<int>::iterator item;
      if ((item = streamsList.find((*it)->id())) == streamsList.end()) {
        toBeDeleted.push_back(*it);
      } else {
        streamsList.erase(item);
        cnvSvc()->updateObj((*it));
      }
    }
    // Delete old objects
    for (std::vector<castor::stager::Stream*>::iterator it = toBeDeleted.begin();
         it != toBeDeleted.end();
         it++) {
      tapePool->removeStreams(*it);
      (*it)->setTapePool(0);
    }
    // Create new objects
    for (std::set<int>::iterator it = streamsList.begin();
         it != streamsList.end();
         it++) {
      IObject* item = cnvSvc()->getObjFromId(*it);
      castor::stager::Stream* remoteObj = 
        dynamic_cast<castor::stager::Stream*>(item);
      tapePool->addStreams(remoteObj);
      remoteObj->setTapePool(tapePool);
    }
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in streamsForTapePool."
        << std::endl << e.what();
      throw ex;
  }
}

// -----------------------------------------------------------------------
// fileRecalled
// -----------------------------------------------------------------------
void castor::db::ora::OraTapeSvc::fileRecalled
(castor::stager::TapeCopy* tapeCopy)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_fileRecalledStatement) {
      m_fileRecalledStatement =
        createStatement(s_fileRecalledStatementString);
      m_fileRecalledStatement->setAutoCommit(true);
    }
    // execute the statement and see whether we found something
    m_fileRecalledStatement->setDouble(1, tapeCopy->id());
    unsigned int nb =
      m_fileRecalledStatement->executeUpdate();
    if (nb == 0) {
      castor::exception::Internal ex;
      ex.getMessage()
        << "fileRecalled : unable to update SubRequest and DiskCopy status.";
      throw ex;
    }
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in fileRecalled."
      << std::endl << e.what();
    throw ex;
  }
}

// -----------------------------------------------------------------------
// fileRecallFailed
// -----------------------------------------------------------------------
void castor::db::ora::OraTapeSvc::fileRecallFailed
(castor::stager::TapeCopy* tapeCopy)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_fileRecallFailedStatement) {
      m_fileRecallFailedStatement =
        createStatement(s_fileRecallFailedStatementString);
      m_fileRecallFailedStatement->setAutoCommit(true);
    }
    // execute the statement and see whether we found something
    m_fileRecallFailedStatement->setDouble(1, tapeCopy->id());
    unsigned int nb =
      m_fileRecallFailedStatement->executeUpdate();
    if (nb == 0) {
      castor::exception::Internal ex;
      ex.getMessage()
        << "fileRecallFailed : unable to update SubRequest and DiskCopy status.";
      throw ex;
    }
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in fileRecallFailed."
      << std::endl << e.what();
    throw ex;
  }
}

// -----------------------------------------------------------------------
// tapesToDo
// -----------------------------------------------------------------------
std::vector<castor::stager::Tape*>
castor::db::ora::OraTapeSvc::tapesToDo()
  throw (castor::exception::Exception) {
  // Check whether the statements are ok
  if (0 == m_tapesToDoStatement) {
    m_tapesToDoStatement = createStatement(s_tapesToDoStatementString);
    m_tapesToDoStatement->setInt(1, castor::stager::TAPE_PENDING);
  }
  std::vector<castor::stager::Tape*> result;
  try {
    oracle::occi::ResultSet *rset = m_tapesToDoStatement->executeQuery();
    while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
      IObject* obj = cnvSvc()->getObjFromId(rset->getInt(1));
      castor::stager::Tape* tape =
        dynamic_cast<castor::stager::Tape*>(obj);
      if (0 == tape) {
        castor::exception::Internal ex;
        ex.getMessage()
          << "In method OraTapeSvc::tapesToDo, got a non tape object";
        delete obj;
        throw ex;
      }
      // Change tape status
      tape->setStatus(castor::stager::TAPE_WAITDRIVE);
      cnvSvc()->updateRep(0, tape, false);
      result.push_back(tape);
    }
    m_tapesToDoStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error in tapesToDo while retrieving list of tapes."
      << std::endl << e.what();
    throw ex;
  }
  // Commit all status changes
  cnvSvc()->commit();
  return result;
}

// -----------------------------------------------------------------------
// streamsToDo
// -----------------------------------------------------------------------
std::vector<castor::stager::Stream*>
castor::db::ora::OraTapeSvc::streamsToDo()
  throw (castor::exception::Exception) {
  std::vector<castor::stager::Stream*> result;
  try {
    // Check whether the statements are ok
    if (0 == m_streamsToDoStatement) {
      m_streamsToDoStatement = createStatement(s_streamsToDoStatementString);
      m_streamsToDoStatement->registerOutParam(1, oracle::occi::OCCICURSOR);
      m_streamsToDoStatement->setAutoCommit(true);
    }
    unsigned int nb = m_streamsToDoStatement->executeUpdate();
    if (0 == nb) {
      rollback();
      castor::exception::Internal ex;
      ex.getMessage()
	<< "streamsToDo : Unable to get streams.";
      throw ex;
    }
    oracle::occi::ResultSet *rs =
      m_streamsToDoStatement->getCursor(1);
    // Run through the cursor
    oracle::occi::ResultSet::Status status = rs->next();
    while(status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      castor::stager::Stream* stream = new castor::stager::Stream();
      stream->setId(rs->getInt(1));
      stream->setInitialSizeToTransfer((u_signed64)rs->getDouble(2));
      stream->setStatus((enum castor::stager::StreamStatusCodes)rs->getInt(3));
      castor::stager::TapePool* tapePool = new castor::stager::TapePool();
      tapePool->setId(rs->getInt(4));
      tapePool->setName(rs->getString(5));
      stream->setTapePool(tapePool);
      tapePool->addStreams(stream);
      result.push_back(stream);      
      status = rs->next();
    }
  } catch (oracle::occi::SQLException e) {
    // cleanup memory if needed
    for (std::vector<castor::stager::Stream*>::iterator it = result.begin();
	 it != result.end();
	 it++) {
      if (0 != (*it)->tapePool()) delete (*it)->tapePool();
      delete *it;
    }
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error in streamsToDo while retrieving list of streams."
      << std::endl << e.what();
    throw ex;
  }
  return result;
}

// -----------------------------------------------------------------------
// selectTapeCopiesForMigration
// -----------------------------------------------------------------------
std::vector<castor::stager::TapeCopy*>
castor::db::ora::OraTapeSvc::selectTapeCopiesForMigration
(castor::stager::SvcClass *svcClass)
  throw (castor::exception::Exception) {
  // Check whether the statements are ok
  if (0 == m_selectTapeCopiesForMigrationStatement) {
    m_selectTapeCopiesForMigrationStatement =
      createStatement(s_selectTapeCopiesForMigrationStatementString);
    m_selectTapeCopiesForMigrationStatement->registerOutParam
        (2, oracle::occi::OCCICURSOR);
    m_selectTapeCopiesForMigrationStatement->setAutoCommit(true);
  }
  // Execute statement and get result
  // The procedure takes internally a lock and performs the commit
  try {
    m_selectTapeCopiesForMigrationStatement->setDouble(1, svcClass->id());
    m_selectTapeCopiesForMigrationStatement->executeUpdate();

    oracle::occi::ResultSet *rset =
      m_selectTapeCopiesForMigrationStatement->getCursor(2);
    // create result
    std::vector<castor::stager::TapeCopy*> result;
    // Fill it as in OraTapeCopyCnv
    while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
      castor::stager::TapeCopy* object = new castor::stager::TapeCopy();
      object->setCopyNb(rset->getInt(1));
      object->setId((u_signed64)rset->getDouble(2));
      object->setStatus((enum castor::stager::TapeCopyStatusCodes)rset->getInt(4));
      result.push_back(object);
    }
    m_selectTapeCopiesForMigrationStatement->closeResultSet(rset);
    return result;
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select TapeCopies for migration :"
      << std::endl << e.getMessage();
    throw ex;
  }
}

// -----------------------------------------------------------------------
// resetStream
// -----------------------------------------------------------------------
void castor::db::ora::OraTapeSvc::resetStream
(castor::stager::Stream* stream)
  throw (castor::exception::Exception) {
    try {
    // Check whether the statements are ok
    if (0 == m_resetStreamStatement) {
      m_resetStreamStatement =
        createStatement(s_resetStreamStatementString);
      m_resetStreamStatement->setAutoCommit(true);
    }
    // execute the statement and see whether we found something
    m_resetStreamStatement->setDouble(1, stream->id());
    (void)m_resetStreamStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in resetStream."
      << std::endl << e.what();
    throw ex;
  }
}

// -----------------------------------------------------------------------
// failedSegments
// -----------------------------------------------------------------------
std::vector<castor::stager::Segment*>
castor::db::ora::OraTapeSvc::failedSegments ()
  throw (castor::exception::Exception) {
  std::vector<castor::stager::Segment*> result;
  try {
    // Check whether the statements are ok
    if (0 == m_failedSegmentsStatement) {
      m_failedSegmentsStatement =
        createStatement(s_failedSegmentsStatementString);
      m_failedSegmentsStatement->registerOutParam
        (1, oracle::occi::OCCICURSOR);
    }
    // execute the statement and see whether we found something
    unsigned int nb = m_failedSegmentsStatement->executeUpdate();
    if (0 == nb) {
      // NO failed Segments. Good news !
      return result;
    }
    oracle::occi::ResultSet *rs =
      m_failedSegmentsStatement->getCursor(1);
    // Run through the cursor
    oracle::occi::ResultSet::Status status = rs->next();
    while(status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      castor::stager::Segment* item =
        new castor::stager::Segment();
      item->setFseq(rs->getInt(1));
      item->setOffset((u_signed64)rs->getDouble(2));
      item->setBytes_in((u_signed64)rs->getDouble(3));
      item->setBytes_out((u_signed64)rs->getDouble(4));
      item->setHost_bytes((u_signed64)rs->getDouble(5));
      item->setSegmCksumAlgorithm(rs->getString(6));
      item->setSegmCksum(rs->getInt(7));
      item->setErrMsgTxt(rs->getString(8));
      item->setErrorCode(rs->getInt(9));
      item->setSeverity(rs->getInt(10));
      item->setBlockId0(rs->getInt(11));
      item->setBlockId1(rs->getInt(12));
      item->setBlockId2(rs->getInt(13));
      item->setBlockId3(rs->getInt(14));
      item->setCreationTime(rs->getInt(15));
      item->setId((u_signed64)rs->getDouble(16));
      item->setStatus((enum castor::stager::SegmentStatusCodes)rs->getInt(19));
      result.push_back(item);
      status = rs->next();
    }
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in failedSegments."
      << std::endl << e.what();
    throw ex;
  }
  return result;
}

// -----------------------------------------------------------------------
// checkFileForRepack
// -----------------------------------------------------------------------
std::string castor::db::ora::OraTapeSvc::checkFileForRepack
(const u_signed64 fileId)
  throw (castor::exception::Exception) {
  std::string repackvid = "";
  try {
    if (0 == m_checkFileForRepackStatement) {
      m_checkFileForRepackStatement =
        createStatement(s_checkFileForRepackStatementString);
      m_checkFileForRepackStatement->registerOutParam
        (2, oracle::occi::OCCISTRING, 2048);
    }
    m_checkFileForRepackStatement->setDouble(1, fileId); 
    m_checkFileForRepackStatement->setAutoCommit(true);
    m_checkFileForRepackStatement->executeUpdate();

    repackvid = m_checkFileForRepackStatement->getString(2);
    
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in checkFileForRepack(): Fileid (" 
      << fileId << ","
      << repackvid<< ")"
      << std::endl << e.what();
    throw ex;
  }
  return repackvid;
}

// -----------------------------------------------------------------------
// getNumFilesByStream
// -----------------------------------------------------------------------
u_signed64 castor::db::ora::OraTapeSvc::getNumFilesByStream
(const u_signed64 streamId)
  throw (castor::exception::Exception) {
  u_signed64 numFile =0;
  try {
    if (0 == m_getNumFilesByStreamStatement) {
      m_getNumFilesByStreamStatement =
        createStatement(s_getNumFilesByStreamStatementString);
      m_getNumFilesByStreamStatement->registerOutParam
        (2, oracle::occi::OCCIDOUBLE);
    }
    m_getNumFilesByStreamStatement->setDouble(1, streamId); 
    m_getNumFilesByStreamStatement->executeUpdate();

    numFile = (u_signed64)m_getNumFilesByStreamStatement->getDouble(2);
    
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in  getNumFilesByStream(): Fileid (" 
      << streamId <<")"
      << std::endl << e.what();
    throw ex;
  }
  return numFile;
}

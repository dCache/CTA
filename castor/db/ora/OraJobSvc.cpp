/******************************************************************************
 *                      OraJobSvc.cpp
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
 *
 * Implementation of the IJobSvc for Oracle
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
#include "castor/stager/Request.hpp"
#include "castor/stager/SvcClass.hpp"
#include "castor/stager/FileClass.hpp"
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/SubRequest.hpp"
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
#include "castor/db/ora/OraJobSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Busy.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/RequestCanceled.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/DiskCopyStatusCodes.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/dlf/Dlf.hpp"
#include "occi.h"
#include "getconfent.h"
#include <Cuuid.h>
#include <string>
#include <sstream>
#include <vector>
#include <Cns_api.h>
#include <vmgr_api.h>
#include <Ctape_api.h>
#include <serrno.h>
#include <string.h>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::SvcFactory<castor::db::ora::OraJobSvc>* s_factoryOraJobSvc =
  new castor::SvcFactory<castor::db::ora::OraJobSvc>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------

/// SQL statement for getUpdateStart
const std::string castor::db::ora::OraJobSvc::s_getUpdateStartStatementString =
  "BEGIN getUpdateStart(:1, :2, :3, :4, :5, :6, :7, :8, :9); END;";

/// SQL statement for putStart
const std::string castor::db::ora::OraJobSvc::s_putStartStatementString =
  "BEGIN putStart(:1, :2, :3, :4, :5, :6); END;";

/// SQL statement for prepareForMigration
const std::string castor::db::ora::OraJobSvc::s_prepareForMigrationStatementString =
"BEGIN prepareForMigration(:1, :2, :3, :4, :5, :6, :7); END;";

/// SQL statement for getUpdateDone
const std::string castor::db::ora::OraJobSvc::s_getUpdateDoneStatementString =
  "BEGIN getUpdateDoneProc(:1); END;";

/// SQL statement for getUpdateFailed
const std::string castor::db::ora::OraJobSvc::s_getUpdateFailedStatementString =
  "BEGIN getUpdateFailedProc(:1); END;";

/// SQL statement for putFailed
const std::string castor::db::ora::OraJobSvc::s_putFailedStatementString =
  "BEGIN putFailedProc(:1); END;";

/// SQL statement for firstByteWritten
const std::string castor::db::ora::OraJobSvc::s_firstByteWrittenStatementString =
  "BEGIN firstByteWrittenProc(:1); END;";

// -----------------------------------------------------------------------------
// OraJobSvc
//------------------------------------------------------------------------------
castor::db::ora::OraJobSvc::OraJobSvc(const std::string name) :
  OraCommonSvc(name),
  m_getUpdateStartStatement(0),
  m_putStartStatement(0),
  m_putDoneStartStatement(0),
  m_prepareForMigrationStatement(0),
  m_getUpdateDoneStatement(0),
  m_getUpdateFailedStatement(0),
  m_putFailedStatement(0),
  m_firstByteWrittenStatement(0) {
}

//------------------------------------------------------------------------------
// ~OraJobSvc
//------------------------------------------------------------------------------
castor::db::ora::OraJobSvc::~OraJobSvc() throw() {
  reset();
}

//------------------------------------------------------------------------------
// id
//------------------------------------------------------------------------------
unsigned int castor::db::ora::OraJobSvc::id() const {
  return ID();
}

//------------------------------------------------------------------------------
// ID
//------------------------------------------------------------------------------
unsigned int castor::db::ora::OraJobSvc::ID() {
  return castor::SVC_ORAJOBSVC;
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraJobSvc::reset() throw() {
  // Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  OraCommonSvc::reset();
  try {
    if (m_getUpdateStartStatement) deleteStatement(m_getUpdateStartStatement);
    if (m_putStartStatement) deleteStatement(m_putStartStatement);
    if (m_prepareForMigrationStatement) deleteStatement(m_prepareForMigrationStatement);
    if (m_getUpdateDoneStatement) deleteStatement(m_getUpdateDoneStatement);
    if (m_getUpdateFailedStatement) deleteStatement(m_getUpdateFailedStatement);
    if (m_putFailedStatement) deleteStatement(m_putFailedStatement);
    if (m_firstByteWrittenStatement) deleteStatement(m_firstByteWrittenStatement);
  } catch (castor::exception::Exception& ignored) {};
  // Now reset all pointers to 0
  m_getUpdateStartStatement = 0;
  m_putStartStatement = 0;
  m_prepareForMigrationStatement = 0;
  m_getUpdateDoneStatement = 0;
  m_getUpdateFailedStatement = 0;
  m_putFailedStatement = 0;
  m_firstByteWrittenStatement = 0;
}

//------------------------------------------------------------------------------
// getUpdateStart
//------------------------------------------------------------------------------
std::string
castor::db::ora::OraJobSvc::getUpdateStart
(castor::stager::SubRequest* subreq,
 std::string diskServerName,
 std::string mountPoint,
 bool* emptyFile,
 u_signed64,
 const std::string)
  throw (castor::exception::Exception) {
  *emptyFile = false;
  try {
    // Check whether the statements are ok
    if (0 == m_getUpdateStartStatement) {
      m_getUpdateStartStatement =
        createStatement(s_getUpdateStartStatementString);
      m_getUpdateStartStatement->registerOutParam
        (4, oracle::occi::OCCIDOUBLE);
      m_getUpdateStartStatement->registerOutParam
        (5, oracle::occi::OCCISTRING, 2048);
      m_getUpdateStartStatement->registerOutParam
        (6, oracle::occi::OCCIINT);
      m_getUpdateStartStatement->registerOutParam
        (7, oracle::occi::OCCIINT);
      m_getUpdateStartStatement->registerOutParam
        (8, oracle::occi::OCCIINT);
      m_getUpdateStartStatement->registerOutParam
        (9, oracle::occi::OCCIDOUBLE);
      m_getUpdateStartStatement->setAutoCommit(true);
    }
    // execute the statement and see whether we found something
    m_getUpdateStartStatement->setDouble(1, subreq->id());
    m_getUpdateStartStatement->setString(2, diskServerName);
    m_getUpdateStartStatement->setString(3, mountPoint);
    unsigned int nb = m_getUpdateStartStatement->executeUpdate();
    if (0 == nb) {
      rollback();
      castor::exception::Internal ex;
      ex.getMessage()
        << "getUpdateStart : unable to schedule SubRequest.";
      throw ex;
    }

    // Check result
    u_signed64 id = (u_signed64)m_getUpdateStartStatement->getDouble(4);
    // If no DiskCopy returned, we have to wait, hence return
    if (0 == id) {
      return 0;
    }

    enum castor::stager::DiskCopyStatusCodes status =
       (enum castor::stager::DiskCopyStatusCodes) m_getUpdateStartStatement->getInt(6);
    // Deal with recalls of empty files
    // the file may have been declared recalled without being created on disk
    if (status == castor::stager::DISKCOPY_STAGED &&
        0 == (u_signed64)m_getUpdateStartStatement->getDouble(9)) {
      *emptyFile = true;
    }

    // return diskCopy Path
    return m_getUpdateStartStatement->getString(5);
  } catch (oracle::occi::SQLException e) {
    // Application specific errors
    if (e.getErrorCode() == 20114) {
      castor::exception::RequestCanceled ex;
      std::string error = e.what();
      ex.getMessage() << error.substr(error.find("ORA-") + 11,
                                      error.find("ORA-", 4) - 12);
      throw ex;
    }
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getUpdateStart."
      << std::endl << e.what();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// putStart
//------------------------------------------------------------------------------
std::string
castor::db::ora::OraJobSvc::putStart
(castor::stager::SubRequest* subreq,
 std::string diskServerName,
 std::string mountPoint,
 u_signed64,
 const std::string)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_putStartStatement) {
      m_putStartStatement =
        createStatement(s_putStartStatementString);
      m_putStartStatement->registerOutParam
        (4, oracle::occi::OCCIDOUBLE);
      m_putStartStatement->registerOutParam
        (5, oracle::occi::OCCIINT);
      m_putStartStatement->registerOutParam
        (6, oracle::occi::OCCISTRING, 2048);
      m_putStartStatement->setAutoCommit(true);
    }
    // execute the statement and see whether we found something
    m_putStartStatement->setDouble(1, subreq->id());
    m_putStartStatement->setString(2, diskServerName);
    m_putStartStatement->setString(3, mountPoint);
    unsigned int nb = m_putStartStatement->executeUpdate();
    if (0 == nb) {
      rollback();
      castor::exception::Internal ex;
      ex.getMessage()
        << "putStart : unable to schedule SubRequest.";
      throw ex;
    }
    // return diskCopy Path
    return m_putStartStatement->getString(6);
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    // Application specific errors
    if (e.getErrorCode() == 20104) {
      castor::exception::RequestCanceled ex;
      std::string error = e.what();
      ex.getMessage() << error.substr(error.find("ORA-") + 11,
                                      error.find("ORA-", 4) - 12);
      throw ex;
    }
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in putStart."
      << std::endl << e.what();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// prepareForMigration
//------------------------------------------------------------------------------
void castor::db::ora::OraJobSvc::prepareForMigration
(u_signed64 subReqId,
 u_signed64 fileSize,
 u_signed64 timeStamp,
 u_signed64,
 const std::string,
 const std::string csumtype,
 const std::string csumvalue)
  throw (castor::exception::Exception) {
  double lastNsOpenTime;
  try {
    // Check whether the statements are ok
    if (0 == m_prepareForMigrationStatement) {
      m_prepareForMigrationStatement =
        createStatement(s_prepareForMigrationStatementString);
      m_prepareForMigrationStatement->setAutoCommit(false);
      m_prepareForMigrationStatement->registerOutParam
        (4, oracle::occi::OCCIDOUBLE);
      m_prepareForMigrationStatement->registerOutParam
        (5, oracle::occi::OCCISTRING, 2048);
      m_prepareForMigrationStatement->registerOutParam
        (6, oracle::occi::OCCIINT);
      m_prepareForMigrationStatement->registerOutParam
        (7, oracle::occi::OCCIDOUBLE);
    }
    // Execute the statement and see whether we found something
    m_prepareForMigrationStatement->setDouble(1, subReqId);
    m_prepareForMigrationStatement->setDouble(2, fileSize);
    m_prepareForMigrationStatement->setDouble(3, timeStamp);
    unsigned int nb = m_prepareForMigrationStatement->executeUpdate();
    if (0 == nb) {
      castor::exception::Internal ex;
      ex.getMessage()
        << "prepareForMigration did not return any result.";
      throw ex;
    }

    // Populate the fileid structure to be passed to Cns_closex and DLF log
    // messages
    struct Cns_fileid fileid;
    fileid.fileid = (u_signed64)m_prepareForMigrationStatement->getDouble(4);
    strncpy(fileid.server,
            m_prepareForMigrationStatement->getString(5).c_str(),
            CA_MAXHOSTNAMELEN);

    // Check for errors
    int returnCode = m_prepareForMigrationStatement->getInt(6);
    if (returnCode == 1) {
      // The file got deleted while it was being written to. This by itself is
      // not an error, but we should not take any action here.
      // "File was deleted while it was written to. Giving up with migration."
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM,
                              DLF_BASE_ORACLELIB, 0, 0, &fileid);
      return;
    }
    lastNsOpenTime = m_prepareForMigrationStatement->getDouble(7);

    // If we got this far we need to update the file size and checksum
    // information related to the file in the name server. By updating the
    // size we also delete any segments associated to that file.

    // Prepare the name server error buffer
    char cns_error_buffer[512];  /* Cns error buffer */
    *cns_error_buffer = 0;
    if (Cns_seterrbuf(cns_error_buffer, sizeof(cns_error_buffer)) != 0) {
      castor::exception::Exception ex(serrno);
      ex.getMessage()
        << "prepareForMigration: Cns_seterrbuf failed.";
      // Rollback the prepareForMigration changes
      cnvSvc()->rollback();
      throw ex;
    }

    // Check if checksum information should be updated in the name space.
    bool useChkSum = true;
    const char *confValue = getconfent("CNS", "USE_CKSUM", 0);
    if (confValue != NULL) {
      if (!strncasecmp(confValue, "no", 2)) {
        useChkSum = false;
      }
    }

    // Call Cns_closex.
    int rc = 0;
    if (useChkSum) {
      rc = Cns_closex(&fileid, fileSize, csumtype.c_str(), csumvalue.c_str(),
                      timeStamp, lastNsOpenTime, NULL);
    } else {
      rc = Cns_closex(&fileid, fileSize, "", "", timeStamp, lastNsOpenTime, NULL);
    }

    if (rc != 0) {
      if (serrno == ENSFILECHG) {
        // Special case where the Cns_closex was not taken into account due to
        // concurrent modifications on the same file on another stager. This is
        // ok, but we still log something.
        // "Cns_closex ignored by name server due to concurrent file modification on another stager"
        castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, DLF_BASE_ORACLELIB + 35, 0, 0, &fileid);
      } else {
        castor::exception::Exception ex(serrno);
        ex.getMessage()
          << "prepareForMigration: Cns_closex failed: ";
        if (!strcmp(cns_error_buffer, "")) {
          ex.getMessage() << sstrerror(serrno);
        } else {
          ex.getMessage() << cns_error_buffer;
        }
        // Rollback the prepareForMigration changes
        cnvSvc()->rollback();
        throw ex;
      }
    }

    // Commit the prepareForMigration changes
    cnvSvc()->commit();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in prepareForMigration."
      << std::endl << e.what();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getUpdateDone
//------------------------------------------------------------------------------
void castor::db::ora::OraJobSvc::getUpdateDone
(u_signed64 subReqId,
 u_signed64,
 const std::string)
  throw (castor::exception::Exception) {
  // Check whether the statements are ok
  if (0 == m_getUpdateDoneStatement) {
    m_getUpdateDoneStatement =
      createStatement(s_getUpdateDoneStatementString);
    m_getUpdateDoneStatement->setAutoCommit(true);
  }
  // Execute statement and get result
  try {
    m_getUpdateDoneStatement->setDouble(1, subReqId);
    m_getUpdateDoneStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to clean Get/Update subRequest :"
      << std::endl << e.getMessage();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getUpdateFailed
//------------------------------------------------------------------------------
void castor::db::ora::OraJobSvc::getUpdateFailed
(u_signed64 subReqId,
 u_signed64,
 const std::string)
  throw (castor::exception::Exception) {
  // Check whether the statements are ok
  if (0 == m_getUpdateFailedStatement) {
    m_getUpdateFailedStatement =
      createStatement(s_getUpdateFailedStatementString);
    m_getUpdateFailedStatement->setAutoCommit(true);
  }
  // Execute statement and get result
  try {
    m_getUpdateFailedStatement->setDouble(1, subReqId);
    m_getUpdateFailedStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to mark subRequest as FAILED :"
      << std::endl << e.getMessage();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// putFailed
//------------------------------------------------------------------------------
void castor::db::ora::OraJobSvc::putFailed
(u_signed64 subReqId,
 u_signed64,
 const std::string)
  throw (castor::exception::Exception) {
  // Check whether the statements are ok
  if (0 == m_putFailedStatement) {
    m_putFailedStatement =
      createStatement(s_putFailedStatementString);
    m_putFailedStatement->setAutoCommit(true);
  }
  // Execute statement and get result
  try {
    m_putFailedStatement->setDouble(1, subReqId);
    m_putFailedStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to clean Put subRequest :"
      << std::endl << e.getMessage();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// firstByteWritten
//------------------------------------------------------------------------------
void castor::db::ora::OraJobSvc::firstByteWritten
(u_signed64 subRequestId,
 u_signed64,
 const std::string)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_firstByteWrittenStatement) {
      m_firstByteWrittenStatement =
        createStatement(s_firstByteWrittenStatementString);
      m_firstByteWrittenStatement->setAutoCommit(true);
    }
    // execute the statement
    m_firstByteWrittenStatement->setDouble(1, subRequestId);
    m_firstByteWrittenStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    // Application specific errors
    if (e.getErrorCode() == 20106) {
      castor::exception::Busy ex;
      std::string error = e.what();
      ex.getMessage() << error.substr(error.find("ORA-") + 11,
                                      error.find("ORA-", 4) - 12);
      throw ex;
    }
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in firstByteWritten."
      << std::endl << e.what();
    throw ex;
  }
}

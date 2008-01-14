/******************************************************************************
 *                      OraGCSvc.cpp
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
 * @(#)$RCSfile: OraGCSvc.cpp,v $ $Revision: 1.31 $ $Release$ $Date: 2008/01/14 17:49:44 $ $Author: waldron $
 *
 * Implementation of the IGCSvc for Oracle
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
#include "castor/db/ora/OraGCSvc.hpp"
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
#include "Cglobals.h"
#include <Cuuid.h>
#include <string>
#include <sstream>
#include <vector>
#include <Cns_api.h>
#include <vmgr_api.h>
#include <Ctape_api.h>
#include <serrno.h>

#define NS_SEGMENT_NOTOK (' ')


//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::SvcFactory<castor::db::ora::OraGCSvc>* s_factoryOraGCSvc =
  new castor::SvcFactory<castor::db::ora::OraGCSvc>();


//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for selectFiles2Delete (select part)
const std::string castor::db::ora::OraGCSvc::s_selectFiles2DeleteStatementString =
  "BEGIN selectFiles2Delete(:1, :2); END;";

/// SQL statement for selectFiles2Delete (update part)
const std::string castor::db::ora::OraGCSvc::s_selectFiles2DeleteStatementString2 =
  "BEGIN updateFiles2Delete(:1); END;";

/// SQL statement for filesDeleted
const std::string castor::db::ora::OraGCSvc::s_filesDeletedStatementString =
  "BEGIN filesDeletedProc(:1, :2); END;";

/// SQL statement for filesDeletedTruncate
/// The statement behind this should really be a TRUNCATE as this is more
/// efficient. However, for unexplained reasons this doesn't work and
/// results in the stager continuously trying to delete the same files
/// from the nameserver over and over again!
const std::string castor::db::ora::OraGCSvc::s_filesDeletedTruncateStatementString =
  "DELETE FROM FilesDeletedProcOutput";

/// SQL statement for filesDeletionFailed
const std::string castor::db::ora::OraGCSvc::s_filesDeletionFailedStatementString =
  "BEGIN filesDeletionFailedProc(:1); END;";

/// SQL statement for nsFilesDeleted
const std::string castor::db::ora::OraGCSvc::s_nsFilesDeletedStatementString =
  "BEGIN nsFilesDeletedProc(:1, :2, :3); END;";

//------------------------------------------------------------------------------
// OraGCSvc
//------------------------------------------------------------------------------
castor::db::ora::OraGCSvc::OraGCSvc(const std::string name) :
  OraCommonSvc(name),
  m_selectFiles2DeleteStatement(0),
  m_selectFiles2DeleteStatement2(0),
  m_filesDeletedStatement(0),
  m_filesDeletedTruncateStatement(0),
  m_filesDeletionFailedStatement(0),
  m_nsFilesDeletedStatement(0) {
}

//------------------------------------------------------------------------------
// ~OraGCSvc
//------------------------------------------------------------------------------
castor::db::ora::OraGCSvc::~OraGCSvc() throw() {
  reset();
}

//------------------------------------------------------------------------------
// id
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraGCSvc::id() const {
  return ID();
}

//------------------------------------------------------------------------------
// ID
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraGCSvc::ID() {
  return castor::SVC_ORAGCSVC;
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraGCSvc::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  OraCommonSvc::reset();
  try {
    if (m_selectFiles2DeleteStatement) deleteStatement(m_selectFiles2DeleteStatement);
    if (m_selectFiles2DeleteStatement2) deleteStatement(m_selectFiles2DeleteStatement2);
    if (m_filesDeletedStatement) deleteStatement(m_filesDeletedStatement);
    if (m_filesDeletedTruncateStatement) deleteStatement(m_filesDeletedTruncateStatement);
    if (m_filesDeletionFailedStatement) deleteStatement(m_filesDeletionFailedStatement);
    if (m_nsFilesDeletedStatement) deleteStatement(m_nsFilesDeletedStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_selectFiles2DeleteStatement = 0;
  m_selectFiles2DeleteStatement2 = 0;
  m_filesDeletedStatement = 0;
  m_filesDeletedTruncateStatement = 0;
  m_filesDeletionFailedStatement = 0;
  m_nsFilesDeletedStatement = 0;
}

//------------------------------------------------------------------------------
// selectFiles2Delete
//------------------------------------------------------------------------------
std::vector<castor::stager::GCLocalFile*>*
castor::db::ora::OraGCSvc::selectFiles2Delete
(std::string diskServer)
  throw (castor::exception::Exception) {
  // Check whether the statements are ok
  if (0 == m_selectFiles2DeleteStatement) {
    m_selectFiles2DeleteStatement =
      createStatement(s_selectFiles2DeleteStatementString);
    m_selectFiles2DeleteStatement->registerOutParam
      (2, oracle::occi::OCCICURSOR);
  }
  if (0 == m_selectFiles2DeleteStatement2) {
    m_selectFiles2DeleteStatement2 =
      createStatement(s_selectFiles2DeleteStatementString2);
  }
  // vector of results
  std::vector<castor::stager::GCLocalFile*>* result = 0;
  // Get files to delete
  unsigned int nb=0;
  ub2 *lens = 0;
  unsigned char (*buffer)[21] = 0;

  try {
    m_selectFiles2DeleteStatement->setString(1, diskServer);
    m_selectFiles2DeleteStatement->executeUpdate();
    bool foundSomething = false;
    // create result
    result = new std::vector<castor::stager::GCLocalFile*>;
    // create list of ids for the update
    std::vector<u_signed64> dcIds;
    // get the result, that is a cursor on the files to delete
    oracle::occi::ResultSet *rset =
      m_selectFiles2DeleteStatement->getCursor(2);
    // Loop over the files returned
    while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
      foundSomething = true;
      // Fill result
      castor::stager::GCLocalFile* f = new castor::stager::GCLocalFile();
      f->setFileName(rset->getString(1));
      f->setDiskCopyId((u_signed64)rset->getDouble(2));
      f->setFileId((u_signed64)rset->getDouble(3));
      f->setNsHost(rset->getString(4));
      result->push_back(f);
      // keep id
      dcIds.push_back(f->diskCopyId());
    }
    if (foundSomething) {
      // Deal with the list of diskcopy ids
      nb = dcIds.size();
      lens=(ub2 *)malloc (sizeof(ub2)*nb);
      buffer=(unsigned char(*)[21]) calloc(nb * 21, sizeof(unsigned char));
      for (unsigned int i = 0; i < nb; i++) {
        oracle::occi::Number n = (double)(dcIds[i]);
        oracle::occi::Bytes b = n.toBytes();
        b.getBytes(buffer[i],b.length());
        lens[i] = b.length();
      }
      ub4 unused = nb;
      m_selectFiles2DeleteStatement2->setDataBufferArray
        (1, buffer, oracle::occi::OCCI_SQLT_NUM,
         nb, &unused, 21, lens);
      // execute the statement
      m_selectFiles2DeleteStatement2->executeUpdate();
      //free allocated memory
      free(lens);
      free(buffer);
    }
    m_selectFiles2DeleteStatement->closeResultSet(rset);
    commit();
    return result;
  } catch (oracle::occi::SQLException e) {

    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select files to delete :\n"
      << e.getMessage();
    // release memory if needed
    if (0 != result) {
      for (std::vector<castor::stager::GCLocalFile*>::iterator it =
             result->begin();
           it != result->end();
           it++) {
        delete *it;
      }
      delete result;
    }
    //free allocated memory
    if (0 != lens) free(lens);
    if (0 != buffer) free(buffer);
    handleException(e);
    throw ex;
  }
}

//------------------------------------------------------------------------------
// filesDeleted
//------------------------------------------------------------------------------
void castor::db::ora::OraGCSvc::filesDeleted
(std::vector<u_signed64*>& diskCopyIds)
  throw (castor::exception::Exception) {
  // Check whether the statements are ok
  if (0 == m_filesDeletedStatement) {
    m_filesDeletedStatement =
      createStatement(s_filesDeletedStatementString);
    m_filesDeletedStatement->registerOutParam
      (2, oracle::occi::OCCICURSOR);
    m_filesDeletedStatement->setAutoCommit(true);
  }
  // Check whether the statements are ok
  if (0 == m_filesDeletedTruncateStatement) {
    m_filesDeletedTruncateStatement =
      createStatement(s_filesDeletedTruncateStatementString);
    m_filesDeletedTruncateStatement->setAutoCommit(true);
  }
  // Execute statement and get result
  //unsigned long id;
  ub2 *lens = 0;
  unsigned char (*buffer)[21] = 0;
  unsigned int nba=0;
  try {
    // Deal with the list of diskcopy ids
    unsigned int nb = diskCopyIds.size();
    // Compute actual length of the buffers : this
    // may be different from the needed one, since
    // Oracle does not like 0 length arrays....
    nba = nb == 0 ? 1 : nb;
    lens=(ub2 *)malloc(sizeof(ub2)*nb);
    buffer=(unsigned char(*)[21]) calloc(nba * 21, sizeof(unsigned char));
    for (unsigned int i = 0; i < nb; i++) {
      oracle::occi::Number n = (double)(*(diskCopyIds[i]));
      oracle::occi::Bytes b = n.toBytes();
      b.getBytes(buffer[i],b.length());
      lens[i] = b.length();
    }
    ub4 unused = nb;
    m_filesDeletedStatement->setDataBufferArray
      (1, buffer, oracle::occi::OCCI_SQLT_NUM,
       nba, &unused, 21, lens);
    // execute the statement
    m_filesDeletedStatement->executeUpdate();
    if (0 == nb) {
      // we want to commit anyway to release locks
      castor::exception::Internal ex;
      ex.getMessage() << "filesDeleted : no rows returned.";
      //free allocated memory
      free(lens);
      free(buffer); 
      throw ex;
    }
    // get the result, that is a cursor on the files to
    // remove from the name server
    oracle::occi::ResultSet *rs =
      m_filesDeletedStatement->getCursor(2);

    // If there are files to be deleted from name server
    oracle::occi::ResultSet::Status status = rs->next();
    if (status == oracle::occi::ResultSet::DATA_AVAILABLE) {

      /////////////////////////////////////////////////////////
      // Here we just want to go through the files to delete //
      // from the nameserver. However, the interface to the  //
      // name server is such that it is quite complicated !  //
      /////////////////////////////////////////////////////////

      // First thing, we need a thread safe variable to store
      // the error messages comming from the name server
      // XXX LIMITATION ON A STRING LENGTH : hardcoded to 1024 !
      static int nsErrBufKey = -1;
      void *errBuf = NULL;
      int errBufLen = 1024;
      bool gotError = false;
      if (-1 == Cglobals_get(&nsErrBufKey, &errBuf, errBufLen)) {
        clog() << ERROR
               << "Unable to get thread safe variable in filesDeleted. "
               << "The following files won't be deleted from name server"
               << " while they should have been (we give fileids here) :"
               << std::endl;
        gotError = true;
      }
      if (!gotError) {
        // Now let's give this buffer to the name server
        if (0 != Cns_seterrbuf((char*)errBuf, errBufLen)) {
          clog() << ERROR
                 << "Error caught when calling Cns_seterrbuf in filesDeleted. "
                 << "The following files won't be deleted from name server"
                 << " while they should have been (we give fileids here) :"
                 << std::endl;
          gotError = true;
        }
      }

      if (gotError) {
        try {
          oracle::occi::ResultSet::Status status = rs->next();
          while (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
            //u_signed64 fileid = (u_signed64) rs->getDouble(1);
            std::string nsHost = rs->getString(2);
            clog() << ERROR << (u_signed64)rs->getDouble(1)
                   << "@" << rs->getString(2)
                   << std::endl;
            status = rs->next();
          }
          m_filesDeletedStatement->closeResultSet(rs);
        } catch (oracle::occi::SQLException e) {
          clog() << ERROR << "Error caught while listing fileids :\n"
                 << e.getMessage() << "\nGiving up." << std::endl;
        }
        // Cleanup the DB
        try {
          m_filesDeletedTruncateStatement->execute();
        } catch (oracle::occi::SQLException e) {
          clog() << ERROR << "Error caught while truncating FilesDeletedProcOutput :\n"
                 << e.getMessage() << std::endl;
        }
        //free allocated memory
        free(lens);
        free(buffer);
        return;
      }

      // Second thing, we need a buffer to store the castor
      // file names.
      // XXX LIMITATION ON A STRING LENGTH
      // XXX THIS IS INHERITED FROM THE NAMESERVER INTERFACE
      // XXX IT IS A RISK OF MEMORY CORRUPTION IN CASE THE
      // XXX NAME SERVER RETURNS A TOO LONG NAME !!!!!
      // However, CA_MAXPATHLEN is also used in the name server
      // so we should be safe as long as it is not modified
      char castorFileName[CA_MAXPATHLEN+1];

      // Now let's go through the files to delete
      while (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
        u_signed64 fileid = (u_signed64) rs->getDouble(1);
        std::string nsHost = rs->getString(2);
        // and first of all, get the file name
        if (0 != Cns_getpath((char*)nsHost.c_str(), fileid, castorFileName)) {
	  if (!strcmp((char *)errBuf, "")) {
	    strncpy((char *)errBuf, sstrerror(serrno), errBufLen);
	  }
          clog() << ERROR
                 << "Error caught when calling Cns_getpath in filesDeleted "
                 << "for fileid " << fileid << " and nsHost "
                 << nsHost << " : " << (char*)errBuf
                 << ". This file won't be "
                 << "deleted from name server while it should have been."
                 << std::endl;
        } else {
          if (0 != Cns_unlink(castorFileName)) {
	    if (strcmp((char *)errBuf, "")) {
	      strncpy((char *)errBuf, sstrerror(serrno), errBufLen);  
	    }
            clog() << ERROR
                   << "Error caught when unlinking "
                   << castorFileName << " (fileid "
                   << fileid << ") from name server "
                   << nsHost << " : " << (char*)errBuf
                   << std::endl;
          }
        }
        status = rs->next();
      }
    }
    m_filesDeletedStatement->closeResultSet(rs);
    // Cleanup the DB
    try {
      m_filesDeletedTruncateStatement->execute();
    } catch (oracle::occi::SQLException e) {
      clog() << ERROR << "Error caught while truncating FilesDeletedProcOutput :\n"
             << e.getMessage() << std::endl;
    }
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to remove deleted files :\n"
      << e.getMessage();
    handleException(e);
    //free allocated memory
    if (0 != lens) free(lens);
    if (buffer != 0) free(buffer);
    throw ex;
  }
}

//------------------------------------------------------------------------------
// filesDeletionFailed
//------------------------------------------------------------------------------
void castor::db::ora::OraGCSvc::filesDeletionFailed
(std::vector<u_signed64*>& diskCopyIds)
  throw (castor::exception::Exception) {
  // Check whether the statements are ok
  if (0 == m_filesDeletionFailedStatement) {
    m_filesDeletionFailedStatement =
      createStatement(s_filesDeletionFailedStatementString);
    m_filesDeletionFailedStatement->setAutoCommit(true);
  }
  // Execute statement and get result
  ub2 *lens;
  unsigned char (*buffer)[21] = 0;
  unsigned int nba;
  try {
    // Deal with the list of diskcopy ids
    unsigned int nb = diskCopyIds.size();
    // Compute actual length of the buffers : this
    // may be different from the needed one, since
    // Oracle does not like 0 length arrays....
    nba = nb == 0 ? 1 : nb;
    lens=(ub2 *)malloc(sizeof(ub2)*nb);
    buffer=(unsigned char(*)[21]) calloc(nba * 21, sizeof(unsigned char));
    for (unsigned int i = 0; i < nb; i++) {
      oracle::occi::Number n = (double)(*(diskCopyIds[i]));
      oracle::occi::Bytes b = n.toBytes();
      b.getBytes(buffer[i],b.length());
      lens[i] = b.length();
    }
    ub4 unused = nb;
    m_filesDeletionFailedStatement->setDataBufferArray
      (1, buffer, oracle::occi::OCCI_SQLT_NUM,
       nba, &unused, 21, lens);
    // execute the statement
    m_filesDeletionFailedStatement->executeUpdate();
    free(lens);
    free(buffer);
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to remove files for which deletion failed :"
      << std::endl << e.getMessage();
    free(lens);
    free(buffer);
    throw ex;
  }
}

//------------------------------------------------------------------------------
// nsFilesDeleted
//------------------------------------------------------------------------------
std::vector<u_signed64> castor::db::ora::OraGCSvc::nsFilesDeleted
(std::vector<u_signed64> &fileIds,
 std::string nsHost) throw() {
  std::vector<u_signed64> orphans;
  // do not call oracle if not needed
  if (0 == fileIds.size()) return orphans;
  ub2 *lens = 0;
  unsigned char (*buffer)[21] = 0;
  try {
    // Check whether the statements are ok
    if (0 == m_nsFilesDeletedStatement) {
      m_nsFilesDeletedStatement =
        createStatement(s_nsFilesDeletedStatementString);
      m_nsFilesDeletedStatement->registerOutParam
        (3, oracle::occi::OCCICURSOR);
    }
    // Deal with the list of fileIds
    unsigned int nb = fileIds.size();
    lens=(ub2 *)malloc(sizeof(ub2)*nb);
    buffer=(unsigned char(*)[21]) calloc(nb * 21, sizeof(unsigned char));
    for (unsigned int i = 0; i < nb; i++) {
      oracle::occi::Number n = (double)(fileIds[i]);
      oracle::occi::Bytes b = n.toBytes();
      b.getBytes(buffer[i],b.length());
      lens[i] = b.length();
    }
    ub4 unused = nb;
    m_nsFilesDeletedStatement->setString(1, nsHost);
    m_nsFilesDeletedStatement->setDataBufferArray
      (2, buffer, oracle::occi::OCCI_SQLT_NUM,
       nb, &unused, 21, lens);
    // execute the statement
    m_nsFilesDeletedStatement->executeUpdate();
    // get the result, that is a cursor on the fileIds that were not
    // present in the stager
    oracle::occi::ResultSet *rset =
      m_nsFilesDeletedStatement->getCursor(3);
    // Loop over the files returned
    while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
      orphans.push_back((u_signed64)rset->getDouble(1));
    }
    commit();
    if (0 != lens) free(lens);
    if (0 != buffer) free(buffer);
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    // "Error caught in nsFilesDeleted"
    castor::dlf::Param params[] =
      {castor::dlf::Param("message", e.getMessage())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, DLF_BASE_ORACLELIB + 15, 1, params);
    if (0 != lens) free(lens);
    if (0 != buffer) free(buffer);
  }
  return orphans;
}

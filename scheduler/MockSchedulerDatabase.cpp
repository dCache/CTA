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

#include "common/exception/Exception.hpp"
#include "nameserver/NameServer.hpp"
#include "scheduler/AdminHost.hpp"
#include "scheduler/AdminUser.hpp"
#include "scheduler/ArchiveToFileRequest.hpp"
#include "scheduler/ArchivalJob.hpp"
#include "scheduler/ArchivalRoute.hpp"
#include "scheduler/DirIterator.hpp"
#include "scheduler/LogicalLibrary.hpp"
#include "scheduler/MockSchedulerDatabase.hpp"
#include "scheduler/RetrievalJob.hpp"
#include "scheduler/RetrieveToFileRequest.hpp"
#include "scheduler/SecurityIdentity.hpp"
#include "scheduler/SqliteColumnNameToIndex.hpp"
#include "scheduler/StorageClass.hpp"
#include "scheduler/Tape.hpp"
#include "scheduler/TapePool.hpp"
#include "scheduler/UserIdentity.hpp"

#include <iostream>
#include <memory>
#include <sstream>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::MockSchedulerDatabase::MockSchedulerDatabase() {
  try {
    if(sqlite3_open(":memory:", &m_dbHandle)) {
      std::ostringstream message;
      message << "SQLite error: Can't open database: " <<
        sqlite3_errmsg(m_dbHandle);
      throw(exception::Exception(message.str()));
    }
    char *zErrMsg = 0;
    if(SQLITE_OK != sqlite3_exec(m_dbHandle, "PRAGMA foreign_keys = ON;", 0, 0,
      &zErrMsg)) {
      std::ostringstream message;
      message << "SqliteDatabase() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(exception::Exception(message.str()));
    }
    createSchema();
  } catch (...) {
    sqlite3_close(m_dbHandle);
    throw;
  }
}

//------------------------------------------------------------------------------
// createSchema
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::createSchema() {  
  char *zErrMsg = 0;
  const int rc = sqlite3_exec(m_dbHandle,
    "CREATE TABLE ARCHIVALROUTE("
      "STORAGECLASS_NAME TEXT,"
      "COPYNB            INTEGER,"
      "TAPEPOOL_NAME     TEXT,"
      "UID               INTEGER,"
      "GID               INTEGER,"
      "CREATIONTIME      INTEGER,"
      "COMMENT           TEXT,"
      "PRIMARY KEY (STORAGECLASS_NAME, COPYNB),"
      "FOREIGN KEY(STORAGECLASS_NAME) REFERENCES STORAGECLASS(NAME),"
      "FOREIGN KEY(TAPEPOOL_NAME)     REFERENCES TAPEPOOL(NAME)"
      ");"
    "CREATE TABLE STORAGECLASS("
      "NAME           TEXT     PRIMARY KEY,"
      "NBCOPIES       INTEGER,"
      "UID            INTEGER,"
      "GID            INTEGER,"
      "CREATIONTIME   INTEGER,"
      "COMMENT        TEXT"
      ");"
    "CREATE TABLE TAPEPOOL("
      "NAME           TEXT     PRIMARY KEY,"
      "NBPARTIALTAPES INTEGER,"
      "UID            INTEGER,"
      "GID            INTEGER,"
      "CREATIONTIME   INTEGER,"
      "COMMENT        TEXT"
      ");"
    "CREATE TABLE TAPE("
      "VID                 TEXT,"
      "LOGICALLIBRARY_NAME TEXT,"
      "TAPEPOOL_NAME       TEXT,"
      "CAPACITY_BYTES      INTEGER,"
      "DATAONTAPE_BYTES    INTEGER,"
      "UID                 INTEGER,"
      "GID                 INTEGER,"
      "CREATIONTIME        INTEGER,"
      "COMMENT             TEXT,"
      "PRIMARY KEY (VID),"
      "FOREIGN KEY (LOGICALLIBRARY_NAME) REFERENCES LOGICALLIBRARY(NAME),"
      "FOREIGN KEY (TAPEPOOL_NAME) REFERENCES TAPEPOOL(NAME)"
      ");"
    "CREATE TABLE LOGICALLIBRARY("
      "NAME           TEXT,"
      "UID            INTEGER,"
      "GID            INTEGER,"
      "CREATIONTIME   INTEGER,"
      "COMMENT        TEXT,"
      "PRIMARY KEY (NAME)"
      ");"
    "CREATE TABLE ADMINUSER("
      "ADMIN_UID      INTEGER,"
      "ADMIN_GID      INTEGER,"
      "UID            INTEGER,"
      "GID            INTEGER,"
      "CREATIONTIME   INTEGER,"
      "COMMENT        TEXT,"
      "PRIMARY KEY (ADMIN_UID,ADMIN_GID)"
      ");"
    "CREATE TABLE ADMINHOST("
      "NAME           TEXT,"
      "UID            INTEGER,"
      "GID            INTEGER,"
      "CREATIONTIME   INTEGER,"
      "COMMENT        TEXT,"
      "PRIMARY KEY (NAME)"
      ");"
    "CREATE TABLE ARCHIVALJOB("
      "STATE          INTEGER,"
      "REMOTEFILE     TEXT,"
      "ARCHIVEFILE    TEXT,"
      "TAPEPOOL_NAME  TEXT,"
      "UID            INTEGER,"
      "GID            INTEGER,"
      "CREATIONTIME   INTEGER,"
      "PRIMARY KEY (ARCHIVEFILE, TAPEPOOL_NAME),"
      "FOREIGN KEY (TAPEPOOL_NAME) REFERENCES TAPEPOOL(NAME)"
      ");"
    "CREATE TABLE RETRIEVALJOB("
      "STATE          INTEGER,"
      "ARCHIVEFILE    TEXT,"
      "REMOTEFILE     TEXT,"
      "VID            TEXT,"
      "UID            INTEGER,"
      "GID            INTEGER,"
      "CREATIONTIME   INTEGER,"
      "PRIMARY KEY (REMOTEFILE, VID),"
      "FOREIGN KEY (VID) REFERENCES TAPE(VID)"
      ");",
    0, 0, &zErrMsg);
  if(SQLITE_OK != rc) {    
      std::ostringstream message;
      message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(exception::Exception(message.str()));
  }
}  

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::MockSchedulerDatabase::~MockSchedulerDatabase() throw() {
  sqlite3_close(m_dbHandle);
}

//------------------------------------------------------------------------------
// queue
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::queue(const ArchiveToDirRequest &rqst,
  const std::list<ArchivalFileTransfer> &fileTransfers) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// queue
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::queue(const ArchiveToFileRequest &rqst,
  const std::list<ArchivalFileTransfer> &fileTransfers) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ARCHIVALJOB(STATE, REMOTEFILE, ARCHIVEFILE,"
    " UID, GID, CREATIONTIME) VALUES(" <<
    (int)cta::ArchivalJobState::PENDING << ",'" << rqst.getRemoteFile() << "','"
    << rqst.getArchiveFile() << "'," << rqst.getUser().user.getUid() << "," <<
    rqst.getUser().user.getGid() << "," << (int)time(NULL) << ");";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// getArchivalJobs
//------------------------------------------------------------------------------
std::map<cta::TapePool, std::list<cta::ArchivalJob> >
  cta::MockSchedulerDatabase::getArchivalJobs(const SecurityIdentity &requester)
  const {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::map<cta::TapePool, std::list<cta::ArchivalJob> > map;
  query << "SELECT STATE, REMOTEFILE, ARCHIVEFILE, TAPEPOOL_NAME, UID, GID,"
    " CREATIONTIME FROM ARCHIVALJOB ORDER BY ARCHIVEFILE;";
  sqlite3_stmt *statement;
  if(SQLITE_OK != sqlite3_prepare(m_dbHandle, query.str().c_str(), -1,
    &statement, 0)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    const std::string tapePoolName = (char *)sqlite3_column_text(statement,
      idx("TAPEPOOL_NAME"));
    const TapePool tapePool = getTapePoolByName(requester, tapePoolName);
    map[tapePool].push_back(ArchivalJob(
      (ArchivalJobState::Enum)sqlite3_column_int(statement,idx("STATE")),
      (char *)sqlite3_column_text(statement,idx("REMOTEFILE")),
      (char *)sqlite3_column_text(statement,idx("ARCHIVEFILE")),
      UserIdentity(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID"))),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return map;
}

//------------------------------------------------------------------------------
// getTapePoolByName
//------------------------------------------------------------------------------
cta::TapePool cta::MockSchedulerDatabase::getTapePoolByName(
  const SecurityIdentity &requester, const std::string &name) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  cta::TapePool pool;
  query << "SELECT NAME, NBPARTIALTAPES, UID, GID, CREATIONTIME, COMMENT"
    " FROM TAPEPOOL WHERE NAME='" << name << "';";
  sqlite3_stmt *statement;
  if(SQLITE_OK != sqlite3_prepare(m_dbHandle, query.str().c_str(), -1,
    &statement, 0)) {
    std::ostringstream message;
    message << __FUNCTION__ <<  " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  switch(sqlite3_step(statement)) {
  case SQLITE_ROW:
    {
      SqliteColumnNameToIndex idx(statement);
      pool = TapePool(
        (char *)sqlite3_column_text(statement,idx("NAME")),
        sqlite3_column_int(statement,idx("NBPARTIALTAPES")),
        UserIdentity(sqlite3_column_int(statement,idx("UID")),
        sqlite3_column_int(statement,idx("GID"))),
        (char *)sqlite3_column_text(statement,idx("COMMENT")),
        time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
      );
    }
    break;
  case SQLITE_DONE:
    {
      std::ostringstream message;
      message << __FUNCTION__ << " - No tape pool found with name: " << name;
      sqlite3_finalize(statement);
      throw(exception::Exception(message.str()));
    }
    break;
  default:
    {
      std::ostringstream message;
      message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      sqlite3_finalize(statement);
      throw(exception::Exception(message.str()));
    }
  }
  sqlite3_finalize(statement);
  return pool;
}

//------------------------------------------------------------------------------
// getArchivalJobs
//------------------------------------------------------------------------------
std::list<cta::ArchivalJob> cta::MockSchedulerDatabase::getArchivalJobs(
  const SecurityIdentity &requester,
  const std::string &tapePoolName) const {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// deleteArchivalJob
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteArchivalJob(
  const SecurityIdentity &requester,
  const std::string &archiveFile) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ARCHIVALJOB WHERE ARCHIVEFILE='" << archiveFile << "';";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Archival job for archive file " << archiveFile <<
      " does not exist";
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// queue
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::queue(const RetrieveToDirRequest &rqst,
  const std::list<RetrievalFileTransfer> &fileTransfers) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// queue
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::queue(const RetrieveToFileRequest &rqst,
  const std::list<RetrievalFileTransfer> &fileTransfers) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO RETRIEVALJOB(STATE, ARCHIVEFILE, REMOTEFILE, UID,"
    " GID, CREATIONTIME) VALUES(" << (int)cta::RetrievalJobState::PENDING <<
    ",'" << rqst.getArchiveFile() << "','" << rqst.getRemoteFile() << "'," <<
    rqst.getUser().user.getUid() << "," << rqst.getUser().user.getGid() <<
    "," << (int)time(NULL) << ");";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// getRetrievalJobs
//------------------------------------------------------------------------------
std::map<cta::Tape, std::list<cta::RetrievalJob> > cta::MockSchedulerDatabase::
  getRetrievalJobs(const SecurityIdentity &requester) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::map<cta::Tape, std::list<cta::RetrievalJob> > map;
  query << "SELECT STATE, ARCHIVEFILE, REMOTEFILE, VID, UID, GID, CREATIONTIME"
    " FROM RETRIEVALJOB ORDER BY REMOTEFILE;";
  sqlite3_stmt *statement;
  if(SQLITE_OK != sqlite3_prepare(m_dbHandle, query.str().c_str(), -1,
    &statement, 0)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    const std::string vid = (char *)sqlite3_column_text(statement,idx("VID"));
    const Tape tape = getTapeByVid(requester, vid);
    map[tape].push_back(RetrievalJob(
      (RetrievalJobState::Enum)sqlite3_column_int(statement,idx("STATE")),
      (char *)sqlite3_column_text(statement,idx("ARCHIVEFILE")),
      (char *)sqlite3_column_text(statement,idx("REMOTEFILE")),
      UserIdentity(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID"))),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return map;
}

//------------------------------------------------------------------------------
// getTapeByVid
//------------------------------------------------------------------------------
cta::Tape cta::MockSchedulerDatabase::getTapeByVid(
  const SecurityIdentity &requester, const std::string &vid) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  cta::Tape tape;
  query << "SELECT VID, LOGICALLIBRARY_NAME, TAPEPOOL_NAME, CAPACITY_BYTES,"
    " DATAONTAPE_BYTES, UID, GID, CREATIONTIME, COMMENT FROM TAPE WHERE VID='"
    << vid << "';";
  sqlite3_stmt *statement;
  if(SQLITE_OK != sqlite3_prepare(m_dbHandle, query.str().c_str(), -1,
    &statement, 0)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  switch(sqlite3_step(statement)) {
  case SQLITE_ROW:
    {
      SqliteColumnNameToIndex idx(statement);
      tape = Tape(
        (char *)sqlite3_column_text(statement,idx("VID")),
        (char *)sqlite3_column_text(statement,idx("LOGICALLIBRARY_NAME")),
        (char *)sqlite3_column_text(statement,idx("TAPEPOOL_NAME")),
        sqlite3_column_int(statement,idx("CAPACITY_BYTES")),
        sqlite3_column_int(statement,idx("DATAONTAPE_BYTES")),
        UserIdentity(sqlite3_column_int(statement,idx("UID")),
        sqlite3_column_int(statement,idx("GID"))),
        (char *)sqlite3_column_text(statement,idx("COMMENT")),
        time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
      );
    }
    break;
  case SQLITE_DONE:
    {
      std::ostringstream message;
      message << "getTapeByVid() - No tape found with vid: " << vid;
      sqlite3_finalize(statement);
      throw(exception::Exception(message.str()));
    }
    break;
  default:
    {
      std::ostringstream message;
      message << "getTapeByVid() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      sqlite3_finalize(statement);
      throw(exception::Exception(message.str()));
    }
  }
  sqlite3_finalize(statement);
  return tape;
}

//------------------------------------------------------------------------------
// getRetrievalJobs
//------------------------------------------------------------------------------
std::list<cta::RetrievalJob> cta::MockSchedulerDatabase::getRetrievalJobs(
  const SecurityIdentity &requester,
  const std::string &vid) const {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}
  
//------------------------------------------------------------------------------
// deleteRetrievalJob
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteRetrievalJob(
  const SecurityIdentity &requester,
  const std::string &remoteFile) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM RETRIEVALJOB WHERE REMOTEFILE='" << remoteFile << "';";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }

  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Retrival job for remote file file " << remoteFile <<
      " does not exist";
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createAdminUser
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::createAdminUser(
  const SecurityIdentity &requester,
  const UserIdentity &user,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ADMINUSER(ADMIN_UID, ADMIN_GID, UID, GID,"
    " CREATIONTIME, COMMENT) VALUES(" << user.getUid() << "," << user.getGid()
    << "," << requester.user.getUid() << "," << requester.user.getGid() << ","
    << (int)time(NULL) << ",'" << comment << "');";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteAdminUser
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteAdminUser(
  const SecurityIdentity &requester,
  const UserIdentity &user) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ADMINUSER WHERE ADMIN_UID=" << user.getUid() <<
    " AND ADMIN_GID=" << user.getGid() <<";";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
      std::ostringstream message;
      message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(exception::Exception(message.str()));
  }
  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Admin user " << user.getUid() << ":" << user.getGid() <<
      " does not exist";
    throw(exception::Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// getAdminUsers
//------------------------------------------------------------------------------
std::list<cta::AdminUser> cta::MockSchedulerDatabase::getAdminUsers(
  const SecurityIdentity &requester) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::AdminUser> list;
  query << "SELECT ADMIN_UID, ADMIN_GID, UID, GID, CREATIONTIME, COMMENT"
    " FROM ADMINUSER ORDER BY ADMIN_UID, ADMIN_GID;";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    const UserIdentity user(sqlite3_column_int(statement,idx("ADMIN_UID")),
      sqlite3_column_int(statement,idx("ADMIN_GID")));
    const UserIdentity creator(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID")));
    list.push_back(AdminUser(
      user,
      creator,
      (char *)sqlite3_column_text(statement,idx("COMMENT")),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return list;
}

//------------------------------------------------------------------------------
// createAdminHost
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::createAdminHost(
  const SecurityIdentity &requester,
  const std::string &hostName,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ADMINHOST(NAME, UID, GID, CREATIONTIME, COMMENT)"
    " VALUES('" << hostName << "',"<< requester.user.getUid() << "," <<
    requester.user.getGid() << "," << (int)time(NULL) << ",'" << comment <<
    "');";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteAdminHost
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteAdminHost(
  const SecurityIdentity &requester,
  const std::string &hostName) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ADMINHOST WHERE NAME='" << hostName << "';";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
      std::ostringstream message;
      message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(exception::Exception(message.str()));
  }

  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Admin host " << hostName << " does not exist";
    throw(exception::Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// getAdminHosts
//------------------------------------------------------------------------------
std::list<cta::AdminHost> cta::MockSchedulerDatabase::getAdminHosts(
  const SecurityIdentity &requester) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::AdminHost> list;
  query << "SELECT NAME, UID, GID, CREATIONTIME, COMMENT"
    " FROM ADMINHOST ORDER BY NAME;";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    const UserIdentity creator(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID")));
    list.push_back(AdminHost(
      (char *)sqlite3_column_text(statement,idx("NAME")),
      creator,
      (char *)sqlite3_column_text(statement,idx("COMMENT")),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return list;
}

//------------------------------------------------------------------------------
// createStorageClass
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::createStorageClass(
  const SecurityIdentity &requester, const std::string &name,
  const uint16_t nbCopies, const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO STORAGECLASS(NAME, NBCOPIES, UID, GID, CREATIONTIME,"
    " COMMENT) VALUES('" << name << "'," << (int)nbCopies << "," <<
    requester.user.getUid() << "," << requester.user.getGid() << "," <<
    (int)time(NULL) << ",'" << comment << "');";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteStorageClass
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteStorageClass(
  const SecurityIdentity &requester,
  const std::string &name) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM STORAGECLASS WHERE NAME='" << name << "';";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }

  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Storage class " << name << " does not exist";
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// getStorageClasses
//------------------------------------------------------------------------------
std::list<cta::StorageClass> cta::MockSchedulerDatabase::getStorageClasses(
  const SecurityIdentity &requester) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::StorageClass> classes;
  query << "SELECT NAME, NBCOPIES, UID, GID, CREATIONTIME, COMMENT FROM"
    " STORAGECLASS ORDER BY NAME;";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    classes.push_back(StorageClass(
      (char *)sqlite3_column_text(statement,idx("NAME")),
      sqlite3_column_int(statement,idx("NBCOPIES")),
      UserIdentity(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID"))),
      (char *)sqlite3_column_text(statement,idx("COMMENT")),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return classes;
}

//------------------------------------------------------------------------------
// createTapePool
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::createTapePool(
  const SecurityIdentity &requester,
  const std::string &name,
  const uint32_t nbPartialTapes,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO TAPEPOOL(NAME, NBPARTIALTAPES, UID, GID, CREATIONTIME,"
    " COMMENT) VALUES('" << name << "'," << (int)nbPartialTapes << "," <<
    requester.user.getUid() << "," << requester.user.getGid() << "," <<
    (int)time(NULL) << ",'" << comment << "');";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteTapePool
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteTapePool(
  const SecurityIdentity &requester,
  const std::string &name) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM TAPEPOOL WHERE NAME='" << name << "';";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }

  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Tape pool " << name << " does not exist";
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// getTapePools
//------------------------------------------------------------------------------
std::list<cta::TapePool> cta::MockSchedulerDatabase::getTapePools(
  const SecurityIdentity &requester) const {

  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::TapePool> pools;
  query << "SELECT NAME, NBPARTIALTAPES, UID, GID, CREATIONTIME, COMMENT FROM"
    " TAPEPOOL ORDER BY NAME;";
  sqlite3_stmt *statement;
  if(SQLITE_OK != sqlite3_prepare(m_dbHandle, query.str().c_str(), -1,
    &statement, 0)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    pools.push_back(TapePool(
      (char *)sqlite3_column_text(statement,idx("NAME")),
      sqlite3_column_int(statement,idx("NBPARTIALTAPES")),
      UserIdentity(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID"))),
      (char *)sqlite3_column_text(statement,idx("COMMENT")),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return pools;
}

//------------------------------------------------------------------------------
// createArchivalRoute
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::createArchivalRoute(
  const SecurityIdentity &requester,
  const std::string &storageClassName,
  const uint16_t copyNb,
  const std::string &tapePoolName,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ARCHIVALROUTE(STORAGECLASS_NAME, COPYNB, TAPEPOOL_NAME,"
    " UID, GID, CREATIONTIME, COMMENT) VALUES('" << storageClassName << "'," <<
    (int)copyNb << ",'" << tapePoolName << "'," << requester.user.getUid() <<
    "," << requester.user.getGid() << "," << (int)time(NULL) << ",'" << comment
    << "');";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteArchivalRoute
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteArchivalRoute(
  const SecurityIdentity &requester,
  const std::string &storageClassName,
  const uint16_t copyNb) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ARCHIVALROUTE WHERE STORAGECLASS_NAME='" <<
    storageClassName << "' AND COPYNB=" << (int)copyNb << ";";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }

  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Archival route for storage class " << storageClassName << 
      " and copy number " << copyNb << " does not exist";
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// getArchivalRoutes
//------------------------------------------------------------------------------
std::list<cta::ArchivalRoute> cta::MockSchedulerDatabase::getArchivalRoutes(
  const SecurityIdentity &requester) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::ArchivalRoute> routes;
  query << "SELECT STORAGECLASS_NAME, COPYNB, TAPEPOOL_NAME, UID, GID,"
    " CREATIONTIME, COMMENT FROM ARCHIVALROUTE ORDER BY STORAGECLASS_NAME,"
    " COPYNB;";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    routes.push_back(ArchivalRoute(
      (char *)sqlite3_column_text(statement,idx("STORAGECLASS_NAME")),
      sqlite3_column_int(statement,idx("COPYNB")),
      (char *)sqlite3_column_text(statement,idx("TAPEPOOL_NAME")),
      UserIdentity(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID"))),
      (char *)sqlite3_column_text(statement,idx("COMMENT")),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return routes;
}

//------------------------------------------------------------------------------
// createLogicalLibrary
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::createLogicalLibrary(
  const SecurityIdentity &requester,
  const std::string &name,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO LOGICALLIBRARY(NAME, UID, GID, CREATIONTIME, COMMENT)"
    " VALUES('" << name << "',"<< requester.user.getUid() << "," <<
    requester.user.getGid() << "," << (int)time(NULL) << ",'" << comment <<
    "');";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteLogicalLibrary
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteLogicalLibrary(
  const SecurityIdentity &requester,
  const std::string &name) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM LOGICALLIBRARY WHERE NAME='" << name << "';";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }

  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Logical library " << name << " does not exist";
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// getLogicalLibraries
//------------------------------------------------------------------------------
std::list<cta::LogicalLibrary> cta::MockSchedulerDatabase::getLogicalLibraries(
  const SecurityIdentity &requester) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::LogicalLibrary> list;
  query << "SELECT NAME, UID, GID, CREATIONTIME, COMMENT"
    " FROM LOGICALLIBRARY ORDER BY NAME;";
  sqlite3_stmt *statement;
  if(SQLITE_OK != sqlite3_prepare(m_dbHandle, query.str().c_str(), -1,
    &statement, 0)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    list.push_back(LogicalLibrary(
      (char *)sqlite3_column_text(statement,idx("NAME")),
      UserIdentity(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID"))),
      (char *)sqlite3_column_text(statement,idx("COMMENT")),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return list;
}

//------------------------------------------------------------------------------
// createTape
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::createTape(
  const SecurityIdentity &requester,
  const std::string &vid,
  const std::string &logicalLibraryName,
  const std::string &tapePoolName,
  const uint64_t capacityInBytes,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO TAPE(VID, LOGICALLIBRARY_NAME, TAPEPOOL_NAME,"
    " CAPACITY_BYTES, DATAONTAPE_BYTES, UID, GID, CREATIONTIME, COMMENT)"
    " VALUES('" << vid << "','" << logicalLibraryName << "','" << tapePoolName
    << "',"<< (long unsigned int)capacityInBytes << ",0," <<
    requester.user.getUid() << "," << requester.user.getGid() << "," <<
    (int)time(NULL) << ",'" << comment << "');";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteTape
//------------------------------------------------------------------------------
void cta::MockSchedulerDatabase::deleteTape(
  const SecurityIdentity &requester,
  const std::string &vid) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM TAPE WHERE NAME='" << vid << "';";
  if(SQLITE_OK != sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0,
    &zErrMsg)) {
    std::ostringstream message;
    message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(exception::Exception(message.str()));
  }

  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 > nbRowsModified) {
    std::ostringstream message;
    message << "Tape " << vid << " does not exist";
    throw(exception::Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// getTapes
//------------------------------------------------------------------------------
std::list<cta::Tape> cta::MockSchedulerDatabase::getTapes(
  const SecurityIdentity &requester) const {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::Tape> tapes;
  query << "SELECT VID, LOGICALLIBRARY_NAME, TAPEPOOL_NAME, CAPACITY_BYTES,"
    " DATAONTAPE_BYTES, UID, GID, CREATIONTIME, COMMENT"
    " FROM TAPE ORDER BY VID;";
  sqlite3_stmt *statement;
  if(SQLITE_OK != sqlite3_prepare(m_dbHandle, query.str().c_str(), -1,
    &statement, 0)) {
    std::ostringstream message;
    message << "selectAllTapes() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_finalize(statement);
    throw(exception::Exception(message.str()));
  }
  while(SQLITE_ROW == sqlite3_step(statement)) {
    SqliteColumnNameToIndex idx(statement);
    tapes.push_back(Tape(
      (char *)sqlite3_column_text(statement,idx("VID")),
      (char *)sqlite3_column_text(statement,idx("LOGICALLIBRARY_NAME")),
      (char *)sqlite3_column_text(statement,idx("TAPEPOOL_NAME")),
      sqlite3_column_int(statement,idx("CAPACITY_BYTES")),
      sqlite3_column_int(statement,idx("DATAONTAPE_BYTES")),
      UserIdentity(sqlite3_column_int(statement,idx("UID")),
      sqlite3_column_int(statement,idx("GID"))),
      (char *)sqlite3_column_text(statement,idx("COMMENT")),
      time_t(sqlite3_column_int(statement,idx("CREATIONTIME")))
    ));
  }
  sqlite3_finalize(statement);
  return tapes;
}

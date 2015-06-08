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
#include "middletier/SQLite/SqliteColumnNameToIndex.hpp"
#include "middletier/SQLite/SqliteDatabase.hpp"
#include "common/Utils.hpp"

#include <iostream>
#include <memory>
#include <sstream>
using cta::exception::Exception;

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::SqliteDatabase::SqliteDatabase() {
  try {
    int rc = sqlite3_open(":memory:", &m_dbHandle);
    if(rc) {
      std::ostringstream message;
      message << "SQLite error: Can't open database: " <<
        sqlite3_errmsg(m_dbHandle);
      throw(Exception(message.str()));
    }
    char *zErrMsg = 0;
    rc = sqlite3_exec(m_dbHandle, "PRAGMA foreign_keys = ON;", 0, 0, &zErrMsg);
    if(rc!=SQLITE_OK) {    
      std::ostringstream message;
      message << "SqliteDatabase() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
    }
    createSchema();
  } catch (...) {
    sqlite3_close(m_dbHandle);
    throw;
  }
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::SqliteDatabase::~SqliteDatabase() throw() {
  sqlite3_close(m_dbHandle);
}

//------------------------------------------------------------------------------
// createSchema
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createSchema() {  
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle,
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
          "CREATE TABLE ARCHIVETOFILEREQUEST("
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
            "SRCPATH        TEXT,"
            "DSTURL         TEXT,"
            "VID            TEXT,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "PRIMARY KEY (DSTURL, VID),"
            "FOREIGN KEY (VID) REFERENCES TAPE(VID)"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createRetrievalJobTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}  
  
//------------------------------------------------------------------------------
// insertTape
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertTape(const SecurityIdentity &requester,
  const std::string &vid, const std::string &logicalLibraryName,
  const std::string &tapePoolName, const uint64_t capacityInBytes,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO TAPE(VID, LOGICALLIBRARY_NAME, TAPEPOOL_NAME,"
    " CAPACITY_BYTES, DATAONTAPE_BYTES, UID, GID, CREATIONTIME, COMMENT)"
    " VALUES('" << vid << "','" << logicalLibraryName << "','" << tapePoolName
    << "',"<< (long unsigned int)capacityInBytes << ",0," <<
    requester.getUser().getUid() << "," << requester.getUser().getGid() << ","
    << (int)time(NULL) << ",'" << comment << "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertTape() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// insertAdminUser
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertAdminUser(const SecurityIdentity &requester,
  const UserIdentity &user, const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ADMINUSER(ADMIN_UID, ADMIN_GID, UID, GID,"
    " CREATIONTIME, COMMENT) VALUES(" << user.getUid() << "," << user.getGid()
    << "," << requester.getUser().getUid() << "," <<
    requester.getUser().getGid() << "," << (int)time(NULL) << ",'" << comment <<
    "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertAdminUser() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// insertAdminHost
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertAdminHost(const SecurityIdentity &requester,
  const std::string &hostName, const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ADMINHOST(NAME, UID, GID, CREATIONTIME, COMMENT)"
    " VALUES('" << hostName << "',"<< requester.getUser().getUid() << "," <<
    requester.getUser().getGid() << "," << (int)time(NULL) << ",'" << comment <<
    "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertAdminHost() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// insertArchiveToFileRequest
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertArchiveToFileRequest(
  const SecurityIdentity &requester,
  const std::string &tapepool,
  const std::string &remoteFile,
  const std::string &archiveFile) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ARCHIVETOFILEREQUEST(REMOTEFILE, ARCHIVEFILE,"
    " TAPEPOOL_NAME, UID, GID, CREATIONTIME) VALUES('" << remoteFile << "','" <<
    archiveFile << "','" << tapepool << "'," << requester.getUser().getUid() <<
    "," << requester.getUser().getGid() << "," <<
    (int)time(NULL) << ");";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      std::cout << message.str() << std::endl;
      throw(Exception(message.str()));
  }
  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 >= nbRowsModified) {
    std::ostringstream message;
    message << "Archive request for archive file " << archiveFile <<
      " already exixts";
    throw(exception::Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// insertRetrievalJob
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertRetrievalJob(const SecurityIdentity &requester,
  const std::string &vid, const std::string &srcPath,
  const std::string &dstUrl) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO RETRIEVALJOB(STATE, SRCPATH, DSTURL, VID, UID, GID,"
    " CREATIONTIME) VALUES(" << (int)cta::RetrievalJobState::PENDING << ",'" <<
    srcPath << "','" << dstUrl << "','" << vid << "'," <<
    requester.getUser().getUid() << "," << requester.getUser().getGid() << ","
    << (int)time(NULL) << ");";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertRetrievalJob() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// insertLogicalLibrary
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertLogicalLibrary(
  const SecurityIdentity &requester, const std::string &name,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO LOGICALLIBRARY(NAME, UID, GID, CREATIONTIME, COMMENT)"
    " VALUES('" << name << "',"<< requester.getUser().getUid() << "," <<
    requester.getUser().getGid() << "," << (int)time(NULL) << ",'" << comment <<
    "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertLogicalLibrary() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// insertTapePool
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertTapePool(const SecurityIdentity &requester,
  const std::string &name, const uint32_t nbPartialTapes,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO TAPEPOOL(NAME, NBPARTIALTAPES, UID, GID, CREATIONTIME,"
    " COMMENT) VALUES('" << name << "'," << (int)nbPartialTapes << "," <<
    requester.getUser().getUid() << "," << requester.getUser().getGid() << ","
    << (int)time(NULL) << ",'" << comment << "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
    std::ostringstream message;
    message << "insertTapePool() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// insertStorageClass
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertStorageClass(const SecurityIdentity &requester,
  const std::string &name, const uint16_t nbCopies,
  const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO STORAGECLASS(NAME, NBCOPIES, UID, GID, CREATIONTIME,"
    " COMMENT) VALUES('" << name << "'," << (int)nbCopies << "," <<
    requester.getUser().getUid() << "," << requester.getUser().getGid() << ","
    << (int)time(NULL) << ",'" << comment << "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertStorageClass() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// insertArchivalRoute
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertArchivalRoute(const SecurityIdentity &requester,
  const std::string &storageClassName, const uint16_t copyNb,
  const std::string &tapePoolName, const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ARCHIVALROUTE(STORAGECLASS_NAME, COPYNB, TAPEPOOL_NAME,"
    " UID, GID, CREATIONTIME, COMMENT) VALUES('" << storageClassName << "'," <<
    (int)copyNb << ",'" << tapePoolName << "'," << requester.getUser().getUid()
    << "," << requester.getUser().getGid() << "," << (int)time(NULL) << ",'" <<
    comment << "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertArchivalRoute() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteTapePool
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteTapePool(const SecurityIdentity &requester,
  const std::string &name) {
  checkTapePoolExists(name);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM TAPEPOOL WHERE NAME='" << name << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteTapePool() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteStorageClass
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteStorageClass(const SecurityIdentity &requester,
  const std::string &name) {
  checkStorageClassExists(name);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM STORAGECLASS WHERE NAME='" << name << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteStorageClass() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteArchivalRoute
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteArchivalRoute(const SecurityIdentity &requester,
  const std::string &storageClassName, const uint16_t copyNb) {
  checkArchivalRouteExists(storageClassName, copyNb);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ARCHIVALROUTE WHERE STORAGECLASS_NAME='" <<
    storageClassName << "' AND COPYNB=" << (int)copyNb << ";";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteMigrationRoute() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteTape
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteTape(const SecurityIdentity &requester,
  const std::string &vid) {
  checkTapeExists(vid);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM TAPE WHERE VID='" << vid << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteTape() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// deleteAdminUser
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteAdminUser(const SecurityIdentity &requester,
  const UserIdentity &user) {
  checkAdminUserExists(user);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ADMINUSER WHERE ADMIN_UID=" << user.getUid() <<
    " AND ADMIN_GID=" << user.getGid() <<";";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteAdminUser() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// deleteAdminHost
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteAdminHost(const SecurityIdentity &requester,
  const std::string &hostName) {
  checkAdminHostExists(hostName);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ADMINHOST WHERE NAME='" << hostName << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteAdminHost() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// deleteArchiveToFileRequest
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteArchiveToFileRequest(
  const SecurityIdentity &requester, const std::string &archiveFile) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ARCHIVALTOFILEREQUEST WHERE ARCHIVEFILE='" <<
    archiveFile << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << __FUNCTION__ << " - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  const int nbRowsModified = sqlite3_changes(m_dbHandle);
  if(0 >= nbRowsModified) {
    std::ostringstream message;
    message << "Archive to file request for archive file " << archiveFile <<
      " does not exist";
    throw(exception::Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// deleteRetrievalJob
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteRetrievalJob(const SecurityIdentity &requester,
  const std::string &dstUrl) {
  checkRetrievalJobExists(dstUrl);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM RETRIEVALJOB WHERE DSTURL='" << dstUrl << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteRetrievalJob() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}
  
//------------------------------------------------------------------------------
// deleteLogicalLibrary
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteLogicalLibrary(
  const SecurityIdentity &requester, const std::string &name) {
  checkLogicalLibraryExists(name);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM LOGICALLIBRARY WHERE NAME='" << name << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteLogicalLibrary() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

struct SQLiteStatementDeleter{
  void operator() (sqlite3_stmt * ptr) {sqlite3_finalize(ptr);}
};

//------------------------------------------------------------------------------
// checkTapeExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkTapeExists(const std::string &vid){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM TAPE WHERE VID='" << vid << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(SQLITE_OK != rc){    
      std::ostringstream message;
      message << "checkTapeExists() - SQLite error: " << sqlite3_errmsg(m_dbHandle);
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "TAPE: " << vid << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkTapeExists() - SQLite error: " << sqlite3_errmsg(m_dbHandle);
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}
  
//------------------------------------------------------------------------------
// checkAdminUserExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkAdminUserExists(const cta::UserIdentity &user){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM ADMINUSER WHERE ADMIN_UID=" << user.getUid() <<
    " AND ADMIN_GID=" << user.getGid() <<";";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkAdminUserExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "ADMINUSER: " << user.getUid() << ":" << user.getGid() <<
      " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkAdminUserExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}
  
//------------------------------------------------------------------------------
// checkAdminHostExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkAdminHostExists(const std::string &hostName) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM ADMINHOST WHERE NAME='" << hostName << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkAdminHostExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "ADMINHOST: " << hostName << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkAdminHostExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}
  
//------------------------------------------------------------------------------
// checkRetrievalJobExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkRetrievalJobExists(const std::string &dstUrl){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM RETRIEVALJOB WHERE DSTURL='" << dstUrl << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkRetrievalJobExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "RETRIEVALJOB: " << dstUrl << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkRetrievalJobExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}
  
//------------------------------------------------------------------------------
// checkLogicalLibraryExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkLogicalLibraryExists(const std::string &name){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM LOGICALLIBRARY WHERE NAME='" << name << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkLogicalLibraryExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "LOGICALLIBRARY: " << name << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkLogicalLibraryExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}

//------------------------------------------------------------------------------
// checkTapePoolExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkTapePoolExists(const std::string &name){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM TAPEPOOL WHERE NAME='" << name << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkTapePoolExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "TAPEPOOL: " << name << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkTapePoolExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}
 
//------------------------------------------------------------------------------
// checkStorageClassExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkStorageClassExists(const std::string &name){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM STORAGECLASS WHERE NAME='" << name << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkStorageClassExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "STORAGECLASS: " << name << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkStorageClassExists() - SQLite error: " << res << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}
 
//------------------------------------------------------------------------------
// checkArchivalRouteExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkArchivalRouteExists(const std::string &name,
  const uint16_t copyNb){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM ARCHIVALROUTE WHERE STORAGECLASS_NAME='" << name <<
    "' AND COPYNB=" << (int)copyNb << ";";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkArchivalRouteExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "ARCHIVALROUTE: " << name << " with COPYNB: " << (int)copyNb <<
      " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkArchivalRouteExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}

//------------------------------------------------------------------------------
// selectAllTapePools
//------------------------------------------------------------------------------
std::list<cta::TapePool> cta::SqliteDatabase::selectAllTapePools(
  const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::TapePool> pools;
  query << "SELECT NAME, NBPARTIALTAPES, UID, GID, CREATIONTIME, COMMENT FROM"
    " TAPEPOOL ORDER BY NAME;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllTapePools() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    pools.push_back(cta::TapePool(
            std::string((char *)sqlite3_column_text(statement.get(),idx("NAME"))),
            sqlite3_column_int(statement.get(),idx("NBPARTIALTAPES")),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),
            sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      ));
  }
  return pools;
}

//------------------------------------------------------------------------------
// selectAllStorageClasses
//------------------------------------------------------------------------------
std::list<cta::StorageClass> cta::SqliteDatabase::selectAllStorageClasses(
  const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::StorageClass> classes;
  query << "SELECT NAME, NBCOPIES, UID, GID, CREATIONTIME, COMMENT FROM"
    " STORAGECLASS ORDER BY NAME;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllStorageClasses() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    classes.push_back(cta::StorageClass(
            std::string((char *)sqlite3_column_text(statement.get(),idx("NAME"))),
            sqlite3_column_int(statement.get(),idx("NBCOPIES")),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),
            sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      ));
  } 
  return classes;
}

//------------------------------------------------------------------------------
// selectAllArchivalRoutes
//------------------------------------------------------------------------------
std::list<cta::ArchivalRoute>  cta::SqliteDatabase::selectAllArchivalRoutes(
  const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::ArchivalRoute> routes;
  query << "SELECT STORAGECLASS_NAME, COPYNB, TAPEPOOL_NAME, UID, GID,"
    " CREATIONTIME, COMMENT FROM ARCHIVALROUTE ORDER BY STORAGECLASS_NAME,"
    " COPYNB;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
    std::ostringstream message;
    message << "selectAllArchivalRoutes() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    routes.push_back(cta::ArchivalRoute(
            std::string((char *)sqlite3_column_text(statement.get(),idx("STORAGECLASS_NAME"))),
            sqlite3_column_int(statement.get(),idx("COPYNB")),
            std::string((char *)sqlite3_column_text(statement.get(),idx("TAPEPOOL_NAME"))),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),
            sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      ));
  }
  return routes;
}

//------------------------------------------------------------------------------
// getArchivalRouteOfStorageClass
//------------------------------------------------------------------------------
cta::ArchivalRoute cta::SqliteDatabase::getArchivalRouteOfStorageClass(
  const SecurityIdentity &requester, const std::string &storageClassName,
  const uint16_t copyNb) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT STORAGECLASS_NAME, COPYNB, TAPEPOOL_NAME, UID, GID,"
    " CREATIONTIME, COMMENT FROM ARCHIVALROUTE WHERE STORAGECLASS_NAME='" <<
    storageClassName <<"' AND COPYNB="<< (int)copyNb <<";";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "getArchivalRouteOfStorageClass() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  cta::ArchivalRoute route;
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    route = cta::ArchivalRoute(
            std::string((char *)sqlite3_column_text(statement.get(),idx("STORAGECLASS_NAME"))),
            sqlite3_column_int(statement.get(),idx("COPYNB")),
            std::string((char *)sqlite3_column_text(statement.get(),idx("TAPEPOOL_NAME"))),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),
            sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      );
  }
  else if(res==SQLITE_DONE) {
    std::ostringstream message;
    message << "getArchivalRouteOfStorageClass() - No archive route found for storage class: " << storageClassName << " and copynb: "<< (int)copyNb;
    throw(Exception(message.str()));
  }
  else {
    std::ostringstream message;
    message << "getArchivalRouteOfStorageClass() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));
  }
  return route;
}

//------------------------------------------------------------------------------
// selectAllTapes
//------------------------------------------------------------------------------
std::list<cta::Tape> cta::SqliteDatabase::selectAllTapes(const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::Tape> tapes;
  query << "SELECT VID, LOGICALLIBRARY_NAME, TAPEPOOL_NAME, CAPACITY_BYTES, DATAONTAPE_BYTES, UID, GID, CREATIONTIME, COMMENT FROM TAPE ORDER BY VID;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllTapes() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    tapes.push_back(cta::Tape(
            std::string((char *)sqlite3_column_text(statement.get(),idx("VID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("LOGICALLIBRARY_NAME"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("TAPEPOOL_NAME"))),
            sqlite3_column_int(statement.get(),idx("CAPACITY_BYTES")),
            sqlite3_column_int(statement.get(),idx("DATAONTAPE_BYTES")),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),
            sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      ));
  }
  return tapes;
}

//------------------------------------------------------------------------------
// selectAllAdminUsers
//------------------------------------------------------------------------------
std::list<cta::AdminUser> cta::SqliteDatabase::selectAllAdminUsers(const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::AdminUser> list;
  query << "SELECT ADMIN_UID, ADMIN_GID, UID, GID, CREATIONTIME, COMMENT FROM ADMINUSER ORDER BY ADMIN_UID, ADMIN_GID;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllAdminUsers() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    const UserIdentity user(sqlite3_column_int(statement.get(),idx("ADMIN_UID")),
      sqlite3_column_int(statement.get(),idx("ADMIN_GID")));
    const UserIdentity creator(sqlite3_column_int(statement.get(),idx("UID")),
            sqlite3_column_int(statement.get(),idx("GID")));
    list.push_back(cta::AdminUser(
      user,
      creator,
      std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
      time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
    ));
  }
  return list;
}

//------------------------------------------------------------------------------
// selectAllAdminHosts
//------------------------------------------------------------------------------
std::list<cta::AdminHost> cta::SqliteDatabase::selectAllAdminHosts(const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::AdminHost> list;
  query << "SELECT NAME, UID, GID, CREATIONTIME, COMMENT FROM ADMINHOST ORDER BY NAME;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllAdminHosts() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    const UserIdentity creator(sqlite3_column_int(statement.get(),idx("UID")),sqlite3_column_int(statement.get(),idx("GID")));
    list.push_back(cta::AdminHost(
      std::string((char *)sqlite3_column_text(statement.get(),idx("NAME"))),
      creator,
      std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
      time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
    ));
  }
  return list;
}
  
//------------------------------------------------------------------------------
// selectAllArchiveToFileRequests
//------------------------------------------------------------------------------
std::map<cta::TapePool, std::list<cta::ArchiveToFileRequest> >
  cta::SqliteDatabase::selectAllArchiveToFileRequests(
  const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::map<cta::TapePool, std::list<cta::ArchiveToFileRequest> > map;
  query << "SELECT REMOTEFILE, ARCHIVEFILE, TAPEPOOL_NAME, UID, GID,"
    " CREATIONTIME FROM ARCHIVETOFILEREQUEST ORDER BY ARCHIVEFILE;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllArchiveToFileRequests() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    const SqliteColumnNameToIndex idx(statement.get());
    const std::string remoteFile = (char *)sqlite3_column_text(statement.get(),
      idx("REMOTEFILE"));
    const std::string archiveFile = (char *)sqlite3_column_text(statement.get(),
      idx("REMOTEFILE"));
    const std::string tapePoolName = (char *)sqlite3_column_text(
      statement.get(),idx("TAPEPOOL_NAME"));
    const TapePool tapePool = getTapePoolByName(requester, tapePoolName);
    const std::map<uint16_t, std::string> copyNbToTapePoolMap;
    const uint16_t createUid = sqlite3_column_int(statement.get(), idx("UID"));
    const uint16_t createGid = sqlite3_column_int(statement.get(), idx("GID"));
    const UserIdentity creator(createUid, createGid);
    const std::string creatorHost = "creator_host";
    const SecurityIdentity creatorAndHost(creator, creatorHost);
    const uint64_t priority = 1;
    map[tapePool].push_back(ArchiveToFileRequest(
      remoteFile,
      archiveFile,
      copyNbToTapePoolMap,
      priority,
      creatorAndHost,
      time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
    ));
  }
  return map;
}

//------------------------------------------------------------------------------
// getTapePoolByName
//------------------------------------------------------------------------------
cta::TapePool cta::SqliteDatabase::getTapePoolByName(const SecurityIdentity &requester, const std::string &name) {
  char *zErrMsg = 0;
  std::ostringstream query;
  cta::TapePool pool;
  query << "SELECT NAME, NBPARTIALTAPES, UID, GID, CREATIONTIME, COMMENT FROM TAPEPOOL WHERE NAME='" << name << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "getTapePoolByName() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    pool = cta::TapePool(
            std::string((char *)sqlite3_column_text(statement.get(),idx("NAME"))),
            sqlite3_column_int(statement.get(),idx("NBPARTIALTAPES")),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      );
  }
  else if(res==SQLITE_DONE) {    
    std::ostringstream message;
    message << "getTapePoolByName() - No tape pool found with name: " << name;
    throw(Exception(message.str()));
  }
  else {    
    std::ostringstream message;
    message << "getTapePoolByName() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));
  }
  return pool;
}

//------------------------------------------------------------------------------
// getStorageClassByName
//------------------------------------------------------------------------------
cta::StorageClass cta::SqliteDatabase::getStorageClassByName(const SecurityIdentity &requester, const std::string &name) {
  char *zErrMsg = 0;
  std::ostringstream query;
  cta::StorageClass stgClass;
  query << "SELECT NAME, NBCOPIES, UID, GID, CREATIONTIME, COMMENT FROM STORAGECLASS WHERE NAME='" << name << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "getStorageClassByName() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    stgClass = cta::StorageClass(
            std::string((char *)sqlite3_column_text(statement.get(),idx("NAME"))),
            sqlite3_column_int(statement.get(),idx("NBCOPIES")),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      );
  }
  else if(res==SQLITE_DONE) {    
    std::ostringstream message;
    message << "getStorageClassByName() - No storage class found with name: " << name;
    throw(Exception(message.str()));
  }
  else {    
    std::ostringstream message;
    message << "getStorageClassByName() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));
  }
  return stgClass;
}

//------------------------------------------------------------------------------
// getTapeByVid
//------------------------------------------------------------------------------
cta::Tape cta::SqliteDatabase::getTapeByVid(const SecurityIdentity &requester, const std::string &vid) {
  char *zErrMsg = 0;
  std::ostringstream query;
  cta::Tape tape;
  query << "SELECT VID, LOGICALLIBRARY_NAME, TAPEPOOL_NAME, CAPACITY_BYTES, DATAONTAPE_BYTES, UID, GID, CREATIONTIME, COMMENT FROM TAPE WHERE VID='" << vid << "';";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){
    std::ostringstream message;
    message << "getTapeByVid() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement.get());
  if(res==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    tape = cta::Tape(
            std::string((char *)sqlite3_column_text(statement.get(),idx("VID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("LOGICALLIBRARY_NAME"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("TAPEPOOL_NAME"))),
            sqlite3_column_int(statement.get(),idx("CAPACITY_BYTES")),
            sqlite3_column_int(statement.get(),idx("DATAONTAPE_BYTES")),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),
            sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT")),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME"))))
      );
  }
  else if(res==SQLITE_DONE) {    
    std::ostringstream message;
    message << "getTapeByVid() - No tape found with vid: " << vid;
    throw(Exception(message.str()));
  }
  else {    
    std::ostringstream message;
    message << "getTapeByVid() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));
  }
  return tape;
}

//------------------------------------------------------------------------------
// selectAllRetrievalJobs
//------------------------------------------------------------------------------
std::map<cta::Tape, std::list<cta::RetrievalJob> > cta::SqliteDatabase::selectAllRetrievalJobs(const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::map<cta::Tape, std::list<cta::RetrievalJob> > map;
  query << "SELECT STATE, SRCPATH, DSTURL, VID, UID, GID, CREATIONTIME FROM RETRIEVALJOB ORDER BY DSTURL;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllRetrievalJobs() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    map[getTapeByVid(requester, std::string((char *)sqlite3_column_text(statement.get(),idx("VID"))))].push_back(cta::RetrievalJob(
            (cta::RetrievalJobState::Enum)sqlite3_column_int(statement.get(),idx("STATE")),
            std::string((char *)sqlite3_column_text(statement.get(),idx("SRCPATH"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("DSTURL"))),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),sqlite3_column_int(statement.get(),idx("GID"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      ));
  }
  return map;
}

//------------------------------------------------------------------------------
// selectAllLogicalLibraries
//------------------------------------------------------------------------------
std::list<cta::LogicalLibrary> cta::SqliteDatabase::selectAllLogicalLibraries(const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::LogicalLibrary> list;
  query << "SELECT NAME, UID, GID, CREATIONTIME, COMMENT FROM LOGICALLIBRARY ORDER BY NAME;";
  sqlite3_stmt *s = NULL;
  const int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &s, 0 );
  std::unique_ptr<sqlite3_stmt, SQLiteStatementDeleter> statement(s);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllLogicalLibraries() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement.get())==SQLITE_ROW) {
    SqliteColumnNameToIndex idx(statement.get());
    list.push_back(cta::LogicalLibrary(
            std::string((char *)sqlite3_column_text(statement.get(),idx("NAME"))),
            cta::UserIdentity(sqlite3_column_int(statement.get(),idx("UID")),sqlite3_column_int(statement.get(),idx("GID"))),
            std::string((char *)sqlite3_column_text(statement.get(),idx("COMMENT"))),
            time_t(sqlite3_column_int(statement.get(),idx("CREATIONTIME")))
      ));
  }
  return list;
}

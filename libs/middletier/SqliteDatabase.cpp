#include "Exception.hpp"
#include "SqliteDatabase.hpp"
#include "Utils.hpp"

#include <iostream>
#include <memory>
#include <sstream>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::SqliteDatabase::SqliteDatabase() {
  try {
    int rc = sqlite3_open(":memory:", &m_dbHandle);
    if(rc) {
      std::ostringstream message;
      message << "SQLite error: Can't open database: " << sqlite3_errmsg(m_dbHandle);
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
// createArchiveRouteTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createArchiveRouteTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE ARCHIVEROUTE("
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
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createArchiveRouteTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createStorageClassTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createStorageClassTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE STORAGECLASS("
            "NAME           TEXT     PRIMARY KEY,"
            "NBCOPIES       INTEGER,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "COMMENT        TEXT"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createStorageClassTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createTapePoolTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createTapePoolTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE TAPEPOOL("
            "NAME           TEXT     PRIMARY KEY,"
            "NBDRIVES       INTEGER,"
            "NBPARTIALTAPES INTEGER,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "COMMENT        TEXT"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createTapePoolTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createDirectoryTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createDirectoryTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE DIRECTORY("
            "PATH              TEXT     PRIMARY KEY,"
            "STORAGECLASS_NAME TEXT,"
            "UID               INTEGER,"
            "GID               INTEGER,"
            "CREATIONTIME      INTEGER,"
            "MODE              INTEGER,"
            "FOREIGN KEY (STORAGECLASS_NAME) REFERENCES STORAGECLASS(NAME)"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createDirectoryTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  std::ostringstream query;
  query << "INSERT INTO DIRECTORY VALUES('/',NULL,0,0," << (int)time(NULL) << ",0);";
  rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createDirectoryTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createFileTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createFileTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE FILE("
            "PATH           TEXT,"
            "NAME           TEXT,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "MODE           INTEGER,"
            "PRIMARY KEY (PATH, NAME),"
            "FOREIGN KEY (PATH) REFERENCES DIRECTORY(PATH)"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createFileTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createTapeTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createTapeTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
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
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createTapeTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createLogicalLibraryTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createLogicalLibraryTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE LOGICALLIBRARY("
            "NAME           TEXT,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "COMMENT        TEXT,"
            "PRIMARY KEY (NAME)"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createLogicalLibraryTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createAdminUserTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createAdminUserTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE ADMINUSER("
            "USER           TEXT,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "COMMENT        TEXT,"
            "PRIMARY KEY (USER)"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createAdminUserTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createAdminHostTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createAdminHostTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE ADMINHOST("
            "NAME           TEXT,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "COMMENT        TEXT,"
            "PRIMARY KEY (NAME)"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createAdminHostTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createArchivalJobTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createArchivalJobTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE ARCHIVALJOB("
            "STATE          INTEGER,"
            "SRCURL         TEXT,"
            "DSTPATH        TEXT,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "PRIMARY KEY (DSTPATH)"
            ");",
          0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "createArchivalJobTable() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// createRetrievalJobTable
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createRetrievalJobTable() {
  char *zErrMsg = 0;
  int rc = sqlite3_exec(m_dbHandle, 
          "CREATE TABLE RETRIEVALJOB("
            "STATE          INTEGER,"
            "SRCPATH        TEXT,"
            "DSTURL         TEXT,"
            "UID            INTEGER,"
            "GID            INTEGER,"
            "CREATIONTIME   INTEGER,"
            "PRIMARY KEY (SRCPATH, DSTURL)"
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
// createSchema
//------------------------------------------------------------------------------
void cta::SqliteDatabase::createSchema() {
  createStorageClassTable();
  createTapePoolTable();
  createArchiveRouteTable();
  createDirectoryTable();
  createFileTable();
  createLogicalLibraryTable();
  createTapeTable();
  createAdminUserTable();  
  createAdminHostTable();
  createArchivalJobTable();
  createRetrievalJobTable();
}
  
//------------------------------------------------------------------------------
// insertFile
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertFile(const SecurityIdentity &requester, const std::string &pathname, const uint16_t mode) {
  cta::Utils::checkAbsolutePathSyntax(pathname);
  std::string path = cta::Utils::getEnclosingDirPath(pathname);
  std::string name = cta::Utils::getEnclosedName(pathname);
  getDirectoryStorageClass(requester, path); //just check if the path exists
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO FILE VALUES('" << path << "','" << name << "',"<< requester.user.getUid() << "," << requester.user.getGid() << "," << (int)time(NULL) << "," << (int)mode << ");";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertFile() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// checkFileExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkFileExists(const std::string &path, const std::string &name){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM FILE WHERE NAME='" << name << "' AND PATH='" << path << "';";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkFileExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement);
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "FILE: " << path << "/" << name << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkFileExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }  
}

//------------------------------------------------------------------------------
// deleteFile
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteFile(const SecurityIdentity &requester, const std::string &pathname) {
  cta::Utils::checkAbsolutePathSyntax(pathname);
  std::string path = cta::Utils::getEnclosingDirPath(pathname);
  std::string name = cta::Utils::getEnclosedName(pathname);
  checkFileExists(path, name);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM FILE WHERE NAME='" << name << "' AND PATH='" << path << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteFile() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// getDirectoryStorageClass
//------------------------------------------------------------------------------
std::string cta::SqliteDatabase::getDirectoryStorageClass(const SecurityIdentity &requester, const std::string &path){
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::TapePool> pools;
  query << "SELECT STORAGECLASS_NAME FROM DIRECTORY WHERE PATH='" << path << "';";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "getDirectoryStorageClass() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement);
  if(res==SQLITE_ROW) {
    return std::string((char *)sqlite3_column_text(statement,0));
  }
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "PATH: " << path << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "getDirectoryStorageClass() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}

//------------------------------------------------------------------------------
// setDirectoryStorageClass
//------------------------------------------------------------------------------
void cta::SqliteDatabase::setDirectoryStorageClass(const SecurityIdentity &requester, const std::string &path, const std::string &storageClassName){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "UPDATE DIRECTORY SET STORAGECLASS_NAME='" << storageClassName << "' WHERE PATH='" << path << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "setDirectoryStorageClass() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// clearDirectoryStorageClass
//------------------------------------------------------------------------------
void cta::SqliteDatabase::clearDirectoryStorageClass(const SecurityIdentity &requester, const std::string &path){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "UPDATE DIRECTORY SET STORAGECLASS_NAME=NULL WHERE PATH='" << path << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "clearDirectoryStorageClass() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// checkDirectoryExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkDirectoryExists(const std::string &path){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM DIRECTORY WHERE PATH='" << path << "';";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkDirectoryExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement);
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "DIRECTORY: " << path << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkDirectoryExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }  
}

//------------------------------------------------------------------------------
// checkDirectoryContainsNoDirectories
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkDirectoryContainsNoDirectories(const std::string &path){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM DIRECTORY WHERE PATH LIKE '" << path << "%';";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkDirectoryContainsNoDirectories() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  sqlite3_step(statement); // we will have at least one match: the directory itself. so we skip the first result!
  int res = sqlite3_step(statement);
  if(res==SQLITE_ROW) {
    std::ostringstream message;
    message << "DIRECTORY " << path << " not empty: contains at least one directory";
    throw(Exception(message.str()));
  } 
  else if(res==SQLITE_DONE){ //OK
  }
  else {
    std::ostringstream message;
    message << "checkDirectoryContainsNoDirectories() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}

//------------------------------------------------------------------------------
// checkDirectoryContainsNoFiles
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkDirectoryContainsNoFiles(const std::string &path){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM FILE WHERE PATH='" << path << "%';";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkDirectoryContainsNoFiles() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement);
  if(res==SQLITE_ROW) {
    std::ostringstream message;
    message << "DIRECTORY " << path << " not empty: contains at least one file";
    throw(Exception(message.str()));
  } 
  else if(res==SQLITE_DONE){ //OK
  }
  else {
    std::ostringstream message;
    message << "checkDirectoryContainsNoFiles() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }
}

//------------------------------------------------------------------------------
// deleteDirectory
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteDirectory(const SecurityIdentity &requester, const std::string &pathname) {
  cta::Utils::checkAbsolutePathSyntax(pathname);
  checkDirectoryExists(pathname);
  checkDirectoryContainsNoDirectories(pathname);
  checkDirectoryContainsNoFiles(pathname);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM DIRECTORY WHERE PATH='" << pathname << "';";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "deleteDirectory() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// insertDirectory
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertDirectory(const SecurityIdentity &requester, const std::string &pathname, const uint16_t mode) {
  cta::Utils::checkAbsolutePathSyntax(pathname);
  std::string path = cta::Utils::getEnclosingDirPath(pathname);
  std::string name = cta::Utils::getEnclosedName(pathname);
  std::string storageClass = getDirectoryStorageClass(requester, path);
  char *zErrMsg = 0;
  std::ostringstream query;
  if(storageClass!="") {
    query << "INSERT INTO DIRECTORY VALUES('" << pathname << "','" << storageClass << "'," << requester.user.getUid() << "," << requester.user.getGid() << "," << (int)time(NULL) << "," << (int)mode << ");";
  }
  else {
    query << "INSERT INTO DIRECTORY VALUES('" << pathname << "',NULL," << requester.user.getUid() << "," << requester.user.getGid() << "," << (int)mode << ");";
  }
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
    std::ostringstream message;
    message << "insertDirectory() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// insertTapePool
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertTapePool(const SecurityIdentity &requester, const std::string &name, const uint16_t nbDrives, const uint32_t nbPartialTapes, const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO TAPEPOOL VALUES('" << name << "'," << (int)nbDrives << "," << (int)nbPartialTapes << "," << requester.user.getUid() << "," << requester.user.getGid() << "," << (int)time(NULL) << ",'" << comment << "');";
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
void cta::SqliteDatabase::insertStorageClass(const SecurityIdentity &requester, const std::string &name, const uint8_t nbCopies, const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO STORAGECLASS VALUES('" << name << "'," << (int)nbCopies << "," << requester.user.getUid() << "," << requester.user.getGid() << "," << (int)time(NULL) << ",'" << comment << "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertStorageClass() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// insertArchiveRoute
//------------------------------------------------------------------------------
void cta::SqliteDatabase::insertArchiveRoute(const SecurityIdentity &requester, const std::string &storageClassName, const uint8_t copyNb, const std::string &tapePoolName, const std::string &comment) {
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "INSERT INTO ARCHIVEROUTE VALUES('" << storageClassName << "'," << (int)copyNb << ",'" << tapePoolName << "'," << requester.user.getUid() << "," << requester.user.getGid() << "," << (int)time(NULL) << ",'" << comment << "');";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "insertArchiveRoute() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
}

//------------------------------------------------------------------------------
// deleteTapePool
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteTapePool(const SecurityIdentity &requester, const std::string &name) {
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
void cta::SqliteDatabase::deleteStorageClass(const SecurityIdentity &requester, const std::string &name) {
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
// deleteArchiveRoute
//------------------------------------------------------------------------------
void cta::SqliteDatabase::deleteArchiveRoute(const SecurityIdentity &requester, const std::string &storageClassName, const uint8_t copyNb) {
  checkArchiveRouteExists(storageClassName, copyNb);
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "DELETE FROM ARCHIVEROUTE WHERE STORAGECLASS_NAME='" << storageClassName << "' AND COPYNB=" << (int)copyNb << ";";
  int rc = sqlite3_exec(m_dbHandle, query.str().c_str(), 0, 0, &zErrMsg);
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
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
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkTapePoolExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement);
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
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkStorageClassExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement);
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
// checkArchiveRouteExists
//------------------------------------------------------------------------------
void cta::SqliteDatabase::checkArchiveRouteExists(const std::string &name, const uint8_t copyNb){
  char *zErrMsg = 0;
  std::ostringstream query;
  query << "SELECT * FROM ARCHIVEROUTE WHERE STORAGECLASS_NAME='" << name << "' AND COPYNB=" << (int)copyNb << ";";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "checkArchiveRouteExists() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  int res = sqlite3_step(statement);
  if(res==SQLITE_ROW) {
    return;
  } 
  else if(res==SQLITE_DONE){
    std::ostringstream message;
    message << "ARCHIVEROUTE: " << name << " with COPYNB: " << (int)copyNb << " does not exist";
    throw(Exception(message.str()));    
  }
  else {
    std::ostringstream message;
    message << "checkArchiveRouteExists() - SQLite error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    throw(Exception(message.str()));    
  }  
}

//------------------------------------------------------------------------------
// selectAllTapePools
//------------------------------------------------------------------------------
std::list<cta::TapePool> cta::SqliteDatabase::selectAllTapePools(const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::TapePool> pools;
  query << "SELECT * FROM TAPEPOOL ORDER BY NAME;";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllTapePools() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement)==SQLITE_ROW) {
    pools.push_back(cta::TapePool(
            std::string((char *)sqlite3_column_text(statement,0)),
            sqlite3_column_int(statement,1),
            sqlite3_column_int(statement,2),
            cta::UserIdentity(sqlite3_column_int(statement,3),sqlite3_column_int(statement,4)),
            time_t(sqlite3_column_int(statement,5)),
            std::string((char *)sqlite3_column_text(statement,6))
      ));
  }
  sqlite3_finalize(statement);  
  return pools;
}

//------------------------------------------------------------------------------
// selectAllStorageClasses
//------------------------------------------------------------------------------
std::list<cta::StorageClass> cta::SqliteDatabase::selectAllStorageClasses(const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::StorageClass> classes;
  query << "SELECT * FROM STORAGECLASS ORDER BY NAME;";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllStorageClasses() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement)==SQLITE_ROW) {
    classes.push_back(cta::StorageClass(
            std::string((char *)sqlite3_column_text(statement,0)),
            sqlite3_column_int(statement,1),
            cta::UserIdentity(sqlite3_column_int(statement,2),sqlite3_column_int(statement,3)),
            time_t(sqlite3_column_int(statement,4)),
            std::string((char *)sqlite3_column_text(statement,5))
      ));
  }
  sqlite3_finalize(statement);  
  return classes;
}

//------------------------------------------------------------------------------
// selectAllArchiveRoutes
//------------------------------------------------------------------------------
std::list<cta::ArchiveRoute>  cta::SqliteDatabase::selectAllArchiveRoutes(const SecurityIdentity &requester) {
  char *zErrMsg = 0;
  std::ostringstream query;
  std::list<cta::ArchiveRoute> routes;
  query << "SELECT * FROM ARCHIVEROUTE ORDER BY STORAGECLASS_NAME, COPYNB;";
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(m_dbHandle, query.str().c_str(), -1, &statement, 0 );
  if(rc!=SQLITE_OK){    
      std::ostringstream message;
      message << "selectAllArchiveRoutes() - SQLite error: " << zErrMsg;
      sqlite3_free(zErrMsg);
      throw(Exception(message.str()));
  }
  while(sqlite3_step(statement)==SQLITE_ROW) {
    routes.push_back(cta::ArchiveRoute(
            std::string((char *)sqlite3_column_text(statement,0)),
            sqlite3_column_int(statement,1),
            std::string((char *)sqlite3_column_text(statement,2)),
            cta::UserIdentity(sqlite3_column_int(statement,3),sqlite3_column_int(statement,4)),
            time_t(sqlite3_column_int(statement,5)),
            std::string((char *)sqlite3_column_text(statement,6))
      ));
  }
  sqlite3_finalize(statement);
  return routes;
}

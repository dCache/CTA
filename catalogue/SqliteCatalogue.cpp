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

#include "catalogue/SqliteCatalogue.hpp"
#include "catalogue/SqliteCatalogueSchema.hpp"
#include "catalogue/SqliteRset.hpp"
#include "catalogue/SqliteStmt.hpp"
#include "common/dataStructures/TapeFile.hpp"
#include "common/exception/Exception.hpp"

#include <memory>
#include <sqlite3.h>
#include <time.h>

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
SqliteCatalogue::SqliteCatalogue():
  m_conn(":memory:"),
  m_nextArchiveFileId(1) {
  createDbSchema();
}

//------------------------------------------------------------------------------
// createSchema
//------------------------------------------------------------------------------
void SqliteCatalogue::createDbSchema() {
  const SqliteCatalogueSchema schema;
  m_conn.enableForeignKeys();
  m_conn.execNonQuery(schema.sql);
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
SqliteCatalogue::~SqliteCatalogue() {
}

//------------------------------------------------------------------------------
// createBootstrapAdminAndHostNoAuth
//------------------------------------------------------------------------------
void SqliteCatalogue::createBootstrapAdminAndHostNoAuth(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const common::dataStructures::UserIdentity &user,
  const std::string &hostName,
  const std::string &comment) {
  createAdminUser(cliIdentity, user, comment);
  createAdminHost(cliIdentity, hostName, comment);
}

//------------------------------------------------------------------------------
// createAdminUser
//------------------------------------------------------------------------------
void SqliteCatalogue::createAdminUser(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const common::dataStructures::UserIdentity &user,
  const std::string &comment) {
  try {
    const uint64_t now = time(NULL);
    const char *const sql =
      "INSERT INTO ADMIN_USER("
        "ADMIN_USER_NAME,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
        "VALUES("
        ":ADMIN_USER_NAME,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":ADMIN_USER_NAME", user.name);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// deleteAdminUser
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteAdminUser(const common::dataStructures::UserIdentity &user) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getAdminUsers
//------------------------------------------------------------------------------
std::list<common::dataStructures::AdminUser>
  SqliteCatalogue::getAdminUsers() const {
  try {
    std::list<common::dataStructures::AdminUser> admins;
    const char *const sql =
      "SELECT "
        "ADMIN_USER_NAME AS ADMIN_USER_NAME,"

        "USER_COMMENT AS USER_COMMENT,"

        "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
        "FROM ADMIN_USER;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      common::dataStructures::AdminUser admin;

      admin.name = rset->columnText("ADMIN_USER_NAME");

      admin.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      admin.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      admin.lastModificationLog = updateLog;

      admins.push_back(admin);
    }

    return admins;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// modifyAdminUserComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyAdminUserComment(const common::dataStructures::SecurityIdentity &cliIdentity, const common::dataStructures::UserIdentity &user, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createAdminHost
//------------------------------------------------------------------------------
void SqliteCatalogue::createAdminHost(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const std::string &hostName,
  const std::string &comment) {
  try {
    const uint64_t now = time(NULL);
    const char *const sql =
      "INSERT INTO ADMIN_HOST("
        "ADMIN_HOST_NAME,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
        "VALUES("
        ":ADMIN_HOST_NAME,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":ADMIN_HOST_NAME", hostName);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// deleteAdminHost
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteAdminHost(const std::string &hostName) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getAdminHosts
//------------------------------------------------------------------------------
std::list<common::dataStructures::AdminHost> SqliteCatalogue::getAdminHosts() const {
  try {
    std::list<common::dataStructures::AdminHost> hosts;
    const char *const sql =
      "SELECT "
        "ADMIN_HOST_NAME AS ADMIN_HOST_NAME,"

        "USER_COMMENT AS USER_COMMENT,"

        "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
        "FROM ADMIN_HOST;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      common::dataStructures::AdminHost host;

      host.name = rset->columnText("ADMIN_HOST_NAME");
      host.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      host.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      host.lastModificationLog = updateLog;

      hosts.push_back(host);
    }

    return hosts;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// modifyAdminHostComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyAdminHostComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &hostName, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createStorageClass
//------------------------------------------------------------------------------
void SqliteCatalogue::createStorageClass(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const std::string &name,
  const uint64_t nbCopies,
  const std::string &comment) {
  try {
    const time_t now = time(NULL);
    const char *const sql =
      "INSERT INTO STORAGE_CLASS("
        "STORAGE_CLASS_NAME,"
        "NB_COPIES,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
        "VALUES("
        ":STORAGE_CLASS_NAME,"
        ":NB_COPIES,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":STORAGE_CLASS_NAME", name);
    stmt->bind(":NB_COPIES", nbCopies);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// deleteStorageClass
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteStorageClass(const std::string &name) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getStorageClasses
//------------------------------------------------------------------------------
std::list<common::dataStructures::StorageClass>
  SqliteCatalogue::getStorageClasses() const {
  try {
    std::list<common::dataStructures::StorageClass> storageClasses;
    const char *const sql =
      "SELECT "
        "STORAGE_CLASS_NAME AS STORAGE_CLASS_NAME,"
        "NB_COPIES AS NB_COPIES,"

        "USER_COMMENT AS USER_COMMENT,"

        "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
        "FROM STORAGE_CLASS;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      common::dataStructures::StorageClass storageClass;

      storageClass.name = rset->columnText("STORAGE_CLASS_NAME");
      storageClass.nbCopies = rset->columnUint64("NB_COPIES");
      storageClass.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      storageClass.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      storageClass.lastModificationLog = updateLog;

      storageClasses.push_back(storageClass);
    }

    return storageClasses;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// modifyStorageClassNbCopies
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyStorageClassNbCopies(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t nbCopies) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyStorageClassComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyStorageClassComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createTapePool
//------------------------------------------------------------------------------
void SqliteCatalogue::createTapePool(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const std::string &name,
  const uint64_t nbPartialTapes,
  const bool encryptionValue,
  const std::string &comment) {
  try {
    const time_t now = time(NULL);
    const char *const sql =
      "INSERT INTO TAPE_POOL("
        "TAPE_POOL_NAME,"
        "NB_PARTIAL_TAPES,"
        "IS_ENCRYPTED,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
        "VALUES("
        ":TAPE_POOL_NAME,"
        ":NB_PARTIAL_TAPES,"
        ":IS_ENCRYPTED,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":TAPE_POOL_NAME", name);
    stmt->bind(":NB_PARTIAL_TAPES", nbPartialTapes);
    stmt->bind(":IS_ENCRYPTED", encryptionValue);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// deleteTapePool
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteTapePool(const std::string &name) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getTapePools
//------------------------------------------------------------------------------
std::list<common::dataStructures::TapePool>
  SqliteCatalogue::getTapePools() const {
  try {
    std::list<common::dataStructures::TapePool> pools;
    const char *const sql =
      "SELECT "
        "TAPE_POOL_NAME AS TAPE_POOL_NAME,"
        "NB_PARTIAL_TAPES AS NB_PARTIAL_TAPES,"
        "IS_ENCRYPTED AS IS_ENCRYPTED,"

        "USER_COMMENT AS USER_COMMENT,"

        "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
        "FROM TAPE_POOL;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      common::dataStructures::TapePool pool;

      pool.name = rset->columnText("TAPE_POOL_NAME");
      pool.nbPartialTapes = rset->columnUint64("NB_PARTIAL_TAPES");
      pool.encryption = rset->columnUint64("IS_ENCRYPTED");

      pool.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      pool.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      pool.lastModificationLog = updateLog;

      pools.push_back(pool);
    }

    return pools;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// modifyTapePoolNbPartialTapes
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapePoolNbPartialTapes(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t nbPartialTapes) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapePoolComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapePoolComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// setTapePoolEncryption
//------------------------------------------------------------------------------
void SqliteCatalogue::setTapePoolEncryption(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const bool encryptionValue) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createArchiveRoute
//------------------------------------------------------------------------------
void SqliteCatalogue::createArchiveRoute(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const std::string &storageClassName,
  const uint64_t copyNb,
  const std::string &tapePoolName,
  const std::string &comment) {
  try {
    const time_t now = time(NULL);
    const char *const sql =
      "INSERT INTO ARCHIVE_ROUTE("
        "STORAGE_CLASS_NAME,"
        "COPY_NB,"
        "TAPE_POOL_NAME,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
        "VALUES("
        ":STORAGE_CLASS_NAME,"
        ":COPY_NB,"
        ":TAPE_POOL_NAME,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":STORAGE_CLASS_NAME", storageClassName);
    stmt->bind(":COPY_NB", copyNb);
    stmt->bind(":TAPE_POOL_NAME", tapePoolName);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// deleteArchiveRoute
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteArchiveRoute(const std::string &storageClassName, const uint64_t copyNb) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getArchiveRoutes
//------------------------------------------------------------------------------
std::list<common::dataStructures::ArchiveRoute>
  SqliteCatalogue::getArchiveRoutes() const {
  try {
    std::list<common::dataStructures::ArchiveRoute> routes;
    const char *const sql =
      "SELECT "
        "STORAGE_CLASS_NAME AS STORAGE_CLASS_NAME,"
        "COPY_NB AS COPY_NB,"
        "TAPE_POOL_NAME AS TAPE_POOL_NAME,"

        "USER_COMMENT AS USER_COMMENT,"

        "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
        "FROM ARCHIVE_ROUTE;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      common::dataStructures::ArchiveRoute route;

      route.storageClassName = rset->columnText("STORAGE_CLASS_NAME");
      route.copyNb = rset->columnUint64("COPY_NB");
      route.tapePoolName = rset->columnText("TAPE_POOL_NAME");

      route.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      route.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      route.lastModificationLog = updateLog;

      routes.push_back(route);
    }

    return routes;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// modifyArchiveRouteTapePoolName
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyArchiveRouteTapePoolName(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &storageClassName, const uint64_t copyNb, const std::string &tapePoolName) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyArchiveRouteComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyArchiveRouteComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &storageClassName, const uint64_t copyNb, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createLogicalLibrary
//------------------------------------------------------------------------------
void SqliteCatalogue::createLogicalLibrary(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const std::string &name,
  const std::string &comment) {
  try {
    const time_t now = time(NULL);
    const char *const sql =
      "INSERT INTO LOGICAL_LIBRARY("
        "LOGICAL_LIBRARY_NAME,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
        "VALUES("
        ":LOGICAL_LIBRARY_NAME,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":LOGICAL_LIBRARY_NAME", name);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.what());
  }
}

//------------------------------------------------------------------------------
// deleteLogicalLibrary
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteLogicalLibrary(const std::string &name) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getLogicalLibraries
//------------------------------------------------------------------------------
std::list<common::dataStructures::LogicalLibrary>
  SqliteCatalogue::getLogicalLibraries() const {
  try {
    std::list<common::dataStructures::LogicalLibrary> libs;
    const char *const sql =
      "SELECT "
        "LOGICAL_LIBRARY_NAME AS LOGICAL_LIBRARY_NAME,"

        "USER_COMMENT AS USER_COMMENT,"

        "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
        "FROM LOGICAL_LIBRARY;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      common::dataStructures::LogicalLibrary lib;

      lib.name = rset->columnText("LOGICAL_LIBRARY_NAME");

      lib.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      lib.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      lib.lastModificationLog = updateLog;

      libs.push_back(lib);
    }

    return libs;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// modifyLogicalLibraryComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyLogicalLibraryComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createTape
//------------------------------------------------------------------------------
void SqliteCatalogue::createTape(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const std::string &vid,
  const std::string &logicalLibraryName,
  const std::string &tapePoolName,
  const std::string &encryptionKey,
  const uint64_t capacityInBytes,
  const bool disabledValue,
  const bool fullValue,
  const std::string &comment) {
  try {
    const time_t now = time(NULL);
    const char *const sql =
      "INSERT INTO TAPE("
        "VID,"
        "LOGICAL_LIBRARY_NAME,"
        "TAPE_POOL_NAME,"
        "ENCRYPTION_KEY,"
        "CAPACITY_IN_BYTES,"
        "DATA_IN_BYTES,"
        "LAST_FSEQ,"
        "IS_DISABLED,"
        "IS_FULL,"
        "LBP_IS_ON,"

        "LABEL_DRIVE,"
        "LABEL_TIME,"

        "LAST_READ_DRIVE,"
        "LAST_READ_TIME,"

        "LAST_WRITE_DRIVE,"
        "LAST_WRITE_TIME,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
        "VALUES("
        ":VID,"
        ":LOGICAL_LIBRARY_NAME,"
        ":TAPE_POOL_NAME,"
        ":ENCRYPTION_KEY,"
        ":CAPACITY_IN_BYTES,"
        ":DATA_IN_BYTES,"
        ":LAST_FSEQ,"
        ":IS_DISABLED,"
        ":IS_FULL,"
        ":LBP_IS_ON,"

        ":LABEL_DRIVE,"
        ":LABEL_TIME,"

        ":LAST_READ_DRIVE,"
        ":LAST_READ_TIME,"

        ":LAST_WRITE_DRIVE,"
        ":LAST_WRITE_TIME,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":VID", vid);
    stmt->bind(":LOGICAL_LIBRARY_NAME", logicalLibraryName);
    stmt->bind(":TAPE_POOL_NAME", tapePoolName);
    stmt->bind(":ENCRYPTION_KEY", encryptionKey);
    stmt->bind(":CAPACITY_IN_BYTES", capacityInBytes);
    stmt->bind(":DATA_IN_BYTES", 0);
    stmt->bind(":LAST_FSEQ", 0);
    stmt->bind(":IS_DISABLED", disabledValue);
    stmt->bind(":IS_FULL", fullValue);
    stmt->bind(":LBP_IS_ON", 1);

    stmt->bind(":LABEL_DRIVE", "");
    stmt->bind(":LABEL_TIME", 0);

    stmt->bind(":LAST_READ_DRIVE", "");
    stmt->bind(":LAST_READ_TIME", 0);

    stmt->bind(":LAST_WRITE_DRIVE", "");
    stmt->bind(":LAST_WRITE_TIME", 0);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// deleteTape
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteTape(const std::string &vid) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getTapes
//------------------------------------------------------------------------------
std::list<common::dataStructures::Tape>
  SqliteCatalogue::getTapes(
  const std::string &vid,
  const std::string &logicalLibraryName,
  const std::string &tapePoolName,
  const std::string &capacityInBytes,
  const std::string &disabledValue,
  const std::string &fullValue,
  const std::string &busyValue,
  const std::string &lbpValue) {
  try {
    std::list<common::dataStructures::Tape> tapes;
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
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
        "FROM TAPE;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      common::dataStructures::Tape tape;

      tape.vid = rset->columnText("VID");
      tape.logicalLibraryName =
        rset->columnText("LOGICAL_LIBRARY_NAME");
      tape.tapePoolName = rset->columnText("TAPE_POOL_NAME");
      tape.encryptionKey = rset->columnText("ENCRYPTION_KEY");
      tape.capacityInBytes = rset->columnUint64("CAPACITY_IN_BYTES");
      tape.dataOnTapeInBytes = rset->columnUint64("DATA_IN_BYTES");
      tape.lastFSeq = rset->columnUint64("LAST_FSEQ");
      tape.disabled = rset->columnUint64("IS_DISABLED");
      tape.full = rset->columnUint64("IS_FULL");
      tape.lbp = rset->columnUint64("LBP_IS_ON");

      tape.labelLog.drive = rset->columnText("LABEL_DRIVE");
      tape.labelLog.time = rset->columnUint64("LABEL_TIME");

      tape.lastReadLog.drive = rset->columnText("LAST_READ_DRIVE");
      tape.lastReadLog.time = rset->columnUint64("LAST_READ_TIME");

      tape.lastWriteLog.drive = rset->columnText("LAST_WRITE_DRIVE");
      tape.lastWriteLog.time = rset->columnUint64("LAST_WRITE_TIME");

      tape.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      tape.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      tape.lastModificationLog = updateLog;

      tapes.push_back(tape);
    }

    return tapes;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// reclaimTape
//------------------------------------------------------------------------------
void SqliteCatalogue::reclaimTape(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapeLogicalLibraryName
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapeLogicalLibraryName(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &logicalLibraryName) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapeTapePoolName
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapeTapePoolName(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &tapePoolName) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapeCapacityInBytes
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapeCapacityInBytes(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const uint64_t capacityInBytes) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapeEncryptionKey
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapeEncryptionKey(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &encryptionKey) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapeLabelLog
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapeLabelLog(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &drive, const uint64_t timestamp) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapeLastWrittenLog
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapeLastWrittenLog(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &drive, const uint64_t timestamp) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapeLastReadLog
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapeLastReadLog(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &drive, const uint64_t timestamp) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// setTapeBusy
//------------------------------------------------------------------------------
void SqliteCatalogue::setTapeBusy(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const bool busyValue) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// setTapeFull
//------------------------------------------------------------------------------
void SqliteCatalogue::setTapeFull(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const bool fullValue) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// setTapeDisabled
//------------------------------------------------------------------------------
void SqliteCatalogue::setTapeDisabled(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const bool disabledValue) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// setTapeLbp
//------------------------------------------------------------------------------
void SqliteCatalogue::setTapeLbp(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const bool lbpValue) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyTapeComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyTapeComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createRequester
//------------------------------------------------------------------------------
void SqliteCatalogue::createRequester(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const common::dataStructures::UserIdentity &user,
  const std::string &mountPolicy,
  const std::string &comment) {
  try {
    const uint64_t now = time(NULL);
    const char *const sql =
      "INSERT INTO REQUESTER("
        "REQUESTER_NAME,"
        "MOUNT_POLICY_NAME,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
      "VALUES("
        ":REQUESTER_NAME,"
        ":MOUNT_POLICY_NAME,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":REQUESTER_NAME", user.name);
    stmt->bind(":MOUNT_POLICY_NAME", mountPolicy);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// deleteRequester
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteRequester(const common::dataStructures::UserIdentity &user) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getRequesters
//------------------------------------------------------------------------------
std::list<common::dataStructures::Requester>
  SqliteCatalogue::getRequesters() const {
  try {
    std::list<common::dataStructures::Requester> users;
    const char *const sql =
      "SELECT "
        "REQUESTER_NAME AS REQUESTER_NAME,"
        "MOUNT_POLICY_NAME AS MOUNT_POLICY_NAME,"

        "USER_COMMENT AS USER_COMMENT,"

        "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
      "FROM REQUESTER;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while(rset->next()) {
      common::dataStructures::Requester user;

      user.name = rset->columnText("REQUESTER_NAME");
      user.group = "N/A";
      user.mountPolicy = rset->columnText("MOUNT_POLICY_NAME");

      user.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      user.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      user.lastModificationLog = updateLog;

      users.push_back(user);
    }

    return users;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// modifyRequesterMountPolicy
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyRequesterMountPolicy(const common::dataStructures::SecurityIdentity &cliIdentity, const common::dataStructures::UserIdentity &user, const std::string &mountPolicy) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyRequesterComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyRequesterComment(const common::dataStructures::SecurityIdentity &cliIdentity, const common::dataStructures::UserIdentity &user, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createMountPolicy
//------------------------------------------------------------------------------
void SqliteCatalogue::createMountPolicy(
  const common::dataStructures::SecurityIdentity &cliIdentity,
  const std::string &name,
  const uint64_t archivePriority,
  const uint64_t minArchiveRequestAge,
  const uint64_t retrievePriority,
  const uint64_t minRetrieveRequestAge,
  const uint64_t maxDrivesAllowed,
  const std::string &comment) {
  try {
    const time_t now = time(NULL);
    const char *const sql =
      "INSERT INTO MOUNT_POLICY("
        "MOUNT_POLICY_NAME,"

        "ARCHIVE_PRIORITY,"
        "ARCHIVE_MIN_REQUEST_AGE,"

        "RETRIEVE_PRIORITY,"
        "RETRIEVE_MIN_REQUEST_AGE,"

        "MAX_DRIVES_ALLOWED,"

        "USER_COMMENT,"

        "CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME)"
        "VALUES("
        ":MOUNT_POLICY_NAME,"

        ":ARCHIVE_PRIORITY,"
        ":ARCHIVE_MIN_REQUEST_AGE,"

        ":RETRIEVE_PRIORITY,"
        ":RETRIEVE_MIN_REQUEST_AGE,"

        ":MAX_DRIVES_ALLOWED,"

        ":USER_COMMENT,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME,"

        ":CREATION_LOG_USER_NAME,"
        ":CREATION_LOG_GROUP_NAME,"
        ":CREATION_LOG_HOST_NAME,"
        ":CREATION_LOG_TIME);";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":MOUNT_POLICY_NAME", name);

    stmt->bind(":ARCHIVE_PRIORITY", archivePriority);
    stmt->bind(":ARCHIVE_MIN_REQUEST_AGE", minArchiveRequestAge);

    stmt->bind(":RETRIEVE_PRIORITY", retrievePriority);
    stmt->bind(":RETRIEVE_MIN_REQUEST_AGE", minRetrieveRequestAge);

    stmt->bind(":MAX_DRIVES_ALLOWED", maxDrivesAllowed);

    stmt->bind(":USER_COMMENT", comment);

    stmt->bind(":CREATION_LOG_USER_NAME", cliIdentity.user.name);
    stmt->bind(":CREATION_LOG_GROUP_NAME", cliIdentity.user.group);
    stmt->bind(":CREATION_LOG_HOST_NAME", cliIdentity.host);
    stmt->bind(":CREATION_LOG_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// deleteMountPolicy
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteMountPolicy(const std::string &name) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getMountPolicies
//------------------------------------------------------------------------------
std::list<common::dataStructures::MountPolicy>
  SqliteCatalogue::getMountPolicies() const {
  try {
    std::list<common::dataStructures::MountPolicy> policies;
    const char *const sql =
      "SELECT "
        "MOUNT_POLICY_NAME AS MOUNT_POLICY_NAME,"

        "ARCHIVE_PRIORITY AS ARCHIVE_PRIORITY,"
        "ARCHIVE_MIN_REQUEST_AGE AS ARCHIVE_MIN_REQUEST_AGE,"

        "RETRIEVE_PRIORITY AS RETRIEVE_PRIORITY,"
        "RETRIEVE_MIN_REQUEST_AGE AS RETRIEVE_MIN_REQUEST_AGE,"

        "MAX_DRIVES_ALLOWED AS MAX_DRIVES_ALLOWED,"

        "USER_COMMENT AS USER_COMMENT,"

        "CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
        "CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
        "CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
        "CREATION_LOG_TIME AS CREATION_LOG_TIME,"

        "LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
        "LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
        "LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
        "LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
        "FROM MOUNT_POLICY;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      common::dataStructures::MountPolicy policy;

      policy.name = rset->columnText("MOUNT_POLICY_NAME");

      policy.archive_priority = rset->columnUint64("ARCHIVE_PRIORITY");
      policy.archive_minRequestAge =
        rset->columnUint64("ARCHIVE_MIN_REQUEST_AGE");

      policy.retrieve_priority =
        rset->columnUint64("RETRIEVE_PRIORITY");
      policy.retrieve_minRequestAge =
        rset->columnUint64("RETRIEVE_MIN_REQUEST_AGE");

      policy.maxDrivesAllowed =
        rset->columnUint64("MAX_DRIVES_ALLOWED");

      policy.comment = rset->columnText("USER_COMMENT");

      common::dataStructures::UserIdentity creatorUI;
      creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
      creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

      common::dataStructures::EntryLog creationLog;
      creationLog.user = creatorUI;
      creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
      creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

      policy.creationLog = creationLog;

      common::dataStructures::UserIdentity updaterUI;
      updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
      updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

      common::dataStructures::EntryLog updateLog;
      updateLog.user = updaterUI;
      updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
      updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

      policy.lastModificationLog = updateLog;

      policies.push_back(policy);
    }

    return policies;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// modifyMountPolicyArchivePriority
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyArchivePriority(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t archivePriority) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyArchiveMinFilesQueued
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyArchiveMinFilesQueued(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t minArchiveFilesQueued) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyArchiveMinBytesQueued
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyArchiveMinBytesQueued(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t archiveMinBytesQueued) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyArchiveMinRequestAge
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyArchiveMinRequestAge(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t minArchiveRequestAge) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyRetrievePriority
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyRetrievePriority(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t retrievePriority) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyRetrieveMinFilesQueued
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyRetrieveMinFilesQueued(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t minRetrieveFilesQueued) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyRetrieveMinBytesQueued
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyRetrieveMinBytesQueued(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t retrieveMinBytesQueued) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyRetrieveMinRequestAge
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyRetrieveMinRequestAge(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t minRetrieveRequestAge) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyMaxDrivesAllowed
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyMaxDrivesAllowed(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t maxDrivesAllowed) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyMountPolicyComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyMountPolicyComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createDedication
//------------------------------------------------------------------------------
void SqliteCatalogue::createDedication(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const common::dataStructures::DedicationType dedicationType,
 const std::string &tag, const std::string &vid, const uint64_t fromTimestamp, const uint64_t untilTimestamp,const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// deleteDedication
//------------------------------------------------------------------------------
void SqliteCatalogue::deleteDedication(const std::string &drivename) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getDedications
//------------------------------------------------------------------------------
std::list<common::dataStructures::Dedication> SqliteCatalogue::getDedications() const {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyDedicationType
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyDedicationType(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const common::dataStructures::DedicationType dedicationType) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyDedicationTag
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyDedicationTag(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const std::string &tag) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyDedicationVid
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyDedicationVid(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const std::string &vid) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyDedicationFrom
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyDedicationFrom(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const uint64_t fromTimestamp) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyDedicationUntil
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyDedicationUntil(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const uint64_t untilTimestamp) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// modifyDedicationComment
//------------------------------------------------------------------------------
void SqliteCatalogue::modifyDedicationComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const std::string &comment) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// createArchiveFile
//------------------------------------------------------------------------------
void SqliteCatalogue::createArchiveFile(
  const common::dataStructures::ArchiveFile &archiveFile) {
  try {
    const time_t now = time(NULL);
    const char *const sql =
      "INSERT INTO ARCHIVE_FILE("
        "ARCHIVE_FILE_ID,"
        "DISK_INSTANCE,"
        "DISK_FILE_ID,"
        "DISK_FILE_PATH,"
        "DISK_FILE_USER,"
        "DISK_FILE_GROUP,"
        "DISK_FILE_RECOVERY_BLOB,"
        "FILE_SIZE,"
        "CHECKSUM_TYPE,"
        "CHECKSUM_VALUE,"
        "STORAGE_CLASS_NAME,"
        "CREATION_TIME,"
        "RECONCILIATION_TIME)"
      "VALUES("
        ":ARCHIVE_FILE_ID,"
        ":DISK_INSTANCE,"
        ":DISK_FILE_ID,"
        ":DISK_FILE_PATH,"
        ":DISK_FILE_USER,"
        ":DISK_FILE_GROUP,"
        ":DISK_FILE_RECOVERY_BLOB,"
        ":FILE_SIZE,"
        ":CHECKSUM_TYPE,"
        ":CHECKSUM_VALUE,"
        ":STORAGE_CLASS_NAME,"
        ":CREATION_TIME,"
        ":RECONCILIATION_TIME)";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

    stmt->bind(":ARCHIVE_FILE_ID", archiveFile.archiveFileID);
    stmt->bind(":DISK_INSTANCE", archiveFile.diskInstance);
    stmt->bind(":DISK_FILE_ID", archiveFile.diskFileID);
    stmt->bind(":DISK_FILE_PATH", archiveFile.drData.drPath);
    stmt->bind(":DISK_FILE_USER", archiveFile.drData.drOwner);
    stmt->bind(":DISK_FILE_GROUP",  archiveFile.drData.drGroup);
    stmt->bind(":DISK_FILE_RECOVERY_BLOB", archiveFile.drData.drBlob);
    stmt->bind(":FILE_SIZE", archiveFile.fileSize);
    stmt->bind(":CHECKSUM_TYPE", archiveFile.checksumType);
    stmt->bind(":CHECKSUM_VALUE", archiveFile.checksumValue);
    stmt->bind(":STORAGE_CLASS_NAME", archiveFile.storageClass);
    stmt->bind(":CREATION_TIME", now);
    stmt->bind(":RECONCILIATION_TIME", now);

    stmt->executeNonQuery();
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// getArchiveFileId
//------------------------------------------------------------------------------
uint64_t SqliteCatalogue::getArchiveFileId(const std::string &diskInstance, const std::string &diskFileId) const {
  try {
    const char *const sql =
      "SELECT "
        "ARCHIVE_FILE_ID AS ARCHIVE_FILE_ID "
        "FROM ARCHIVE_FILE WHERE "
        "DISK_INSTANCE = :DISK_INSTANCE AND "
        "DISK_FILE_ID = :DISK_FILE_ID;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    stmt->bind(":DISK_INSTANCE", diskInstance);
    stmt->bind(":DISK_FILE_ID", diskFileId);
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());

    if (rset->next()) {
      return rset->columnUint64("ARCHIVE_FILE_ID");
    } else {
      throw std::runtime_error(std::string("Could not find archive file with disk instance ") + diskInstance +
        "and disk file ID " + diskFileId);
    }
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + "failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// getArchiveFiles
//------------------------------------------------------------------------------
std::list<common::dataStructures::ArchiveFile> SqliteCatalogue::getArchiveFiles(
  const std::string &id,
  const std::string &eosid,
  const std::string &copynb,
  const std::string &tapepool,
  const std::string &vid,
  const std::string &owner,
  const std::string &group,
  const std::string &storageclass,
  const std::string &path) {
  try {
    const char *const sql =
      "SELECT "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID AS ARCHIVE_FILE_ID,"
        "ARCHIVE_FILE.DISK_INSTANCE AS DISK_INSTANCE,"
        "ARCHIVE_FILE.DISK_FILE_ID AS DISK_FILE_ID,"
        "ARCHIVE_FILE.DISK_FILE_PATH AS DISK_FILE_PATH,"
        "ARCHIVE_FILE.DISK_FILE_USER AS DISK_FILE_USER,"
        "ARCHIVE_FILE.DISK_FILE_GROUP AS DISK_FILE_GROUP,"
        "ARCHIVE_FILE.DISK_FILE_RECOVERY_BLOB AS DISK_FILE_RECOVERY_BLOB,"
        "ARCHIVE_FILE.FILE_SIZE AS FILE_SIZE,"
        "ARCHIVE_FILE.CHECKSUM_TYPE AS CHECKSUM_TYPE,"
        "ARCHIVE_FILE.CHECKSUM_VALUE AS CHECKSUM_VALUE,"
        "ARCHIVE_FILE.STORAGE_CLASS_NAME AS STORAGE_CLASS_NAME,"
        "ARCHIVE_FILE.CREATION_TIME AS ARCHIVE_FILE_CREATION_TIME,"
        "ARCHIVE_FILE.RECONCILIATION_TIME AS RECONCILIATION_TIME,"
        "TAPE_FILE.VID AS VID,"
        "TAPE_FILE.FSEQ AS FSEQ,"
        "TAPE_FILE.BLOCK_ID AS BLOCK_ID,"
        "TAPE_FILE.COMPRESSED_SIZE AS COMPRESSED_SIZE,"
        "TAPE_FILE.COPY_NB AS COPY_NB,"
        "TAPE_FILE.CREATION_TIME AS TAPE_FILE_CREATION_TIME "
      "FROM "
        "ARCHIVE_FILE "
      "LEFT OUTER JOIN TAPE_FILE ON "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID = TAPE_FILE.ARCHIVE_FILE_ID "
      "ORDER BY "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID,"
        "TAPE_FILE.COPY_NB;"
    ;
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    std::list<common::dataStructures::ArchiveFile> archiveFiles;
    while (rset->next()) {
      const uint64_t archiveFileId = rset->columnUint64("ARCHIVE_FILE_ID");

      if(archiveFiles.empty() || archiveFiles.back().archiveFileID != archiveFileId) {
        common::dataStructures::ArchiveFile archiveFile;

        archiveFile.archiveFileID = archiveFileId;
        archiveFile.diskInstance = rset->columnText("DISK_INSTANCE");
        archiveFile.diskFileID = rset->columnText("DISK_FILE_ID");
        archiveFile.drData.drPath = rset->columnText("DISK_FILE_PATH");
        archiveFile.drData.drOwner = rset->columnText("DISK_FILE_USER");
        archiveFile.drData.drGroup = rset->columnText("DISK_FILE_GROUP");
        archiveFile.drData.drBlob = rset->columnText("DISK_FILE_RECOVERY_BLOB");
        archiveFile.fileSize = rset->columnUint64("FILE_SIZE");
        archiveFile.checksumType = rset->columnText("CHECKSUM_TYPE");
        archiveFile.checksumValue = rset->columnText("CHECKSUM_VALUE");
        archiveFile.storageClass = rset->columnText("STORAGE_CLASS_NAME");
        archiveFile.creationTime = rset->columnUint64("ARCHIVE_FILE_CREATION_TIME");
        archiveFile.reconciliationTime = rset->columnUint64("RECONCILIATION_TIME");

        archiveFiles.push_back(archiveFile);
      }

      common::dataStructures::ArchiveFile &archiveFile = archiveFiles.back();

      // If there is a tape file
      if(!rset->columnIsNull("VID")) {
        common::dataStructures::TapeFile tapeFile;
        tapeFile.vid = rset->columnText("VID");
        tapeFile.fSeq = rset->columnUint64("FSEQ");
        tapeFile.blockId = rset->columnUint64("BLOCK_ID");
        tapeFile.compressedSize = rset->columnUint64("COMPRESSED_SIZE");
        tapeFile.copyNb = rset->columnUint64("COPY_NB");
        tapeFile.creationTime = rset->columnUint64("TAPE_FILE_CREATION_TIME");

        archiveFile.tapeFiles[rset->columnUint64("COPY_NB")] = tapeFile;
      }
    }

    return archiveFiles;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " +ne.what());
  }
}

//------------------------------------------------------------------------------
// getArchiveFileSummary
//------------------------------------------------------------------------------
common::dataStructures::ArchiveFileSummary SqliteCatalogue::getArchiveFileSummary(const std::string &id, const std::string &eosid,
        const std::string &copynb, const std::string &tapepool, const std::string &vid, const std::string &owner, const std::string &group, const std::string &storageclass, const std::string &path) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// getArchiveFileById
//------------------------------------------------------------------------------
common::dataStructures::ArchiveFile SqliteCatalogue::getArchiveFileById(const uint64_t id) {
  std::unique_ptr<common::dataStructures::ArchiveFile> file(getArchiveFile(id));

  // Throw an exception if the archive file does not exist
  if(NULL == file.get()) {
    exception::Exception ex;
    ex.getMessage() << __FUNCTION__ << " failed: No such archive file with ID " << id;
    throw(ex);
  }

  return *file;
}
          
//------------------------------------------------------------------------------
// setDriveStatus
//------------------------------------------------------------------------------
void SqliteCatalogue::setDriveStatus(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &driveName, const bool up, const bool force) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
}

//------------------------------------------------------------------------------
// fileWrittenToTape
//------------------------------------------------------------------------------
void SqliteCatalogue::fileWrittenToTape(const TapeFileWritten &event) {
  const time_t now = time(NULL);
  std::lock_guard<std::mutex> m_lock(m_mutex);

  std::unique_ptr<common::dataStructures::ArchiveFile> archiveFile = getArchiveFile(event.archiveFileId);

  // If the archive file does not already exist then create one
  if(NULL == archiveFile.get()) {
    archiveFile.reset(new common::dataStructures::ArchiveFile);
    archiveFile->archiveFileID = event.archiveFileId;
    archiveFile->diskFileID = event.diskFileId;
    archiveFile->diskInstance = event.diskInstance;
    archiveFile->fileSize = event.size;
    archiveFile->checksumType = "HELP";
    archiveFile->checksumValue = "HELP";
    archiveFile->storageClass = event.storageClassName;
    archiveFile->drData.drBlob = event.diskFileRecoveryBlob;
    archiveFile->drData.drGroup = event.diskFileGroup;
    archiveFile->drData.drOwner = event.diskFileUser;
    archiveFile->drData.drPath = event.diskFilePath;
    createArchiveFile(*archiveFile);
  }

  // Create the tape file
  common::dataStructures::TapeFile tapeFile;
  tapeFile.vid            = event.vid;
  tapeFile.fSeq           = event.fSeq;
  tapeFile.blockId        = event.blockId;
  tapeFile.compressedSize = event.compressedSize;
  tapeFile.copyNb         = event.copyNb;
  tapeFile.creationTime   = now;
  createTapeFile(tapeFile, event.archiveFileId);
}

//------------------------------------------------------------------------------
// prepareForNewFile
//------------------------------------------------------------------------------
common::dataStructures::ArchiveFileQueueCriteria
  SqliteCatalogue::prepareForNewFile(
  const std::string &storageClass, const common::dataStructures::UserIdentity &user) {
  const common::dataStructures::TapeCopyToPoolMap copyToPoolMap =
    getTapeCopyToPoolMap(storageClass);
  const uint64_t expectedNbRoutes = getExpectedNbArchiveRoutes(storageClass);

  // Check that the number of archive routes is correct
  if(copyToPoolMap.empty()) {
    exception::Exception ex;
    ex.getMessage() << "Storage class " << storageClass << " has no archive"
      " routes";
    throw ex;
  }
  if(copyToPoolMap.size() != expectedNbRoutes) {
    exception::Exception ex;
    ex.getMessage() << "Storage class " << storageClass << " does not have the"
      " expected number of archive routes routes: expected=" << expectedNbRoutes
      << ", actual=" << copyToPoolMap.size();
    throw ex;
  }

  common::dataStructures::MountPolicy mountPolicy = getMountPolicyForAUser(user);

  // Now that we have both the archive routes and the mount policy it's safe to
  // consume an archive file identifier
  const uint64_t archiveFileId = m_nextArchiveFileId++;

  return common::dataStructures::ArchiveFileQueueCriteria(archiveFileId,
    copyToPoolMap, mountPolicy);
}

//------------------------------------------------------------------------------
// getTapeCopyToPoolMap
//------------------------------------------------------------------------------
common::dataStructures::TapeCopyToPoolMap SqliteCatalogue::
  getTapeCopyToPoolMap(const std::string &storageClass) const {
  try {
    common::dataStructures::TapeCopyToPoolMap copyToPoolMap;
    const char *const sql =
      "SELECT "
        "COPY_NB AS COPY_NB,"
        "TAPE_POOL_NAME AS TAPE_POOL_NAME "
        "FROM ARCHIVE_ROUTE WHERE "
        "STORAGE_CLASS_NAME = :STORAGE_CLASS_NAME;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    stmt->bind(":STORAGE_CLASS_NAME", storageClass);
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      const uint64_t copyNb = rset->columnUint64("COPY_NB");
      const std::string tapePoolName = rset->columnText("TAPE_POOL_NAME");
      copyToPoolMap[copyNb] = tapePoolName;
    }

    return copyToPoolMap;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// getExpectedNbArchiveRoutes
//------------------------------------------------------------------------------
uint64_t SqliteCatalogue::getExpectedNbArchiveRoutes(const std::string &storageClass) const {
  try {
    uint64_t nbRoutes = 0;
    const char *const sql =
      "SELECT "
        "COUNT(*) AS NB_ROUTES "
        "FROM ARCHIVE_ROUTE WHERE "
        "STORAGE_CLASS_NAME = :STORAGE_CLASS_NAME;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    stmt->bind(":STORAGE_CLASS_NAME", storageClass);
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    while (rset->next()) {
      nbRoutes = rset->columnUint64("NB_ROUTES");
    }
    return nbRoutes;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ne.what());
  }
}

//------------------------------------------------------------------------------
// getArchiveMountPolicy
//------------------------------------------------------------------------------
common::dataStructures::MountPolicy SqliteCatalogue::
  getMountPolicyForAUser(const common::dataStructures::UserIdentity &user) const {
  const char *const sql =
    "SELECT "
      "MOUNT_POLICY.MOUNT_POLICY_NAME AS MOUNT_POLICY_NAME,"

      "ARCHIVE_PRIORITY AS ARCHIVE_PRIORITY,"
      "ARCHIVE_MIN_REQUEST_AGE AS ARCHIVE_MIN_REQUEST_AGE,"
      "RETRIEVE_PRIORITY AS RETRIEVE_PRIORITY,"
      "RETRIEVE_MIN_REQUEST_AGE AS RETRIEVE_MIN_REQUEST_AGE,"

      "MAX_DRIVES_ALLOWED AS MAX_DRIVES_ALLOWED,"

      "MOUNT_POLICY.USER_COMMENT AS USER_COMMENT,"

      "MOUNT_POLICY.CREATION_LOG_USER_NAME AS CREATION_LOG_USER_NAME,"
      "MOUNT_POLICY.CREATION_LOG_GROUP_NAME AS CREATION_LOG_GROUP_NAME,"
      "MOUNT_POLICY.CREATION_LOG_HOST_NAME AS CREATION_LOG_HOST_NAME,"
      "MOUNT_POLICY.CREATION_LOG_TIME AS CREATION_LOG_TIME,"

      "MOUNT_POLICY.LAST_UPDATE_USER_NAME AS LAST_UPDATE_USER_NAME,"
      "MOUNT_POLICY.LAST_UPDATE_GROUP_NAME AS LAST_UPDATE_GROUP_NAME,"
      "MOUNT_POLICY.LAST_UPDATE_HOST_NAME AS LAST_UPDATE_HOST_NAME,"
      "MOUNT_POLICY.LAST_UPDATE_TIME AS LAST_UPDATE_TIME "
    "FROM MOUNT_POLICY INNER JOIN REQUESTER ON "
      "MOUNT_POLICY.MOUNT_POLICY_NAME = REQUESTER.MOUNT_POLICY_NAME "
    "WHERE "
      "REQUESTER.REQUESTER_NAME = :REQUESTER_NAME;";
  std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
  stmt->bind(":REQUESTER_NAME", user.name);
  std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
  if(rset->next()) {
    common::dataStructures::MountPolicy policy;

    policy.name = rset->columnText("MOUNT_POLICY_NAME");

    policy.archive_priority = rset->columnUint64("ARCHIVE_PRIORITY");
    policy.archive_minRequestAge = rset->columnUint64("ARCHIVE_MIN_REQUEST_AGE");

    policy.retrieve_priority = rset->columnUint64("RETRIEVE_PRIORITY");
    policy.retrieve_minRequestAge = rset->columnUint64("RETRIEVE_MIN_REQUEST_AGE");

    policy.maxDrivesAllowed =
      rset->columnUint64("MAX_DRIVES_ALLOWED");

    policy.comment = rset->columnText("USER_COMMENT");

    common::dataStructures::UserIdentity creatorUI;
    creatorUI.name = rset->columnText("CREATION_LOG_USER_NAME");
    creatorUI.group = rset->columnText("CREATION_LOG_GROUP_NAME");

    common::dataStructures::EntryLog creationLog;
    creationLog.user = creatorUI;
    creationLog.host = rset->columnText("CREATION_LOG_HOST_NAME");
    creationLog.time = rset->columnUint64("CREATION_LOG_TIME");

    policy.creationLog = creationLog;

    common::dataStructures::UserIdentity updaterUI;
    updaterUI.name = rset->columnText("LAST_UPDATE_USER_NAME");
    updaterUI.group = rset->columnText("LAST_UPDATE_GROUP_NAME");

    common::dataStructures::EntryLog updateLog;
    updateLog.user = updaterUI;
    updateLog.host = rset->columnText("LAST_UPDATE_HOST_NAME");
    updateLog.time = rset->columnUint64("LAST_UPDATE_TIME");

    policy.lastModificationLog = updateLog;
    return policy;
  } else {
    exception::Exception ex;
    ex.getMessage() << "Failed to find a mount policy for user " <<
      user;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// isAdmin
//------------------------------------------------------------------------------
bool SqliteCatalogue::isAdmin(const common::dataStructures::SecurityIdentity &cliIdentity) const {
  return userIsAdmin(cliIdentity.user.name) && hostIsAdmin(cliIdentity.host);
}

//------------------------------------------------------------------------------
// userIsAdmin
//------------------------------------------------------------------------------
bool SqliteCatalogue::userIsAdmin(const std::string &userName) const {
  const char *const sql =
    "SELECT "
      "ADMIN_USER_NAME AS ADMIN_USER_NAME "
    "FROM ADMIN_USER WHERE "
      "ADMIN_USER_NAME = :ADMIN_USER_NAME;";
  std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
  stmt->bind(":ADMIN_USER_NAME", userName);
  std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
  return rset->next();
}

//------------------------------------------------------------------------------
// hostIsAdmin
//------------------------------------------------------------------------------
bool SqliteCatalogue::hostIsAdmin(const std::string &hostName) const {
  const char *const sql =
    "SELECT "
      "ADMIN_HOST_NAME AS ADMIN_HOST_NAME "
    "FROM ADMIN_HOST WHERE "
      "ADMIN_HOST_NAME = :ADMIN_HOST_NAME;";
  std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
  stmt->bind(":ADMIN_HOST_NAME", hostName);
  std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
  return rset->next();
}

//------------------------------------------------------------------------------
// createTapeFile
//------------------------------------------------------------------------------
void SqliteCatalogue::createTapeFile(const common::dataStructures::TapeFile &tapeFile, const uint64_t archiveFileId) {
  const time_t now = time(NULL);
  const char *const sql =
    "INSERT INTO TAPE_FILE("
      "VID,"
      "FSEQ,"
      "BLOCK_ID,"
      "COMPRESSED_SIZE,"
      "COPY_NB,"
      "CREATION_TIME,"
      "ARCHIVE_FILE_ID)"
    "VALUES("
      ":VID,"
      ":FSEQ,"
      ":BLOCK_ID,"
      ":COMPRESSED_SIZE,"
      ":COPY_NB,"
      ":CREATION_TIME,"
      ":ARCHIVE_FILE_ID);";
  std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));

  stmt->bind(":VID", tapeFile.vid);
  stmt->bind(":FSEQ", tapeFile.fSeq);
  stmt->bind(":BLOCK_ID", tapeFile.blockId);
  stmt->bind(":COMPRESSED_SIZE", tapeFile.compressedSize);
  stmt->bind(":COPY_NB", tapeFile.copyNb);
  stmt->bind(":CREATION_TIME", now);
  stmt->bind(":ARCHIVE_FILE_ID", archiveFileId);

  stmt->executeNonQuery();
}

//------------------------------------------------------------------------------
// getTapeFiles
//------------------------------------------------------------------------------
std::list<common::dataStructures::TapeFile> SqliteCatalogue::getTapeFiles() const {
  std::list<common::dataStructures::TapeFile> files;
  const char *const sql =
    "SELECT "
      "VID AS VID,"
      "FSEQ AS FSEQ,"
      "BLOCK_ID AS BLOCK_ID,"
      "COMPRESSED_SIZE AS COMPRESSED_SIZE,"
      "COPY_NB AS COPY_NB,"
      "CREATION_TIME AS CREATION_TIME "
    "FROM TAPE_FILE;";
  std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
  std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
  while(rset->next()) {
    common::dataStructures::TapeFile file;

    file.vid = rset->columnText("VID");
    file.fSeq = rset->columnUint64("FSEQ");
    file.blockId = rset->columnUint64("BLOCK_ID");
    file.compressedSize = rset->columnUint64("COMPRESSED_SIZE");
    file.copyNb = rset->columnUint64("COPY_NB");
    file.creationTime = rset->columnUint64("CREATION_TIME");

    files.push_back(file);
  }

  return files;
}

//------------------------------------------------------------------------------
// setTapeLastFseq
//------------------------------------------------------------------------------
void SqliteCatalogue::setTapeLastFSeq(const std::string &vid, const uint64_t lastFSeq) {
  std::lock_guard<std::mutex> lock(m_mutex);

  const uint64_t currentValue = getTapeLastFSeq(vid);
  if(lastFSeq != currentValue + 1) {
    exception::Exception ex;
    ex.getMessage() << __FUNCTION__ << " failed: The last FSeq MUST be incremented by exactly one: "
      "currentValue=" << currentValue << ",nextValue=" << lastFSeq;
    throw ex;
  }
  const char *const sql =
    "UPDATE TAPE SET "
      "LAST_FSEQ = :LAST_FSEQ "
    "WHERE "
      "VID=:VID;";
  std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
  stmt->bind(":VID", vid);
  stmt->bind(":LAST_FSEQ", lastFSeq);
  stmt->executeNonQuery();
}

//------------------------------------------------------------------------------
// getTapeLastFSeq
//------------------------------------------------------------------------------
uint64_t SqliteCatalogue::getTapeLastFSeq(const std::string &vid) const {
  const char *const sql =
    "SELECT "
      "LAST_FSEQ AS LAST_FSEQ "
    "FROM TAPE WHERE "
      "VID = :VID;";
  std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
  stmt->bind(":VID", vid);
  std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
  if(rset->next()) {
    return rset->columnUint64("LAST_FSEQ");
  } else {
    exception::Exception ex;
    ex.getMessage() << __FUNCTION__ << " failed: No such tape with vid=" << vid;
    throw ex;
  }
  return rset->columnUint64("LAST_FSEQ");
}

//------------------------------------------------------------------------------
// getArchiveFile
//------------------------------------------------------------------------------
std::unique_ptr<common::dataStructures::ArchiveFile> SqliteCatalogue::getArchiveFile(const uint64_t archiveFileId) const {
  try {
    const char *const sql =
      "SELECT "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID AS ARCHIVE_FILE_ID,"
        "ARCHIVE_FILE.DISK_INSTANCE AS DISK_INSTANCE,"
        "ARCHIVE_FILE.DISK_FILE_ID AS DISK_FILE_ID,"
        "ARCHIVE_FILE.DISK_FILE_PATH AS DISK_FILE_PATH,"
        "ARCHIVE_FILE.DISK_FILE_USER AS DISK_FILE_USER,"
        "ARCHIVE_FILE.DISK_FILE_GROUP AS DISK_FILE_GROUP,"
        "ARCHIVE_FILE.DISK_FILE_RECOVERY_BLOB AS DISK_FILE_RECOVERY_BLOB,"
        "ARCHIVE_FILE.FILE_SIZE AS FILE_SIZE,"
        "ARCHIVE_FILE.CHECKSUM_TYPE AS CHECKSUM_TYPE,"
        "ARCHIVE_FILE.CHECKSUM_VALUE AS CHECKSUM_VALUE,"
        "ARCHIVE_FILE.STORAGE_CLASS_NAME AS STORAGE_CLASS_NAME,"
        "ARCHIVE_FILE.CREATION_TIME AS ARCHIVE_FILE_CREATION_TIME,"
        "ARCHIVE_FILE.RECONCILIATION_TIME AS RECONCILIATION_TIME,"
        "TAPE_FILE.VID AS VID,"
        "TAPE_FILE.FSEQ AS FSEQ,"
        "TAPE_FILE.BLOCK_ID AS BLOCK_ID,"
        "TAPE_FILE.COMPRESSED_SIZE AS COMPRESSED_SIZE,"
        "TAPE_FILE.COPY_NB AS COPY_NB,"
        "TAPE_FILE.CREATION_TIME AS TAPE_FILE_CREATION_TIME "
      "FROM "
        "ARCHIVE_FILE "
      "LEFT OUTER JOIN TAPE_FILE ON "
          "ARCHIVE_FILE.ARCHIVE_FILE_ID = TAPE_FILE.ARCHIVE_FILE_ID "
      "WHERE "
        "ARCHIVE_FILE.ARCHIVE_FILE_ID = :ARCHIVE_FILE_ID;";
    std::unique_ptr<SqliteStmt> stmt(m_conn.createStmt(sql));
    stmt->bind(":ARCHIVE_FILE_ID", archiveFileId);
    std::unique_ptr<SqliteRset> rset(stmt->executeQuery());
    std::unique_ptr<common::dataStructures::ArchiveFile> archiveFile;
    while (rset->next()) {
      if(NULL == archiveFile.get()) {
        archiveFile.reset(new common::dataStructures::ArchiveFile);

        archiveFile->archiveFileID = rset->columnUint64("ARCHIVE_FILE_ID");
        archiveFile->diskInstance = rset->columnText("DISK_INSTANCE");
        archiveFile->diskFileID = rset->columnText("DISK_FILE_ID");
        archiveFile->drData.drPath = rset->columnText("DISK_FILE_PATH");
        archiveFile->drData.drOwner = rset->columnText("DISK_FILE_USER");
        archiveFile->drData.drGroup = rset->columnText("DISK_FILE_GROUP");
        archiveFile->drData.drBlob = rset->columnText("DISK_FILE_RECOVERY_BLOB");
        archiveFile->fileSize = rset->columnUint64("FILE_SIZE");
        archiveFile->checksumType = rset->columnText("CHECKSUM_TYPE");
        archiveFile->checksumValue = rset->columnText("CHECKSUM_VALUE");
        archiveFile->storageClass = rset->columnText("STORAGE_CLASS_NAME");
        archiveFile->creationTime = rset->columnUint64("ARCHIVE_FILE_CREATION_TIME");
        archiveFile->reconciliationTime = rset->columnUint64("RECONCILIATION_TIME");
      }

      // If there is a tape file
      if(!rset->columnIsNull("VID")) {
        // Add the tape file to the archive file's in-memory structure
        common::dataStructures::TapeFile tapeFile;
        tapeFile.vid = rset->columnText("VID");
        tapeFile.fSeq = rset->columnUint64("FSEQ");
        tapeFile.blockId = rset->columnUint64("BLOCK_ID");
        tapeFile.compressedSize = rset->columnUint64("COMPRESSED_SIZE");
        tapeFile.creationTime = rset->columnUint64("TAPE_FILE_CREATION_TIME");
        archiveFile->tapeFiles[rset->columnUint64("COPY_NB")] = tapeFile;
      }
    }

    return archiveFile;
  } catch(std::exception &ne) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " +ne.what());
  }
}

} // namespace catalogue
} // namespace cta

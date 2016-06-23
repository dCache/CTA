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

#include "catalogue/ArchiveFileRow.hpp"
#include "catalogue/CatalogueFactory.hpp"
#include "catalogue/UserError.hpp"
#include "common/exception/Exception.hpp"

#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <set>

namespace unitTests {

class cta_catalogue_InMemoryCatalogueTest : public ::testing::Test {
public:
  cta_catalogue_InMemoryCatalogueTest():
    m_bootstrapComment("bootstrap") {

    m_cliSI.username = "cli_user_name";
    m_cliSI.host = "cli_host";

    m_bootstrapAdminSI.username = "bootstrap_admin_user_name";
    m_bootstrapAdminSI.host = "bootstrap_host";

    m_adminSI.username = "admin_user_name";
    m_adminSI.host = "admin_host";
  }

protected:

  virtual void SetUp() {
    using namespace cta;
    using namespace cta::catalogue;

    const DbLogin catalogueLogin(DbLogin::DBTYPE_IN_MEMORY, "", "", "");
    m_catalogue.reset(CatalogueFactory::create(catalogueLogin));

    {
      const std::list<common::dataStructures::AdminUser> adminUsers = m_catalogue->getAdminUsers();
      for(auto &adminUser: adminUsers) {
        m_catalogue->deleteAdminUser(adminUser.name);
      }
    }
    {
      const std::list<common::dataStructures::AdminHost> adminHosts = m_catalogue->getAdminHosts();
      for(auto &adminHost: adminHosts) {
        m_catalogue->deleteAdminHost(adminHost.name);
      }
    }
    {
      const std::list<common::dataStructures::ArchiveRoute> archiveRoutes = m_catalogue->getArchiveRoutes();
      for(auto &archiveRoute: archiveRoutes) {
        m_catalogue->deleteArchiveRoute(archiveRoute.storageClassName, archiveRoute.copyNb);
      }
    }
    {
      const std::list<common::dataStructures::RequesterMountRule> rules = m_catalogue->getRequesterMountRules();
      for(auto &rule: rules) {
        m_catalogue->deleteRequesterMountRule(rule.name);
      }
    }
    {
      const std::list<common::dataStructures::RequesterGroupMountRule> rules =
        m_catalogue->getRequesterGroupMountRules();
      for(auto &rule: rules) {
        m_catalogue->deleteRequesterGroupMountRule(rule.name);
      }
    }
    {
      std::unique_ptr<ArchiveFileItor> itor = m_catalogue->getArchiveFileItor();
      while(itor->hasMore()) {
        m_catalogue->deleteArchiveFile(itor->next().archiveFileID);
      }
    }
    {
      const std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes("", "", "", "", "", "", "", "");
      for(auto &tape: tapes) {
        m_catalogue->deleteTape(tape.vid);
      }
    }
    {
      const std::list<common::dataStructures::StorageClass> storageClasses = m_catalogue->getStorageClasses();
      for(auto &storageClass: storageClasses) {
        m_catalogue->deleteStorageClass(storageClass.name);
      }
    }
    {
      const std::list<common::dataStructures::TapePool> tapePools = m_catalogue->getTapePools();
      for(auto &tapePool: tapePools) {
        m_catalogue->deleteTapePool(tapePool.name);
      }
    }
    {
      const std::list<common::dataStructures::LogicalLibrary> logicalLibraries = m_catalogue->getLogicalLibraries();
      for(auto &logicalLibrary: logicalLibraries) {
        m_catalogue->deleteLogicalLibrary(logicalLibrary.name);
      }
    }
    {
      const std::list<common::dataStructures::MountPolicy> mountPolicies = m_catalogue->getMountPolicies();
      for(auto &mountPolicy: mountPolicies) {
        m_catalogue->deleteMountPolicy(mountPolicy.name);
      }
    }
  }

  virtual void TearDown() {
    m_catalogue.reset();
  }

  std::unique_ptr<cta::catalogue::Catalogue> m_catalogue;
  const std::string m_bootstrapComment;
  cta::common::dataStructures::SecurityIdentity m_cliSI;
  cta::common::dataStructures::SecurityIdentity m_bootstrapAdminSI;
  cta::common::dataStructures::SecurityIdentity m_adminSI;

  /**
   * Creates a map from VID to tape given the specified list of tapes.
   *
   * @param listOfTapes The list of tapes from which the map shall be created.
   * @return The map from VID to tape.
   */
  std::map<std::string, cta::common::dataStructures::Tape> tapeListToMap(
    const std::list<cta::common::dataStructures::Tape> &listOfTapes) {
    using namespace cta;

    try {
      std::map<std::string, cta::common::dataStructures::Tape> vidToTape;

      for (auto &&tape: listOfTapes) {
        if(vidToTape.end() != vidToTape.find(tape.vid)) {
          throw exception::Exception(std::string("Duplicate VID: value=") + tape.vid);
        }
        vidToTape[tape.vid] = tape;
      }

      return vidToTape;
    } catch(exception::Exception &ex) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
    }
  }

  /**
   * Creates a map from archive file ID to archive file from the specified iterator.
   *
   * @param itor Iterator over archive files.
   * @return Map from archive file ID to archive file.
   */
  std::map<uint64_t, cta::common::dataStructures::ArchiveFile> archiveFileItorToMap(cta::catalogue::ArchiveFileItor &itor) {
    using namespace cta;

    try {
      std::map<uint64_t, common::dataStructures::ArchiveFile> m;
      while(itor.hasMore()) {
        const common::dataStructures::ArchiveFile archiveFile = itor.next();
        if(m.end() != m.find(archiveFile.archiveFileID)) {
          exception::Exception ex;
          ex.getMessage() << "Archive file with ID " << archiveFile.archiveFileID << " is a duplicate";
          throw ex;
        }
        m[archiveFile.archiveFileID] = archiveFile;
      }
      return m;
    } catch(exception::Exception &ex) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
    }
  }
};

TEST_F(cta_catalogue_InMemoryCatalogueTest, createBootstrapAdminAndHostNoAuth) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getAdminUsers().empty());
  ASSERT_TRUE(m_catalogue->getAdminHosts().empty());

  m_catalogue->createBootstrapAdminAndHostNoAuth(
    m_cliSI, m_bootstrapAdminSI.username, m_bootstrapAdminSI.host, m_bootstrapComment);

  {
    std::list<common::dataStructures::AdminUser> admins;
    admins = m_catalogue->getAdminUsers();
    ASSERT_EQ(1, admins.size());

    const common::dataStructures::AdminUser admin = admins.front();
    ASSERT_EQ(m_bootstrapComment, admin.comment);

    const common::dataStructures::EntryLog creationLog = admin.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      admin.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  {
    std::list<common::dataStructures::AdminHost> hosts;
    hosts = m_catalogue->getAdminHosts();
    ASSERT_EQ(1, hosts.size());

    const common::dataStructures::AdminHost host = hosts.front();
    ASSERT_EQ(m_bootstrapComment, host.comment);

    const common::dataStructures::EntryLog creationLog = host.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      host.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createAdminUser) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getAdminUsers().empty());

  m_catalogue->createBootstrapAdminAndHostNoAuth(
    m_cliSI, m_bootstrapAdminSI.username, m_bootstrapAdminSI.host, m_bootstrapComment);

  {
    std::list<common::dataStructures::AdminUser> admins;
    admins = m_catalogue->getAdminUsers();
    ASSERT_EQ(1, admins.size());

    const common::dataStructures::AdminUser admin = admins.front();
    ASSERT_EQ(m_bootstrapComment, admin.comment);

    const common::dataStructures::EntryLog creationLog = admin.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      admin.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  const std::string createAdminUserComment = "create admin user";
  m_catalogue->createAdminUser(m_bootstrapAdminSI, m_adminSI.username, createAdminUserComment);

  {
    std::list<common::dataStructures::AdminUser> admins;
    admins = m_catalogue->getAdminUsers();
    ASSERT_EQ(2, admins.size());

    const common::dataStructures::AdminUser a1 = admins.front();
    admins.pop_front();
    const common::dataStructures::AdminUser a2 = admins.front();

    ASSERT_NE(a1, a2);
    ASSERT_TRUE((a1.name == m_bootstrapAdminSI.username && a2.name == m_adminSI.username) ||
      (a2.name == m_bootstrapAdminSI.username && a1.name == m_adminSI.username));

    if(a1.name == m_bootstrapAdminSI.username) {
      ASSERT_EQ(m_bootstrapAdminSI.username, a1.name);
      ASSERT_EQ(m_bootstrapComment, a1.comment);
      ASSERT_EQ(m_cliSI.username, a1.creationLog.username);
      ASSERT_EQ(m_cliSI.host, a1.creationLog.host);
      ASSERT_EQ(m_cliSI.username, a1.lastModificationLog.username);
      ASSERT_EQ(m_cliSI.host, a1.lastModificationLog.host);

      ASSERT_EQ(m_adminSI.username, a2.name);
      ASSERT_EQ(createAdminUserComment, a2.comment);
      ASSERT_EQ(m_bootstrapAdminSI.username, a2.creationLog.username);
      ASSERT_EQ(m_bootstrapAdminSI.host, a2.creationLog.host);
      ASSERT_EQ(m_bootstrapAdminSI.username, a2.lastModificationLog.username);
      ASSERT_EQ(m_bootstrapAdminSI.host, a2.lastModificationLog.host);
    } else {
      ASSERT_EQ(m_bootstrapAdminSI.username, a2.name);
      ASSERT_EQ(m_bootstrapComment, a2.comment);
      ASSERT_EQ(m_cliSI.username, a2.creationLog.username);
      ASSERT_EQ(m_cliSI.host, a2.creationLog.host);
      ASSERT_EQ(m_cliSI.username, a2.lastModificationLog.username);
      ASSERT_EQ(m_cliSI.host, a2.lastModificationLog.host);

      ASSERT_EQ(m_adminSI.username, a1.name);
      ASSERT_EQ(createAdminUserComment, a1.comment);
      ASSERT_EQ(m_bootstrapAdminSI.username, a1.creationLog.username);
      ASSERT_EQ(m_bootstrapAdminSI.host, a1.creationLog.host);
      ASSERT_EQ(m_bootstrapAdminSI.username, a1.lastModificationLog.username);
      ASSERT_EQ(m_bootstrapAdminSI.host, a1.lastModificationLog.host);
    }
  }
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createAdminUser_same_twice) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getAdminUsers().empty());

  m_catalogue->createBootstrapAdminAndHostNoAuth(
    m_cliSI, m_bootstrapAdminSI.username, m_bootstrapAdminSI.host, m_bootstrapComment);

  {
    std::list<common::dataStructures::AdminUser> admins;
    admins = m_catalogue->getAdminUsers();
    ASSERT_EQ(1, admins.size());

    const common::dataStructures::AdminUser admin = admins.front();
    ASSERT_EQ(m_bootstrapComment, admin.comment);

    const common::dataStructures::EntryLog creationLog = admin.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      admin.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  m_catalogue->createAdminUser(m_bootstrapAdminSI, m_adminSI.username, "comment 1");

  ASSERT_THROW(m_catalogue->createAdminUser(m_bootstrapAdminSI, m_adminSI.username,
    "comment 2"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteAdminUser) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getAdminUsers().empty());

  m_catalogue->createBootstrapAdminAndHostNoAuth(
    m_cliSI, m_bootstrapAdminSI.username, m_bootstrapAdminSI.host, m_bootstrapComment);

  {
    std::list<common::dataStructures::AdminUser> admins;
    admins = m_catalogue->getAdminUsers();
    ASSERT_EQ(1, admins.size());

    const common::dataStructures::AdminUser admin = admins.front();
    ASSERT_EQ(m_bootstrapComment, admin.comment);

    const common::dataStructures::EntryLog creationLog = admin.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      admin.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  m_catalogue->deleteAdminUser(m_bootstrapAdminSI.username);

  ASSERT_TRUE(m_catalogue->getAdminUsers().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteAdminUser_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getAdminUsers().empty());
  ASSERT_THROW(m_catalogue->deleteAdminUser("non_existant_sdmin_user"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createAdminHost) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getAdminHosts().empty());

  m_catalogue->createBootstrapAdminAndHostNoAuth(
    m_cliSI, m_bootstrapAdminSI.username, m_bootstrapAdminSI.host, m_bootstrapComment);

  {
    std::list<common::dataStructures::AdminUser> admins;
    admins = m_catalogue->getAdminUsers();
    ASSERT_EQ(1, admins.size());

    const common::dataStructures::AdminUser admin = admins.front();
    ASSERT_EQ(m_bootstrapComment, admin.comment);

    const common::dataStructures::EntryLog creationLog = admin.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      admin.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  const std::string createAdminHostComment = "create host user";
  const std::string anotherAdminHost = "another_admin_host";
  m_catalogue->createAdminHost(m_bootstrapAdminSI,
    anotherAdminHost, createAdminHostComment);

  {
    std::list<common::dataStructures::AdminHost> hosts;
    hosts = m_catalogue->getAdminHosts();
    ASSERT_EQ(2, hosts.size());

    const common::dataStructures::AdminHost h1 = hosts.front();
    hosts.pop_front();
    const common::dataStructures::AdminHost h2 = hosts.front();

    ASSERT_NE(h1, h2);
    ASSERT_TRUE(
      (h1.name == m_bootstrapAdminSI.host && h2.name == anotherAdminHost)
      ||
      (h2.name == anotherAdminHost && h1.name == m_bootstrapAdminSI.host)
    );

    if(h1.name == m_bootstrapAdminSI.host) {
      ASSERT_EQ(m_bootstrapAdminSI.host, h1.name);
      ASSERT_EQ(m_bootstrapComment, h1.comment);
      ASSERT_EQ(m_cliSI.username, h1.creationLog.username);
      ASSERT_EQ(m_cliSI.host, h1.creationLog.host);
      ASSERT_EQ(m_cliSI.username, h1.lastModificationLog.username);
      ASSERT_EQ(m_cliSI.host, h1.lastModificationLog.host);

      ASSERT_EQ(anotherAdminHost, h2.name);
      ASSERT_EQ(createAdminHostComment, h2.comment);
      ASSERT_EQ(m_bootstrapAdminSI.username, h2.creationLog.username);
      ASSERT_EQ(m_bootstrapAdminSI.host, h2.creationLog.host);
      ASSERT_EQ(m_bootstrapAdminSI.username, h2.lastModificationLog.username);
      ASSERT_EQ(m_bootstrapAdminSI.host, h2.lastModificationLog.host);
    } else {
      ASSERT_EQ(m_bootstrapAdminSI.host, h2.name);
      ASSERT_EQ(m_bootstrapComment, h2.comment);
      ASSERT_EQ(m_cliSI.username, h2.creationLog.username);
      ASSERT_EQ(m_cliSI.host, h2.creationLog.host);
      ASSERT_EQ(m_cliSI.username, h2.lastModificationLog.username);
      ASSERT_EQ(m_cliSI.host, h2.lastModificationLog.host);

      ASSERT_EQ(anotherAdminHost, h1.name);
      ASSERT_EQ(createAdminHostComment, h1.comment);
      ASSERT_EQ(m_bootstrapAdminSI.username, h1.creationLog.username);
      ASSERT_EQ(m_bootstrapAdminSI.host, h1.creationLog.host);
      ASSERT_EQ(m_bootstrapAdminSI.username, h1.lastModificationLog.username);
      ASSERT_EQ(m_bootstrapAdminSI.host, h1.lastModificationLog.host);
    }
  }
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createAdminHost_same_twice) {
  using namespace cta;

  m_catalogue->createBootstrapAdminAndHostNoAuth(
    m_cliSI, m_bootstrapAdminSI.username, m_bootstrapAdminSI.host, m_bootstrapComment);

  {
    std::list<common::dataStructures::AdminUser> admins;
    admins = m_catalogue->getAdminUsers();
    ASSERT_EQ(1, admins.size());

    const common::dataStructures::AdminUser admin = admins.front();
    ASSERT_EQ(m_bootstrapComment, admin.comment);

    const common::dataStructures::EntryLog creationLog = admin.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      admin.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  const std::string anotherAdminHost = "another_admin_host";

  m_catalogue->createAdminHost(m_bootstrapAdminSI, anotherAdminHost, "comment 1");

  ASSERT_THROW(m_catalogue->createAdminHost(m_bootstrapAdminSI,
    anotherAdminHost, "comment 2"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteAdminHost) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getAdminHosts().empty());

  m_catalogue->createBootstrapAdminAndHostNoAuth(
    m_cliSI, m_bootstrapAdminSI.username, m_bootstrapAdminSI.host, m_bootstrapComment);

  {
    std::list<common::dataStructures::AdminUser> admins;
    admins = m_catalogue->getAdminUsers();
    ASSERT_EQ(1, admins.size());

    const common::dataStructures::AdminUser admin = admins.front();
    ASSERT_EQ(m_bootstrapComment, admin.comment);

    const common::dataStructures::EntryLog creationLog = admin.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      admin.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  m_catalogue->deleteAdminHost(m_bootstrapAdminSI.host);

  ASSERT_TRUE(m_catalogue->getAdminHosts().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteAdminHost_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getAdminHosts().empty());
  ASSERT_THROW(m_catalogue->deleteAdminHost("non_exstant_admin_host"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, isAdmin_false) {
  using namespace cta;

  ASSERT_FALSE(m_catalogue->isAdmin(m_cliSI));
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, isAdmin_true) {
  using namespace cta;

  m_catalogue->createBootstrapAdminAndHostNoAuth(
    m_cliSI, m_bootstrapAdminSI.username, m_bootstrapAdminSI.host, m_bootstrapComment);

  ASSERT_TRUE(m_catalogue->isAdmin(m_bootstrapAdminSI));
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createStorageClass) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getStorageClasses().empty());

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  const std::string comment = "create storage class";
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, comment);

  const std::list<common::dataStructures::StorageClass> storageClasses =
    m_catalogue->getStorageClasses();

  ASSERT_EQ(1, storageClasses.size());

  const common::dataStructures::StorageClass storageClass =
    storageClasses.front();
  ASSERT_EQ(storageClassName, storageClass.name);
  ASSERT_EQ(nbCopies, storageClass.nbCopies);
  ASSERT_EQ(comment, storageClass.comment);

  const common::dataStructures::EntryLog creationLog = storageClass.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    storageClass.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createStorageClass_same_twice) {
  using namespace cta;

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  const std::string comment = "create storage class";
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, comment);
  ASSERT_THROW(m_catalogue->createStorageClass(m_cliSI,
    storageClassName, nbCopies, comment), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteStorageClass) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getStorageClasses().empty());

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  const std::string comment = "create storage class";
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, comment);

  const std::list<common::dataStructures::StorageClass> storageClasses =
    m_catalogue->getStorageClasses();

  ASSERT_EQ(1, storageClasses.size());

  const common::dataStructures::StorageClass storageClass =
    storageClasses.front();
  ASSERT_EQ(storageClassName, storageClass.name);
  ASSERT_EQ(nbCopies, storageClass.nbCopies);
  ASSERT_EQ(comment, storageClass.comment);

  const common::dataStructures::EntryLog creationLog = storageClass.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    storageClass.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  m_catalogue->deleteStorageClass(storageClass.name);
  ASSERT_TRUE(m_catalogue->getStorageClasses().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteStorageClass_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getStorageClasses().empty());
  ASSERT_THROW(m_catalogue->deleteStorageClass("non_existant_storage_class"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createTapePool) {
  using namespace cta;
      
  ASSERT_TRUE(m_catalogue->getTapePools().empty());
      
  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  const std::string comment = "create tape pool";
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted,
    comment);
      
  const std::list<common::dataStructures::TapePool> pools =
    m_catalogue->getTapePools();
      
  ASSERT_EQ(1, pools.size());
      
  const common::dataStructures::TapePool pool = pools.front();
  ASSERT_EQ(tapePoolName, pool.name);
  ASSERT_EQ(nbPartialTapes, pool.nbPartialTapes);
  ASSERT_EQ(is_encrypted, pool.encryption);
  ASSERT_EQ(comment, pool.comment);

  const common::dataStructures::EntryLog creationLog = pool.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);
  
  const common::dataStructures::EntryLog lastModificationLog =
    pool.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);
}
  
TEST_F(cta_catalogue_InMemoryCatalogueTest, createTapePool_same_twice) {
  using namespace cta;
  
  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  const std::string comment = "create tape pool";
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted,
    comment);
  ASSERT_THROW(m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted, comment),
    catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteTapePool) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getTapePools().empty());

  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  const std::string comment = "create tape pool";
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted,
    comment);

  const std::list<common::dataStructures::TapePool> pools =
    m_catalogue->getTapePools();

  ASSERT_EQ(1, pools.size());

  const common::dataStructures::TapePool pool = pools.front();
  ASSERT_EQ(tapePoolName, pool.name);
  ASSERT_EQ(nbPartialTapes, pool.nbPartialTapes);
  ASSERT_EQ(is_encrypted, pool.encryption);
  ASSERT_EQ(comment, pool.comment);

  const common::dataStructures::EntryLog creationLog = pool.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    pool.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  m_catalogue->deleteTapePool(pool.name);
  ASSERT_TRUE(m_catalogue->getTapePools().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteTapePool_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getTapePools().empty());
  ASSERT_THROW(m_catalogue->deleteTapePool("non_existant_tape_pool"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createArchiveRoute) {
  using namespace cta;
      
  ASSERT_TRUE(m_catalogue->getArchiveRoutes().empty());

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");
      
  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted, "create tape pool");

  const uint64_t copyNb = 1;
  const std::string comment = "create archive route";
  m_catalogue->createArchiveRoute(m_cliSI, storageClassName, copyNb, tapePoolName,
    comment);
      
  const std::list<common::dataStructures::ArchiveRoute> routes =
    m_catalogue->getArchiveRoutes();
      
  ASSERT_EQ(1, routes.size());
      
  const common::dataStructures::ArchiveRoute route = routes.front();
  ASSERT_EQ(storageClassName, route.storageClassName);
  ASSERT_EQ(copyNb, route.copyNb);
  ASSERT_EQ(tapePoolName, route.tapePoolName);
  ASSERT_EQ(comment, route.comment);

  const common::dataStructures::EntryLog creationLog = route.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);
  
  const common::dataStructures::EntryLog lastModificationLog =
    route.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  common::dataStructures::TapeCopyToPoolMap copyToPoolMap =
    m_catalogue->getTapeCopyToPoolMap(storageClassName);
  ASSERT_EQ(1, copyToPoolMap.size());
  std::pair<uint64_t, std::string> maplet = *(copyToPoolMap.begin());
  ASSERT_EQ(copyNb, maplet.first);
  ASSERT_EQ(tapePoolName, maplet.second);
}
  
TEST_F(cta_catalogue_InMemoryCatalogueTest, createArchiveRouteTapePool_same_twice) {
  using namespace cta;
  
  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");
      
  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted,
    "create tape pool");

  const uint64_t copyNb = 1;
  const std::string comment = "create archive route";
  m_catalogue->createArchiveRoute(m_cliSI, storageClassName, copyNb, tapePoolName,
    comment);
  ASSERT_THROW(m_catalogue->createArchiveRoute(m_cliSI,
    storageClassName, copyNb, tapePoolName, comment),
    exception::Exception);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteArchiveRoute) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getArchiveRoutes().empty());

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");

  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted, "create tape pool");

  const uint64_t copyNb = 1;
  const std::string comment = "create archive route";
  m_catalogue->createArchiveRoute(m_cliSI, storageClassName, copyNb, tapePoolName,
    comment);

  const std::list<common::dataStructures::ArchiveRoute> routes =
    m_catalogue->getArchiveRoutes();

  ASSERT_EQ(1, routes.size());

  const common::dataStructures::ArchiveRoute route = routes.front();
  ASSERT_EQ(storageClassName, route.storageClassName);
  ASSERT_EQ(copyNb, route.copyNb);
  ASSERT_EQ(tapePoolName, route.tapePoolName);
  ASSERT_EQ(comment, route.comment);

  const common::dataStructures::EntryLog creationLog = route.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    route.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  common::dataStructures::TapeCopyToPoolMap copyToPoolMap =
    m_catalogue->getTapeCopyToPoolMap(storageClassName);
  ASSERT_EQ(1, copyToPoolMap.size());
  std::pair<uint64_t, std::string> maplet = *(copyToPoolMap.begin());
  ASSERT_EQ(copyNb, maplet.first);
  ASSERT_EQ(tapePoolName, maplet.second);

  m_catalogue->deleteArchiveRoute(storageClassName, copyNb);

  ASSERT_TRUE(m_catalogue->getArchiveRoutes().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteArchiveRoute_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getArchiveRoutes().empty());
  ASSERT_THROW(m_catalogue->deleteArchiveRoute("non_existant_storage_class", 1234), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createArchiveRoute_deleteStorageClass) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getArchiveRoutes().empty());

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");

  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted, "create tape pool");

  const uint64_t copyNb = 1;
  const std::string comment = "create archive route";
  m_catalogue->createArchiveRoute(m_cliSI, storageClassName, copyNb, tapePoolName,
                               comment);

  const std::list<common::dataStructures::ArchiveRoute> routes =
    m_catalogue->getArchiveRoutes();

  ASSERT_EQ(1, routes.size());

  const common::dataStructures::ArchiveRoute route = routes.front();
  ASSERT_EQ(storageClassName, route.storageClassName);
  ASSERT_EQ(copyNb, route.copyNb);
  ASSERT_EQ(tapePoolName, route.tapePoolName);
  ASSERT_EQ(comment, route.comment);

  const common::dataStructures::EntryLog creationLog = route.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    route.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  common::dataStructures::TapeCopyToPoolMap copyToPoolMap =
    m_catalogue->getTapeCopyToPoolMap(storageClassName);
  ASSERT_EQ(1, copyToPoolMap.size());
  std::pair<uint64_t, std::string> maplet = *(copyToPoolMap.begin());
  ASSERT_EQ(copyNb, maplet.first);
  ASSERT_EQ(tapePoolName, maplet.second);

  ASSERT_THROW(m_catalogue->deleteStorageClass(storageClassName), exception::Exception);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createLogicalLibrary) {
  using namespace cta;
      
  ASSERT_TRUE(m_catalogue->getLogicalLibraries().empty());
      
  const std::string logicalLibraryName = "logical_library";
  const std::string comment = "create logical library";
  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName, comment);
      
  const std::list<common::dataStructures::LogicalLibrary> libs =
    m_catalogue->getLogicalLibraries();
      
  ASSERT_EQ(1, libs.size());
      
  const common::dataStructures::LogicalLibrary lib = libs.front();
  ASSERT_EQ(logicalLibraryName, lib.name);
  ASSERT_EQ(comment, lib.comment);

  const common::dataStructures::EntryLog creationLog = lib.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);
  
  const common::dataStructures::EntryLog lastModificationLog =
    lib.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);
}
  
TEST_F(cta_catalogue_InMemoryCatalogueTest, createLogicalLibrary_same_twice) {
  using namespace cta;
  
  const std::string logicalLibraryName = "logical_library";
  const std::string comment = "create logical library";
  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName, comment);
  ASSERT_THROW(m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName, comment), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteLogicalLibrary) {
  using namespace cta;
      
  ASSERT_TRUE(m_catalogue->getLogicalLibraries().empty());
      
  const std::string logicalLibraryName = "logical_library";
  const std::string comment = "create logical library";
  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName, comment);
      
  const std::list<common::dataStructures::LogicalLibrary> libs =
    m_catalogue->getLogicalLibraries();
      
  ASSERT_EQ(1, libs.size());
      
  const common::dataStructures::LogicalLibrary lib = libs.front();
  ASSERT_EQ(logicalLibraryName, lib.name);
  ASSERT_EQ(comment, lib.comment);

  const common::dataStructures::EntryLog creationLog = lib.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);
  
  const common::dataStructures::EntryLog lastModificationLog =
    lib.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  m_catalogue->deleteLogicalLibrary(logicalLibraryName);
  ASSERT_TRUE(m_catalogue->getLogicalLibraries().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteLogicalLibrary_non_existant) {
  using namespace cta;
      
  ASSERT_TRUE(m_catalogue->getLogicalLibraries().empty());
  ASSERT_THROW(m_catalogue->deleteLogicalLibrary("non_existant_logical_library"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createTape) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getTapes("", "", "", "", "", "", "", "").empty());

  const std::string vid = "vid";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName,
    "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid, logicalLibraryName, tapePoolName,
    encryptionKey, capacityInBytes, disabledValue, fullValue,
    comment);

  const std::list<common::dataStructures::Tape> tapes =
    m_catalogue->getTapes("", "", "", "", "", "", "", "");

  ASSERT_EQ(1, tapes.size());

  const common::dataStructures::Tape tape = tapes.front();
  ASSERT_EQ(vid, tape.vid);
  ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
  ASSERT_EQ(tapePoolName, tape.tapePoolName);
  ASSERT_EQ(encryptionKey, tape.encryptionKey);
  ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
  ASSERT_TRUE(disabledValue == tape.disabled);
  ASSERT_TRUE(fullValue == tape.full);
  ASSERT_EQ(comment, tape.comment);

  const common::dataStructures::EntryLog creationLog = tape.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    tape.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createTape_same_twice) {
  using namespace cta;

  const std::string vid = "vid";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName,
    "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid, logicalLibraryName, tapePoolName,
    encryptionKey, capacityInBytes, disabledValue, fullValue, comment);
  ASSERT_THROW(m_catalogue->createTape(m_cliSI, vid, logicalLibraryName,
    tapePoolName, encryptionKey, capacityInBytes, disabledValue, fullValue,
    comment), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteTape) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getTapes("", "", "", "", "", "", "", "").empty());

  const std::string vid = "vid";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName,
    "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid, logicalLibraryName, tapePoolName,
    encryptionKey, capacityInBytes, disabledValue, fullValue,
    comment);

  const std::list<common::dataStructures::Tape> tapes =
    m_catalogue->getTapes("", "", "", "", "", "", "", "");

  ASSERT_EQ(1, tapes.size());

  const common::dataStructures::Tape tape = tapes.front();
  ASSERT_EQ(vid, tape.vid);
  ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
  ASSERT_EQ(tapePoolName, tape.tapePoolName);
  ASSERT_EQ(encryptionKey, tape.encryptionKey);
  ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
  ASSERT_TRUE(disabledValue == tape.disabled);
  ASSERT_TRUE(fullValue == tape.full);
  ASSERT_EQ(comment, tape.comment);

  const common::dataStructures::EntryLog creationLog = tape.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    tape.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  m_catalogue->deleteTape(tape.vid);
  ASSERT_TRUE(m_catalogue->getTapes("", "", "", "", "", "", "", "").empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteTape_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getTapes("", "", "", "", "", "", "", "").empty());
  ASSERT_THROW(m_catalogue->deleteTape("non_exsitant_tape"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, getTapesForWriting) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getTapes("", "", "", "", "", "", "", "").empty());

  const std::string vid = "vid";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = false;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName,
    "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid, logicalLibraryName, tapePoolName,
    encryptionKey, capacityInBytes, disabledValue, fullValue,
    comment);

  const std::list<catalogue::TapeForWriting> tapes = m_catalogue->getTapesForWriting(logicalLibraryName);

  ASSERT_EQ(1, tapes.size());

  const catalogue::TapeForWriting tape = tapes.front();
  ASSERT_EQ(vid, tape.vid);
  ASSERT_EQ(tapePoolName, tape.tapePool);
  ASSERT_EQ(0, tape.lastFSeq);
  ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
  ASSERT_EQ(0, tape.dataOnTapeInBytes);
  ASSERT_FALSE(tape.lbp);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createMountPolicy) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getMountPolicies().empty());

  const std::string name = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 2;
  const uint64_t retrievePriority = 3;
  const uint64_t minRetrieveRequestAge = 4;
  const uint64_t maxDrivesAllowed = 5;
  const std::string &comment = "create mount policy";

  m_catalogue->createMountPolicy(
    m_cliSI,
    name,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    comment);

  const std::list<common::dataStructures::MountPolicy> mountPolicies =
    m_catalogue->getMountPolicies();

  ASSERT_EQ(1, mountPolicies.size());

  const common::dataStructures::MountPolicy mountPolicy = mountPolicies.front();

  ASSERT_EQ(name, mountPolicy.name);

  ASSERT_EQ(archivePriority, mountPolicy.archivePriority);
  ASSERT_EQ(minArchiveRequestAge, mountPolicy.archiveMinRequestAge);

  ASSERT_EQ(retrievePriority, mountPolicy.retrievePriority);
  ASSERT_EQ(minRetrieveRequestAge, mountPolicy.retrieveMinRequestAge);

  ASSERT_EQ(maxDrivesAllowed, mountPolicy.maxDrivesAllowed);

  ASSERT_EQ(comment, mountPolicy.comment);

  const common::dataStructures::EntryLog creationLog = mountPolicy.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    mountPolicy.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createMountPolicy_same_twice) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getMountPolicies().empty());

  const std::string name = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 4;
  const uint64_t retrievePriority = 5;
  const uint64_t minRetrieveRequestAge = 8;
  const uint64_t maxDrivesAllowed = 9;
  const std::string &comment = "create mount policy";

  m_catalogue->createMountPolicy(
    m_cliSI,
    name,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    comment);

  ASSERT_THROW(m_catalogue->createMountPolicy(
    m_cliSI,
    name,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    comment), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteMountPolicy) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getMountPolicies().empty());

  const std::string name = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 2;
  const uint64_t retrievePriority = 3;
  const uint64_t minRetrieveRequestAge = 4;
  const uint64_t maxDrivesAllowed = 5;
  const std::string &comment = "create mount policy";

  m_catalogue->createMountPolicy(
    m_cliSI,
    name,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    comment);

  const std::list<common::dataStructures::MountPolicy> mountPolicies =
    m_catalogue->getMountPolicies();

  ASSERT_EQ(1, mountPolicies.size());

  const common::dataStructures::MountPolicy mountPolicy = mountPolicies.front();

  ASSERT_EQ(name, mountPolicy.name);

  ASSERT_EQ(archivePriority, mountPolicy.archivePriority);
  ASSERT_EQ(minArchiveRequestAge, mountPolicy.archiveMinRequestAge);

  ASSERT_EQ(retrievePriority, mountPolicy.retrievePriority);
  ASSERT_EQ(minRetrieveRequestAge, mountPolicy.retrieveMinRequestAge);

  ASSERT_EQ(maxDrivesAllowed, mountPolicy.maxDrivesAllowed);

  ASSERT_EQ(comment, mountPolicy.comment);

  const common::dataStructures::EntryLog creationLog = mountPolicy.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    mountPolicy.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  m_catalogue->deleteMountPolicy(name);

  ASSERT_TRUE(m_catalogue->getMountPolicies().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteMountPolicy_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getMountPolicies().empty());
  ASSERT_THROW(m_catalogue->deleteMountPolicy("non_existant_mount_policy"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createRequesterMountRule) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 4;
  const uint64_t retrievePriority = 5;
  const uint64_t minRetrieveRequestAge = 8;
  const uint64_t maxDrivesAllowed = 9;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester";
  const std::string requesterName = "requester_name";
  m_catalogue->createRequesterMountRule(m_cliSI, mountPolicyName, requesterName, comment);

  const std::list<common::dataStructures::RequesterMountRule> rules = m_catalogue->getRequesterMountRules();
  ASSERT_EQ(1, rules.size());

  const common::dataStructures::RequesterMountRule rule = rules.front();

  ASSERT_EQ(requesterName, rule.name);
  ASSERT_EQ(mountPolicyName, rule.mountPolicy);
  ASSERT_EQ(comment, rule.comment);
  ASSERT_EQ(m_cliSI.username, rule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, rule.creationLog.host);
  ASSERT_EQ(rule.creationLog, rule.lastModificationLog);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createRequesterMountRule_same_twice) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 4;
  const uint64_t retrievePriority = 5;
  const uint64_t minRetrieveRequestAge = 8;
  const uint64_t maxDrivesAllowed = 9;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester";
  const std::string requesterName = "requester_name";
  m_catalogue->createRequesterMountRule(m_cliSI, mountPolicyName, requesterName, comment);
  ASSERT_THROW(m_catalogue->createRequesterMountRule(m_cliSI, mountPolicyName, requesterName, comment),
    catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createRequesterMountRule_non_existant_mount_policy) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());

  const std::string comment = "create mount rule for requester";
  const std::string mountPolicyName = "non_existant_mount_policy";
  const std::string requesterName = "requester_name";
  ASSERT_THROW(m_catalogue->createRequesterMountRule(m_cliSI, mountPolicyName, requesterName, comment),
    catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteRequesterMountRule) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 4;
  const uint64_t retrievePriority = 5;
  const uint64_t minRetrieveRequestAge = 8;
  const uint64_t maxDrivesAllowed = 9;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester";
  const std::string requesterName = "requester_name";
  m_catalogue->createRequesterMountRule(m_cliSI, mountPolicyName, requesterName, comment);

  const std::list<common::dataStructures::RequesterMountRule> rules = m_catalogue->getRequesterMountRules();
  ASSERT_EQ(1, rules.size());

  const common::dataStructures::RequesterMountRule rule = rules.front();

  ASSERT_EQ(requesterName, rule.name);
  ASSERT_EQ(mountPolicyName, rule.mountPolicy);
  ASSERT_EQ(comment, rule.comment);
  ASSERT_EQ(m_cliSI.username, rule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, rule.creationLog.host);
  ASSERT_EQ(rule.creationLog, rule.lastModificationLog);

  m_catalogue->deleteRequesterMountRule(requesterName);
  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteRequesterMountRule_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());
  ASSERT_THROW(m_catalogue->deleteRequesterMountRule("non_existant_requester"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createRequesterGroupMountRule) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterGroupMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 4;
  const uint64_t retrievePriority = 5;
  const uint64_t minRetrieveRequestAge = 8;
  const uint64_t maxDrivesAllowed = 9;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester group";
  const std::string requesterGroupName = "requester_group_name";
  m_catalogue->createRequesterGroupMountRule(m_cliSI, mountPolicyName, requesterGroupName, comment);

  const std::list<common::dataStructures::RequesterGroupMountRule> rules =
    m_catalogue->getRequesterGroupMountRules();
  ASSERT_EQ(1, rules.size());

  const common::dataStructures::RequesterGroupMountRule rule = rules.front();

  ASSERT_EQ(requesterGroupName, rule.name);
  ASSERT_EQ(mountPolicyName, rule.mountPolicy);
  ASSERT_EQ(comment, rule.comment);
  ASSERT_EQ(m_cliSI.username, rule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, rule.creationLog.host);
  ASSERT_EQ(rule.creationLog, rule.lastModificationLog);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createRequesterGroupMountRule_same_twice) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterGroupMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 4;
  const uint64_t retrievePriority = 5;
  const uint64_t minRetrieveRequestAge = 8;
  const uint64_t maxDrivesAllowed = 9;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester group";
  const std::string requesterGroupName = "requester_group_name";
  m_catalogue->createRequesterGroupMountRule(m_cliSI, mountPolicyName, requesterGroupName, comment);
  ASSERT_THROW(m_catalogue->createRequesterGroupMountRule(m_cliSI, mountPolicyName, requesterGroupName, comment),
    catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, createRequesterGroupMountRule_non_existant_mount_policy) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterGroupMountRules().empty());

  const std::string comment = "create mount rule for requester group";
  const std::string mountPolicyName = "non_existant_mount_policy";
  const std::string requesterGroupName = "requester_group_name";
  ASSERT_THROW(m_catalogue->createRequesterGroupMountRule(m_cliSI, mountPolicyName, requesterGroupName, comment),
    catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteRequesterGroupMountRule) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterGroupMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 4;
  const uint64_t retrievePriority = 5;
  const uint64_t minRetrieveRequestAge = 8;
  const uint64_t maxDrivesAllowed = 9;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester group";
  const std::string requesterGroupName = "requester_group_name";
  m_catalogue->createRequesterGroupMountRule(m_cliSI, mountPolicyName, requesterGroupName, comment);

  const std::list<common::dataStructures::RequesterGroupMountRule> rules = m_catalogue->getRequesterGroupMountRules();
  ASSERT_EQ(1, rules.size());

  const common::dataStructures::RequesterGroupMountRule rule = rules.front();

  ASSERT_EQ(requesterGroupName, rule.name);
  ASSERT_EQ(mountPolicyName, rule.mountPolicy);
  ASSERT_EQ(comment, rule.comment);
  ASSERT_EQ(m_cliSI.username, rule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, rule.creationLog.host);
  ASSERT_EQ(rule.creationLog, rule.lastModificationLog);

  m_catalogue->deleteRequesterGroupMountRule(requesterGroupName);
  ASSERT_TRUE(m_catalogue->getRequesterGroupMountRules().empty());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteRequesterGroupMountRule_non_existant) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterGroupMountRules().empty());
  ASSERT_THROW(m_catalogue->deleteRequesterGroupMountRule("non_existant_requester_group"), catalogue::UserError);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, prepareForNewFile_requester_mount_rule) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 2;
  const uint64_t retrievePriority = 3;
  const uint64_t minRetrieveRequestAge = 4;
  const uint64_t maxDrivesAllowed = 5;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester";
  const std::string requesterName = "requester_name";
  m_catalogue->createRequesterMountRule(m_cliSI, mountPolicyName, requesterName, comment);

  const std::list<common::dataStructures::RequesterMountRule> rules = m_catalogue->getRequesterMountRules();
  ASSERT_EQ(1, rules.size());

  const common::dataStructures::RequesterMountRule rule = rules.front();

  ASSERT_EQ(requesterName, rule.name);
  ASSERT_EQ(mountPolicyName, rule.mountPolicy);
  ASSERT_EQ(comment, rule.comment);
  ASSERT_EQ(m_cliSI.username, rule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, rule.creationLog.host);
  ASSERT_EQ(rule.creationLog, rule.lastModificationLog);

  ASSERT_TRUE(m_catalogue->getArchiveRoutes().empty());

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies,
    "create storage class");

  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted, "create tape pool");

  const uint64_t copyNb = 1;
  const std::string archiveRouteComment = "create archive route";
  m_catalogue->createArchiveRoute(m_cliSI, storageClassName, copyNb, tapePoolName, archiveRouteComment);

  const std::list<common::dataStructures::ArchiveRoute> routes = m_catalogue->getArchiveRoutes();

  ASSERT_EQ(1, routes.size());

  const common::dataStructures::ArchiveRoute route = routes.front();
  ASSERT_EQ(storageClassName, route.storageClassName);
  ASSERT_EQ(copyNb, route.copyNb);
  ASSERT_EQ(tapePoolName, route.tapePoolName);
  ASSERT_EQ(archiveRouteComment, route.comment);

  const common::dataStructures::EntryLog creationLog = route.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog = route.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  common::dataStructures::TapeCopyToPoolMap copyToPoolMap = m_catalogue->getTapeCopyToPoolMap(storageClassName);
  ASSERT_EQ(1, copyToPoolMap.size());
  std::pair<uint64_t, std::string> maplet = *(copyToPoolMap.begin());
  ASSERT_EQ(copyNb, maplet.first);
  ASSERT_EQ(tapePoolName, maplet.second);

  common::dataStructures::UserIdentity userIdentity;
  userIdentity.name = requesterName;
  userIdentity.group = "group";
  uint64_t expectedArchiveFileId = 0;
  for(uint64_t i = 0; i<10; i++) {
    const common::dataStructures::ArchiveFileQueueCriteria queueCriteria =
      m_catalogue->prepareForNewFile(storageClassName, userIdentity);

    if(0 == i) {
      expectedArchiveFileId = queueCriteria.fileId;
    } else {
      expectedArchiveFileId++;
    }
    ASSERT_EQ(expectedArchiveFileId, queueCriteria.fileId);

    ASSERT_EQ(1, queueCriteria.copyToPoolMap.size());
    ASSERT_EQ(copyNb, queueCriteria.copyToPoolMap.begin()->first);
    ASSERT_EQ(tapePoolName, queueCriteria.copyToPoolMap.begin()->second);
    ASSERT_EQ(archivePriority, queueCriteria.mountPolicy.archivePriority);
    ASSERT_EQ(minArchiveRequestAge, queueCriteria.mountPolicy.archiveMinRequestAge);
    ASSERT_EQ(maxDrivesAllowed, queueCriteria.mountPolicy.maxDrivesAllowed);
  }
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, prepareForNewFile_requester_group_mount_rule) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 2;
  const uint64_t retrievePriority = 3;
  const uint64_t minRetrieveRequestAge = 4;
  const uint64_t maxDrivesAllowed = 5;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester group";
  const std::string requesterGroupName = "requester_group_name";
  m_catalogue->createRequesterGroupMountRule(m_cliSI, mountPolicyName, requesterGroupName, comment);

  const std::list<common::dataStructures::RequesterGroupMountRule> rules = m_catalogue->getRequesterGroupMountRules();
  ASSERT_EQ(1, rules.size());

  const common::dataStructures::RequesterGroupMountRule rule = rules.front();

  ASSERT_EQ(requesterGroupName, rule.name);
  ASSERT_EQ(mountPolicyName, rule.mountPolicy);
  ASSERT_EQ(comment, rule.comment);
  ASSERT_EQ(m_cliSI.username, rule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, rule.creationLog.host);
  ASSERT_EQ(rule.creationLog, rule.lastModificationLog);

  ASSERT_TRUE(m_catalogue->getArchiveRoutes().empty());

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");

  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted, "create tape pool");

  const uint64_t copyNb = 1;
  const std::string archiveRouteComment = "create archive route";
  m_catalogue->createArchiveRoute(m_cliSI, storageClassName, copyNb, tapePoolName, archiveRouteComment);

  const std::list<common::dataStructures::ArchiveRoute> routes = m_catalogue->getArchiveRoutes();

  ASSERT_EQ(1, routes.size());

  const common::dataStructures::ArchiveRoute route = routes.front();
  ASSERT_EQ(storageClassName, route.storageClassName);
  ASSERT_EQ(copyNb, route.copyNb);
  ASSERT_EQ(tapePoolName, route.tapePoolName);
  ASSERT_EQ(archiveRouteComment, route.comment);

  const common::dataStructures::EntryLog creationLog = route.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog = route.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  common::dataStructures::TapeCopyToPoolMap copyToPoolMap =
    m_catalogue->getTapeCopyToPoolMap(storageClassName);
  ASSERT_EQ(1, copyToPoolMap.size());
  std::pair<uint64_t, std::string> maplet = *(copyToPoolMap.begin());
  ASSERT_EQ(copyNb, maplet.first);
  ASSERT_EQ(tapePoolName, maplet.second);

  common::dataStructures::UserIdentity userIdentity;
  userIdentity.name = "username";
  userIdentity.group = requesterGroupName;
  uint64_t expectedArchiveFileId = 0;
  for(uint64_t i = 0; i<10; i++) {
    const common::dataStructures::ArchiveFileQueueCriteria queueCriteria =
      m_catalogue->prepareForNewFile(storageClassName, userIdentity);

    if(0 == i) {
      expectedArchiveFileId = queueCriteria.fileId;
    } else {
      expectedArchiveFileId++;
    }
    ASSERT_EQ(expectedArchiveFileId, queueCriteria.fileId);

    ASSERT_EQ(1, queueCriteria.copyToPoolMap.size());
    ASSERT_EQ(copyNb, queueCriteria.copyToPoolMap.begin()->first);
    ASSERT_EQ(tapePoolName, queueCriteria.copyToPoolMap.begin()->second);
    ASSERT_EQ(archivePriority, queueCriteria.mountPolicy.archivePriority);
    ASSERT_EQ(minArchiveRequestAge, queueCriteria.mountPolicy.archiveMinRequestAge);
    ASSERT_EQ(maxDrivesAllowed, queueCriteria.mountPolicy.maxDrivesAllowed);
  }
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, prepareForNewFile_requester_mount_rule_overide) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getRequesterMountRules().empty());

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 2;
  const uint64_t retrievePriority = 3;
  const uint64_t minRetrieveRequestAge = 4;
  const uint64_t maxDrivesAllowed = 5;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string requesterRuleComment = "create mount rule for requester";
  const std::string requesterName = "requester_name";
  m_catalogue->createRequesterMountRule(m_cliSI, mountPolicyName, requesterName, requesterRuleComment);

  const std::list<common::dataStructures::RequesterMountRule> requesterRules = m_catalogue->getRequesterMountRules();
  ASSERT_EQ(1, requesterRules.size());

  const common::dataStructures::RequesterMountRule requesterRule = requesterRules.front();

  ASSERT_EQ(requesterName, requesterRule.name);
  ASSERT_EQ(mountPolicyName, requesterRule.mountPolicy);
  ASSERT_EQ(requesterRuleComment, requesterRule.comment);
  ASSERT_EQ(m_cliSI.username, requesterRule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, requesterRule.creationLog.host);
  ASSERT_EQ(requesterRule.creationLog, requesterRule.lastModificationLog);

  const std::string requesterGroupRuleComment = "create mount rule for requester group";
  const std::string requesterGroupName = "requester_group_name";
  m_catalogue->createRequesterGroupMountRule(m_cliSI, mountPolicyName, requesterName, requesterGroupRuleComment);

  const std::list<common::dataStructures::RequesterGroupMountRule> requesterGroupRules =
    m_catalogue->getRequesterGroupMountRules();
  ASSERT_EQ(1, requesterGroupRules.size());

  const common::dataStructures::RequesterGroupMountRule requesterGroupRule = requesterGroupRules.front();

  ASSERT_EQ(requesterName, requesterGroupRule.name);
  ASSERT_EQ(mountPolicyName, requesterGroupRule.mountPolicy);
  ASSERT_EQ(requesterGroupRuleComment, requesterGroupRule.comment);
  ASSERT_EQ(m_cliSI.username, requesterGroupRule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, requesterGroupRule.creationLog.host);
  ASSERT_EQ(requesterGroupRule.creationLog, requesterGroupRule.lastModificationLog);

  ASSERT_TRUE(m_catalogue->getArchiveRoutes().empty());

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies,
    "create storage class");

  const std::string tapePoolName = "tape_pool";
  const uint64_t nbPartialTapes = 2;
  const bool is_encrypted = true;
  m_catalogue->createTapePool(m_cliSI, tapePoolName, nbPartialTapes, is_encrypted, "create tape pool");

  const uint64_t copyNb = 1;
  const std::string archiveRouteComment = "create archive route";
  m_catalogue->createArchiveRoute(m_cliSI, storageClassName, copyNb, tapePoolName, archiveRouteComment);

  const std::list<common::dataStructures::ArchiveRoute> routes = m_catalogue->getArchiveRoutes();

  ASSERT_EQ(1, routes.size());

  const common::dataStructures::ArchiveRoute route = routes.front();
  ASSERT_EQ(storageClassName, route.storageClassName);
  ASSERT_EQ(copyNb, route.copyNb);
  ASSERT_EQ(tapePoolName, route.tapePoolName);
  ASSERT_EQ(archiveRouteComment, route.comment);

  const common::dataStructures::EntryLog creationLog = route.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog = route.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  common::dataStructures::TapeCopyToPoolMap copyToPoolMap = m_catalogue->getTapeCopyToPoolMap(storageClassName);
  ASSERT_EQ(1, copyToPoolMap.size());
  std::pair<uint64_t, std::string> maplet = *(copyToPoolMap.begin());
  ASSERT_EQ(copyNb, maplet.first);
  ASSERT_EQ(tapePoolName, maplet.second);

  common::dataStructures::UserIdentity userIdentity;
  userIdentity.name = requesterName;
  userIdentity.group = "group";
  uint64_t expectedArchiveFileId = 0;
  for(uint64_t i = 0; i<10; i++) {
    const common::dataStructures::ArchiveFileQueueCriteria queueCriteria =
      m_catalogue->prepareForNewFile(storageClassName, userIdentity);

    if(0 == i) {
      expectedArchiveFileId = queueCriteria.fileId;
    } else {
      expectedArchiveFileId++;
    }
    ASSERT_EQ(expectedArchiveFileId, queueCriteria.fileId);

    ASSERT_EQ(1, queueCriteria.copyToPoolMap.size());
    ASSERT_EQ(copyNb, queueCriteria.copyToPoolMap.begin()->first);
    ASSERT_EQ(tapePoolName, queueCriteria.copyToPoolMap.begin()->second);
    ASSERT_EQ(archivePriority, queueCriteria.mountPolicy.archivePriority);
    ASSERT_EQ(minArchiveRequestAge, queueCriteria.mountPolicy.archiveMinRequestAge);
    ASSERT_EQ(maxDrivesAllowed, queueCriteria.mountPolicy.maxDrivesAllowed);
  }
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, prepareToRetrieveFile) {
  using namespace cta;

  ASSERT_TRUE(m_catalogue->getTapes("", "", "", "", "", "", "", "").empty());

  const std::string vid1 = "VID123";
  const std::string vid2 = "VID456";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string createTapeComment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName, "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid1, logicalLibraryName, tapePoolName, encryptionKey, capacityInBytes,
    disabledValue, fullValue, createTapeComment);
  m_catalogue->createTape(m_cliSI, vid2, logicalLibraryName, tapePoolName, encryptionKey, capacityInBytes,
    disabledValue, fullValue, createTapeComment);

  const std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes("", "", "", "", "", "", "", "");
  const std::map<std::string, common::dataStructures::Tape> vidToTape = tapeListToMap(tapes);
  {
    auto it = vidToTape.find(vid1);
    const common::dataStructures::Tape &tape = it->second;
    ASSERT_EQ(vid1, tape.vid);
    ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
    ASSERT_EQ(tapePoolName, tape.tapePoolName);
    ASSERT_EQ(encryptionKey, tape.encryptionKey);
    ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
    ASSERT_TRUE(disabledValue == tape.disabled);
    ASSERT_TRUE(fullValue == tape.full);
    ASSERT_EQ(createTapeComment, tape.comment);

    const common::dataStructures::EntryLog creationLog = tape.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      tape.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }
  {
    auto it = vidToTape.find(vid2);
    const common::dataStructures::Tape &tape = it->second;
    ASSERT_EQ(vid2, tape.vid);
    ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
    ASSERT_EQ(tapePoolName, tape.tapePoolName);
    ASSERT_EQ(encryptionKey, tape.encryptionKey);
    ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
    ASSERT_TRUE(disabledValue == tape.disabled);
    ASSERT_TRUE(fullValue == tape.full);
    ASSERT_EQ(createTapeComment, tape.comment);

    const common::dataStructures::EntryLog creationLog = tape.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      tape.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  const uint64_t archiveFileId = 1234;

  ASSERT_FALSE(m_catalogue->getArchiveFileItor()->hasMore());
  ASSERT_THROW(m_catalogue->getArchiveFileById(archiveFileId), exception::Exception);

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");

  const uint64_t archiveFileSize = 1;

  catalogue::TapeFileWritten file1Written;
  file1Written.archiveFileId        = archiveFileId;
  file1Written.diskInstance         = "PUBLIC";
  file1Written.diskFileId           = "5678";
  file1Written.diskFilePath         = "/public_dir/public_file";
  file1Written.diskFileUser         = "public_disk_user";
  file1Written.diskFileGroup        = "public_disk_group";
  file1Written.diskFileRecoveryBlob = "opaque_disk_file_recovery_contents";
  file1Written.size                 = archiveFileSize;
  file1Written.storageClassName     = storageClassName;
  file1Written.vid                  = vid1;
  file1Written.fSeq                 = 1;
  file1Written.blockId              = 4321;
  file1Written.compressedSize       = 1;
  file1Written.copyNb               = 1;
  m_catalogue->fileWrittenToTape(file1Written);

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->getArchiveFileById(archiveFileId);

    ASSERT_EQ(file1Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file1Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file1Written.size, archiveFile.fileSize);
    ASSERT_EQ(file1Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file1Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file1Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file1Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file1Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file1Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(1, archiveFile.tapeFiles.size());
    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);
  }

  catalogue::TapeFileWritten file2Written;
  file2Written.archiveFileId        = file1Written.archiveFileId;
  file2Written.diskInstance         = file1Written.diskInstance;
  file2Written.diskFileId           = file1Written.diskFileId;
  file2Written.diskFilePath         = file1Written.diskFilePath;
  file2Written.diskFileUser         = file1Written.diskFileUser;
  file2Written.diskFileGroup        = file1Written.diskFileGroup;
  file2Written.diskFileRecoveryBlob = file1Written.diskFileRecoveryBlob;
  file2Written.size                 = archiveFileSize;
  file2Written.storageClassName     = storageClassName;
  file2Written.vid                  = vid2;
  file2Written.fSeq                 = 1;
  file2Written.blockId              = 4331;
  file2Written.compressedSize       = 1;
  file2Written.copyNb               = 2;
  m_catalogue->fileWrittenToTape(file2Written);

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->getArchiveFileById(archiveFileId);

    ASSERT_EQ(file2Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file2Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file2Written.size, archiveFile.fileSize);
    ASSERT_EQ(file2Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file2Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file2Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file2Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file2Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file2Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(2, archiveFile.tapeFiles.size());

    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);

    auto copyNbToTapeFile2Itor = archiveFile.tapeFiles.find(2);
    ASSERT_FALSE(copyNbToTapeFile2Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile2 = copyNbToTapeFile2Itor->second;
    ASSERT_EQ(file2Written.vid, tapeFile2.vid);
    ASSERT_EQ(file2Written.fSeq, tapeFile2.fSeq);
    ASSERT_EQ(file2Written.blockId, tapeFile2.blockId);
    ASSERT_EQ(file2Written.compressedSize, tapeFile2.compressedSize);
  }

  const std::string mountPolicyName = "mount_policy";
  const uint64_t archivePriority = 1;
  const uint64_t minArchiveRequestAge = 2;
  const uint64_t retrievePriority = 3;
  const uint64_t minRetrieveRequestAge = 4;
  const uint64_t maxDrivesAllowed = 5;

  m_catalogue->createMountPolicy(
    m_cliSI,
    mountPolicyName,
    archivePriority,
    minArchiveRequestAge,
    retrievePriority,
    minRetrieveRequestAge,
    maxDrivesAllowed,
    "create mount policy");

  const std::string comment = "create mount rule for requester";
  const std::string requesterName = "requester_name";
  m_catalogue->createRequesterMountRule(m_cliSI, mountPolicyName, requesterName, comment);

  const std::list<common::dataStructures::RequesterMountRule> rules = m_catalogue->getRequesterMountRules();
  ASSERT_EQ(1, rules.size());

  const common::dataStructures::RequesterMountRule rule = rules.front();

  ASSERT_EQ(requesterName, rule.name);
  ASSERT_EQ(mountPolicyName, rule.mountPolicy);
  ASSERT_EQ(comment, rule.comment);
  ASSERT_EQ(m_cliSI.username, rule.creationLog.username);
  ASSERT_EQ(m_cliSI.host, rule.creationLog.host);
  ASSERT_EQ(rule.creationLog, rule.lastModificationLog);

  common::dataStructures::UserIdentity userIdentity;
  userIdentity.name = requesterName;
  userIdentity.group = "group";
  const common::dataStructures::RetrieveFileQueueCriteria queueCriteria =
    m_catalogue->prepareToRetrieveFile(archiveFileId, userIdentity);

  ASSERT_EQ(2, queueCriteria.tapeFiles.size());
  ASSERT_EQ(archivePriority, queueCriteria.mountPolicy.archivePriority);
  ASSERT_EQ(minArchiveRequestAge, queueCriteria.mountPolicy.archiveMinRequestAge);
  ASSERT_EQ(maxDrivesAllowed, queueCriteria.mountPolicy.maxDrivesAllowed);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, getArchiveFileItor_zero_prefetch) {
  using namespace cta;

  ASSERT_THROW(m_catalogue->getArchiveFileItor(catalogue::ArchiveFileSearchCriteria(), 0), exception::Exception);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, fileWrittenToTape_many_archive_files) {
  using namespace cta;

  const std::string vid = "VID123";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName, "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid, logicalLibraryName, tapePoolName, encryptionKey, capacityInBytes, disabledValue,
    fullValue, comment);

  const std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes("", "", "", "", "", "", "", "");

  ASSERT_EQ(1, tapes.size());

  const std::map<std::string, common::dataStructures::Tape> vidToTape = tapeListToMap(tapes);
  {
    auto it = vidToTape.find(vid);
    const common::dataStructures::Tape &tape = it->second;
    ASSERT_EQ(vid, tape.vid);
    ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
    ASSERT_EQ(tapePoolName, tape.tapePoolName);
    ASSERT_EQ(encryptionKey, tape.encryptionKey);
    ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
    ASSERT_TRUE(disabledValue == tape.disabled);
    ASSERT_TRUE(fullValue == tape.full);
    ASSERT_EQ(comment, tape.comment);

    const common::dataStructures::EntryLog creationLog = tape.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      tape.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 1;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");

  ASSERT_FALSE(m_catalogue->getArchiveFileItor()->hasMore());
  const uint64_t nbArchiveFiles = 100;
  for(uint64_t i = 1; i <= nbArchiveFiles; i++) {
    const uint64_t archiveFileSize = 1;
    std::ostringstream diskFileId;
    diskFileId << (12345677 + i);
    std::ostringstream diskFilePath;
    diskFilePath << "/public_dir/public_file_" << i;

    catalogue::TapeFileWritten fileWritten;
    fileWritten.archiveFileId = i;
    fileWritten.diskInstance = "PUBLIC";
    fileWritten.diskFileId = diskFileId.str();
    fileWritten.diskFilePath = diskFilePath.str();
    fileWritten.diskFileUser = "public_disk_user";
    fileWritten.diskFileGroup = "public_disk_group";
    fileWritten.diskFileRecoveryBlob = "opaque_disk_file_recovery_contents";
    fileWritten.size = archiveFileSize;
    fileWritten.storageClassName = storageClassName;
    fileWritten.vid = vid;
    fileWritten.fSeq = i;
    fileWritten.blockId = i * 100;
    fileWritten.compressedSize = 1;
    fileWritten.copyNb = 1;
    m_catalogue->fileWrittenToTape(fileWritten);

    const std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes("", "", "", "", "", "", "", "");
    ASSERT_EQ(1, tapes.size());
    const common::dataStructures::Tape &tape = tapes.front();
    ASSERT_EQ(vid, tape.vid);
    ASSERT_EQ(i, tape.lastFSeq);
  }

  std::list<uint64_t> prefetches = {1, 2, 3, nbArchiveFiles - 1, nbArchiveFiles, nbArchiveFiles+1, nbArchiveFiles*2};
  for(auto prefetch: prefetches) {
    std::unique_ptr<catalogue::ArchiveFileItor> archiveFileItor =
      m_catalogue->getArchiveFileItor(catalogue::ArchiveFileSearchCriteria(), prefetch);
    std::map<uint64_t, common::dataStructures::ArchiveFile> m = archiveFileItorToMap(*archiveFileItor);
    ASSERT_EQ(nbArchiveFiles, m.size());

    for(uint64_t i = 1; i <= nbArchiveFiles; i++) {
      const uint64_t archiveFileSize = 1;
      std::ostringstream diskFileId;
      diskFileId << (12345677 + i);
      std::ostringstream diskFilePath;
      diskFilePath << "/public_dir/public_file_" << i;

      catalogue::TapeFileWritten fileWritten;
      fileWritten.archiveFileId = i;
      fileWritten.diskInstance = "PUBLIC";
      fileWritten.diskFileId = diskFileId.str();
      fileWritten.diskFilePath = diskFilePath.str();
      fileWritten.diskFileUser = "public_disk_user";
      fileWritten.diskFileGroup = "public_disk_group";
      fileWritten.diskFileRecoveryBlob = "opaque_disk_file_recovery_contents";
      fileWritten.size = archiveFileSize;
      fileWritten.storageClassName = storageClassName;
      fileWritten.vid = vid;
      fileWritten.fSeq = i;
      fileWritten.blockId = i * 100;
      fileWritten.compressedSize = 1;
      fileWritten.copyNb = 1;

      const auto idAndFile = m.find(i);
      ASSERT_FALSE(m.end() == idAndFile);
      const common::dataStructures::ArchiveFile archiveFile = idAndFile->second;
      ASSERT_EQ(fileWritten.archiveFileId, archiveFile.archiveFileID);
      ASSERT_EQ(fileWritten.diskInstance, archiveFile.diskInstance);
      ASSERT_EQ(fileWritten.diskFileId, archiveFile.diskFileId);
      ASSERT_EQ(fileWritten.diskFilePath, archiveFile.diskFileInfo.path);
      ASSERT_EQ(fileWritten.diskFileUser, archiveFile.diskFileInfo.owner);
      ASSERT_EQ(fileWritten.diskFileGroup, archiveFile.diskFileInfo.group);
      ASSERT_EQ(fileWritten.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);
      ASSERT_EQ(fileWritten.size, archiveFile.fileSize);
      ASSERT_EQ(fileWritten.storageClassName, archiveFile.storageClass);
      ASSERT_EQ(1, archiveFile.tapeFiles.size());
      ASSERT_EQ(fileWritten.vid, archiveFile.tapeFiles.begin()->second.vid);
      ASSERT_EQ(fileWritten.fSeq, archiveFile.tapeFiles.begin()->second.fSeq);
      ASSERT_EQ(fileWritten.blockId, archiveFile.tapeFiles.begin()->second.blockId);
      ASSERT_EQ(fileWritten.compressedSize, archiveFile.tapeFiles.begin()->second.compressedSize);
      ASSERT_EQ(fileWritten.copyNb, archiveFile.tapeFiles.begin()->first);
      ASSERT_EQ(fileWritten.copyNb, archiveFile.tapeFiles.begin()->second.copyNb);
    }
  }
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, fileWrittenToTape_2_tape_files_different_tapes) {
  using namespace cta;

  const std::string vid1 = "VID123";
  const std::string vid2 = "VID456";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName,
                                    "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid1, logicalLibraryName, tapePoolName,
                          encryptionKey, capacityInBytes, disabledValue, fullValue,
                          comment);
  m_catalogue->createTape(m_cliSI, vid2, logicalLibraryName, tapePoolName,
                          encryptionKey, capacityInBytes, disabledValue, fullValue,
                          comment);

  const std::list<common::dataStructures::Tape> tapes =
    m_catalogue->getTapes("", "", "", "", "", "", "", "");

  ASSERT_EQ(2, tapes.size());

  const std::map<std::string, common::dataStructures::Tape> vidToTape = tapeListToMap(tapes);
  {
    auto it = vidToTape.find(vid1);
    const common::dataStructures::Tape &tape = it->second;
    ASSERT_EQ(vid1, tape.vid);
    ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
    ASSERT_EQ(tapePoolName, tape.tapePoolName);
    ASSERT_EQ(encryptionKey, tape.encryptionKey);
    ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
    ASSERT_TRUE(disabledValue == tape.disabled);
    ASSERT_TRUE(fullValue == tape.full);
    ASSERT_EQ(comment, tape.comment);

    const common::dataStructures::EntryLog creationLog = tape.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      tape.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }
  {
    auto it = vidToTape.find(vid2);
    const common::dataStructures::Tape &tape = it->second;
    ASSERT_EQ(vid2, tape.vid);
    ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
    ASSERT_EQ(tapePoolName, tape.tapePoolName);
    ASSERT_EQ(encryptionKey, tape.encryptionKey);
    ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
    ASSERT_TRUE(disabledValue == tape.disabled);
    ASSERT_TRUE(fullValue == tape.full);
    ASSERT_EQ(comment, tape.comment);

    const common::dataStructures::EntryLog creationLog = tape.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      tape.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  const uint64_t archiveFileId = 1234;

  ASSERT_FALSE(m_catalogue->getArchiveFileItor()->hasMore());
  ASSERT_THROW(m_catalogue->getArchiveFileById(archiveFileId), exception::Exception);

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");

  const uint64_t archiveFileSize = 1;

  catalogue::TapeFileWritten file1Written;
  file1Written.archiveFileId        = archiveFileId;
  file1Written.diskInstance         = "PUBLIC";
  file1Written.diskFileId           = "5678";
  file1Written.diskFilePath         = "/public_dir/public_file";
  file1Written.diskFileUser         = "public_disk_user";
  file1Written.diskFileGroup        = "public_disk_group";
  file1Written.diskFileRecoveryBlob = "opaque_disk_file_recovery_contents";
  file1Written.size                 = archiveFileSize;
  file1Written.storageClassName     = storageClassName;
  file1Written.vid                  = vid1;
  file1Written.fSeq                 = 1;
  file1Written.blockId              = 4321;
  file1Written.compressedSize       = 1;
  file1Written.copyNb               = 1;
  m_catalogue->fileWrittenToTape(file1Written);

  {
    std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes(file1Written.vid, "", "", "", "", "", "", "");
    ASSERT_EQ(1, tapes.size());
    const common::dataStructures::Tape &tape = tapes.front();
    ASSERT_EQ(1, tape.lastFSeq);
  }

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->getArchiveFileById(archiveFileId);

    ASSERT_EQ(file1Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file1Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file1Written.size, archiveFile.fileSize);
    ASSERT_EQ(file1Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file1Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file1Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file1Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file1Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file1Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(1, archiveFile.tapeFiles.size());
    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);
  }

  catalogue::TapeFileWritten file2Written;
  file2Written.archiveFileId        = file1Written.archiveFileId;
  file2Written.diskInstance         = file1Written.diskInstance;
  file2Written.diskFileId           = file1Written.diskFileId;
  file2Written.diskFilePath         = file1Written.diskFilePath;
  file2Written.diskFileUser         = file1Written.diskFileUser;
  file2Written.diskFileGroup        = file1Written.diskFileGroup;
  file2Written.diskFileRecoveryBlob = file1Written.diskFileRecoveryBlob;
  file2Written.size                 = archiveFileSize;
  file2Written.storageClassName     = storageClassName;
  file2Written.vid                  = vid2;
  file2Written.fSeq                 = 1;
  file2Written.blockId              = 4331;
  file2Written.compressedSize       = 1;
  file2Written.copyNb               = 2;
  m_catalogue->fileWrittenToTape(file2Written);

  {
    ASSERT_EQ(2, m_catalogue->getTapes("", "", "", "", "", "", "", "").size());
    std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes(file2Written.vid, "", "", "", "", "", "", "");
    ASSERT_EQ(1, tapes.size());
    const common::dataStructures::Tape &tape = tapes.front();
    ASSERT_EQ(1, tape.lastFSeq);
  }

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->getArchiveFileById(archiveFileId);

    ASSERT_EQ(file2Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file2Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file2Written.size, archiveFile.fileSize);
    ASSERT_EQ(file2Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file2Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file2Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file2Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file2Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file2Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(2, archiveFile.tapeFiles.size());

    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);

    auto copyNbToTapeFile2Itor = archiveFile.tapeFiles.find(2);
    ASSERT_FALSE(copyNbToTapeFile2Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile2 = copyNbToTapeFile2Itor->second;
    ASSERT_EQ(file2Written.vid, tapeFile2.vid);
    ASSERT_EQ(file2Written.fSeq, tapeFile2.fSeq);
    ASSERT_EQ(file2Written.blockId, tapeFile2.blockId);
    ASSERT_EQ(file2Written.compressedSize, tapeFile2.compressedSize);
    ASSERT_EQ(file2Written.copyNb, tapeFile2.copyNb);
  }
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, fileWrittenToTape_2_tape_files_same_archive_file_same_tape) {
  using namespace cta;

  const std::string vid = "VID123";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName,
                                    "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid, logicalLibraryName, tapePoolName,
                          encryptionKey, capacityInBytes, disabledValue, fullValue,
                          comment);

  const std::list<common::dataStructures::Tape> tapes =
    m_catalogue->getTapes("", "", "", "", "", "", "", "");

  ASSERT_EQ(1, tapes.size());

  const std::map<std::string, common::dataStructures::Tape> vidToTape = tapeListToMap(tapes);
  {
    auto it = vidToTape.find(vid);
    const common::dataStructures::Tape &tape = it->second;
    ASSERT_EQ(vid, tape.vid);
    ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
    ASSERT_EQ(tapePoolName, tape.tapePoolName);
    ASSERT_EQ(encryptionKey, tape.encryptionKey);
    ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
    ASSERT_TRUE(disabledValue == tape.disabled);
    ASSERT_TRUE(fullValue == tape.full);
    ASSERT_EQ(comment, tape.comment);

    const common::dataStructures::EntryLog creationLog = tape.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      tape.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  const uint64_t archiveFileId = 1234;

  ASSERT_FALSE(m_catalogue->getArchiveFileItor()->hasMore());
  ASSERT_THROW(m_catalogue->getArchiveFileById(archiveFileId), exception::Exception);

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");

  const uint64_t archiveFileSize = 1;

  catalogue::TapeFileWritten file1Written;
  file1Written.archiveFileId        = archiveFileId;
  file1Written.diskInstance         = "PUBLIC";
  file1Written.diskFileId           = "5678";
  file1Written.diskFilePath         = "/public_dir/public_file";
  file1Written.diskFileUser         = "public_disk_user";
  file1Written.diskFileGroup        = "public_disk_group";
  file1Written.diskFileRecoveryBlob = "opaque_disk_file_recovery_contents";
  file1Written.size                 = archiveFileSize;
  file1Written.storageClassName     = storageClassName;
  file1Written.vid                  = vid;
  file1Written.fSeq                 = 1;
  file1Written.blockId              = 4321;
  file1Written.compressedSize       = 1;
  file1Written.copyNb               = 1;
  m_catalogue->fileWrittenToTape(file1Written);

  {
    std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes(file1Written.vid, "", "", "", "", "", "", "");
    ASSERT_EQ(1, tapes.size());
    const common::dataStructures::Tape &tape = tapes.front();
    ASSERT_EQ(1, tape.lastFSeq);
  }

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->getArchiveFileById(archiveFileId);

    ASSERT_EQ(file1Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file1Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file1Written.size, archiveFile.fileSize);
    ASSERT_EQ(file1Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file1Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file1Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file1Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file1Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file1Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(1, archiveFile.tapeFiles.size());
    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);
  }

  catalogue::TapeFileWritten file2Written;
  file2Written.archiveFileId        = file1Written.archiveFileId;
  file2Written.diskInstance         = file1Written.diskInstance;
  file2Written.diskFileId           = file1Written.diskFileId;
  file2Written.diskFilePath         = file1Written.diskFilePath;
  file2Written.diskFileUser         = file1Written.diskFileUser;
  file2Written.diskFileGroup        = file1Written.diskFileGroup;
  file2Written.diskFileRecoveryBlob = file1Written.diskFileRecoveryBlob;
  file2Written.size                 = archiveFileSize;
  file2Written.storageClassName     = storageClassName;
  file2Written.vid                  = vid;
  file2Written.fSeq                 = 2;
  file2Written.blockId              = 4331;
  file2Written.compressedSize       = 1;
  file2Written.copyNb               = 2;
  ASSERT_THROW(m_catalogue->fileWrittenToTape(file2Written), exception::Exception);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, fileWrittenToTape_2_tape_files_corrupted_diskFilePath) {
  using namespace cta;

  const std::string vid = "VID123";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName,
                                    "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid, logicalLibraryName, tapePoolName,
                          encryptionKey, capacityInBytes, disabledValue, fullValue,
                          comment);

  const std::list<common::dataStructures::Tape> tapes =
    m_catalogue->getTapes("", "", "", "", "", "", "", "");

  ASSERT_EQ(1, tapes.size());

  const common::dataStructures::Tape tape = tapes.front();
  ASSERT_EQ(vid, tape.vid);
  ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
  ASSERT_EQ(tapePoolName, tape.tapePoolName);
  ASSERT_EQ(encryptionKey, tape.encryptionKey);
  ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
  ASSERT_TRUE(disabledValue == tape.disabled);
  ASSERT_TRUE(fullValue == tape.full);
  ASSERT_EQ(comment, tape.comment);

  const common::dataStructures::EntryLog creationLog = tape.creationLog;
  ASSERT_EQ(m_cliSI.username, creationLog.username);
  ASSERT_EQ(m_cliSI.host, creationLog.host);

  const common::dataStructures::EntryLog lastModificationLog =
    tape.lastModificationLog;
  ASSERT_EQ(creationLog, lastModificationLog);

  const uint64_t archiveFileId = 1234;

  ASSERT_FALSE(m_catalogue->getArchiveFileItor()->hasMore());
  ASSERT_THROW(m_catalogue->getArchiveFileById(archiveFileId), exception::Exception);

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies,
    "create storage class");

  const uint64_t archiveFileSize = 1;

  catalogue::TapeFileWritten file1Written;
  file1Written.archiveFileId        = archiveFileId;
  file1Written.diskInstance         = "PUBLIC";
  file1Written.diskFileId           = "5678";
  file1Written.diskFilePath         = "/public_dir/public_file";
  file1Written.diskFileUser         = "public_disk_user";
  file1Written.diskFileGroup        = "public_disk_group";
  file1Written.diskFileRecoveryBlob = "opaque_disk_file_recovery_contents";
  file1Written.size                 = archiveFileSize;
  file1Written.storageClassName     = storageClassName;
  file1Written.vid                  = vid;
  file1Written.fSeq                 = 1;
  file1Written.blockId              = 4321;
  file1Written.compressedSize       = 1;
  file1Written.copyNb               = 1;
  m_catalogue->fileWrittenToTape(file1Written);

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->getArchiveFileById(archiveFileId);

    ASSERT_EQ(file1Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file1Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file1Written.size, archiveFile.fileSize);
    ASSERT_EQ(file1Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file1Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file1Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file1Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file1Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file1Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(1, archiveFile.tapeFiles.size());
    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);
  }

  catalogue::TapeFileWritten file2Written;
  file2Written.archiveFileId        = file1Written.archiveFileId;
  file2Written.diskInstance         = file1Written.diskInstance;
  file2Written.diskFileId           = file1Written.diskFileId;
  file2Written.diskFilePath         = "CORRUPTED disk file path";
  file2Written.diskFileUser         = file1Written.diskFileUser;
  file2Written.diskFileGroup        = file1Written.diskFileGroup;
  file2Written.diskFileRecoveryBlob = file1Written.diskFileRecoveryBlob;
  file2Written.size                 = archiveFileSize;
  file2Written.storageClassName     = storageClassName;
  file2Written.vid                  = "VID123";
  file2Written.fSeq                 = 2;
  file2Written.blockId              = 4331;
  file2Written.compressedSize       = 1;
  file2Written.copyNb               = 2;

  ASSERT_THROW(m_catalogue->fileWrittenToTape(file2Written), exception::Exception);
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteArchiveFile) {
  using namespace cta;

  const std::string vid1 = "VID123";
  const std::string vid2 = "VID456";
  const std::string logicalLibraryName = "logical_library_name";
  const std::string tapePoolName = "tape_pool_name";
  const std::string encryptionKey = "encryption_key";
  const uint64_t capacityInBytes = (uint64_t)10 * 1000 * 1000 * 1000 * 1000;
  const bool disabledValue = true;
  const bool fullValue = false;
  const std::string comment = "create tape";

  m_catalogue->createLogicalLibrary(m_cliSI, logicalLibraryName,
    "create logical library");
  m_catalogue->createTapePool(m_cliSI, tapePoolName, 2, true, "create tape pool");
  m_catalogue->createTape(m_cliSI, vid1, logicalLibraryName, tapePoolName,
    encryptionKey, capacityInBytes, disabledValue, fullValue,
    comment);
  m_catalogue->createTape(m_cliSI, vid2, logicalLibraryName, tapePoolName,
    encryptionKey, capacityInBytes, disabledValue, fullValue,
    comment);

  const std::list<common::dataStructures::Tape> tapes =
    m_catalogue->getTapes("", "", "", "", "", "", "", "");

  ASSERT_EQ(2, tapes.size());

  const std::map<std::string, common::dataStructures::Tape> vidToTape = tapeListToMap(tapes);
  {
    auto it = vidToTape.find(vid1);
    const common::dataStructures::Tape &tape = it->second;
    ASSERT_EQ(vid1, tape.vid);
    ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
    ASSERT_EQ(tapePoolName, tape.tapePoolName);
    ASSERT_EQ(encryptionKey, tape.encryptionKey);
    ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
    ASSERT_TRUE(disabledValue == tape.disabled);
    ASSERT_TRUE(fullValue == tape.full);
    ASSERT_EQ(comment, tape.comment);

    const common::dataStructures::EntryLog creationLog = tape.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      tape.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }
  {
    auto it = vidToTape.find(vid2);
    const common::dataStructures::Tape &tape = it->second;
    ASSERT_EQ(vid2, tape.vid);
    ASSERT_EQ(logicalLibraryName, tape.logicalLibraryName);
    ASSERT_EQ(tapePoolName, tape.tapePoolName);
    ASSERT_EQ(encryptionKey, tape.encryptionKey);
    ASSERT_EQ(capacityInBytes, tape.capacityInBytes);
    ASSERT_TRUE(disabledValue == tape.disabled);
    ASSERT_TRUE(fullValue == tape.full);
    ASSERT_EQ(comment, tape.comment);

    const common::dataStructures::EntryLog creationLog = tape.creationLog;
    ASSERT_EQ(m_cliSI.username, creationLog.username);
    ASSERT_EQ(m_cliSI.host, creationLog.host);

    const common::dataStructures::EntryLog lastModificationLog =
      tape.lastModificationLog;
    ASSERT_EQ(creationLog, lastModificationLog);
  }

  const uint64_t archiveFileId = 1234;

  ASSERT_FALSE(m_catalogue->getArchiveFileItor()->hasMore());
  ASSERT_THROW(m_catalogue->getArchiveFileById(archiveFileId), exception::Exception);

  const std::string storageClassName = "storage_class";
  const uint64_t nbCopies = 2;
  m_catalogue->createStorageClass(m_cliSI, storageClassName, nbCopies, "create storage class");

  const uint64_t archiveFileSize = 1;

  catalogue::TapeFileWritten file1Written;
  file1Written.archiveFileId        = archiveFileId;
  file1Written.diskInstance         = "PUBLIC";
  file1Written.diskFileId           = "5678";
  file1Written.diskFilePath         = "/public_dir/public_file";
  file1Written.diskFileUser         = "public_disk_user";
  file1Written.diskFileGroup        = "public_disk_group";
  file1Written.diskFileRecoveryBlob = "opaque_disk_file_recovery_contents";
  file1Written.size                 = archiveFileSize;
  file1Written.storageClassName     = storageClassName;
  file1Written.vid                  = vid1;
  file1Written.fSeq                 = 1;
  file1Written.blockId              = 4321;
  file1Written.compressedSize       = 1;
  file1Written.copyNb               = 1;
  m_catalogue->fileWrittenToTape(file1Written);

  {
    std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes(file1Written.vid, "", "", "", "", "", "", "");
    ASSERT_EQ(1, tapes.size());
    const common::dataStructures::Tape &tape = tapes.front();
    ASSERT_EQ(1, tape.lastFSeq);
  }

  {
    std::unique_ptr<catalogue::ArchiveFileItor> archiveFileItor = m_catalogue->getArchiveFileItor();
    const std::map<uint64_t, common::dataStructures::ArchiveFile> m = archiveFileItorToMap(*archiveFileItor);
    ASSERT_EQ(1, m.size());
    auto mItor = m.find(file1Written.archiveFileId);
    ASSERT_FALSE(m.end() == mItor);

    const common::dataStructures::ArchiveFile archiveFile = mItor->second;

    ASSERT_EQ(file1Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file1Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file1Written.size, archiveFile.fileSize);
    ASSERT_EQ(file1Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file1Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file1Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file1Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file1Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file1Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(1, archiveFile.tapeFiles.size());
    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);
  }

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->getArchiveFileById(archiveFileId);

    ASSERT_EQ(file1Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file1Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file1Written.size, archiveFile.fileSize);
    ASSERT_EQ(file1Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file1Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file1Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file1Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file1Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file1Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(1, archiveFile.tapeFiles.size());
    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);
  }

  catalogue::TapeFileWritten file2Written;
  file2Written.archiveFileId        = file1Written.archiveFileId;
  file2Written.diskInstance         = file1Written.diskInstance;
  file2Written.diskFileId           = file1Written.diskFileId;
  file2Written.diskFilePath         = file1Written.diskFilePath;
  file2Written.diskFileUser         = file1Written.diskFileUser;
  file2Written.diskFileGroup        = file1Written.diskFileGroup;
  file2Written.diskFileRecoveryBlob = file1Written.diskFileRecoveryBlob;
  file2Written.size                 = archiveFileSize;
  file2Written.storageClassName     = storageClassName;
  file2Written.vid                  = vid2;
  file2Written.fSeq                 = 1;
  file2Written.blockId              = 4331;
  file2Written.compressedSize       = 1;
  file2Written.copyNb               = 2;
  m_catalogue->fileWrittenToTape(file2Written);

  {
    ASSERT_EQ(2, m_catalogue->getTapes("", "", "", "", "", "", "", "").size());
    std::list<common::dataStructures::Tape> tapes = m_catalogue->getTapes(file2Written.vid, "", "", "", "", "", "", "");
    ASSERT_EQ(1, tapes.size());
    const common::dataStructures::Tape &tape = tapes.front();
    ASSERT_EQ(1, tape.lastFSeq);
  }

  {
    std::unique_ptr<catalogue::ArchiveFileItor> archiveFileItor = m_catalogue->getArchiveFileItor();
    const std::map<uint64_t, common::dataStructures::ArchiveFile> m = archiveFileItorToMap(*archiveFileItor);
    ASSERT_EQ(1, m.size());

    {
      auto mItor = m.find(file1Written.archiveFileId);
      ASSERT_FALSE(m.end() == mItor);

      const common::dataStructures::ArchiveFile archiveFile = mItor->second;

      ASSERT_EQ(file2Written.archiveFileId, archiveFile.archiveFileID);
      ASSERT_EQ(file2Written.diskFileId, archiveFile.diskFileId);
      ASSERT_EQ(file2Written.size, archiveFile.fileSize);
      ASSERT_EQ(file2Written.storageClassName, archiveFile.storageClass);

      ASSERT_EQ(file2Written.diskInstance, archiveFile.diskInstance);
      ASSERT_EQ(file2Written.diskFilePath, archiveFile.diskFileInfo.path);
      ASSERT_EQ(file2Written.diskFileUser, archiveFile.diskFileInfo.owner);
      ASSERT_EQ(file2Written.diskFileGroup, archiveFile.diskFileInfo.group);
      ASSERT_EQ(file2Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

      ASSERT_EQ(2, archiveFile.tapeFiles.size());

      auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
      ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
      const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
      ASSERT_EQ(file1Written.vid, tapeFile1.vid);
      ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
      ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
      ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
      ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);

      auto copyNbToTapeFile2Itor = archiveFile.tapeFiles.find(2);
      ASSERT_FALSE(copyNbToTapeFile2Itor == archiveFile.tapeFiles.end());
      const common::dataStructures::TapeFile &tapeFile2 = copyNbToTapeFile2Itor->second;
      ASSERT_EQ(file2Written.vid, tapeFile2.vid);
      ASSERT_EQ(file2Written.fSeq, tapeFile2.fSeq);
      ASSERT_EQ(file2Written.blockId, tapeFile2.blockId);
      ASSERT_EQ(file2Written.compressedSize, tapeFile2.compressedSize);
      ASSERT_EQ(file2Written.copyNb, tapeFile2.copyNb);
    }
  }

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->getArchiveFileById(archiveFileId);

    ASSERT_EQ(file2Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file2Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file2Written.size, archiveFile.fileSize);
    ASSERT_EQ(file2Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file2Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file2Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file2Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file2Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file2Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(2, archiveFile.tapeFiles.size());

    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);

    auto copyNbToTapeFile2Itor = archiveFile.tapeFiles.find(2);
    ASSERT_FALSE(copyNbToTapeFile2Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile2 = copyNbToTapeFile2Itor->second;
    ASSERT_EQ(file2Written.vid, tapeFile2.vid);
    ASSERT_EQ(file2Written.fSeq, tapeFile2.fSeq);
    ASSERT_EQ(file2Written.blockId, tapeFile2.blockId);
    ASSERT_EQ(file2Written.compressedSize, tapeFile2.compressedSize);
    ASSERT_EQ(file2Written.copyNb, tapeFile2.copyNb);
  }

  {
    const common::dataStructures::ArchiveFile archiveFile = m_catalogue->deleteArchiveFile(archiveFileId);

    ASSERT_EQ(file2Written.archiveFileId, archiveFile.archiveFileID);
    ASSERT_EQ(file2Written.diskFileId, archiveFile.diskFileId);
    ASSERT_EQ(file2Written.size, archiveFile.fileSize);
    ASSERT_EQ(file2Written.storageClassName, archiveFile.storageClass);

    ASSERT_EQ(file2Written.diskInstance, archiveFile.diskInstance);
    ASSERT_EQ(file2Written.diskFilePath, archiveFile.diskFileInfo.path);
    ASSERT_EQ(file2Written.diskFileUser, archiveFile.diskFileInfo.owner);
    ASSERT_EQ(file2Written.diskFileGroup, archiveFile.diskFileInfo.group);
    ASSERT_EQ(file2Written.diskFileRecoveryBlob, archiveFile.diskFileInfo.recoveryBlob);

    ASSERT_EQ(2, archiveFile.tapeFiles.size());

    auto copyNbToTapeFile1Itor = archiveFile.tapeFiles.find(1);
    ASSERT_FALSE(copyNbToTapeFile1Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile1 = copyNbToTapeFile1Itor->second;
    ASSERT_EQ(file1Written.vid, tapeFile1.vid);
    ASSERT_EQ(file1Written.fSeq, tapeFile1.fSeq);
    ASSERT_EQ(file1Written.blockId, tapeFile1.blockId);
    ASSERT_EQ(file1Written.compressedSize, tapeFile1.compressedSize);
    ASSERT_EQ(file1Written.copyNb, tapeFile1.copyNb);

    auto copyNbToTapeFile2Itor = archiveFile.tapeFiles.find(2);
    ASSERT_FALSE(copyNbToTapeFile2Itor == archiveFile.tapeFiles.end());
    const common::dataStructures::TapeFile &tapeFile2 = copyNbToTapeFile2Itor->second;
    ASSERT_EQ(file2Written.vid, tapeFile2.vid);
    ASSERT_EQ(file2Written.fSeq, tapeFile2.fSeq);
    ASSERT_EQ(file2Written.blockId, tapeFile2.blockId);
    ASSERT_EQ(file2Written.compressedSize, tapeFile2.compressedSize);
    ASSERT_EQ(file2Written.copyNb, tapeFile2.copyNb);
  }

  ASSERT_FALSE(m_catalogue->getArchiveFileItor()->hasMore());
}

TEST_F(cta_catalogue_InMemoryCatalogueTest, deleteArchiveFile_non_existant) {
  using namespace cta;

  ASSERT_FALSE(m_catalogue->getArchiveFileItor()->hasMore());
  ASSERT_THROW(m_catalogue->deleteArchiveFile(12345678), catalogue::UserError);
}

} // namespace unitTests

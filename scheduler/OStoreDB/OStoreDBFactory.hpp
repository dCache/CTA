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

#pragma once

#include "scheduler/SchedulerDatabaseFactory.hpp"
#include "scheduler/OStoreDB/OStoreDB.hpp"
#include "scheduler/StorageClass.hpp"
#include "scheduler/TapePool.hpp"
#include "scheduler/Tape.hpp"
#include "scheduler/LogicalLibrary.hpp"
#include "scheduler/ArchiveToTapeCopyRequest.hpp"
#include "scheduler/RetrieveFromTapeCopyRequest.hpp"
#include "objectstore/RootEntry.hpp"
#include "objectstore/Agent.hpp"
#include "objectstore/BackendVFS.hpp"

namespace cta {
  
namespace objectstore {
  class Backend;
}

namespace {
/**
 * A self contained class providing a "backend+SchedulerDB" in a box,
 * allowing creation (and deletion) of an object store for each instance of the
 * of the unit tests, as they require a fresh object store for each unit test.
 * It can be instantiated with both types of backends
 */
template <class BackendType>
class OStoreDBWrapper: public SchedulerDatabase {
public:
  OStoreDBWrapper(): m_backend(), m_OStoreDB(m_backend) {
    // We need to populate the root entry before using.
    objectstore::RootEntry re(m_backend);
    re.initialize();
    re.insert();
    objectstore::ScopedExclusiveLock rel(re);
    re.fetch();
    objectstore::Agent ag(m_backend);
    ag.generateName("OStoreDBFactory");
    ag.initialize();
    objectstore::CreationLog cl(1111, 1111, "systemhost", 
      time(NULL), "Initial creation of the  object store structures");
    re.addOrGetAgentRegisterPointerAndCommit(ag,cl);
    rel.release();
    ag.insertAndRegisterSelf();
    rel.lock(re);
    re.addOrGetDriveRegisterPointerAndCommit(ag, cl);
    rel.release();
    objectstore::ScopedExclusiveLock agl(ag);
    ag.removeAndUnregisterSelf();
  }
  
  virtual ~OStoreDBWrapper() throw() {}
  
  virtual void assertIsAdminOnAdminHost(const SecurityIdentity& id) const {
    m_OStoreDB.assertIsAdminOnAdminHost(id);
  }

  virtual void createAdminHost(const SecurityIdentity& requester, const std::string& hostName, const std::string& comment) {
    m_OStoreDB.createAdminHost(requester, hostName, comment);
  }

  virtual void createAdminUser(const SecurityIdentity& requester, const UserIdentity& user, const std::string& comment) {
    m_OStoreDB.createAdminUser(requester, user, comment);
  }

  virtual void createArchivalRoute(const SecurityIdentity& requester, const std::string& storageClassName, const uint16_t copyNb, const std::string& tapePoolName, const std::string& comment) {
    m_OStoreDB.createArchivalRoute(requester, storageClassName, copyNb, tapePoolName, comment);
  }

  virtual void createLogicalLibrary(const SecurityIdentity& requester, const std::string& name, const std::string& comment) {
    m_OStoreDB.createLogicalLibrary(requester, name, comment);
  }

  virtual void createStorageClass(const SecurityIdentity& requester, const std::string& name, const uint16_t nbCopies, const std::string& comment) {
    m_OStoreDB.createStorageClass(requester, name, nbCopies, comment);
  }

  virtual void createTape(const SecurityIdentity& requester, const std::string& vid, const std::string& logicalLibraryName, const std::string& tapePoolName, const uint64_t capacityInBytes, const std::string& comment) {
    m_OStoreDB.createTape(requester, vid, logicalLibraryName, tapePoolName, capacityInBytes, comment);
  }

  virtual void createTapePool(const SecurityIdentity& requester, const std::string& name, const uint32_t nbPartialTapes, const std::string& comment) {
    m_OStoreDB.createTapePool(requester, name, nbPartialTapes, comment);
  }

  virtual void deleteAdminHost(const SecurityIdentity& requester, const std::string& hostName) {
    m_OStoreDB.deleteAdminHost(requester, hostName);
  }

  virtual void deleteAdminUser(const SecurityIdentity& requester, const UserIdentity& user) {
    m_OStoreDB.deleteAdminUser(requester, user);
  }

  virtual void deleteArchivalRoute(const SecurityIdentity& requester, const std::string& storageClassName, const uint16_t copyNb) {
    m_OStoreDB.deleteArchivalRoute(requester, storageClassName, copyNb);
  }

  virtual void deleteArchiveRequest(const SecurityIdentity& requester, const std::string& archiveFile) {
    m_OStoreDB.deleteArchiveRequest(requester, archiveFile);
  }

  virtual void deleteLogicalLibrary(const SecurityIdentity& requester, const std::string& name) {
    m_OStoreDB.deleteLogicalLibrary(requester, name);
  }

  virtual void deleteRetrieveRequest(const SecurityIdentity& requester, const std::string& remoteFile) {
    m_OStoreDB.deleteRetrieveRequest(requester, remoteFile);
  }

  virtual void deleteStorageClass(const SecurityIdentity& requester, const std::string& name) {
    m_OStoreDB.deleteStorageClass(requester, name);
  }

  virtual void deleteTape(const SecurityIdentity& requester, const std::string& vid) {
    m_OStoreDB.deleteTape(requester, vid);
  }

  virtual void deleteTapePool(const SecurityIdentity& requester, const std::string& name) {
    m_OStoreDB.deleteTapePool(requester, name);
  }

  virtual void fileEntryCreatedInNS(const std::string& archiveFile) {
    m_OStoreDB.fileEntryCreatedInNS(archiveFile);
  }

  virtual std::list<AdminHost> getAdminHosts() const {
    return m_OStoreDB.getAdminHosts();
  }


  virtual std::list<AdminUser> getAdminUsers() const {
    return m_OStoreDB.getAdminUsers();
  }


  virtual std::list<ArchivalRoute> getArchivalRoutes(const std::string& storageClassName) const {
    return m_OStoreDB.getArchivalRoutes(storageClassName);
  }

  virtual std::list<ArchivalRoute> getArchivalRoutes() const{ 
    return m_OStoreDB.getArchivalRoutes();
  }

  virtual std::list<ArchiveToTapeCopyRequest> getArchiveRequests(const std::string& tapePoolName) const {
    return getArchiveRequests(tapePoolName);
  }

  virtual std::map<TapePool, std::list<ArchiveToTapeCopyRequest> > getArchiveRequests() const {
    return m_OStoreDB.getArchiveRequests();
  }

  virtual std::list<LogicalLibrary> getLogicalLibraries() const {
    return m_OStoreDB.getLogicalLibraries();
  }

  virtual std::map<Tape, std::list<RetrieveFromTapeCopyRequest> > getRetrieveRequests() const {
    return m_OStoreDB.getRetrieveRequests();
  }

  virtual std::list<RetrieveFromTapeCopyRequest> getRetrieveRequests(const std::string& vid) const {
    return m_OStoreDB.getRetrieveRequests(vid);
  }

  virtual StorageClass getStorageClass(const std::string& name) const {
    return m_OStoreDB.getStorageClass(name);
  }

  virtual std::list<StorageClass> getStorageClasses() const {
    return m_OStoreDB.getStorageClasses();
  }

  virtual std::list<TapePool> getTapePools() const {
    return m_OStoreDB.getTapePools();
  }

  virtual std::list<Tape> getTapes() const {
    return m_OStoreDB.getTapes();
  }

  virtual void queue(const ArchiveToFileRequest& rqst) {
    m_OStoreDB.queue(rqst);
  }

  virtual void queue(const ArchiveToDirRequest& rqst) {
    m_OStoreDB.queue(rqst);
  }

  virtual void queue(const RetrieveToFileRequest& rqst) {
    m_OStoreDB.queue(rqst);
  }

  virtual void queue(const RetrieveToDirRequest& rqst) {
    m_OStoreDB.queue(rqst);
  }
private:
  BackendType m_backend;
  cta::OStoreDB m_OStoreDB;
};

}

/**
 * A concrete implementation of a scheduler database factory that creates mock
 * scheduler database objects.
 */
template <class BackendType>
class OStoreDBFactory: public SchedulerDatabaseFactory {
public:
  /**
   * Constructor
   */
  OStoreDBFactory() {}
  
  /**
   * Destructor.
   */
  ~OStoreDBFactory() throw() {}
  
  /**
   * Returns a newly created scheduler database object.
   *
   * @return A newly created scheduler database object.
   */
  std::unique_ptr<SchedulerDatabase> create() const {
    return std::unique_ptr<SchedulerDatabase>(new OStoreDBWrapper<BackendType>());
  }
}; // class OStoreDBFactory

} // namespace cta
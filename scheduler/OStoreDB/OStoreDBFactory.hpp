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
#include "common/TapePool.hpp"
#include "common/dataStructures/SecurityIdentity.hpp"
#include "scheduler/LogicalLibrary.hpp"
#include "scheduler/RetrieveRequestDump.hpp"
#include "objectstore/RootEntry.hpp"
#include "objectstore/Agent.hpp"
#include "objectstore/AgentReference.hpp"
#include "objectstore/BackendVFS.hpp"
#include "objectstore/BackendRados.hpp"
#include "objectstore/BackendFactory.hpp"
#include "common/log/DummyLogger.hpp"
#include "catalogue/DummyCatalogue.hpp"
#include <memory>

namespace cta {
  
namespace objectstore {
  class Backend;

/**
 * As slight variation of the SchedulerDatabase interface allowing 
 * access to the underlying backend (this is hence OStoreDB specific).
 * This interface allows tests to go behind the scenes and create "broken" situations
 * and test recovery.
 */

class OStoreDBWrapperInterface: public SchedulerDatabase {
public:
  virtual objectstore::Backend & getBackend() = 0;
  virtual objectstore::AgentReference & getAgentReference() = 0;
  virtual cta::OStoreDB & getOstoreDB() = 0;
};

}

namespace {

/**
 * A self contained class providing a "backend+SchedulerDB" in a box,
 * allowing creation (and deletion) of an object store for each instance of the
 * of the unit tests, as they require a fresh object store for each unit test.
 * It can be instantiated with both types of backends
 */
template <class BackendType>
class OStoreDBWrapper: public cta::objectstore::OStoreDBWrapperInterface {
public:
  OStoreDBWrapper(const std::string &context, const std::string &URL = "");
  
  ~OStoreDBWrapper() throw () {}
  
  objectstore::Backend& getBackend() override { return *m_backend; }
  
  objectstore::AgentReference& getAgentReference() override { return m_agentReference; }
  
  cta::OStoreDB& getOstoreDB() override { return m_OStoreDB; }

  void ping() override {
    m_OStoreDB.ping();
  }

  void queueArchive(const std::string &instanceName, const cta::common::dataStructures::ArchiveRequest& request, const cta::common::dataStructures::ArchiveFileQueueCriteria& criteria, log::LogContext &logContext) override {
    return m_OStoreDB.queueArchive(instanceName, request, criteria, logContext);
  }

  
  void deleteArchiveRequest(const std::string &diskInstanceName, uint64_t archiveFileId) override {
    m_OStoreDB.deleteArchiveRequest(diskInstanceName, archiveFileId);
  }

  std::unique_ptr<cta::SchedulerDatabase::ArchiveToFileRequestCancelation> markArchiveRequestForDeletion(const common::dataStructures::SecurityIdentity &cliIdentity, uint64_t fileId) override {
    return m_OStoreDB.markArchiveRequestForDeletion(cliIdentity, fileId);
  }

  void deleteRetrieveRequest(const common::dataStructures::SecurityIdentity& cliIdentity, const std::string& remoteFile) override {
    m_OStoreDB.deleteRetrieveRequest(cliIdentity, remoteFile);
  }
  
  std::list<cta::common::dataStructures::RetrieveJob> getRetrieveJobs(const std::string& tapePoolName) const override {
    return m_OStoreDB.getRetrieveJobs(tapePoolName);
  }

  std::map<std::string, std::list<common::dataStructures::RetrieveJob> > getRetrieveJobs() const override {
    return m_OStoreDB.getRetrieveJobs();
  }

  std::map<std::string, std::list<common::dataStructures::ArchiveJob> > getArchiveJobs() const override {
    return m_OStoreDB.getArchiveJobs();
  }
  
  std::list<cta::common::dataStructures::ArchiveJob> getArchiveJobs(const std::string& tapePoolName) const override {
    return m_OStoreDB.getArchiveJobs(tapePoolName);
  }

  std::map<std::string, std::list<RetrieveRequestDump> > getRetrieveRequests() const override {
    return m_OStoreDB.getRetrieveRequests();
  }

  std::list<RetrieveRequestDump> getRetrieveRequestsByVid(const std::string& vid) const override {
    return m_OStoreDB.getRetrieveRequestsByVid(vid);
  }
  
  std::list<RetrieveRequestDump> getRetrieveRequestsByRequester(const std::string& requester) const override {
    return m_OStoreDB.getRetrieveRequestsByRequester(requester);
  }

  
  std::unique_ptr<TapeMountDecisionInfo> getMountInfo() override {
    return m_OStoreDB.getMountInfo();
  }

  std::unique_ptr<TapeMountDecisionInfo> getMountInfoNoLock() override {
    return m_OStoreDB.getMountInfoNoLock();
  }

  std::list<RetrieveQueueStatistics> getRetrieveQueueStatistics(const cta::common::dataStructures::RetrieveFileQueueCriteria& criteria,
          const std::set<std::string> & vidsToConsider) override {
    return m_OStoreDB.getRetrieveQueueStatistics(criteria, vidsToConsider);
  }

  std::string queueRetrieve(const common::dataStructures::RetrieveRequest& rqst,
    const common::dataStructures::RetrieveFileQueueCriteria &criteria) override {
    return m_OStoreDB.queueRetrieve(rqst, criteria);
  }
  
  std::list<cta::common::dataStructures::DriveState> getDriveStates() const override {
    return m_OStoreDB.getDriveStates();
  }
  
  void setDesiredDriveState(const std::string& drive, const common::dataStructures::DesiredDriveState & desiredState) override {
    return m_OStoreDB.setDesiredDriveState(drive, desiredState);
  }
  
  void reportDriveStatus(const common::dataStructures::DriveInfo& driveInfo, cta::common::dataStructures::MountType mountType, 
    common::dataStructures::DriveStatus status, time_t reportTime, uint64_t mountSessionId, uint64_t byteTransfered, 
    uint64_t filesTransfered, double latestBandwidth, const std::string& vid, const std::string& tapepool) override {
    m_OStoreDB.reportDriveStatus(driveInfo, mountType, status, reportTime, mountSessionId, byteTransfered, filesTransfered,
       latestBandwidth, vid, tapepool);
  }

private:
  std::unique_ptr <cta::objectstore::Backend> m_backend;
  std::unique_ptr <cta::log::Logger> m_logger;
  std::unique_ptr <cta::catalogue::Catalogue> m_catalogue;
  cta::OStoreDB m_OStoreDB;
  objectstore::AgentReference m_agentReference;
};

template <>
OStoreDBWrapper<cta::objectstore::BackendVFS>::OStoreDBWrapper(
        const std::string &context, const std::string &URL) :
m_backend(new cta::objectstore::BackendVFS()), m_logger(new cta::log::DummyLogger("")), 
m_catalogue(new cta::catalogue::DummyCatalogue(*m_logger)),
m_OStoreDB(*m_backend, *m_catalogue, *m_logger), m_agentReference("OStoreDBFactory") {
  // We need to populate the root entry before using.
  objectstore::RootEntry re(*m_backend);
  re.initialize();
  re.insert();
  objectstore::ScopedExclusiveLock rel(re);
  re.fetch();
  objectstore::Agent agent(m_agentReference.getAgentAddress(), *m_backend);
  agent.initialize();
  objectstore::EntryLogSerDeser cl("user0", "systemhost", time(NULL));
  re.addOrGetAgentRegisterPointerAndCommit(m_agentReference, cl);
  rel.release();
  agent.insertAndRegisterSelf();
  rel.lock(re);
  re.fetch();
  re.addOrGetDriveRegisterPointerAndCommit(m_agentReference, cl);
  re.addOrGetSchedulerGlobalLockAndCommit(m_agentReference, cl);
  rel.release();
  m_OStoreDB.setAgentReference(&m_agentReference);
}

template <>
OStoreDBWrapper<cta::objectstore::BackendRados>::OStoreDBWrapper(
        const std::string &context, const std::string &URL) :
m_backend(cta::objectstore::BackendFactory::createBackend(URL).release()), m_logger(new cta::log::DummyLogger("")), 
m_catalogue(new cta::catalogue::DummyCatalogue(*m_logger)),
m_OStoreDB(*m_backend, *m_catalogue, *m_logger),  m_agentReference("OStoreDBFactory") {
  // We need to first clean up possible left overs in the pool
  auto l = m_backend->list();
  for (auto o=l.begin(); o!=l.end(); o++) {
    try {
      m_backend->remove(*o);
    } catch (std::exception &) {}
  }
  // We need to populate the root entry before using.
  objectstore::RootEntry re(*m_backend);
  re.initialize();
  re.insert();
  objectstore::ScopedExclusiveLock rel(re);
  re.fetch();
  objectstore::Agent agent(m_agentReference.getAgentAddress(), *m_backend);
  agent.initialize();
  objectstore::EntryLogSerDeser cl("user0", "systemhost", time(NULL));
  re.addOrGetAgentRegisterPointerAndCommit(m_agentReference, cl);
  rel.release();
  agent.insertAndRegisterSelf();
  rel.lock(re);
  re.fetch();
  re.addOrGetDriveRegisterPointerAndCommit(m_agentReference, cl);
  re.addOrGetSchedulerGlobalLockAndCommit(m_agentReference, cl);
  rel.release();
  m_OStoreDB.setAgentReference(&m_agentReference);
}

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
  OStoreDBFactory(const std::string & URL = ""): m_URL(URL) {}
  
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
    return std::unique_ptr<SchedulerDatabase>(new OStoreDBWrapper<BackendType>("UnitTest", m_URL));
  }
  
  private:
    std::string m_URL;
}; // class OStoreDBFactory

} // namespace cta

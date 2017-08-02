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

#include "scheduler/SchedulerDatabase.hpp"
#include "objectstore/Agent.hpp"
#include "objectstore/AgentReference.hpp"
#include "objectstore/ArchiveRequest.hpp"
#include "objectstore/ArchiveQueue.hpp"
#include "objectstore/ArchiveRequest.hpp"
#include "objectstore/DriveRegister.hpp"
#include "objectstore/RetrieveRequest.hpp"
#include "objectstore/SchedulerGlobalLock.hpp"
#include "catalogue/Catalogue.hpp"
#include "common/log/Logger.hpp"

namespace cta {
  
namespace objectstore {
  class Backend;
  class Agent;
  class RootEntry;
}

namespace ostoredb {
  template <class, class>
  class MemQueue;
}
  
class OStoreDB: public SchedulerDatabase {
  template <class, class>
  friend class cta::ostoredb::MemQueue;
public:
  OStoreDB(objectstore::Backend & be, catalogue::Catalogue & catalogue, log::Logger &logger);
  virtual ~OStoreDB() throw();
  
  /* === Object store and agent handling ==================================== */
  void setAgentReference(objectstore::AgentReference *agentReference);
  CTA_GENERATE_EXCEPTION_CLASS(AgentNotSet);
private:
  void assertAgentAddressSet();
public:
  
  CTA_GENERATE_EXCEPTION_CLASS(NotImplemented);
  /*============ Basic IO check: validate object store access ===============*/
  void ping() override;

  /* === Session handling =================================================== */
  class TapeMountDecisionInfo: public SchedulerDatabase::TapeMountDecisionInfo {
    friend class OStoreDB;
  public:
    CTA_GENERATE_EXCEPTION_CLASS(SchedulingLockNotHeld);
    CTA_GENERATE_EXCEPTION_CLASS(TapeNotWritable);
    CTA_GENERATE_EXCEPTION_CLASS(TapeIsBusy);
    std::unique_ptr<SchedulerDatabase::ArchiveMount> createArchiveMount(
      const catalogue::TapeForWriting & tape,
      const std::string driveName, const std::string& logicalLibrary, 
      const std::string & hostName, time_t startTime) override;
    std::unique_ptr<SchedulerDatabase::RetrieveMount> createRetrieveMount(
      const std::string & vid, const std::string & tapePool,
      const std::string driveName,
      const std::string& logicalLibrary, const std::string& hostName, 
      time_t startTime) override;
    virtual ~TapeMountDecisionInfo();
  private:
    TapeMountDecisionInfo (objectstore::Backend &, objectstore::AgentReference &, catalogue::Catalogue &, log::Logger &l);
    bool m_lockTaken;
    objectstore::ScopedExclusiveLock m_lockOnSchedulerGlobalLock;
    std::unique_ptr<objectstore::SchedulerGlobalLock> m_schedulerGlobalLock;
    objectstore::Backend & m_objectStore;
    catalogue::Catalogue & m_catalogue;
    log::Logger & m_logger;
    objectstore::AgentReference & m_agentReference;
  };
  
  class TapeMountDecisionInfoNoLock: public SchedulerDatabase::TapeMountDecisionInfo {
  public:
    std::unique_ptr<SchedulerDatabase::ArchiveMount> createArchiveMount(
      const catalogue::TapeForWriting & tape,
      const std::string driveName, const std::string& logicalLibrary, 
      const std::string & hostName, time_t startTime) override;
    std::unique_ptr<SchedulerDatabase::RetrieveMount> createRetrieveMount(
      const std::string & vid, const std::string & tapePool,
      const std::string driveName,
      const std::string& logicalLibrary, const std::string& hostName, 
      time_t startTime) override;
    virtual ~TapeMountDecisionInfoNoLock();
  };

private:
  /**
   * An internal helper function with commonalities of both following functions
   * @param tmdi The TapeMountDecisionInfo where to store the data.
   * @param re A RootEntry object that should be locked and fetched.
   */
  void fetchMountInfo(SchedulerDatabase::TapeMountDecisionInfo &tmdi, objectstore::RootEntry &re);
public:
  std::unique_ptr<SchedulerDatabase::TapeMountDecisionInfo> getMountInfo() override;
  std::unique_ptr<SchedulerDatabase::TapeMountDecisionInfo> getMountInfoNoLock() override;
  void trimEmptyQueues(log::LogContext& lc) override;

  /* === Archive Mount handling ============================================= */
  class ArchiveMount: public SchedulerDatabase::ArchiveMount {
    friend class TapeMountDecisionInfo;
  private:
    ArchiveMount(objectstore::Backend &, objectstore::AgentReference &, catalogue::Catalogue &, log::Logger &);
    objectstore::Backend & m_objectStore;
    catalogue::Catalogue & m_catalogue;
    log::Logger & m_logger;
    objectstore::AgentReference & m_agentReference;
  public:
    CTA_GENERATE_EXCEPTION_CLASS(MaxFSeqNotGoingUp);
    const MountInfo & getMountInfo() override;
    std::list<std::unique_ptr<ArchiveJob> > getNextJobBatch(uint64_t filesRequested, 
      uint64_t bytesRequested, log::LogContext& logContext) override;
    void complete(time_t completionTime) override;
    void setDriveStatus(cta::common::dataStructures::DriveStatus status, time_t completionTime) override;
    void setTapeSessionStats(const castor::tape::tapeserver::daemon::TapeSessionStats &stats) override;
  };
  
  /* === Archive Job Handling =============================================== */
  class ArchiveJob: public SchedulerDatabase::ArchiveJob {
    friend class OStoreDB::ArchiveMount;
  public:
    CTA_GENERATE_EXCEPTION_CLASS(JobNowOwned);
    CTA_GENERATE_EXCEPTION_CLASS(NoSuchJob);
    bool succeed() override;
    void fail(log::LogContext & lc) override;
    void bumpUpTapeFileCount(uint64_t newFileCount) override;
    ~ArchiveJob() override;
  private:
    ArchiveJob(const std::string &, objectstore::Backend &, catalogue::Catalogue &,
      log::Logger &, objectstore::AgentReference &, ArchiveMount &);
    bool m_jobOwned;
    uint64_t m_mountId;
    std::string m_tapePool;
    objectstore::Backend & m_objectStore;
    catalogue::Catalogue & m_catalogue;
    log::Logger & m_logger;
    objectstore::AgentReference & m_agentReference;
    objectstore::ArchiveRequest m_archiveRequest;
    ArchiveMount & m_archiveMount;
  };
  
  /* === Retrieve Mount handling ============================================ */
  class RetrieveMount: public SchedulerDatabase::RetrieveMount {
    friend class TapeMountDecisionInfo;
  private:
    RetrieveMount(objectstore::Backend &, objectstore::AgentReference &, catalogue::Catalogue &);
    objectstore::Backend & m_objectStore;
    objectstore::AgentReference & m_agentReference;
    catalogue::Catalogue & m_catalogue;
  public:
    const MountInfo & getMountInfo() override;
    std::list<std::unique_ptr<RetrieveJob> > getNextJobBatch(uint64_t filesRequested, uint64_t bytesRequested, log::LogContext& logContext) override;
    std::unique_ptr<RetrieveJob> getNextJob(log::LogContext & logContext) override;
    void complete(time_t completionTime) override;
    void setDriveStatus(cta::common::dataStructures::DriveStatus status, time_t completionTime) override;
    void setTapeSessionStats(const castor::tape::tapeserver::daemon::TapeSessionStats &stats) override;
  };
  
  /* === Retrieve Job handling ============================================== */
  class RetrieveJob: public SchedulerDatabase::RetrieveJob {
    friend class OStoreDB::RetrieveMount;
  public:
    CTA_GENERATE_EXCEPTION_CLASS(JobNowOwned);
    CTA_GENERATE_EXCEPTION_CLASS(NoSuchJob);
    virtual void succeed() override;
    virtual void fail(log::LogContext &) override;
    virtual ~RetrieveJob() override;
  private:
    RetrieveJob(const std::string &, objectstore::Backend &, catalogue::Catalogue &, 
      objectstore::AgentReference &, RetrieveMount &);
    bool m_jobOwned;
    uint64_t m_mountId;
    objectstore::Backend & m_objectStore;
    catalogue::Catalogue & m_catalogue;
    objectstore::AgentReference & m_agentReference;
    objectstore::RetrieveRequest m_retrieveRequest;
    OStoreDB::RetrieveMount & m_retrieveMount;
  };
  
  /* === Archive requests handling  ========================================= */
  CTA_GENERATE_EXCEPTION_CLASS(ArchiveRequestHasNoCopies);
  CTA_GENERATE_EXCEPTION_CLASS(ArchiveRequestAlreadyCompleteOrCanceled);
  CTA_GENERATE_EXCEPTION_CLASS(NoSuchArchiveQueue);
  
  void queueArchive(const std::string &instanceName, const cta::common::dataStructures::ArchiveRequest &request, 
    const cta::common::dataStructures::ArchiveFileQueueCriteria &criteria, log::LogContext &logContext) override;

  CTA_GENERATE_EXCEPTION_CLASS(NoSuchArchiveRequest);
  CTA_GENERATE_EXCEPTION_CLASS(ArchiveRequestAlreadyDeleted);
  virtual void deleteArchiveRequest(const std::string &diskInstanceName, uint64_t fileId) override;
  class ArchiveToFileRequestCancelation:
    public SchedulerDatabase::ArchiveToFileRequestCancelation {
  public:
    ArchiveToFileRequestCancelation(objectstore::AgentReference & agentReference, 
      objectstore::Backend & be, catalogue::Catalogue & catalogue, log::Logger &logger): m_request(be), m_lock(), m_objectStore(be),
      m_catalogue(catalogue), m_logger(logger), m_agentReference(agentReference), m_closed(false) {} 
    virtual ~ArchiveToFileRequestCancelation();
    void complete() override;
  private:
    objectstore::ArchiveRequest m_request;
    objectstore::ScopedExclusiveLock m_lock;
    objectstore::Backend & m_objectStore;
    catalogue::Catalogue & m_catalogue;
    log::Logger & m_logger;
    objectstore::AgentReference &m_agentReference;
    bool m_closed;
    friend class OStoreDB;
  };

  std::map<std::string, std::list<common::dataStructures::ArchiveJob> > getArchiveJobs() const override;
  
  std::list<cta::common::dataStructures::ArchiveJob> getArchiveJobs(const std::string& tapePoolName) const override;

  /* === Retrieve requests handling  ======================================== */
  std::list<RetrieveQueueStatistics> getRetrieveQueueStatistics(const cta::common::dataStructures::RetrieveFileQueueCriteria& criteria, const std::set<std::string>& vidsToConsider) override;
  
  CTA_GENERATE_EXCEPTION_CLASS(RetrieveRequestHasNoCopies);
  CTA_GENERATE_EXCEPTION_CLASS(TapeCopyNumberOutOfRange);
  std::string queueRetrieve(const cta::common::dataStructures::RetrieveRequest& rqst,
    const cta::common::dataStructures::RetrieveFileQueueCriteria &criteria, log::LogContext &logContext) override;

  std::list<RetrieveRequestDump> getRetrieveRequestsByVid(const std::string& vid) const override;
  
  std::list<RetrieveRequestDump> getRetrieveRequestsByRequester(const std::string& vid) const override;

  std::map<std::string, std::list<RetrieveRequestDump> > getRetrieveRequests() const override;

  void deleteRetrieveRequest(const common::dataStructures::SecurityIdentity& requester, 
    const std::string& remoteFile) override;

  std::list<cta::common::dataStructures::RetrieveJob> getRetrieveJobs(const std::string& tapePoolName) const override;

  std::map<std::string, std::list<common::dataStructures::RetrieveJob> > getRetrieveJobs() const override;

  
  /* === Drive state handling  ============================================== */
  /**
   * Get states of all drives.
   * @return list of all known drive states
   */
  std::list<cta::common::dataStructures::DriveState> getDriveStates() const override;
  
  void setDesiredDriveState(const std::string& drive, const common::dataStructures::DesiredDriveState & desiredState) override;
  
  void reportDriveStatus(const common::dataStructures::DriveInfo& driveInfo, cta::common::dataStructures::MountType mountType, 
    common::dataStructures::DriveStatus status, time_t reportTime, uint64_t mountSessionId, uint64_t byteTransfered, 
    uint64_t filesTransfered, double latestBandwidth, const std::string& vid, const std::string& tapepool) override;
  
  /* --- Private helper part implementing state transition logic -------------*/
  /*
   * The drive register should gracefully handle reports of status from the drive
   * in all conditions, including when the drive's entry is absent. This ensures
   * that the drive is the leader and register always yields incase of conflicting
   * information.
   */
private:
  /** Collection of smaller scale parts of reportDriveStatus */
  struct ReportDriveStatusInputs {
    common::dataStructures::DriveStatus status;
    cta::common::dataStructures::MountType mountType;
    time_t reportTime; 
    uint64_t mountSessionId;
    uint64_t byteTransferred;
    uint64_t filesTransferred;
    double latestBandwidth;
    std::string vid;
    std::string tapepool;
  };
  /** Collection of smaller scale parts of reportDriveStats */
  struct ReportDriveStatsInputs {
    time_t reportTime; 
    uint64_t bytesTransferred;
    uint64_t filesTransferred;
  };
  /**
   * Utility implementing the operation get drive state or create, update, set on an
   * already locked and fetched DriveRegistry. It in used in reportDriveStatus as well
   * as implicitly in scheduling and other places.
   * Those function are static as they will be used by sub classes as well (like 
   * @param dr
   * @param inputs
   */
  static void updateDriveStatusInRegitry(objectstore::DriveRegister & dr, const common::dataStructures::DriveInfo & driveInfo, 
    const ReportDriveStatusInputs & inputs);
  
  /**
   * Utility implementing the operation get drive state and update stats in it if present, set on an
   * already locked and fetched DriveRegistry. 
   *  
   * @param dr
   * @param driveInfo
   * @param inputs
   */
  static void updateDriveStatsInRegitry(objectstore::DriveRegister & dr, const common::dataStructures::DriveInfo & driveInfo, 
    const ReportDriveStatsInputs & inputs);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveDown(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case. Going UP means drive is ready. It will be instead marked as
   * down if this is the requested state */
  static void setDriveUpOrMaybeDown(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveStarting(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveMounting(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveTransferring(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveUnloading(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveUnmounting(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveDrainingToDisk(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveCleaningUp(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
private:
  objectstore::Backend & m_objectStore;
  catalogue::Catalogue & m_catalogue;
  log::Logger & m_logger;
  objectstore::AgentReference *m_agentReference = nullptr;
};
  
}

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
#include "objectstore/RootEntry.hpp"
#include "objectstore/ArchiveQueue.hpp"
#include "objectstore/RetrieveQueue.hpp"
#include "objectstore/Agent.hpp"
#include "objectstore/AgentReference.hpp"
#include "objectstore/ArchiveRequest.hpp"
#include "objectstore/ArchiveQueue.hpp"
#include "objectstore/ArchiveRequest.hpp"
#include "objectstore/DriveRegister.hpp"
#include "objectstore/RetrieveRequest.hpp"
#include "objectstore/RetrieveActivityCountMap.hpp"
#include "objectstore/RepackQueue.hpp"
#include "objectstore/RepackRequest.hpp"
#include "objectstore/SchedulerGlobalLock.hpp"
#include "catalogue/Catalogue.hpp"
#include "common/log/Logger.hpp"
#include "common/threading/BlockingQueue.hpp"
#include "common/threading/Thread.hpp"
#include "QueueItor.hpp"

namespace cta {

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
  /*============ Thread pool for queueing bottom halfs ======================*/
private:
  typedef std::function<void()> EnqueueingTask;
  cta::threading::BlockingQueue<EnqueueingTask*> m_enqueueingTasksQueue;
  class EnqueueingWorkerThread: private cta::threading::Thread {
  public:
    EnqueueingWorkerThread(cta::threading::BlockingQueue<EnqueueingTask*> & etq): 
      m_enqueueingTasksQueue(etq) {}
    void start() { cta::threading::Thread::start(); }
    void wait() { cta::threading::Thread::wait(); }
  private:
    void run() override;
    cta::threading::BlockingQueue<EnqueueingTask*> & m_enqueueingTasksQueue;
  };
  std::vector<EnqueueingWorkerThread *> m_enqueueingWorkerThreads;
  std::atomic<uint64_t> m_taskQueueSize; ///< This counter ensures destruction happens after the last thread completed.
  /// Delay introduced before posting to the task queue when it becomes too long.
  void delayIfNecessary(log::LogContext &lc);
  cta::threading::Semaphore m_taskPostingSemaphore;
public:
  void waitSubthreadsComplete() override;
  void setThreadNumber(uint64_t threadNumber);
  void setBottomHalfQueueSize(uint64_t tasksNumber);
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
      common::dataStructures::MountType mountType,
      const catalogue::TapeForWriting & tape,
      const std::string driveName, const std::string& logicalLibrary, 
      const std::string & hostName, 
      const std::string& vo, const std::string& mediaType,
      const std::string& vendor,uint64_t capacityInBytes,
      time_t startTime) override;
    std::unique_ptr<SchedulerDatabase::RetrieveMount> createRetrieveMount(
      const std::string & vid, const std::string & tapePool,
      const std::string driveName,
      const std::string& logicalLibrary, const std::string& hostName, 
      const std::string& vo, const std::string& mediaType,
      const std::string& vendor,
      const uint64_t capacityInBytes,
      time_t startTime, const optional<common::dataStructures::DriveState::ActivityAndWeight> &activityAndWeight) override;
    virtual ~TapeMountDecisionInfo();
  private:
    TapeMountDecisionInfo (OStoreDB & oStoreDB);
    bool m_lockTaken;
    objectstore::ScopedExclusiveLock m_lockOnSchedulerGlobalLock;
    std::unique_ptr<objectstore::SchedulerGlobalLock> m_schedulerGlobalLock;
    OStoreDB & m_oStoreDB;
  };
  friend class TapeMountDecisionInfo;
  
  class TapeMountDecisionInfoNoLock: public SchedulerDatabase::TapeMountDecisionInfo {
  public:
    std::unique_ptr<SchedulerDatabase::ArchiveMount> createArchiveMount(
      common::dataStructures::MountType mountType,
      const catalogue::TapeForWriting & tape,
      const std::string driveName, const std::string& logicalLibrary, 
      const std::string & hostName, const std::string& vo, const std::string& mediaType,
      const std::string& vendor,uint64_t capacityInBytes,
      time_t startTime) override;
    std::unique_ptr<SchedulerDatabase::RetrieveMount> createRetrieveMount(
      const std::string & vid, const std::string & tapePool,
      const std::string driveName,
      const std::string& logicalLibrary, const std::string& hostName, 
      const std::string& vo, const std::string& mediaType,
      const std::string& vendor,
      const uint64_t capacityInBytes,
      time_t startTime, const optional<common::dataStructures::DriveState::ActivityAndWeight> &activityAndWeight) override;
    virtual ~TapeMountDecisionInfoNoLock();
  };

private:
  /**
   * An internal helper function with commonalities of both following functions
   * @param tmdi The TapeMountDecisionInfo where to store the data.
   * @param re A RootEntry object that should be locked and fetched.
   */
  void fetchMountInfo(SchedulerDatabase::TapeMountDecisionInfo &tmdi, objectstore::RootEntry &re, SchedulerDatabase::PurposeGetMountInfo purpose,
    log::LogContext & logContext);
  
  /**
   * An internal helper function to build a list of mount policies with the map of the
   * mount policies coming from the queue JobsSummary object
   * The map contains the name of the mount policies, so it is just a conversion from the name to an entire mount policy object
   * @param mountPoliciesInCatalogue the list of the mountPolicies given by the Catalogue
   * @param queueMountPolicyMap the map of the <mountPolicyName,counter> coming from the queue JobsSummary object
   * @return the list of MountPolicies that are in the map
   */
  std::list<common::dataStructures::MountPolicy> getMountPoliciesInQueue(const std::list<common::dataStructures::MountPolicy> & mountPoliciesInCatalogue, const std::map<std::string,uint64_t> & queueMountPolicyMap);
  
  /**
   * Return the best archive mount policy from the mountPolicies passed in parameter
   * The aim is to do the same as ArchiveQueue::getJobsSummary() regarding the priority, maxDrivesAllowed, minArchiveRequestAge attributes of the queue
   * @param mountPolicies the list of mount policies in order to create the best one.
   * @return the best archive mount policy to use
   */
  common::dataStructures::MountPolicy createBestArchiveMountPolicy(const std::list<common::dataStructures::MountPolicy> & mountPolicies);
  
  /**
   * Return the best retrieve mount policy from the mountPolicies passed in parameter
   * The aim is to do the same as RetrieveQueue::getJobsSummary() regarding the priority, maxDrivesAllowed, minArchiveRequestAge attributes of the queue 
   * @param mountPolicies the list of mount policies in order to create the best one.
   * @return the best retrieve mount policy to use
   */
  common::dataStructures::MountPolicy createBestRetrieveMountPolicy(const std::list<common::dataStructures::MountPolicy> & mountPolicies);
  
public:
  std::unique_ptr<SchedulerDatabase::TapeMountDecisionInfo> getMountInfo(log::LogContext& logContext) override;
  
  std::unique_ptr<SchedulerDatabase::TapeMountDecisionInfo> getMountInfoNoLock(PurposeGetMountInfo purpose, log::LogContext& logContext) override;
  void trimEmptyQueues(log::LogContext& lc) override;

  /* === Archive Mount handling ============================================= */
  class ArchiveJob;
  class ArchiveMount: public SchedulerDatabase::ArchiveMount {
    friend class TapeMountDecisionInfo;
  private:
    ArchiveMount(OStoreDB & oStoreDB, objectstore::JobQueueType queueType);
    OStoreDB & m_oStoreDB;
    objectstore::JobQueueType m_queueType;
  public:
    CTA_GENERATE_EXCEPTION_CLASS(MaxFSeqNotGoingUp);
    const MountInfo & getMountInfo() override;
    std::list<std::unique_ptr<SchedulerDatabase::ArchiveJob>> getNextJobBatch(uint64_t filesRequested, 
      uint64_t bytesRequested, log::LogContext &logContext) override;
    void complete(time_t completionTime) override;
    void setDriveStatus(cta::common::dataStructures::DriveStatus status, time_t completionTime, const cta::optional<std::string> & reason = cta::nullopt) override;
    void setTapeSessionStats(const castor::tape::tapeserver::daemon::TapeSessionStats &stats) override;
  public:
    void setJobBatchTransferred(
      std::list<std::unique_ptr<SchedulerDatabase::ArchiveJob>> &jobsBatch, log::LogContext &lc) override;
  };
  friend class ArchiveMount;
  
  /* === Archive Job Handling =============================================== */
  class ArchiveJob: public SchedulerDatabase::ArchiveJob {
    friend class OStoreDB::ArchiveMount;
    friend class OStoreDB;
  public:
    CTA_GENERATE_EXCEPTION_CLASS(JobNotOwned);
    CTA_GENERATE_EXCEPTION_CLASS(NoSuchJob);
    void failTransfer(const std::string& failureReason, log::LogContext& lc) override;
    void failReport(const std::string& failureReason, log::LogContext& lc) override;
  private:
    void asyncSucceedTransfer();
    /** Returns true if the jobs was the last one and the request should be queued for report. */
    void waitAsyncSucceed();
    objectstore::ArchiveRequest::RepackInfo getRepackInfoAfterAsyncSuccess();
    bool isLastAfterAsyncSuccess();
    void asyncDeleteRequest();
    void waitAsyncDelete();
  public:
    void bumpUpTapeFileCount(uint64_t newFileCount) override;
    ~ArchiveJob() override;
  private:
    ArchiveJob(const std::string &, OStoreDB &);
    bool m_jobOwned;
    uint64_t m_mountId;
    std::string m_tapePool;
    OStoreDB & m_oStoreDB;
    objectstore::ArchiveRequest m_archiveRequest;
    std::unique_ptr<objectstore::ArchiveRequest::AsyncTransferSuccessfulUpdater> m_succesfulTransferUpdater;
    std::unique_ptr<objectstore::ArchiveRequest::AsyncRequestDeleter> m_requestDeleter;
  };
  //friend class ArchiveJob;
  static ArchiveJob * castFromSchedDBJob(SchedulerDatabase::ArchiveJob * job);
  
  /* === Retrieve Mount handling ============================================ */
  class RetrieveJob;
  class RetrieveMount: public SchedulerDatabase::RetrieveMount {
    friend class TapeMountDecisionInfo;
  private:
    RetrieveMount(OStoreDB &oStoreDB);
    OStoreDB & m_oStoreDB;
  public:
    const MountInfo & getMountInfo() override;
    std::list<std::unique_ptr<cta::SchedulerDatabase::RetrieveJob> > getNextJobBatch(uint64_t filesRequested, uint64_t bytesRequested, 
      cta::disk::DiskSystemFreeSpaceList & diskSystemFreeSpace, log::LogContext& logContext) override;
  private:
    void requeueJobBatch(std::list<std::unique_ptr<OStoreDB::RetrieveJob> >& jobBatch,
      log::LogContext& logContext);
    std::map<std::string, uint64_t> getExistingDrivesReservations(); 
    void reserveDiskSpace(const DiskSpaceReservationRequest& diskSpaceReservation, log::LogContext & lc);
    struct DiskSystemToSkip {
      std::string name;
      uint64_t sleepTime;
      bool operator<(const DiskSystemToSkip & o) const { return name < o.name; }
    };
    std::set<DiskSystemToSkip> m_diskSystemsToSkip;
  public:
    /// Public but non overriding function used by retrieve jobs (on failure to transfer):
    void releaseDiskSpace(const DiskSpaceReservationRequest& diskSpaceReservation, log::LogContext & lc);
    void complete(time_t completionTime) override;
    void setDriveStatus(cta::common::dataStructures::DriveStatus status, time_t completionTime, const cta::optional<std::string> & reason = cta::nullopt) override;
    void setTapeSessionStats(const castor::tape::tapeserver::daemon::TapeSessionStats &stats) override;
  public:
    void flushAsyncSuccessReports(std::list<cta::SchedulerDatabase::RetrieveJob*>& jobsBatch, log::LogContext& lc) override;
  };
  friend class RetrieveMount;
  
  /* === Retrieve Job handling ============================================== */
  class RetrieveJob: public SchedulerDatabase::RetrieveJob {
    friend class OStoreDB::RetrieveMount;
    friend class OStoreDB;
  public:
    CTA_GENERATE_EXCEPTION_CLASS(JobNotOwned);
    CTA_GENERATE_EXCEPTION_CLASS(NoSuchJob);
    /** Async set job successful.  Either delete (user transfer) or change status (repack)
     * Wait will happen in RetrieveMount::flushAsyncSuccessReports().
     */
    virtual void asyncSetSuccessful() override;
    void failTransfer(const std::string& failureReason, log::LogContext& lc) override;
    void failReport(const std::string& failureReason, log::LogContext& lc) override;
    void abort(const std::string& abortReason, log::LogContext &lc) override;
    void fail() override;
    virtual ~RetrieveJob() override;
  private:
    // Can be instantiated from a mount (queue to transfer) or a report queue
    RetrieveJob(const std::string &jobAddress, OStoreDB &oStoreDB, RetrieveMount *rm) :
      m_jobOwned(false), m_oStoreDB(oStoreDB),
      m_retrieveRequest(jobAddress, m_oStoreDB.m_objectStore),
      m_retrieveMount(rm) { }
    void setJobOwned(bool b = true) { m_jobOwned = b; }

  private:
    bool m_jobOwned;
    uint64_t m_mountId;
    OStoreDB & m_oStoreDB;
    objectstore::RetrieveRequest m_retrieveRequest;
    OStoreDB::RetrieveMount *m_retrieveMount;
    std::unique_ptr<objectstore::RetrieveRequest::AsyncJobDeleter> m_jobDelete;
    std::unique_ptr<objectstore::RetrieveRequest::AsyncJobSucceedForRepackReporter> m_jobSucceedForRepackReporter;
    objectstore::RetrieveRequest::RepackInfo m_repackInfo;
    optional<objectstore::RetrieveActivityDescription> m_activityDescription;
    optional<std::string> m_diskSystemName;
  };
  static RetrieveJob * castFromSchedDBJob(SchedulerDatabase::RetrieveJob * job);

  /* === Archive requests handling  ========================================= */
  CTA_GENERATE_EXCEPTION_CLASS(ArchiveRequestHasNoCopies);
  CTA_GENERATE_EXCEPTION_CLASS(ArchiveRequestAlreadyCompleteOrCanceled);
  CTA_GENERATE_EXCEPTION_CLASS(NoSuchArchiveQueue);
  
  std::string queueArchive(const std::string &instanceName, const cta::common::dataStructures::ArchiveRequest &request, 
    const cta::common::dataStructures::ArchiveFileQueueCriteriaAndFileId &criteria, log::LogContext &logContext) override;

  std::map<std::string, std::list<common::dataStructures::ArchiveJob>> getArchiveJobs() const override;
  
  std::list<cta::common::dataStructures::ArchiveJob> getArchiveJobs(const std::string& tapePoolName) const override;

  typedef QueueItor<objectstore::RootEntry::ArchiveQueueDump, objectstore::ArchiveQueue> ArchiveQueueItor_t;

  ArchiveQueueItor_t getArchiveJobItor(const std::string &tapePoolName,
    objectstore::JobQueueType queueType = objectstore::JobQueueType::JobsToTransferForUser) const;

  ArchiveQueueItor_t* getArchiveJobItorPtr(const std::string &tapePoolName,
    objectstore::JobQueueType queueType = objectstore::JobQueueType::JobsToTransferForUser) const;

  std::list<std::unique_ptr<SchedulerDatabase::ArchiveJob> > getNextArchiveJobsToReportBatch(uint64_t filesRequested, 
     log::LogContext & logContext) override;

  JobsFailedSummary getArchiveJobsFailedSummary(log::LogContext &logContext) override;
  
  void setArchiveJobBatchReported(std::list<cta::SchedulerDatabase::ArchiveJob*> & jobsBatch,
     log::TimingList & timingList, utils::Timer & t, log::LogContext & lc) override;
  
  /* === Retrieve requests handling  ======================================== */
  std::list<RetrieveQueueStatistics> getRetrieveQueueStatistics(const cta::common::dataStructures::RetrieveFileQueueCriteria& criteria, const std::set<std::string>& vidsToConsider) override;
  
  CTA_GENERATE_EXCEPTION_CLASS(RetrieveRequestHasNoCopies);
  CTA_GENERATE_EXCEPTION_CLASS(TapeCopyNumberOutOfRange);
  SchedulerDatabase::RetrieveRequestInfo queueRetrieve(cta::common::dataStructures::RetrieveRequest& rqst,
    const cta::common::dataStructures::RetrieveFileQueueCriteria &criteria, const optional<std::string> diskSystemName, 
    log::LogContext &logContext) override;
  void cancelRetrieve(const std::string& instanceName, const cta::common::dataStructures::CancelRetrieveRequest& rqst,
    log::LogContext& lc) override;

  std::list<RetrieveRequestDump> getRetrieveRequestsByVid(const std::string& vid) const override;
  
  std::list<RetrieveRequestDump> getRetrieveRequestsByRequester(const std::string& vid) const override;

  std::map<std::string, std::list<RetrieveRequestDump> > getRetrieveRequests() const override;

  void deleteRetrieveRequest(const common::dataStructures::SecurityIdentity& requester, 
    const std::string& remoteFile) override;
  
  /**
   * Idempotently deletes the specified ArchiveRequest from the objectstore
   * @param address, the address of the ArchiveRequest
   * @param archiveFileID the archiveFileID of the file to delete.
   */
  virtual void cancelArchive(const common::dataStructures::DeleteArchiveRequest& request, log::LogContext & lc) override;

  virtual void deleteFailed(const std::string &objectId, log::LogContext &lc) override;

  std::list<cta::common::dataStructures::RetrieveJob> getRetrieveJobs(const std::string &vid) const override;

  std::map<std::string, std::list<common::dataStructures::RetrieveJob>> getRetrieveJobs() const override;

  typedef QueueItor<objectstore::RootEntry::RetrieveQueueDump, objectstore::RetrieveQueue> RetrieveQueueItor_t;

  RetrieveQueueItor_t getRetrieveJobItor(const std::string &vid,
    objectstore::JobQueueType queueType = objectstore::JobQueueType::JobsToTransferForUser) const;

  RetrieveQueueItor_t* getRetrieveJobItorPtr(const std::string &vid,
    objectstore::JobQueueType queueType = objectstore::JobQueueType::JobsToTransferForUser) const;

  std::list<std::unique_ptr<SchedulerDatabase::RetrieveJob>> getNextRetrieveJobsToReportBatch(uint64_t filesRequested, log::LogContext &logContext) override;

  void setRetrieveJobBatchReportedToUser(std::list<cta::SchedulerDatabase::RetrieveJob*> & jobsBatch,
     log::TimingList & timingList, utils::Timer & t, log::LogContext & lc) override;

  std::list<std::unique_ptr<SchedulerDatabase::RetrieveJob>> getNextRetrieveJobsFailedBatch(uint64_t filesRequested, log::LogContext &logContext) override;

  JobsFailedSummary getRetrieveJobsFailedSummary(log::LogContext &logContext) override;
  
  /* === Repack requests handling =========================================== */
  std::string queueRepack(const SchedulerDatabase::QueueRepackRequest & repackRequest, log::LogContext &logContext) override;
  
  std::list<common::dataStructures::RepackInfo> getRepackInfo() override;
  common::dataStructures::RepackInfo getRepackInfo(const std::string& vid) override;
  void cancelRepack(const std::string& vid, log::LogContext & lc) override;
  
  class RepackRequest: public SchedulerDatabase::RepackRequest
  {
    friend class OStoreDB;
  public:
    RepackRequest(const std::string &jobAddress, OStoreDB &oStoreDB) :
    m_oStoreDB(oStoreDB), m_repackRequest(jobAddress, m_oStoreDB.m_objectStore){}
    uint64_t addSubrequestsAndUpdateStats(std::list<Subrequest>& repackSubrequests, cta::common::dataStructures::ArchiveRoute::FullMap& archiveRoutesMap,
      uint64_t maxFSeqLowBound, const uint64_t maxAddedFSeq, const TotalStatsFiles &totalStatsFiles, disk::DiskSystemList diskSystemList, 
      log::LogContext& lc) override;
    void expandDone() override;
    void fail() override;
    void requeueInToExpandQueue(log::LogContext& lc) override;
    void setExpandStartedAndChangeStatus() override;
    void fillLastExpandedFSeqAndTotalStatsFile(uint64_t& fSeq, TotalStatsFiles& totalStatsFiles) override;
    uint64_t getLastExpandedFSeq() override;
    void setLastExpandedFSeq(uint64_t fseq) override;

  private:
    OStoreDB & m_oStoreDB;
    objectstore::RepackRequest m_repackRequest;
  };
  friend class RepackRequest;
  
  /**
   * A class holding a lock on the pending repack request queue. This is the first
   * container we will have to lock if we decide to pop a/some request(s)
   */
  class RepackRequestPromotionStatistics: public SchedulerDatabase::RepackRequestStatistics {
    friend class OStoreDB;
  public:
    PromotionToToExpandResult promotePendingRequestsForExpansion(size_t requestCount, log::LogContext &lc) override;
    virtual ~RepackRequestPromotionStatistics() {};
  private:
    RepackRequestPromotionStatistics(objectstore::Backend & backend,
              objectstore::AgentReference & agentReference);
    objectstore::Backend & m_backend;
    objectstore::AgentReference &m_agentReference;
    objectstore::RepackQueuePending m_pendingRepackRequestQueue;
    objectstore::ScopedExclusiveLock m_lockOnPendingRepackRequestsQueue;
  };
  
  class RepackRequestPromotionStatisticsNoLock: public SchedulerDatabase::RepackRequestStatistics {\
    friend class OStoreDB;
  public:
    PromotionToToExpandResult promotePendingRequestsForExpansion(size_t requestCount, log::LogContext &lc) override {
      throw (SchedulingLockNotHeld("In RepackRequestPromotionStatisticsNoLock::promotePendingRequestsForExpansion"));
    }
    virtual ~RepackRequestPromotionStatisticsNoLock() {}
  };
  
private:
  void populateRepackRequestsStatistics (objectstore::RootEntry & re, SchedulerDatabase::RepackRequestStatistics &stats);
public:
  
  std::unique_ptr<RepackRequestStatistics> getRepackStatistics() override;
  std::unique_ptr<RepackRequestStatistics> getRepackStatisticsNoLock() override;
  
  /**
   * Returns the Object Store representation of a RepackRequest that is in
   * the RepackQueueToExpand queue (the repack request has Status "ToExpand"
   * @return a unique_ptr holding the RepackRequest
   */
  std::unique_ptr<SchedulerDatabase::RepackRequest> getNextRepackJobToExpand() override;
 
  class RepackRetrieveSuccessesReportBatch;
  class RepackRetrieveFailureReportBatch;
  class RepackArchiveReportBatch;
  class RepackArchiveSuccessesReportBatch;
  class RepackArchiveFailureReportBatch;
  friend class RepackRetrieveSuccessesReportBatch;
  friend class RepackRetrieveFailureReportBatch;
  friend class RepackArchiveSuccessesReportBatch;
  friend class RepackArchiveFailureReportBatch;
  /**
   * Base class handling the commonalities
   */
  class RepackReportBatch: public SchedulerDatabase::RepackReportBatch {
    friend class OStoreDB;
    friend class RepackRetrieveSuccessesReportBatch;
    friend class RepackRetrieveFailureReportBatch;
    friend class RepackArchiveSuccessesReportBatch;
    friend class RepackArchiveFailureReportBatch;
    RepackReportBatch(objectstore::Backend & backend, OStoreDB & oStoreDb): m_repackRequest(backend), m_oStoreDb(oStoreDb) {}
  protected:
    objectstore::RepackRequest m_repackRequest;
    OStoreDB & m_oStoreDb;
    template <class SR> 
    struct SubrequestInfo {
      /// CopyNb is only useful for archive requests where we want to distinguish several jobs.
      uint32_t archivedCopyNb = 0;
      /** Status map is only useful for archive requests, where we need to know other job's status to decide
       * whether we should delete the request (all done). It's more efficient to get the information on pop
       * in order to save a read in the most common case (only one job), and trigger immediate deletion of
       * the request after succeeding/failing. */
      std::map<uint32_t, objectstore::serializers::ArchiveJobStatus> archiveJobsStatusMap;
      cta::objectstore::AgentReference * owner;
      std::shared_ptr<SR> subrequest;
      common::dataStructures::ArchiveFile archiveFile;
      typename SR::RepackInfo repackInfo;
      typedef std::list<SubrequestInfo> List;
    };
  };
  
  class RepackRetrieveSuccessesReportBatch: public RepackReportBatch {
    friend class OStoreDB;
    RepackRetrieveSuccessesReportBatch(objectstore::Backend & backend, OStoreDB & oStoreDb):
      RepackReportBatch(backend,oStoreDb) {}
  public:
    void report(log::LogContext& lc) override;
  private:
    typedef RepackReportBatch::SubrequestInfo<objectstore::RetrieveRequest> SubrequestInfo;
    SubrequestInfo::List m_subrequestList;
  };
  
  class RepackRetrieveFailureReportBatch: public RepackReportBatch{
    friend class OStoreDB;
    RepackRetrieveFailureReportBatch(objectstore::Backend& backend, OStoreDB &oStoreDb):RepackReportBatch(backend,oStoreDb){}
  public:
    void report(log::LogContext& lc) override;
  private:
    typedef RepackReportBatch::SubrequestInfo<objectstore::RetrieveRequest> SubrequestInfo;
    SubrequestInfo::List m_subrequestList;
  };
  
  /**
   * Super class that holds the common code for the reporting
   * of Archive successes and failures
   */
  class RepackArchiveReportBatch: public RepackReportBatch{
    friend class OStoreDB;
  protected:
    typedef RepackReportBatch::SubrequestInfo<objectstore::ArchiveRequest> SubrequestInfo;
    SubrequestInfo::List m_subrequestList;
  public:
    RepackArchiveReportBatch(objectstore::Backend & backend, OStoreDB & oStoreDb):RepackReportBatch(backend,oStoreDb){}
    void report(log::LogContext &lc);
  private:
    objectstore::RepackRequest::SubrequestStatistics::List prepareReport();
    virtual cta::objectstore::serializers::RepackRequestStatus recordReport(objectstore::RepackRequest::SubrequestStatistics::List& ssl, log::TimingList& timingList, utils::Timer& t) = 0;
    virtual cta::objectstore::serializers::ArchiveJobStatus getNewStatus() = 0;
  };
  
  class RepackArchiveSuccessesReportBatch: public RepackArchiveReportBatch {
    friend class OStoreDB;
    RepackArchiveSuccessesReportBatch(objectstore::Backend & backend, OStoreDB & oStoreDb):
      RepackArchiveReportBatch(backend,oStoreDb) {}
  public:
    void report(log::LogContext& lc) override;
  private:
    cta::objectstore::serializers::RepackRequestStatus recordReport(objectstore::RepackRequest::SubrequestStatistics::List& ssl, log::TimingList& timingList, utils::Timer& t) override;
    cta::objectstore::serializers::ArchiveJobStatus getNewStatus() override;
  };
  
  class RepackArchiveFailureReportBatch: public RepackArchiveReportBatch {
    friend class OStoreDB;
    
    RepackArchiveFailureReportBatch(objectstore::Backend & backend, OStoreDB & oStoreDb):
      RepackArchiveReportBatch(backend,oStoreDb) {}
  public:
    void report(log::LogContext& lc) override;
  private:
    cta::objectstore::serializers::RepackRequestStatus recordReport(objectstore::RepackRequest::SubrequestStatistics::List& ssl, log::TimingList& timingList, utils::Timer& t) override;
    cta::objectstore::serializers::ArchiveJobStatus getNewStatus() override;
  };
  
  std::unique_ptr<SchedulerDatabase::RepackReportBatch> getNextRepackReportBatch(log::LogContext& lc) override;
  
  std::list<std::unique_ptr<SchedulerDatabase::RepackReportBatch>> getRepackReportBatches(log::LogContext &lc) override;
  
  std::unique_ptr<SchedulerDatabase::RepackReportBatch> getNextSuccessfulRetrieveRepackReportBatch(log::LogContext& lc);
  std::unique_ptr<SchedulerDatabase::RepackReportBatch> getNextFailedRetrieveRepackReportBatch(log::LogContext& lc);
  std::unique_ptr<SchedulerDatabase::RepackReportBatch> getNextSuccessfulArchiveRepackReportBatch(log::LogContext& lc);
  std::unique_ptr<SchedulerDatabase::RepackReportBatch> getNextFailedArchiveRepackReportBatch(log::LogContext &lc);
  CTA_GENERATE_EXCEPTION_CLASS(NoRepackReportBatchFound);
private:
  const size_t c_repackArchiveReportBatchSize = 10000;
  const size_t c_repackRetrieveReportBatchSize = 10000;
public:

  /* === Drive state handling  ============================================== */
  /**
   * Get states of all drives.
   * @return list of all known drive states
   */
  std::list<cta::common::dataStructures::DriveState> getDriveStates(log::LogContext & lc) const override;
  
  void setDesiredDriveState(const std::string& drive, const common::dataStructures::DesiredDriveState & desiredState, 
    log::LogContext & lc) override;
    
  void removeDrive(const std::string & drive, log::LogContext & logContext) override;
  
  void reportDriveStatus(const common::dataStructures::DriveInfo& driveInfo, cta::common::dataStructures::MountType mountType, 
    common::dataStructures::DriveStatus status, time_t reportTime, log::LogContext & lc, uint64_t mountSessionId, uint64_t byteTransfered, 
    uint64_t filesTransfered, double latestBandwidth, const std::string& vid, const std::string& tapepool) override;
  
  void reportDriveConfig(const cta::tape::daemon::TpconfigLine& tpConfigLine, const cta::tape::daemon::TapedConfiguration& tapedConfig,log::LogContext& lc) override;
  
  void checkDriveCanBeCreated(const cta::common::dataStructures::DriveInfo & driveInfo) override;
  
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
    optional<common::dataStructures::DriveState::ActivityAndWeight> activityAndWeigh;
    optional<std::string> reason;
  };
  /** Collection of smaller scale parts of reportDriveStats */
  struct ReportDriveStatsInputs {
    time_t reportTime; 
    uint64_t bytesTransferred;
    uint64_t filesTransferred;
  };
  /**
   * Utility implementing the operation of finding/creating the drive state object and 
   * updating it.
   */
  void updateDriveStatus(const common::dataStructures::DriveInfo & driveInfo, 
    const ReportDriveStatusInputs & inputs, log::LogContext & lc);
  
  /**
   * Utility implementing the operation get drive state and update stats in it if present, set on an
   * already locked and fetched DriveRegistry. 
   *  
   * @param dr
   * @param driveInfo
   * @param inputs
   */
  void updateDriveStatistics(const common::dataStructures::DriveInfo & driveInfo, 
    const ReportDriveStatsInputs & inputs, log::LogContext & lc);
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveDown(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
  /** Helper for setting a drive state in a specific case. Going UP means drive is ready. It will be instead marked as
   * down if this is the requested state */
  static void setDriveUpOrMaybeDown(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
 
  /** Helper for setting a drive state in a specific case */
  static void setDriveProbing(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs);
  
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
  
  /** Helper for setting a drive state in a specific case */
  static void setDriveShutdown(common::dataStructures::DriveState & driveState, const ReportDriveStatusInputs & inputs); 
private:
  objectstore::Backend & m_objectStore;
  catalogue::Catalogue & m_catalogue;
  log::Logger & m_logger;
  objectstore::AgentReference *m_agentReference = nullptr;
};
  
} // namespace cta

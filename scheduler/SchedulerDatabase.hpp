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

#include "common/dataStructures/RetrieveFileQueueCriteria.hpp"
#include "common/dataStructures/DriveState.hpp"
#include "common/MountControl.hpp"
#include "common/dataStructures/ArchiveJob.hpp"
#include "common/dataStructures/ArchiveFile.hpp"
#include "common/dataStructures/ArchiveRequest.hpp"
#include "common/dataStructures/ArchiveFileQueueCriteriaAndFileId.hpp"
#include "common/dataStructures/ArchiveRoute.hpp"
#include "common/dataStructures/DriveInfo.hpp"
#include "common/dataStructures/MountType.hpp"
#include "common/dataStructures/MountPolicy.hpp"
#include "common/dataStructures/RetrieveJob.hpp"
#include "common/dataStructures/RetrieveRequest.hpp"
#include "common/dataStructures/RepackInfo.hpp"
#include "common/dataStructures/SecurityIdentity.hpp"
#include "common/remoteFS/RemotePathAndStatus.hpp"
#include "common/exception/Exception.hpp"
#include "common/log/LogContext.hpp"
#include "catalogue/TapeForWriting.hpp"
#include "scheduler/TapeMount.hpp"
#include "tapeserver/daemon/TapedConfiguration.hpp"

#include <list>
#include <limits>
#include <map>
#include <stdint.h>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <set>

namespace cta {
// Forward declarations for opaque references.
namespace common {
namespace admin {
  class AdminUser;
} // cta::common::admin
namespace archiveRoute {
  class ArchiveRoute;
} // cta::common::archiveRoute
} // cta::common
namespace log {
  class TimingList;
} // cta::log
namespace utils {
  class Timer;
} // cta::utils
class ArchiveRequest;
class LogicalLibrary;
class RetrieveRequestDump;
class SchedulerDatabase;
class StorageClass;
class Tape;
class TapeMount;
class TapeSession;
class RepackRequest;
namespace objectstore{
  class RetrieveRequest;
  class ArchiveRequest;
}
} // cta

namespace cta {

/**
 * Abstract class defining the interface to the database of a tape resource
 * scheduler.
 */
class SchedulerDatabase {
public:

  /**
   * Destructor.
   */
  virtual ~SchedulerDatabase() throw() = 0;
  
  
  /*============ Sub thread handling, mostly for unit tests =================*/
  virtual void waitSubthreadsComplete() = 0;
  
  /*============ Basic IO check: validate object store access ===============*/
  /**
   * Validates that the scheduler database is accessible. A trivial operation
   * will be executed to check. The exception is let through in case of problem.
   */
  virtual void ping() = 0;
  
  /*============ Archive management: user side ==============================*/
  /**
   * Queues the specified request.
   *
   * @param rqst The request.
   * @param criteria The criteria retrieved from the CTA catalogue to be used to
   * decide how to queue the request.
   * @param logContext context allowing logging db operation
   */
  virtual void queueArchive(const std::string &instanceName, const cta::common::dataStructures::ArchiveRequest &request, 
    const cta::common::dataStructures::ArchiveFileQueueCriteriaAndFileId &criteria,
    log::LogContext &logContext) = 0;
  
  virtual void queueArchiveForRepack(std::unique_ptr<cta::objectstore::ArchiveRequest> request,log::LogContext &logContext) = 0;

  /**
   * Returns all of the queued archive jobs.  The returned jobs are
   * grouped by tape pool and then sorted by creation time, oldest first.
   *
   * @return The queued jobs.
   */
  virtual std::map<std::string, std::list<common::dataStructures::ArchiveJob> >
    getArchiveJobs() const = 0;

  /**
   * Returns the list of queued jobs queued on the specified tape pool.
   * The returned requests are sorted by creation time, oldest first.
   *
   * @param tapePoolName The name of the tape pool.
   * @return The queued requests.
   */
  virtual std::list<cta::common::dataStructures::ArchiveJob> getArchiveJobs(
    const std::string &tapePoolName) const = 0;

  /*============ Archive management: tape server side =======================*/
  /**
   * The class used by the scheduler database to track the archive mounts
   */
  class ArchiveJob;
  class ArchiveMount {
  public:
    struct MountInfo {
      std::string vid;
      std::string logicalLibrary;
      std::string tapePool;
      std::string vo;
      std::string mediaType;
      std::string vendor;
      std::string drive;
      std::string host;
      uint64_t mountId;
      uint64_t capacityInBytes;
      cta::common::dataStructures::MountType mountType;
    } mountInfo;
    virtual const MountInfo & getMountInfo() = 0;
    virtual std::list<std::unique_ptr<ArchiveJob>> getNextJobBatch(uint64_t filesRequested,
      uint64_t bytesRequested, log::LogContext& logContext) = 0;
    virtual void complete(time_t completionTime) = 0;
    virtual void setDriveStatus(common::dataStructures::DriveStatus status, time_t completionTime) = 0;
    virtual void setTapeSessionStats(const castor::tape::tapeserver::daemon::TapeSessionStats &stats) = 0;
    virtual void setJobBatchTransferred(
      std::list<std::unique_ptr<cta::SchedulerDatabase::ArchiveJob>> & jobsBatch, log::LogContext & lc) = 0;
    virtual ~ArchiveMount() {}
    uint32_t nbFilesCurrentlyOnTape;
  };
  
  /**
   * The class to handle the DB-side of a tape job.
   */
  class ArchiveJob {
    friend class ArchiveMount;
  public:
    std::string srcURL;
    std::string archiveReportURL;
    std::string errorReportURL;
    std::string latestError;
    enum class ReportType: uint8_t {
      NoReportRequired,
      CompletionReport,
      FailureReport,
      Report ///< A generic grouped type
    } reportType;
    cta::common::dataStructures::ArchiveFile archiveFile;
    cta::common::dataStructures::TapeFile tapeFile;
    virtual void failTransfer(const std::string & failureReason, log::LogContext & lc) = 0;
    virtual void failReport(const std::string & failureReason, log::LogContext & lc) = 0;
    virtual void bumpUpTapeFileCount(uint64_t newFileCount) = 0;
    virtual ~ArchiveJob() {}
  };
  
  /**
   * Get a a set of jobs to report to the clients. This function is like 
   * ArchiveMount::getNextJobBatch. It it not in the context of a mount as any 
   * process can grab a batch of jobs to report and proceed with the reporting.
   * After reporting, setJobReported will be the last step of the job's lifecycle.
   * @return A list of process-owned jobs to report.
   */
  virtual std::list<std::unique_ptr<ArchiveJob>> getNextArchiveJobsToReportBatch(uint64_t filesRequested,
    log::LogContext & logContext) = 0;

  /*======================= Failed archive jobs support ======================*/
  struct JobsFailedSummary {
     JobsFailedSummary(uint64_t f = 0, uint64_t b = 0) : totalFiles(f), totalBytes(b) {}
     uint64_t totalFiles;
     uint64_t totalBytes;
  };

  virtual JobsFailedSummary getArchiveJobsFailedSummary(log::LogContext &logContext) = 0;

  /**
   * Set a batch of jobs as reported (modeled on ArchiveMount::setJobBatchSuccessful().
   * @param jobsBatch
   * @param lc
   */
  virtual void setArchiveJobBatchReported(std::list<cta::SchedulerDatabase::ArchiveJob *> & jobsBatch,
    log::TimingList & timingList, utils::Timer & t, log::LogContext & lc) = 0;
  
  /*============ Retrieve  management: user side ============================*/

  /**
   * A representation of an existing retrieve queue. This is a (simpler) relative 
   * to the PotentialMount used for mount scheduling. This summary will be used to 
   * decide which retrieve job to use for multiple copy files. 
   * In order to have a stable comparison, we compare on byte number and not file counts.
   */
  struct RetrieveQueueStatistics {
    std::string vid;
    uint64_t bytesQueued;
    uint64_t filesQueued;
    uint64_t currentPriority;
    
    bool operator <(const RetrieveQueueStatistics& right) const {
      return right > * this; // Reuse greater than operator
    }
    
    bool operator >(const RetrieveQueueStatistics& right) const {
      return bytesQueued > right.bytesQueued || currentPriority > right.currentPriority;
    }
    
    static bool leftGreaterThanRight (const RetrieveQueueStatistics& left, const RetrieveQueueStatistics& right) {
      return left > right;
    }

  };
  
  /**
   * Get the retrieve queue statistics for the vids of the tape files from the criteria, that are also
   * listed in the set. 
   * @param criteria the retrieve criteria, containing the list of tape files.
   * @param vidsToConsider list of vids to considers. Other vids should not be considered.
   * @return the list of statistics.
   */
  virtual std::list<RetrieveQueueStatistics> getRetrieveQueueStatistics(
    const cta::common::dataStructures::RetrieveFileQueueCriteria &criteria,
    const std::set<std::string> & vidsToConsider) = 0;
  /**
   * Queues the specified request. As the object store has access to the catalogue,
   * the best queue (most likely to go, and not disabled can be chosen directly there).
   *
   * @param rqst The request.
   * @param criteria The criteria retrieved from the CTA catalogue to be used to
   * decide how to quue the request.
   * @param logContext context allowing logging db operation
   * @return the selected vid (mostly for logging)
   */
  virtual std::string queueRetrieve(cta::common::dataStructures::RetrieveRequest &rqst,
    const cta::common::dataStructures::RetrieveFileQueueCriteria &criteria, log::LogContext &logContext) = 0;

  /**
   * Returns all of the existing retrieve jobs grouped by tape and then
   * sorted by creation time in ascending order (oldest first).
   *
   * @return All of the existing retrieve jobs grouped by tape and then
   * sorted by creation time in ascending order (oldest first).
   */
  virtual std::map<std::string, std::list<RetrieveRequestDump> > getRetrieveRequests()
    const = 0;

  /**
   * Returns the list of retrieve jobs associated with the specified tape
   * sorted by creation time in ascending order (oldest first).
   *
   * @param vid The volume identifier of the tape.
   * @return The list of retrieve jobs associated with the specified tape
   * sorted by creation time in ascending order (oldest first).
   */
  virtual std::list<RetrieveRequestDump> getRetrieveRequestsByVid(
    const std::string &vid) const = 0;
  
  /**
   * Returns the list of retrieve jobs associated with the specified requester
   * sorted by creation time in ascending order (oldest first).
   *
   * @param requester The requester who created the retrieve request.
   * @return The list of retrieve jobs associated with the specified tape
   * sorted by creation time in ascending order (oldest first).
   */
  virtual std::list<RetrieveRequestDump> getRetrieveRequestsByRequester(
    const std::string &requester) const = 0;
  
  /**
   * Deletes the specified retrieve job.
   *
   * @param requester The identity of the requester.
   * @param remoteFile The URL of the destination file.
   */
  virtual void deleteRetrieveRequest(
    const common::dataStructures::SecurityIdentity &cliIdentity,
    const std::string &remoteFile) = 0;
  
  /**
   * Returns all of the queued archive jobs.  The returned jobs are
   * grouped by tape pool and then sorted by creation time, oldest first.
   *
   * @return The queued jobs.
   */
  virtual std::map<std::string, std::list<common::dataStructures::RetrieveJob> >
    getRetrieveJobs() const = 0;

  /**
   * Returns the list of queued jobs queued on the specified tape pool.
   * The returned requests are sorted by creation time, oldest first.
   *
   * @param tapePoolName The name of the tape pool.
   * @return The queued requests.
   */
  virtual std::list<cta::common::dataStructures::RetrieveJob> getRetrieveJobs(
    const std::string &tapePoolName) const = 0;
  
  
  /*============ Retrieve management: tape server side ======================*/

  class RetrieveJob;
  class RetrieveMount {
  public:
    struct MountInfo {
      std::string vid;
      std::string logicalLibrary;
      std::string tapePool;
      std::string vo;
      std::string mediaType;
      std::string vendor;
      std::string drive;
      std::string host;
      uint64_t capacityInBytes;
      uint64_t mountId;
      optional<std::string> activity;
    } mountInfo;
    virtual const MountInfo & getMountInfo() = 0;
    virtual std::list<std::unique_ptr<cta::SchedulerDatabase::RetrieveJob>> getNextJobBatch(uint64_t filesRequested,
      uint64_t bytesRequested, log::LogContext& logContext) = 0;
    virtual void complete(time_t completionTime) = 0;
    virtual void setDriveStatus(common::dataStructures::DriveStatus status, time_t completionTime) = 0;
    virtual void setTapeSessionStats(const castor::tape::tapeserver::daemon::TapeSessionStats &stats) = 0;
    virtual void flushAsyncSuccessReports(std::list<cta::SchedulerDatabase::RetrieveJob *> & jobsBatch, log::LogContext & lc) = 0;
    virtual ~RetrieveMount() {}
    uint32_t nbFilesCurrentlyOnTape;
  };
  
  class RetrieveJob {
    friend class RetrieveMount;
  public:
    std::string errorReportURL;
    enum class ReportType: uint8_t {
      NoReportRequired,
      //CompletionReport,
      FailureReport,
      Report //!< A generic grouped type
    } reportType;
    cta::common::dataStructures::ArchiveFile archiveFile;
    cta::common::dataStructures::RetrieveRequest retrieveRequest;
    uint32_t selectedCopyNb;
    bool isRepack = false;
    /** Set the job successful (async). Wait() and end of report happen in RetrieveMount::flushAsyncSuccessReports() */
    virtual void asyncSetSuccessful() = 0;
    virtual void failTransfer(const std::string &failureReason, log::LogContext &lc) = 0;
    virtual void failReport(const std::string &failureReason, log::LogContext &lc) = 0;
    virtual ~RetrieveJob() {}
  private:
  };

  /*============ Repack management: user side ================================*/
  virtual void queueRepack(const std::string & vid, const std::string & bufferURL,
      common::dataStructures::RepackInfo::Type repackType, const common::dataStructures::MountPolicy &mountPolicy, log::LogContext & lc) = 0;
  virtual std::list<common::dataStructures::RepackInfo> getRepackInfo() = 0;
  virtual common::dataStructures::RepackInfo getRepackInfo(const std::string & vid) = 0;
  virtual void cancelRepack(const std::string & vid, log::LogContext & lc) = 0;
  
  /**
   * A class containing all the information needed for pending repack requests promotion.
   * We need to promote repack requests from "Pending" to "ToExpand" in a controlled
   * manner. This will ensure the presence of a sufficient amount of repack subrequests 
   * in the system in order to keep things going without clogging the system with too many
   * requests in the case of a massive repack.
   * The mechanism is the same as for TapeMountDecision info. Polling functions (implemented
   * in the Scheduler) get a lock free version of the requests summary, and if a promotion seems
   * required does so after re-taking a locked version of the statistics and re-ensuring that the
   * conditions are still valid, avoiding a race condition system wide.
   */
  class RepackRequestStatistics: public std::map<common::dataStructures::RepackInfo::Status, size_t> {
  public:
    RepackRequestStatistics();
    struct PromotionToToExpandResult {
      size_t pendingBefore;
      size_t toEnpandBefore;
      size_t pendingAfter;
      size_t toExpandAfter;
      size_t promotedRequests;
    };
    virtual PromotionToToExpandResult promotePendingRequestsForExpansion(size_t requestCount,
            log::LogContext &lc) = 0;
    virtual ~RepackRequestStatistics() {}
    // The pending request queue could be absent. This is not a big problem as 
    // there will be nothing to schedule anyway. This exception is thrown by the 
    // locking version only.
    CTA_GENERATE_EXCEPTION_CLASS(NoPendingRequestQueue);
  };
  CTA_GENERATE_EXCEPTION_CLASS(SchedulingLockNotHeld);
  virtual std::unique_ptr<RepackRequestStatistics> getRepackStatistics() = 0;
  virtual std::unique_ptr<RepackRequestStatistics> getRepackStatisticsNoLock() = 0;
  
  /**
   * A class providing the per repack request interface. It is also used to create the per file
   * subrequests in the object store.
   */
  class RepackRequest {
  public:
    cta::common::dataStructures::RepackInfo repackInfo;
    virtual uint64_t getLastExpandedFSeq() = 0;
    virtual void setLastExpandedFSeq(uint64_t fseq) = 0;
    struct Subrequest {
      uint64_t fSeq;
      cta::common::dataStructures::ArchiveFile archiveFile;
      std::set<uint32_t> copyNbsToRearchive;
      std::string fileBufferURL;
    };
    
    //Struct to hold the RepackRequest's total stats
    struct TotalStatsFiles{
      uint64_t totalFilesToArchive = 0;
      uint64_t totalBytesToArchive = 0;
      uint64_t totalFilesToRetrieve = 0;
      uint64_t totalBytesToRetrieve = 0;
      //TODO : userprovidedfiles and userprovidedbytes
    };
    
    virtual void addSubrequestsAndUpdateStats(std::list<Subrequest>& repackSubrequests, 
      cta::common::dataStructures::ArchiveRoute::FullMap & archiveRoutesMap, uint64_t maxFSeqLowBound, const uint64_t maxAddedFSeq, const TotalStatsFiles &totalStatsFiles, log::LogContext & lc) = 0;
    virtual void expandDone() = 0;
    virtual void fail() = 0;
    virtual void requeueInToExpandQueue(log::LogContext &lc) = 0;
    virtual void setExpandStartedAndChangeStatus() = 0;
    virtual void fillLastExpandedFSeqAndTotalStatsFile(uint64_t &fSeq, TotalStatsFiles &totalStatsFiles) = 0;
    virtual ~RepackRequest() {}
  };
  
  /***/
  virtual std::unique_ptr<RepackRequest> getNextRepackJobToExpand() = 0;
  
  /*============ Repack management: maintenance process side =========================*/
  
  /*!
   * Get a a set of failed jobs to report to the client.
   *
   * This method is like RetrieveMount::getNextJobBatch. It it not in the context of a mount as any
   * process can grab a batch of jobs to report and proceed with the reporting.
   *
   * @returns    A list of process-owned jobs to report
   */
  virtual std::list<std::unique_ptr<RetrieveJob>> getNextRetrieveJobsToReportBatch(uint64_t filesRequested, log::LogContext &logContext) = 0;
  virtual std::list<std::unique_ptr<RetrieveJob>> getNextRetrieveJobsFailedBatch(uint64_t filesRequested, log::LogContext &logContext) = 0;
  
  /**
   * A base class handling the various types of reports to repack. Implementation if left to Db implementer.
   */
  class RepackReportBatch {
  public:
    virtual ~RepackReportBatch() {}
    virtual void report(log::LogContext & lc) = 0;
  };
  
  /**
   * Extracts a batch of subrequests from the database to be reported to repack.
   * @param lc log context
   * @return Next batch to report.
   */
  virtual std::unique_ptr<RepackReportBatch> getNextRepackReportBatch(log::LogContext & lc) = 0;
  
  virtual std::unique_ptr<RepackReportBatch> getNextSuccessfulRetrieveRepackReportBatch(log::LogContext &lc) = 0;
  
  virtual std::unique_ptr<RepackReportBatch> getNextSuccessfulArchiveRepackReportBatch(log::LogContext &lc) = 0;
  
  virtual std::unique_ptr<RepackReportBatch> getNextFailedRetrieveRepackReportBatch(log::LogContext &lc) = 0;
  
  virtual std::unique_ptr<RepackReportBatch> getNextFailedArchiveRepackReportBatch(log::LogContext &lc) = 0;
  
  /**
   * Return all batches of subrequests from the database to be reported to repack.
   * @param lc log context
   * @return the list of all batches to be reported
   */
  virtual std::list<std::unique_ptr<RepackReportBatch>> getRepackReportBatches(log::LogContext &lc) = 0;
  
  /**
   * Set a batch of jobs as reported (modeled on ArchiveMount::setJobBatchSuccessful().
   * @param jobsBatch
   * @param lc
   */
  virtual void setRetrieveJobBatchReportedToUser(std::list<cta::SchedulerDatabase::RetrieveJob*> & jobsBatch, log::TimingList & timingList, utils::Timer & t, log::LogContext & lc) = 0;

  virtual JobsFailedSummary getRetrieveJobsFailedSummary(log::LogContext &logContext) = 0;

  /*============ Label management: user side =================================*/
  // TODO
  
  /*============ Label management: tape server side ==========================*/
  class LabelMount {}; // TODO
  
  /*============ Session management ==========================================*/
  /**
   * A structure describing a potential mount with all the information allowing
   * comparison between mounts.
   */
  struct PotentialMount {
    cta::common::dataStructures::MountType type;    /**< Is this an archive, retireve or label? */
    std::string vid;              /**< The tape VID (for a retieve) */
    std::string tapePool;         /**< The name of the tape pool for both archive 
                                   * and retrieve */
    std::string vo;               // Virtual organization of the tape
    std::string mediaType;        // Media type of the tape
    std::string vendor;           // Vendor of the tape
    uint64_t capacityInBytes;     // Capacity in bytes of the tape
    
    uint64_t priority;            /**< The priority for the mount, defined as the highest 
                                   * priority of all queued jobs */
    uint64_t maxDrivesAllowed;    /**< The maximum number of drives allowed for this 
                                   * tape pool, defined as the highest value amongst 
                                   * jobs */
    time_t minRequestAge;         /**< The maximum amount of time to wait before 
                                   * forcing a mount in the absence of enough data. 
                                   * Defined as the smallest value amongst jobs.*/
    uint64_t filesQueued;         /**< The number of files queued for this queue */
    uint64_t bytesQueued;         /**< The amount of data currently queued */
    time_t oldestJobStartTime;    /**< Creation time of oldest request */
    std::string logicalLibrary;   /**< The logical library (for a retrieve) */
    double ratioOfMountQuotaUsed; /**< The [ 0.0, 1.0 ] ratio of existing 
                                   * mounts/quota (for faire share of mounts)*/
    uint32_t mountCount;          /**< The number of mounts for this tape pool (which is the current "chargeable" entity for quotas. */
    struct ActivityNameAndWeightedMountCount {
      std::string activity;
      double weight = 0.0;
      uint32_t mountCount = 0;
      double weightedMountCount = 0.0;
    };                            /**< Struct describing the activity if we have one for this mount. */
    
    optional<ActivityNameAndWeightedMountCount> activityNameAndWeightedMountCount;
                                  /**< Description if the activity for this potential mount. */
    
    
    bool operator < (const PotentialMount &other) const {
      if (priority < other.priority)
        return true;
      if (priority > other.priority)
        return false;
      if (type == cta::common::dataStructures::MountType::ArchiveForUser && other.type != cta::common::dataStructures::MountType::ArchiveForUser)
        return false;
      if (other.type == cta::common::dataStructures::MountType::ArchiveForUser && type != cta::common::dataStructures::MountType::ArchiveForUser)
        return true;
      // If we have achieved a HIGHER ratio of our mount allowance, then the other mount will be privileged
      if (ratioOfMountQuotaUsed > other.ratioOfMountQuotaUsed)
        return true;
      if (ratioOfMountQuotaUsed < other.ratioOfMountQuotaUsed)
        return false;
      // If we have activities (and the mounts are for the same tape pool) we can compare them.
      // If not, it does not matter too much: one mount will go, increasing its ratio, and next time it will
      // the tapepool. So for different tape pools, we do not order. Likewise, both mounts should have an activity to
      // be comparable
      if (activityNameAndWeightedMountCount && other.activityNameAndWeightedMountCount && tapePool == other.tapePool) {
        if (activityNameAndWeightedMountCount.value().weightedMountCount > other.activityNameAndWeightedMountCount.value().weightedMountCount)
          return true;
        if (activityNameAndWeightedMountCount.value().weightedMountCount < other.activityNameAndWeightedMountCount.value().weightedMountCount)
          return false;
      }
      //The smaller the oldest job start time is, the bigger the age is, hence the inverted comparison
      if(oldestJobStartTime > other.oldestJobStartTime)
	return true;
      if(oldestJobStartTime < other.oldestJobStartTime)
	return false;
      /**
       * For the tests, we try to have the priority by 
       * alphabetical order : vid1 / tapepool1 should be treated before vid2/tapepool2,
       * so if this->vid < other.vid : then this > other.vid, so return false
       */
      if(vid < other.vid)
	return false;
      if(vid > other.vid)
	return true;
      
      if(tapePool < other.tapePool)
	return false;
      if(tapePool > other.tapePool)
	return true;
      
      return false;
    }
  };
  

  /**
   * Information about the existing mounts.
   */
  struct ExistingMount {
    std::string driveName;
    cta::common::dataStructures::MountType type;
    std::string tapePool;
    std::string vid;
    bool currentMount; ///< True if the mount is current (othermise, it's a next mount).
    uint64_t bytesTransferred;
    uint64_t filesTransferred;
    double latestBandwidth;
    uint64_t priority;
    optional<std::string> activity;
  };
  
  /**
   * An entry (to be indexed by drive name (string) in a map) for the dedication
   * lists of each drive.
   */
  struct DedicationEntry {
    // TODO.
  };
  
  /**
   * A class containing all the information needed for mount decision
   * and whose creation implicitly takes a global lock on the drive register
   * so that only one mount scheduling happens at a time. Two member functions
   * then allow the 
   */
  class TapeMountDecisionInfo {
  public:
    std::vector<PotentialMount> potentialMounts; /**< All the potential mounts */
    std::vector<ExistingMount> existingOrNextMounts; /**< Existing mounts */
    std::map<std::string, DedicationEntry> dedicationInfo; /**< Drives dedication info */
    bool queueTrimRequired = false; /**< Indicates an empty queue was encountered */
    /**
     * Create a new archive mount. This implicitly releases the global scheduling
     * lock.
     */
    virtual std::unique_ptr<ArchiveMount> createArchiveMount(
      common::dataStructures::MountType mountType,
      const catalogue::TapeForWriting & tape, const std::string driveName, 
      const std::string & logicalLibrary, const std::string & hostName,
      const std::string& vo, const std::string& mediaType,
      const std::string& vendor,
      const uint64_t capacityInBytes,
      time_t startTime) = 0;
    /**
     * Create a new retrieve mount. This implicitly releases the global scheduling
     * lock.
     */
    virtual std::unique_ptr<RetrieveMount> createRetrieveMount(const std::string & vid,
      const std::string & tapePool, const std::string driveName, 
      const std::string& logicalLibrary, const std::string& hostName,
      const std::string& vo, const std::string& mediaType,
      const std::string& vendor,
      const uint64_t capacityInBytes,
      time_t startTime, const optional<common::dataStructures::DriveState::ActivityAndWeight> &) = 0;
    /** Destructor: releases the global lock if not already done */
    virtual ~TapeMountDecisionInfo() {};
  };
  
  /**
   * A function dumping the relevant mount information for deciding which
   * tape to mount next. This also starts the mount decision process (and takes
   * a global lock on for scheduling).
   */
  virtual std::unique_ptr<TapeMountDecisionInfo> getMountInfo(log::LogContext& logContext) = 0;
  
  /**
   * A function running a queue trim. This should be called if the corresponding
   * bit was set in the TapeMountDecisionInfo returned by getMountInfo().
   */
  virtual void trimEmptyQueues(log::LogContext & lc) = 0;
  
  /**
   * A function dumping the relevant mount information for reporting the system
   * status. It is identical to getMountInfo, yet does not take the global lock.
   */
  virtual std::unique_ptr<TapeMountDecisionInfo> getMountInfoNoLock(log::LogContext& logContext) = 0;
  
  /* === Drive state handling  ============================================== */
  /**
   * Returns the current list of registered drives.
   *
   * @return The current list of registered drives.
   */
  virtual std::list<cta::common::dataStructures::DriveState> getDriveStates(log::LogContext & lc) const = 0;
  
  /**
   * Sets the administrative desired state (up/down/force down) for an existing drive.
   * Will throw an excpeiton is the drive does not exist
   * @param drive
   * @param desiredState
   */
  virtual void setDesiredDriveState(const std::string & drive, const cta::common::dataStructures::DesiredDriveState & state,
    log::LogContext & lc) = 0;
  
  /**
   * Remove drive from the drive register.
   * Will throw an exception is the drive does not exist.
   * 
   * @param drive The drive name.
   */
  virtual void removeDrive(const std::string & drive, log::LogContext & logContext) = 0;
  
  /**
   * Sets the drive status in the object store. The drive status will be recorded in all cases,
   * although some historical information is needed to provide an accurate view of the
   * current session state. This allows the system to gracefully handle drive entry
   * deletion at any time (an operator operation).
   * @param driveInfo Fundamental information about the drive.
   * @param mountType Mount type (required).
   * @param status Current drive status (required).
   * @param reportTime Time of report (required).
   * @param mountSessionId (optional, required by some statuses).
   * @param byteTransfered (optional, required by some statuses).
   * @param filesTransfered (optional, required by some statuses).
   * @param latestBandwidth (optional, required by some statuses).
   * @param vid (optional, required by some statuses).
   * @param tapepool (optional, required by some statuses).
   */
  virtual void reportDriveStatus (const common::dataStructures::DriveInfo & driveInfo,
    cta::common::dataStructures::MountType mountType,
    common::dataStructures::DriveStatus status, 
    time_t reportTime,
    log::LogContext & lc,
    uint64_t mountSessionId = std::numeric_limits<uint64_t>::max(),
    uint64_t byteTransfered = std::numeric_limits<uint64_t>::max(),
    uint64_t filesTransfered = std::numeric_limits<uint64_t>::max(),
    double latestBandwidth = std::numeric_limits<double>::max(),
    const std::string & vid = "",
    const std::string & tapepool = "") = 0;
  
  virtual void reportDriveConfig(const cta::tape::daemon::TpconfigLine& tpConfigLine, const cta::tape::daemon::TapedConfiguration& tapedConfig,log::LogContext& lc) = 0;
}; // class SchedulerDatabase

} // namespace cta

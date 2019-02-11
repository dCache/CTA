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
#include "common/threading/Mutex.hpp"
#include "common/threading/MutexLocker.hpp"
#include "catalogue/Catalogue.hpp"
#include "scheduler/OStoreDB/OStoreDB.hpp"
#include "JobQueueType.hpp"
#include "RepackQueueType.hpp"
#include <string>
#include <set>
#include <future>

/**
 * A collection of helper functions for commonly used multi-object operations
 */

namespace cta { namespace objectstore {

class ScopedExclusiveLock;
class AgentReference;
class DriveState;
class RepackQueue;

/**
 * A class with static functions allowing multi-object operations
 */
class Helpers {
public:
  /**
   * Find or create an archive or retrieve queue, and return it locked and fetched to the caller
   * (Queue and ScopedExclusiveLock objects are provided empty)
   * @param queue the queue object, empty
   * @param queueLock the lock, not initialized
   * @param agentReference the agent reference that will be needed in case of object creation
   * @param tapePool or vid the name of the needed tape pool
   */
  template <class Queue>
  static void getLockedAndFetchedJobQueue(Queue & queue, 
    ScopedExclusiveLock & queueLock, AgentReference & agentReference, 
    const cta::optional<std::string> & tapePoolOrVid, JobQueueType queueType, log::LogContext & lc);
  
  /**
   * Find or create a repack queue, and return it locked and fetched to the caller
   * (Queue and ScopedExclusiveLock objects are provided empty)
   * @param queue the queue object, empty
   * @param queueLock the lock, not initialized
   * @param agentReference the agent reference that will be needed in case of object creation
   */
  static void getLockedAndFetchedRepackQueue(RepackQueue & queue, 
    ScopedExclusiveLock & queueLock, AgentReference & agentReference, 
    RepackQueueType queueType, log::LogContext & lc);
  
  CTA_GENERATE_EXCEPTION_CLASS(NoTapeAvailableForRetrieve);
  /**
   * Find the most appropriate queue (bid) to add the retrieve request to. The potential
   * VIDs (VIDs for non-failed copies) is provided by the caller. The status of the
   * the tapes (disabled or not, and available queue size) are all cached to avoid 
   * frequent access to the object store. The caching create a small inefficiency 
   * to the algorithm, but will help performance drastically for a very similar result
   */
  static std::string selectBestRetrieveQueue (const std::set<std::string> & candidateVids, cta::catalogue::Catalogue & catalogue, 
  objectstore::Backend & objectstore);
  
  /**
   * Gets the retrieve queue statistics for a set of Vids (extracted from the OStoreDB
   * so it can be used in the Helper context without passing the DB object.
   */
  static std::list<SchedulerDatabase::RetrieveQueueStatistics> getRetrieveQueueStatistics(
    const cta::common::dataStructures::RetrieveFileQueueCriteria& criteria,
    const std::set<std::string> & vidsToConsider,
    objectstore::Backend & objectstore);
  
  /**
   * Opportunistic updating of the queue stats cache as we access it. This implies the
   * tape is not disabled (full status not fetched).
   */
  static void updateRetrieveQueueStatisticsCache(const std::string & vid, uint64_t files, uint64_t bytes, uint64_t priority);
  
private:
  /** Lock for the retrieve queues stats */
  static cta::threading::Mutex g_retrieveQueueStatisticsMutex;
  /** A struct holding together RetrieveQueueStatistics, tape status and an update time. */
  struct RetrieveQueueStatisticsWithTime {
    cta::SchedulerDatabase::RetrieveQueueStatistics stats;
    cta::common::dataStructures::Tape tapeStatus;
    bool updating;
    /** The shared future will allow all updating safely an entry of the cache while 
     * releasing the global mutex to allow threads interested in other VIDs to carry on.*/
    std::shared_future<void> updateFuture; 
    time_t updateTime;
  };
  /** The stats for the queues */
  static std::map<std::string, RetrieveQueueStatisticsWithTime> g_retrieveQueueStatistics;
  /** Time between cache updates */
  static const time_t c_retrieveQueueCacheMaxAge = 10;
  
public:
  
  enum class CreateIfNeeded: bool {
    create=true,
    doNotCreate=false
  };
  
  CTA_GENERATE_EXCEPTION_CLASS(NoSuchDrive);
  /**
   * Helper to create a drive status object if needed, and return
   * it locked exclusively and fetched.
   * If the state is created, it will be populated with default values.
   */
  static void getLockedAndFetchedDriveState(DriveState & driveState, ScopedExclusiveLock & driveStateLock,
    AgentReference & agentReference, const std::string & driveName, log::LogContext & lc, CreateIfNeeded doCreate=CreateIfNeeded::create);
  
  /**
   * Helper to fetch in parallel all the drive statuses.
   */
  static std::list<cta::common::dataStructures::DriveState> getAllDriveStates(Backend & backend, log::LogContext & lc);
  
  /**
   * Helper to register a repack request in the repack index. 
   * As this structure was developed late, we potentially have to create it on the fly.
   */
  static void registerRepackRequestToIndex(const std::string & vid, const std::string & requestAddress,
      AgentReference & agentReference, Backend & backend, log::LogContext & lc);
  
  /**
   * Helper to remove an entry form the repack index.
   */
  static void removeRepackRequestToIndex(const std::string & vid, Backend & backend, log::LogContext & lc);
};

}} // namespace cta::objectstore

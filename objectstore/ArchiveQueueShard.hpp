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

#include "ArchiveQueue.hpp"

namespace cta { namespace objectstore {
  
class ArchiveQueueShard: public ObjectOps<serializers::ArchiveQueueShard, serializers::ArchiveQueueShard_t>  {
public:
  // Constructor with undefined address
  ArchiveQueueShard(Backend & os);
  
  // Constructor
  ArchiveQueueShard(const std::string & address, Backend & os);
  
  // Upgrader form generic object
  ArchiveQueueShard(GenericObject & go);
  
  // Forbid/hide base initializer
  void initialize() = delete;
  
  // Initializer
  void initialize(const std::string & owner);
  
  // dumper
  std::string dump();
  
  void garbageCollect(const std::string& presumedOwner, AgentReference& agentReference, log::LogContext& lc, cta::catalogue::Catalogue& catalogue) override;

  struct JobInfo {
    uint64_t size;
    std::string address;
    uint32_t copyNb;
    uint64_t priority;
    uint64_t minArchiveRequestAge;
    uint64_t maxDrivesAllowed;
    time_t startTime;
  };
  std::list<JobInfo> dumpJobs();
  
  struct JobsSummary {
    uint64_t jobs;
    uint64_t bytes;
  };
  JobsSummary getJobsSummary();
  
  /**
   * adds job, returns new size
   */
  uint64_t addJob(ArchiveQueue::JobToAdd & jobToAdd);
  
  
  struct RemovalResult {
    uint64_t jobsRemoved = 0;
    uint64_t jobsAfter = 0;
    uint64_t bytesRemoved = 0;
    uint64_t bytesAfter = 0;
    std::list<JobInfo> removedJobs;
  };
  /**
   * Removes jobs from shard (and from the to remove list). Returns list of removed jobs.
   */
  RemovalResult removeJobs(const std::list<std::string> & jobsToRemove);
  
  ArchiveQueue::CandidateJobList getCandidateJobList(uint64_t maxBytes, uint64_t maxFiles, std::set<std::string> archiveRequestsToSkip);
  
  /** Re compute summaries in case they do not match the array content. */
  void rebuild();
  
};

}} // namespace cta::objectstore
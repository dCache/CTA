/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2015-2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ObjectOps.hpp"
#include "objectstore/cta.pb.h"
#include "RetrieveRequest.hpp"
#include "scheduler/RetrieveRequestDump.hpp"
#include "RetrieveActivityCountMap.hpp"

namespace cta { namespace objectstore {
  
class Backend;
class Agent;
class GenericObject;

class RetrieveQueue: public ObjectOps<serializers::RetrieveQueue, serializers::RetrieveQueue_t> {
public:
  // Trait to specify the type of jobs associated with this type of queue
  typedef common::dataStructures::RetrieveJob job_t;

  RetrieveQueue(const std::string & address, Backend & os);
  // Undefined object constructor
  RetrieveQueue(Backend & os);
  RetrieveQueue(GenericObject & go);
  void initialize(const std::string & vid);
  void commit();
  void getPayloadFromHeader() override;

private:
  // Validates all summaries are in accordance with each other.
  bool checkMapsAndShardsCoherency();
  
  // Rebuild from shards if something goes wrong.
  void rebuild();
public:
  
  void garbageCollect(const std::string &presumedOwner, AgentReference & agentReference, log::LogContext & lc,
    cta::catalogue::Catalogue & catalogue) override;
  bool isEmpty();
  CTA_GENERATE_EXCEPTION_CLASS(NotEmpty);
  void removeIfEmpty(log::LogContext & lc);
  std::string dump();
  
  // Retrieve jobs management ==================================================
  struct JobToAdd {
    uint32_t copyNb;
    uint64_t fSeq;
    std::string retrieveRequestAddress;
    uint64_t fileSize;
    cta::common::dataStructures::MountPolicy policy;
    time_t startTime;
    optional<RetrieveActivityDescription> activityDescription;
    optional<std::string> diskSystemName;
  };
  void addJobsAndCommit(std::list<JobToAdd> & jobsToAdd, AgentReference & agentReference, log::LogContext & lc);
  // This version will check for existence of the job in the queue before
  // returns the count and sizes of actually added jobs (if any).
  struct AdditionSummary {
    uint64_t files = 0;
    uint64_t bytes = 0;
  };
  AdditionSummary addJobsIfNecessaryAndCommit(std::list<JobToAdd> & jobsToAdd,
    AgentReference & agentReference, log::LogContext & lc);
  struct JobsSummary {
    uint64_t jobs;
    uint64_t bytes;
    time_t oldestJobStartTime;
    uint64_t priority;
    uint64_t minRetrieveRequestAge;
    std::map<std::string, uint64_t> mountPolicyCountMap;
    struct ActivityCount {
      std::string diskInstanceName;
      std::string activity;
      double weight;
      uint64_t count;
    };
    std::list<ActivityCount> activityCounts;
    struct SleepInfo {
      time_t sleepStartTime;
      std::string diskSystemSleptFor;
      uint64_t sleepTime;
    };
    optional<SleepInfo> sleepInfo;
  };
  JobsSummary getJobsSummary();
  struct JobDump {
    std::string address;
    uint32_t copyNb;
    uint64_t size;
    struct ActivityDescription {
      std::string diskInstanceName;
      std::string activity;
    };
    optional<ActivityDescription> activity;
    optional<std::string> diskSystemName;
  };
  std::list<JobDump> dumpJobs();
  struct CandidateJobList {
    uint64_t remainingFilesAfterCandidates = 0;
    uint64_t remainingBytesAfterCandidates = 0;
    uint64_t candidateFiles = 0;
    uint64_t candidateBytes = 0;
    std::list<JobDump> candidates;
  };
  // The set of retrieve requests to skip are requests previously identified by the caller as bad,
  // which still should be removed from the queue. They will be disregarded from  listing.
  CandidateJobList getCandidateList(uint64_t maxBytes, uint64_t maxFiles, const std::set<std::string> & retrieveRequestsToSkip,
    const std::set<std::string> & diskSystemsToSkip);
 

  //! Return a summary of the number of jobs and number of bytes in the queue
  CandidateJobList getCandidateSummary();
  
  void removeJobsAndCommit(const std::list<std::string> & jobsToRemove);
  // -- Generic parameters
  std::string getVid();
  
  // Support for sleep waiting free space (back pressure).
  // This data is queried through getJobsSummary().
  void setSleepForFreeSpaceStartTimeAndName(time_t time, const std::string & diskSystemName, uint64_t sleepTime);
  void resetSleepForFreeSpaceStartTime();
  
private:
  struct ShardForAddition {
    bool newShard=false;
    bool creationDone=false;
    bool splitDone=false;
    bool toSplit=false;
    bool comitted=false;
    ShardForAddition * splitDestination = nullptr;
    bool fromSplit=false;
    ShardForAddition * splitSource = nullptr;
    std::string address;
    uint64_t minFseq;
    uint64_t maxFseq;
    uint64_t jobsCount;
    std::list<RetrieveQueue::JobToAdd> jobsToAdd;
    size_t shardIndex = std::numeric_limits<size_t>::max();
  };
  
  void updateShardLimits(uint64_t fSeq, ShardForAddition & sfa);
  
  void addJobToShardAndMaybeSplit(RetrieveQueue::JobToAdd & jobToAdd, 
    std::list<ShardForAddition>::iterator & shardForAddition, std::list<ShardForAddition> & shardList);
  
public:
  /** Helper function for unit tests: use smaller shard size to validate ordered insertion */
  void setShardSize(uint64_t shardSize);
  
  /** Helper function for unit tests: validate that we have the expected number of shards */
  uint64_t getShardCount();
private:
  // The shard size. From experience, 100k is where we start to see performance difference,
  // but nothing prevents us from using a smaller size.
  // The performance will be roughly flat until the queue size reaches the square of this limit
  // (meaning the queue object updates start to take too much time).
  // with this current value of 25k, the performance should be roughly flat until 25k^2=625M.
  static const uint64_t c_defaultMaxShardSize = 25000;
  
  uint64_t m_maxShardSize = c_defaultMaxShardSize;
};

class RetrieveQueueToTransfer : public RetrieveQueue { using RetrieveQueue::RetrieveQueue; };
class RetrieveQueueToReportForUser : public RetrieveQueue { using RetrieveQueue::RetrieveQueue; };
class RetrieveQueueFailed : public RetrieveQueue { using RetrieveQueue::RetrieveQueue; };
class RetrieveQueueToReportToRepackForSuccess : public RetrieveQueue { using RetrieveQueue::RetrieveQueue; };
class RetrieveQueueToReportToRepackForFailure: public RetrieveQueue { using RetrieveQueue::RetrieveQueue; };

}}


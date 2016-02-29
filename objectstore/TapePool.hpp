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

#include "Backend.hpp"
#include "ObjectOps.hpp"
#include <string>
#include "objectstore/cta.pb.h"
#include "common/CreationLog.hpp"
#include "common/MountControl.hpp"
#include "ArchiveToFileRequest.hpp"
#include "ArchiveRequest.hpp"
#include "CreationLog.hpp"
#include "Agent.hpp"
#include "common/archiveNS/Tape.hpp"

namespace cta { namespace objectstore {
  
class GenericObject;

class TapePool: public ObjectOps<serializers::TapePool> {
public:
  // Constructor
  TapePool(const std::string & address, Backend & os);
  
  // Upgrader form generic object
  TapePool(GenericObject & go);

  // In memory initialiser
  void initialize(const std::string & name);
  
  // Set/get name
  void setName(const std::string & name);
  std::string getName();
  
  // Tapes management ==========================================================
  std::string addOrGetTapeAndCommit(const std::string &vid, 
    const std::string &logicalLibraryName, const uint64_t capacityInBytes,
    Agent & agent, const cta::CreationLog & CreationLog);
  CTA_GENERATE_EXCEPTION_CLASS(NoSuchTape);
  CTA_GENERATE_EXCEPTION_CLASS(WrongTape);
  void removeTapeAndCommit(const std::string &vid);
  std::string getTapeAddress(const std::string &vid);
  class TapeBasicDump {
  public:
    std::string vid;
    std::string address;
    std::string logicalLibraryName;
    uint64_t capacityInBytes;
    objectstore::CreationLog log;
  };
  std::list<TapeBasicDump> dumpTapes();
  class TapeDump {
  public:
    std::string vid;
    std::string address;
    std::string logicalLibraryName;
    uint64_t capacityInBytes;
    objectstore::CreationLog log;
    cta::Tape::Status status;
  };
  std::list<TapeDump> dumpTapesAndFetchStatus();
  
  // Archive jobs management ===================================================
  void addJob(const ArchiveToFileRequest::JobDump & job,
    const std::string & archiveToFileAddress, const std::string & path,
    uint64_t size, uint64_t priority, time_t startTime);
  /// This version will check for existence of the job in the queue before
  // returns true if a new job was actually inserted.
  bool addJobIfNecessary(const ArchiveToFileRequest::JobDump & job,
    const std::string & archiveToFileAddress, 
    const std::string & path, uint64_t size);
  /// This version will check for existence of the job in the queue before
  // returns true if a new job was actually inserted.
  bool addOrphanedJobPendingNsCreation(const ArchiveToFileRequest::JobDump& job,
    const std::string& archiveToFileAddress, const std::string & path,
    uint64_t size);
  /// This version will check for existence of the job in the queue before
  // returns true if a new job was actually inserted.
  bool addOrphanedJobPendingNsDeletion(const ArchiveToFileRequest::JobDump& job,
    const std::string& archiveToFileAddress,
    const std::string & path, uint64_t size);
  
  void addJob(const ArchiveRequest::JobDump & job,
    const std::string & archiveToFileAddress, const std::string & path,
    uint64_t size, uint64_t priority, time_t startTime);
  /// This version will check for existence of the job in the queue before
  // returns true if a new job was actually inserted.
  bool addJobIfNecessary(const ArchiveRequest::JobDump & job,
    const std::string & archiveToFileAddress, 
    const std::string & path, uint64_t size);
  /// This version will check for existence of the job in the queue before
  // returns true if a new job was actually inserted.
  bool addOrphanedJobPendingNsCreation(const ArchiveRequest::JobDump& job,
    const std::string& archiveToFileAddress, const std::string & path,
    uint64_t size);
  /// This version will check for existence of the job in the queue before
  // returns true if a new job was actually inserted.
  bool addOrphanedJobPendingNsDeletion(const ArchiveRequest::JobDump& job,
    const std::string& archiveToFileAddress,
    const std::string & path, uint64_t size);
  
  struct JobsSummary {
    uint64_t files;
    uint64_t bytes;
    time_t oldestJobStartTime;
    uint64_t priority;
  };
  JobsSummary getJobsSummary();
  
  void removeJob(const std::string &archiveToFileAddress);
  class JobDump {
  public:
    uint64_t size;
    std::string address;
    uint16_t copyNb;
  };
  std::list<JobDump> dumpJobs();
  
  // Mount management ==========================================================
  MountCriteriaByDirection getMountCriteriaByDirection();
  void setMountCriteriaByDirection(const MountCriteriaByDirection & mountCriteria);

  
  // Retry limits ==============================================================
  uint16_t getMaxRetriesWithinMount();
  void setMaxRetriesWithinMount(uint16_t maxRetriesPerMount);
  uint16_t getMaxTotalRetries();
  void setMaxTotalRetries( uint16_t maxTotalRetries);
  
  // Check that the tape pool is empty (of both tapes and jobs)
  bool isEmpty();
 
  CTA_GENERATE_EXCEPTION_CLASS(NotEmpty);
  // Garbage collection
  void garbageCollect(const std::string &presumedOwner);
  
  std::string dump();
};
  
}}
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

#include "GenericObject.hpp"
#include "Agent.hpp"
#include "AgentWatchdog.hpp"
#include "AgentRegister.hpp"
#include "JobQueueType.hpp"
#include "common/log/LogContext.hpp"

/**
 * Plan => Garbage collector keeps track of the agents.
 * If an agent is declared dead => tape ownership of owned objects
 * Using the backup owner, re-post the objet to the container.
 * All containers will have a "repost" method, which is more thourough 
 * (and expensive) than the usual one. It can for example prevent double posting.
 */

namespace cta { namespace objectstore {

class ArchiveRequest;
class RetrieveRequest;

class GarbageCollector {
public:
  GarbageCollector(Backend & os, AgentReference & agentReference, catalogue::Catalogue & catalogue);
  
  void runOnePass(log::LogContext & lc);
  
  void aquireTargets(log::LogContext & lc);
  
  void trimGoneTargets(log::LogContext & lc);
  
  void checkHeartbeats(log::LogContext & lc);
  
  void cleanupDeadAgent(const std::string & name, std::list<log::Param> agentDetails, log::LogContext & lc);

  /** Structure allowing the sorting of owned objects, so they can be requeued in batches,
    * one batch per queue. */
  struct OwnedObjectSorter {
    std::map<std::tuple<std::string, JobQueueType>, std::list<std::shared_ptr <ArchiveRequest>>> archiveQueuesAndRequests;
    std::map<std::tuple<std::string, JobQueueType>, std::list<std::shared_ptr <RetrieveRequest>>> retrieveQueuesAndRequests;
    std::list<std::shared_ptr<GenericObject>> otherObjects;
    /// Fill up the fetchedObjects with objects of interest.
    void fetchOwnedObjects(Agent & agent, std::list<std::shared_ptr<GenericObject>> & fetchedObjects, Backend & objectStore,
        log::LogContext & lc);
    /// Fill up the sorter with the fetched objects
    void sortFetchedObjects(Agent & agent, std::list<std::shared_ptr<GenericObject>> & fetchedObjects, Backend & objectStore,
        cta::catalogue::Catalogue & catalogue, log::LogContext & lc);
    /// Lock, fetch and update archive jobs
    void lockFetchAndUpdateArchiveJobs(Agent & agent, AgentReference & agentReference, Backend & objectStore, log::LogContext & lc);
    /// Lock, fetch and update retrieve jobs
    void lockFetchAndUpdateRetrieveJobs(Agent & agent, AgentReference & agentReference, Backend & objectStore, log::LogContext & lc);
    // Lock, fetch and update other objects
    void lockFetchAndUpdateOtherObjects(Agent & agent, AgentReference & agentReference, Backend & objectStore,
        cta::catalogue::Catalogue & catalogue, log::LogContext & lc);
  };
private:
  Backend & m_objectStore;
  catalogue::Catalogue & m_catalogue;
  AgentReference & m_ourAgentReference;
  AgentRegister m_agentRegister;
  std::map<std::string, AgentWatchdog * > m_watchedAgents;
};
  
}}
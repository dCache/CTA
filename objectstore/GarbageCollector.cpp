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

#include "GarbageCollector.hpp"
#include "AgentReference.hpp"
#include "ArchiveRequest.hpp"
#include "RetrieveRequest.hpp"
#include "ArchiveQueueAlgorithms.hpp"
#include "RetrieveQueue.hpp"
#include "Helpers.hpp"
#include "RootEntry.hpp"
#include <algorithm>
#include <unistd.h>

namespace cta { namespace objectstore {

GarbageCollector::GarbageCollector(Backend & os, AgentReference & agentReference, catalogue::Catalogue & catalogue): 
  m_objectStore(os), m_catalogue(catalogue), m_ourAgentReference(agentReference), m_agentRegister(os) {
  RootEntry re(m_objectStore);
  ScopedSharedLock reLock(re);
  re.fetch();
  m_agentRegister.setAddress(re.getAgentRegisterAddress());
  reLock.release();
  ScopedSharedLock arLock(m_agentRegister);
  m_agentRegister.fetch();
}

void GarbageCollector::runOnePass(log::LogContext & lc) {
  trimGoneTargets(lc);
  aquireTargets(lc);
  checkHeartbeats(lc);
}
  
void GarbageCollector::trimGoneTargets(log::LogContext & lc) {
  m_agentRegister.fetchNoLock();
  std::list<std::string> agentList = m_agentRegister.getAgents();
  // Find the agents we knew about and are not listed anymore.
  // We will just stop looking for them.
  for (std::map<std::string, AgentWatchdog * >::iterator wa
        = m_watchedAgents.begin();
      wa != m_watchedAgents.end();) {
    if (agentList.end() == std::find(agentList.begin(), agentList.end(), wa->first)) {
      delete wa->second;
      log::ScopedParamContainer params(lc);
      params.add("agentAddress", wa->first);
      m_watchedAgents.erase(wa++);
      lc.log(log::INFO, "In GarbageCollector::trimGoneTargets(): removed now gone agent.");
    } else {
      wa++;
    }
  }
}

void GarbageCollector::aquireTargets(log::LogContext & lc) {
  m_agentRegister.fetchNoLock();
  // We will now watch all agents we do not know about yet.
  std::list<std::string> candidatesList = m_agentRegister.getUntrackedAgents();
  // Build a set of our own tracked agents.
  std::set<std::string> alreadyTrackedAgents;
  for (auto &ata: m_watchedAgents) {
    alreadyTrackedAgents.insert(ata.first);
  }
  for (auto &c: candidatesList) {
    // We don't monitor ourselves
    if (c != m_ourAgentReference.getAgentAddress() && !alreadyTrackedAgents.count(c)) {
      // So we have a candidate we might want to monitor
      // First, check that the agent entry exists, and that ownership
      // is indeed pointing to the agent register
      Agent ag(c, m_objectStore);
      try {
        ag.fetchNoLock();
      } catch (...) {
        // The agent could simply be gone... (If not, let the complain go through).
        if (m_objectStore.exists(c)) throw;
        continue;
      }
      if (ag.getOwner() == m_agentRegister.getAddressIfSet()) {
      }
      log::ScopedParamContainer params(lc);
      params.add("agentAddress", ag.getAddressIfSet())
            .add("gcAgentAddress", m_ourAgentReference.getAgentAddress());
      lc.log(log::INFO, "In GarbageCollector::aquireTargets(): started tracking an untracked agent");
      // Agent is to be tracked, let's track it.
      double timeout=ag.getTimeout();
      // The creation of the watchdog could fail as well (if agent gets deleted in the mean time).
      try {
        m_watchedAgents[c] =
          new AgentWatchdog(c, m_objectStore);
        m_watchedAgents[c]->setTimeout(timeout);
      } catch (...) {
        if (m_objectStore.exists(c)) throw;
        m_watchedAgents.erase(c);
        continue;
      }
    }
  }
}
 
void GarbageCollector::checkHeartbeats(log::LogContext & lc) {
  // Check the heartbeats of the watched agents
  // We can still fail on many steps
  for (std::map<std::string, AgentWatchdog * >::iterator wa = m_watchedAgents.begin();
      wa != m_watchedAgents.end();) {
    // Get the heartbeat. Clean dead agents and remove references to them
    try {
      if (!wa->second->checkAlive()) {
        cleanupDeadAgent(wa->first, wa->second->getDeadAgentDetails(), lc);
        delete wa->second;
        m_watchedAgents.erase(wa++);
      } else {
        wa++;
      }
    } catch (cta::exception::Exception & ex) {
      if (wa->second->checkExists()) {
        // We really have a problem: we failed to check on an agent, that is still present.
        throw;
      } else {
        // The agent is simply gone on the wrong time. It will be trimmed from the list on the next pass.
        wa++;
      }
    }
  }
}

void GarbageCollector::cleanupDeadAgent(const std::string & address, std::list<log::Param> agentDetails, log::LogContext & lc) {
  // We detected a dead agent. Try and take ownership of it. It could already be owned
  // by another garbage collector.
  // To minimize locking, take a lock on the agent and check its ownership first.
  // We do not need to be defensive about exception here as calling function will
  // deal with them.
  Agent agent(address, m_objectStore);
  ScopedExclusiveLock agLock;
  try {
    // The agent could be gone while we try to lock it.
    agLock.lock(agent);
  } catch (Backend::NoSuchObject & ex) {
    log::ScopedParamContainer params(lc);
    params.add("agentAddress", agent.getAddressIfSet())
          .add("gcAgentAddress", m_ourAgentReference.getAgentAddress());
    lc.log(log::INFO, "In GarbageCollector::cleanupDeadAgent(): agent already deleted when trying to lock it. Skipping it.");
    return;
  }
  agent.fetch();
  log::ScopedParamContainer params(lc);
  params.add("agentAddress", agent.getAddressIfSet())
        .add("gcAgentAddress", m_ourAgentReference.getAgentAddress());
  if (agent.getOwner() != m_agentRegister.getAddressIfSet()) {
    params.add("agentOwner", agent.getOwner());
    lc.log(log::INFO, "In GarbageCollector::cleanupDeadAgent(): skipping agent which is not owned by agent register anymore.");
    // The agent will be removed from our ownership by the calling function: we're done.
    return;
  }
  // Aquire ownership of the agent.
  m_ourAgentReference.addToOwnership(address,m_objectStore);
  agent.setOwner(m_ourAgentReference.getAgentAddress());
  agent.commit();
  // Update the register
  ScopedExclusiveLock arl(m_agentRegister);
  m_agentRegister.fetch();
  m_agentRegister.trackAgent(address);
  m_agentRegister.commit();
  arl.release();
  {
    log::ScopedParamContainer params2(lc);
    for (auto p: agentDetails) params2.add(p.getName(), p.getValue());
    lc.log(log::INFO, "In GarbageCollector::cleanupDeadAgent(): will cleanup dead agent.");
  }
  // Return all objects owned by the agent to their respective backup owners
  
  OwnedObjectSorter ownedObjectSorter;
  std::list<std::shared_ptr<GenericObject>> fetchedObjects;
  ownedObjectSorter.fetchOwnedObjects(agent, fetchedObjects, m_objectStore, lc);
  ownedObjectSorter.sortFetchedObjects(agent, fetchedObjects, m_objectStore, m_catalogue, lc);
  ownedObjectSorter.lockFetchAndUpdateArchiveJobs(agent, m_ourAgentReference, m_objectStore, lc);
  ownedObjectSorter.lockFetchAndUpdateRetrieveJobs(agent, m_ourAgentReference, m_objectStore, lc);
  ownedObjectSorter.lockFetchAndUpdateOtherObjects(agent, m_ourAgentReference, m_objectStore, m_catalogue, lc);
}

void GarbageCollector::OwnedObjectSorter::fetchOwnedObjects(Agent& agent, std::list<std::shared_ptr<GenericObject> >& fetchedObjects,
    Backend & objectStore, log::LogContext & lc) {
  const auto ownedObjectAddresses = agent.getOwnershipList();
  // Parallel fetch (lock free) all the objects to assess their status (check ownership,
  // type and decide to which queue they will go.
  std::list<std::shared_ptr<GenericObject>> ownedObjects;
  std::map<GenericObject *, std::unique_ptr<GenericObject::AsyncLockfreeFetcher>> ownedObjectsFetchers;
  // This will be the list of objects we failed to garbage collect. This means the garbage collection
  // will be partial (looping?).
  std::list<std::string> skippedObjects;
  // This will be the list of objects that are simply gone. We will still need to remove the from the ownership
  // list of agent.
  std::list<std::string> goneObjects;
  // 1 launch the async fetch of all the objects.
  for (auto & obj : ownedObjectAddresses) {
    // Fetch generic objects
    ownedObjects.emplace_back(new GenericObject(obj, objectStore));
    try {
      ownedObjectsFetchers[ownedObjects.back().get()].reset(ownedObjects.back()->asyncLockfreeFetch());
    } catch (cta::exception::Exception & ex) {
      // We failed to lauch the fetch. This is unepected (absence of object will come later). We will not be able
      // to garbage collect this object.
      skippedObjects.emplace_back(obj);
      // Cleanup object reference. We will skip on this object. That means the garbage collection will
      // be left incomplete
      ownedObjectsFetchers.erase(ownedObjects.back().get());
      ownedObjects.pop_back();
      // Log the error.
      log::ScopedParamContainer params(lc);
      params.add("objectAddress", obj)
            .add("exceptionMessage", ex.getMessageValue());
      lc.log(log::ERR, "In GarbageCollector::OwnedObjectSorter::fetchOwnedObjects(): "
          "failed to asyncLockfreeFetch(): skipping object. Garbage collection will be incomplete.");
    }
  }
  
  // 2 find out the result of the fetches
  bool ownershipUdated=false;
  for (auto & obj : ownedObjects) {
    log::ScopedParamContainer params2(lc);
    params2.add("objectAddress", obj->getAddressIfSet());
    try {
      ownedObjectsFetchers.at(obj.get())->wait();
    } catch (Backend::NoSuchObject & ex) {
      goneObjects.push_back(obj->getAddressIfSet());
      lc.log(log::INFO, "In GarbageCollector::OwnedObjectSorter::fetchOwnedObjects(): skipping garbage collection of now gone object.");
      ownedObjectsFetchers.erase(obj.get());
      agent.removeFromOwnership(obj->getAddressIfSet());
      ownershipUdated=true;
      continue;
    } catch (cta::exception::Exception & ex) {
      // Again, we have a problem. We will skip the object and have an incomplete GC.
      skippedObjects.push_back(obj->getAddressIfSet());
      params2.add("exceptionMessage", ex.getMessageValue());
      lc.log(log::ERR, "In GarbageCollector::OwnedObjectSorter::fetchOwnedObjects(): "
          "failed to AsyncLockfreeFetch::wait(): skipping object. Garbage collection will be incomplete.");
      ownedObjectsFetchers.erase(obj.get());
      continue;
    }
    // This object passed the cut, we can record it for next round.
    ownedObjectsFetchers.erase(obj.get());
    fetchedObjects.emplace_back(obj);
  }
  // The generic objects we are interested in are now also stored in fetchedObjects.
  ownedObjects.clear();
  if (ownershipUdated) agent.commit();
}

void GarbageCollector::OwnedObjectSorter::sortFetchedObjects(Agent& agent, std::list<std::shared_ptr<GenericObject> >& fetchedObjects,
    Backend & objectStore, cta::catalogue::Catalogue & catalogue, log::LogContext & lc) {
  // 3 Now decide the fate of each fetched and owned object.
  bool ownershipUdated=false;
  using serializers::ArchiveJobStatus;
  std::set<ArchiveJobStatus> inactiveArchiveJobStatuses({ArchiveJobStatus::AJS_Complete, ArchiveJobStatus::AJS_Failed});
  using serializers::RetrieveJobStatus;
  std::set<RetrieveJobStatus> inactiveRetrieveJobStatuses({RetrieveJobStatus::RJS_Complete, RetrieveJobStatus::RJS_Failed});
  for (auto & obj: fetchedObjects) {
    log::ScopedParamContainer params2(lc);
    params2.add("objectAddress", obj->getAddressIfSet());
    if (obj->getOwner() != agent.getAddressIfSet()) {
      // For all object types except ArchiveRequests, this means we do
      // not need to deal with this object.
      if (obj->type() == serializers::ArchiveRequest_t) {
        ArchiveRequest ar(*obj);
        for (auto & j:ar.dumpJobs()) if (j.owner == agent.getAddressIfSet()) goto doGCObject;
      } else {
        // Log the owner (except for archiveRequests which can have several owners).
        params2.add("actualOwner", obj->getAddressIfSet());
      }
      lc.log(log::WARNING, "In GarbageCollector::OwnedObjectSorter::sortFetchedObjects(): skipping object which is not owned by this agent");
      agent.removeFromOwnership(obj->getAddressIfSet());
      ownershipUdated=true;
      continue;
    }
  doGCObject:
    switch (obj->type()) {
      case serializers::ArchiveRequest_t:
      {
        // We need to find out in which queue or queues the owned job(s)
        // Decision is simple: if the job is owned and active, it needs to be requeued
        // in its destination archive queue.
        // Get hold of an (unlocked) archive request:
        std::shared_ptr<ArchiveRequest> ar(new ArchiveRequest(*obj));
        obj.reset();
        bool jobRequeued=false;
        for (auto &j: ar->dumpJobs()) {
          if ((j.owner == agent.getAddressIfSet() || ar->getOwner() == agent.getAddressIfSet())
              && !inactiveArchiveJobStatuses.count(j.status)) {
            archiveQueuesAndRequests[j.tapePool].emplace_back(ar);
            log::ScopedParamContainer params3(lc);
            params3.add("tapepool", j.tapePool)
                   .add("copynb", j.copyNb)
                   .add("fileId", ar->getArchiveFile().archiveFileID);
            lc.log(log::INFO, "Selected archive request for requeueing to tape pool");
            jobRequeued=true;
          }
        }
        if (!jobRequeued) {
          log::ScopedParamContainer params3(lc);
          params3.add("fileId", ar->getArchiveFile().archiveFileID);
          lc.log(log::INFO, "No active archive job to requeue found. Request will remain as-is.");
        }
        break;
      }
      case serializers::RetrieveRequest_t:
      {
        // We need here to re-determine the best tape (and queue) for the retrieve request.
        std::shared_ptr<RetrieveRequest> rr(new RetrieveRequest(*obj));
        obj.reset();
        // Get the list of vids for non failed tape files.
        std::set<std::string> candidateVids;
        for (auto & j: rr->dumpJobs()) {
          if (!inactiveRetrieveJobStatuses.count(j.status)) {
            candidateVids.insert(rr->getArchiveFile().tapeFiles.at(j.copyNb).vid);
          }
        }
        if (candidateVids.empty()) {
          log::ScopedParamContainer params3(lc);
          params3.add("fileId", rr->getArchiveFile().archiveFileID);
          lc.log(log::INFO, "No active retrieve job to requeue found. Marking request for normal GC (and probably deletion).");
          otherObjects.emplace_back(new GenericObject(rr->getAddressIfSet(), objectStore));
          break;
        }
        std::string vid;
        try {
          vid=Helpers::selectBestRetrieveQueue(candidateVids, catalogue, objectStore);
        } catch (Helpers::NoTapeAvailableForRetrieve & ex) {
          log::ScopedParamContainer params3(lc);
          params3.add("fileId", rr->getArchiveFile().archiveFileID);
          lc.log(log::INFO, "No available tape found. Marking request for normal GC (and probably deletion).");
          otherObjects.emplace_back(new GenericObject(rr->getAddressIfSet(), objectStore));
          break;
        }
        retrieveQueuesAndRequests[vid].emplace_back(rr);
        log::ScopedParamContainer params3(lc);
        // Find copyNb for logging
        size_t copyNb = std::numeric_limits<size_t>::max();
        uint64_t fSeq = std::numeric_limits<uint64_t>::max();
        for (auto & tc: rr->getArchiveFile().tapeFiles) { if (tc.second.vid==vid) { copyNb=tc.first; fSeq=tc.second.fSeq; } }
        params3.add("fileId", rr->getArchiveFile().archiveFileID)
               .add("copyNb", copyNb)
               .add("vid", vid)
               .add("fSeq", fSeq);
        lc.log(log::INFO, "Selected vid to be requeued for retrieve request.");
        break;
      }
      default:
        // For other objects, we will not implement any optimization and simply call
        // their individual garbageCollect method.
        otherObjects.emplace_back(obj);
        obj.reset();
        break;
    }
    // We can now get rid of the generic object (data was transferred in a (typed) object in the sorter).
  }
  // We are now done with the next container.
  if (ownershipUdated) agent.commit();
  fetchedObjects.clear();
}

void GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateArchiveJobs(Agent& agent, AgentReference& agentReference, Backend & objectStore,
    log::LogContext & lc) {
  // We can now start updating the objects efficiently. We still need to re-fetch them locked 
  // and validate ownership.
  // 
  // 1) Get the archive requests done.
  for (auto & tapepool: archiveQueuesAndRequests) {
    // The number of objects to requeue could be very high. In order to limit the time taken by the
    // individual requeue operations, we limit the number of concurrently requeued objects to an 
    // arbitrary 500.
    while (tapepool.second.size()) {
      decltype (tapepool.second) currentJobBatch;
      while (tapepool.second.size() && currentJobBatch.size() <= 500) {
        currentJobBatch.emplace_back(std::move(tapepool.second.front()));
        tapepool.second.pop_front();
      }
      utils::Timer t;
      typedef ContainerAlgorithms<ArchiveQueue> AqAlgos;
      AqAlgos aqcl(objectStore, agentReference);
      decltype(aqcl)::InsertedElement::list jobsToAdd;
      for (auto & ar: currentJobBatch) {
        // Determine the copy number and feed the queue with it.
        for (auto &j: ar->dumpJobs()) {
          if (j.tapePool == tapepool.first) {
            jobsToAdd.push_back({ar, j.copyNb, ar->getArchiveFile(), ar->getMountPolicy()});         
          }
        }
      }
      std::set<std::string> jobsIndividuallyGCed;
      std::set<std::string> jobsNotRequeued;
      std::string queueAddress;
      try {
        aqcl.referenceAndSwitchOwnershipIfNecessary(tapepool.first, agent.getAddressIfSet(), queueAddress, jobsToAdd, lc);
      } catch (AqAlgos::OwnershipSwitchFailure & failure) {
        for (auto &failedAR: failure.failedElements) {
          try {
            std::rethrow_exception(failedAR.failure);
          } catch (cta::exception::Exception & e) {
            // Update did not go through. It could be benign
            std::string debugType=typeid(e).name();
            auto & arup=*failedAR.element;
            jobsNotRequeued.insert(arup.archiveRequest->getAddressIfSet());
            if (typeid(e) == typeid(Backend::NoSuchObject) || typeid(e) == typeid(Backend::WrongPreviousOwner)) {
              // The object was not present or not owned during update, so we skip it.
              // This is nevertheless unexpected (from previous fetch, so this is an error).
              log::ScopedParamContainer params(lc);
              params.add("archiveRequestObject", arup.archiveRequest->getAddressIfSet())
                    .add("copyNb", arup.copyNb)
                    .add("fileId", arup.archiveRequest->getArchiveFile().archiveFileID)
                    .add("exceptionType", debugType);
              lc.log(log::ERR, 
                  "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateArchiveJobs(): "
                  "failed to requeue gone/not owned archive job. Removed from queue.");
            } else {
              // We have an unexpected error. We will handle this with the request-by-request garbage collection.
              log::ScopedParamContainer params(lc);
              params.add("archiveRequestObject", arup.archiveRequest->getAddressIfSet())
                    .add("copyNb", arup.copyNb)
                    .add("fileId", arup.archiveRequest->getArchiveFile().archiveFileID)
                    .add("exceptionType", debugType)
                    .add("exceptionMessage", e.getMessageValue());
              lc.log(log::ERR, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateArchiveJobs(): "
                  "failed to requeue archive job with unexpected error. "
                  "Removing from queue and will re-run individual garbage collection.");
              // We will re-run the individual GC for this one.
              jobsIndividuallyGCed.insert(arup.archiveRequest->getAddressIfSet());
              otherObjects.emplace_back(new GenericObject(arup.archiveRequest->getAddressIfSet(), objectStore));
            }
          }
        }
      }
      // We can now log individually requeued jobs.
      for (auto & arup: jobsToAdd) {
        if (!jobsNotRequeued.count(arup.archiveRequest->getAddressIfSet())) {
            // OK, the job made it to the queue
            log::ScopedParamContainer params(lc);
            params.add("archiveRequestObject", arup.archiveRequest->getAddressIfSet())
                  .add("copyNb", arup.copyNb)
                  .add("fileId", arup.archiveRequest->getArchiveFile().archiveFileID)
                  .add("tapepool", tapepool.first)
                  .add("archiveQueueObject", queueAddress)
                  .add("garbageCollectedPreviousOwner", agent.getAddressIfSet());
            lc.log(log::INFO, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateArchiveJobs(): requeued archive job.");
        }
      }
      jobsToAdd.clear();
      // We can now forget pool level list. But before that, we can remove the objects 
      // from agent ownership if this was the last reference to it.
      // The usage of use_count() is safe here because we are in a single threaded environment.
      // In a multi threaded environment, its usage would not be appropriate.
      // See for example http://en.cppreference.com/w/cpp/memory/shared_ptr/use_count
      bool ownershipUpdated=false;
      auto agentOwnership=agent.getOwnershipSet();
      for (auto &ar: currentJobBatch) {
        if (ar.use_count() == 1 && !jobsIndividuallyGCed.count(ar->getAddressIfSet())) {
          // This tapepool is the last users of this archive request. We will remove it from ownership.
          agentOwnership.erase(ar->getAddressIfSet());
          ownershipUpdated=true;
          log::ScopedParamContainer params(lc);
          params.add("archiveRequestObject", ar->getAddressIfSet());
          lc.log(log::DEBUG, "Removed AR from agent ownership.");
        } else {
          log::ScopedParamContainer params(lc);
          params.add("archiveRequestObject", ar->getAddressIfSet())
                .add("use_count", ar.use_count())
                .add("IndividuallyGCed", jobsIndividuallyGCed.count(ar->getAddressIfSet()));
          lc.log(log::DEBUG, "Did not remove AR from agent ownership.");          
        }
      }
      if (ownershipUpdated) {
        agent.resetOwnership(agentOwnership);
        agent.commit();
      }
      currentJobBatch.clear();
      // Sleep a bit if we have oher rounds to go not to hog the queue
      if (tapepool.second.size()) sleep (5);
    }
  }
}

void GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateRetrieveJobs(Agent& agent, AgentReference& agentReference,
    Backend & objectStore, log::LogContext & lc) {  
  // 2) Get the retrieve requests done. They are simpler as retrieve requests are fully owned.
  // Then should hence not have changes since we pre-fetched them.
  for (auto & tape: retrieveQueuesAndRequests) {
    while (tape.second.size()) {
      decltype (tape.second) currentJobBatch;
      while (tape.second.size() && currentJobBatch.size() <= 500) {
        currentJobBatch.emplace_back(std::move(tape.second.front()));
        tape.second.pop_front();
      }
      double queueLockFetchTime=0;
      double queueProcessAndCommitTime=0;
      double requestsUpdatePreparationTime=0;
      double requestsUpdatingTime=0;
      double queueRecommitTime=0;
      uint64_t filesQueued=0;
      uint64_t filesDequeued=0;
      uint64_t bytesQueued=0;
      uint64_t bytesDequeued=0;
      uint64_t filesBefore=0;
      uint64_t bytesBefore=0;
      utils::Timer t;
      // Get the retrieve queue and add references to the jobs to it.
      RetrieveQueue rq(objectStore);
      ScopedExclusiveLock rql;
      Helpers::getLockedAndFetchedQueue<RetrieveQueue>(rq,rql, agentReference, tape.first, QueueType::LiveJobs, lc);
      queueLockFetchTime = t.secs(utils::Timer::resetCounter);
      auto jobsSummary=rq.getJobsSummary();
      filesBefore=jobsSummary.files;
      bytesBefore=jobsSummary.bytes;
      // Prepare the list of requests to add to the queue (if needed).
      std::list<RetrieveQueue::JobToAdd> jta;
      // We have the queue. We will loop on the requests, add them to the list. We will launch their updates 
      // after committing the queue.
      for (auto & rr: currentJobBatch) {
        // Determine the copy number and feed the queue with it.
        for (auto &tf: rr->getArchiveFile().tapeFiles) {
          if (tf.second.vid == tape.first) {
            jta.push_back({tf.second.copyNb, tf.second.fSeq, rr->getAddressIfSet(), rr->getArchiveFile().fileSize, 
                rr->getRetrieveFileQueueCriteria().mountPolicy, rr->getEntryLog().time});
          }
        }
      }
      auto addedJobs = rq.addJobsIfNecessaryAndCommit(jta, agentReference, lc);
      queueProcessAndCommitTime = t.secs(utils::Timer::resetCounter);
      // If we have an unexpected failure, we will re-run the individual garbage collection. Before that, 
      // we will NOT remove the object from agent's ownership. This variable is declared a bit ahead so
      // the goto will not cross its initialization.
      std::set<std::string> jobsIndividuallyGCed;
      if (!addedJobs.files) {
        goto agentCleanupForRetrieve;
      }
      // We will keep individual references for each job update we launch so that we make
      // our life easier downstream.
      struct RRUpdatedParams {
        std::unique_ptr<RetrieveRequest::AsyncJobOwnerUpdater> updater;
        std::shared_ptr<RetrieveRequest> retrieveRequest;
        uint16_t copyNb;
      };
      {
        std::list<RRUpdatedParams> rrUpdatersParams;
        for (auto & rr: currentJobBatch) {
          for (auto & tf: rr->getArchiveFile().tapeFiles) {
            if (tf.second.vid == tape.first) {
              rrUpdatersParams.emplace_back();
              rrUpdatersParams.back().retrieveRequest = rr;
              rrUpdatersParams.back().copyNb = tf.second.copyNb;
              rrUpdatersParams.back().updater.reset(rr->asyncUpdateJobOwner(tf.second.copyNb,
                  rq.getAddressIfSet(), agent.getAddressIfSet()));
            }
          }
        }
        requestsUpdatePreparationTime = t.secs(utils::Timer::resetCounter);
        // Now collect the results.
        std::list<std::string> requestsToDequeue;
        for (auto & rrup: rrUpdatersParams) {
          try {
            rrup.updater->wait();
            // OK, the job made it to the queue
            log::ScopedParamContainer params(lc);
            params.add("retrieveRequestObject", rrup.retrieveRequest->getAddressIfSet())
                  .add("copyNb", rrup.copyNb)
                  .add("fileId", rrup.retrieveRequest->getArchiveFile().archiveFileID)
                  .add("vid", tape.first)
                  .add("retreveQueueObject", rq.getAddressIfSet())
                  .add("garbageCollectedPreviousOwner", agent.getAddressIfSet());
            lc.log(log::INFO, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateRetrieveJobs(): requeued retrieve job.");
          } catch (cta::exception::Exception & e) {
           // Update did not go through. It could be benign
            std::string debugType=typeid(e).name();
            if (typeid(e) == typeid(Backend::NoSuchObject) ||
                typeid(e) == typeid(Backend::WrongPreviousOwner)) {
              // The object was not present or not owned during update, so we skip it.
              // This is nevertheless unexpected (from previous fetch, so this is an error).
              log::ScopedParamContainer params(lc);
              params.add("retrieveRequestObject", rrup.retrieveRequest->getAddressIfSet())
                    .add("copyNb", rrup.copyNb)
                    .add("fileId", rrup.retrieveRequest->getArchiveFile().archiveFileID)
                    .add("exceptionType", debugType);
              lc.log(log::ERR, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateRetrieveJobs(): "
                  "failed to requeue gone/not owned retrieve job. Removing from queue.");
            } else {
              // We have an unexpected error. Log it, and remove form queue. Not much we can
              // do at this point.
              log::ScopedParamContainer params(lc);
              params.add("retrieveRequestObject", rrup.retrieveRequest->getAddressIfSet())
                    .add("copyNb", rrup.copyNb)
                    .add("fileId", rrup.retrieveRequest->getArchiveFile().archiveFileID)
                    .add("exceptionType", debugType)
                    .add("exceptionMessage", e.getMessageValue());
              lc.log(log::ERR, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateRetrieveJobs(): "
                  "failed to requeue retrieve job with unexpected error. Removing from queue and will re-run individual garbage collection.");
              // We will re-run the individual GC for this one.
              jobsIndividuallyGCed.insert(rrup.retrieveRequest->getAddressIfSet());
              otherObjects.emplace_back(new GenericObject(rrup.retrieveRequest->getAddressIfSet(), objectStore));
            }
            // In all cases, the object did NOT make it to the queue.
            filesDequeued ++;
            bytesDequeued += rrup.retrieveRequest->getArchiveFile().fileSize;
            requestsToDequeue.push_back(rrup.retrieveRequest->getAddressIfSet());
          }
        }
        requestsUpdatingTime = t.secs(utils::Timer::resetCounter);
        if (requestsToDequeue.size()) {
          rq.removeJobsAndCommit(requestsToDequeue);
          log::ScopedParamContainer params(lc);
          params.add("retreveQueueObject", rq.getAddressIfSet());
          lc.log(log::INFO, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateRetrieveJobs(): "
              "Cleaned up and re-committed retrieve queue after error handling.");
          queueRecommitTime = t.secs(utils::Timer::resetCounter);
        }
      }
      {
        log::ScopedParamContainer params(lc);
        auto jobsSummary = rq.getJobsSummary();
        params.add("vid", tape.first)
              .add("retrieveQueueObject", rq.getAddressIfSet())
              .add("filesAdded", filesQueued - filesDequeued)
              .add("bytesAdded", bytesQueued - bytesDequeued)
              .add("filesAddedInitially", filesQueued)
              .add("bytesAddedInitially", bytesQueued)
              .add("filesDequeuedAfterErrors", filesDequeued)
              .add("bytesDequeuedAfterErrors", bytesDequeued)
              .add("filesBefore", filesBefore)
              .add("bytesBefore", bytesBefore)
              .add("filesAfter", jobsSummary.files)
              .add("bytesAfter", jobsSummary.bytes)
              .add("queueLockFetchTime", queueLockFetchTime)
              .add("queuePreparationTime", queueProcessAndCommitTime)
              .add("requestsUpdatePreparationTime", requestsUpdatePreparationTime)
              .add("requestsUpdatingTime", requestsUpdatingTime)
              .add("queueRecommitTime", queueRecommitTime);
        lc.log(log::INFO, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateRetrieveJobs(): "
            "Requeued a batch of retrieve requests.");
      }
      // We can now forget pool level list. But before that, we can remove the objects 
      // from agent ownership if this was the last reference to it.
      // The usage of use_count() is safe here because we are in a single threaded environment.
      // In a multi threaded environment, its usage would not be appropriate.
      // See for example http://en.cppreference.com/w/cpp/memory/shared_ptr/use_count
    agentCleanupForRetrieve:
      bool ownershipUpdated=false;
      for (auto &rr: currentJobBatch) {
        if (rr.use_count() == 1 && !jobsIndividuallyGCed.count(rr->getAddressIfSet())) {
          // This tapepool is the last users of this archive request. We will remove is from ownership.
          agent.removeFromOwnership(rr->getAddressIfSet());
          ownershipUpdated=true;
        }
      }
      if (ownershipUpdated) agent.commit();
      currentJobBatch.clear();
      // Sleep a bit if we have oher rounds to go not to hog the queue
      if (tape.second.size()) sleep (5);
    }
  }
}

void GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateOtherObjects(Agent& agent, AgentReference& agentReference,
    Backend & objectStore, cta::catalogue::Catalogue & catalogue, log::LogContext & lc) {  
  // 3) are done with the objects requiring mutualized queueing, and hence special treatement.
  // The rest will be garbage collected on a object-by-object basis.
  for (auto & go : otherObjects) { 
   // Find the object
   log::ScopedParamContainer params2(lc);
   params2.add("objectAddress", go->getAddressIfSet());
   // If the object does not exist, we're done.
   if (go->exists()) {
     ScopedExclusiveLock goLock(*go);
     go->fetch();
     // Call GenericOpbject's garbage collect method, which in turn will
     // delegate to the object type's garbage collector.
     go->garbageCollectDispatcher(goLock, agent.getAddressIfSet(), agentReference, lc, catalogue);
     lc.log(log::INFO, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateOtherObjects(): garbage collected owned object.");
   } else {
     lc.log(log::INFO, "In GarbageCollector::OwnedObjectSorter::lockFetchAndUpdateOtherObjects(): "
         "skipping garbage collection of now gone object.");
   }
   // In all cases, relinquish ownership for this object
   agent.removeFromOwnership(go->getAddressIfSet());
   agent.commit();
  }
  // We now processed all the owned objects. We can delete the agent's entry
  agent.removeAndUnregisterSelf(lc);
  lc.log(log::INFO, "In GarbageCollector::cleanupDeadAgent(): agent entry removed.");
  // We can remove the agent from our own ownership.
  agentReference.removeFromOwnership(agent.getAddressIfSet(), objectStore);
}

}}

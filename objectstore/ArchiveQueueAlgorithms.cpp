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

#include "ArchiveQueueAlgorithms.hpp"
#include "common/Timer.hpp"
#include "common/make_unique.hpp"

namespace cta { namespace objectstore {

void ContainerTraits<ArchiveQueue>::getLockedAndFetched(Container& cont, ScopedExclusiveLock& aqL, AgentReference& agRef,
    const ContainerIdentifyer& contId, log::LogContext& lc) {
  Helpers::getLockedAndFetchedQueue<Container>(cont, aqL, agRef, contId, QueueType::LiveJobs, lc);
}

void ContainerTraits<ArchiveQueue>::addReferencesAndCommit(Container& cont, InsertedElement::list& elemMemCont,
    AgentReference& agentRef, log::LogContext& lc) {
  std::list<ArchiveQueue::JobToAdd> jobsToAdd;
  for (auto & e: elemMemCont) {
    ElementDescriptor jd;
    jd.copyNb = e.copyNb;
    jd.tapePool = cont.getTapePool();
    jd.owner = cont.getAddressIfSet();
    ArchiveRequest & ar = *e.archiveRequest;
    jobsToAdd.push_back({jd, ar.getAddressIfSet(), e.archiveFile.archiveFileID, e.archiveFile.fileSize,
        e.mountPolicy, time(nullptr)});
  }
  cont.addJobsAndCommit(jobsToAdd, agentRef, lc);
}

void ContainerTraits<ArchiveQueue>::removeReferencesAndCommit(Container& cont, OpFailure<InsertedElement>::list& elementsOpFailures) {
  std::list<std::string> elementsToRemove;
  for (auto & eof: elementsOpFailures) {
    elementsToRemove.emplace_back(eof.element->archiveRequest->getAddressIfSet());
  }
  cont.removeJobsAndCommit(elementsToRemove);
}

void ContainerTraits<ArchiveQueue>::removeReferencesAndCommit(Container& cont, std::list<ElementAddress>& elementAddressList) {
  cont.removeJobsAndCommit(elementAddressList);
}

auto ContainerTraits<ArchiveQueue>::switchElementsOwnership(InsertedElement::list& elemMemCont, const ContainerAddress& contAddress, const ContainerAddress& previousOwnerAddress, log::LogContext& lc) ->  OpFailure<InsertedElement>::list {
  std::list<std::unique_ptr<ArchiveRequest::AsyncJobOwnerUpdater>> updaters;
  for (auto & e: elemMemCont) {
    ArchiveRequest & ar = *e.archiveRequest;
    auto copyNb = e.copyNb;
    updaters.emplace_back(ar.asyncUpdateJobOwner(copyNb, contAddress, previousOwnerAddress));
  }
  auto u = updaters.begin();
  auto e = elemMemCont.begin();
  OpFailure<InsertedElement>::list ret;
  while (e != elemMemCont.end()) {
    try {
      u->get()->wait();
    } catch (...) {
      ret.push_back(OpFailure<InsertedElement>());
      ret.back().element = &(*e);
      ret.back().failure = std::current_exception();
    }
    u++;
    e++;
  }
  return ret;
}


void ContainerTraits<ArchiveQueue>::getLockedAndFetchedNoCreate(Container& cont, ScopedExclusiveLock& contLock,
    const ContainerIdentifyer& cId, log::LogContext& lc) {
  // Try and get access to a queue.
  size_t attemptCount = 0;
  retry:
  objectstore::RootEntry re(cont.m_objectStore);
  re.fetchNoLock();
  std::string aqAddress;
  auto aql = re.dumpArchiveQueues(QueueType::LiveJobs);
  for (auto & aqp : aql) {
    if (aqp.tapePool == cId)
      aqAddress = aqp.address;
  }
  if (!aqAddress.size()) throw NoSuchContainer("In ContainerTraits<ArchiveQueue>::getLockedAndFetchedNoCreate(): no such archive queue");
  // try and lock the archive queue. Any failure from here on means the end of the getting jobs.
  objectstore::ArchiveQueue aq(aqAddress, cont.m_objectStore);
  objectstore::ScopedExclusiveLock aqlock;
  //findQueueTime += localFindQueueTime = t.secs(utils::Timer::resetCounter);
  try {
    aqlock.lock(aq);
    aq.fetch();
    //lockFetchQueueTime += localLockFetchQueueTime = t.secs(utils::Timer::resetCounter);
  } catch (cta::exception::Exception & ex) {
    // The queue is now absent. We can remove its reference in the root entry.
    // A new queue could have been added in the mean time, and be non-empty.
    // We will then fail to remove from the RootEntry (non-fatal).
    ScopedExclusiveLock rexl(re);
    re.fetch();
    try {
      re.removeArchiveQueueAndCommit(cId, QueueType::LiveJobs, lc);
      log::ScopedParamContainer params(lc);
      params.add("tapepool", cId)
            .add("queueObject", aq.getAddressIfSet());
      lc.log(log::INFO, "In ArchiveMount::getNextJobBatch(): de-referenced missing queue from root entry");
    } catch (RootEntry::ArchiveQueueNotEmpty & ex) {
      // TODO: improve: if we fail here we could retry to fetch a job.
      log::ScopedParamContainer params(lc);
      params.add("tapepool", cId)
            .add("queueObject", aq.getAddressIfSet())
            .add("Message", ex.getMessageValue());
      lc.log(log::INFO, "In ArchiveMount::getNextJobBatch(): could not de-referenced missing queue from root entry");
    } catch (RootEntry::NoSuchArchiveQueue & ex) {
      // Somebody removed the queue in the mean time. Barely worth mentioning.
      log::ScopedParamContainer params(lc);
      params.add("tapepool", cId)
            .add("queueObject", aq.getAddressIfSet());
      lc.log(log::DEBUG, "In ArchiveMount::getNextJobBatch(): could not de-referenced missing queue from root entry: already done.");
    }
    //emptyQueueCleanupTime += localEmptyCleanupQueueTime = t.secs(utils::Timer::resetCounter);
    attemptCount++;
    goto retry;
  }
}

auto ContainerTraits<ArchiveQueue>::getPoppingElementsCandidates(Container& cont, PopCriteria& unfulfilledCriteria,
    ElementsToSkipSet& elemtsToSkip, log::LogContext& lc) -> PoppedElementsBatch {
  PoppedElementsBatch ret;
  auto candidateJobsFromQueue=cont.getCandidateList(unfulfilledCriteria.bytes, unfulfilledCriteria.files, elemtsToSkip);
  for (auto &cjfq: candidateJobsFromQueue.candidates) {
    ret.elements.emplace_back(PoppedElement{cta::make_unique<ArchiveRequest>(cjfq.address, cont.m_objectStore), cjfq.copyNb, cjfq.size});
  }
  return ret;
    }

auto ContainerTraits<ArchiveQueue>::getElementSummary(const PoppedElement& poppedElement) -> PoppedElementsSummary {
  PoppedElementsSummary ret;
  ret.bytes = poppedElement.bytes;
  ret.files = 1;
  return ret;
}

void ContainerTraits<ArchiveQueue>::PoppedElementsList::insertBack(PoppedElementsList&& insertedList) {
  for (auto &e: insertedList) {
    std::list<PoppedElement>::emplace_back(std::move(e));
  }
}

void ContainerTraits<ArchiveQueue>::PoppedElementsList::insertBack(PoppedElement&& e) {
  std::list<PoppedElement>::emplace_back(std::move(e));
}

auto ContainerTraits<ArchiveQueue>::PopCriteria::operator-=(const PoppedElementsSummary& pes) -> PopCriteria & {
  bytes -= pes.bytes;
  files -= pes.files;
  return *this;
}




}} // namespace cta::objectstore

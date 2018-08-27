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

#include "ArchiveRequest.hpp"
#include "GenericObject.hpp"
#include "ArchiveQueue.hpp"
#include "Helpers.hpp"
#include "common/dataStructures/EntryLog.hpp"
#include "MountPolicySerDeser.hpp"

#include <algorithm>
#include <google/protobuf/util/json_util.h>
#include <cmath>

namespace cta { namespace objectstore {

cta::objectstore::ArchiveRequest::ArchiveRequest(const std::string& address, Backend& os): 
  ObjectOps<serializers::ArchiveRequest, serializers::ArchiveRequest_t>(os, address){ }

cta::objectstore::ArchiveRequest::ArchiveRequest(Backend& os): 
  ObjectOps<serializers::ArchiveRequest, serializers::ArchiveRequest_t>(os) { }

cta::objectstore::ArchiveRequest::ArchiveRequest(GenericObject& go):
  ObjectOps<serializers::ArchiveRequest, serializers::ArchiveRequest_t>(go.objectStore()) {
  // Here we transplant the generic object into the new object
  go.transplantHeader(*this);
  // And interpret the header.
  getPayloadFromHeader();
}

void cta::objectstore::ArchiveRequest::initialize() {
  // Setup underlying object
  ObjectOps<serializers::ArchiveRequest, serializers::ArchiveRequest_t>::initialize();
  // Setup the fields in the payload
  m_payload.set_reportdecided(false);
  // This object is good to go (to storage)
  m_payloadInterpreted = true;
}

void cta::objectstore::ArchiveRequest::addJob(uint16_t copyNumber,
  const std::string& tapepool, const std::string& initialOwner, 
    uint16_t maxRetiesWithinMount, uint16_t maxTotalRetries, uint16_t maxReportRetries) {
  checkPayloadWritable();
  auto *j = m_payload.add_jobs();
  j->set_copynb(copyNumber);
  j->set_status(serializers::ArchiveJobStatus::AJS_ToTransfer);
  j->set_tapepool(tapepool);
  j->set_owner(initialOwner);
  j->set_archivequeueaddress("");
  j->set_totalretries(0);
  j->set_retrieswithinmount(0);
  j->set_lastmountwithfailure(0);
  j->set_maxretrieswithinmount(maxRetiesWithinMount);
  j->set_maxtotalretries(maxTotalRetries);
  j->set_totalreportretries(0);
  j->set_maxreportretries(maxReportRetries);
}

QueueType ArchiveRequest::getJobQueueType(uint16_t copyNumber) {
  checkPayloadReadable();
  for (auto &j: m_payload.jobs()) {
    if (j.copynb() == copyNumber) {
      switch (j.status()) {
      case serializers::ArchiveJobStatus::AJS_ToTransfer:
        return QueueType::JobsToTransfer;
      case serializers::ArchiveJobStatus::AJS_Complete:
        throw JobNotQueueable("In ArchiveRequest::getJobQueueType(): Complete jobs are not queueable. They are finished and pend siblings completion.");
      case serializers::ArchiveJobStatus::AJS_ToReportForTransfer:
        // We should report a success...
        return QueueType::JobsToReport;
      case serializers::ArchiveJobStatus::AJS_ToReportForFailure:
        // We should report a failure. The report queue can be shared.
        return QueueType::JobsToReport;
      case serializers::ArchiveJobStatus::AJS_Failed:
        return QueueType::FailedJobs;
      case serializers::ArchiveJobStatus::AJS_Abandoned:
        throw JobNotQueueable("In ArchiveRequest::getJobQueueType(): Abandoned jobs are not queueable. They are finished and pend siblings completion.");
      }
    }
  }
  throw exception::Exception("In ArchiveRequest::getJobQueueType(): Copy number not found.");
}


auto cta::objectstore::ArchiveRequest::addTransferFailure(uint16_t copyNumber,
    uint64_t mountId, const std::string & failureReason, log::LogContext & lc) -> EnqueueingNextStep {
  checkPayloadWritable();
  // Find the job and update the number of failures
  for (size_t i=0; i<(size_t)m_payload.jobs_size(); i++) {
    auto &j=*m_payload.mutable_jobs(i);
    if (j.copynb() == copyNumber) {
      if (j.lastmountwithfailure() == mountId) {
        j.set_retrieswithinmount(j.retrieswithinmount() + 1);
      } else {
        j.set_retrieswithinmount(1);
        j.set_lastmountwithfailure(mountId);
      }
      j.set_totalretries(j.totalretries() + 1);
      * j.mutable_failurelogs()->Add() = failureReason;
    }
    if (j.totalretries() >= j.maxtotalretries()) {
      // We have to determine if this was the last copy to fail/succeed.
      return determineNextStep(copyNumber, JobEvent::TransferFailed, lc);
    } else {
      EnqueueingNextStep ret;
      ret.nextStatus = serializers::ArchiveJobStatus::AJS_ToTransfer;
      // Decide if we want the job to have a chance to come back to this mount (requeue) or not. In the latter
      // case, the job will remain owned by this session and get garbage collected.
      if (j.retrieswithinmount() >= j.maxretrieswithinmount())
        ret.nextStep = EnqueueingNextStep::NextStep::Nothing;
      else
        ret.nextStep = EnqueueingNextStep::NextStep::EnqueueForTransfer;
      return ret;
    }
  }
  throw NoSuchJob ("In ArchiveRequest::addJobFailure(): could not find job");
}

auto ArchiveRequest::addReportFailure(uint16_t copyNumber, uint64_t sessionId, const std::string& failureReason,
    log::LogContext& lc) -> EnqueueingNextStep {
  checkPayloadWritable();
  // Find the job and update the number of failures
  for (size_t i=0; i<(size_t)m_payload.jobs_size(); i++) {
    auto &j=*m_payload.mutable_jobs(i);
    if (j.copynb() == copyNumber) {
      j.set_totalreportretries(j.totalreportretries() + 1);
      * j.mutable_reportfailurelogs()->Add() = failureReason;
    }
    EnqueueingNextStep ret;
    if (j.totalreportretries() >= j.maxreportretries()) {
      // Status is now failed
      ret.nextStatus = serializers::ArchiveJobStatus::AJS_Failed;
      ret.nextStep = EnqueueingNextStep::NextStep::StoreInFailedJobsContainer;
    } else {
      // Status is unchanged
      ret.nextStatus = j.status();
      ret.nextStep = EnqueueingNextStep::NextStep::EnqueueForReport;
    }
    return ret;
  }
  throw NoSuchJob ("In ArchiveRequest::addJobFailure(): could not find job");
}

ArchiveRequest::RetryStatus ArchiveRequest::getRetryStatus(const uint16_t copyNumber) {
  checkPayloadReadable();
  for (auto &j: m_payload.jobs()) {
    if (copyNumber == j.copynb()) {
      RetryStatus ret;
      ret.retriesWithinMount = j.retrieswithinmount();
      ret.maxRetriesWithinMount = j.maxretrieswithinmount();
      ret.totalRetries = j.totalretries();
      ret.maxTotalRetries = j.maxtotalretries();
      return ret;
    }
  }
  throw cta::exception::Exception("In ArchiveRequest::getRetryStatus(): job not found()");
}

void ArchiveRequest::setArchiveFile(const cta::common::dataStructures::ArchiveFile& archiveFile) {
  checkPayloadWritable();
  // TODO: factor out the archivefile structure from the flat ArchiveRequest.
  m_payload.set_archivefileid(archiveFile.archiveFileID);
  m_payload.set_checksumtype(archiveFile.checksumType);
  m_payload.set_checksumvalue(archiveFile.checksumValue);
  m_payload.set_creationtime(archiveFile.creationTime);
  m_payload.set_diskfileid(archiveFile.diskFileId);
  m_payload.mutable_diskfileinfo()->set_group(archiveFile.diskFileInfo.group);
  m_payload.mutable_diskfileinfo()->set_owner(archiveFile.diskFileInfo.owner);
  m_payload.mutable_diskfileinfo()->set_path(archiveFile.diskFileInfo.path);
  m_payload.mutable_diskfileinfo()->set_recoveryblob(archiveFile.diskFileInfo.recoveryBlob);
  m_payload.set_diskinstance(archiveFile.diskInstance);
  m_payload.set_filesize(archiveFile.fileSize);
  m_payload.set_reconcilationtime(archiveFile.reconciliationTime);
  m_payload.set_storageclass(archiveFile.storageClass);
}

cta::common::dataStructures::ArchiveFile ArchiveRequest::getArchiveFile() {
  checkPayloadReadable();
  cta::common::dataStructures::ArchiveFile ret;
  ret.archiveFileID = m_payload.archivefileid();
  ret.checksumType = m_payload.checksumtype();
  ret.checksumValue = m_payload.checksumvalue();
  ret.creationTime = m_payload.creationtime();
  ret.diskFileId = m_payload.diskfileid();
  ret.diskFileInfo.group = m_payload.diskfileinfo().group();
  ret.diskFileInfo.owner = m_payload.diskfileinfo().owner();
  ret.diskFileInfo.path = m_payload.diskfileinfo().path();
  ret.diskFileInfo.recoveryBlob = m_payload.diskfileinfo().recoveryblob();
  ret.diskInstance = m_payload.diskinstance();
  ret.fileSize = m_payload.filesize();
  ret.reconciliationTime = m_payload.reconcilationtime();
  ret.storageClass = m_payload.storageclass();
  return ret;
}

//------------------------------------------------------------------------------
// setArchiveReportURL
//------------------------------------------------------------------------------
void ArchiveRequest::setArchiveReportURL(const std::string& URL) {
  checkPayloadWritable();
  m_payload.set_archivereporturl(URL);
}

//------------------------------------------------------------------------------
// getArviveReportURL
//------------------------------------------------------------------------------
std::string ArchiveRequest::getArchiveReportURL() {
  checkPayloadReadable();
  return m_payload.archivereporturl();
}

//------------------------------------------------------------------------------
// setArchiveErrorReportURL
//------------------------------------------------------------------------------
void ArchiveRequest::setArchiveErrorReportURL(const std::string& URL) {
  checkPayloadWritable();
  m_payload.set_archiveerrorreporturl(URL);
}

//------------------------------------------------------------------------------
// getArchiveErrorReportURL
//------------------------------------------------------------------------------
std::string ArchiveRequest::getArchiveErrorReportURL() {
  checkPayloadReadable();
  return m_payload.archiveerrorreporturl();
}

//------------------------------------------------------------------------------
// setMountPolicy
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setMountPolicy(const cta::common::dataStructures::MountPolicy &mountPolicy) {
  checkPayloadWritable();
  MountPolicySerDeser(mountPolicy).serialize(*m_payload.mutable_mountpolicy());
}

//------------------------------------------------------------------------------
// getMountPolicy
//------------------------------------------------------------------------------
cta::common::dataStructures::MountPolicy cta::objectstore::ArchiveRequest::getMountPolicy() {
  checkPayloadReadable();
  MountPolicySerDeser mp;
  mp.deserialize(m_payload.mountpolicy());
  return mp;
}

//------------------------------------------------------------------------------
// setRequester
//------------------------------------------------------------------------------
void ArchiveRequest::setRequester(const cta::common::dataStructures::UserIdentity &requester) {
  checkPayloadWritable();
  auto payloadRequester = m_payload.mutable_requester();
  payloadRequester->set_name(requester.name);
  payloadRequester->set_group(requester.group);
}

//------------------------------------------------------------------------------
// getRequester
//------------------------------------------------------------------------------
cta::common::dataStructures::UserIdentity ArchiveRequest::getRequester() {
  checkPayloadReadable();
  cta::common::dataStructures::UserIdentity requester;
  auto payloadRequester = m_payload.requester();
  requester.name=payloadRequester.name();
  requester.group=payloadRequester.group();
  return requester;
}

//------------------------------------------------------------------------------
// setSrcURL
//------------------------------------------------------------------------------
void ArchiveRequest::setSrcURL(const std::string &srcURL) {
  checkPayloadWritable();
  m_payload.set_srcurl(srcURL);
}

//------------------------------------------------------------------------------
// getSrcURL
//------------------------------------------------------------------------------
std::string ArchiveRequest::getSrcURL() {
  checkPayloadReadable();
  return m_payload.srcurl();
}

//------------------------------------------------------------------------------
// setCreationLog
//------------------------------------------------------------------------------
void ArchiveRequest::setEntryLog(const cta::common::dataStructures::EntryLog &creationLog) {
  checkPayloadWritable();
  auto payloadCreationLog = m_payload.mutable_creationlog();
  payloadCreationLog->set_time(creationLog.time);
  payloadCreationLog->set_host(creationLog.host);
  payloadCreationLog->set_username(creationLog.username);
}

//------------------------------------------------------------------------------
// getCreationLog
//------------------------------------------------------------------------------
cta::common::dataStructures::EntryLog ArchiveRequest::getEntryLog() {
  checkPayloadReadable();
  EntryLogSerDeser el;
  el.deserialize(m_payload.creationlog());
  return el;
}

auto ArchiveRequest::dumpJobs() -> std::list<ArchiveRequest::JobDump> {
  checkPayloadReadable();
  std::list<JobDump> ret;
  auto & jl = m_payload.jobs();
  for (auto j=jl.begin(); j!=jl.end(); j++) {
    ret.push_back(JobDump());
    ret.back().copyNb = j->copynb();
    ret.back().tapePool = j->tapepool();
    ret.back().owner = j->owner();
    ret.back().status = j->status();
  }
  return ret;
}

//------------------------------------------------------------------------------
// garbageCollect
//------------------------------------------------------------------------------
void ArchiveRequest::garbageCollect(const std::string &presumedOwner, AgentReference & agentReference, log::LogContext & lc,
    cta::catalogue::Catalogue & catalogue) {
  checkPayloadWritable();
  // We need to find which job(s) we should actually work on. The job(s) will be
  // requeued to the relevant queues depending on their statuses.
  auto * jl = m_payload.mutable_jobs();
  bool anythingGarbageCollected=false;
  using serializers::ArchiveJobStatus;
  std::set<ArchiveJobStatus> statusesImplyingQueueing ({ArchiveJobStatus::AJS_ToTransfer, ArchiveJobStatus::AJS_ToReportForTransfer,
      ArchiveJobStatus::AJS_ToReportForFailure, ArchiveJobStatus::AJS_Failed});
  for (auto j=jl->begin(); j!=jl->end(); j++) {
    auto owner=j->owner();
    auto status=j->status();
    if ( statusesImplyingQueueing.count(status) && owner==presumedOwner) {
      // The job is in a state which implies queuing.
      std::string queueObject="Not defined yet";
      anythingGarbageCollected=true;
      try {
        utils::Timer t;
        // Get the queue where we should requeue the job. The queue might need to be
        // recreated (this will be done by helper).
        ArchiveQueue aq(m_objectStore);
        ScopedExclusiveLock aql;
        Helpers::getLockedAndFetchedQueue<ArchiveQueue>(aq, aql, agentReference, j->tapepool(), getQueueType(status), lc);
        queueObject=aq.getAddressIfSet();
        ArchiveRequest::JobDump jd;
        jd.copyNb = j->copynb();
        jd.tapePool = j->tapepool();
        jd.owner = j->owner();
        jd.status = j->status();
        std::list<ArchiveQueue::JobToAdd> jta;
        jta.push_back({jd, getAddressIfSet(), getArchiveFile().archiveFileID,
          getArchiveFile().fileSize, getMountPolicy(), getEntryLog().time});
        aq.addJobsIfNecessaryAndCommit(jta, agentReference, lc);
        auto queueUpdateTime = t.secs(utils::Timer::resetCounter);
        j->set_owner(aq.getAddressIfSet());
        commit();
        aql.release();
        auto commitUnlockQueueTime = t.secs(utils::Timer::resetCounter);
        {
          log::ScopedParamContainer params(lc);
          params.add("jobObject", getAddressIfSet())
                .add("queueObject", queueObject)
                .add("presumedOwner", presumedOwner)
                .add("copyNb", j->copynb())
                .add("queueUpdateTime", queueUpdateTime)
                .add("commitUnlockQueueTime", commitUnlockQueueTime);
          lc.log(log::INFO, "In ArchiveRequest::garbageCollect(): requeued job.");
        }
        timespec ts;
        // We will sleep a bit to make sure other processes can also access the queue
        // as we are very likely to be part of a tight loop.
        // TODO: ideally, end of session requeueing and garbage collection should be
        // done in parallel.
        // We sleep half the time it took to queue to give a chance to other lockers.
        double secSleep, fracSecSleep;
        fracSecSleep = std::modf(queueUpdateTime / 2, &secSleep);
        ts.tv_sec = secSleep;
        ts.tv_nsec = std::round(fracSecSleep * 1000 * 1000 * 1000);
        nanosleep(&ts, nullptr);
        auto sleepTime = t.secs();
        log::ScopedParamContainer params(lc);
        params.add("jobObject", getAddressIfSet())
              .add("queueObject", queueObject)
              .add("presumedOwner", presumedOwner)
              .add("copyNb", j->copynb())
              .add("queueUpdateTime", queueUpdateTime)
              .add("commitUnlockQueueTime", commitUnlockQueueTime)
              .add("sleepTime", sleepTime);
        lc.log(log::INFO, "In ArchiveRequest::garbageCollect(): slept some time to not sit on the queue after GC requeueing.");
      } catch (...) {
        // We could not requeue the job: fail it.
        j->set_status(serializers::AJS_Failed);
        log::ScopedParamContainer params(lc);
        params.add("jobObject", getAddressIfSet())
              .add("queueObject", queueObject)
              .add("presumedOwner", presumedOwner)
              .add("copyNb", j->copynb());
        // Log differently depending on the exception type.
        std::string backtrace = "";
        try {
          std::rethrow_exception(std::current_exception());
        } catch (cta::exception::Exception &ex) {
          params.add("exceptionMessage", ex.getMessageValue());
          backtrace = ex.backtrace();
        } catch (std::exception & ex) {
          params.add("exceptionWhat", ex.what());
        } catch (...) {
          params.add("exceptionType", "unknown");
        }        
          commit();
        lc.log(log::ERR, "In ArchiveRequest::garbageCollect(): failed to requeue the job and failed it. Internal error: the job is now orphaned.");
        }
      }
    }
  if (!anythingGarbageCollected) {
    log::ScopedParamContainer params(lc);
    params.add("jobObject", getAddressIfSet())
          .add("presumedOwner", presumedOwner);
    lc.log(log::INFO, "In ArchiveRequest::garbageCollect(): nothing to garbage collect.");
  }
}

void ArchiveRequest::setJobOwner(
  uint16_t copyNumber, const std::string& owner) {
  checkPayloadWritable();
  // Find the right job
  auto mutJobs = m_payload.mutable_jobs();
  for (auto job=mutJobs->begin(); job!=mutJobs->end(); job++) {
    if (job->copynb() == copyNumber) {
      job->set_owner(owner);
      return;
    }
  }
  throw NoSuchJob("In ArchiveRequest::setJobOwner: no such job");
}

ArchiveRequest::AsyncJobOwnerUpdater* ArchiveRequest::asyncUpdateJobOwner(uint16_t copyNumber,
  const std::string& owner, const std::string& previousOwner, const cta::optional<serializers::ArchiveJobStatus>& newStatus) {
  std::unique_ptr<AsyncJobOwnerUpdater> ret(new AsyncJobOwnerUpdater);
  // Passing a reference to the unique pointer led to strange behaviors.
  auto & retRef = *ret;
  ret->m_updaterCallback=
      [this, copyNumber, owner, previousOwner, &retRef, newStatus](const std::string &in)->std::string {
        // We have a locked and fetched object, so we just need to work on its representation.
        retRef.m_timingReport.lockFetchTime = retRef.m_timer.secs(utils::Timer::resetCounter);
        serializers::ObjectHeader oh;
        if (!oh.ParseFromString(in)) {
          // Use a the tolerant parser to assess the situation.
          oh.ParsePartialFromString(in);
          throw cta::exception::Exception(std::string("In ArchiveRequest::asyncUpdateJobOwner(): could not parse header: ")+
            oh.InitializationErrorString());
        }
        if (oh.type() != serializers::ObjectType::ArchiveRequest_t) {
          std::stringstream err;
          err << "In ArchiveRequest::asyncUpdateJobOwner()::lambda(): wrong object type: " << oh.type();
          throw cta::exception::Exception(err.str());
        }
        serializers::ArchiveRequest payload;
        if (!payload.ParseFromString(oh.payload())) {
          // Use a the tolerant parser to assess the situation.
          payload.ParsePartialFromString(oh.payload());
          throw cta::exception::Exception(std::string("In ArchiveRequest::asyncUpdateJobOwner(): could not parse payload: ")+
            payload.InitializationErrorString());
        }
        // Find the copy number and change the owner.
        auto *jl=payload.mutable_jobs();
        for (auto j=jl->begin(); j!=jl->end(); j++) {
          if (j->copynb() == copyNumber) {
            // The owner might already be the right one (in garbage collection cases), in which case, it's job done.
            if (j->owner() != owner) {
              if (j->owner() != previousOwner) {
                throw Backend::WrongPreviousOwner("In ArchiveRequest::asyncUpdateJobOwner()::lambda(): Job not owned.");
              }
              j->set_owner(owner);
            }
            // If a status change was requested, do it.
            if (newStatus) j->set_status(*newStatus);
            // We also need to gather all the job content for the user to get in-memory
            // representation.
            // TODO this is an unfortunate duplication of the getXXX() members of ArchiveRequest.
            // We could try and refactor this.
            retRef.m_archiveFile.archiveFileID = payload.archivefileid();
            retRef.m_archiveFile.checksumType = payload.checksumtype();
            retRef.m_archiveFile.checksumValue = payload.checksumvalue();
            retRef.m_archiveFile.creationTime = payload.creationtime();
            retRef.m_archiveFile.diskFileId = payload.diskfileid();
            retRef.m_archiveFile.diskFileInfo.group = payload.diskfileinfo().group();
            retRef.m_archiveFile.diskFileInfo.owner = payload.diskfileinfo().owner();
            retRef.m_archiveFile.diskFileInfo.path = payload.diskfileinfo().path();
            retRef.m_archiveFile.diskFileInfo.recoveryBlob = payload.diskfileinfo().recoveryblob();
            retRef.m_archiveFile.diskInstance = payload.diskinstance();
            retRef.m_archiveFile.fileSize = payload.filesize();
            retRef.m_archiveFile.reconciliationTime = payload.reconcilationtime();
            retRef.m_archiveFile.storageClass = payload.storageclass();
            retRef.m_archiveReportURL = payload.archivereporturl();
            retRef.m_archiveErrorReportURL = payload.archiveerrorreporturl();
            retRef.m_srcURL = payload.srcurl();
            if (j->failurelogs_size()) {
              retRef.m_latestError = j->failurelogs(j->failurelogs_size()-1);
            }
            retRef.m_jobStatus = j->status();
            oh.set_payload(payload.SerializePartialAsString());
            retRef.m_timingReport.processTime = retRef.m_timer.secs(utils::Timer::resetCounter);
            return oh.SerializeAsString();
          }
        }
        // If we do not find the copy, return not owned as well...
        throw Backend::WrongPreviousOwner("In ArchiveRequest::asyncUpdateJobOwner()::lambda(): copyNb not found.");
      };
  ret->m_backendUpdater.reset(m_objectStore.asyncUpdate(getAddressIfSet(), ret->m_updaterCallback));
  return ret.release();
}

void ArchiveRequest::AsyncJobOwnerUpdater::wait() {
  m_backendUpdater->wait();
  m_timingReport.commitUnlockTime = m_timer.secs();
}

ArchiveRequest::AsyncJobOwnerUpdater::TimingsReport ArchiveRequest::AsyncJobOwnerUpdater::getTimeingsReport() {
  return m_timingReport;
}


const common::dataStructures::ArchiveFile& ArchiveRequest::AsyncJobOwnerUpdater::getArchiveFile() {
  return m_archiveFile;
}

const std::string& ArchiveRequest::AsyncJobOwnerUpdater::getArchiveReportURL() {
  return m_archiveReportURL;
}

const std::string& ArchiveRequest::AsyncJobOwnerUpdater::getArchiveErrorReportURL() {
  return m_archiveErrorReportURL;
}

const std::string& ArchiveRequest::AsyncJobOwnerUpdater::getSrcURL() {
  return m_srcURL;
}

objectstore::serializers::ArchiveJobStatus ArchiveRequest::AsyncJobOwnerUpdater::getJobStatus() {
  return m_jobStatus;
}

ArchiveRequest::AsyncTransferSuccessfulUpdater * ArchiveRequest::asyncUpdateTransferSuccessful(const uint16_t copyNumber ) {
  std::unique_ptr<AsyncTransferSuccessfulUpdater> ret(new AsyncTransferSuccessfulUpdater);  
  // Passing a reference to the unique pointer led to strange behaviors.
  auto & retRef = *ret;
  ret->m_updaterCallback=
    [this,copyNumber, &retRef](const std::string &in)->std::string { 
      // We have a locked and fetched object, so we just need to work on its representation.
      serializers::ObjectHeader oh;
      oh.ParseFromString(in);
      if (oh.type() != serializers::ObjectType::ArchiveRequest_t) {
        std::stringstream err;
        err << "In ArchiveRequest::asyncUpdateJobSuccessful()::lambda(): wrong object type: " << oh.type();
        throw cta::exception::Exception(err.str());
      }
      serializers::ArchiveRequest payload;
      payload.ParseFromString(oh.payload());
      auto * jl = payload.mutable_jobs();
      bool otherJobsToTransfer = false;
      for (auto j=jl->begin(); j!=jl->end(); j++) {
        if (j->copynb() != copyNumber && j->status() == serializers::ArchiveJobStatus::AJS_ToTransfer)
          otherJobsToTransfer = true;
      }
      for (auto j=jl->begin(); j!=jl->end(); j++) {
        if (j->copynb() == copyNumber) {
          if (otherJobsToTransfer || m_payload.reportdecided()) {
            // There are still other jobs to report or another failed and will
            // report. No next step for this job.
            j->set_status(serializers::ArchiveJobStatus::AJS_Complete);
            j->set_owner("");
            retRef.m_doReportTransferSuccess = false;
          } else {
            // We will report success with this job as it is the last to transfer and no report (for failure)
            // happened.
            j->set_status(serializers::ArchiveJobStatus::AJS_ToReportForTransfer);
            retRef.m_doReportTransferSuccess = true;
          }
          oh.set_payload(payload.SerializePartialAsString());
          return oh.SerializeAsString();
        }
      }
      std::stringstream err;
      err << "In ArchiveRequest::asyncUpdateJobSuccessful()::lambda(): copyNb not found";
      throw cta::exception::Exception(err.str());
    };
  ret->m_backendUpdater.reset(m_objectStore.asyncUpdate(getAddressIfSet(), ret->m_updaterCallback));
  return ret.release();
}

void ArchiveRequest::AsyncTransferSuccessfulUpdater::wait() {
  m_backendUpdater->wait();
}

ArchiveRequest::AsyncRequestDeleter* ArchiveRequest::asyncDeleteRequest() {
  std::unique_ptr<AsyncRequestDeleter> ret(new AsyncRequestDeleter);
  ret->m_backendDeleter.reset(m_objectStore.asyncDelete(getAddressIfSet()));
  return ret.release();
}

void ArchiveRequest::AsyncRequestDeleter::wait() {
  m_backendDeleter->wait();
}

std::string ArchiveRequest::getJobOwner(uint16_t copyNumber) {
  checkPayloadReadable();
  auto jl = m_payload.jobs();
  auto j=std::find_if(jl.begin(), jl.end(), [&](decltype(*jl.begin())& j2){ return j2.copynb() == copyNumber; });
  if (jl.end() == j)
    throw NoSuchJob("In ArchiveRequest::getJobOwner: no such job");
  return j->owner();
}

QueueType ArchiveRequest::getQueueType(const serializers::ArchiveJobStatus& status) {
  using serializers::ArchiveJobStatus;
  switch(status) {
  case ArchiveJobStatus::AJS_ToTransfer:
    return QueueType::JobsToTransfer;
  case ArchiveJobStatus::AJS_ToReportForTransfer:
  case ArchiveJobStatus::AJS_ToReportForFailure:
    return QueueType::JobsToReport;
  case ArchiveJobStatus::AJS_Failed:
    return QueueType::FailedJobs;
  default:
    throw cta::exception::Exception("In ArchiveRequest::getQueueType(): invalid status for queueing.");
  }
}

std::string ArchiveRequest::statusToString(const serializers::ArchiveJobStatus& status) {
  switch(status) {
  case serializers::ArchiveJobStatus::AJS_ToTransfer:
    return "ToTransfer";
  case serializers::ArchiveJobStatus::AJS_ToReportForTransfer:
    return "ToReportForTransfer";
  case serializers::ArchiveJobStatus::AJS_ToReportForFailure:
    return "ToReportForFailure";
  case serializers::ArchiveJobStatus::AJS_Complete:
    return "Complete";
  case serializers::ArchiveJobStatus::AJS_Failed:
    return "Failed";
  case serializers::ArchiveJobStatus::AJS_Abandoned:
    return "Abandoned";
  default:
    return std::string("Unknown (")+std::to_string((uint64_t) status)+")";
  }
}

std::string ArchiveRequest::eventToString(JobEvent jobEvent) {
  switch(jobEvent) {
  case JobEvent::ReportFailed:
    return "ReportFailed";
  case JobEvent::TransferFailed:
    return "EventFailed";
  default:
    return std::string("Unknown (")+std::to_string((uint64_t) jobEvent)+")";
  }
}


auto ArchiveRequest::determineNextStep(uint16_t copyNumberUpdated, JobEvent jobEvent, 
    log::LogContext& lc) -> EnqueueingNextStep {
  checkPayloadWritable();
  // We have to determine which next step should be taken.
  // - When transfer succeeds or fails:
  //   - If the job got transferred and is not the last (other(s) remain to transfer), it becomes complete.
  //   - If the job failed and is not the last, we will queue it as "failed" in the failed jobs queue.
  //   - If the job is the last and all jobs succeeded, this jobs becomes ToReportForTransfer
  //   - If the job is the last and any (including this one) failed, this job becomes ToReportForFailure.
  
  // - When report completes of fails:
  //   - If the report was for a failure, the job is 
  auto & jl=m_payload.jobs();
  using serializers::ArchiveJobStatus;
  // Validate the current status.
  // Get status
  cta::optional<ArchiveJobStatus> currentStatus;
  for (auto &j:jl) { if (j.copynb() == copyNumberUpdated) currentStatus = j.status(); }
  if (!currentStatus) {
    std::stringstream err;
    err << "In ArchiveRequest::updateJobStatus(): copynb not found : " << copyNumberUpdated
        << "exiing ones: ";
    for (auto &j: jl) err << j.copynb() << "  ";
    throw cta::exception::Exception(err.str());
  }
  // Check status compatibility with event.
  switch (jobEvent) {
  case JobEvent::TransferFailed:
    if (*currentStatus != ArchiveJobStatus::AJS_ToTransfer) {
      // Wrong status, but the context leaves no ambiguity. Just warn.
      log::ScopedParamContainer params(lc);
      params.add("event", eventToString(jobEvent))
            .add("status", statusToString(*currentStatus))
            .add("fileId", m_payload.archivefileid());
      lc.log(log::WARNING, "In ArchiveRequest::updateJobStatus(): unexpected status. Assuming ToTransfer.");
    }
    break;
  case JobEvent::ReportFailed:
    if (*currentStatus != ArchiveJobStatus::AJS_ToReportForFailure && *currentStatus != ArchiveJobStatus::AJS_ToReportForTransfer) {
      // Wrong status, but end status will be the same anyway.
      log::ScopedParamContainer params(lc);
      params.add("event", eventToString(jobEvent))
              .add("status", statusToString(*currentStatus))
            .add("fileId", m_payload.archivefileid());
      lc.log(log::WARNING, "In ArchiveRequest::updateJobStatus(): unexpected status. Failing the job.");
    }
  }
  // We are in the normal cases now.
  EnqueueingNextStep ret;
  switch (jobEvent) {  
  case JobEvent::TransferFailed:
  {
    if (!m_payload.reportdecided()) {
      m_payload.set_reportdecided(true);
      ret.nextStep = EnqueueingNextStep::NextStep::EnqueueForReport;
      ret.nextStatus = serializers::ArchiveJobStatus::AJS_ToReportForFailure;
    } else {
      ret.nextStep = EnqueueingNextStep::NextStep::StoreInFailedJobsContainer;
      ret.nextStatus = serializers::ArchiveJobStatus::AJS_Failed;
    }
  }
  case JobEvent::ReportFailed:
  {
    ret.nextStep = EnqueueingNextStep::NextStep::StoreInFailedJobsContainer;
    ret.nextStatus = serializers::ArchiveJobStatus::AJS_Failed;
  }
  }
  return ret;
}

std::string ArchiveRequest::dump() {
  checkPayloadReadable();
  google::protobuf::util::JsonPrintOptions options;
  options.add_whitespace = true;
  options.always_print_primitive_fields = true;
  std::string headerDump;
  google::protobuf::util::MessageToJsonString(m_payload, &headerDump, options);
  return headerDump;
}

std::list<std::string> ArchiveRequest::getFailures() {
  checkPayloadReadable();
  std::list<std::string> ret;
  for (auto &j: m_payload.jobs()) {
    for (auto &f: j.failurelogs()) {
      ret.push_back(f);
    }
  }
  return ret;
}

void ArchiveRequest::setJobStatus(uint16_t copyNumber, const serializers::ArchiveJobStatus& status) {
  checkPayloadWritable();
  for (auto j=m_payload.mutable_jobs()->begin(); j!=m_payload.mutable_jobs()->end(); j++) {
    if (j->copynb() == copyNumber) {
      j->set_status(status);
      return;
    }
  }
  throw exception::Exception("In ArchiveRequest::setJobStatus(): job not found.");
}

std::string ArchiveRequest::getTapePoolForJob(uint16_t copyNumber) {
  checkPayloadReadable();
  for (auto j:m_payload.jobs()) if (j.copynb() == copyNumber) return j.tapepool();
  throw exception::Exception("In ArchiveRequest::getTapePoolForJob(): job not found.");
}




}} // namespace cta::objectstore


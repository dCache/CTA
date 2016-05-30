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
#include "common/dataStructures/EntryLog.hpp"
#include <json-c/json.h>

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
  // This object is good to go (to storage)
  m_payloadInterpreted = true;
}

void cta::objectstore::ArchiveRequest::addJob(uint16_t copyNumber,
  const std::string& tapepool, const std::string& archivequeueaddress) {
  checkPayloadWritable();
  auto *j = m_payload.add_jobs();
  j->set_copynb(copyNumber);
  j->set_status(serializers::ArchiveJobStatus::AJS_PendingNsCreation);
  j->set_tapepool(tapepool);
  j->set_owner("");
  j->set_archivequeueaddress(archivequeueaddress);
  j->set_totalretries(0);
  j->set_retrieswithinmount(0);
  j->set_lastmountwithfailure(0);
  // Those 2 values are set to 0 as at creation time, we do not read the 
  // tape pools yet.
  j->set_maxretrieswithinmount(0);
  j->set_maxtotalretries(0);
}

bool cta::objectstore::ArchiveRequest::setJobSuccessful(uint16_t copyNumber) {
  checkPayloadWritable();
  auto * jl = m_payload.mutable_jobs();
  for (auto j=jl->begin(); j!=jl->end(); j++) {
    if (j->copynb() == copyNumber) {
      j->set_status(serializers::ArchiveJobStatus::AJS_Complete);
      for (auto j2=jl->begin(); j2!=jl->end(); j2++) {
        if (j2->status()!= serializers::ArchiveJobStatus::AJS_Complete && 
            j2->status()!= serializers::ArchiveJobStatus::AJS_Failed)
          return false;
      }
      return true;
    }
  }
  throw NoSuchJob("In ArchiveRequest::setJobSuccessful(): job not found");
}


void cta::objectstore::ArchiveRequest::setJobFailureLimits(uint16_t copyNumber,
    uint16_t maxRetiesWithinMount, uint16_t maxTotalRetries) {
  checkPayloadWritable();
  auto * jl = m_payload.mutable_jobs();
  for (auto j=jl->begin(); j!=jl->end(); j++) {
    if (j->copynb() == copyNumber) {
      j->set_maxretrieswithinmount(maxRetiesWithinMount);
      j->set_maxtotalretries(maxTotalRetries);
      return;
    }
  }
  throw NoSuchJob("In ArchiveRequest::setJobFailureLimits(): job not found");
}

bool cta::objectstore::ArchiveRequest::addJobFailure(uint16_t copyNumber,
    uint64_t mountId) {
  checkPayloadWritable();
  auto * jl = m_payload.mutable_jobs();
  // Find the job and update the number of failures (and return the new count)
  for (auto j=jl->begin(); j!=jl->end(); j++) {
    if (j->copynb() == copyNumber) {
      if (j->lastmountwithfailure() == mountId) {
        j->set_retrieswithinmount(j->retrieswithinmount() + 1);
      } else {
        j->set_retrieswithinmount(1);
        j->set_lastmountwithfailure(mountId);
      }
      j->set_totalretries(j->totalretries() + 1);
    }
    if (j->totalretries() >= j->maxtotalretries()) {
      j->set_status(serializers::AJS_Failed);
      finishIfNecessary();
      return true;
    } else {
      j->set_status(serializers::AJS_PendingMount);
      return false;
    }
  }
  throw NoSuchJob ("In ArchiveRequest::addJobFailure(): could not find job");
}


void cta::objectstore::ArchiveRequest::setAllJobsLinkingToArchiveQueue() {
  checkPayloadWritable();
  auto * jl=m_payload.mutable_jobs();
  for (auto j=jl->begin(); j!=jl->end(); j++) {
    j->set_status(serializers::AJS_LinkingToArchiveQueue);
  }
}

void cta::objectstore::ArchiveRequest::setAllJobsFailed() {
  checkPayloadWritable();
  auto * jl=m_payload.mutable_jobs();
  for (auto j=jl->begin(); j!=jl->end(); j++) {
    j->set_status(serializers::AJS_Failed);
  }
}

void cta::objectstore::ArchiveRequest::setAllJobsPendingNSdeletion() {
  checkPayloadWritable();
  auto * jl=m_payload.mutable_jobs();
  for (auto j=jl->begin(); j!=jl->end(); j++) {
    j->set_status(serializers::AJS_PendingNsDeletion);
  }
}

//------------------------------------------------------------------------------
// setArchiveFileID
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setArchiveFileID(const uint64_t archiveFileID) {
  checkPayloadWritable();
  m_payload.set_archivefileid(archiveFileID);
}

//------------------------------------------------------------------------------
// getArchiveFileID
//------------------------------------------------------------------------------
uint64_t cta::objectstore::ArchiveRequest::getArchiveFileID() {
  checkPayloadReadable();
  return m_payload.archivefileid();
}

//------------------------------------------------------------------------------
// setChecksumType
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setChecksumType(const std::string &checksumType) {
  checkPayloadWritable();
  m_payload.set_checksumtype(checksumType);
}

//------------------------------------------------------------------------------
// getChecksumType
//------------------------------------------------------------------------------
std::string cta::objectstore::ArchiveRequest::getChecksumType() {
  checkPayloadReadable();
  return m_payload.checksumtype();
}

//------------------------------------------------------------------------------
// setChecksumValue
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setChecksumValue(const std::string &checksumValue) {
  checkPayloadWritable();
  m_payload.set_checksumvalue(checksumValue);
}

//------------------------------------------------------------------------------
// getChecksumValue
//------------------------------------------------------------------------------
std::string cta::objectstore::ArchiveRequest::getChecksumValue() {
  checkPayloadReadable();
  return m_payload.checksumvalue();
}

//------------------------------------------------------------------------------
// setDiskpoolName
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setDiskpoolName(const std::string &diskpoolName) {
  checkPayloadWritable();
  m_payload.set_diskpoolname(diskpoolName);
}

//------------------------------------------------------------------------------
// getDiskpoolName
//------------------------------------------------------------------------------
std::string cta::objectstore::ArchiveRequest::getDiskpoolName() {
  checkPayloadReadable();
  return m_payload.diskpoolname();
}

//------------------------------------------------------------------------------
// setDiskpoolThroughput
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setDiskpoolThroughput(const uint64_t diskpoolThroughput) {
  checkPayloadWritable();
  m_payload.set_diskpoolthroughput(diskpoolThroughput);
}

//------------------------------------------------------------------------------
// getDiskpoolThroughput
//------------------------------------------------------------------------------
uint64_t cta::objectstore::ArchiveRequest::getDiskpoolThroughput() {
  checkPayloadReadable();
  return m_payload.diskpoolthroughput();
}

//------------------------------------------------------------------------------
// setDiskpoolThroughput
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setMountPolicy(const cta::common::dataStructures::MountPolicy &mountPolicy) {
  checkPayloadWritable();
  auto payloadMountPolicy = m_payload.mutable_mountpolicy();
  payloadMountPolicy->set_name(mountPolicy.name);
  payloadMountPolicy->set_maxdrives(mountPolicy.maxDrivesAllowed);
  payloadMountPolicy->set_minrequestage(mountPolicy.archiveMinRequestAge);
  payloadMountPolicy->set_priority(mountPolicy.archivePriority);
}

//------------------------------------------------------------------------------
// setDiskpoolThroughput
//------------------------------------------------------------------------------
cta::common::dataStructures::MountPolicy cta::objectstore::ArchiveRequest::getMountPolicy() {
  checkPayloadReadable();
  cta::common::dataStructures::MountPolicy mountPolicy;
  auto payloadMountPolicy = m_payload.mountpolicy();
  mountPolicy.maxDrivesAllowed=payloadMountPolicy.maxdrives();
  mountPolicy.archiveMinRequestAge=payloadMountPolicy.minrequestage();
  mountPolicy.archivePriority=payloadMountPolicy.priority();
  return mountPolicy;
}

//------------------------------------------------------------------------------
// setDrData
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setDrData(const cta::common::dataStructures::DRData &drData) {
  checkPayloadWritable();
  auto payloadDrData = m_payload.mutable_drdata();
  payloadDrData->set_drblob(drData.drBlob);
  payloadDrData->set_drgroup(drData.drGroup);
  payloadDrData->set_drowner(drData.drOwner);
  payloadDrData->set_drpath(drData.drPath);
}

//------------------------------------------------------------------------------
// getDrData
//------------------------------------------------------------------------------
cta::common::dataStructures::DRData cta::objectstore::ArchiveRequest::getDrData() {
  checkPayloadReadable();
  cta::common::dataStructures::DRData drData;
  auto payloadDrData = m_payload.drdata();
  drData.drBlob=payloadDrData.drblob();
  drData.drGroup=payloadDrData.drgroup();
  drData.drOwner=payloadDrData.drowner();
  drData.drPath=payloadDrData.drpath();
  return drData;
}

//------------------------------------------------------------------------------
// setDiskFileID
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setDiskFileID(const std::string &diskFileID) {
  checkPayloadWritable();
  m_payload.set_diskfileid(diskFileID);
}

//------------------------------------------------------------------------------
// getDiskFileID
//------------------------------------------------------------------------------
std::string cta::objectstore::ArchiveRequest::getDiskFileID() {
  checkPayloadReadable();
  return m_payload.diskfileid();
}

//------------------------------------------------------------------------------
// setInstance
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setInstance(const std::string &instance) {
  checkPayloadWritable();
  m_payload.set_instance(instance);
}

//------------------------------------------------------------------------------
// getInstance
//------------------------------------------------------------------------------
std::string cta::objectstore::ArchiveRequest::getInstance() {
  checkPayloadReadable();
  return m_payload.instance();
}

//------------------------------------------------------------------------------
// setFileSize
//------------------------------------------------------------------------------
void cta::objectstore::ArchiveRequest::setFileSize(const uint64_t fileSize) {
  checkPayloadWritable();
  m_payload.set_filesize(fileSize);
}

//------------------------------------------------------------------------------
// getFileSize
//------------------------------------------------------------------------------
uint64_t cta::objectstore::ArchiveRequest::getFileSize() {
  checkPayloadReadable();
  return m_payload.filesize();
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
// setStorageClass
//------------------------------------------------------------------------------
void ArchiveRequest::setStorageClass(const std::string &storageClass) {
  checkPayloadWritable();
  m_payload.set_storageclass(storageClass);
}

//------------------------------------------------------------------------------
// getStorageClass
//------------------------------------------------------------------------------
std::string ArchiveRequest::getStorageClass() {
  checkPayloadReadable();
  return m_payload.storageclass();
}

//------------------------------------------------------------------------------
// setCreationLog
//------------------------------------------------------------------------------
void ArchiveRequest::setCreationLog(const cta::common::dataStructures::EntryLog &creationLog) {
  checkPayloadWritable();
  auto payloadCreationLog = m_payload.mutable_creationlog();
  payloadCreationLog->set_time(creationLog.time);
  payloadCreationLog->set_host(creationLog.host);
  payloadCreationLog->set_username(creationLog.username);
}

//------------------------------------------------------------------------------
// getCreationLog
//------------------------------------------------------------------------------
cta::common::dataStructures::EntryLog ArchiveRequest::getCreationLog() {
  checkPayloadReadable();
  cta::common::dataStructures::EntryLog creationLog;
  cta::common::dataStructures::UserIdentity user;
  auto payloadCreationLog = m_payload.creationlog();
  creationLog.username=payloadCreationLog.username();
  creationLog.host=payloadCreationLog.host();
  creationLog.time=payloadCreationLog.time();
  return creationLog;  
}

auto ArchiveRequest::dumpJobs() -> std::list<ArchiveRequest::JobDump> {
  checkPayloadReadable();
  std::list<JobDump> ret;
  auto & jl = m_payload.jobs();
  for (auto j=jl.begin(); j!=jl.end(); j++) {
    ret.push_back(JobDump());
    ret.back().copyNb = j->copynb();
    ret.back().tapePool = j->tapepool();
    ret.back().ArchiveQueueAddress = j->archivequeueaddress();
  }
  return ret;
}

void ArchiveRequest::garbageCollect(const std::string &presumedOwner) {
  checkPayloadWritable();
  // The behavior here depends on which job the agent is supposed to own.
  // We should first find this job (if any). This is for covering the case
  // of a selected job. The Request could also still being connected to tape
  // pools. In this case we will finish the connection to tape pools unconditionally.
  auto * jl = m_payload.mutable_jobs();
  for (auto j=jl->begin(); j!=jl->end(); j++) {
    auto owner=j->owner();
    auto status=j->status();
    if (status==serializers::AJS_LinkingToArchiveQueue ||
        (status==serializers::AJS_Selected && owner==presumedOwner)) {
        // If the job was being connected to the tape pool or was selected
        // by the dead agent, then we have to ensure it is indeed connected to
        // the tape pool and set its status to pending.
        // (Re)connect the job to the tape pool and make it pending.
        // If we fail to reconnect, we have to fail the job and potentially
        // finish the request.
      try {
        ArchiveQueue aq(j->archivequeueaddress(), m_objectStore);
        ScopedExclusiveLock tpl(aq);
        aq.fetch();
        ArchiveRequest::JobDump jd;
        jd.copyNb = j->copynb();
        jd.tapePool = j->tapepool();
        jd.ArchiveQueueAddress = j->archivequeueaddress();
        if (aq.addJobIfNecessary(jd, getAddressIfSet(), m_payload.archivefileid(),
          m_payload.filesize()))
          aq.commit();
        j->set_status(serializers::AJS_PendingMount);
        commit();
      } catch (...) {
        j->set_status(serializers::AJS_Failed);
        // This could be the end of the request, with various consequences.
        // This is handled here:
        if (finishIfNecessary())
          return;
      }
    } else if (status==serializers::AJS_PendingNsCreation) {
      // If the job is pending NsCreation, we have to queue it in the tape pool's
      // queue for files orphaned pending ns creation. Some user process will have
      // to pick them up actively (recovery involves schedulerDB + NameServerDB)
      try {
        ArchiveQueue aq(j->archivequeueaddress(), m_objectStore);
        ScopedExclusiveLock tpl(aq);
        aq.fetch();
        ArchiveRequest::JobDump jd;
        jd.copyNb = j->copynb();
        jd.tapePool = j->tapepool();
        jd.ArchiveQueueAddress = j->archivequeueaddress();
        if (aq.addOrphanedJobPendingNsCreation(jd, getAddressIfSet(),
          m_payload.archivefileid(), m_payload.filesize()))
          aq.commit();
      } catch (...) {
        j->set_status(serializers::AJS_Failed);
        // This could be the end of the request, with various consequences.
        // This is handled here:
        if (finishIfNecessary())
          return;
      }
    } else if (status==serializers::AJS_PendingNsDeletion) {
      // If the job is pending NsDeletion, we have to queue it in the tape pool's
      // queue for files orphaned pending ns deletion. Some user process will have
      // to pick them up actively (recovery involves schedulerDB + NameServerDB)
      try {
        ArchiveQueue aq(j->archivequeueaddress(), m_objectStore);
        ScopedExclusiveLock tpl(aq);
        aq.fetch();
        ArchiveRequest::JobDump jd;
        jd.copyNb = j->copynb();
        jd.tapePool = j->tapepool();
        jd.ArchiveQueueAddress = j->archivequeueaddress();
        if (aq.addOrphanedJobPendingNsCreation(jd, getAddressIfSet(), 
          m_payload.archivefileid(), m_payload.filesize()))
          aq.commit();
        j->set_status(serializers::AJS_PendingMount);
        commit();
      } catch (...) {
        j->set_status(serializers::AJS_Failed);
        // This could be the end of the request, with various consequences.
        // This is handled here:
        if (finishIfNecessary())
          return;
      }
    } else {
      return;
    }
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

bool ArchiveRequest::finishIfNecessary() {
  checkPayloadWritable();
  // This function is typically called after changing the status of one job
  // in memory. If the job is complete, we will just remove it.
  // TODO: we will have to push the result to the ArchiveToDirRequest when
  // it gets implemented.
  // If all the jobs are either complete or failed, we can remove the request.
  auto & jl=m_payload.jobs();
  for (auto j=jl.begin(); j!=jl.end(); j++) {
    if (j->status() != serializers::AJS_Complete 
        && j->status() != serializers::AJS_Failed) {
      return false;
    }
  }
  remove();
  return true;
}

std::string ArchiveRequest::dump() {
  checkPayloadReadable();
  std::stringstream ret;
  ret << "ArchiveRequest" << std::endl;
  struct json_object * jo = json_object_new_object();
  json_object_object_add(jo, "checksumtype", json_object_new_string(m_payload.checksumtype().c_str()));
  json_object_object_add(jo, "checksumvalue", json_object_new_string(m_payload.checksumvalue().c_str()));
  json_object_object_add(jo, "diskpoolname", json_object_new_string(m_payload.diskpoolname().c_str()));
  json_object_object_add(jo, "diskpoolthroughput", json_object_new_int64(m_payload.diskpoolthroughput()));
  json_object_object_add(jo, "diskfileid", json_object_new_string(m_payload.diskfileid().c_str()));
  json_object_object_add(jo, "instance", json_object_new_string(m_payload.instance().c_str()));
  json_object_object_add(jo, "filesize", json_object_new_int64(m_payload.filesize()));
  json_object_object_add(jo, "srcurl", json_object_new_string(m_payload.srcurl().c_str()));
  json_object_object_add(jo, "storageclass", json_object_new_string(m_payload.storageclass().c_str()));
  // Object for creation log
  json_object * jaf = json_object_new_object();
  json_object_object_add(jaf, "host", json_object_new_string(m_payload.creationlog().host().c_str()));
  json_object_object_add(jaf, "time", json_object_new_int64(m_payload.creationlog().time()));
  json_object_object_add(jaf, "username", json_object_new_string(m_payload.creationlog().username().c_str()));
  json_object_object_add(jo, "creationlog", jaf);
  // Array for jobs
  json_object * jja = json_object_new_array();
  auto & jl = m_payload.jobs();
  for (auto j=jl.begin(); j!=jl.end(); j++) {
    // Object for job
    json_object * jj = json_object_new_object();
    json_object_object_add(jj, "copynb", json_object_new_int64(j->copynb()));
    json_object_object_add(jj, "lastmountwithfailure", json_object_new_int64(j->lastmountwithfailure()));
    json_object_object_add(jj, "maxretrieswithinmount", json_object_new_int64(j->maxretrieswithinmount()));
    json_object_object_add(jj, "maxtotalretries", json_object_new_int64(j->maxtotalretries()));
    json_object_object_add(jj, "owner", json_object_new_string(j->owner().c_str()));
    json_object_object_add(jj, "retrieswithinmount", json_object_new_int64(j->retrieswithinmount()));
    json_object_object_add(jj, "status", json_object_new_int64(j->status()));
    json_object_object_add(jj, "tapepool", json_object_new_string(j->tapepool().c_str()));
    json_object_object_add(jj, "tapepoolAddress", json_object_new_string(j->archivequeueaddress().c_str()));
    json_object_object_add(jj, "totalRetries", json_object_new_int64(j->totalretries()));
    json_object_array_add(jja, jj);
  }
  json_object_object_add(jo, "jobs", jja);
  // Object for drdata
  json_object * jlog = json_object_new_object();
  json_object_object_add(jlog, "drblob", json_object_new_string(m_payload.drdata().drblob().c_str()));
  json_object_object_add(jlog, "drgroup", json_object_new_string(m_payload.drdata().drgroup().c_str()));
  json_object_object_add(jlog, "drowner", json_object_new_string(m_payload.drdata().drowner().c_str()));
  json_object_object_add(jlog, "drpath", json_object_new_string(m_payload.drdata().drpath().c_str()));
  json_object_object_add(jo, "drdata", jlog);
  // Object for requester
  json_object * jrf = json_object_new_object();
  json_object_object_add(jrf, "name", json_object_new_string(m_payload.requester().name().c_str()));
  json_object_object_add(jrf, "group", json_object_new_string(m_payload.requester().group().c_str()));
  json_object_object_add(jo, "requester", jrf);
  ret << json_object_to_json_string_ext(jo, JSON_C_TO_STRING_PRETTY) << std::endl;
  json_object_put(jo);
  return ret.str();
}

}} // namespace cta::objectstore




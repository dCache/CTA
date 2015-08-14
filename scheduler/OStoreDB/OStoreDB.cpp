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

#include "OStoreDB.hpp"
#include "common/SecurityIdentity.hpp"
#include "objectstore/RootEntry.hpp"
#include "objectstore/TapePool.hpp"
#include "objectstore/Tape.hpp"
#include "objectstore/DriveRegister.hpp"
#include "objectstore/ArchiveToFileRequest.hpp"
#include "objectstore/RetrieveToFileRequest.hpp"
#include "common/exception/Exception.hpp"
#include "common/admin/AdminHost.hpp"
#include "common/admin/AdminUser.hpp"
#include "common/archiveRoutes/ArchiveRoute.hpp"
#include "scheduler/ArchiveRequest.hpp"
#include "scheduler/ArchiveToFileRequest.hpp"
#include "scheduler/LogicalLibrary.hpp"
#include "scheduler/RetrieveToFileRequest.hpp"
#include "common/archiveNS/StorageClass.hpp"
#include "common/TapePool.hpp"
#include "common/archiveNS/Tape.hpp"
#include "ArchiveToDirRequest.hpp"
#include "RetrieveToFileRequest.hpp"
#include "common/archiveNS/TapeCopyInfo.hpp"
#include "RetrieveToDirRequest.hpp"
#include "ArchiveToTapeCopyRequest.hpp"
#include <algorithm>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

namespace cta {
  
using namespace objectstore;

OStoreDB::OStoreDB(objectstore::Backend& be):
  m_objectStore(be), m_agent(NULL) {}


OStoreDB::~OStoreDB() throw() {}

void OStoreDB::setAgent(objectstore::Agent& agent) {
  m_agent = & agent;
  }

void OStoreDB::assertAgentSet() {
  if (!m_agent)
    throw AgentNotSet("In OStoreDB::assertAgentSet: Agent pointer not set");
  }

std::unique_ptr<SchedulerDatabase::TapeMountDecisionInfo> 
  OStoreDB::getMountInfo() {
  //Allocate the getMountInfostructure to return.
  std::unique_ptr<TapeMountDecisionInfo> privateRet (new TapeMountDecisionInfo());
  TapeMountDecisionInfo & tmdi=*privateRet;
  // Get all the tape pools and tapes with queues (potential mounts)
  objectstore::RootEntry re(m_objectStore);
  objectstore::ScopedSharedLock rel(re);
  re.fetch();
  auto tpl = re.dumpTapePools();
  for (auto tpp=tpl.begin(); tpp!=tpl.end(); tpp++) {
    // Get the tape pool object
    objectstore::TapePool tpool(tpp->address, m_objectStore);
    // debug utility variable
    std::string __attribute__((__unused__)) poolName = tpp->tapePool;
    objectstore::ScopedSharedLock tpl(tpool);
    tpool.fetch();
    // If there are files queued, we create an entry for this tape pool in the
    // mount candidates list.
    if (tpool.getJobsSummary().files) {
      tmdi.potentialMounts.push_back(SchedulerDatabase::PotentialMount());
      auto & m = tmdi.potentialMounts.back();
      m.type = cta::MountType::ARCHIVE;
      m.bytesQueued = tpool.getJobsSummary().bytes;
      m.filesQueued = tpool.getJobsSummary().files;      
      m.oldestJobStartTime = tpool.getJobsSummary().oldestJobStartTime;
      m.priority = tpool.getJobsSummary().priority;
      
      m.mountCriteria.maxFilesQueued = 
          tpool.getMountCriteria().archive.maxFilesQueued;
      m.mountCriteria.maxBytesQueued = 
          tpool.getMountCriteria().archive.maxBytesQueued;
      m.mountCriteria.maxAge = 
          tpool.getMountCriteria().archive.maxAge;

      m.mountQuota.quota = 
          tpool.getMountQuota().archive.quota;
      m.mountQuota.allowedOverhead = 
          tpool.getMountQuota().archive.allowedOverhead;
    
      m.logicalLibrary = "";

    }
    // For each tape in the pool, list the tapes with work
    auto tl = tpool.dumpTapes();
    for (auto tp = tl.begin(); tp!= tl.end(); tp++) {
      objectstore::Tape t(tp->address, m_objectStore);
      objectstore::ScopedSharedLock tl(t);
      t.fetch();
      if (t.getJobsSummary().files) {
        tmdi.potentialMounts.push_back(PotentialMount());
        auto & m = tmdi.potentialMounts.back();
        m.type = cta::MountType::RETRIEVE;
        m.bytesQueued = t.getJobsSummary().bytes;
        m.filesQueued = t.getJobsSummary().files;
        m.oldestJobStartTime = t.getJobsSummary().oldestJobStartTime;
        m.priority = t.getJobsSummary().priority;
        
        m.mountCriteria.maxFilesQueued = 
            tpool.getMountCriteria().retrieve.maxFilesQueued;
        m.mountCriteria.maxBytesQueued = 
            tpool.getMountCriteria().retrieve.maxBytesQueued;
        m.mountCriteria.maxAge = 
            tpool.getMountCriteria().retrieve.maxAge;

        m.mountQuota.quota = 
            tpool.getMountQuota().retrieve.quota;
        m.mountQuota.allowedOverhead = 
            tpool.getMountQuota().retrieve.allowedOverhead;
      
        m.logicalLibrary = t.getLogicalLibrary();
      }
    }
  }
  // Dedication information comes here
  // TODO
  // 
  // Collect information about the existing mounts
  objectstore::DriveRegister dr(re.getDriveRegisterAddress(), m_objectStore);
  objectstore::ScopedSharedLock drl(dr);
  dr.fetch();
  auto dl = dr.dumpDrives();
  for (auto d=dl.begin(); d!= dl.end(); d++) {
    if (d->mountStatus != objectstore::DriveRegister::MountStatus::NoMount) {
      tmdi.existingMounts.push_back(ExistingMount());
      switch (d->mountStatus) {
        case objectstore::DriveRegister::MountStatus::ArchiveMount:
          tmdi.existingMounts.back().type = cta::MountType::ARCHIVE;
          break;
        case objectstore::DriveRegister::MountStatus::RetrieveMount:
          tmdi.existingMounts.back().type = cta::MountType::ARCHIVE;
          break;
        default:
          throw exception::Exception("In OStoreDB::getMountInfo(): got drive with unexpected mount type");
      }
      tmdi.existingMounts.back().tapePool = d->currentTapePool;
    }
  }
  std::unique_ptr<SchedulerDatabase::TapeMountDecisionInfo> ret(std::move(privateRet));
  return ret;
}


std::unique_ptr<SchedulerDatabase::TapeMount> 
  OStoreDB::getNextMount(const std::string& logicalLibrary, const std::string& driveName) {
  throw NotImplemented("");
}


void OStoreDB::createAdminHost(const std::string& hostName,
    const cta::CreationLog & creationLog) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.addAdminHost(hostName, creationLog);
  re.commit();
}

std::list<AdminHost> OStoreDB::getAdminHosts() const {
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  std::list<AdminHost> ret;
  auto hl=re.dumpAdminHosts();
  rel.release();
  for (auto h=hl.begin(); h!=hl.end(); h++) {
    ret.push_back(AdminHost(h->hostname, 
        h->log));
  }
  return ret;
}


void OStoreDB::deleteAdminHost(const SecurityIdentity& requester, 
  const std::string& hostName) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.removeAdminHost(hostName);
  re.commit();
}

void OStoreDB::createAdminUser(const SecurityIdentity& requester, 
  const cta::UserIdentity& user, const std::string& comment) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  objectstore::CreationLog cl(requester.getUser(), requester.getHost(),
    time(NULL), comment);
  re.addAdminUser(objectstore::UserIdentity(user.uid, user.gid), cl);
  re.commit();
}

void OStoreDB::deleteAdminUser(const SecurityIdentity& requester, 
  const cta::UserIdentity& user) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.removeAdminUser(objectstore::UserIdentity(user.uid, user.gid));
  re.commit();
}

std::list<AdminUser> OStoreDB::getAdminUsers() const {
  std::list<AdminUser> ret;
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto aul = re.dumpAdminUsers();
  for (auto au=aul.begin(); au!=aul.end(); au++) {
    ret.push_back(
      AdminUser(
        cta::UserIdentity(au->user.uid, au->user.gid),
        au->log
    ));
  }
  rel.release();
  return ret;
}

void OStoreDB::assertIsAdminOnAdminHost(const SecurityIdentity& id) const {
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  if (!re.isAdminUser(objectstore::UserIdentity(id.getUser().uid,
      id.getUser().gid))) {
    std::ostringstream msg;
    msg << "User uid=" << id.getUser().uid 
        << " gid=" << id.getUser().gid
        << " is not an administrator";
    throw exception::Exception(msg.str());
  }
  if (!re.isAdminHost(id.getHost())) {
    std::ostringstream msg;
    msg << "Host " << id.getHost() << " is not an administration host";
    throw exception::Exception(msg.str());
  }
}

void OStoreDB::createStorageClass(const std::string& name,
  const uint16_t nbCopies, const cta::CreationLog& creationLog) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.addStorageClass(name, nbCopies, creationLog);
  re.commit();
}

StorageClass OStoreDB::getStorageClass(const std::string& name) const {
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto sc = re.dumpStorageClass(name);
  return cta::StorageClass(name,
    sc.copyCount,
    sc.log);
}

std::list<StorageClass> OStoreDB::getStorageClasses() const {
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto scl = re.dumpStorageClasses();
  decltype (getStorageClasses()) ret;
  for (auto sc = scl.begin(); sc != scl.end(); sc++) {
    ret.push_back(StorageClass(sc->storageClass, 
        sc->copyCount, sc->log));
  }
  ret.sort();
  return ret;
}

void OStoreDB::deleteStorageClass(const SecurityIdentity& requester, 
  const std::string& name) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.removeStorageClass(name);
  re.commit();
}

void OStoreDB::createArchiveRoute(const std::string& storageClassName,
  const uint16_t copyNb, const std::string& tapePoolName,
  const cta::CreationLog& creationLog) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.addArchiveRoute(storageClassName, copyNb, tapePoolName, 
    objectstore::CreationLog(creationLog));
  re.commit();
}

std::list<ArchiveRoute> 
  OStoreDB::getArchiveRoutes(const std::string& storageClassName) const {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  auto scd = re.dumpStorageClass(storageClassName);
  rel.release();
  if (scd.routes.size() != scd.copyCount) {
    throw IncompleteRouting("In OStoreDB::getArchiveRoutes: routes are incomplete");
  }
  std::list<ArchiveRoute> ret;
  for (auto r=scd.routes.begin(); r!=scd.routes.end(); r++) {
    ret.push_back(ArchiveRoute(storageClassName, 
      r->copyNumber, r->tapePool, r->log));
  }
  return ret;
}

std::list<ArchiveRoute> OStoreDB::getArchiveRoutes() const {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  auto scd = re.dumpStorageClasses();
  rel.release();
  std::list<ArchiveRoute> ret;
  for (auto sc=scd.begin(); sc!=scd.end(); sc++) {
    for (auto r=sc->routes.begin(); r!=sc->routes.end(); r++) {
      ret.push_back(ArchiveRoute(sc->storageClass, 
        r->copyNumber, r->tapePool, r->log));
    }
  }
  return ret;
}

void OStoreDB::deleteArchiveRoute(const SecurityIdentity& requester, 
  const std::string& storageClassName, const uint16_t copyNb) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.removeArchiveRoute(storageClassName, copyNb);
  re.commit();
}

void OStoreDB::ArchiveToFileRequestCreation::complete() {
  // We inherited all the objects from the creation.
  // Lock is still here at that point.
  // First, record that we are fine for next step.
  m_request.setJobsLinkingToTapePool();
  m_request.commit();
  objectstore::RootEntry re(m_objectStore);
  // We can now plug the request onto its tape pools.
  // We can discover at that point that a tape pool is actually not
  // really owned by the root entry, and hence a dangling pointer
  // We should then unlink the jobs from that already connected
  // tape pools and abort the job creation.
  // The list of done tape pools is held here for this purpose
  // Reconstruct the job list
  auto jl = m_request.dumpJobs();
  std::list<std::string> linkedTapePools;
  try {
    for (auto j=jl.begin(); j!=jl.end(); j++) {
      objectstore::TapePool tp(j->tapePoolAddress, m_objectStore);
      ScopedExclusiveLock tpl(tp);
      tp.fetch();
      if (tp.getOwner() != re.getAddressIfSet())
        throw NoSuchTapePool("In OStoreDB::queue: non-existing tape pool found "
            "(dangling pointer): cancelling request creation.");
      tp.addJob(*j, m_request.getAddressIfSet(), m_request.getArchiveFile(), 
        m_request.getRemoteFile().status.size, m_request.getPriority());
      tp.commit();
      linkedTapePools.push_back(j->tapePoolAddress);
    }
  } catch (NoSuchTapePool &) {
    // Unlink the request from already connected tape pools
    for (auto tpa=linkedTapePools.begin(); tpa!=linkedTapePools.end(); tpa++) {
      objectstore::TapePool tp(*tpa, m_objectStore);
      ScopedExclusiveLock tpl(tp);
      tp.fetch();
      tp.removeJob(m_request.getAddressIfSet());
      tp.commit();
      m_request.remove();
    }
    throw;
  }
  // The request is now fully set. As it's multi-owned, we do not set the owner,
  // just to disown it from the agent.
  m_request.setOwner("");
  m_request.commit();
  m_lock.release();
  // And remove reference from the agent
  {
    objectstore::ScopedExclusiveLock al(*m_agent);
    m_agent->fetch();
    m_agent->removeFromOwnership(m_request.getAddressIfSet());
    m_agent->commit();
  }
  m_closed=true;
  return;
}

void OStoreDB::ArchiveToFileRequestCreation::cancel() {
  // We inherited everything from the creation, and all we have to
  // do here is to delete the request from storage and dereference it from
  // the agent's entry
  if (m_closed) {
    throw ArchiveRequestAlreadyCompleteOrCanceled(
      "In OStoreDB::ArchiveToFileRequestCreation::cancel: trying the close "
      "the request creation twice");
  }
  m_request.remove();
  {
    objectstore::ScopedExclusiveLock al(*m_agent);
    m_agent->fetch();
    m_agent->removeFromOwnership(m_request.getAddressIfSet());
    m_agent->commit();
  }
  m_closed=true;
  return;
}

OStoreDB::ArchiveToFileRequestCreation::~ArchiveToFileRequestCreation() {
  // We have to determine whether complete() or cancel() were called, in which
  // case there is nothing to do, or not, in which case we have to garbage
  // collect the archive to file request. This will queue it to the appropriate
  // tape pool(s) orphanesArchiveToFileCreations. The schedule will then 
  // determine its fate depending on the status of the NS entry creation
  // (no entry, just cancel, already created in NS, carry on).
  if (m_closed)
    return;
  try {
    m_request.garbageCollect(m_agent->getAddressIfSet());
    {
      objectstore::ScopedExclusiveLock al(*m_agent);
      m_agent->fetch();
      m_agent->removeFromOwnership(m_request.getAddressIfSet());
      m_agent->commit();
    }
    m_closed=true;
  } catch (...) {}
}

void OStoreDB::createTapePool(const std::string& name,
  const uint32_t nbPartialTapes, const cta::CreationLog &creationLog) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  assertAgentSet();
  re.addOrGetTapePoolAndCommit(name, nbPartialTapes, *m_agent, creationLog);
  re.commit();
}

std::list<cta::TapePool> OStoreDB::getTapePools() const {
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto tpd = re.dumpTapePools();
  rel.release();
  std::list<cta::TapePool> ret;
  for (auto tp=tpd.begin(); tp!=tpd.end(); tp++) {
    ret.push_back(cta::TapePool(tp->tapePool, tp->nbPartialTapes, tp->log));
  }
  return ret;
}

void OStoreDB::deleteTapePool(const SecurityIdentity& requester, 
  const std::string& name) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.removeTapePoolAndCommit(name);
  re.commit();
}

void OStoreDB::createTape(const std::string& vid, 
  const std::string& logicalLibraryName, 
  const std::string& tapePoolName, const uint64_t capacityInBytes, 
  const cta::CreationLog& creationLog) {
  // To create a tape, we have to
  // - Find the storage class and lock for write.
  // - Create the tape object.
  // - Connect the tape object to the tape pool.
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  // Check the library exists
  auto libs = re.dumpLibraries();
  for (auto l=libs.begin(); l!=libs.end(); l++) {
    if (l->library == logicalLibraryName)
      goto found;
  }
  throw NoSuchLibrary("In OStoreDB::createTape: trying to create a tape in a non-existing library");
  found:
  std::string tpAddress = re.getTapePoolAddress(tapePoolName);
  // Take hold of the tape pool
  objectstore::TapePool tp(tpAddress, m_objectStore);
  ScopedExclusiveLock tpl(tp);
  tp.fetch();
  // Check the tape pool is owned by the root entry. If not, it should be
  // considered as a dangling pointer.
  if (tp.getOwner() != re.getAddressIfSet())
    throw NoSuchTapePool("In OStoreDB::createTape: trying to create a tape in a"
      " non-existing tape pool (dangling pointer)");
  // Check that the tape exists and throw an exception if it does.
  // TODO: we should check in all tape pools (or have a central index)
  try {
    tp.getTapeAddress(vid);
    throw TapeAlreadyExists("In OStoreDB::createTape: trying to create an existing tape.");
  } catch (cta::exception::Exception &) {}
  // Create the tape. The tape pool method takes care of the gory details for us.
  tp.addOrGetTapeAndCommit(vid, logicalLibraryName, capacityInBytes, 
      *m_agent, creationLog);
  tp.commit();
}

cta::Tape OStoreDB::getTape(const std::string &vid) const {
  cta::Tape ret;
  // Got through all tape pools. Get the list of them
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto tpl = re.dumpTapePools();
  for (auto tpi=tpl.begin(); tpi!=tpl.end(); tpi++) {
    objectstore::TapePool tp(tpi->address, m_objectStore);
    ScopedSharedLock tpl(tp);
    tp.fetch();
    // Check the tape pool is owned by the root entry. If not, it should be
    // considered as a dangling pointer (and skip it)
    if (tp.getOwner() != re.getAddressIfSet())
      continue;
    auto tl=tp.dumpTapes();
    for (auto ti=tl.begin(); ti!=tl.end(); ti++) {
      if (vid == ti->vid) {
        objectstore::Tape t(ti->address, m_objectStore);
        ScopedSharedLock tl(t);
        t.fetch();
        const uint64_t nbFiles = 0; // TO BE DONE
        return cta::Tape(ti->vid, nbFiles, ti->logicalLibraryName,
          tpi->tapePool, ti->capacityInBytes, t.getStoredData(), ti->log);
      }
    }
  }
  throw NoSuchTape("In OStoreDB::getTape: No such tape");
}

std::list<cta::Tape> OStoreDB::getTapes() const {
  std::list<cta::Tape> ret;
  // Got through all tape pools. Get the list of them
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto tpl = re.dumpTapePools();
  for (auto tpi=tpl.begin(); tpi!=tpl.end(); tpi++) {
    objectstore::TapePool tp(tpi->address, m_objectStore);
    ScopedSharedLock tpl(tp);
    tp.fetch();
    // Check the tape pool is owned by the root entry. If not, it should be
    // considered as a dangling pointer (and skip it)
    if (tp.getOwner() != re.getAddressIfSet())
      continue;
    auto tl=tp.dumpTapes();
    for (auto ti=tl.begin(); ti!=tl.end(); ti++) {
      objectstore::Tape t(ti->address, m_objectStore);
      ScopedSharedLock tl(t);
      t.fetch();
      const uint64_t nbFiles = 0; // TO BE DONE
      ret.push_back(cta::Tape(ti->vid, nbFiles, ti->logicalLibraryName,
        tpi->tapePool, ti->capacityInBytes, t.getStoredData(), ti->log));
    }
  }
  return ret;
}

void OStoreDB::deleteTape(const SecurityIdentity& requester, 
  const std::string& vid) {
  throw exception::Exception("Not Implemented");
}

void OStoreDB::createLogicalLibrary(const std::string& name, 
  const cta::CreationLog& creationLog) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.addLibrary(name, creationLog);
  re.commit();
}

std::list<LogicalLibrary> OStoreDB::getLogicalLibraries() const {
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto ll = re.dumpLibraries();
  rel.release();
  std::list<LogicalLibrary> ret;
  for (auto l=ll.begin(); l!=ll.end(); l++) {
    ret.push_back(LogicalLibrary(l->library, l->log));
  }
  return ret;
}

void OStoreDB::deleteLogicalLibrary(const SecurityIdentity& requester, 
  const std::string& name) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  // Check we are not deleting a non-empty library
  auto tpl = re.dumpTapePools();
  for (auto tpp=tpl.begin(); tpp!=tpl.end(); tpp++) {
    objectstore::TapePool tp(tpp->address, m_objectStore);
    ScopedSharedLock tplock(tp);
    tp.fetch();
    // Check the tape pool is owned by the root entry. If not, it should be
    // considered as a dangling pointer.
    if (tp.getOwner() != re.getAddressIfSet())
      continue;
    auto tl=tp.dumpTapes();
    for (auto t=tl.begin(); t!=tl.end(); t++) {
      if (t->logicalLibraryName == name)
        throw LibraryInUse("In OStoreDB::deleteLogicalLibrary: trying to delete a library used by a tape.");
    }
  }
  re.fetch();
  re.removeLibrary(name);
  re.commit();
}

std::unique_ptr<cta::SchedulerDatabase::ArchiveToFileRequestCreation>
  OStoreDB::queue(const cta::ArchiveToFileRequest& rqst) {
  assertAgentSet();
  // Construct the return value immediately
  std::unique_ptr<cta::OStoreDB::ArchiveToFileRequestCreation> 
    internalRet(new cta::OStoreDB::ArchiveToFileRequestCreation(m_agent, m_objectStore));
  cta::objectstore::ArchiveToFileRequest & atfr = internalRet->m_request;
  atfr.setAddress(m_agent->nextId("ArchiveToFileRequest"));
  atfr.initialize();
  atfr.setArchiveFile(rqst.archiveFile);
  atfr.setRemoteFile(rqst.remoteFile);
  atfr.setPriority(rqst.priority);
  atfr.setCreationLog(rqst.creationLog);
  // We will need to identity tapepools is order to construct the request
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto & cl = rqst.copyNbToPoolMap;
  std::list<cta::objectstore::ArchiveToFileRequest::JobDump> jl;
  for (auto copy=cl.begin(); copy != cl.end(); copy++) {
    std::string tpaddr = re.getTapePoolAddress(copy->second);
    atfr.addJob(copy->first, copy->second, tpaddr);
    jl.push_back(cta::objectstore::ArchiveToFileRequest::JobDump());
    jl.back().copyNb = copy->first;
    jl.back().tapePool = copy->second;
    jl.back().tapePoolAddress = tpaddr;
  }
  if (!jl.size()) {
    throw ArchiveRequestHasNoCopies("In OStoreDB::queue: the archive to file request has no copy");
  }
  // We create the object here
  {
    objectstore::ScopedExclusiveLock al(*m_agent);
    m_agent->fetch();
    m_agent->addToOwnership(atfr.getAddressIfSet());
    m_agent->commit();
  }
  atfr.setOwner(m_agent->getAddressIfSet());
  atfr.insert();
  internalRet->m_lock.lock(atfr);  
  // We successfully prepared the object. It will remain attached to the agent 
  // entry for the time being and get plugged to the tape pools on a second
  // pass. 
  // TODO: this can be improved by passing an opaque set of data to the called
  // (including the lock) in order to optimise the acesses to the object store.
  // In the mean time, the step 2 of this insertion will be done by finding the
  // archiveRequest from the agent's owned object. Crude, but should not be too
  // bad as the agent is not supposed to own many objects in this place.
  std::unique_ptr<cta::SchedulerDatabase::ArchiveToFileRequestCreation> ret;
  ret.reset(internalRet.release());
  return ret;
}

void OStoreDB::queue(const ArchiveToDirRequest& rqst) {
  auto & archiveToFileRequests = rqst.getArchiveToFileRequests();
  for(auto req=archiveToFileRequests.begin(); req!=archiveToFileRequests.end(); req++) {
    queue(*req);
  }
}

void OStoreDB::deleteArchiveRequest(const SecurityIdentity& requester, 
  const std::string& archiveFile) {
  // First of, find the archive request form all the tape pools.
  objectstore::RootEntry re(m_objectStore);
  objectstore::ScopedSharedLock rel(re);
  re.fetch();
  auto tpl = re.dumpTapePools();
  rel.release();
  for (auto tpp=tpl.begin(); tpp!= tpl.end(); tpp++) {
    objectstore::TapePool tp(tpp->address, m_objectStore);
    ScopedSharedLock tplock(tp);
    tp.fetch();
    auto ajl=tp.dumpJobs();
    tplock.release();
    for (auto ajp=ajl.begin(); ajp!=ajl.end(); ajp++) {
      objectstore::ArchiveToFileRequest atfr(ajp->address, m_objectStore);
      ScopedSharedLock atfrl(atfr);
      atfr.fetch();
      if (atfr.getArchiveFile() == archiveFile) {
        atfrl.release();
        objectstore::ScopedExclusiveLock al(*m_agent);
        m_agent->fetch();
        m_agent->addToOwnership(atfr.getAddressIfSet());
        m_agent->commit();
        ScopedExclusiveLock atfrxl(atfr);
        atfr.fetch();
        atfr.setJobsFailed();
        atfr.setOwner(m_agent->getAddressIfSet());
        atfr.commit();
        auto jl = atfr.dumpJobs();
        for (auto j=jl.begin(); j!=jl.end(); j++) {
          try {
            objectstore::TapePool tp(j->tapePoolAddress, m_objectStore);
            ScopedExclusiveLock tpl(tp);
            tp.fetch();
            tp.removeJob(atfr.getAddressIfSet());
            tp.commit();
          } catch (...) {}
        }
        atfr.remove();
        m_agent->removeFromOwnership(atfr.getAddressIfSet());
        m_agent->commit();
      }
    }
  }
  throw NoSuchArchiveRequest("In OStoreDB::deleteArchiveRequest: ArchiveToFileRequest not found");
}

std::unique_ptr<SchedulerDatabase::ArchiveToFileRequestCancelation>
  OStoreDB::markArchiveRequestForDeletion(const SecurityIdentity& requester,
  const std::string& archiveFile) {
  assertAgentSet();
  // Construct the return value immediately
  std::unique_ptr<cta::OStoreDB::ArchiveToFileRequestCancelation>
    internalRet(new cta::OStoreDB::ArchiveToFileRequestCancelation(m_agent, m_objectStore));
  cta::objectstore::ArchiveToFileRequest & atfr = internalRet->m_request;
  cta::objectstore::ScopedExclusiveLock & atfrl = internalRet->m_lock;
  // Attempt to find the request
  objectstore::RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto tpl=re.dumpTapePools();
  rel.release();
  for (auto tpp=tpl.begin(); tpp!=tpl.end(); tpp++) {
    try {
      objectstore::TapePool tp(tpp->address, m_objectStore);
      ScopedSharedLock tpl(tp);
      tp.fetch();
      auto arl = tp.dumpJobs();
      tpl.release();
      for (auto arp=arl.begin(); arp!=arl.end(); arp++) {
        objectstore::ArchiveToFileRequest tatfr(arp->address, m_objectStore);
        objectstore::ScopedSharedLock tatfrl(tatfr);
        tatfr.fetch();
        if (tatfr.getArchiveFile() == archiveFile) {
          // Point the agent to the request
          ScopedExclusiveLock agl(*m_agent);
          m_agent->fetch();
          m_agent->addToOwnership(arp->address);
          m_agent->commit();
          agl.release();
          // Mark all jobs are being pending NS deletion (for being deleted them selves) 
          tatfrl.release();
          atfr.setAddress(arp->address);
          atfrl.lock(atfr);
          atfr.fetch();
          atfr.setJobsPendingNSdeletion();
          atfr.commit();
          // Unlink the jobs from the tape pools (it is safely referenced in the agent)
          auto atpl=atfr.dumpJobs();
          for (auto atpp=atpl.begin(); atpp!=atpl.end(); atpp++) {
            objectstore::TapePool atp(atpp->tapePoolAddress, m_objectStore);
            objectstore::ScopedExclusiveLock atpl(atp);
            atp.fetch();
            atp.removeJob(arp->address);
            atp.commit();
          }
          // Return the object to the caller, so complete() can be called later.
          std::unique_ptr<cta::SchedulerDatabase::ArchiveToFileRequestCancelation> ret;
          ret.reset(internalRet.release());
          return ret;
        }
      }
    } catch (...) {}
  }
  throw NoSuchArchiveRequest("In OStoreDB::markArchiveRequestForDeletion: ArchiveToFileRequest no found");
  }

void OStoreDB::ArchiveToFileRequestCancelation::complete() {
  if (m_closed)
    throw ArchiveRequestAlreadyDeleted("OStoreDB::ArchiveToFileRequestCancelation::complete(): called twice");
  // We just need to delete the object and forget it
  m_request.remove();
  objectstore::ScopedExclusiveLock al (*m_agent);
  m_agent->fetch();
  m_agent->removeFromOwnership(m_request.getAddressIfSet());
  m_agent->commit();
  m_closed = true;
  }

OStoreDB::ArchiveToFileRequestCancelation::~ArchiveToFileRequestCancelation() {
  if (!m_closed) {
    try {
      m_request.garbageCollect(m_agent->getAddressIfSet());
      objectstore::ScopedExclusiveLock al (*m_agent);
      m_agent->fetch();
      m_agent->removeFromOwnership(m_request.getAddressIfSet());
      m_agent->commit();
    } catch (...) {}
  }
}



std::map<cta::TapePool, std::list<ArchiveToTapeCopyRequest> >
  OStoreDB::getArchiveRequests() const {
  objectstore::RootEntry re(m_objectStore);
  objectstore::ScopedSharedLock rel(re);
  re.fetch();
  std::map<cta::TapePool, std::list<ArchiveToTapeCopyRequest> > ret;
  auto tpl = re.dumpTapePools();
  rel.release();
  for (auto tpp=tpl.begin(); tpp!=tpl.end(); tpp++) {
    cta::TapePool tp(tpp->tapePool, tpp->nbPartialTapes, tpp->log);
    objectstore::TapePool ostp(tpp->address, m_objectStore);
    ScopedSharedLock ostpl(ostp);
    ostp.fetch();
    auto arl = ostp.dumpJobs();
    ostpl.release();
    for (auto ar=arl.begin(); ar!=arl.end(); ar++) {
      objectstore::ArchiveToFileRequest osar(ar->address, m_objectStore);
      ScopedSharedLock osarl(osar);
      osar.fetch();
      // Find which copy number is for this tape pool.
      // skip the request if not found
      auto jl = osar.dumpJobs();
      uint16_t copynb;
      bool copyndFound=false;
      for (auto j=jl.begin(); j!=jl.end(); j++) {
        if (j->tapePool == tpp->tapePool) {
          copynb = j->copyNb;
          copyndFound = true;
          break;
        }
      }
      if (!copyndFound) continue;
      ret[tp].push_back(cta::ArchiveToTapeCopyRequest(
        osar.getRemoteFile(),
        osar.getArchiveFile(),
        copynb,
        tpp->tapePool,
        osar.getPriority(),
        osar.getCreationLog()));
    }
  }
  return ret;
}

std::list<ArchiveToTapeCopyRequest>
  OStoreDB::getArchiveRequests(const std::string& tapePoolName) const {
  objectstore::RootEntry re(m_objectStore);
  objectstore::ScopedSharedLock rel(re);
  re.fetch();
  auto tpl = re.dumpTapePools();
  rel.release();
  for (auto tpp=tpl.begin(); tpp!=tpl.end(); tpp++) {
    if (tpp->tapePool != tapePoolName) continue;
    std::list<ArchiveToTapeCopyRequest> ret;
    cta::TapePool tp(tpp->tapePool, tpp->nbPartialTapes, tpp->log);
    objectstore::TapePool ostp(tpp->address, m_objectStore);
    ScopedSharedLock ostpl(ostp);
    ostp.fetch();
    auto arl = ostp.dumpJobs();
    ostpl.release();
    for (auto ar=arl.begin(); ar!=arl.end(); ar++) {
      objectstore::ArchiveToFileRequest osar(ar->address, m_objectStore);
      ScopedSharedLock osarl(osar);
      osar.fetch();
      // Find which copy number is for this tape pool.
      // skip the request if not found
      auto jl = osar.dumpJobs();
      uint16_t copynb;
      bool copyndFound=false;
      for (auto j=jl.begin(); j!=jl.end(); j++) {
        if (j->tapePool == tpp->tapePool) {
          copynb = j->copyNb;
          copyndFound = true;
          break;
        }
      }
      if (!copyndFound) continue;
      ret.push_back(cta::ArchiveToTapeCopyRequest(
        osar.getRemoteFile(),
        osar.getArchiveFile(),
        copynb,
        tpp->tapePool,
        osar.getPriority(),
        osar.getCreationLog()));
    }
    return ret;
  }
  throw NoSuchTapePool("In OStoreDB::getArchiveRequests: tape pool not found");
}

void OStoreDB::queue(const cta::RetrieveToFileRequest& rqst) {
  assertAgentSet();
  // In order to post the job, construct it first.
  objectstore::RetrieveToFileRequest rtfr(m_agent->nextId("RetrieveToFileRequest"), m_objectStore);
  rtfr.initialize();
  rtfr.setArchiveFile(rqst.getArchiveFile());
  rtfr.setRemoteFile(rqst.getRemoteFile());
  rtfr.setPriority(rqst.priority);
  rtfr.setCreationLog(rqst.creationLog);
  // We will need to identity tapes is order to construct the request
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto & tc = rqst.getTapeCopies();
  auto numberOfCopies = tc.size();
  srand (time(NULL));
  auto chosenCopyNumber = rand() % numberOfCopies;
  auto it = tc.begin();
  std::advance(it, chosenCopyNumber);
  auto tapePools = re.dumpTapePools();
  std::string tapeAddress = "";
  for(auto pool=tapePools.begin(); pool!=tapePools.end(); pool++) {
    objectstore::TapePool tp(pool->address, m_objectStore);
    auto tapes = tp.dumpTapes();
    for(auto tape=tapes.begin(); tape!=tapes.end(); tape++) {
      if(tape->vid==it->vid) {
        tapeAddress = tape->address;
        break;
      }
    }
  }
  rtfr.addJob(chosenCopyNumber, it->vid, tapeAddress);
  auto jl = rtfr.dumpJobs();
  if (!jl.size()) {
    throw RetrieveRequestHasNoCopies("In OStoreDB::queue: the retrieve to file request has no copies");
  }
  // We successfully prepared the object. Time to create it and plug it to
  // the tree.
  rtfr.setOwner(m_agent->getAddressIfSet());
  rtfr.insert();
  ScopedExclusiveLock rtfrl(rtfr);
  // We can now plug the request onto its tape
  for (auto j=jl.begin(); j!=jl.end(); j++) {
    objectstore::Tape tp(j->tapeAddress, m_objectStore);
    ScopedExclusiveLock tpl(tp);
    tp.fetch();
    tp.addJob(*j, rtfr.getAddressIfSet(), rtfr.getSize(), rqst.priority, time(NULL));
    tp.commit();
  }
  // The request is now fully set. As it's multi-owned, we do not set the owner
  rtfr.setOwner("");
  rtfr.commit();
  // And remove reference from the agent
  {
    objectstore::ScopedExclusiveLock al(*m_agent);
    m_agent->fetch();
    m_agent->removeFromOwnership(rtfr.getAddressIfSet());
    m_agent->commit();
  }
}

void OStoreDB::queue(const RetrieveToDirRequest& rqst) {
  auto & retrieveToFileRequests = rqst.getRetrieveToFileRequests();
  for(auto req=retrieveToFileRequests.begin(); req!=retrieveToFileRequests.end(); req++) {
    queue(*req);
  }
}

std::list<RetrieveFromTapeCopyRequest> OStoreDB::getRetrieveRequests(const std::string& vid) const {
  throw exception::Exception("Not Implemented");
}

std::map<cta::Tape, std::list<RetrieveFromTapeCopyRequest> > OStoreDB::getRetrieveRequests() const {
  throw exception::Exception("Not Implemented");
}

void OStoreDB::deleteRetrieveRequest(const SecurityIdentity& requester, 
  const std::string& remoteFile) {
  throw exception::Exception("Not Implemented");
  }

  OStoreDB::TapeMountDecisionInfo::~TapeMountDecisionInfo() {
 }


    
}

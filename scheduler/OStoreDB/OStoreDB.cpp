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
#include "scheduler/SecurityIdentity.hpp"
#include "objectstore/RootEntry.hpp"
#include "common/exception/Exception.hpp"
#include "scheduler/AdminHost.hpp"
#include "scheduler/AdminUser.hpp"
#include "scheduler/ArchivalRoute.hpp"
#include "scheduler/LogicalLibrary.hpp"
#include "scheduler/StorageClass.hpp"
#include "scheduler/TapePool.hpp"
#include <algorithm>

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



void OStoreDB::createAdminHost(const SecurityIdentity& requester,
    const std::string& hostName, const std::string& comment) {
  RootEntry re(m_objectStore);
  objectstore::CreationLog cl(requester.getUser(), requester.getHost(), 
    time(NULL), comment);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.addAdminHost(hostName, cl);
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
        cta::UserIdentity(h->log.user.uid, h->log.user.gid),
        h->log.comment, h->log.time));
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
        cta::UserIdentity(au->log.user.uid, au->log.user.gid),
        au->log.comment,
        au->log.time
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

void OStoreDB::createArchivalRoute(const std::string& storageClassName,
  const uint16_t copyNb, const std::string& tapePoolName,
  const cta::CreationLog& creationLog) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.addArchivalRoute(storageClassName, copyNb, tapePoolName, 
    objectstore::CreationLog(creationLog));
  re.commit();
}

std::list<ArchivalRoute> 
  OStoreDB::getArchivalRoutes(const std::string& storageClassName) const {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  auto scd = re.dumpStorageClass(storageClassName);
  rel.release();
  std::list<ArchivalRoute> ret;
  for (auto r=scd.routes.begin(); r!=scd.routes.end(); r++) {
    ret.push_back(ArchivalRoute(storageClassName, 
      r->copyNumber, r->tapePool, r->log));
  }
  return ret;
}

std::list<ArchivalRoute> OStoreDB::getArchivalRoutes() const {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  auto scd = re.dumpStorageClasses();
  rel.release();
  std::list<ArchivalRoute> ret;
  for (auto sc=scd.begin(); sc!=scd.end(); sc++) {
    for (auto r=sc->routes.begin(); r!=sc->routes.end(); r++) {
      ret.push_back(ArchivalRoute(sc->storageClass, 
        r->copyNumber, r->tapePool, r->log));
    }
  }
  return ret;
}

void OStoreDB::deleteArchivalRoute(const SecurityIdentity& requester, 
  const std::string& storageClassName, const uint16_t copyNb) {
  RootEntry re(m_objectStore);
  ScopedExclusiveLock rel(re);
  re.fetch();
  re.removeArchivalRoute(storageClassName, copyNb);
  re.commit();
}

void OStoreDB::fileEntryCreatedInNS(const SecurityIdentity& requester,
  const std::string &archiveFile) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
} 

void OStoreDB::fileEntryDeletedFromNS(const SecurityIdentity& requester,
  const std::string &archiveFile) {
  throw exception::Exception(std::string(__FUNCTION__) + " not implemented");
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

std::list<TapePool> OStoreDB::getTapePools() const {
  RootEntry re(m_objectStore);
  ScopedSharedLock rel(re);
  re.fetch();
  auto tpd = re.dumpTapePools();
  rel.release();
  std::list<TapePool> ret;
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

void OStoreDB::createTape(const SecurityIdentity& requester, 
  const std::string& vid, const std::string& logicalLibraryName, 
  const std::string& tapePoolName, const uint64_t capacityInBytes, 
  const std::string& comment) {
  throw exception::Exception("Not Implemented");
}

std::list<Tape> OStoreDB::getTapes() const {
  throw exception::Exception("Not Implemented");
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
  re.fetch();
  re.removeLibrary(name);
  re.commit();
}

void OStoreDB::queue(const ArchiveToFileRequest& rqst) {
  throw exception::Exception("Not Implemented");
}

void OStoreDB::queue(const ArchiveToDirRequest& rqst) {
  throw exception::Exception("Not Implemented");
}

void OStoreDB::deleteArchiveRequest(const SecurityIdentity& requester, 
  const std::string& archiveFile) {
  throw exception::Exception("Not Implemented");
}

void OStoreDB::markArchiveRequestForDeletion(const SecurityIdentity& requester,
  const std::string& archiveFile) {
  throw exception::Exception("Not Implemented");
}

std::map<TapePool, std::list<ArchiveToTapeCopyRequest> >
  OStoreDB::getArchiveRequests() const {
  throw exception::Exception("Not Implemented");
}

std::list<ArchiveToTapeCopyRequest>
  OStoreDB::getArchiveRequests(const std::string& tapePoolName) const {
  throw exception::Exception("Not Implemented");
}

void OStoreDB::queue(const RetrieveToFileRequest& rqst) {
  throw exception::Exception("Not Implemented");
}

void OStoreDB::queue(const RetrieveToDirRequest& rqst) {
  throw exception::Exception("Not Implemented");
}

std::list<RetrieveFromTapeCopyRequest> OStoreDB::getRetrieveRequests(const std::string& vid) const {
  throw exception::Exception("Not Implemented");
}

std::map<Tape, std::list<RetrieveFromTapeCopyRequest> > OStoreDB::getRetrieveRequests() const {
  throw exception::Exception("Not Implemented");
}

void OStoreDB::deleteRetrieveRequest(const SecurityIdentity& requester, 
  const std::string& remoteFile) {
  throw exception::Exception("Not Implemented");
}


    
}

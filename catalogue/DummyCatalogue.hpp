/*
 * The CERN Tape Archive(CTA) project
 * Copyright(C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
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

#include "Catalogue.hpp"

namespace cta {

namespace catalogue {

/**
 * An empty implementation of the Catalogue used to populate unit tests of the scheduler database
 * as they need a reference to a Catalogue, used in very few situations (requeueing of retrieve
 * requests).
 */
class DummyCatalogue: public Catalogue {
public:
  DummyCatalogue() {}
  virtual ~DummyCatalogue() { }

  void createAdminUser(const common::dataStructures::SecurityIdentity& admin, const std::string& username, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void createArchiveRoute(const common::dataStructures::SecurityIdentity& admin, const std::string& diskInstanceName, const std::string& storageClassName, const uint32_t copyNb, const std::string& tapePoolName, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void createLogicalLibrary(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const bool isDisabled, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void createMountPolicy(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const uint64_t archivePriority, const uint64_t minArchiveRequestAge, const uint64_t retrievePriority, const uint64_t minRetrieveRequestAge, const uint64_t maxDrivesAllowed, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void createRequesterGroupMountRule(const common::dataStructures::SecurityIdentity& admin, const std::string& mountPolicyName, const std::string& diskInstanceName, const std::string& requesterGroupName, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void createRequesterMountRule(const common::dataStructures::SecurityIdentity& admin, const std::string& mountPolicyName, const std::string& diskInstance, const std::string& requesterName, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void createStorageClass(const common::dataStructures::SecurityIdentity& admin, const common::dataStructures::StorageClass& storageClass) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void createTape(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const std::string &mediaType, const std::string &vendor, const std::string& logicalLibraryName, const std::string& tapePoolName, const uint64_t capacityInBytes, const bool disabled, const bool full, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void createTapePool(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const std::string & vo, const uint64_t nbPartialTapes, const bool encryptionValue, const cta::optional<std::string> &supply, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteAdminUser(const std::string& username) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteArchiveFile(const std::string& instanceName, const uint64_t archiveFileId, log::LogContext &lc) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteArchiveRoute(const std::string& diskInstanceName, const std::string& storageClassName, const uint32_t copyNb) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteLogicalLibrary(const std::string& name) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteMountPolicy(const std::string& name) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteRequesterGroupMountRule(const std::string& diskInstanceName, const std::string& requesterGroupName) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteRequesterMountRule(const std::string& diskInstanceName, const std::string& requesterName) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteStorageClass(const std::string& diskInstanceName, const std::string& storageClassName) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteTape(const std::string& vid) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void deleteTapePool(const std::string& name) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void filesWrittenToTape(const std::set<TapeItemWrittenPointer>& event) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::AdminUser> getAdminUsers() const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  common::dataStructures::ArchiveFile getArchiveFileById(const uint64_t id) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  ArchiveFileItor getArchiveFilesItor(const TapeFileSearchCriteria& searchCriteria) const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::ArchiveFile> getFilesForRepack(const std::string &vid, const uint64_t startFSeq, const uint64_t maxNbFiles) const override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  ArchiveFileItor getArchiveFilesForRepackItor(const std::string &vid, const uint64_t startFSeq) const override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::ArchiveRoute> getArchiveRoutes() const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::LogicalLibrary> getLogicalLibraries() const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::MountPolicy> getMountPolicies() const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::RequesterGroupMountRule> getRequesterGroupMountRules() const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::RequesterMountRule> getRequesterMountRules() const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::StorageClass> getStorageClasses() const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  common::dataStructures::ArchiveFileSummary getTapeFileSummary(const TapeFileSearchCriteria& searchCriteria) const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<TapePool> getTapePools() const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<common::dataStructures::Tape> getTapes(const TapeSearchCriteria& searchCriteria) const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
 // getTapesByVid is implemented below (and works).
  common::dataStructures::VidToTapeMap getAllTapes() const override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  std::list<TapeForWriting> getTapesForWriting(const std::string& logicalLibraryName) const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  bool isAdmin(const common::dataStructures::SecurityIdentity& admin) const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyAdminUserComment(const common::dataStructures::SecurityIdentity& admin, const std::string& username, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyArchiveRouteComment(const common::dataStructures::SecurityIdentity& admin, const std::string& instanceName, const std::string& storageClassName, const uint32_t copyNb, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyArchiveRouteTapePoolName(const common::dataStructures::SecurityIdentity& admin, const std::string& instanceName, const std::string& storageClassName, const uint32_t copyNb, const std::string& tapePoolName) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyLogicalLibraryComment(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void setLogicalLibraryDisabled(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const bool disabledValue) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyMountPolicyArchiveMinRequestAge(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const uint64_t minArchiveRequestAge) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyMountPolicyArchivePriority(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const uint64_t archivePriority) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyMountPolicyComment(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyMountPolicyMaxDrivesAllowed(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const uint64_t maxDrivesAllowed) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyMountPolicyRetrieveMinRequestAge(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const uint64_t minRetrieveRequestAge) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyMountPolicyRetrievePriority(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const uint64_t retrievePriority) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyRequesteMountRuleComment(const common::dataStructures::SecurityIdentity& admin, const std::string& instanceName, const std::string& requesterName, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyRequesterGroupMountRuleComment(const common::dataStructures::SecurityIdentity& admin, const std::string& instanceName, const std::string& requesterGroupName, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyRequesterGroupMountRulePolicy(const common::dataStructures::SecurityIdentity& admin, const std::string& instanceName, const std::string& requesterGroupName, const std::string& mountPolicy) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyRequesterMountRulePolicy(const common::dataStructures::SecurityIdentity& admin, const std::string& instanceName, const std::string& requesterName, const std::string& mountPolicy) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyStorageClassComment(const common::dataStructures::SecurityIdentity& admin, const std::string& instanceName, const std::string& name, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyStorageClassNbCopies(const common::dataStructures::SecurityIdentity& admin, const std::string& instanceName, const std::string& name, const uint64_t nbCopies) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapeCapacityInBytes(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const uint64_t capacityInBytes) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapeComment(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapeEncryptionKey(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const std::string& encryptionKey) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapeMediaType(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const std::string& mediaType) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapeVendor(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const std::string& vendor) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapeLogicalLibraryName(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const std::string& logicalLibraryName) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapePoolComment(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const std::string& comment) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapePoolVo(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &vo) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapePoolNbPartialTapes(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const uint64_t nbPartialTapes) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapePoolSupply(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const std::string& supply) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void modifyTapeTapePoolName(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const std::string& tapePoolName) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void noSpaceLeftOnTape(const std::string& vid) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void ping() override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  uint64_t checkAndGetNextArchiveFileId(const std::string &diskInstanceName, const std::string &storageClassName, const common::dataStructures::UserIdentity &user) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  common::dataStructures::ArchiveFileQueueCriteria getArchiveFileQueueCriteria(const std::string &diskInstanceName,
    const std::string &storageClassName, const common::dataStructures::UserIdentity &user) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  common::dataStructures::RetrieveFileQueueCriteria prepareToRetrieveFile(const std::string& instanceName, const uint64_t archiveFileId, const common::dataStructures::UserIdentity& user, log::LogContext &lc) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void reclaimTape(const common::dataStructures::SecurityIdentity& admin, const std::string& vid) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void fakeReclaimTapeForTests(const common::dataStructures::SecurityIdentity& admin, const std::string& vid) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void setTapeDisabled(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const bool disabledValue) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void setTapeFull(const common::dataStructures::SecurityIdentity& admin, const std::string& vid, const bool fullValue) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void setTapePoolEncryption(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const bool encryptionValue) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  bool tapeExists(const std::string& vid) const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void tapeLabelled(const std::string& vid, const std::string& drive) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void tapeMountedForArchive(const std::string& vid, const std::string& drive) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  void tapeMountedForRetrieve(const std::string& vid, const std::string& drive) override { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  bool tapePoolExists(const std::string& tapePoolName) const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }
  bool existNonSupersededFilesAfterFSeqAndDeleteTapeFilesForWriting(const std::string& vid, const uint64_t fSeq) const { throw exception::Exception(std::string("In ")+__PRETTY_FUNCTION__+": not implemented"); }

  // Special functions for unit tests.
  void addEnabledTape(const std::string & vid) {
    threading::MutexLocker lm(m_tapeEnablingMutex);
    m_tapeEnabling[vid]=true;
  }
  void addDisabledTape(const std::string & vid) {
    threading::MutexLocker lm(m_tapeEnablingMutex);
    m_tapeEnabling[vid]=false;
  }
  common::dataStructures::VidToTapeMap getTapesByVid(const std::set<std::string>& vids) const {
    // Minimal implementation of VidToMap for retrieve request unit tests. We just support
    // disabled status for the tapes.
    // If the tape is not listed, it is listed as enabled in the return value.
    threading::MutexLocker lm(m_tapeEnablingMutex);
    common::dataStructures::VidToTapeMap ret;
    for (const auto & v: vids) {
      try {
        ret[v].disabled = !m_tapeEnabling.at(v);
      } catch (std::out_of_range &) {
        ret[v].disabled = false;
      }
    }
    return ret;
  }
private:
  mutable threading::Mutex m_tapeEnablingMutex;
  std::map<std::string, bool> m_tapeEnabling;
};

}} // namespace cta::catalogue.

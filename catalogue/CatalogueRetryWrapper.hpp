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

#include "catalogue/Catalogue.hpp"
#include "catalogue/retryOnLostConnection.hpp"

#include <memory>

namespace cta {

namespace catalogue {

/**
 * Wrapper around a CTA catalogue object that retries a method if a
 * LostConnectionException is thrown.
 */
class CatalogueRetryWrapper: public Catalogue {
public:
  /**
   * Constructor.
   *
   * @param log Object representing the API to the CTA logging system.
   * @param catalogue The catalogue to be wrapped.
   * @param maxTriesToConnect The maximum number of times a single method should
   * try to connect to the database in the event of LostDatabaseConnection
   * exceptions being thrown.
   */
  CatalogueRetryWrapper(log::Logger &log, std::unique_ptr<Catalogue> catalogue, const uint32_t maxTriesToConnect = 3):
    m_log(log),
    m_catalogue(std::move(catalogue)),
    m_maxTriesToConnect(maxTriesToConnect) {
  }

  /**
   * Deletion of the copy constructor.
   */
  CatalogueRetryWrapper(CatalogueRetryWrapper &) = delete;

  /**
   * Destructor.
   */
  ~CatalogueRetryWrapper() override = default;

  /**
   * Deletion of the copy assignment operator.
   */
  CatalogueRetryWrapper &operator=(const CatalogueRetryWrapper &) = delete;

  void tapeLabelled(const std::string &vid, const std::string &drive) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->tapeLabelled(vid, drive);}, m_maxTriesToConnect);
  }

  uint64_t checkAndGetNextArchiveFileId(const std::string &diskInstanceName, const std::string &storageClassName, const common::dataStructures::RequesterIdentity &user) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->checkAndGetNextArchiveFileId(diskInstanceName, storageClassName, user);}, m_maxTriesToConnect);
  }

  common::dataStructures::ArchiveFileQueueCriteria getArchiveFileQueueCriteria(const std::string &diskInstanceName,
    const std::string &storageClassName, const common::dataStructures::RequesterIdentity &user) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getArchiveFileQueueCriteria(diskInstanceName, storageClassName, user);}, m_maxTriesToConnect);
  }

  std::list<TapeForWriting> getTapesForWriting(const std::string &logicalLibraryName) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getTapesForWriting(logicalLibraryName);}, m_maxTriesToConnect);
  }

  void filesWrittenToTape(const std::set<TapeItemWrittenPointer> &event) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->filesWrittenToTape(event);}, m_maxTriesToConnect);
  }

  void tapeMountedForArchive(const std::string &vid, const std::string &drive) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->tapeMountedForArchive(vid, drive);}, m_maxTriesToConnect);
  }

  common::dataStructures::RetrieveFileQueueCriteria prepareToRetrieveFile(const std::string& diskInstanceName, const uint64_t archiveFileId, const common::dataStructures::RequesterIdentity& user, const optional<std::string>& activity, log::LogContext& lc) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->prepareToRetrieveFile(diskInstanceName, archiveFileId, user, activity, lc);}, m_maxTriesToConnect);
  }

  void tapeMountedForRetrieve(const std::string &vid, const std::string &drive) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->tapeMountedForRetrieve(vid, drive);}, m_maxTriesToConnect);
  }

  void noSpaceLeftOnTape(const std::string &vid) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->noSpaceLeftOnTape(vid);}, m_maxTriesToConnect);
  }

  void createAdminUser(const common::dataStructures::SecurityIdentity &admin, const std::string &username, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createAdminUser(admin, username, comment);}, m_maxTriesToConnect);
  }

  void deleteAdminUser(const std::string &username) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteAdminUser(username);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::AdminUser> getAdminUsers() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getAdminUsers();}, m_maxTriesToConnect);
  }

  void modifyAdminUserComment(const common::dataStructures::SecurityIdentity &admin, const std::string &username, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyAdminUserComment(admin, username, comment);}, m_maxTriesToConnect);
  }
  
  void createVirtualOrganization(const common::dataStructures::SecurityIdentity &admin, const common::dataStructures::VirtualOrganization &vo) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createVirtualOrganization(admin, vo);}, m_maxTriesToConnect);
  }
  
  void deleteVirtualOrganization(const std::string &voName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteVirtualOrganization(voName);}, m_maxTriesToConnect);
  }
  
  std::list<common::dataStructures::VirtualOrganization> getVirtualOrganizations() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getVirtualOrganizations();}, m_maxTriesToConnect);
  }
  
  void modifyVirtualOrganizationName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentVoName, const std::string &newVoName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyVirtualOrganizationName(admin,currentVoName,newVoName);}, m_maxTriesToConnect);
  }
  
  void modifyVirtualOrganizationComment(const common::dataStructures::SecurityIdentity &admin, const std::string &voName, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyVirtualOrganizationComment(admin,voName,comment);}, m_maxTriesToConnect);
  }
    
  void createStorageClass(const common::dataStructures::SecurityIdentity &admin, const common::dataStructures::StorageClass &storageClass) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createStorageClass(admin, storageClass);}, m_maxTriesToConnect);
  }

  void deleteStorageClass(const std::string &storageClassName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteStorageClass(storageClassName);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::StorageClass> getStorageClasses() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getStorageClasses();}, m_maxTriesToConnect);
  }

  void modifyStorageClassNbCopies(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t nbCopies) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyStorageClassNbCopies(admin, name, nbCopies);}, m_maxTriesToConnect);
  }

  void modifyStorageClassComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyStorageClassComment(admin, name, comment);}, m_maxTriesToConnect);
  }

  void modifyStorageClassName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentName, const std::string &newName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyStorageClassName(admin, currentName, newName);}, m_maxTriesToConnect);
  }
  
  void modifyStorageClassVo(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &vo) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyStorageClassVo(admin, name, vo);}, m_maxTriesToConnect);
  }

  void createTapePool(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &vo, const uint64_t nbPartialTapes, const bool encryptionValue, const cta::optional<std::string> &supply, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createTapePool(admin, name, vo, nbPartialTapes, encryptionValue, supply, comment);}, m_maxTriesToConnect);
  }

  void deleteTapePool(const std::string &name) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteTapePool(name);}, m_maxTriesToConnect);
  }

  std::list<TapePool> getTapePools() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getTapePools();}, m_maxTriesToConnect);
  }

  void modifyTapePoolVo(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &vo) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapePoolVo(admin, name, vo);}, m_maxTriesToConnect);
  }

  void modifyTapePoolNbPartialTapes(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t nbPartialTapes) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapePoolNbPartialTapes(admin, name, nbPartialTapes);}, m_maxTriesToConnect);
  }

  void modifyTapePoolComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapePoolComment(admin, name, comment);}, m_maxTriesToConnect);
  }

  void setTapePoolEncryption(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const bool encryptionValue) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->setTapePoolEncryption(admin, name, encryptionValue);}, m_maxTriesToConnect);
  }

  void modifyTapePoolSupply(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &supply) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapePoolSupply(admin, name, supply);}, m_maxTriesToConnect);
  }

  void modifyTapePoolName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentName, const std::string &newName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapePoolName(admin, currentName, newName);}, m_maxTriesToConnect);
  }

  void createArchiveRoute(const common::dataStructures::SecurityIdentity &admin, const std::string &storageClassName, const uint32_t copyNb, const std::string &tapePoolName, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createArchiveRoute(admin, storageClassName, copyNb, tapePoolName, comment);}, m_maxTriesToConnect);
  }

  void deleteArchiveRoute(const std::string &storageClassName, const uint32_t copyNb) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteArchiveRoute(storageClassName, copyNb);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::ArchiveRoute> getArchiveRoutes() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getArchiveRoutes();}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::ArchiveRoute> getArchiveRoutes(const std::string &storageClassName, const std::string &tapePoolName) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getArchiveRoutes(storageClassName, tapePoolName);}, m_maxTriesToConnect);
  }

  void modifyArchiveRouteTapePoolName(const common::dataStructures::SecurityIdentity &admin, const std::string &storageClassName, const uint32_t copyNb, const std::string &tapePoolName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyArchiveRouteTapePoolName(admin, storageClassName, copyNb, tapePoolName);}, m_maxTriesToConnect);
  }

  void modifyArchiveRouteComment(const common::dataStructures::SecurityIdentity &admin, const std::string &storageClassName, const uint32_t copyNb, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyArchiveRouteComment(admin, storageClassName, copyNb, comment);}, m_maxTriesToConnect);
  }

  void createLogicalLibrary(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const bool isDisabled, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createLogicalLibrary(admin, name, isDisabled, comment);}, m_maxTriesToConnect);
  }

  void deleteLogicalLibrary(const std::string &name) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteLogicalLibrary(name);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::LogicalLibrary> getLogicalLibraries() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getLogicalLibraries();}, m_maxTriesToConnect);
  }

  void modifyLogicalLibraryName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentName, const std::string &newName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyLogicalLibraryName(admin, currentName, newName);}, m_maxTriesToConnect);
  }

  void modifyLogicalLibraryComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyLogicalLibraryComment(admin, name, comment);}, m_maxTriesToConnect);
  }

  void setLogicalLibraryDisabled(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const bool disabledValue) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->setLogicalLibraryDisabled(admin, name, disabledValue);}, m_maxTriesToConnect);
  }

  void createTape(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &mediaType, const std::string &vendor, const std::string &logicalLibraryName, const std::string &tapePoolName, const uint64_t capacityInBytes, const bool disabled, const bool full, const bool readOnly, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createTape(admin, vid, mediaType, vendor, logicalLibraryName, tapePoolName, capacityInBytes, disabled, full, readOnly, comment);}, m_maxTriesToConnect);
  }

  void deleteTape(const std::string &vid) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteTape(vid);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::Tape> getTapes(const TapeSearchCriteria &searchCriteria) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getTapes(searchCriteria);}, m_maxTriesToConnect);
  }

  common::dataStructures::VidToTapeMap getTapesByVid(const std::set<std::string> &vids) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getTapesByVid(vids);}, m_maxTriesToConnect);
  }

  common::dataStructures::VidToTapeMap getAllTapes() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getAllTapes();}, m_maxTriesToConnect);
  }

  void reclaimTape(const common::dataStructures::SecurityIdentity &admin, const std::string &vid) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->reclaimTape(admin, vid);}, m_maxTriesToConnect);
  }

  void checkTapeForLabel(const std::string &vid) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->checkTapeForLabel(vid);}, m_maxTriesToConnect);
  }
  
  uint64_t getNbFilesOnTape(const std::string &vid) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getNbFilesOnTape(vid);}, m_maxTriesToConnect);
  }
  
  void modifyTapeMediaType(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &mediaType) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapeMediaType(admin, vid, mediaType);}, m_maxTriesToConnect);
  }

  void modifyTapeVendor(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &vendor) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapeVendor(admin, vid, vendor);}, m_maxTriesToConnect);
  }

  void modifyTapeLogicalLibraryName(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &logicalLibraryName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapeLogicalLibraryName(admin, vid, logicalLibraryName);}, m_maxTriesToConnect);
  }

  void modifyTapeTapePoolName(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &tapePoolName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapeTapePoolName(admin, vid, tapePoolName);}, m_maxTriesToConnect);
  }

  void modifyTapeCapacityInBytes(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const uint64_t capacityInBytes) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapeCapacityInBytes(admin, vid, capacityInBytes);}, m_maxTriesToConnect);
  }

  void modifyTapeEncryptionKeyName(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &encryptionKeyName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapeEncryptionKeyName(admin, vid, encryptionKeyName);}, m_maxTriesToConnect);
  }

  void setTapeFull(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const bool fullValue) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->setTapeFull(admin, vid, fullValue);}, m_maxTriesToConnect);
  }
  
  void setTapeReadOnly(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const bool readOnlyValue) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->setTapeReadOnly(admin, vid, readOnlyValue);}, m_maxTriesToConnect);
  }
  
  void setTapeReadOnlyOnError(const std::string &vid) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->setTapeReadOnlyOnError(vid);}, m_maxTriesToConnect);
  }

  void setTapeIsFromCastorInUnitTests(const std::string &vid) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->setTapeIsFromCastorInUnitTests(vid);}, m_maxTriesToConnect);
  }
  
  void setTapeDisabled(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const bool disabledValue) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->setTapeDisabled(admin, vid, disabledValue);}, m_maxTriesToConnect);
  }
  
  void setTapeDirty(const std::string & vid) override {
    return retryOnLostConnection(m_log,[&]{ return m_catalogue->setTapeDirty(vid);}, m_maxTriesToConnect);
  }

  void modifyTapeComment(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyTapeComment(admin, vid, comment);}, m_maxTriesToConnect);
  }

  void modifyRequesterMountRulePolicy(const common::dataStructures::SecurityIdentity &admin, const std::string &instanceName, const std::string &requesterName, const std::string &mountPolicy) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyRequesterMountRulePolicy(admin, instanceName, requesterName, mountPolicy);}, m_maxTriesToConnect);
  }

  void modifyRequesteMountRuleComment(const common::dataStructures::SecurityIdentity &admin, const std::string &instanceName, const std::string &requesterName, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyRequesteMountRuleComment(admin, instanceName, requesterName, comment);}, m_maxTriesToConnect);
  }

  void modifyRequesterGroupMountRulePolicy(const common::dataStructures::SecurityIdentity &admin, const std::string &instanceName, const std::string &requesterGroupName, const std::string &mountPolicy) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyRequesterGroupMountRulePolicy(admin, instanceName, requesterGroupName, mountPolicy);}, m_maxTriesToConnect);
  }

  void modifyRequesterGroupMountRuleComment(const common::dataStructures::SecurityIdentity &admin, const std::string &instanceName, const std::string &requesterGroupName, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyRequesterGroupMountRuleComment(admin, instanceName, requesterGroupName, comment);}, m_maxTriesToConnect);
  }

  void createMountPolicy(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t archivePriority, const uint64_t minArchiveRequestAge, const uint64_t retrievePriority, const uint64_t minRetrieveRequestAge, const uint64_t maxDrivesAllowed, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createMountPolicy(admin, name, archivePriority, minArchiveRequestAge, retrievePriority, minRetrieveRequestAge, maxDrivesAllowed, comment);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::MountPolicy> getMountPolicies() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getMountPolicies();}, m_maxTriesToConnect);
  }

  void deleteMountPolicy(const std::string &name) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteMountPolicy(name);}, m_maxTriesToConnect);
  }

  void createRequesterMountRule(const common::dataStructures::SecurityIdentity &admin, const std::string &mountPolicyName, const std::string &diskInstance, const std::string &requesterName, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createRequesterMountRule(admin, mountPolicyName, diskInstance, requesterName, comment);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::RequesterMountRule> getRequesterMountRules() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getRequesterMountRules();}, m_maxTriesToConnect);
  }

  void deleteRequesterMountRule(const std::string &diskInstanceName, const std::string &requesterName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteRequesterMountRule(diskInstanceName, requesterName);}, m_maxTriesToConnect);
  }

  void createRequesterGroupMountRule(const common::dataStructures::SecurityIdentity &admin, const std::string &mountPolicyName, const std::string &diskInstanceName, const std::string &requesterGroupName, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createRequesterGroupMountRule(admin, mountPolicyName, diskInstanceName, requesterGroupName, comment);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::RequesterGroupMountRule> getRequesterGroupMountRules() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getRequesterGroupMountRules();}, m_maxTriesToConnect);
  }

  void deleteRequesterGroupMountRule(const std::string &diskInstanceName, const std::string &requesterGroupName) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteRequesterGroupMountRule(diskInstanceName, requesterGroupName);}, m_maxTriesToConnect);
  }

  void modifyMountPolicyArchivePriority(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t archivePriority) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyMountPolicyArchivePriority(admin, name, archivePriority);}, m_maxTriesToConnect);
  }

  void modifyMountPolicyArchiveMinRequestAge(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t minArchiveRequestAge) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyMountPolicyArchiveMinRequestAge(admin, name, minArchiveRequestAge);}, m_maxTriesToConnect);
  }

  void modifyMountPolicyRetrievePriority(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t retrievePriority) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyMountPolicyRetrievePriority(admin, name, retrievePriority);}, m_maxTriesToConnect);
  }

  void modifyMountPolicyRetrieveMinRequestAge(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t minRetrieveRequestAge) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyMountPolicyRetrieveMinRequestAge(admin, name, minRetrieveRequestAge);}, m_maxTriesToConnect);
  }

  void modifyMountPolicyMaxDrivesAllowed(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t maxDrivesAllowed) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyMountPolicyMaxDrivesAllowed(admin, name, maxDrivesAllowed);}, m_maxTriesToConnect);
  }

  void modifyMountPolicyComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyMountPolicyComment(admin, name, comment);}, m_maxTriesToConnect);
  }
  
  void createActivitiesFairShareWeight(const common::dataStructures::SecurityIdentity& admin, const std::string& diskInstanceName, const std::string& acttivity, double weight, const std::string & comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createActivitiesFairShareWeight(admin, diskInstanceName, acttivity, weight, comment);}, m_maxTriesToConnect);
  }
  
  void modifyActivitiesFairShareWeight(const common::dataStructures::SecurityIdentity& admin, const std::string& diskInstanceName, const std::string& acttivity, double weight, const std::string & comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyActivitiesFairShareWeight(admin, diskInstanceName, acttivity, weight, comment);}, m_maxTriesToConnect);
  }
  
  void deleteActivitiesFairShareWeight(const common::dataStructures::SecurityIdentity& admin, const std::string& diskInstanceName, const std::string& acttivity) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteActivitiesFairShareWeight(admin, diskInstanceName, acttivity);}, m_maxTriesToConnect);
  }
  
  std::list<common::dataStructures::ActivitiesFairShareWeights> getActivitiesFairShareWeights() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getActivitiesFairShareWeights();}, m_maxTriesToConnect);
  }

  disk::DiskSystemList getAllDiskSystems() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getAllDiskSystems();}, m_maxTriesToConnect);
  }
  
  void createDiskSystem(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &fileRegexp, const std::string &freeSpaceQueryURL, const uint64_t refreshInterval, const uint64_t targetedFreeSpace, const uint64_t sleepTime, const std::string &comment)  override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->createDiskSystem(admin, name, fileRegexp, freeSpaceQueryURL, refreshInterval, targetedFreeSpace, sleepTime, comment);}, m_maxTriesToConnect);
  }
  
  void deleteDiskSystem(const std::string &name) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteDiskSystem(name);}, m_maxTriesToConnect);
  }
  
  void modifyDiskSystemFileRegexp(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &fileRegexp) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyDiskSystemFileRegexp(admin, name, fileRegexp);}, m_maxTriesToConnect);
  }

  void modifyDiskSystemFreeSpaceQueryURL(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &freeSpaceQueryURL) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyDiskSystemFreeSpaceQueryURL(admin, name, freeSpaceQueryURL);}, m_maxTriesToConnect);  
  }
  
  void modifyDiskSystemRefreshInterval(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t refreshInterval) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyDiskSystemRefreshInterval(admin, name, refreshInterval);}, m_maxTriesToConnect);      
  }
 
  void modifyDiskSystemTargetedFreeSpace(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t targetedFreeSpace) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyDiskSystemTargetedFreeSpace(admin, name, targetedFreeSpace);}, m_maxTriesToConnect);      
  }
  
  void modifyDiskSystemComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyDiskSystemComment(admin, name, comment);}, m_maxTriesToConnect);
  }
  
  void modifyDiskSystemSleepTime(const common::dataStructures::SecurityIdentity& admin, const std::string& name, const uint64_t sleepTime) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->modifyDiskSystemSleepTime(admin, name, sleepTime);}, m_maxTriesToConnect);
  }
  
  ArchiveFileItor getArchiveFilesItor(const TapeFileSearchCriteria &searchCriteria) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getArchiveFilesItor(searchCriteria);}, m_maxTriesToConnect);
  }

  std::list<common::dataStructures::ArchiveFile> getFilesForRepack(const std::string &vid, const uint64_t startFSeq, const uint64_t maxNbFiles) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getFilesForRepack(vid, startFSeq, maxNbFiles);}, m_maxTriesToConnect);
  }

  ArchiveFileItor getArchiveFilesForRepackItor(const std::string &vid, const uint64_t startFSeq) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getArchiveFilesForRepackItor(vid, startFSeq);}, m_maxTriesToConnect);
  }

  common::dataStructures::ArchiveFileSummary getTapeFileSummary(const TapeFileSearchCriteria &searchCriteria) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getTapeFileSummary(searchCriteria);}, m_maxTriesToConnect);
  }

  common::dataStructures::ArchiveFile getArchiveFileById(const uint64_t id) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getArchiveFileById(id);}, m_maxTriesToConnect);
  }

  void deleteArchiveFile(const std::string &instanceName, const uint64_t archiveFileId, log::LogContext &lc) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->deleteArchiveFile(instanceName, archiveFileId, lc);}, m_maxTriesToConnect);
  }

  bool isAdmin(const common::dataStructures::SecurityIdentity &admin) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->isAdmin(admin);}, m_maxTriesToConnect);
  }

  void ping() override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->ping();}, m_maxTriesToConnect);
  }

  void verifySchemaVersion() override{
    return retryOnLostConnection(m_log, [&]{return m_catalogue->verifySchemaVersion();}, m_maxTriesToConnect);
  }

  SchemaVersion getSchemaVersion() const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->getSchemaVersion();}, m_maxTriesToConnect);
  }

  bool tapePoolExists(const std::string &tapePoolName) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->tapePoolExists(tapePoolName);}, m_maxTriesToConnect);
  }

  bool tapeExists(const std::string &vid) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->tapeExists(vid);}, m_maxTriesToConnect);
  }

  bool diskSystemExists(const std::string &name) const override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->diskSystemExists(name);}, m_maxTriesToConnect);
  } 

  void updateDiskFileId(uint64_t archiveFileId, const std::string &diskInstance, const std::string &diskFileId) override {
    return retryOnLostConnection(m_log, [&]{return m_catalogue->updateDiskFileId(archiveFileId, diskInstance, diskFileId);}, m_maxTriesToConnect);
  }

protected:

  /**
   * Object representing the API to the CTA logging system.
   */
  log::Logger &m_log;

  /**
   * The wrapped catalogue.
   */
  std::unique_ptr<Catalogue> m_catalogue;

  /**
   * The maximum number of times a single method should try to connect to the
   * database in the event of LostDatabaseConnection exceptions being thrown.
   */
  uint32_t m_maxTriesToConnect;

}; // class CatalogueRetryWrapper

} // namespace catalogue
} // namespace cta

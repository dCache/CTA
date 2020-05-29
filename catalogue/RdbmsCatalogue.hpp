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

#include "catalogue/Catalogue.hpp"
#include "catalogue/RequesterAndGroupMountPolicies.hpp"
#include "catalogue/TimeBasedCache.hpp"
#include "common/threading/Mutex.hpp"
#include "rdbms/ConnPool.hpp"
#include "rdbms/Login.hpp"

#include <memory>

namespace cta {
namespace common {
namespace dataStructures {

/**
 * Forward declaration.
 */
class TapeFile;

} // namespace dataStructures
} // namespace catalogue
} // namespace cta

namespace cta {
namespace catalogue {

/**
 * Forward declaration.
 */
class ArchiveFileRow;

/**
 * Forward declaration.
 */
class RdbmsCatalogueGetArchiveFilesItor;

/**
 * CTA catalogue implemented using a relational database backend.
 */
class RdbmsCatalogue: public Catalogue {
protected:

  /**
   * Protected constructor only to be called by sub-classes.
   *
   * @param log Object representing the API to the CTA logging system.
   * @param login The database login details to be used to create new
   * connections.
   * @param nbConns The maximum number of concurrent connections to the
   * underlying relational database for all operations accept listing archive
   * files which can be relatively long operations.
   * @param nbArchiveFileListingConns The maximum number of concurrent
   * connections to the underlying relational database for the sole purpose of
   * listing archive files.
   */
  RdbmsCatalogue(
    log::Logger &log,
    const rdbms::Login &login,
    const uint64_t nbConns,
    const uint64_t nbArchiveFileListingConns);

public:

  /**
   * Destructor.
   */
  ~RdbmsCatalogue() override;

  /////////////////////////////////////////////////////////////////////
  // START OF METHODS DIRECTLY INVOLVED IN DATA TRANSFER AND SCHEDULING
  /////////////////////////////////////////////////////////////////////

  /**
   * Notifies the catalogue that the specified tape was labelled.
   *
   * @param vid The volume identifier of the tape.
   * @param drive The name of tape drive that was used to label the tape.
   */
  void tapeLabelled(const std::string &vid, const std::string &drive) override;

  /**
   * Checks the specified archival could take place and returns a new and
   * unique archive file identifier that can be used by a new archive file
   * within the catalogue.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * storage class belongs.
   * @param storageClassName The name of the storage class of the file to be
   * archived.  The storage class name is only guaranteed to be unique within
   * its disk instance.  The storage class name will be used by the Catalogue
   * to determine the destination tape pool for each tape copy.
   * @param user The user for whom the file is to be archived.  This will be
   * used by the Catalogue to determine the mount policy to be used when
   * archiving the file.
   * @return The new archive file identifier.
   */
  uint64_t checkAndGetNextArchiveFileId(
    const std::string &diskInstanceName,
    const std::string &storageClassName,
    const common::dataStructures::RequesterIdentity &user) override;

  /**
   * Returns the information required to queue an archive request.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * storage class belongs.
   * @param storageClassName The name of the storage class of the file to be
   * archived.  The storage class name is only guaranteed to be unique within
   * its disk instance.  The storage class name will be used by the Catalogue
   * to determine the destination tape pool for each tape copy.
   * @param user The user for whom the file is to be archived.  This will be
   * used by the Catalogue to determine the mount policy to be used when
   * archiving the file.
   * @return The information required to queue an archive request.
   */
  common::dataStructures::ArchiveFileQueueCriteria getArchiveFileQueueCriteria(
    const std::string &diskInstanceName,
    const std::string &storageClassName,
    const common::dataStructures::RequesterIdentity &user) override;

  /**
   * Returns the list of tapes that can be written to by a tape drive in the
   * specified logical library, in other words tapes that are labelled, not
   * disabled, not full, not read-only and are in the specified logical library.
   *
   * @param logicalLibraryName The name of the logical library.
   * @return The list of tapes for writing.
   */
  std::list<TapeForWriting> getTapesForWriting(const std::string &logicalLibraryName) const override;

  /**
   * Notifies the CTA catalogue that the specified tape has been mounted in
   * order to archive files.
   *
   * The purpose of this method is to keep track of which drive mounted a given
   * tape for archiving files last.
   *
   * @param vid The volume identifier of the tape.
   * @param drive The name of the drive where the tape was mounted.
   */
  void tapeMountedForArchive(const std::string &vid, const std::string &drive) override;

  /**
   * Prepares for a file retrieval by returning the information required to
   * queue the associated retrieve request(s).
   *
   * @param diskInstanceName The name of the instance from where the retrieval
   * request originated
   * @param archiveFileId The unique identifier of the archived file that is
   * to be retrieved.
   * @param user The user for whom the file is to be retrieved.  This will be
   * used by the Catalogue to determine the mount policy to be used when
   * retrieving the file.
   * @param activity The activity under which the user wants to start the retrieve
   * The call will fail if the activity is set and unknown. 
   * @param lc The log context.
   *
   * @return The information required to queue the associated retrieve request(s).
   */
  common::dataStructures::RetrieveFileQueueCriteria prepareToRetrieveFile(
    const std::string &diskInstanceName,
    const uint64_t archiveFileId,
    const common::dataStructures::RequesterIdentity &user,
    const optional<std::string> & activity,
    log::LogContext &lc) override;

  /**
   * Notifies the CTA catalogue that the specified tape has been mounted in
   * order to retrieve files.
   *
   * The purpose of this method is to keep track of which drive mounted a given
   * tape for retrieving files last.
   *
   * @param vid The volume identifier of the tape.
   * @param drive The name of the drive where the tape was mounted.
   */
  void tapeMountedForRetrieve(const std::string &vid, const std::string &drive) override;

  /**
   * This method notifies the CTA catalogue that there is no more free space on
   * the specified tape.
   *
   * @param vid The volume identifier of the tape.
   */
  void noSpaceLeftOnTape(const std::string &vid) override;

  ///////////////////////////////////////////////////////////////////
  // END OF METHODS DIRECTLY INVOLVED IN DATA TRANSFER AND SCHEDULING
  ///////////////////////////////////////////////////////////////////

  void createAdminUser(const common::dataStructures::SecurityIdentity &admin, const std::string &username, const std::string &comment) override;
  void deleteAdminUser(const std::string &username) override;
  std::list<common::dataStructures::AdminUser> getAdminUsers() const override;
  void modifyAdminUserComment(const common::dataStructures::SecurityIdentity &admin, const std::string &username, const std::string &comment) override;
  
  /**
   * Creates the specified Virtual Organization
   * @param admin The administrator.
   * @param vo the Virtual Organization
   */
  void createVirtualOrganization(const common::dataStructures::SecurityIdentity &admin, const common::dataStructures::VirtualOrganization &vo) override;
  
  /**
   * Deletes the specified Virtual Organization
   * @param voName the name of the VirtualOrganization to delete
   */
  void deleteVirtualOrganization(const std::string &voName) override;
  
  /**
   * Get all the Virtual Organizations from the Catalogue
   * @return the list of all the Virtual Organizations
   */
  std::list<common::dataStructures::VirtualOrganization> getVirtualOrganizations() const override;
  
  /**
   * Modifies the name of the specified Virtual Organization.
   *
   * @param currentVoName The current name of the Virtual Organization.
   * @param newVoName The new name of the Virtual Organization.
   */
  void modifyVirtualOrganizationName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentVoName, const std::string &newVoName) override;
  
  /**
   * Modifies the comment of the specified Virtual Organization
   *
   * @param voName The name of the Virtual Organization.
   * @param comment The new comment of the Virtual Organization.
   */
  void modifyVirtualOrganizationComment(const common::dataStructures::SecurityIdentity &admin, const std::string &voName, const std::string &comment) override;
  
  /**
   * Creates the specified storage class.
   *
   * @param admin The administrator.
   * @param storageClass The storage class.
   */
  void createStorageClass(
    const common::dataStructures::SecurityIdentity &admin,
    const common::dataStructures::StorageClass &storageClass) override;

  /**
   * Deletes the specified storage class.
   *
   * @param stoargeClassName The name of the storage class which is only
   * guaranteed to be unique within its disk isntance.
   */
  void deleteStorageClass(const std::string &storageClassName) override;

  std::list<common::dataStructures::StorageClass> getStorageClasses() const override;
  void modifyStorageClassNbCopies(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t nbCopies) override;
  void modifyStorageClassComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override;
  void modifyStorageClassVo(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &vo) override;
  /**
   * Modifies the name of the specified storage class.
   *
   * @param currentName The current name of the storage class.
   * @param newName The new name of the storage class.
   */
  void modifyStorageClassName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentName, const std::string &newName) override;

  /**
   * Creates a tape media type.
   *
   * @param mediaType The tape media type.
   */
  void createMediaType(const common::dataStructures::SecurityIdentity &admin, const MediaType &mediaType) override;

  /**
   * Deletes the specified tape media type.
   *
   * @param name The name of the tape media type.
   */
  void deleteMediaType(const std::string &name) override;

  /**
   * Returns all tape media types.
   *
   * @return All tape media types.
   */
  std::list<MediaTypeWithLogs> getMediaTypes() const override;

  /**
   * Modifies the name of the specified tape media type.
   *
   * @param currentName The current name of the tape media type.
   * @param newName The new name of the tape media type.
   */
  void modifyMediaTypeName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentName, const std::string &newName) override;

  /**
   * Modifies the cartidge of the specified tape media type.
   *
   * @param admin The administrator.
   * @param name The name of the tape media type.
   * @param cartridge The new cartidge.
   */
  void modifyMediaTypeCartridge(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &cartridge) override;

  /**
   * Modify the capacity in bytes of a tape media type.
   *
   * @param admin The administrator.
   * @param name The name of the tape media type.
   * @param capacityInBytes The new capacity in bytes.
   */
  void modifyMediaTypeCapacityInBytes(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t capacityInBytes) override;

  /**
   * Modify the SCSI primary density code a tape media type.
   *
   * @param admin The administrator.
   * @param name The name of the tape media type.
   * @param primaryDensityCode The new SCSI primary density code.
   */
  void modifyMediaTypePrimaryDensityCode(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint8_t primaryDensityCode) override;

  /**
   * Modify the SCSI secondary density code a tape media type.
   *
   * @param admin The administrator.
   * @param name The name of the tape media type.
   * @param secondaryDensityCode The new SCSI secondary density code.
   */
  void modifyMediaTypeSecondaryDensityCode(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint8_t secondaryDensityCode) override;

  /**
   * Modify the number of tape wraps of a tape media type.
   *
   * @param admin The administrator.
   * @param name The name of the tape media type.
   * @param nbWraps The new number of tape wraps.
   */
  void modifyMediaTypeNbWraps(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const cta::optional<std::uint32_t> &nbWraps) override;
  
  /**
   * Modify the minimum longitudinal tape position of a tape media type.
   *
   * @param admin The administrator.
   * @param name The name of the tape media type.
   * @param minLPos The new minimum longitudinal tape position.
   */
  void modifyMediaTypeMinLPos(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const cta::optional<std::uint64_t> &minLPos) override;
  
  /**
   * Modify the maximum longitudinal tape position of a tape media type.
   *
   * @param admin The administrator.
   * @param name The name of the tape media type.
   * @param maxLPos The new maximum longitudinal tape position.
   */
  void modifyMediaTypeMaxLPos(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const cta::optional<std::uint64_t> &maxLPos) override;

  /**
   * Modify the comment of a tape media type.
   *
   * @param admin The administrator.
   * @param name The name of the tape media type.
   * @param comment The new comment.
   */
  void modifyMediaTypeComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override;

  void createTapePool(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &vo, const uint64_t nbPartialTapes, const bool encryptionValue, const cta::optional<std::string> &supply, const std::string &comment) override;
  void deleteTapePool(const std::string &name) override;
  std::list<TapePool> getTapePools() const override;
  void modifyTapePoolVo(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &vo) override;
  void modifyTapePoolNbPartialTapes(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t nbPartialTapes) override;
  void modifyTapePoolComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override;
  void setTapePoolEncryption(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const bool encryptionValue) override;
  void modifyTapePoolSupply(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &supply) override;

  /**
   * Modifies the name of the specified tape pool.
   *
   * @param admin The administrator.
   * @param currentName The current name of the tape pool.
   * @param newName The new name of the tape pool.
   */
  void modifyTapePoolName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentName, const std::string &newName) override;

  void createArchiveRoute(
    const common::dataStructures::SecurityIdentity &admin,
    const std::string &storageClassName,
    const uint32_t copyNb,
    const std::string &tapePoolName,
    const std::string &comment) override;


  /**
   * Deletes the specified archive route.
   *
   * @param storageClassName The name of the storage class which is unique
   * @param copyNb The copy number of the tape file.
   */
  void deleteArchiveRoute(
    const std::string &storageClassName, 
    const uint32_t copyNb) override;

  std::list<common::dataStructures::ArchiveRoute> getArchiveRoutes() const override;

  /**
   * @return the archive routes of the given storage class and destination tape
   * pool.
   *
   * Under normal circumstances this method should return either 0 or 1 route.
   * For a given storage class there should be no more than one route to any
   * given tape pool.
   *
   * @param storageClassName The name of the storage class which is unique
   * @param tapePoolName The name of the tape pool.
   */
  std::list<common::dataStructures::ArchiveRoute> getArchiveRoutes(
    const std::string &storageClassName,
    const std::string &tapePoolName) const override;

  void modifyArchiveRouteTapePoolName(const common::dataStructures::SecurityIdentity &admin, const std::string &storageClassName, const uint32_t copyNb, const std::string &tapePoolName) override;
  void modifyArchiveRouteComment(const common::dataStructures::SecurityIdentity &admin, const std::string &storageClassName, const uint32_t copyNb, const std::string &comment) override;

  void createLogicalLibrary(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const bool isDisabled, const std::string &comment) override;
  void deleteLogicalLibrary(const std::string &name) override;
  std::list<common::dataStructures::LogicalLibrary> getLogicalLibraries() const override;

  /**
   * Modifies the name of the specified logical library.
   *
   * @param admin The administrator.
   * @param currentName The current name of the logical library.
   * @param newName The new name of the logical library.
   */
  void modifyLogicalLibraryName(const common::dataStructures::SecurityIdentity &admin, const std::string &currentName, const std::string &newName) override;

  void modifyLogicalLibraryComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override;
  virtual void setLogicalLibraryDisabled(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const bool disabledValue) override;

  /**
   * Creates a tape.
   */
  void createTape(
    const common::dataStructures::SecurityIdentity &admin,
    const std::string &vid,
    const std::string &mediaType,
    const std::string &vendor,
    const std::string &logicalLibraryName,
    const std::string &tapePoolName,
    const uint64_t capacityInBytes,
    const bool disabled,
    const bool full,
    const bool readOnly,
    const std::string &comment) override;

  void deleteTape(const std::string &vid) override;

  /**
   * Returns the list of tapes that meet the specified search criteria.
   *
   * @param searchCriteria The search criteria.
   * @return The list of tapes.
   */
  std::list<common::dataStructures::Tape> getTapes(const TapeSearchCriteria &searchCriteria) const override;

  /**
   * Returns the tapes with the specified volume identifiers.
   *
   * This method will throw an exception if it cannot find ALL of the specified
   * tapes.
   *
   * @param vids The tape volume identifiers (VIDs).
   * @return Map from tape volume identifier to tape.
   */
  common::dataStructures::VidToTapeMap getTapesByVid(const std::set<std::string> &vids) const override;

  /**
   * Returns all the tapes within the CTA catalogue.
   *
   * @return Map from tape volume identifier to tape.
   */
  common::dataStructures::VidToTapeMap getAllTapes() const override;

  /**
   * Reclaims the specified tape.
   *
   * This method will throw an exception if the specified tape does not exist.
   *
   * This method will throw an exception if the specified tape is not FULL.
   *
   * This method will throw an exception if there is still at least one tape
   * file recorded in the cataligue as being on the specified tape.
   *
   * @param admin The administrator.
   * @param vid The volume identifier of the tape to be reclaimed.
   * @param lc the logContext
   */
  void reclaimTape(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, cta::log::LogContext & lc) override;
  
  /**
   * Checks the specified tape for the tape label command.
   *
   * This method checks if the tape is safe to be labeled and will throw an 
   * exception if the specified tape does not ready to be labeled.
   *
   * @param vid The volume identifier of the tape to be checked.
   */
  void checkTapeForLabel(const std::string &vid) override;
  
  /**
   * Returns the number of non superseded files contained in the tape identified by its vid
   * @param conn the database connection
   * @param vid the vid in which we will count non superseded files
   * @return the number of non superseded files on the vid
   */
  uint64_t getNbNonSupersededFilesOnTape(rdbms::Conn &conn, const std::string &vid) const;
  
  /**
   * Returns the number of any files contained in the tape identified by its vid
   * @param vid the vid in which we will count non superseded files
   * @return the number of files on the tape
   */
  uint64_t getNbFilesOnTape(const std::string &vid) const override;
  
  /**
   * Returns the number of any files contained in the tape identified by its vid
   * @param conn the database connection
   * @param vid the vid in which we will count non superseded files
   * @return the number of files on the tape
   */
  uint64_t getNbFilesOnTape(rdbms::Conn &conn, const std::string &vid) const;
  
  /**
   * Delete all the tape files of the VID passed in parameter
   * @param conn the database connection
   * @param vid the vid in which we want to remove all the tape files
   */
  void deleteTapeFiles(rdbms::Conn &conn, const std::string& vid) const;
  
  /**
   * Set the DIRTY flag to true
   * @param conn the database connection
   * @param vid	the vid in which we want to set it as dirty
   */
  void setTapeDirty(rdbms::Conn &conn, const std::string &vid) const;
  
  /**
   * Reset the counters of a tape
   * @param conn the database connection
   * @param admin the administrator
   * @param vid the vid to reset the counters
   */
  void resetTapeCounters(rdbms::Conn &conn, const common::dataStructures::SecurityIdentity &admin ,const std::string& vid) const;
  void modifyTapeMediaType(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &mediaType) override;
  void modifyTapeVendor(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &vendor) override;
  void modifyTapeLogicalLibraryName(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &logicalLibraryName) override;
  void modifyTapeTapePoolName(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &tapePoolName) override;
  void modifyTapeCapacityInBytes(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const uint64_t capacityInBytes) override;
  void modifyTapeEncryptionKeyName(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &encryptionKeyName) override;

  /**
   * Sets the full status of the specified tape.
   *
   * Please note that this method is to be called by the CTA front-end in
   * response to a command from the CTA command-line interface (CLI).
   *
   * @param admin The administrator.
   * @param vid The volume identifier of the tape to be marked as full.
   * @param fullValue Set to true if the tape is full.
   */
  void setTapeFull(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const bool fullValue) override;
  
  /**
   * Sets the read-only status of the specified tape.
   *
   * Please note that this method is to be called by the CTA front-end in
   * response to a command from the CTA command-line interface (CLI).
   *
   * @param admin The administrator.
   * @param vid The volume identifier of the tape to be marked as read-only.
   * @param readOnlyValue Set to true if the tape is read-only.
   */
  void setTapeReadOnly(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const bool readOnlyValue) override;
  
  /**
   * This method notifies the CTA catalogue to set the specified tape read-only
   * in case of a problem.
   *
   * @param vid The volume identifier of the tape.
   */
  void setTapeReadOnlyOnError(const std::string &vid) override;
  
  /**
   * This method notifies the CTA catalogue to set the specified tape is from CASTOR.
   * This method only for unitTests and MUST never be called in CTA!!! 
   *
   * @param vid The volume identifier of the tape.
   */
  void setTapeIsFromCastorInUnitTests(const std::string &vid) override;
  
  void setTapeDisabled(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const bool disabledValue) override;
  void setTapeDirty(const std::string & vid) override;
  void modifyTapeComment(const common::dataStructures::SecurityIdentity &admin, const std::string &vid, const std::string &comment) override;

  void modifyRequesterMountRulePolicy(const common::dataStructures::SecurityIdentity &admin, const std::string &instanceName, const std::string &requesterName, const std::string &mountPolicy) override;
  void modifyRequesteMountRuleComment(const common::dataStructures::SecurityIdentity &admin, const std::string &instanceName, const std::string &requesterName, const std::string &comment) override;
  void modifyRequesterGroupMountRulePolicy(const common::dataStructures::SecurityIdentity &admin, const std::string &instanceName, const std::string &requesterGroupName, const std::string &mountPolicy) override;
  void modifyRequesterGroupMountRuleComment(const common::dataStructures::SecurityIdentity &admin, const std::string &instanceName, const std::string &requesterGroupName, const std::string &comment) override;

  void createMountPolicy(
    const common::dataStructures::SecurityIdentity &admin,
    const std::string &name,
    const uint64_t archivePriority,
    const uint64_t minArchiveRequestAge,
    const uint64_t retrievePriority,
    const uint64_t minRetrieveRequestAge,
    const uint64_t maxDrivesAllowed,
    const std::string &comment) override;

  /**
   * Returns the list of all existing mount policies.
   *
   * @return the list of all existing mount policies.
   */
  std::list<common::dataStructures::MountPolicy> getMountPolicies() const override;

  /**
   * Deletes the specified mount policy.
   *
   * @param name The name of the mount policy.
   */
  void deleteMountPolicy(const std::string &name) override;

  /**
   * Creates the rule that the specified mount policy will be used for the
   * specified requester.
   *
   * Please note that requester mount-rules overrule requester-group
   * mount-rules.
   *
   * @param admin The administrator.
   * @param mountPolicyName The name of the mount policy.
   * @param diskInstance The name of the disk instance to which the requester
   * belongs.
   * @param requesterName The name of the requester which is only guarantted to
   * be unique within its disk instance.
   * @param comment Comment.
   */
  void createRequesterMountRule(
    const common::dataStructures::SecurityIdentity &admin,
    const std::string &mountPolicyName,
    const std::string &diskInstance,
    const std::string &requesterName,
    const std::string &comment) override;

  /**
   * Returns the rules that specify which mount policy is be used for which
   * requester.
   *
   * @return the rules that specify which mount policy is be used for which
   * requester.
   */
  std::list<common::dataStructures::RequesterMountRule> getRequesterMountRules() const override;

  /**
   * Deletes the specified mount rule.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester belongs.
   * @param requesterName The name of the requester which is only guaranteed to
   * be unique within its disk instance.
   */
  void deleteRequesterMountRule(const std::string &diskInstanceName, const std::string &requesterName) override;

  /**
   * Creates the rule that the specified mount policy will be used for the
   * specified requester group.
   *
   * Please note that requester mount-rules overrule requester-group
   * mount-rules.
   *
   * @param admin The administrator.
   * @param mountPolicyName The name of the mount policy.
   * @param diskInstanceName The name of the disk instance to which the
   * requester group belongs.
   * @param requesterGroupName The name of the requester group which is only
   * guarantted to be unique within its disk instance.
   * @param comment Comment.
   */
  void createRequesterGroupMountRule(
    const common::dataStructures::SecurityIdentity &admin,
    const std::string &mountPolicyName,
    const std::string &diskInstanceName,
    const std::string &requesterGroupName,
    const std::string &comment) override;

  /**
   * Returns the rules that specify which mount policy is be used for which
   * requester group.
   *
   * @return the rules that specify which mount policy is be used for which
   * requester group.
   */
  std::list<common::dataStructures::RequesterGroupMountRule> getRequesterGroupMountRules() const override;

  /**
   * Deletes the specified mount rule.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester group belongs.
   * @param requesterGroupName The name of the requester group which is only
   * guaranteed to be unique within its disk instance.
   */
  void deleteRequesterGroupMountRule(
    const std::string &diskInstanceName,
    const std::string &requesterGroupName) override;

  void modifyMountPolicyArchivePriority(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t archivePriority) override;
  void modifyMountPolicyArchiveMinRequestAge(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t minArchiveRequestAge) override;
  void modifyMountPolicyRetrievePriority(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t retrievePriority) override;
  void modifyMountPolicyRetrieveMinRequestAge(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t minRetrieveRequestAge) override;
  void modifyMountPolicyMaxDrivesAllowed(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const uint64_t maxDrivesAllowed) override;
  void modifyMountPolicyComment(const common::dataStructures::SecurityIdentity &admin, const std::string &name, const std::string &comment) override;
  
  void createActivitiesFairShareWeight(const common::dataStructures::SecurityIdentity &admin, const std::string & diskInstanceName, const std::string & activity,
    double weight, const std::string & comment) override;
  void modifyActivitiesFairShareWeight(const common::dataStructures::SecurityIdentity &admin, const std::string & diskInstanceName, const std::string & activity,
    double weight, const std::string & comment) override;
  void deleteActivitiesFairShareWeight(const common::dataStructures::SecurityIdentity &admin, const std::string & diskInstanceName, const std::string & activity) override;
  std::list<common::dataStructures::ActivitiesFairShareWeights> getActivitiesFairShareWeights() const override;

  /**
   * Returns all the disk systems within the CTA catalogue.
   *
   * @return The disk systems.
   * requester group.
   */
  disk::DiskSystemList getAllDiskSystems() const override;
  
  /**
   * Creates a disk system.
   * 
   * @param admin The administrator.
   * @param name The name of the disk system.
   * @param fileRegexp The regular expression allowing matching destination URLs
   * for this disk system.
   * @param freeSpaceQueryURL The query URL that describes a method to query the
   * free space from the disk system.
   * @param refreshInterval The refresh interval (seconds) defining how long do 
   * we use a free space value.
   * @param targetedFreeSpace The targeted free space (margin) based on the free
   * space update latency (inherent to the file system and induced by the refresh 
   * interval), and the expected external bandwidth from sources external to CTA.
   * @param comment Comment.
   */
  void createDiskSystem(
    const common::dataStructures::SecurityIdentity &admin,
    const std::string &name,
    const std::string &fileRegexp,
    const std::string &freeSpaceQueryURL,
    const uint64_t refreshInterval,
    const uint64_t targetedFreeSpace,
    const uint64_t sleepTime,
    const std::string &comment) override;
  
  /**
   * Deletes a disk system.
   * 
   * @param name The name of the disk system.
   */
  void deleteDiskSystem(const std::string &name) override;
   
  void modifyDiskSystemFileRegexp(const common::dataStructures::SecurityIdentity &admin,
    const std::string &name, const std::string &fileRegexp) override;
  void modifyDiskSystemFreeSpaceQueryURL(const common::dataStructures::SecurityIdentity &admin,
    const std::string &name, const std::string &freeSpaceQueryURL) override;
  void modifyDiskSystemRefreshInterval(const common::dataStructures::SecurityIdentity &admin,
    const std::string &name, const uint64_t refreshInterval) override;
  void modifyDiskSystemTargetedFreeSpace(const common::dataStructures::SecurityIdentity &admin,
    const std::string &name, const uint64_t targetedFreeSpace) override;
  void modifyDiskSystemComment(const common::dataStructures::SecurityIdentity &admin,
    const std::string &name, const std::string &comment) override;
  void modifyDiskSystemSleepTime(const common::dataStructures::SecurityIdentity& admin,
    const std::string& name, const uint64_t sleepTime) override;

  
  /**
   * Throws a UserError exception if the specified searchCriteria is not valid
   * due to a user error.
   *
   * @param searchCriteria The search criteria.
   */
  void checkTapeFileSearchCriteria(const TapeFileSearchCriteria &searchCriteria) const;

  /**
   * Returns the specified archive files.  Please note that the list of files
   * is ordered by archive file ID.
   *
   * @param searchCriteria The search criteria.
   * @return The archive files.
   */
  ArchiveFileItor getArchiveFilesItor(const TapeFileSearchCriteria &searchCriteria) const override;

  /**
   * Returns the specified deleted archive files.  Please note that the list of files
   * is ordered by archive file ID.
   *
   * @param searchCriteria The search criteria.
   * @return The deleted archive files.
   */
  DeletedArchiveFileItor getDeletedArchiveFilesItor(const TapeFileSearchCriteria& searchCriteria) const override;
  
  /**
   * Returns the specified files in tape file sequence order.
   *
   * @param vid The volume identifier of the tape.
   * @param startFSeq The file sequence number of the first file.  Please note
   * that there might not be a file with this exact file sequence number.
   * @param maxNbFiles The maximum number of files to be returned.
   * @return The specified files in tape file sequence order.
   */
  std::list<common::dataStructures::ArchiveFile> getFilesForRepack(
    const std::string &vid,
    const uint64_t startFSeq,
    const uint64_t maxNbFiles) const override;

  /**
   * Returns all the tape copies (no matter their VIDs) of the archive files
   * associated with the tape files on the specified tape in FSEQ order
   * starting at the specified startFSeq.
   *
   * @param vid The volume identifier of the tape.
   * @param startFSeq The file sequence number of the first file.  Please note
   * that there might not be a file with this exact file sequence number.
   * @return The specified files in FSEQ order.
   */
  ArchiveFileItor getArchiveFilesForRepackItor(
    const std::string &vid,
    const uint64_t startFSeq) const override;

  /**
   * Returns a summary of the tape files that meet the specified search
   * criteria.
   *
   * @param searchCriteria The search criteria.
   * @return The summary.
   */
  common::dataStructures::ArchiveFileSummary getTapeFileSummary(
    const TapeFileSearchCriteria &searchCriteria) const override;

  /**
   * Returns the archive file with the specified unique identifier.
   *
   * This method assumes that the archive file being requested exists and will
   * therefore throw an exception if it does not.
   *
   * @param id The unique identifier of the archive file.
   * @return The archive file.
   */
  common::dataStructures::ArchiveFile getArchiveFileById(const uint64_t id) override;

  /**
   * Returns true if the specified user has administrator privileges.
   *
   * @param admin The administrator.
   * @return True if the specified user has administrator privileges.
   */
  bool isAdmin(const common::dataStructures::SecurityIdentity &admin) const override;

  /**
   * Checks that the most trivial query goes through. Returns true on success,
   * false on failure.
   * 
   * @return True if the query went through.
   */
  void ping() override;
  
  /**
   * Checks that the online database schema MAJOR version number matches the schema MAJOR version number defined in version.h
   */
  void verifySchemaVersion() override;
  
  /**
   * Returns the SchemaVersion object corresponding to the catalogue schema version:
   * - SCHEMA_VERSION_MAJOR
   * - SCHEMA_VERSION_MINOR
   * - SCHEMA_VERSION_MAJOR_NEXT (future major version number of the schema in case of upgrade)
   * - SCHEMA_VERSION_MINOR_NEXT (future minor version number of the schema in case of upgrade)
   * - STATUS (UPGRADING or PRODUCTION)	
   * 
   * @return The SchemaVersion object corresponding to the catalogue schema version
   */
  SchemaVersion getSchemaVersion() const override;
  
  /**
   * Returns the names of all the tables in the database schema in alphabetical
   * order.
   *
   * @return The names of all the tables in the database schema in alphabetical
   * order.
   */
  std::list<std::string> getTableNames() const;

protected:

  /**
   * Object representing the API to the CTA logging system.
   */
  log::Logger &m_log;

  /**
   * Mutex to be used to a take a global lock on the database.
   */
  threading::Mutex m_mutex;

  /**
   * The pool of connections to the underlying relational database to be used
   * for all operations accept listing archive files which can be relatively
   * long operations.
   */
  mutable rdbms::ConnPool m_connPool;

  /**
   * The pool of connections to the underlying relational database to be used
   * for the sole purpose of listing archive files.
   */
  mutable rdbms::ConnPool m_archiveFileListingConnPool;

  /**
   * Returns true if the specified admin user exists.
   *
   * @param conn The database connection.
   * @param adminUsername The name of the admin user.
   * @return True if the admin user exists.
   */
  bool adminUserExists(rdbms::Conn &conn, const std::string adminUsername) const;

  /**
   * Returns true if the specified vo exists.
   *
   * @param conn The database connection.
   * @param voName The name of the vo
   * @return True if the vo exists, false otherwise
   */
  bool virtualOrganizationExists(rdbms::Conn &conn, const std::string &voName) const;

  /**
   * Returns true if the specified media type exists.
   *
   * @param conn The database connection.
   * @param name The name of the media type.
   * @return True if the media type exists.
   */
  bool mediaTypeExists(rdbms::Conn &conn, const std::string &name) const;

  /**
   * Returns true if the specified storage class exists.
   *
   * @param conn The database connection.
   * @param storageClassName The name of the storage class.
   * @return True if the storage class exists.
   */
  bool storageClassExists(rdbms::Conn &conn, const std::string &storageClassName) const;

  /**
   * Returns true if the specified tape pool exists.
   *
   * @param tapePoolName The name of the tape pool.
   * @return True if the tape pool exists.
   */
  bool tapePoolExists(const std::string &tapePoolName) const override;

  /**
   * Returns true if the specified tape pool exists.
   *
   * @param conn The database connection.
   * @param tapePoolName The name of the tape pool.
   * @return True if the tape pool exists.
   */
  bool tapePoolExists(rdbms::Conn &conn, const std::string &tapePoolName) const;

  /**
   * Returns true if the specified archive file identifier exists.
   *
   * @param conn The database connection.
   * @param archiveFileId The archive file identifier.
   * @return True if the archive file identifier exists.
   */
  bool archiveFileIdExists(rdbms::Conn &conn, const uint64_t archiveFileId) const;

  /**
   * Returns true if the specified disk file identifier exists.
   *
   * @param conn The database connection.
   * @param diskInstanceName The name of the disk instance to which the disk
   * file identifier belongs.
   * @param diskFileId The disk file identifier.
   * @return True if the disk file identifier exists.
   */
  bool diskFileIdExists(rdbms::Conn &conn, const std::string &diskInstanceName, const std::string &diskFileId) const;

  /**
   * Returns true if the specified disk file user exists.
   *
   * @param conn              The database connection.
   * @param diskInstanceName  The name of the disk instance to which the disk file user belongs.
   * @param diskFileOwnerUid  The user ID of the disk file owner.
   * @return                  True if the disk file user exists.
   */
  bool diskFileUserExists(rdbms::Conn &conn, const std::string &diskInstanceName, uint32_t diskFileOwnerUid) const;

  /**
   * Returns true if the specified disk file group exists.
   *
   * @param conn              The database connection.
   * @param diskInstanceName  The name of the disk instance to which the disk file group belongs.
   * @param diskFileGid       The group ID of the disk file.
   * @return                  True if the disk file group exists.
   */
  bool diskFileGroupExists(rdbms::Conn &conn, const std::string &diskInstanceName, uint32_t diskFileGid) const;

  /**
   * Returns true if the specified archive route exists.
   *
   * @param conn The database connection.
   * @param storageClassName The name of the storage class which is only
   * guaranteed to be unique within its disk instance.
   * @param copyNb The copy number of the tape file.
   * @return True if the archive route exists.
   */
  bool archiveRouteExists(rdbms::Conn &conn, const std::string &storageClassName,
    const uint32_t copyNb) const;

  /**
   * @return the archive routes of the given storage class and destination tape
   * pool.
   *
   * Under normal circumstances this method should return either 0 or 1 route.
   * For a given storage class there should be no more than one route to any
   * given tape pool.
   *
   * @param conn The database connection.
   * @param storageClassName The name of the storage class which is only
   * guaranteed to be unique within its disk instance.
   * @param tapePoolName The name of the tape pool.
   */
  std::list<common::dataStructures::ArchiveRoute> getArchiveRoutes(rdbms::Conn &conn,
    const std::string &storageClassName, const std::string &tapePoolName) const;

  /**
   * Returns true if the specified tape exists.
   *
   * @param vid The volume identifier of the tape.
   * @return True if the tape exists.
   */
  bool tapeExists(const std::string &vid) const override;

  /**
   * Returns true if the specified tape exists.
   *
   * @param conn The database connection.
   * @param vid The volume identifier of the tape.
   * @return True if the tape exists.
   */
  bool tapeExists(rdbms::Conn &conn, const std::string &vid) const;
  
  /**
   * Returns true if the specified disk system exists.
   *
   * @param name The name identifier of the disk system.
   * @return True if the tape exists.
   */
  bool diskSystemExists(const std::string &name) const override;  
  
  /**
   * Returns true if the specified disk system exists.
   *
   * @param conn The database connection.
   * @param name The name identifier of the disk system.
   * @return True if the disk system exists.
   */
  bool diskSystemExists(rdbms::Conn &conn, const std::string &name) const;

  /**
   * Returns the list of tapes that meet the specified search criteria.
   *
   * @param conn The database connection.
   * @param searchCriteria The search criteria.
   * @return The list of tapes.
   */
  std::list<common::dataStructures::Tape> getTapes(rdbms::Conn &conn, const TapeSearchCriteria &searchCriteria) const;
  
  /**
   * Returns true if the specified logical library exists.
   *
   * @param conn The database connection.
   * @param logicalLibraryName The name of the logical library.
   * @return True if the logical library exists.
   */
  bool logicalLibraryExists(rdbms::Conn &conn, const std::string &logicalLibraryName) const;

  /**
   * Returns true if the specified mount policy exists.
   *
   * @param conn The database connection.
   * @param mountPolicyName The name of the mount policy
   * @return True if the mount policy exists.
   */
  bool mountPolicyExists(rdbms::Conn &conn, const std::string &mountPolicyName) const;

  /**
   * Returns true if the specified requester mount-rule exists.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester belongs.
   * @param requesterName The username of the requester which is only guaranteed
   * to be unique within its disk instance.
   * @return True if the requester mount-rule exists.
   */
  bool requesterMountRuleExists(rdbms::Conn &conn, const std::string &diskInstanceName,
    const std::string &requesterName) const;

  /**
    * A fully qualified user, in other words the name of the disk instance and
    * the name of the group.
    */
  struct User {
    /**
     * The name of the disk instance to which the user name belongs.
     */
    std::string diskInstanceName;

    /**
     * The name of the user which is only guaranteed to be unique within its
     * disk instance.
     */
    std::string username;

    /**
     * Constructor.
     *
     * @param d The name of the disk instance to which the group name belongs.
     * @param u The name of the group which is only guaranteed to be unique
     * within its disk instance.
     */
    User(const std::string &d, const std::string &u): diskInstanceName(d), username(u) {
    }

    /**
     * Less than operator.
     *
     * @param rhs The argument on the right hand side of the operator.
     * @return True if this object is less than the argument on the right hand
     * side of the operator.
     */
    bool operator<(const User &rhs) const {
      return diskInstanceName < rhs.diskInstanceName || username < rhs.username;
    }
  }; // struct User

  /**
   * Returns a cached version of the specified requester mount-policy or nullopt
   * if one does not exist.
   *
   * @param user The fully qualified user, in other words the name of the disk
   * instance and the name of the group.
   * @return The mount policy or nullopt if one does not exists.
   */
  optional<common::dataStructures::MountPolicy> getCachedRequesterMountPolicy(const User &user) const;

  /**
   * Returns the specified requester mount-policy or nullopt if one does not
   * exist.
   *
   * @param conn The database connection.
   * @param user The fully qualified user, in other words the name of the disk
   * instance and the name of the group.
   * @return The mount policy or nullopt if one does not exists.
   */
  optional<common::dataStructures::MountPolicy> getRequesterMountPolicy(rdbms::Conn &conn, const User &user) const;

  /**
   * Returns true if the specified requester-group mount-rule exists.
   *
   * @param conn The database connection.
   * @param diskInstanceName The name of the disk instance to which the
   * requester group belongs.
   * @param requesterGroupName The name of the requester group which is only
   * guaranteed to be unique within its disk instance.
   * @return True if the requester-group mount-rule exists.
   */
  bool requesterGroupMountRuleExists(rdbms::Conn &conn, const std::string &diskInstanceName,
    const std::string &requesterGroupName) const;

  /**
    * A fully qualified user group, in other words the name of the disk instance
    * and the name of the group.
    */
  struct Group {
    /**
     * The name of the disk instance to which the group name belongs.
     */
    std::string diskInstanceName;

    /**
     * The name of the group which is only guaranteed to be unique within its
     * disk instance.
     */
    std::string groupName;

    /**
     * Constructor.
     *
     * @param d The name of the disk instance to which the group name belongs.
     * @param g The name of the group which is only guaranteed to be unique
     * within its disk instance.
     */
    Group(const std::string &d, const std::string &g): diskInstanceName(d), groupName(g) {
    }

    /**
     * Less than operator.
     *
     * @param rhs The argument on the right hand side of the operator.
     * @return True if this object is less than the argument on the right hand
     * side of the operator.
     */
    bool operator<(const Group &rhs) const {
      return diskInstanceName < rhs.diskInstanceName || groupName < rhs.groupName;
    }
  }; // struct Group

  /**
   * Returns a cached version of the specified requester-group mount-policy or
   * nullptr if one does not exist.
   *
   * This method updates the cache when necessary.
   *
   * @param group The fully qualified group, in other words the name of the disk
   * instance and the name of the group.
   * @return The cached mount policy or nullopt if one does not exists.
   */
  optional<common::dataStructures::MountPolicy> getCachedRequesterGroupMountPolicy(const Group &group) const;

  /**
   * Returns the specified requester-group mount-policy or nullptr if one does
   * not exist.
   *
   * @param conn The database connection.
   * @param group The fully qualified group, in other words the name of the disk
   * instance and the name of the group.
   * @return The mount policy or nullopt if one does not exists.
   */
  optional<common::dataStructures::MountPolicy> getRequesterGroupMountPolicy(rdbms::Conn &conn, const Group &group)
    const;

  /**
   * Returns the specified tape log information from the specified database
   * result set.
   *
   * @param rset The result set.
   * @param driveColName The name of the database column that contains the name
   * of the tape drive.
   * @param timeColNAme The name of the database column that contains the time
   * stamp.
   */
  optional<common::dataStructures::TapeLog> getTapeLogFromRset(const rdbms::Rset &rset,
    const std::string &driveColName, const std::string &timeColName) const;

  /**
   * An RdbmsCatalogue specific method that inserts the specified row into the
   * ArchiveFile table.
   *
   * @param conn The database connection.
   * @param row The row to be inserted.
   */
  void insertArchiveFile(rdbms::Conn &conn, const ArchiveFileRow &row);

  /**
   * Creates the database schema.
   */
  void createDbSchema();

  /**
   * A fully qualified storage class, in other words the name of the disk
   * instance and the name of the storage class.
   */
  struct StorageClass {

    /**
     * The name of the storage class which is only guaranteed to be unique
     */
    std::string storageClassName;

    /**
     * Constructor.
     *
     * @param sN The name of the storage class which is only guaranteed to be
     * unique within its disk instance.
     */
    StorageClass(const std::string &s): storageClassName(s) {
    }

    /**
     * Less than operator.
     *
     * @param rhs The argument on the right hand side of the operator.
     * @return True if this object is less than the argument on the right hand
     * side of the operator.
     */
    bool operator<(const StorageClass &rhs) const {
      return storageClassName < rhs.storageClassName;
    }
  }; // struct StorageClass

  /**
   * Returns a cached version of the expected number of archive routes for the
   * specified storage class as specified by the call to the
   * createStorageClass() method as opposed to the actual number entered so far
   * using the createArchiveRoute() method.
   *
   * This method updates the cache when necessary.
   *
   * @param storageClass The fully qualified storage class, in other words the
   * name of the disk instance and the name of the storage class.
   * @return The expected number of archive routes.
   */
  uint64_t getCachedExpectedNbArchiveRoutes(const StorageClass &storageClass) const;

  /**
   * Returns the expected number of archive routes for the specified storage
   * class as specified by the call to the createStorageClass() method as
   * opposed to the actual number entered so far using the createArchiveRoute()
   * method.
   *
   * @param conn The database connection.
   * @param storageClass The fully qualified storage class, in other words the
   * name of the disk instance and the name of the storage class.
   * @return The expected number of archive routes.
   */
  uint64_t getExpectedNbArchiveRoutes(rdbms::Conn &conn, const StorageClass &storageClass) const;

  /**
   * Inserts the specified tape file into the Tape table.
   *
   * @param conn The database connection.
   * @param tapeFile The tape file.
   * @param archiveFileId The identifier of the archive file of which the tape
   * file is a copy.
   */
  void insertTapeFile(
    rdbms::Conn &conn,
    const common::dataStructures::TapeFile &tapeFile,
    const uint64_t archiveFileId);

  /**
   * Sets the last FSeq of the specified tape to the specified value.
   *
   * @param conn The database connection.
   * @param vid The volume identifier of the tape.
   * @param lastFseq The new value of the last FSeq.
   */
  void setTapeLastFSeq(rdbms::Conn &conn, const std::string &vid, const uint64_t lastFSeq);

  /**
   * Returns the last FSeq of the specified tape.
   *
   * @param conn The database connection.
   * @param vid The volume identifier of the tape.
   * @return The last FSeq.
   */
  uint64_t getTapeLastFSeq(rdbms::Conn &conn, const std::string &vid) const;

  /**
   * Updates the specified tape with the specified information.
   *
   * @param conn The database connection.
   * @param vid The volume identifier of the tape.
   * @param lastFSeq The sequence number of the last tape file written to the
   * tape.
   * @param compressedBytesWritten The number of compressed bytes written to
   * the tape.
   * @param tapeDrive The name of the tape drive that last wrote to the tape.
   */
  void updateTape(
    rdbms::Conn &conn,
    const std::string &vid,
    const uint64_t lastFSeq,
    const uint64_t compressedBytesWritten,
    const std::string &tapeDrive);

  /**
   * Returns the specified archive file.   A nullptr pointer is returned if
   * there is no corresponding row in the ARCHIVE_FILE table.  Please note that
   * a non-nullptr is returned if there is a row in the ARCHIVE_FILE table and
   * there are no rows in the TAPE_FILE table.
   *
   * Please note that this method performs a LEFT OUTER JOIN from the
   * ARCHIVE_FILE table to the TAPE_FILE table.
   *
   * @param conn The database connection.
   * @param archiveFileId The identifier of the archive file.
   * @return The archive file or nullptr.
   */
  std::unique_ptr<common::dataStructures::ArchiveFile> getArchiveFileByArchiveFileId(
    rdbms::Conn &conn,
    const uint64_t archiveFileId) const;

  /**
   * Returns the specified archive file.   A nullptr pointer is returned if
   * there are no corresponding rows in the TAPE_FILE table.
   *
   * @param conn The database connection.
   * @param archiveFileId The identifier of the archive file.
   * @return The archive file or nullptr.
   */
  std::unique_ptr<common::dataStructures::ArchiveFile> getArchiveFileToRetrieveByArchiveFileId(
    rdbms::Conn &conn,
    const uint64_t archiveFileId) const;

  /**
   * Returns a cached version of the (possibly empty) activities to weight map
   * for the given dsk instance.
   * @param diskInstance
   * @return activities to weight map (ActivitiesFairShareWeights)
   */
  common::dataStructures::ActivitiesFairShareWeights getCachedActivitiesWeights(
    const std::string &diskInstanceName) const;
  
  /**
   * Returns a the (possibly empty) activities to weight map for the given dsk instance.
   * @param conn The database connection.
   * @param diskInstance
   * @return activities to weight map (ActivitiesFairShareWeights)
   */
  common::dataStructures::ActivitiesFairShareWeights getActivitiesWeights(
    rdbms::Conn &conn,
    const std::string &diskInstanceName) const;  
  
  /**
   * Returns the specified archive file.   A nullptr pointer is returned if
   * there is no corresponding row in the ARCHIVE_FILE table.  Please note that
   * a non-nullptr is returned if there is a row in the ARCHIVE_FILE table and
   * there are no rows in the TAPE_FILE table.
   *
   * Please note that this method performs a LEFT OUTER JOIN from the
   * ARCHIVE_FILE table to the TAPE_FILE table.
   *
   * @param conn The database connection.
   * @param diskInstanceName The name of the disk instance.
   * @param diskFileId The identifier of the source disk file which is unique
   * within it's host disk system.  Two files from different disk systems may
   * have the same identifier.  The combination of diskInstanceName and
   * diskFileId must be globally unique, in other words unique within the CTA
   * catalogue.
   * @return The archive file or nullptr.
   * an empty list.
   */
  std::unique_ptr<common::dataStructures::ArchiveFile> getArchiveFileByDiskFileId(
    rdbms::Conn &conn,
    const std::string &diskInstance,
    const std::string &diskFileId) const;

  /**
   * Returns the specified archive file.   A nullptr pointer is returned if
   * there are no corresponding rows in the TAPE_FILE table.
   *
   * @param conn The database connection.
   * @param diskInstanceName The name of the disk instance.
   * @param diskFileId The identifier of the source disk file which is unique
   * within it's host disk system.  Two files from different disk systems may
   * have the same identifier.  The combination of diskInstanceName and
   * diskFileId must be globally unique, in other words unique within the CTA
   * catalogue.
   * @return The archive file or nullptr.
   * an empty list.
   */
  std::unique_ptr<common::dataStructures::ArchiveFile> getArchiveFileToRetrieveByDiskFileId(
    rdbms::Conn &conn,
    const std::string &diskInstance,
    const std::string &diskFileId) const;

  /**
   * Returns the mount policies for the specified requester and requester group.
   *
   * @param conn The database connection.
   * @param diskInstanceName The name of the disk instance to which the
   * requester and requester group belong.
   * @param requesterName The name of the requester which is only guaranteed to
   * be unique within its disk instance.
   * @param requesterGroupName The name of the requester group which is only
   * guaranteed to be unique within its disk instance.
   * @return The mount policies.
   */
  RequesterAndGroupMountPolicies getMountPolicies(
    rdbms::Conn &conn,
    const std::string &diskInstanceName,
    const std::string &requesterName,
    const std::string &requesterGroupName) const;

  /**
   * Returns a unique archive ID that can be used by a new archive file within
   * the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return A unique archive ID that can be used by a new archive file within
   * the catalogue.
   */
  virtual uint64_t getNextArchiveFileId(rdbms::Conn &conn) = 0;

  /**
   * Returns a unique logical library ID that can be used by a new logical
   * library within the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique logical library ID that can be used by a new logical
   * library storage class within the catalogue.
   */
  virtual uint64_t getNextLogicalLibraryId(rdbms::Conn &conn) = 0;

  /**
   * Returns a unique virtual organization ID that can be used by a new Virtual Organization
   * within the catalogue.
   * 
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   * 
   * @param conn The database connection
   * @return a unique virtual organization ID that can be used by a new Virtual Organization
   * within the catalogue.
   */
  virtual uint64_t getNextVirtualOrganizationId(rdbms::Conn &conn) = 0;
  
  /**
   * Returns a unique media type ID that can be used by a new media type within
   * the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique media type ID that can be used by a new media type
   * within the catalogue.
   */
  virtual uint64_t getNextMediaTypeId(rdbms::Conn &conn) = 0;
  
  /**
   * Returns a unique storage class ID that can be used by a new storage class
   * within the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique storage class ID that can be used by a new storage class
   * within the catalogue.
   */
  virtual uint64_t getNextStorageClassId(rdbms::Conn &conn) = 0;

  /**
   * Returns a unique tape pool ID that can be used by a new tape pool within
   * the catalogue.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because different database technologies propose different solution to the
   * problem of generating ever increasing numeric identifiers.
   *
   * @param conn The database connection.
   * @return a unique tape pool ID that can be used by a new tape pool within
   * the catalogue.
   */
  virtual uint64_t getNextTapePoolId(rdbms::Conn &conn) = 0;

  /**
   * Returns a cached version of the mapping from tape copy to tape pool for the
   * specified storage class.
   *
   * This method updates the cache when necessary.
   *
   * @param storageClass The fully qualified storage class, in other words the
   * name of the disk instance and the name of the storage class.
   * @return The mapping from tape copy to tape pool for the specified storage
   * class.
   */
  common::dataStructures::TapeCopyToPoolMap getCachedTapeCopyToPoolMap(const StorageClass &storageClass) const;

  /**
   * Returns the mapping from tape copy to tape pool for the specified storage
   * class.
   *
   * @param conn The database connection.
   * @param storageClass The fully qualified storage class, in other words the
   * name of the disk instance and the name of the storage class.
   * @return The mapping from tape copy to tape pool for the specified storage
   * class.
   */
  common::dataStructures::TapeCopyToPoolMap getTapeCopyToPoolMap(rdbms::Conn &conn,
    const StorageClass &storageClass) const;

  /**
   * Throws an exception if one of the fields of the specified event have not
   * been set.
   *
   * @param callingFunc The name of the calling function.
   * @param event The evnt to be checked.
   */
  void checkTapeItemWrittenFieldsAreSet(const std::string &callingFunc, const TapeItemWritten &event) const;
  
  /**
   * Throws an exception if one of the fields of the specified event have not
   * been set.
   *
   * @param callingFunc The name of the calling function.
   * @param event The evnt to be checked.
   */
  void checkTapeFileWrittenFieldsAreSet(const std::string &callingFunc, const TapeFileWritten &event) const;
  
  /**
   * Throws an exception if the delete request passed in parameter is not consistent
   * to allow a deletion of the ArchiveFile from the Catalogue.
   * @param deleteRequest, the deleteRequest to check the consistency.
   * @param archiveFile the ArchiveFile to delete to check the deleteRequest consistency against.
   */
  void checkDeleteRequestConsistency(const cta::common::dataStructures::DeleteArchiveRequest deleteRequest, const cta::common::dataStructures::ArchiveFile & archiveFile) const;

  /**
   * Returns a cached version of the result of calling isAdmin().
   *
   * @param admin The administrator.
   * @return True if the specified user has administrator privileges.
   */
  bool isCachedAdmin(const common::dataStructures::SecurityIdentity &admin) const;

  /**
   * Returns true if the specified user has administrator privileges.
   *
   * Please note that this method always queries the Catalogue database.
   *
   * @param admin The administrator.
   * @return True if the specified user has administrator privileges.
   */
  bool isNonCachedAdmin(const common::dataStructures::SecurityIdentity &admin) const;

  /**
   * Returns the number of tapes in the specified tape pool.
   *
   * If the tape pool does not exist then this method returns 0.
   *
   * @param conn The database connection.
   * @param name The name of the tape pool.
   * @return The number of tapes in the specified tape pool.
   */
  uint64_t getNbTapesInPool(rdbms::Conn &conn, const std::string &name) const;

  /**
   * Returns true if the specified optional string is both set and empty.
   *
   * @param optionalStr The optional string.
   * @return True if the specified optional string is both set and empty.
   */
  bool isSetAndEmpty(const optional<std::string> &optionalStr) const;

  /**
   * Returns true if the specified optional string list is both set and empty.
   *
   * @param optionalStr The optional string list.
   * @return True if the specified optional string list is both set and empty.
   */
  bool isSetAndEmpty(const optional<std::vector<std::string>> &optionalStrList) const;

  /**
   * Returns true if the specified media type is currently being used by one or
   * more archive tapes.
   *
   * @param conn The database connection.
   * @param name The name of the media type
   */
  bool mediaTypeIsUsedByTapes(rdbms::Conn &conn, const std::string &name) const;

  /**
   * Returns true if the specified storage class is currently being used by one
   * or more archive routes.
   *
   * @param conn The database connection.
   * @param storageClassName The name of the storage class.
   */
  bool storageClassIsUsedByArchiveRoutes(rdbms::Conn &conn, const std::string &storageClassName) const;

  /**
   * Returns true if the specified storage class is currently being used by one
   * or more archive files.
   *
   * @param conn The database connection.
   * @param storageClassName The name of the storage class.
   */
  bool storageClassIsUsedByArchiveFiles(rdbms::Conn &conn, const std::string &storageClassName) const;
  
  /**
   * Returns true if the specified Virtual Organization is currently being used by one
   * or more StorageClasses
   *
   * @param conn The database connection.
   * @param voName The name of the Virtual Organization.
   */
  bool virtualOrganizationIsUsedByStorageClasses(rdbms::Conn &conn, const std::string &voName) const;
  
  /**
   * Returns true if the specified Virtual Organization is currently being used by one
   * or more Tapepools
   *
   * @param conn The database connection.
   * @param voName The name of the Virtual Organization.
   */
  bool virtualOrganizationIsUsedByTapepools(rdbms::Conn &conn, const std::string &voName) const;

  /**
   * Returns the ID of the specified logical library or nullopt if the logical
   * library does not exist.
   *
   * @param conn The database connection.
   * @param name The name of the tape pool.
   * @return the ID of the specified tape pool.
   */
  optional<uint64_t> getLogicalLibraryId(rdbms::Conn &conn, const std::string &name) const;

  /**
   * Returns the ID of the specified tape pool or nullopt if it the tape pool
   * does not exist.
   *
   * @param conn The database connection.
   * @param name The name of the tape pool.
   * @return the ID of the specified tape pool.
   */
  optional<uint64_t> getTapePoolId(rdbms::Conn &conn, const std::string &name) const;

  /**
   * Updates the disk file ID of the specified archive file.
   *
   * @param archiveFileId The unique identifier of the archive file.
   * @param diskInstance The instance name of the source disk system.
   * @param diskFileId The identifier of the source disk file which is unique
   * within it's host disk system.  Two files from different disk systems may
   * have the same identifier.  The combination of diskInstance and diskFileId
   * must be globally unique, in other words unique within the CTA catalogue.
   */
  void updateDiskFileId(uint64_t archiveFileId, const std::string &diskInstance,
    const std::string &diskFileId) override;

   /**
   * Insert the tape files and ArchiveFiles entries in the recycle-bin and delete
   * them from the TAPE_FILE and ARCHIVE_FILE  tables
   * @param request the DeleteRequest object that holds information about the file to delete.
   * @param lc the logContext
   */
  void moveArchiveFileToRecycleBin(const common::dataStructures::DeleteArchiveRequest &request, 
  log::LogContext & lc) override;
  
  /**
   * Copy the archiveFile and the associated tape files from the ARCHIVE_FILE and TAPE_FILE tables to the recycle-bin tables
   * and deletes the ARCHIVE_FILE and TAPE_FILE entries.
   * @param conn the database connection
   * @param request the request that contains the necessary informations to identify the archiveFile to copy to the recycle-bin
   * @param lc the log context
   */
  virtual void copyArchiveFileToRecycleBinAndDelete(rdbms::Conn & conn,const common::dataStructures::DeleteArchiveRequest &request, log::LogContext & lc) = 0;
  
  /**
   * Copies the ARCHIVE_FILE and TAPE_FILE entries to the recycle-bin tables 
   * @param conn the database connection
   * @param request the request that contains the necessary informations to identify the archiveFile to copy to the recycle-bin
   */
  void copyArchiveFileToRecycleBin(rdbms::Conn & conn, const common::dataStructures::DeleteArchiveRequest & request);
  
  
  /**
   * Delete the TAPE_FILE and ARCHIVE_FILE entries from the Catalogue
   * @param conn the database connection
   * @param request the request that contains the necessary informations to identify the archiveFile to copy to the recycle-bin
   */
  void deleteArchiveFileAndTapeFiles(rdbms::Conn & conn, const common::dataStructures::DeleteArchiveRequest & request);
  
  /**
   * Delete the archiveFile and the associated tape files from the recycle-bin
   * @param archiveFileId the archiveFileId of the archive file to delete
   * @param lc the logContext
   */
  void deleteFileFromRecycleBin(const uint64_t archiveFileId, log::LogContext &lc);
  
  /**
   * Delete the archiveFile and the associated tape files that are on the specified tape from the recycle-bin
   * @param vid the vid of the tape where the files to be deleted are located
   * @param lc the logContext
   */
  void deleteFilesFromRecycleBin(rdbms::Conn & conn,const std::string & vid, log::LogContext & lc);
  
  /**
   * Delete the TapeFiles and the ArchiveFile from the recycle-bin in one transaction
   * @param conn the database connection
   * @param archiveFileId the archiveFileId of the file to delete from the recycle-bin
   */
  virtual void deleteTapeFilesAndArchiveFileFromRecycleBin(rdbms::Conn & conn, const uint64_t archiveFileId, log::LogContext & lc) = 0;
  
  /**
   * Delete the tape files from the TAPE_FILE recycle-bin
   * @param conn the database connection
   * @param archiveFileId the archiveFileId of the tape files to delete
   */
  void deleteTapeFilesFromRecycleBin(rdbms::Conn & conn, const uint64_t archiveFileId);
  
  /**
   * Delete the archive file from the ARCHIVE_FILE recycle-bin
   * @param conn the database connection
   * @param archiveFileId the archiveFileId of the archive file to delete
   */
  void deleteArchiveFileFromRecycleBin(rdbms::Conn & conn, const uint64_t archiveFileId);
  
  /**
   * Cached versions of tape copy to tape tape pool mappings for specific
   * storage classes.
   */
  mutable TimeBasedCache<StorageClass, common::dataStructures::TapeCopyToPoolMap> m_tapeCopyToPoolCache;

  /**
   * Cached versions of mount policies for specific user groups.
   */
  mutable TimeBasedCache<Group, optional<common::dataStructures::MountPolicy> > m_groupMountPolicyCache;

  /**
   * Cached versions of mount policies for specific users.
   */
  mutable TimeBasedCache<User, optional<common::dataStructures::MountPolicy> > m_userMountPolicyCache;

  /**
   * Cached versions of the expected number of archive routes for specific
   * storage classes as specified by the call to the createStorageClass()
   * method as opposed to the actual number entered so far using the
   * createArchiveRoute() method.
   */
  mutable TimeBasedCache<StorageClass, uint64_t> m_expectedNbArchiveRoutesCache;

  /**
   * Cached version of isAdmin() results.
   */
  mutable TimeBasedCache<common::dataStructures::SecurityIdentity, bool> m_isAdminCache;
  
  /**
   * Cached version of the activities to weight maps.
   */
  mutable TimeBasedCache<std::string, common::dataStructures::ActivitiesFairShareWeights> m_activitiesFairShareWeights;

}; // class RdbmsCatalogue

} // namespace catalogue
} // namespace cta

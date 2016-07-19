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
#include "rdbms/DbConn.hpp"
#include "catalogue/RequesterAndGroupMountPolicies.hpp"

#include <memory>
#include <mutex>

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
 * CTA catalogue implemented using a relational database backend.
 */
class RdbmsCatalogue: public Catalogue {
protected:

  /**
   * Protected constructor only to be called by sub-classes.
   */
  RdbmsCatalogue();

public:

  /**
   * Destructor.
   */
  virtual ~RdbmsCatalogue();
  
  virtual void createBootstrapAdminAndHostNoAuth(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &username, const std::string &hostName, const std::string &comment) override;

  virtual void createAdminUser(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &username, const std::string &comment) override;
  virtual void deleteAdminUser(const std::string &username) override;
  virtual std::list<common::dataStructures::AdminUser> getAdminUsers() const override;
  virtual void modifyAdminUserComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &username, const std::string &comment) override;

  virtual void createAdminHost(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &hostName, const std::string &comment) override;
  virtual void deleteAdminHost(const std::string &hostName) override;
  virtual std::list<common::dataStructures::AdminHost> getAdminHosts() const override;
  virtual void modifyAdminHostComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &hostName, const std::string &comment) override;

  /**
   * Creates the specified storage class.
   *
   * @param cliIdentity The identity of the command-line interface.
   * @param storageClass The storage class.
   */
  virtual void createStorageClass(
    const common::dataStructures::SecurityIdentity &cliIdentity,
    const common::dataStructures::StorageClass &storageClass) override;

  /**
   * Deletes the specified storage class.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * storage class belongs.
   * @param stoargeClassName The name of the storage class which is only
   * guaranteed to be unique within its disk isntance.
   */
  virtual void deleteStorageClass(const std::string &diskInstanceName, const std::string &storageClassName) override;

  virtual std::list<common::dataStructures::StorageClass> getStorageClasses() const override;
  virtual void modifyStorageClassNbCopies(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &instanceName, const std::string &name, const uint64_t nbCopies) override;
  virtual void modifyStorageClassComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &instanceName, const std::string &name, const std::string &comment) override;

  virtual void createTapePool(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t nbPartialTapes, const bool encryptionValue, const std::string &comment) override;
  virtual void deleteTapePool(const std::string &name) override;
  virtual std::list<common::dataStructures::TapePool> getTapePools() const override;
  virtual void modifyTapePoolNbPartialTapes(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t nbPartialTapes) override;
  virtual void modifyTapePoolComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const std::string &comment) override;
  virtual void setTapePoolEncryption(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const bool encryptionValue) override;

  virtual void createArchiveRoute(
    const common::dataStructures::SecurityIdentity &cliIdentity,
    const std::string &diskInstanceName,
    const std::string &storageClassName,
    const uint64_t copyNb,
    const std::string &tapePoolName,
    const std::string &comment) override;


  /**
   * Deletes the specified archive route.
   *
   * @param diskInstanceName The name of the disk instance to which the storage
   * class belongs.
   * @param storageClassName The name of the storage class which is only
   * guaranteed to be unique within its disk instance.
   * @param copyNb The copy number of the tape file.
   */
  virtual void deleteArchiveRoute(
    const std::string &diskInstanceName,
    const std::string &storageClassName, 
    const uint64_t copyNb) override;

  virtual std::list<common::dataStructures::ArchiveRoute> getArchiveRoutes() const override;
  virtual void modifyArchiveRouteTapePoolName(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &instanceName, const std::string &storageClassName, const uint64_t copyNb, const std::string &tapePoolName) override;
  virtual void modifyArchiveRouteComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &instanceName, const std::string &storageClassName, const uint64_t copyNb, const std::string &comment) override;

  virtual void createLogicalLibrary(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const std::string &comment) override;
  virtual void deleteLogicalLibrary(const std::string &name) override;
  virtual std::list<common::dataStructures::LogicalLibrary> getLogicalLibraries() const override;
  virtual void modifyLogicalLibraryComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const std::string &comment) override;

  /**
   * Creates a tape which is assumed to have logical block protection (LBP)
   * enabled.
   */
  virtual void createTape(
    const common::dataStructures::SecurityIdentity &cliIdentity,
    const std::string &vid,
    const std::string &logicalLibraryName,
    const std::string &tapePoolName,
    const std::string &encryptionKey,
    const uint64_t capacityInBytes,
    const bool disabled,
    const bool full,
    const std::string &comment) override;

  virtual void deleteTape(const std::string &vid) override;

  /**
   * Returns the list of tapes that meet the specified search criteria.
   *
   * @param searchCriteria The search criteria.
   * @return The list of tapes.
   */
  virtual std::list<common::dataStructures::Tape> getTapes(const TapeSearchCriteria &searchCriteria) const override;

  /**
   * Returns the tapes with the specified volume identifiers.
   *
   * This method will throw an exception if it cannot find ALL of the specified
   * tapes.
   *
   * @param vids The tape volume identifiers (VIDs).
   * @return Map from tape volume identifier to tape.
   */
  virtual common::dataStructures::VidToTapeMap getTapesByVid(const std::set<std::string> &vids) const override;

  virtual void reclaimTape(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid) override;
  virtual void modifyTapeLogicalLibraryName(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &logicalLibraryName) override;
  virtual void modifyTapeTapePoolName(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &tapePoolName) override;
  virtual void modifyTapeCapacityInBytes(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const uint64_t capacityInBytes) override;
  virtual void modifyTapeEncryptionKey(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &encryptionKey) override;
  virtual void modifyTapeLabelLog(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &drive, const uint64_t timestamp) override; // internal function (noCLI)
  virtual void modifyTapeLastWrittenLog(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &drive, const uint64_t timestamp) override; // internal function (noCLI)
  virtual void modifyTapeLastReadLog(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &drive, const uint64_t timestamp) override; // internal function (noCLI)
  virtual void setTapeBusy(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const bool busyValue) override; // internal function not exposed to the Admin CLI
  virtual void setTapeFull(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const bool fullValue) override;
  virtual void setTapeDisabled(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const bool disabledValue) override;
  virtual void setTapeLbp(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const bool lbpValue) override; // internal function (noCLI)
  virtual void modifyTapeComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &vid, const std::string &comment) override;

  virtual void modifyRequesterMountPolicy(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &instanceName, const std::string &requesterName, const std::string &mountPolicy) override;
  virtual void modifyRequesterComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &instanceName, const std::string &requesterName, const std::string &comment) override;
  virtual void modifyRequesterGroupMountPolicy(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &instanceName, const std::string &requesterGroupName, const std::string &mountPolicy) override;
  virtual void modifyRequesterGroupComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &instanceName, const std::string &requesterGroupName, const std::string &comment) override;

  virtual void createMountPolicy(
    const common::dataStructures::SecurityIdentity &cliIdentity,
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
  virtual std::list<common::dataStructures::MountPolicy> getMountPolicies() const override;

  /**
   * Deletes the specified mount policy.
   *
   * @param name The name of the mount policy.
   */
  virtual void deleteMountPolicy(const std::string &name) override;

  /**
   * Creates the rule that the specified mount policy will be used for the
   * specified requester.
   *
   * Please note that requester mount-rules overrule requester-group
   * mount-rules.
   *
   * @param cliIdentity The user of the command-line tool.
   * @param mountPolicyName The name of the mount policy.
   * @param diskInstance The name of the disk instance to which the requester
   * belongs.
   * @param requesterName The name of the requester which is only guarantted to
   * be unique within its disk instance.
   * @param comment Comment.
   */
  virtual void createRequesterMountRule(
    const common::dataStructures::SecurityIdentity &cliIdentity,
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
  virtual std::list<common::dataStructures::RequesterMountRule> getRequesterMountRules() const override;

  /**
   * Deletes the specified mount rule.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester belongs.
   * @param requesterName The name of the requester which is only guaranteed to
   * be unique within its disk instance.
   */
  virtual void deleteRequesterMountRule(const std::string &diskInstanceName, const std::string &requesterName) override;

  /**
   * Creates the rule that the specified mount policy will be used for the
   * specified requester group.
   *
   * Please note that requester mount-rules overrule requester-group
   * mount-rules.
   *
   * @param cliIdentity The user of the command-line tool.
   * @param mountPolicyName The name of the mount policy.
   * @param diskInstanceName The name of the disk instance to which the
   * requester group belongs.
   * @param requesterGroupName The name of the requester group which is only
   * guarantted to be unique within its disk instance.
   * @param comment Comment.
   */
  virtual void createRequesterGroupMountRule(
    const common::dataStructures::SecurityIdentity &cliIdentity,
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
  virtual std::list<common::dataStructures::RequesterGroupMountRule> getRequesterGroupMountRules() const override;

  /**
   * Deletes the specified mount rule.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester group belongs.
   * @param requesterGroupName The name of the requester group which is only
   * guaranteed to be unique within its disk instance.
   */
  virtual void deleteRequesterGroupMountRule(
    const std::string &diskInstanceName,
    const std::string &requesterGroupName) override;

  virtual void modifyMountPolicyArchivePriority(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t archivePriority) override;
  virtual void modifyMountPolicyArchiveMinFilesQueued(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t minArchiveFilesQueued) override;
  virtual void modifyMountPolicyArchiveMinBytesQueued(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t archiveMinBytesQueued) override;
  virtual void modifyMountPolicyArchiveMinRequestAge(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t minArchiveRequestAge) override;
  virtual void modifyMountPolicyRetrievePriority(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t retrievePriority) override;
  virtual void modifyMountPolicyRetrieveMinFilesQueued(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t minRetrieveFilesQueued) override;
  virtual void modifyMountPolicyRetrieveMinBytesQueued(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t retrieveMinBytesQueued) override;
  virtual void modifyMountPolicyRetrieveMinRequestAge(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t minRetrieveRequestAge) override;
  virtual void modifyMountPolicyMaxDrivesAllowed(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const uint64_t maxDrivesAllowed) override;
  virtual void modifyMountPolicyComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &name, const std::string &comment) override;

  virtual void createDedication(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const common::dataStructures::DedicationType dedicationType,
   const std::string &tag, const std::string &vid, const uint64_t fromTimestamp, const uint64_t untilTimestamp,const std::string &comment) override;
  virtual void deleteDedication(const std::string &drivename) override;
  virtual std::list<common::dataStructures::Dedication> getDedications() const override;
  virtual void modifyDedicationType(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const common::dataStructures::DedicationType dedicationType) override;
  virtual void modifyDedicationTag(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const std::string &tag) override;
  virtual void modifyDedicationVid(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const std::string &vid) override;
  virtual void modifyDedicationFrom(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const uint64_t fromTimestamp) override;
  virtual void modifyDedicationUntil(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const uint64_t untilTimestamp) override;
  virtual void modifyDedicationComment(const common::dataStructures::SecurityIdentity &cliIdentity, const std::string &drivename, const std::string &comment) override;

  /**
   * Returns an iterator over the list of archive files that meet the specified
   * search criteria.
   *
   * Please note that the list is ordered by archive file ID.
   *
   * Please note that this method will throw an exception if the
   * nbArchiveFilesToPrefetch parameter is set to 0.  The parameter must be set
   * to a value greater than or equal to 1.
   *
   * @param searchCriteria The search criteria.
   * @param nbArchiveFilesToPrefetch The number of archive files to prefetch.
   * This parameter must be set to a value equal to or greater than 1.
   * @return An iterator over the list of archive files.
   */
  virtual std::unique_ptr<ArchiveFileItor> getArchiveFileItor(const ArchiveFileSearchCriteria &searchCriteria,
    const uint64_t nbArchiveFilesToPrefetch) const override;

  /**
   * Returns a summary of the archive files that meet the specified search
   * criteria.
   *
   * @param searchCriteria The search criteria.
   * @return The summary.
   */
  virtual common::dataStructures::ArchiveFileSummary getArchiveFileSummary(
    const ArchiveFileSearchCriteria &searchCriteria) const override;

  /**
   * Returns the archive file with the specified unique identifier.
   *
   * This method assumes that the archive file being requested exists and will
   * therefore throw an exception if it does not.
   *
   * @param id The unique identifier of the archive file.
   * @return The archive file.
   */
  virtual common::dataStructures::ArchiveFile getArchiveFileById(const uint64_t id) override;
  
  /**
   * Prepares the catalogue for a new archive file and returns the information
   * required to queue the associated archive request.
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
   * @return The information required to queue the associated archive request.
   */
  virtual common::dataStructures::ArchiveFileQueueCriteria prepareForNewFile(
    const std::string &diskInstanceName,
    const std::string &storageClassName,
    const common::dataStructures::UserIdentity &user) override;

  /**
   * Notifies the catalogue that a file has been written to tape.
   *
   * @param event The tape file written event.
   */
  virtual void fileWrittenToTape(const TapeFileWritten &event) override;

  /**
   * Prepares for a file retrieval by returning the information required to
   * queue the associated retrieve request(s).
   *
   * @param instanceName The name of the instance from where the retrieval request originated
   * @param archiveFileId The unique identifier of the archived file that is
   * to be retrieved.
   * @param user The user for whom the file is to be retrieved.  This will be
   * used by the Catalogue to determine the mount policy to be used when
   * retrieving the file.
   *
   * @return The information required to queue the associated retrieve request(s).
   */
  virtual common::dataStructures::RetrieveFileQueueCriteria prepareToRetrieveFile(
    const std::string &instanceName, 
    const uint64_t archiveFileId,
    const common::dataStructures::UserIdentity &user) override;

  /**
   * Returns true if the specified user running the CTA command-line tool on
   * the specified host has administrator privileges.
   *
   * @param cliIdentity The name of the user and the host on which they are
   * running the CTA command-line tool.
   * @return True if the specified user running the CTA command-line tool on
   * the specified host has administrator privileges.
   */
  virtual bool isAdmin(const common::dataStructures::SecurityIdentity &cliIdentity) const override;

  /**
   * Returns the list of tapes that can be written to by a tape drive in the
   * specified logical library, in other words tapes that are not disabled, not
   * full and are in the specified logical library.
   *
   * @param logicalLibraryName The name of the logical library.
   */
  virtual std::list<TapeForWriting> getTapesForWriting(const std::string &logicalLibraryName) const override;

protected:

  /**
   * Returns true if the specified admin user exists.
   *
   * @param adminUsername The name of the admin user.
   * @return True if the admin user exists.
   */
  bool adminUserExists(const std::string adminUsername) const;

  /**
   * Returns true if the specified admin host exists.
   *
   * @param adminHost The name of the admin host.
   * @return True if the admin host exists.
   */
  bool adminHostExists(const std::string adminHost) const;

  /**
   * Returns true if the specified storage class exists.
   *
   * @param diskInstanceName The name of the disk instance to which the storage
   * class belongs.
   * @param storageClassName The name of the storage class.
   * @return True if the storage class exists.
   */
  bool storageClassExists(const std::string &diskInstanceName, const std::string &storageClassName) const;

  /**
   * Returns true if the specified tape pool exists.
   *
   * @param tapePoolName The name of the tape pool.
   * @return True if the tape pool exists.
   */
  bool tapePoolExists(const std::string &tapePoolName) const;

  /**
   * Returns true if the specified tape exists.
   *
   * @param vid The volume identifier of the tape.
   * @return True if the tape exists.
   */
  bool tapeExists(const std::string &vid) const;

  /**
   * Returns true if the specified logical library exists.
   *
   * @param logicalLibraryName The name of the logical library.
   * @return True if the logical library exists.
   */
  bool logicalLibraryExists(const std::string &logicalLibraryName) const;

  /**
   * Returns true if the specified mount policy exists.
   *
   * @param mountPolicyName The name of the mount policy
   * @return True if the mount policy exists.
   */
  bool mountPolicyExists(const std::string &mountPolicyName) const;

  /**
   * Returns true if the specified requester mount-rule exists.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester belongs.
   * @param requesterName The username of the requester which is only guaranteed
   * to be unique within its disk instance.
   * @return True if the requester mount-rule exists.
   */
  bool requesterMountRuleExists(const std::string &diskInstanceName, const std::string &requesterName) const;

  /**
   * Returns the specified requester mount-policy or NULL if one does not exist.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester belongs.
   * @param requesterName The name of the requester which is only guaranteed to
   * be unique within its disk instance.
   * @return The mount policy or NULL if one does not exists.
   */
  common::dataStructures::MountPolicy *getRequesterMountPolicy(const std::string &diskInstanceName,
    const std::string &requesterName) const;

  /**
   * Returns true if the specified requester-group mount-rule exists.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester group belongs.
   * @param requesterGroupName The name of the requester group which is only
   * guaranteed to be unique within its disk instance.
   * @return True if the requester-group mount-rule exists.
   */
  bool requesterGroupMountRuleExists(const std::string &diskInstanceName, const std::string &requesterGroupName) const;

  /**
   * Returns the specified requester-group mount-policy or NULL if one does not
   * exist.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester group belongs.
   * @param requesterGroupName The name of the requester group which is only
   * guaranteed to be unique within its disk instance.
   * @return The mount policy or NULL if one does not exists.
   */
  common::dataStructures::MountPolicy *getRequesterGroupMountPolicy(const std::string &diskInstanceName,
    const std::string &requesterGroupName) const;

  /**
   * Returns the specified tape log information from the specified database
   * result set.
   *
   * @param rset The result set.
   * @param driveColNAme The name of the database column that contains the name
   * of the tape drive.
   * @param timeColNAme The name of the database column that contains the time
   * stamp.
   */
  optional<common::dataStructures::TapeLog> getTapeLogFromRset(const rdbms::DbRset &rset,
    const std::string &driveColName, const std::string &timeColName) const;

  /**
   * An RdbmsCatalogue specific method that inserts the specified row into the
   * ArchiveFile table.
   *
   * @param row The row to be inserted.
   */
  void insertArchiveFile(const ArchiveFileRow &row);

  /**
   * Mutex to be used to a take a global lock on the in-memory database.
   */
  std::mutex m_mutex;

  /**
   * The connection to the underlying relational database.
   */
  std::unique_ptr<rdbms::DbConn> m_conn;

  /**
   * Creates the database schema.
   */
  void createDbSchema();

  /**
   * Returns true if the specified user name is listed in the ADMIN_USER table.
   *
   * @param userName The name of the user to be search for in the ADMIN_USER
   * table.
   * @return true if the specified user name is listed in the ADMIN_USER table.
   */
  bool userIsAdmin(const std::string &userName) const;

  /**
   * Returns true if the specified host name is listed in the ADMIN_HOST table.
   *
   * @param userName The name of the host to be search for in the ADMIN_HOST
   * table.
   * @return true if the specified host name is listed in the ADMIN_HOST table.
   */
  bool hostIsAdmin(const std::string &userName) const;

  /**
   * Returns the expected number of archive routes for the specified storage
   * class as specified by the call to the createStorageClass() method as
   * opposed to the actual number entered so far using the createArchiveRoute()
   * method.
   *
   * @return The expected number of archive routes.
   */
  uint64_t getExpectedNbArchiveRoutes(const std::string &storageClass) const;

  /**
   * Inserts the specified tape file into the Tape table.
   *
   * @param tapeFile The tape file.
   * @param archiveFileId The identifier of the archive file of which the tape
   * file is a copy.
   */
  void insertTapeFile(const common::dataStructures::TapeFile &tapeFile, const uint64_t archiveFileId);

  /**
   * Sets the last FSeq of the specified tape to the specified value.
   *
   * @param vid The volume identifier of the tape.
   * @param lastFseq The new value of the last FSeq.
   */
  void setTapeLastFSeq(const std::string &vid, const uint64_t lastFSeq);

  /**
   * Returns the last FSeq of the specified tape.
   *
   * @param vid The volume identifier of the tape.
   * @return The last FSeq.
   */
  uint64_t getTapeLastFSeq(const std::string &vid) const;

  /**
   * Updates the appropriate tape based on the ocuurence of the specified event.
   *
   * @param event
   */
  void updateTape(const TapeFileWritten &event);

  /**
   * Returns the specified archive file or a NULL pointer if it does not exist.
   *
   * @param archiveFileId The identifier of the archive file.
   * @return The archive file or NULL.
   * an empty list.
   */
  std::unique_ptr<common::dataStructures::ArchiveFile> getArchiveFile(const uint64_t archiveFileId) const;

  /**
   * Throws an exception if the there is a mismatch between the expected and
   * actual common event data.
   *
   * @param expected The expected event data.
   * @param actual The actual event data.
   */
  void throwIfCommonEventDataMismatch(const common::dataStructures::ArchiveFile &expected,
    const TapeFileWritten &actual) const;

  /**
   * Returns the mount policies for the specified requester and requester group.
   *
   * @param diskInstanceName The name of the disk instance to which the
   * requester and requester group belong.
   * @param requesterName The name of the requester which is only guaranteed to
   * be unique within its disk instance.
   * @param requesterGroupName The name of the requester group which is only
   * guaranteed to be unique within its disk instance.
   * @return The mount policies.
   */
  RequesterAndGroupMountPolicies getMountPolicies(
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
   */
  virtual uint64_t getNextArchiveFileId() = 0;

  /**
   * Selects the specified tape within th eTape table for update.
   *
   * This method must be implemented by the sub-classes of RdbmsCatalogue
   * because some database technologies directly support SELECT FOR UPDATE
   * whilst others do not.
   *
   * @param vid The volume identifier of the tape.
   */
  virtual common::dataStructures::Tape selectTapeForUpdate(const std::string &vid) = 0;

  /**
   * Nested class used to implement the getArchiveFileItor() method.
   */
  class ArchiveFileItorImpl: public ArchiveFileItor {
  public:

    /**
     * Constructor.
     *
     * @param catalogue The RdbmsCatalogue.
     * @param nbArchiveFilesToPrefetch The number of archive files to prefetch.
     * @param searchCriteria The criteria used to select the archive files.
     */
    ArchiveFileItorImpl(
      const RdbmsCatalogue &catalogue,
      const uint64_t nbArchiveFilesToPrefetch,
      const ArchiveFileSearchCriteria &searchCriteria);

    /**
     * Destructor.
     */
    virtual ~ArchiveFileItorImpl();

    /**
     * Returns true if a call to next would return another archive file.
     */
    virtual bool hasMore() const;

    /**
     * Returns the next archive or throws an exception if there isn't one.
     */
    virtual common::dataStructures::ArchiveFile next();

  private:

    /**
     * The RdbmsCatalogue.
     */
    const RdbmsCatalogue &m_catalogue;

    /**
     * The number of archive files to prefetch.
     */
    const uint64_t m_nbArchiveFilesToPrefetch;

    /**
     * The criteria used to select the archive files.
     */
    ArchiveFileSearchCriteria m_searchCriteria;

    /**
     * The current offset into the list of archive files in the form of an
     * archive file ID.
     */
    uint64_t m_nextArchiveFileId;

    /**
     * The current list of prefetched archive files.
     */
    std::list<common::dataStructures::ArchiveFile> m_prefechedArchiveFiles;
  };

  /**
   * Returns the specified archive files.  This method is called by the nested
   * class ArchiveFileItorImpl.  Please note that the list of files is ordered
   * by archive file IDs.
   *
   * @param startingArchiveFileId The unique identifier of the first archive
   * file to be returned.
   * @param nbArchiveFiles The maximum number of archive files to be returned.
   * @param searchCriteria The criteria used to select the archive files.
   * @return The archive files.
   */
  std::list<common::dataStructures::ArchiveFile> getArchiveFilesForItor(
    const uint64_t startingArchiveFileId,
    const uint64_t maxNbArchiveFiles,
    const ArchiveFileSearchCriteria &searchCriteria) const;

  /**
   * Returns the mapping from tape copy to tape pool for the specified storage
   * class.
   *
   * @param diskInstanceName The name of the disk instance to which the storage
   * class belongs.
   * @param storageClassName The name of the storage class which is only
   * guaranteed to be unique within its disk instance.
   * @return The mapping from tape copy to tape pool for the specified storage
   * class.
   */
  virtual common::dataStructures::TapeCopyToPoolMap getTapeCopyToPoolMap(const std::string &diskInstanceName,
    const std::string &storageClassName) const;

}; // class RdbmsCatalogue

} // namespace catalogue
} // namespace cta

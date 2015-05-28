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

#include "scheduler/SchedulerDatabase.hpp"

#include <sqlite3.h>

namespace cta {

/**
 * A mock implementation of the database of a tape resource scheduler.
 */
class MockSchedulerDatabase: public SchedulerDatabase {
public:

  /**
   * Constructor.
   */
  MockSchedulerDatabase();

  /**
   * Destructor.
   */
  ~MockSchedulerDatabase() throw();

  /**
   * Queues the specified request and its associated file transfers.
   *
   * @param rqst The request.
   * @param fileTransfers The file transfers.
   */
  void queue(const ArchiveToDirRequest &rqst,
   const std::list<ArchivalFileTransfer> &fileTransfers);

  /**
   * Queues the specified request and its associated file transfers.
   *
   * @param rqst The request.
   * @param fileTransfers The file transfers.
   */
  void queue(const ArchiveToFileRequest &rqst,
    const std::list<ArchivalFileTransfer> &fileTransfers);

  /**
   * Returns all of the existing archival jobs grouped by tape pool and then
   * sorted by creation time in ascending order (oldest first).
   *
   * @param requester The identity of the user requesting the list.
   * @return All of the existing archival jobs grouped by tape pool and then
   * sorted by creation time in ascending order (oldest first).
   */
  std::map<TapePool, std::list<ArchivalJob> > getArchivalJobs(
    const SecurityIdentity &requester) const;

  /**
   * Returns the list of archival jobs associated with the specified tape pool
   * sorted by creation time in ascending order (oldest first).
   *
   * @param requester The identity of the user requesting the list.
   * @param tapePoolName The name of the tape pool.
   * @return The list of archival jobs associated with the specified tape pool
   * sorted by creation time in ascending order (oldest first).
   */
  std::list<ArchivalJob> getArchivalJobs(
    const SecurityIdentity &requester,
    const std::string &tapePoolName) const;

  /**
   * Deletes the specified archival job.
   *
   * @param requester The identity of the user requesting the deletion of the
   * tape.
   * @param archiveFile The absolute path of the destination file within the
   * archive namespace.
   */
  void deleteArchivalJob(
    const SecurityIdentity &requester,
    const std::string &archiveFile);

  /**
   * Queues the specified request and its asscoaited file transfers.
   *
   * @param rqst The request.
   * @param fileTransfers The file transfers.
   */
  void queue(const RetrieveToDirRequest &rqst,
    const std::list<RetrievalFileTransfer> &fileTransfers);

  /**
   * Queues the specified request and its associated file transfers.
   *
   * @param rqst The request.
   * @param fileTransfers The file transfers.
   */
  void queue(const RetrieveToFileRequest &rqst,
    const std::list<RetrievalFileTransfer> &fileTransfers);

  /**
   * Returns all of the existing retrieval jobs grouped by tape and then
   * sorted by creation time in ascending order (oldest first).
   *
   * @param requester The identity of the user requesting the list.
   * @return All of the existing retrieval jobs grouped by tape and then
   * sorted by creation time in ascending order (oldest first).
   */
  std::map<Tape, std::list<RetrievalJob> > getRetrievalJobs(
    const SecurityIdentity &requester) const;

  /**
   * Returns the list of retrieval jobs associated with the specified tape
   * sorted by creation time in ascending order (oldest first).
   *
   * @param requester The identity of the user requesting the list.
   * @param vid The volume identifier of the tape.
   * @return The list of retrieval jobs associated with the specified tape
   * sorted by creation time in ascending order (oldest first).
   */
  std::list<RetrievalJob> getRetrievalJobs(
    const SecurityIdentity &requester,
    const std::string &vid) const;
  
  /**
   * Deletes the specified retrieval job.
   *
   * @param requester The identity of the user requesting the deletion of the
   * tape.
   * @param dstUrl The URL of the destination file or directory.
   */
  void deleteRetrievalJob(
    const SecurityIdentity &requester,
    const std::string &dstUrl);

  /**
   * Creates the specified administrator.
   *
   * @param requester The identity of the user requesting the creation of the
   * administrator.
   * @param user The identity of the administrator.
   * @param comment The comment describing the sministrator.
   */
  void createAdminUser(
    const SecurityIdentity &requester,
    const UserIdentity &user,
    const std::string &comment);

  /**
   * Deletes the specified administrator.
   *
   * @param requester The identity of the user requesting the deletion of the
   * administrator.
   * @param user The identity of the administrator.
   */
  void deleteAdminUser(
    const SecurityIdentity &requester,
    const UserIdentity &user);

  /**
   * Returns the current list of administrators in lexicographical order.
   *
   * @param requester The identity of the user requesting the list.
   * @return The current list of administrators in lexicographical order.
   */
  std::list<AdminUser> getAdminUsers(const SecurityIdentity &requester)
   const;

  /**
   * Creates the specified administration host.
   *
   * @param requester The identity of the user requesting the creation of the
   * administration host.
   * @param hostName The network name of the administration host.
   * @param comment The comment describing the administration host.
   */
  void createAdminHost(
    const SecurityIdentity &requester,
    const std::string &hostName,
    const std::string &comment);

  /**
   * Deletes the specified administration host.
   *
   * @param requester The identity of the user requesting the deletion of the
   * administration host.
   * @param hostName The network name of the administration host.
   */
  void deleteAdminHost(
    const SecurityIdentity &requester,
    const std::string &hostName);

  /**
   * Returns the current list of administration hosts in lexicographical order.
   *
   * @param requester The identity of the user requesting the list.
   * @return The current list of administration hosts in lexicographical order.
   */
  std::list<AdminHost> getAdminHosts(const SecurityIdentity &requester)
   const;

  /**
   * Creates the specified storage class.
   *
   * @param requester The identity of the user requesting the creation of the
   * storage class.
   * @param name The name of the storage class.
   * @param nbCopies The number of copies a file associated with this storage
   * class should have on tape.
   * @param comment The comment describing the storage class.
   */
  void createStorageClass(
    const SecurityIdentity &requester,
    const std::string &name,
    const uint16_t nbCopies,
    const std::string &comment);

  /**
   * Deletes the specified storage class.
   *
   * @param requester The identity of the user requesting the deletion of the
   * storage class.
   * @param name The name of the storage class.
   */
  void deleteStorageClass(
    const SecurityIdentity &requester,
    const std::string &name);

  /**
   * Gets the current list of storage classes in lexicographical order.
   *
   * @param requester The identity of the user requesting the list.
   * @return The current list of storage classes in lexicographical order.
   */
  std::list<StorageClass> getStorageClasses(
    const SecurityIdentity &requester) const;

  /**
   * Creates a tape pool with the specifed name.
   *
   * @param requester The identity of the user requesting the creation of the
   * tape pool.
   * @param name The name of the tape pool.
   * @param nbPartialTapes The maximum number of tapes that can be partially
   * full at any moment in time.
   * @param comment The comment describing the tape pool.
   */
  void createTapePool(
    const SecurityIdentity &requester,
    const std::string &name,
    const uint32_t nbPartialTapes,
    const std::string &comment);

  /**
   * Delete the tape pool with the specifed name.
   *
   * @param requester The identity of the user requesting the deletion of the
   * tape pool.
   * @param name The name of the tape pool.
   */
  void deleteTapePool(
    const SecurityIdentity &requester,
    const std::string &name);

  /**
   * Gets the current list of tape pools in lexicographical order.
   *
   * @param requester The identity of the user requesting the list.
   * @return The current list of tape pools in lexicographical order.
   */
  std::list<TapePool> getTapePools(
    const SecurityIdentity &requester) const;

  /**
   * Creates the specified archival route.
   *
   * @param requester The identity of the user requesting the creation of the
   * archival route.
   * @param storageClassName The name of the storage class that identifies the
   * source disk files.
   * @param copyNb The tape copy number.
   * @param tapePoolName The name of the destination tape pool.
   * @param comment The comment describing the archival route.
   */
  void createArchivalRoute(
    const SecurityIdentity &requester,
    const std::string &storageClassName,
    const uint16_t copyNb,
    const std::string &tapePoolName,
    const std::string &comment);

  /**
   * Deletes the specified archival route.
   *
   * @param requester The identity of the user requesting the deletion of the
   * archival route.
   * @param storageClassName The name of the storage class that identifies the
   * source disk files.
   * @param copyNb The tape copy number.
   */
  void deleteArchivalRoute(
    const SecurityIdentity &requester,
    const std::string &storageClassName,
    const uint16_t copyNb);

  /**
   * Gets the current list of archival routes.
   *
   * @param requester The identity of the user requesting the list.
   */
  std::list<ArchivalRoute> getArchivalRoutes(
    const SecurityIdentity &requester) const;

  /**
   * Creates a logical library with the specified name.
   *
   * @param requester The identity of the user requesting the creation of the
   * logical library.
   * @param name The name of the logical library.
   * @param comment The comment describing the logical library.
   */
  void createLogicalLibrary(
    const SecurityIdentity &requester,
    const std::string &name,
    const std::string &comment);

  /**
   * Deletes the logical library with the specified name.
   *
   * @param requester The identity of the user requesting the deletion of the
   * logical library.
   * @param name The name of the logical library.
   */
  void deleteLogicalLibrary(
    const SecurityIdentity &requester,
    const std::string &name);

  /**
   * Returns the current list of libraries in lexicographical order.
   *
   * @param requester The identity of the user requesting the list.
   * @return The current list of libraries in lexicographical order.
   */
  std::list<LogicalLibrary> getLogicalLibraries(
    const SecurityIdentity &requester) const;

  /**
   * Creates a tape.
   *
   * @param requester The identity of the user requesting the creation of the
   * tape.
   * @param vid The volume identifier of the tape.
   * @param logicalLibraryName The name of the logical library to which the tape
   * belongs.
   * @param tapePoolName The name of the tape pool to which the tape belongs.
   * @param capacityInBytes The capacity of the tape.
   * @param comment The comment describing the logical library.
   */
  void createTape(
    const SecurityIdentity &requester,
    const std::string &vid,
    const std::string &logicalLibraryName,
    const std::string &tapePoolName,
    const uint64_t capacityInBytes,
    const std::string &comment);

  /**
   * Deletes the tape with the specified volume identifier.
   *
   * @param requester The identity of the user requesting the deletion of the
   * tape.
   * @param vid The volume identifier of the tape.
   */
  void deleteTape(
    const SecurityIdentity &requester,
    const std::string &vid);

  /**
   * Returns the current list of tapes in the lexicographical order of their
   * volume identifiers.
   *
   * @param requester The identity of the user requesting the list.
   * @return The current list of tapes in the lexicographical order of their
   * volume identifiers.
   */
  std::list<Tape> getTapes(
    const SecurityIdentity &requester) const;

private:

  /**
   * SQLite database handle.
   */
  sqlite3 *m_dbHandle;

  /**
   * Creates the database schema.
   */
  void createSchema();

  /**
   * Returns the tape pool with specified name.
   *
   * @param requester The identity of the user requesting the list.
   * @param name The name of teh tape pool.
   * @return The tape pool with specified name.
   */
  TapePool getTapePoolByName(const SecurityIdentity &requester,
    const std::string &name) const;

}; // class MockSchedulerDatabase

} // namespace cta

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

#include "common/RemotePathAndStatus.hpp"
#include "scheduler/ArchiveRequest.hpp"

#include <map>
#include <stdint.h>
#include <string>

namespace cta {

/**
 * Class representing a user request to archive to a single remote file to a
 * single destination archive file.
 */
class ArchiveToFileRequest: public ArchiveRequest {
public:

  /**
   * Constructor.
   */
  ArchiveToFileRequest();

  /**
   * Destructor.
   */
  ~ArchiveToFileRequest() throw();

  /**
   * Constructor.
   *
   * @param remoteFile The path and status of the remote file to be archived.
   * @param archiveFile The full path of the destination archive file.
   * @param remoteFileStatus The status gotten from stat-ing the remote file.
   * @param copyNbToPoolMap The mapping from archive copy number to destination
   * tape pool.
   * @param priority The priority of the request.
   * @param creationLog The creation information
   */
  ArchiveToFileRequest(
    const RemotePathAndStatus &remoteFilePath,
    const std::string &archiveFile,
    const std::map<uint16_t, std::string> &copyNbToPoolMap,
    const uint64_t priority,
    const CreationLog & creationLog);

  /**
   * Returns the path and status of the remote file to be archived.
   *
   * @return The path and status of the remote file to be archived.
   */
  const RemotePathAndStatus &getRemoteFile() const throw();

  /**
   * Returns the full path of the destination archive file.
   *
   * @return The full path of the destination archive file.
   */
  const std::string &getArchiveFile() const throw();

  /**
   * Returns the mapping from archive copy number to destination tape pool.
   *
   * @return the mapping from archive copy number to destination tape pool.
   */
  const std::map<uint16_t, std::string> &getCopyNbToPoolMap() const throw();

private:

  /**
   * The path ans status of the remote file to be archived.
   */
  RemotePathAndStatus m_remoteFile;

  /**
   * The full path of the source archive file.
   */
  std::string m_archiveFile;

  /**
   * The mapping from archive copy number to destination tape pool.
   */
  std::map<uint16_t, std::string> m_copyNbToPoolMap;

}; // class ArchiveToFileRequest

} // namespace cta

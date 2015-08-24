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

#include "scheduler/RetrieveRequest.hpp"
#include "common/archiveNS/TapeFileLocation.hpp"

#include <list>
#include <string>

namespace cta {

/**
 * Class representing a user request to retrieve a single tape copy of an
 * archived file to a single remote file.
 */
struct RetrieveFromTapeCopyRequest: public RetrieveRequest {

  /**
   * Constructor.
   */
  RetrieveFromTapeCopyRequest();

  /**
   * Destructor.
   */
  ~RetrieveFromTapeCopyRequest() throw();

  /**
   * Constructor.
   *
   * @param archiveFile The full path of the source archive file.
   * @param copyNb The tape copy number.
   * @param tapeCopy The location of the tape copy.
   * @param remoteFile The URL of the destination remote file.
   * @param priority The priority of the request.
   * @param user The identity of the user who made the request.
   * @param creationTime Optionally the absolute time at which the user request
   * was created.  If no value is given then the current time is used.
   */
  RetrieveFromTapeCopyRequest(
    const std::string &archiveFile,
    const uint64_t copyNb,
    const TapeFileLocation &tapeCopy,
    const std::string &remoteFile,
    const uint64_t priority,
    const CreationLog &creationLog);

  /**
   * The full path of the source archive file.
   */
  std::string archiveFile;

  /**
   * The tape copy number.
   */
  uint64_t copyNb;

  /**
   * The location of the copy on tape.
   */
  TapeFileLocation tapeCopy;

  /**
   * The URL of the destination remote file.
   */
  std::string remoteFile;

}; // struct RetrieveFromTapeCopyRequest

} // namespace cta

/*
 * The CERN Tape Retrieve (CTA) project
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

#include "common/archiveNS/ArchiveFile.hpp"
#include "common/archiveNS/TapeFileLocation.hpp"
#include "common/exception/Exception.hpp"
#include "common/remoteFS/RemotePathAndStatus.hpp"

#include <string>

namespace cta {

class RetrieveMount;

/**
 * The transfer of a single copy of a tape file to a remote file.
 */
class RetrieveJob {
  
  /**
   * The RetrieveMount class is a friend so that it can call the private
   * constructor of RetrieveJob.
   */
  friend class RetrieveMount;

public:
  
  /**
   * Positioning methods
   */
  enum class PositioningMethod: uint8_t {
    ByBlock = 0,
    ByFSeq = 1
  };

protected:
  
  /**
   * Empty constructor. TODO: to be removed in the future when we put in the reference to the owning mount;
   */
  RetrieveJob() {}

  /**
   * Constructor. It is not public as it is generated by the RetrieveMount.
   *
   * @param mount the mount that generated this job
   * @param archiveFile informations about the file that we are storing
   * @param remotePathAndStatus location and properties of the remote file
   * @param tapeFileLocation the location of the tape file
   * @param positioningMethod the positioning method
   */
  RetrieveJob(/*RetrieveMount &mount,*/
  const ArchiveFile &archiveFile,
  const RemotePathAndStatus &remotePathAndStatus,
  const TapeFileLocation &tapeFileLocation,
  const PositioningMethod positioningMethod);

public:

  /**
   * Destructor.
   */
  virtual ~RetrieveJob() throw() = 0;

  /**
   * Indicates that the job was successful
   *
   * @param checksumOfTransfer The adler-32 checksum of the file as calculated
   * during the execution of the job.
   * @param fileSizeOfTransfer The size of the file as calculated during the
   * execution of the job.
   */
  virtual void complete(const uint32_t checksumOfTransfer,
    const uint64_t fileSizeOfTransfer);
  
  /**
   * Indicates that the job failed
   *
   * @param ex The reason for the failure.
   */
  virtual void failed(const std::exception &ex);
  
  /**
   * Indicates that the job should be tried again (typically reaching the end 
   * of the tape).
   */
  void retry();
  
  /**
   * Converts the positioning method into string
   * 
   * @param pm the positioning method
   * @return the method in string format
   */
  std::string positioningMethodtoString(PositioningMethod pm);

public:

  /**
   * The mount to which the job belongs.
   */
  //RetrieveMount &mount;  
  
  /**
   * The NS archive file information
   */
  ArchiveFile archiveFile;
  
  /**
   * The remote file information
   */
  RemotePathAndStatus remotePathAndStatus; 
                
  /**
   * The location of the tape file
   */
  TapeFileLocation tapeFileLocation;
  
  /**
   * The positioning method
   */
  PositioningMethod positioningMethod;
  
}; // class RetrieveJob

} // namespace cta

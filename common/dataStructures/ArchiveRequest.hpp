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

#include <list>
#include <map>
#include <stdint.h>
#include <string>

#include "common/dataStructures/DiskFileInfo.hpp"
#include "common/dataStructures/EntryLog.hpp"
#include "common/dataStructures/UserIdentity.hpp"

namespace cta {
namespace common {
namespace dataStructures {

/**
 * This struct holds all the command line parameters of a CTA archive command 
 */
struct ArchiveRequest {

  ArchiveRequest();

  bool operator==(const ArchiveRequest &rhs) const;

  bool operator!=(const ArchiveRequest &rhs) const;

  UserIdentity requester;
  std::string diskFileID;
  std::string instance;
  std::string srcURL;
  uint64_t fileSize;
  /**
   * The human readable checksum type. Ex: ADLER32 
   */
  std::string checksumType;
  /**
   * The human readable checksum value. Ex: 0X1292AB12 
   */
  std::string checksumValue;
  std::string storageClass;
  DiskFileInfo diskFileInfo;
  std::string diskpoolName;
  uint64_t diskpoolThroughput;
  EntryLog creationLog;

}; // struct ArchiveRequest

std::ostream &operator<<(std::ostream &os, const ArchiveRequest &obj);

} // namespace dataStructures
} // namespace common
} // namespace cta

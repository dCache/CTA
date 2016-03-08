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

#include "common/dataStructures/EntryLog.hpp"
#include "common/dataStructures/RepackType.hpp"

namespace cta {
namespace common {
namespace dataStructures {

struct RepackInfo {


  bool operator==(const RepackInfo &rhs) const;

  bool operator!=(const RepackInfo &rhs) const;

  cta::common::dataStructures::EntryLog creationLog;
  std::map<uint64_t,std::string> errors;
  uint64_t filesArchived;
  uint64_t filesFailed;
  uint64_t filesToArchive;
  uint64_t filesToRetrieve;
  std::string repackStatus;
  cta::common::dataStructures::RepackType repackType;
  std::string tag;
  uint64_t totalFiles;
  uint64_t totalSize;
  std::string vid;

}; // struct RepackInfo

} // namespace dataStructures
} // namespace common
} // namespace cta

std::ostream &operator<<(std::ostream &os, const cta::common::dataStructures::RepackInfo &obj);

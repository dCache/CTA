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


namespace cta {
namespace common {
namespace dataStructures {

struct WriteTestResult {

  ~WriteTestResult() throw();

  bool operator==(const WriteTestResult &rhs) const;

  bool operator!=(const WriteTestResult &rhs) const;

  std::map<uint64_t,std::pair<std::string,std::string>> checksums;
  std::string driveName;
  std::map<uint64_t,std::string> errors;
  uint64_t noOfFilesWritten;
  uint64_t totalBytesWritten;
  uint64_t totalFilesWritten;
  uint64_t totalTimeInSeconds;
  std::string vid;

}; // struct WriteTestResult

} // namespace dataStructures
} // namespace common
} // namespace cta

std::ostream &operator<<(std::ostream &os, const cta::common::dataStructures::WriteTestResult &obj);

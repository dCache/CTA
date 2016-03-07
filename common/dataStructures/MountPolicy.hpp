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

struct MountPolicy {

  ~MountPolicy() throw();

  bool operator==(const MountPolicy &rhs) const;

  bool operator!=(const MountPolicy &rhs) const;

  uint64_t maxDrives;
  uint64_t minBytesQueued;
  uint64_t minFilesQueued;
  uint64_t minRequestAge;
  uint64_t priority;

}; // struct MountPolicy

} // namespace dataStructures
} // namespace common
} // namespace cta

std::ostream &operator<<(std::ostream &os, const cta::common::dataStructures::MountPolicy &obj);

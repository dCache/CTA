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

namespace cta {
namespace common {
namespace dataStructures {

/**
 * The archive route specifies which tape pool will be used as a destination of 
 * a specific copy of a storage class 
 */
struct ArchiveRoute {

  ArchiveRoute();

  bool operator==(const ArchiveRoute &rhs) const;

  bool operator!=(const ArchiveRoute &rhs) const;

  std::string storageClassName;
  uint64_t copyNb;
  std::string tapePoolName;
  EntryLog creationLog;
  EntryLog lastModificationLog;
  std::string comment;

}; // struct ArchiveRoute

std::ostream &operator<<(std::ostream &os, const ArchiveRoute &obj);

} // namespace dataStructures
} // namespace common
} // namespace cta

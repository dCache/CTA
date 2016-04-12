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

#include "common/dataStructures/MountGroup.hpp"
#include "common/dataStructures/utils.hpp"
#include "common/exception/Exception.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::common::dataStructures::MountGroup::MountGroup():
  archive_priority(0),
  archive_minRequestAge(0),
  retrieve_priority(0),
  retrieve_minRequestAge(0),
  maxDrivesAllowed(0) {}

//------------------------------------------------------------------------------
// operator==
//------------------------------------------------------------------------------
bool cta::common::dataStructures::MountGroup::operator==(const MountGroup &rhs) const {
  return name==rhs.name
      && archive_priority==rhs.archive_priority
      && archive_minRequestAge==rhs.archive_minRequestAge
      && retrieve_priority==rhs.retrieve_priority
      && retrieve_minRequestAge==rhs.retrieve_minRequestAge
      && maxDrivesAllowed==rhs.maxDrivesAllowed
      && creationLog==rhs.creationLog
      && lastModificationLog==rhs.lastModificationLog
      && comment==rhs.comment;
}

//------------------------------------------------------------------------------
// operator!=
//------------------------------------------------------------------------------
bool cta::common::dataStructures::MountGroup::operator!=(const MountGroup &rhs) const {
  return !operator==(rhs);
}

//------------------------------------------------------------------------------
// operator<<
//------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const cta::common::dataStructures::MountGroup &obj) {
  os << "(name=" << obj.name
     << " archive_priority=" << obj.archive_priority
     << " archive_minRequestAge=" << obj.archive_minRequestAge
     << " retrieve_priority=" << obj.retrieve_priority
     << " retrieve_minRequestAge=" << obj.retrieve_minRequestAge
     << " maxDrivesAllowed=" << obj.maxDrivesAllowed
     << " creationLog=" << obj.creationLog
     << " lastModificationLog=" << obj.lastModificationLog
     << " comment=" << obj.comment << ")";
  return os;
}


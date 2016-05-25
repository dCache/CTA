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

#include "scheduler/ArchiveToTapeCopyRequest.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::ArchiveToTapeCopyRequest::ArchiveToTapeCopyRequest() {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::ArchiveToTapeCopyRequest::~ArchiveToTapeCopyRequest() throw() {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::ArchiveToTapeCopyRequest::ArchiveToTapeCopyRequest(
  const std::string & diskFileID,
  const uint64_t archiveFileID,
  const uint16_t copyNb,
  const std::string tapePoolName,
  const uint64_t priority,
  const common::dataStructures::EntryLog &entryLog):
  UserArchiveRequest(priority, entryLog),
  diskFileID(diskFileID),
  archiveFileID(archiveFileID),
  copyNb(copyNb),
  tapePoolName(tapePoolName) {
}

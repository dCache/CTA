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

#include "common/archiveNS/ArchiveFileStatus.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::common::archiveNS::ArchiveFileStatus::ArchiveFileStatus():
  fileId(0),
  mode(0),
  size(0) {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::common::archiveNS::ArchiveFileStatus::ArchiveFileStatus(
  const UserIdentity &owner,
  const uint64_t fileId,
  const mode_t mode,
  const uint64_t size,
  const Checksum &checksum,
  const std::string &storageClassName):
  owner(owner),
  fileId(fileId),
  mode(mode),
  size(size),
  checksum(checksum),
  storageClassName(storageClassName) {
}

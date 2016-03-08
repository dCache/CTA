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

#include "common/dataStructures/RetrieveRequest.hpp"
#include "common/dataStructures/utils.hpp"
#include "common/exception/Exception.hpp"

//------------------------------------------------------------------------------
// operator==
//------------------------------------------------------------------------------
bool cta::common::dataStructures::RetrieveRequest::operator==(const RetrieveRequest &rhs) const {
  return archiveFileID==rhs.archiveFileID
      && creationLog==rhs.creationLog
      && diskpoolName==rhs.diskpoolName
      && diskpoolThroughput==rhs.diskpoolThroughput
      && drData==rhs.drData
      && dstURL==rhs.dstURL
      && requester==rhs.requester;
}

//------------------------------------------------------------------------------
// operator!=
//------------------------------------------------------------------------------
bool cta::common::dataStructures::RetrieveRequest::operator!=(const RetrieveRequest &rhs) const {
  return !operator==(rhs);
}

//------------------------------------------------------------------------------
// operator<<
//------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const cta::common::dataStructures::RetrieveRequest &obj) {
  os << "(archiveFileID=" << obj.archiveFileID
     << " creationLog=" << obj.creationLog
     << " diskpoolName=" << obj.diskpoolName
     << " diskpoolThroughput=" << obj.diskpoolThroughput
     << " drData=" << obj.drData
     << " dstURL=" << obj.dstURL
     << " requester=" << obj.requester << ")";
  return os;
}


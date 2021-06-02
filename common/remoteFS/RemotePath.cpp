/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common/remoteFS/RemotePath.hpp"
#include "common/exception/Exception.hpp"

#include <sstream>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::RemotePath::RemotePath() {
}

//------------------------------------------------------------------------------
// setter
//------------------------------------------------------------------------------
void cta::RemotePath::setPath(const std::string &raw)
{
  m_raw = raw;
  auto const pos = raw.find(':');

  if(std::string::npos == pos) {
    std::ostringstream msg;
    msg << "Failed to instantiate RemotePath object for URI " << raw <<
      " because the colon is missing as in scheme:hierarchical_part";
    throw exception::Exception(msg.str());
  }

  if(0 >= pos) {
    std::ostringstream msg;
    msg << "Failed to instantiate RemotePath object for URI " << raw <<
     " because there is no scheme";
    throw exception::Exception(msg.str());
  }

  const auto indexOfLastChar = raw.length() - 1;
  if(pos == indexOfLastChar) {
    std::ostringstream msg;
    msg << "Failed to instantiate RemotePath object for URI " << raw <<
     " because there nothing after the scheme";
    throw exception::Exception(msg.str());
  }

  m_scheme = raw.substr(0, pos);
  m_afterScheme = raw.substr(pos + 1, indexOfLastChar - pos);
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::RemotePath::RemotePath(const std::string& raw) {
  setPath(raw);
}


//------------------------------------------------------------------------------
// operator==
//------------------------------------------------------------------------------
bool cta::RemotePath::operator==(const RemotePath &rhs) const {
  return m_raw == rhs.m_raw;
}

//------------------------------------------------------------------------------
// operator<
//------------------------------------------------------------------------------
bool cta::RemotePath::operator<(const RemotePath &rhs) const {
  return m_raw < rhs.m_raw;
}

//------------------------------------------------------------------------------
// empty
//------------------------------------------------------------------------------
bool cta::RemotePath::empty() const {
  return m_raw.empty();
}

//------------------------------------------------------------------------------
// getRaw
//------------------------------------------------------------------------------
const std::string &cta::RemotePath::getRaw() const {
  if(empty()) {
    throw exception::Exception(std::string(__FUNCTION__) +
      ": Empty remote path");
  }

  return m_raw;
}

//------------------------------------------------------------------------------
// getScheme
//------------------------------------------------------------------------------
const std::string &cta::RemotePath::getScheme() const {
  if(empty()) {
    throw exception::Exception(std::string(__FUNCTION__) +
      ": Empty remote path");
  }

  return m_scheme;
}

//------------------------------------------------------------------------------
// getAfterScheme
//------------------------------------------------------------------------------
const std::string &cta::RemotePath::getAfterScheme() const {
  if(empty()) {
    throw exception::Exception(std::string(__FUNCTION__) +
      ": Empty remote path");
  }

  return m_afterScheme;
}

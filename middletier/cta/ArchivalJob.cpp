/**
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

#include "cta/ArchivalJob.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::ArchivalJob::ArchivalJob():
  m_state(ArchivalJobState::NONE),
  m_creationTime(time(NULL)) {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::ArchivalJob::ArchivalJob(
  const ArchivalJobState::Enum state,
  const std::string &srcUrl,
  const std::string &dstPath,
  const UserIdentity &creator,
  const time_t creationTime):
  m_state(state),
  m_srcUrl(srcUrl),
  m_dstPath(dstPath),
  m_creationTime(creationTime),
  m_creator(creator) {
}

//------------------------------------------------------------------------------
// setState
//------------------------------------------------------------------------------
void cta::ArchivalJob::setState(const ArchivalJobState::Enum state) {
  m_state = state;
}

//------------------------------------------------------------------------------
// getState
//------------------------------------------------------------------------------
cta::ArchivalJobState::Enum cta::ArchivalJob::getState() const throw() {
  return m_state;
}

//------------------------------------------------------------------------------
// getStateStr
//------------------------------------------------------------------------------
const char *cta::ArchivalJob::getStateStr() const throw() {
  return ArchivalJobState::toStr(m_state);
}

//------------------------------------------------------------------------------
// getSrcUrl
//------------------------------------------------------------------------------
const std::string &cta::ArchivalJob::getSrcUrl() const throw() {
  return m_srcUrl;
}

//------------------------------------------------------------------------------
// getDstPath
//------------------------------------------------------------------------------
const std::string &cta::ArchivalJob::getDstPath() const throw() {
  return m_dstPath;
}

//------------------------------------------------------------------------------
// getCreationTime
//------------------------------------------------------------------------------
time_t cta::ArchivalJob::getCreationTime() const throw() {
  return m_creationTime;
}

//------------------------------------------------------------------------------
// getCreator
//------------------------------------------------------------------------------
const cta::UserIdentity &cta::ArchivalJob::getCreator() const throw() {
  return m_creator;
}

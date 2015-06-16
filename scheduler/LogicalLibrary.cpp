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

#include "scheduler/LogicalLibrary.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::LogicalLibrary::LogicalLibrary() {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::LogicalLibrary::~LogicalLibrary() throw() {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::LogicalLibrary::LogicalLibrary(
  const std::string &name,
  const CreationLog &creationLog):
  m_name(name), m_creationLog(creationLog) {
}

//------------------------------------------------------------------------------
// getName
//------------------------------------------------------------------------------
const std::string &cta::LogicalLibrary::getName() const throw() {
  return m_name;
}

//------------------------------------------------------------------------------
// getCreationLog
//------------------------------------------------------------------------------
auto cta::LogicalLibrary::getCreationLog() const throw() -> const CreationLog & {
  return m_creationLog;
}

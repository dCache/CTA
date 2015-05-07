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

#include "cta/ArchivalRoute.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::ArchivalRoute::ArchivalRoute():
  m_copyNb(0) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::ArchivalRoute::~ArchivalRoute() throw() {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::ArchivalRoute::ArchivalRoute(
  const std::string &storageClassName,
  const uint16_t copyNb,
  const std::string &tapePoolName,
  const UserIdentity &creator,
  const std::string &comment,
  const time_t creationTime):
  ConfigurationItem(creator, comment, creationTime),
  m_storageClassName(storageClassName),
  m_copyNb(copyNb),
  m_tapePoolName(tapePoolName) {
}

//------------------------------------------------------------------------------
// getStorageClassName
//------------------------------------------------------------------------------
const std::string &cta::ArchivalRoute::getStorageClassName() const throw() {
  return m_storageClassName;
}

//------------------------------------------------------------------------------
// getCopyNb
//------------------------------------------------------------------------------
uint16_t cta::ArchivalRoute::getCopyNb() const throw() {
  return m_copyNb;
}

//------------------------------------------------------------------------------
// getTapePoolName
//------------------------------------------------------------------------------
const std::string &cta::ArchivalRoute::getTapePoolName() const throw() {
  return m_tapePoolName;
}

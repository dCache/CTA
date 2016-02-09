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

#include "common/dataStructures/ArchiveRoute.hpp"
#include "common/exception/Exception.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::dataStructures::ArchiveRoute::ArchiveRoute() {  
  m_commentSet = false;
  m_copyNbSet = false;
  m_creationLogSet = false;
  m_lastModificationLogSet = false;
  m_storageClassNameSet = false;
  m_tapePoolNameSet = false;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::dataStructures::ArchiveRoute::~ArchiveRoute() throw() {
}

//------------------------------------------------------------------------------
// allFieldsSet
//------------------------------------------------------------------------------
bool cta::dataStructures::ArchiveRoute::allFieldsSet() const {
  return m_commentSet
      && m_copyNbSet
      && m_creationLogSet
      && m_lastModificationLogSet
      && m_storageClassNameSet
      && m_tapePoolNameSet;
}

//------------------------------------------------------------------------------
// setComment
//------------------------------------------------------------------------------
void cta::dataStructures::ArchiveRoute::setComment(const std::string &comment) {
  m_comment = comment;
  m_commentSet = true;
}

//------------------------------------------------------------------------------
// getComment
//------------------------------------------------------------------------------
std::string cta::dataStructures::ArchiveRoute::getComment() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the ArchiveRoute have been set!");
  }
  return m_comment;
}

//------------------------------------------------------------------------------
// setCopyNb
//------------------------------------------------------------------------------
void cta::dataStructures::ArchiveRoute::setCopyNb(const uint32_t copyNb) {
  m_copyNb = copyNb;
  m_copyNbSet = true;
}

//------------------------------------------------------------------------------
// getCopyNb
//------------------------------------------------------------------------------
uint32_t cta::dataStructures::ArchiveRoute::getCopyNb() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the ArchiveRoute have been set!");
  }
  return m_copyNb;
}

//------------------------------------------------------------------------------
// setCreationLog
//------------------------------------------------------------------------------
void cta::dataStructures::ArchiveRoute::setCreationLog(const cta::dataStructures::EntryLog &creationLog) {
  m_creationLog = creationLog;
  m_creationLogSet = true;
}

//------------------------------------------------------------------------------
// getCreationLog
//------------------------------------------------------------------------------
cta::dataStructures::EntryLog cta::dataStructures::ArchiveRoute::getCreationLog() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the ArchiveRoute have been set!");
  }
  return m_creationLog;
}

//------------------------------------------------------------------------------
// setLastModificationLog
//------------------------------------------------------------------------------
void cta::dataStructures::ArchiveRoute::setLastModificationLog(const cta::dataStructures::EntryLog &lastModificationLog) {
  m_lastModificationLog = lastModificationLog;
  m_lastModificationLogSet = true;
}

//------------------------------------------------------------------------------
// getLastModificationLog
//------------------------------------------------------------------------------
cta::dataStructures::EntryLog cta::dataStructures::ArchiveRoute::getLastModificationLog() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the ArchiveRoute have been set!");
  }
  return m_lastModificationLog;
}

//------------------------------------------------------------------------------
// setStorageClassName
//------------------------------------------------------------------------------
void cta::dataStructures::ArchiveRoute::setStorageClassName(const std::string &storageClassName) {
  m_storageClassName = storageClassName;
  m_storageClassNameSet = true;
}

//------------------------------------------------------------------------------
// getStorageClassName
//------------------------------------------------------------------------------
std::string cta::dataStructures::ArchiveRoute::getStorageClassName() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the ArchiveRoute have been set!");
  }
  return m_storageClassName;
}

//------------------------------------------------------------------------------
// setTapePoolName
//------------------------------------------------------------------------------
void cta::dataStructures::ArchiveRoute::setTapePoolName(const std::string &tapePoolName) {
  m_tapePoolName = tapePoolName;
  m_tapePoolNameSet = true;
}

//------------------------------------------------------------------------------
// getTapePoolName
//------------------------------------------------------------------------------
std::string cta::dataStructures::ArchiveRoute::getTapePoolName() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the ArchiveRoute have been set!");
  }
  return m_tapePoolName;
}

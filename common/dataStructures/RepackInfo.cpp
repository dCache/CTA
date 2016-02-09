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

#include "common/dataStructures/RepackInfo.hpp"
#include "common/exception/Exception.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::dataStructures::RepackInfo::RepackInfo() {  
  m_creationLogSet = false;
  m_errorsSet = false;
  m_filesFailedSet = false;
  m_filesMigratedSet = false;
  m_filesToMigrSet = false;
  m_filesToRecallSet = false;
  m_repackStatusSet = false;
  m_repackTypeSet = false;
  m_tagSet = false;
  m_totalFilesSet = false;
  m_totalSizeSet = false;
  m_vidSet = false;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::dataStructures::RepackInfo::~RepackInfo() throw() {
}

//------------------------------------------------------------------------------
// allFieldsSet
//------------------------------------------------------------------------------
bool cta::dataStructures::RepackInfo::allFieldsSet() const {
  return m_creationLogSet
      && m_errorsSet
      && m_filesFailedSet
      && m_filesMigratedSet
      && m_filesToMigrSet
      && m_filesToRecallSet
      && m_repackStatusSet
      && m_repackTypeSet
      && m_tagSet
      && m_totalFilesSet
      && m_totalSizeSet
      && m_vidSet;
}

//------------------------------------------------------------------------------
// setCreationLog
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setCreationLog(const cta::dataStructures::EntryLog &creationLog) {
  m_creationLog = creationLog;
  m_creationLogSet = true;
}

//------------------------------------------------------------------------------
// getCreationLog
//------------------------------------------------------------------------------
cta::dataStructures::EntryLog cta::dataStructures::RepackInfo::getCreationLog() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_creationLog;
}

//------------------------------------------------------------------------------
// setErrors
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setErrors(const std::map<int,std::string> &errors) {
  m_errors = errors;
  m_errorsSet = true;
}

//------------------------------------------------------------------------------
// getErrors
//------------------------------------------------------------------------------
std::map<int,std::string> cta::dataStructures::RepackInfo::getErrors() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_errors;
}

//------------------------------------------------------------------------------
// setFilesFailed
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setFilesFailed(const uint64_t filesFailed) {
  m_filesFailed = filesFailed;
  m_filesFailedSet = true;
}

//------------------------------------------------------------------------------
// getFilesFailed
//------------------------------------------------------------------------------
uint64_t cta::dataStructures::RepackInfo::getFilesFailed() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_filesFailed;
}

//------------------------------------------------------------------------------
// setFilesMigrated
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setFilesMigrated(const uint64_t filesMigrated) {
  m_filesMigrated = filesMigrated;
  m_filesMigratedSet = true;
}

//------------------------------------------------------------------------------
// getFilesMigrated
//------------------------------------------------------------------------------
uint64_t cta::dataStructures::RepackInfo::getFilesMigrated() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_filesMigrated;
}

//------------------------------------------------------------------------------
// setFilesToMigr
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setFilesToMigr(const uint64_t filesToMigr) {
  m_filesToMigr = filesToMigr;
  m_filesToMigrSet = true;
}

//------------------------------------------------------------------------------
// getFilesToMigr
//------------------------------------------------------------------------------
uint64_t cta::dataStructures::RepackInfo::getFilesToMigr() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_filesToMigr;
}

//------------------------------------------------------------------------------
// setFilesToRecall
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setFilesToRecall(const uint64_t filesToRecall) {
  m_filesToRecall = filesToRecall;
  m_filesToRecallSet = true;
}

//------------------------------------------------------------------------------
// getFilesToRecall
//------------------------------------------------------------------------------
uint64_t cta::dataStructures::RepackInfo::getFilesToRecall() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_filesToRecall;
}

//------------------------------------------------------------------------------
// setRepackStatus
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setRepackStatus(const std::string &repackStatus) {
  m_repackStatus = repackStatus;
  m_repackStatusSet = true;
}

//------------------------------------------------------------------------------
// getRepackStatus
//------------------------------------------------------------------------------
std::string cta::dataStructures::RepackInfo::getRepackStatus() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_repackStatus;
}

//------------------------------------------------------------------------------
// setRepackType
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setRepackType(const cta::dataStructures::RepackType &repackType) {
  m_repackType = repackType;
  m_repackTypeSet = true;
}

//------------------------------------------------------------------------------
// getRepackType
//------------------------------------------------------------------------------
cta::dataStructures::RepackType cta::dataStructures::RepackInfo::getRepackType() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_repackType;
}

//------------------------------------------------------------------------------
// setTag
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setTag(const std::string &tag) {
  m_tag = tag;
  m_tagSet = true;
}

//------------------------------------------------------------------------------
// getTag
//------------------------------------------------------------------------------
std::string cta::dataStructures::RepackInfo::getTag() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_tag;
}

//------------------------------------------------------------------------------
// setTotalFiles
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setTotalFiles(const uint64_t totalFiles) {
  m_totalFiles = totalFiles;
  m_totalFilesSet = true;
}

//------------------------------------------------------------------------------
// getTotalFiles
//------------------------------------------------------------------------------
uint64_t cta::dataStructures::RepackInfo::getTotalFiles() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_totalFiles;
}

//------------------------------------------------------------------------------
// setTotalSize
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setTotalSize(const uint64_t totalSize) {
  m_totalSize = totalSize;
  m_totalSizeSet = true;
}

//------------------------------------------------------------------------------
// getTotalSize
//------------------------------------------------------------------------------
uint64_t cta::dataStructures::RepackInfo::getTotalSize() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_totalSize;
}

//------------------------------------------------------------------------------
// setVid
//------------------------------------------------------------------------------
void cta::dataStructures::RepackInfo::setVid(const std::string &vid) {
  m_vid = vid;
  m_vidSet = true;
}

//------------------------------------------------------------------------------
// getVid
//------------------------------------------------------------------------------
std::string cta::dataStructures::RepackInfo::getVid() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the RepackInfo have been set!");
  }
  return m_vid;
}

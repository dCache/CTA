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

#include <map>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/exception/Exception.hpp"
#include "common/remoteFS/RemotePath.hpp"
#include "common/Utils.hpp"
#include "remotens/MockRemoteNS.hpp"


//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::MockRemoteNS::MockRemoteNS() {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::MockRemoteNS::MockRemoteNS(std::map<RemotePath, RemoteFileStatus> &entries): m_entries(entries){
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::MockRemoteNS::~MockRemoteNS() throw() {
}

//------------------------------------------------------------------------------
// statFile
//------------------------------------------------------------------------------
std::unique_ptr<cta::RemoteFileStatus> cta::MockRemoteNS::statFile(
  const RemotePath &path) const {
  auto it = m_entries.find(path);
  if(m_entries.end() == it) {
    return std::unique_ptr<RemoteFileStatus>();
  }
  return std::unique_ptr<RemoteFileStatus>(
    new RemoteFileStatus(m_entries.at(path)));
}

//------------------------------------------------------------------------------
// rename
//------------------------------------------------------------------------------
void cta::MockRemoteNS::rename(const RemotePath &remoteFile,
  const RemotePath &newRemoteFile) {
  auto it = m_entries.find(newRemoteFile);
  if(m_entries.end() != it) {
    throw exception::Exception("MockRemoteNS: destination path exists, cannot overwrite it");
  }
  it = m_entries.find(remoteFile);
  if(m_entries.end() == it) {
    throw exception::Exception("MockRemoteNS: source path does not exist");
  }
  m_entries[newRemoteFile] = m_entries[remoteFile];
  m_entries.erase(remoteFile);
}

//------------------------------------------------------------------------------
// getFilename
//------------------------------------------------------------------------------
std::string cta::MockRemoteNS::getFilename(const RemotePath &remoteFile) const {
  const std::string afterScheme = remoteFile.getAfterScheme();
  const std::string afterSchemeTrimmed = Utils::trimSlashes(afterScheme);
  return Utils::getEnclosedName(afterSchemeTrimmed);
}

//------------------------------------------------------------------------------
// createEntry
//------------------------------------------------------------------------------
void cta::MockRemoteNS::createEntry(const RemotePath &path,
  const RemoteFileStatus &status){
  auto it = m_entries.find(path);
  if(m_entries.end() != it) {
    throw exception::Exception("MockRemoteNS: path exists, cannot overwrite it");
  }
  m_entries[path] = status;
}

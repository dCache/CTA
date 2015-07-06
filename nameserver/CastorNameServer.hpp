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

#pragma once

#include "common/ArchiveDirIterator.hpp"
#include "common/UserIdentity.hpp"
#include "nameserver/NameServer.hpp"
#include "scheduler/SecurityIdentity.hpp"

#include <list>
#include <string>

namespace cta {

/**
 * The Castor Name Server
 */
class CastorNameServer: public NameServer {

public:

  /**
   * Constructor.
   */
  CastorNameServer();

  /**
   * Destructor.
   */
  ~CastorNameServer() throw();  
  
  void setDirStorageClass(const SecurityIdentity &requester, const std::string &path, const std::string &storageClassName);
  
  void clearDirStorageClass(const SecurityIdentity &requester, const std::string &path);
  
  std::string getDirStorageClass(const SecurityIdentity &requester, const std::string &path) const;
  
  void createFile(const SecurityIdentity &requester, const std::string &path, const mode_t mode);

  void setOwner(const SecurityIdentity &requester, const std::string &path, const UserIdentity &owner);

  UserIdentity getOwner(const SecurityIdentity &requester, const std::string &path) const;
  
  void createDir(const SecurityIdentity &requester, const std::string &path, const mode_t mode);
  
  void deleteFile(const SecurityIdentity &requester, const std::string &path);
  
  void deleteDir(const SecurityIdentity &requester, const std::string &path);
  
  ArchiveFileStatus statFile(const SecurityIdentity &requester, const std::string &path) const;
  
  ArchiveDirIterator getDirContents(const SecurityIdentity &requester, const std::string &path) const;
  
  bool regularFileExists(const SecurityIdentity &requester, const std::string &path) const;

  bool dirExists(const SecurityIdentity &requester, const std::string &path) const;
  
  std::string getVidOfFile(const SecurityIdentity &requester, const std::string &path, const uint16_t copyNb) const;
  
  void assertStorageClassIsNotInUse(const SecurityIdentity &requester, const std::string &storageClass, const std::string &path) const;
  
  void createStorageClass(const SecurityIdentity &requester, const std::string &name, const uint16_t nbCopies);

  void deleteStorageClass(const SecurityIdentity &requester, const std::string &name);
  
  void updateStorageClass(const SecurityIdentity &requester, const std::string &name, const uint16_t nbCopies);

private:
  
  /**
   * The castor name server host
   */
  std::string m_server;
  
  /**
   * Gets the list of directory entries of a specific path
   * @param requester The identity of the requester.
   * @param path      The absolute path of the directory to get the entries from.
   * @return the list of directory entries of the specified path
   */
  std::list<cta::ArchiveDirEntry> getDirEntries(const SecurityIdentity &requester, const std::string &path) const;

  /**
   * Returns the directory entry corresponding to the specified path.
   *
   * @param requester The identity of the requester.
   * @param The absolute path of the namespace entry.
   */
  ArchiveDirEntry getArchiveDirEntry(const SecurityIdentity &requester, const std::string &path) const;
  
}; // class CastorNameServer

} // namespace cta

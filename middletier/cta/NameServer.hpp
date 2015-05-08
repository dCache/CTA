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

#pragma once

#include "cta/DirIterator.hpp"
#include "cta/SecurityIdentity.hpp"

#include <string>

namespace cta {

/**
 * Abstract class specifying the interface to the name server containing the
 * archive namespace.
 */
class NameServer {
public:

  /**
   * Destructor.
   */
  virtual ~NameServer() throw() = 0;

  /**
   * Creates the specified directory.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the directory.
   * @param mode The mode bits of the directory entry.
   */
  virtual void createDir(
    const SecurityIdentity &requester,
    const std::string &path,
    const uint16_t mode) = 0;

  /**
   * Deletes the specified directory.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the directory.
   */
  virtual void deleteDir(
   const SecurityIdentity &requester,
   const std::string &path) = 0;

  /**
   * Gets the contents of the specified directory.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the directory.
   * @return An iterator over the contents of the directory.
   */
  virtual DirIterator getDirContents(
    const SecurityIdentity &requester,
    const std::string &path) const = 0;

  /**
   * Returns the directory entry information for the specified directory or file
   * within the archive namespace.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the directory or file within the archive
   * namespace.
   * @return The directory entry information for the specified directory or file
   * within the archive namespace.
   */
  virtual DirEntry statDirEntry(
    const SecurityIdentity &requester,
    const std::string &path) const = 0;

  /**
   * Sets the storage class of the specified directory to the specified value.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the directory.
   * @param storageClassName The name of the storage class.
   */
  virtual void setDirStorageClass(
    const SecurityIdentity &requester,
    const std::string &path,
    const std::string &storageClassName) = 0;

  /**
   * Clears the storage class of the specified directory.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the directory.
   */
  virtual void clearDirStorageClass(
    const SecurityIdentity &requester,
    const std::string &path) = 0;

  /**
   * Returns the name of the storage class of the specified directory.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the directory.
   * @return The name of the storage class of the specified directory.
   */
  virtual std::string getDirStorageClass(
    const SecurityIdentity &requester,
    const std::string &path) const = 0;

  /**
   * Creates the specified file entry.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the directory.
   * @param mode The mode bits of the file entry.
   */
  virtual void createFile(
    const SecurityIdentity &requester,
    const std::string &path,
    const uint16_t mode) = 0;

  /**
   * Deletes the specified file entry.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the file.
   */
  virtual void deleteFile(
    const SecurityIdentity &requester,
    const std::string &path) = 0;

  /**
   * Returns true if the specified directory exists.
   *
   * @param requester The identity of the requester.
   * @param path The absolute path of the file.

   * @return True if the specified directory exists.
   */
  virtual bool dirExists(
    const SecurityIdentity &requester,
    const std::string &path) const = 0;

}; // class NameServer

} // namespace 

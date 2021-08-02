/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2015-2021 CERN
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

#pragma once
 
#include <string>
#include "EntryLog.hpp"

namespace cta { 
namespace common {
namespace dataStructures {
  
struct VirtualOrganization {
  /**
   * The name
   */
  std::string name;
  /**
   * The comment.
   */
  std::string comment;
  /**
   * Maximum number of drives allocated for reading per VO
   */
  uint64_t readMaxDrives;
  /**
   * Max number of drives allocated for writing per VO
   */
  uint64_t writeMaxDrives;
  /**
   * The creation log.
   */
  EntryLog creationLog;

  /**
   * The last modification log.
   */
  EntryLog lastModificationLog;
  
  bool operator==(const VirtualOrganization & other) const{
    return (name == other.name && comment == other.comment && readMaxDrives == other.readMaxDrives && writeMaxDrives == other.writeMaxDrives);
  }
};

}}}



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

#include <list>
#include <map>
#include <stdint.h>
#include <string>

#include "common/dataStructures/EntryLog.hpp"

namespace cta {
namespace common {
namespace dataStructures {

/**
 * The logical library is an attribute of both drives and tapes, it declares 
 * which tapes can be mounted in which drives 
 */
struct LogicalLibrary {

  /**
   * Constructor.
   *
   * Initialises isDisabled to false.
   */
  LogicalLibrary();

  bool operator==(const LogicalLibrary &rhs) const;

  bool operator!=(const LogicalLibrary &rhs) const;

  std::string name;
  bool isDisabled;
  EntryLog creationLog;
  EntryLog lastModificationLog;
  std::string comment;

}; // struct LogicalLibrary

std::ostream &operator<<(std::ostream &os, const LogicalLibrary &obj);

} // namespace dataStructures
} // namespace common
} // namespace cta

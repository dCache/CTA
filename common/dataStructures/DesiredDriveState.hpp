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

#include <ostream>
#include "common/optional.hpp"

namespace cta {
namespace common {
namespace dataStructures {

/**
 * Structure describing the instructions to the drive from operators.
 * The values are reset to all false when the drive goes down (including
 * at startup).
 */
class DesiredDriveState {
public:
  DesiredDriveState();
  DesiredDriveState(const DesiredDriveState & ds);
  DesiredDriveState & operator=(const DesiredDriveState & ds);
  bool up;        ///< Should the drive be up?
  bool forceDown; ///< Should going down preempt an existig mount?
  cta::optional<std::string> reason; ///< The reason why operators put the drive down or up
  cta::optional<std::string> comment; ///< General informations about the drive given by the operators
  bool operator==(const DesiredDriveState &rhs) const {
    return up == rhs.up && forceDown == rhs.forceDown;
  }
  static std::string c_tpsrvPrefixComment;
  void setReasonFromLogMsg(const int logLevel, const std::string & msg);
  static std::string generateReasonFromLogMsg(const int logLevel, const std::string & msg);
};

std::ostream &operator<<(std::ostream& os, const DesiredDriveState& obj);

}}} // namespace cta::common::dataStructures
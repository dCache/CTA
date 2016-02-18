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

#include "common/dataStructures/DriveStatus.hpp"

std::string cta::common::dataStructures::toString(cta::common::dataStructures::DriveStatus type) {
  switch(type) {
    case cta::common::dataStructures::DriveStatus::Down:
      return "Down";
    case cta::common::dataStructures::DriveStatus::Up:
      return "Up";
    case cta::common::dataStructures::DriveStatus::Starting:
      return "Starting";
    case cta::common::dataStructures::DriveStatus::Mounting:
      return "Mounting";
    case cta::common::dataStructures::DriveStatus::Transfering:
      return "Transfering";
    case cta::common::dataStructures::DriveStatus::Unloading:
      return "Unloading";
    case cta::common::dataStructures::DriveStatus::Unmounting:
      return "Unmounting";
    case cta::common::dataStructures::DriveStatus::DrainingToDisk:
      return "DrainingToDisk";
    case cta::common::dataStructures::DriveStatus::CleaningUp:
      return "CleaningUp";
    default:
      return "UNKNOWN";
  }
}

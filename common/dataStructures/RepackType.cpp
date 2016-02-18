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

#include "common/dataStructures/RepackType.hpp"

std::string cta::common::dataStructures::toString(cta::common::dataStructures::RepackType type) {
  switch(type) {
    case cta::common::dataStructures::RepackType::expandandrepack:
      return "expandandrepack";
    case cta::common::dataStructures::RepackType::justexpand:
      return "justexpand";
    case cta::common::dataStructures::RepackType::justrepack:
      return "justrepack";
    default:
      return "UNKNOWN";
  }
}

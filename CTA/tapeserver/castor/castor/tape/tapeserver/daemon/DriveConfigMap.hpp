/******************************************************************************
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include "castor/tape/tapeserver/daemon/DriveConfig.hpp"
#include "castor/tape/tapeserver/daemon/TpconfigLine.hpp"
#include "castor/tape/tapeserver/daemon/TpconfigLines.hpp"
#include "castor/mediachanger/LibrarySlotParser.hpp"

#include <map>
#include <string>

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {

/**
 * Datatype representing a map from the unit name of a tape drive to its
 * configuration as specified in /etc/castor/TPCONFIG.
 */
class DriveConfigMap: public std::map<std::string, DriveConfig> {
public:

  /**
   * Enters the specified list of parsed lines from the TPCONFIG file into
   * the map of drive configurations.
   *
   * @param lines The lines of the TPCONFIG file.
   */
  void enterTpconfigLines(const TpconfigLines &lines);

private:

  /**
   * Enters the specified parsed line from the TPCONFIG file into the map
   * of drive configurations.
   *
   * @param line A line from the TPCONFIG file.
   */
  void enterTpconfigLine(const TpconfigLine &line);

}; // class DriveConfigMap

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

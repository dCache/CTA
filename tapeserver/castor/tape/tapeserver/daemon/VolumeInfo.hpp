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
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include "scheduler/MountType.hpp"

#include <string>

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {

  /**
   * Class holding the result of a Volume request
   */
    class VolumeInfo {
    public:
      VolumeInfo() {};
      /** The VID we will work on */
      std::string vid;
      /** The density of the volume */
      std::string density;
      /** The mount type: archive or retrieve */
      cta::MountType::Enum mountType;
    };

} // namespace daemon
} // namespace tapesever
} // namespace tape
} // namespace castor

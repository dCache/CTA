// ----------------------------------------------------------------------
// File: Drive/DriveList.hh
// Author: Eric Cano - CERN
// ----------------------------------------------------------------------

/************************************************************************
 * Tape Server                                                          *
 * Copyright (C) 2013 CERN/Switzerland                                  *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 ************************************************************************/

#pragma once
#include <vector>


namespace Tape{
/**
 * Contains data about a tape drive
 */
  struct DriveInfo: public SCSI::Device {
    
  };
  
/**
 * Detects the present tape drives on the system and gathers the basic
 * information about them.
 */
  class DriveList {
  public:
    DriveList();
    virtual ~DriveList();
  private:
    listScsi();
    std::vector<DriveInfo> m_
  };
  
}; // namespace Tape
// ----------------------------------------------------------------------
// File: SCSI/TapeDriveInfo.cc
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

/**
 * Test main program. For development use.
 */

#include "../System/Wrapper.hh"
#include "../SCSI/Device.hh"
#include "Drive.hh"
#include <iostream>

int main ()
{
  Tape::System::realWrapper sWrapper;
  SCSI::DeviceVector<Tape::System::realWrapper> dl(sWrapper);
  for(SCSI::DeviceVector<Tape::System::realWrapper>::iterator i = dl.begin();
          i != dl.end(); i++) {
    SCSI::DeviceInfo & dev = (*i);
    std::cout << dev.sg_dev << std::endl;
    if (dev.type == SCSI::Types::tape) {
      Tape::Drive<Tape::System::realWrapper> drive(dev,sWrapper);
      drive.SCSI_inquiry();
    }
  }
}

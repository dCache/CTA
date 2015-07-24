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

#include "castor/log/LogContext.hpp"
#include "castor/log/Logger.hpp"
#include "castor/mediachanger/MediaChangerFacade.hpp"
#include "castor/tape/tapeserver/daemon/DriveConfig.hpp"
#include "castor/tape/tapeserver/drive/DriveInterface.hpp"
#include "castor/tape/tapeserver/file/Structures.hpp"
#include "castor/tape/tapeserver/SCSI/Device.hpp"
#include "castor/tape/tapeserver/system/Wrapper.hpp"

#include <memory>

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
  /**
   * Class responsible for probing a tape drive to see if it empty and
   * accessible.
   */
  class EmptyDriveProbe {
  public:

    /**
     * Constructor
     * 
     * @param log Object representing the API to the CASTOR logging system.
     * @param driveConfig Configuration of the tape drive to be probed.
     * @param sysWrapper Object representing the operating system.
     */
    EmptyDriveProbe(
      castor::log::Logger &log,
      const DriveConfig &driveConfig,
      System::virtualWrapper &sysWrapper);
    
    /** 
     * Probes the tape drive to determine whether it is empty and accessible.
     *
     * @return True if the drive is empty and accessible.
     */
    bool driveIsEmpty() throw();
    
  private:

    /**
     * The logging object     
     */
    castor::log::Logger &m_log;
    
    /**
     * The configuration of the tape drive to be probed.
     */
    const DriveConfig m_driveConfig;
    
    /**
     * The system wrapper used to find the device and instantiate the drive object
     */
    System::virtualWrapper &m_sysWrapper;

    /** 
     * Probes the tape drive to determine whether it is empty and accessible.
     *
     * @return True if the drive is empty and accessible.
     */
    bool exceptionThrowingDriveIsEmpty();

    /**
     * Creates and returns the object that represents the tape drive to be
     * probed.
     *
     * @return The tape drive.
     */
    std::unique_ptr<drive::DriveInterface> createDrive();
    
  }; // class EmptyDriveProbe

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

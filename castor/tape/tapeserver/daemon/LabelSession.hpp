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

#include "castor/legacymsg/TapeLabelRqstMsgBody.hpp"
#include "castor/log/Logger.hpp"
#include "castor/legacymsg/RmcProxy.hpp"
#include "castor/log/LogContext.hpp"
#include "castor/tape/tapeserver/system/Wrapper.hpp"
#include "castor/tape/utils/DriveConfig.hpp"
#include "castor/tape/utils/utils.hpp"
#include "castor/tape/tapeserver/client/ClientProxy.hpp"
#include "castor/legacymsg/NsProxy.hpp"
#include "castor/tape/tapeserver/SCSI/Device.hpp"
#include "castor/tape/tapeserver/drive/DriveInterface.hpp"

#include <memory>

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
  /**
   * Class responsible for handling a tape label session.
   */
  class LabelSession {
  public:
    
    /**
     *  Constructor 
     *
     * @param rmc Proxy object representing the rmcd daemon.
     * @param clientRequest The request to label a tape received from the label
     * tape command.
     * @param log Object representing the API to the CASTOR logging system.
     * @param sysWrapper Object representing the operating system.
     * @param driveConfig the configuration of the tape-drive to be used to
     * label a tape.
     * @param force The flag that, if set to true, allows labeling a non-blank
     * tape.
     */
    LabelSession(
      legacymsg::RmcProxy &rmc,
      legacymsg::NsProxy &ns,
      const legacymsg::TapeLabelRqstMsgBody &clientRequest, 
      castor::log::Logger &log,
      System::virtualWrapper &sysWrapper,
      const utils::DriveConfig &driveConfig,
      const bool force);
    
    /**
     *  The only method. It will call executeLabel to do the actual job 
     */
    void execute() ;
    
  private:
    
    /**
     * The network timeout in seconds
     */
    int m_timeout;

    enum TapeNsStatus {
      LABEL_SESSION_STEP_SUCCEEDED,
      LABEL_SESSION_STEP_FAILED
    }; 

    /**
     * Performs some meta-data checks that need to be done before deciding to
     * mount the tape for labeling.
     */ 
    void performPreMountChecks();

    /**
     * A meta-data check that sees if the user of the client is either the
     * owner of the tape pool containing the tape to be labelled or is an ADMIN
     * user within the CUPV privileges database.
     */
    void checkClientIsOwnerOrAdmin();

    /**
     * A meta-data check that sees if the tape to be labelled contains files
     * that are registered in the CASTOR name-server as either active or
     * disabled segments.
     */
    void checkIfVidStillHasSegments();
    
    /**
     * Returns a Drive object representing the tape drive to be used to label
     * a tape.
     *
     * @return The drive object.
     */
    std::auto_ptr<castor::tape::tapeserver::drives::DriveInterface> getDriveObject();

    /**
     * Mounts the tap eto be labelled.
     */
    void mountTape();
    
    /**
     * Waits for the tape to be loaded into the tape drive.
     *
     * @param drive Object representing the drive hardware.
     * @param timeoutSecond The number of seconds to wait for the tape to be
     * loaded into the tape drive. 
     */
    void waitUntilTapeLoaded(drives::DriveInterface *const drive,
      const int timeoutSecond);

    /**
     * Checks that the now loaded tape is writable.
     *
     * @param drive Object representing the drive hardware.
     */
    void checkTapeIsWritable(drives::DriveInterface *const drive);
    
    /**
     * The function carrying out the actual labeling
     * @param drive The drive object pointer
     * @return 
     */
    void labelTheTape(drives::DriveInterface *const drive);
    
    /**
     * The object representing the rmcd daemon.
     */
    legacymsg::RmcProxy &m_rmc;

    /**
     * The object representing the rmcd daemon.
     */
    legacymsg::NsProxy &m_ns;
    
    /**
     * The label request message body
     */
    legacymsg::TapeLabelRqstMsgBody m_request;
    
    /**
     * The logging object     
     */
    castor::log::Logger & m_log;
    
    /**
     * The system wrapper used to find the device and instantiate the drive object
     */
    System::virtualWrapper & m_sysWrapper;
    
    /**
     * The configuration of the tape drive to be used to label a tape.
     */
    const utils::DriveConfig m_driveConfig;
    
    /**
     * The flag that, if set to true, allows labeling a non-blank tape
     */
    const bool m_force;

 }; // class LabelSession

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

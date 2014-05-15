/******************************************************************************
 *                      LabelSession.hpp
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
 * @author dkruse@cern.ch
 *****************************************************************************/

#pragma once

#include "castor/legacymsg/TapeLabelRqstMsgBody.hpp"
#include "castor/log/Logger.hpp"
#include "castor/legacymsg/RmcProxy.hpp"
#include "castor/log/LogContext.hpp"
#include "castor/tape/tapeserver/system/Wrapper.hpp"
#include "castor/tape/utils/utils.hpp"
#include "castor/tape/tapeserver/client/ClientProxy.hpp"
#include "castor/legacymsg/NsProxy.hpp"
#include "castor/tape/tapeserver/SCSI/Device.hpp"
#include "castor/tape/tapeserver/drive/Drive.hpp"

#include <memory>

using namespace castor::tape;
using namespace castor::log;

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
     * @param labelCmdConnection The file descriptor of the connection with the
     * label command.
     * @param rmc Proxy object representing the rmcd daemon.
     * @param clientRequest The request to label a tape received from the label
     * tape command.
     * @param logger Object reprsenting the API to the CASTOR logging system.
     * @param sysWrapper Object representing the operating system.
     * @param tpConfig The parsed lines of /etc/castor/TPCONFIG.
     * @param force The flag that, if set to true, allows labeling a non-blank
     * tape.
     */
    LabelSession(
      const int labelCmdConnection,
      legacymsg::RmcProxy &rmc,
      legacymsg::NsProxy &ns,
      const legacymsg::TapeLabelRqstMsgBody &clientRequest, 
      castor::log::Logger &logger,
      System::virtualWrapper &sysWrapper,
      const utils::TpconfigLines &tpConfig,
      const bool force);
    
    /**
     *  The only method. It will call executeLabel to do the actual job 
     */
    void execute() ;
    
  private:

    /**
     * The file descriptor of the connection with the label command.
     */
    int m_labelCmdConnection;
    
    enum TapeNsStatus {
      LABEL_SESSION_STEP_SUCCEEDED,
      LABEL_SESSION_STEP_FAILED
    };
    
    /**
     * 
     * @param configLine The configuration line containing the drive-capability specification
     * @return 
     */
    int checkIfVidStillHasSegments(utils::TpconfigLines::const_iterator &configLine);
    
    /**
     * 
     * @param configLine The configuration line containing the drive-capability specification
     * @return 
     */
    int identifyDrive(utils::TpconfigLines::const_iterator &configLine);
    
    /**
     * 
     * @param configLine The configuration line containing the drive-capability specification
     * @return 
     */
    int mountTape(utils::TpconfigLines::const_iterator &configLine);
    
    /**
     * 
     * @param configLine The configuration line containing the drive-capability specification
     * @param driveInfo Structure containing information of the device file of the drive
     * @return 
     */
    int getDeviceInfo(utils::TpconfigLines::const_iterator &configLine, castor::tape::SCSI::DeviceInfo &driveInfo);
    
    /**
     * 
     * @param configLine The configuration line containing the drive-capability specification
     * @param driveInfo Structure containing information of the device file of the drive
     * @param drive Drive object
     * @return 
     */
    int getDriveObject(utils::TpconfigLines::const_iterator &configLine, castor::tape::SCSI::DeviceInfo &driveInfo, std::auto_ptr<castor::tape::drives::DriveInterface> &drive);
    
    /**
     * 
     * @param drive
     * @return 
     */
    int checkDriveObject(castor::tape::drives::DriveInterface *drive);
    
    /**
     * 
     * @param drive
     * @return 
     */
    int labelTheTape(castor::tape::drives::DriveInterface *drive);
    
    /**
     * 
     * @param configLine The configuration line containing the drive-capability specification
     * @return 
     */
    int unmountTape(utils::TpconfigLines::const_iterator &configLine);
    
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
    castor::log::Logger & m_logger;
    
    /**
     * The system wrapper used to find the device and instantiate the drive object
     */
    System::virtualWrapper & m_sysWrapper;
    
    /**
     * The object containing the information found in the TPCONFIG file
     */
    const utils::TpconfigLines & m_tpConfig;
    
    /**
     * The flag that, if set to true, allows labeling a non-blank tape
     */
    const bool m_force;
    
    /**
     * This is the function performing the actual labeling work (it calls the castor::tape::drive:LabelSession to do so)
     * 
     * @param lc the log context
     */
    void executeLabel();

 }; // class LabelSession

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

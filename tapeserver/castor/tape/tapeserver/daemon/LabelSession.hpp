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
#include "castor/log/LogContext.hpp"
#include "castor/log/Logger.hpp"
#include "castor/mediachanger/MediaChangerFacade.hpp"
#include "castor/messages/TapeserverProxy.hpp"
#include "castor/server/ProcessCap.hpp"
#include "castor/tape/tapeserver/client/ClientProxy.hpp"
#include "castor/tape/tapeserver/daemon/DriveConfig.hpp"
#include "castor/tape/tapeserver/daemon/Session.hpp"
#include "castor/tape/tapeserver/drive/DriveInterface.hpp"
#include "castor/tape/tapeserver/SCSI/Device.hpp"
#include "castor/tape/tapeserver/system/Wrapper.hpp"

#include <memory>

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {

/**
 * Class responsible for handling a tape label session.
 */
class LabelSession: public Session {
public:
  
  /**
   *  Constructor 
   *
   * @param capUtils Object providing support for UNIX capabilities.
   * @param tapeserver Proxy object representing the tapeserverd daemon.
   * @param mc Object representing the media changer.
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
    server::ProcessCap &capUtils,
    messages::TapeserverProxy &tapeserver,
    mediachanger::MediaChangerFacade &mc,
    const legacymsg::TapeLabelRqstMsgBody &clientRequest, 
    castor::log::Logger &log,
    System::virtualWrapper &sysWrapper,
    const DriveConfig &driveConfig,
    const bool force);
  
  /** 
   * Execute the session and return the type of action to be performed
   * immediately after the session has completed.
   *
   * @return Returns the type of action to be performed after the session has
   * completed.
   */
  EndOfSessionAction execute() throw();
    
private:

  /**
   * Object providing support for UNIX capabilities.
   */
  server::ProcessCap &m_capUtils;

  /**
   * Proxy object representing the tapeserverd daemon.
   */
  messages::TapeserverProxy &m_tapeserver;
    
  /**
   * The object representing the media changer.
   */
  mediachanger::MediaChangerFacade &m_mc;

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
  System::virtualWrapper &m_sysWrapper;
  
  /**
   * The configuration of the tape drive to be used to label a tape.
   */
  const DriveConfig m_driveConfig;
  
  /**
   * The flag that, if set to true, allows labeling a non-blank tape
   */
  const bool m_force;

  /** 
   * Execute the session and return the type of action to be performed
   * immediately after the session has completed.
   *
   * @return Returns the type of action to be performed after the session has
   * completed.
   */
  EndOfSessionAction exceptionThrowingExecute();

  /**
   * Sets the capabilities of the process and logs the result.
   *
   * @param capabilities The string representation of the capabilities.
   */
  void setProcessCapabilities(const std::string &capabilities);
    
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
   * Returns a Drive object representing the tape drive to be used to label
   * a tape.
   *
   * @return The drive object.
   */
  std::unique_ptr<drive::DriveInterface> createDrive();

  /**
   * Mounts the tape to be labelled.
   */
  void mountTape();
  
  /**
   * Waits for the tape to be loaded into the tape drive.
   *
   * @param drive Object representing the drive hardware.
   * @param timeoutSecond The number of seconds to wait for the tape to be
   * loaded into the tape drive. 
   */
  void waitUntilTapeLoaded(drive::DriveInterface &drive,
    const int timeoutSecond);

  /**
   * Rewinds the specified tape drive.
   *
   * @param drive The tape drive.
   */
  void rewindDrive(drive::DriveInterface &drive);

  /**
   * Notifies the tape-server parent-process of the specified user error.
   *
   * @param message The error message.
   */
  void notifyTapeserverOfUserError(const std::string message);
  
  /**
   * Writes the label file to the tape.
   *
   * This method assumes the tape has been rewound.
   *
   * @param drive The tape drive.
   */
  void writeLabelToTape(drive::DriveInterface &drive);

  /**
   * Unloads the specified tape from the specified tape drive.
   *
   * @param vid The volume identifier of the tape to be unloaded.  Please note
   * that the value of this field is only used for logging purposes.
   * @param drive The tape drive.
   */
  void unloadTape(const std::string &vid, drive::DriveInterface &drive);

  /**
   * Dismounts the specified tape.
   *
   * @param vid The volume identifier of the tape to be dismounted.
   */
  void dismountTape(const std::string &vid);

  /**
   * Returns the string representation of the specified boolean value.
   *
   * @param value The boolean value.
   * @return The string representation.
   */
  const char *boolToStr(const bool value);

}; // class LabelSession

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

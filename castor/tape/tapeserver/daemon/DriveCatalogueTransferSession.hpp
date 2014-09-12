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

#include "castor/legacymsg/RtcpJobRqstMsgBody.hpp"
#include "castor/tape/tapeserver/daemon/DataTransferSession.hpp"
#include "castor/tape/tapeserver/daemon/DriveCatalogueSession.hpp"
#include "castor/tape/tapeserver/daemon/ProcessForkerProxy.hpp"
#include "castor/legacymsg/RtcpJobRqstMsgBody.hpp"

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {

/**
 * Derived class containing info about the label session attached to a drive
 * catalogue entry.
 */
class DriveCatalogueTransferSession : public DriveCatalogueSession {
public:
  /**
   * Creates a DriveCatalogueTransferSession object.
   *
   * Except in the case of unit testing, a DriveCatalogueLabelSession object
   * should only be created using the static create() method.
   *
   * @param log Object representing the API of the CASTOR logging system.
   * @param driveConfig The configuration of the tape drive.
   * @param dataTransferConfig The configuration of a data-transfer session.
   * @param vdqmJob job received from the vdqmd daemon.
   * @param rmcPort The TCP/IP port on which the rmcd daemon is listening.
   * @param processForker Proxy object representing the ProcessForker.
   * @param vdqm Proxy object representing the vdqmd daemon.
   * @param hostName The name of the host on which the daemon is running.  This
   * name is needed to fill in messages to be sent to the vdqmd daemon.
   * @return A newly created DriveCatalogueTransferSession object.
   */
  static DriveCatalogueTransferSession *create(
    log::Logger &log,
    const tape::utils::DriveConfig &driveConfig,
    const DataTransferSession::CastorConf &dataTransferConfig,
    const legacymsg::RtcpJobRqstMsgBody &vdqmJob,
    const unsigned short rmcPort,
    ProcessForkerProxy &processForker,
    legacymsg::VdqmProxy &vdqm,
    const std::string &hostName);

  /**
   * To be called when the session has ended with success.
   */
  void sessionSucceeded();

  /**
   * To be called when the session has ended with failure.
   */
  void sessionFailed();

  /**
   * Assigns the tape drive in the vdqm.
   */
  void assignDriveInVdqm();

  /**
   * Gets the time at which the tape drive was assigned a data transfer job.
   */
  time_t getAssignmentTime() const throw();

  /**
   * vdqmJob getter method
   * 
   * @return vdqm job received from the vdqmd daemon
   */
  castor::legacymsg::RtcpJobRqstMsgBody getVdqmJob() const;

  /**
   * Notifies the tape-drive catalogue entry that a recall job has been received
   * for the tape drive.
   *
   * @param vid The volume identifier of the tape to be mounted for recall.
   */
  void receivedRecallJob(const std::string &vid);

  /**
   * Notifies the tape-drive catalogue entry that a migration job has been
   * received for the tape drive.
   *
   * @param vid The volume identifier of the tape to be mounted for migration.
   */
  void receivedMigrationJob(const std::string &vid);
  
  /**
   * Gets the volume identifier of the tape associated with the tape drive.
   *
   * @return The volume identifier of the tape associated with the tape drive.
   * If the volume indentifier is not known then this method returns the empty
   * string.
   */
  std::string getVid() const;

  /**
   * Gets the access mode, either recall (WRITE_DISABLE) or migration
   * (WRITE_ENABLE).
   *
   * This method throws a castor::exception::Exception if the access mode is
   * not yet nknown.
   *
   * @return The access mode.
   */
  int getMode() const;

  /**
   * Gets the process identifier of the session.
   * 
   * @return The process identifier of the session.
   */
  pid_t getPid() const throw();

  /**
   * Notifies the tape-drive catalogue entry that the specified tape has been
   * mounted for migration in the tape drive.
   *
   * @param vid The volume identifier of the tape.
   */
  void tapeMountedForMigration(const std::string &vid);
    
  /**
   * Notifies the tape-drive catalogue entry that the specified tape has been
   * mounted for recall in the tape drive.
   *
   * @param vid The volume identifier of the tape.
   */
  void tapeMountedForRecall(const std::string &vid);

  /**
   * Returns true if a tape is in the process of being mounted.
   *
   * @return True if a tape is in the process of being mounted.
   */
  bool tapeIsBeingMounted() const throw();

protected:

  /**
   * Protected constructor.
   *
   * Except in the case of unit testing a DriveCatalogueTransferSession object
   * should only be created using the static create() method.  This constructor
   * is protected so that unit tests can go around this restriction for sole
   * purpose of unit testing.
   * 
   * @param pid The process identifier of the session.
   * @param log Object representing the API of the CASTOR logging system.
   * @param driveConfig The configuration of the tape drive.
   * @param dataTransferConfig The configuration of a data-transfer session.
   * @param vdqmJob job received from the vdqmd daemon.
   * @param vdqm Proxy object representing the vdqmd daemon.
   * @param hostName The name of the host on which the daemon is running.  This
   * name is needed to fill in messages to be sent to the vdqmd daemon.
   */
  DriveCatalogueTransferSession(
    const pid_t pid,
    log::Logger &log,
    const tape::utils::DriveConfig &driveConfig,
    const DataTransferSession::CastorConf &dataTransferConfig,
    const legacymsg::RtcpJobRqstMsgBody &vdqmJob,
    legacymsg::VdqmProxy &vdqm,
    const std::string &hostName) throw();

private:

  /**
   * The process identifier of the session.
   */
  const pid_t m_pid;

  /**
   * Enumeration of the states of a catalogue transfer-session.
   */
  enum TransferState {
    TRANSFERSTATE_WAIT_ASSIGN,
    TRANSFERSTATE_WAIT_JOB,
    TRANSFERSTATE_WAIT_MOUNTED,
    TRANSFERSTATE_RUNNING};

  /**
   * Returns the string repsentation of the specified transfer state.
   *
   * If the specified transfer state is unknown then a string explaining this
   * is returned.
   */
  const char *transferStateToStr(const TransferState state) const throw();

  /**
   * The current state of the transfer session.
   */
  TransferState m_state;

  /**
   * The volume identifier of the tape assocaited with the tape drive.
   */
  std::string m_vid;

  /**
   * Are we mounting for recall (WRITE_DISABLE) or migration (WRITE_ENABLE).
   */
  uint16_t m_mode;

  /**
   * The time at which the tape drive was assigned a data transfer job.
   */
  const time_t m_assignmentTime;

  /**
   * Object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

  /**
   * The configuration of the tape drive.
   */
  const tape::utils::DriveConfig m_driveConfig;

  /**
   * The configuration of a data-transfer session.
   */
  const DataTransferSession::CastorConf m_dataTransferConfig;
  
  /**
   * The job received from the vdqmd daemon.
   */
  const legacymsg::RtcpJobRqstMsgBody m_vdqmJob;

  /**
   * Proxy object representing the vdqmd daemon.
   */
  legacymsg::VdqmProxy &m_vdqm;

  /**
   * The name of the host on which the daemon is running.  This name is
   * needed to fill in messages to be sent to the vdqmd daemon.
   */   
  const std::string m_hostName;

  /**
   * Uses the ProcessForker to fork a data-transfer session.
   *
   * @param processForker Proxy object representing the ProcessForker.
   * @param driveConfig The configuration of the tape drive.
   * @param vdqmJob job received from the vdqmd daemon.
   * @param dataTransferConfig The configuration of a data-transfer session.
   * @param rmcPort The TCP/IP port on which the rmcd daemon is listening.
   * @return The process identifier of the session.
   */
  static pid_t forkTransferSession(ProcessForkerProxy &processForker, 
    const tape::utils::DriveConfig &driveConfig,
    const legacymsg::RtcpJobRqstMsgBody &vdqmJob,
    const DataTransferSession::CastorConf &dataTransferConfig,
    const unsigned short rmcPort);

}; // class DriveCatalogueTransferSession

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

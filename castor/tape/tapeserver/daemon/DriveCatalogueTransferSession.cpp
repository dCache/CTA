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

#include "castor/common/CastorConfiguration.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/legacymsg/VdqmProxy.hpp"
#include "castor/tape/tapeserver/daemon/DriveCatalogueTransferSession.hpp"
#include "h/Ctape_constants.h"
#include "h/rmc_constants.h"

//------------------------------------------------------------------------------
// create
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::DriveCatalogueTransferSession*
  castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::create(
  log::Logger &log,
  const tape::utils::DriveConfig &driveConfig,
  const DataTransferSession::CastorConf &dataTransferConfig,
  const legacymsg::RtcpJobRqstMsgBody &vdqmJob,
  const unsigned short rmcPort,
  ProcessForkerProxy &processForker,
  legacymsg::VdqmProxy &vdqm,
  const std::string &hostName) {

  const pid_t pid = forkTransferSession(processForker, driveConfig, vdqmJob,
    dataTransferConfig, rmcPort);

  return new DriveCatalogueTransferSession(
    pid,
    log,
    driveConfig,
    dataTransferConfig,
    vdqmJob,
    vdqm,
    hostName);
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  DriveCatalogueTransferSession(
  const pid_t pid,
  log::Logger &log,
  const tape::utils::DriveConfig &driveConfig,
  const DataTransferSession::CastorConf &dataTransferConfig,
  const legacymsg::RtcpJobRqstMsgBody &vdqmJob,
  legacymsg::VdqmProxy &vdqm,
  const std::string &hostName) throw():
  m_pid(pid),
  m_state(TRANSFERSTATE_WAIT_ASSIGN),
  m_mode(WRITE_DISABLE),
  m_assignmentTime(time(0)),
  m_log(log),
  m_driveConfig(driveConfig),
  m_dataTransferConfig(dataTransferConfig),
  m_vdqmJob(vdqmJob),
  m_vdqm(vdqm),
  m_hostName(hostName) {
}

//------------------------------------------------------------------------------
// forkTransferSession
//------------------------------------------------------------------------------
pid_t castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  forkTransferSession(ProcessForkerProxy &processForker,
  const tape::utils::DriveConfig &driveConfig,
  const legacymsg::RtcpJobRqstMsgBody &vdqmJob,
  const DataTransferSession::CastorConf &dataTransferConfig,
  const unsigned short rmcPort) {
  try {
    return processForker.forkDataTransfer(driveConfig, vdqmJob,
      dataTransferConfig, rmcPort);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to fork data-transfer session: unitName=" <<
      driveConfig.unitName << ": " << ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// sessionSucceeded
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  sessionSucceeded() {
}

//------------------------------------------------------------------------------
// sessionFailed
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  sessionFailed() {
}

//------------------------------------------------------------------------------
// assignDriveInVdqm
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  assignDriveInVdqm() {
  try {
    if(TRANSFERSTATE_WAIT_ASSIGN != m_state) {
      castor::exception::Exception ex;
      ex.getMessage() << "Catalogue transfer-session state-mismatch: "
        "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_ASSIGN) <<
        " actual=" << transferStateToStr(m_state);
      throw ex;
    }

    m_vdqm.assignDrive(m_hostName, m_driveConfig.unitName,
      m_vdqmJob.dgn, m_vdqmJob.volReqId, m_pid);
    m_state = TRANSFERSTATE_WAIT_JOB;
    log::Param params[] = {
      log::Param("server", m_hostName),
      log::Param("unitName", m_driveConfig.unitName),
      log::Param("dgn", std::string(m_vdqmJob.dgn)),
      log::Param("volReqId", m_vdqmJob.volReqId),
      log::Param("dataTransferPid", m_pid)};
    m_log(LOG_INFO, "Assigned the drive in the vdqm", params);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to assign drive in vdqm: " <<
      ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getAssignmentTime
//------------------------------------------------------------------------------
time_t castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  getAssignmentTime() const throw() {
  return m_assignmentTime;
}

//------------------------------------------------------------------------------
// getVdqmJob
//------------------------------------------------------------------------------
castor::legacymsg::RtcpJobRqstMsgBody castor::tape::tapeserver::daemon::
  DriveCatalogueTransferSession::getVdqmJob() const{
  return m_vdqmJob;
}

//-----------------------------------------------------------------------------
// receivedRecallJob
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  receivedRecallJob(const std::string &vid) {
  if(TRANSFERSTATE_WAIT_JOB != m_state) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept reception of recall job"
      ": Catalogue transfer-session state-mismatch: "
      "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_JOB) <<
      " actual=" << transferStateToStr(m_state);
    throw ex;
  }
  m_state = TRANSFERSTATE_WAIT_MOUNTED;

  m_mode = WRITE_DISABLE;
  m_vid = vid;
}

//-----------------------------------------------------------------------------
// receivedMigrationJob
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  receivedMigrationJob(const std::string &vid) {
  if(TRANSFERSTATE_WAIT_JOB != m_state) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept reception of recall job"
      ": Catalogue transfer-session state-mismatch: "
      "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_JOB) <<
      " actual=" << transferStateToStr(m_state);
    throw ex;
  }
  m_state = TRANSFERSTATE_WAIT_MOUNTED;

  m_mode = WRITE_ENABLE;
  m_vid = vid;
}

//------------------------------------------------------------------------------
// getVid
//------------------------------------------------------------------------------
std::string castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  getVid() const {
  switch(m_state) {
  case TRANSFERSTATE_WAIT_MOUNTED:
  case TRANSFERSTATE_RUNNING:
    return m_vid;
  default:
    return "";
  }
}

//------------------------------------------------------------------------------
// getMode
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  getMode() const {
  switch(m_state) {
  case TRANSFERSTATE_WAIT_MOUNTED:
  case TRANSFERSTATE_RUNNING:
    return m_mode;
  default:
    {
      castor::exception::Exception ex;
      ex.getMessage() << "Failed to get access mode from catalogue"
        " transfer-session"
        ": Catalogue transfer-session is in an incompatible state: "
        " state=" << transferStateToStr(m_state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// getPid
//-----------------------------------------------------------------------------
pid_t castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  getPid() const throw() {
  return m_pid;
}

//-----------------------------------------------------------------------------
// tapeMountedForMigration
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  tapeMountedForMigration(const std::string &vid) {
  if(TRANSFERSTATE_WAIT_MOUNTED != m_state) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for migration"
      ": Catalogue transfer-session state-mismatch: "
      "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_MOUNTED) <<
      " actual=" << transferStateToStr(m_state);
    throw ex;
  }
  m_state = TRANSFERSTATE_RUNNING;

  // If the volume identifier of the data transfer job does not match the
  // mounted tape
  if(m_vid != vid) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for migration"
      ": VID mismatch: expected=" << m_vid << " actual=" << vid;
    throw ex;
  }

  //migratiomigration the data transfer bob is not for migration
  if(WRITE_ENABLE != m_mode) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for migration"
      ": Data transfer job is not for migration";
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// tapeMountedForRecall
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  tapeMountedForRecall(const std::string &vid) {
  if(TRANSFERSTATE_WAIT_MOUNTED != m_state) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for recall"
      ": Catalogue transfer-session state-mismatch: "
      "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_MOUNTED) <<
      " actual=" << transferStateToStr(m_state);
    throw ex;
  }
  m_state = TRANSFERSTATE_RUNNING;

  // If the volume identifier of the data transfer job does not match the
  // mounted tape
  if(m_vid != vid) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for recall"
      ": VID mismatch: expected=" << m_vid << " actual=" << vid;
    throw ex;
  }

  // If the data transfer bob is not for recall
  if(WRITE_DISABLE != m_mode) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for recall"
      ": Data transfer job is not for recall";
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// transferStateToStr
//-----------------------------------------------------------------------------
const char *castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  transferStateToStr(const TransferState state) const throw() {
  switch(state) {
  case TRANSFERSTATE_WAIT_ASSIGN : return "WAIT_ASSIGN";
  case TRANSFERSTATE_WAIT_JOB    : return "WAIT_JOB";
  case TRANSFERSTATE_WAIT_MOUNTED: return "WAIT_MOUNTED";
  case TRANSFERSTATE_RUNNING     : return "RUNNING";
  default                        : return "UNKNOWN";
  }
}

//-----------------------------------------------------------------------------
// tapeIsBeingMounted
//-----------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::DriveCatalogueTransferSession::
  tapeIsBeingMounted() const throw() {
  return TRANSFERSTATE_WAIT_MOUNTED == m_state;
}

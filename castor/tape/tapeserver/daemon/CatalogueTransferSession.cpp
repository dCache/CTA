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
#include "castor/legacymsg/CupvProxy.hpp"
#include "castor/legacymsg/VmgrProxy.hpp"
#include "castor/tape/tapeserver/daemon/CatalogueTransferSession.hpp"
#include "h/Ctape_constants.h"
#include "h/Cupv_constants.h"
#include "h/rmc_constants.h"
#include "h/vmgr_constants.h"

#include <sys/types.h>
#include <signal.h>

//------------------------------------------------------------------------------
// create
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::CatalogueTransferSession*
  castor::tape::tapeserver::daemon::CatalogueTransferSession::create(
    log::Logger &log,
    const int netTimeout,
    const tape::utils::DriveConfig &driveConfig,
    const legacymsg::RtcpJobRqstMsgBody &vdqmJob,
    legacymsg::VmgrProxy &vmgr,
    legacymsg::CupvProxy &cupv,
    const std::string &hostName,
    const time_t waitJobTimeoutInSecs,
    const time_t mountTimeoutInSecs,
    const time_t blockMoveTimeoutInSecs,
    ProcessForkerProxy &processForker) {

  const pid_t pid = processForker.forkDataTransfer(driveConfig, vdqmJob);

  return new CatalogueTransferSession(
    log,
    netTimeout,
    pid,
    driveConfig,
    vdqmJob,
    vmgr,
    cupv,
    hostName,
    waitJobTimeoutInSecs,
    mountTimeoutInSecs,
    blockMoveTimeoutInSecs);
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::CatalogueTransferSession::
  CatalogueTransferSession(
  log::Logger &log,
  const int netTimeout,
  const pid_t pid,
  const tape::utils::DriveConfig &driveConfig,
  const legacymsg::RtcpJobRqstMsgBody &vdqmJob,
  legacymsg::VmgrProxy &vmgr,
  legacymsg::CupvProxy &cupv,
  const std::string &hostName,
  const time_t waitJobTimeoutInSecs,
  const time_t mountTimeoutInSecs,
  const time_t blockMoveTimeoutInSecs) throw():
  CatalogueSession(SESSION_TYPE_TRANSFER, log, netTimeout, pid, driveConfig),
  m_state(TRANSFERSTATE_WAIT_JOB),
  m_mode(WRITE_DISABLE),
  m_assignmentTime(time(0)),
  m_mountStartTime(0),
  m_lastTimeSomeBlocksWereMoved(0),
  m_vdqmJob(vdqmJob),
  m_vmgr(vmgr),
  m_cupv(cupv),
  m_hostName(hostName),
  m_waitJobTimeoutInSecs(waitJobTimeoutInSecs),
  m_mountTimeoutInSecs(mountTimeoutInSecs),
  m_blockMoveTimeoutInSecs(blockMoveTimeoutInSecs),
  m_sessionLogContext(log)
{
  // Record immediately the parameters from the vdqm request for the end of
  // session logs.
  typedef castor::log::Param Param;
  m_sessionLogContext.pushOrReplace(Param("volReqId", m_vdqmJob.volReqId));
  m_sessionLogContext.pushOrReplace(Param("dgn", m_vdqmJob.dgn));
  m_sessionLogContext.pushOrReplace(Param("driveUnit", m_vdqmJob.driveUnit));
  m_sessionLogContext.pushOrReplace(Param("clientHost", m_vdqmJob.clientHost));
  m_sessionLogContext.pushOrReplace(Param("clientPort", m_vdqmJob.clientPort));
  m_sessionLogContext.pushOrReplace(Param("clientEuid", m_vdqmJob.clientEuid));
  m_sessionLogContext.pushOrReplace(Param("clientEgid", m_vdqmJob.clientEgid));
}

//------------------------------------------------------------------------------
// handleTick
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::CatalogueTransferSession::handleTick() {
  switch(m_state) {
  case TRANSFERSTATE_WAIT_JOB:     return handleTickWhilstWaitJob();
  case TRANSFERSTATE_WAIT_MOUNTED: return handleTickWhilstWaitMounted();
  case TRANSFERSTATE_RUNNING:      return handleTickWhilstRunning();
  case TRANSFERSTATE_KILLED:       return handleTickWhilstKilled();
  default: return true; // Continue the main event loop
  }
}

//------------------------------------------------------------------------------
// handleTickWhilstWaitJob
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::CatalogueTransferSession::
  handleTickWhilstWaitJob() {
  const time_t now = time(0);
  const time_t secsWaiting = now - m_assignmentTime;
  const bool timeOutExceeded = secsWaiting > m_waitJobTimeoutInSecs;

  if(timeOutExceeded) {
    std::list<log::Param> params;
    params.push_back(log::Param("transferSessionPid", m_pid));
    params.push_back(log::Param("secsWaiting", secsWaiting));
    params.push_back(log::Param("waitJobTimeoutInSecs",
      m_waitJobTimeoutInSecs));
    m_log(LOG_ERR,
      "Killing data-transfer session because transfer job is too late",
      params);

    if(kill(m_pid, SIGKILL)) {
      const std::string errnoStr = castor::utils::errnoToString(errno);
      params.push_back(log::Param("message", errnoStr));
      m_log(LOG_ERR, "Failed to kill data-transfer session", params);
    } else {
      m_state = TRANSFERSTATE_KILLED;
    }
  }

  return true; // Continue the main event loop
}

//------------------------------------------------------------------------------
// handleTickWhilstWaitMounted
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::CatalogueTransferSession::
  handleTickWhilstWaitMounted() {
  const time_t now = time(0);
  const time_t secsWaiting = now - m_mountStartTime;
  const bool timeOutExceeded = secsWaiting > m_mountTimeoutInSecs;

  if(timeOutExceeded) {
    std::list<log::Param> params;
    params.push_back(log::Param("transferSessionPid", m_pid));
    params.push_back(log::Param("secsWaiting", secsWaiting));
    params.push_back(log::Param("mountTimeoutInSecs",
      m_mountTimeoutInSecs));
    m_log(LOG_ERR,
      "Killing data-transfer session because tape mount is taking too long",
      params);

    if(kill(m_pid, SIGKILL)) {
      const std::string errnoStr = castor::utils::errnoToString(errno);
      params.push_back(log::Param("message", errnoStr));
      m_log(LOG_ERR, "Failed to kill data-transfer session", params);
    } else {
      m_state = TRANSFERSTATE_KILLED;
    }
  }

  return true; // Continue the main event loop
}

//------------------------------------------------------------------------------
// handleTickWhilstRunning
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::CatalogueTransferSession::
  handleTickWhilstRunning() {
  const time_t now = time(0);
  const time_t secsWaiting = now - m_lastTimeSomeBlocksWereMoved;
  const bool timeOutExceeded = secsWaiting > m_blockMoveTimeoutInSecs;

  if(timeOutExceeded) {
    std::list<log::Param> params;
    params.push_back(log::Param("transferSessionPid", m_pid));
    params.push_back(log::Param("secsWaiting", secsWaiting));
    params.push_back(log::Param("blockMoveTimeoutInSecs",
      m_blockMoveTimeoutInSecs));
    m_log(LOG_ERR,
      "Killing data-transfer session because data blocks are not being moved",
      params);

    if(kill(m_pid, SIGKILL)) {
      const std::string errnoStr = castor::utils::errnoToString(errno);
      params.push_back(log::Param("message", errnoStr));
      m_log(LOG_ERR, "Failed to kill data-transfer session", params);
    } else {
      m_state = TRANSFERSTATE_KILLED;
    }
  }

  return true; // Continue the main event loop
}

//------------------------------------------------------------------------------
// handleTickWhilstKilled
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::CatalogueTransferSession::
  handleTickWhilstKilled() {
  return true; // Continue the main event loop
}

//------------------------------------------------------------------------------
// sessionSucceeded
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  sessionSucceeded() {
  m_sessionLogContext.pushOrReplace(log::Param("status","success"));
  m_sessionLogContext.log(LOG_INFO, "Tape session finished");
}

//------------------------------------------------------------------------------
// sessionFailed
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  sessionFailed() {
  m_sessionLogContext.pushOrReplace(log::Param("status","failed"));
  m_sessionLogContext.log(LOG_INFO, "Tape session finished");
}

//------------------------------------------------------------------------------
// getAssignmentTime
//------------------------------------------------------------------------------
time_t castor::tape::tapeserver::daemon::CatalogueTransferSession::
  getAssignmentTime() const throw() {
  return m_assignmentTime;
}

//------------------------------------------------------------------------------
// getVdqmJob
//------------------------------------------------------------------------------
castor::legacymsg::RtcpJobRqstMsgBody castor::tape::tapeserver::daemon::
  CatalogueTransferSession::getVdqmJob() const{
  return m_vdqmJob;
}

//-----------------------------------------------------------------------------
// receivedRecallJob
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  receivedRecallJob(const std::string &vid) {
  const char *const task = "accept reception of recall job";

  if(TRANSFERSTATE_WAIT_JOB != m_state) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to " << task <<
      ": Catalogue transfer-session state-mismatch: "
      "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_JOB) <<
      " actual=" << transferStateToStr(m_state);
    throw ex;
  }

  checkUserCanRecallFromTape(vid);

  m_mountStartTime = time(0);
  m_state = TRANSFERSTATE_WAIT_MOUNTED;

  m_mode = WRITE_DISABLE;
  m_vid = vid;
}

//-----------------------------------------------------------------------------
// checkUserCanRecallFromTape
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  checkUserCanRecallFromTape(const std::string &vid) {
  std::list<log::Param> params;

  params.push_back(log::Param("vid", vid));
  params.push_back(log::Param("clientEuid", m_vdqmJob.clientEuid));
  params.push_back(log::Param("clientEgid", m_vdqmJob.clientEgid));

  const legacymsg::VmgrTapeInfoMsgBody vmgrTape = m_vmgr.queryTape(vid);
  params.push_back(log::Param("status",
    castor::utils::tapeStatusToString(vmgrTape.status)));
  params.push_back(log::Param("poolName", vmgrTape.poolName));
  m_log(LOG_INFO, "Queried vmgr for the tape to be recalled", params);

  if(vmgrTape.status & EXPORTED) {
    castor::exception::Exception ex;
    ex.getMessage() << "Cannot recall from an EXPORTED tape: vid=" << vid;
    throw ex;
  }

  if(vmgrTape.status & ARCHIVED) {
    castor::exception::Exception ex;
    ex.getMessage() << "Cannot recall from an ARCHIVED tape: vid=" << vid;
    throw ex;
  }

  // Only tape operators can recall from a DISABLED tape
  if(vmgrTape.status & DISABLED) {
    const bool userIsTapeOperator = m_cupv.isGranted(
      m_vdqmJob.clientEuid,
      m_vdqmJob.clientEgid,
      m_vdqmJob.clientHost,
      m_hostName,
      P_TAPE_OPERATOR);
    params.push_back(log::Param("userIsTapeOperator", userIsTapeOperator ?
      "true" : "false"));
    m_log(LOG_INFO, "Tape is DISABLED, therefore querying cupv to see if user"
      " is a tape operator", params);

    if(!userIsTapeOperator) {
      castor::exception::Exception ex;
      ex.getMessage() << "Only a tape operator can recall from a DISABLED tape"
        ": vid=" << vid;
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// receivedMigrationJob
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  receivedMigrationJob(const std::string &vid) {
  if(TRANSFERSTATE_WAIT_JOB != m_state) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept reception of recall job"
      ": Catalogue transfer-session state-mismatch: "
      "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_JOB) <<
      " actual=" << transferStateToStr(m_state);
    throw ex;
  }

  checkUserCanMigrateToTape(vid);

  m_mountStartTime = time(0);
  m_state = TRANSFERSTATE_WAIT_MOUNTED;

  m_mode = WRITE_ENABLE;
  m_vid = vid;
}

//-----------------------------------------------------------------------------
// checkUserCanMigrateToTape
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  checkUserCanMigrateToTape(const std::string &vid) {
  std::list<log::Param> params;

  params.push_back(log::Param("vid", vid));
  params.push_back(log::Param("clientEuid", m_vdqmJob.clientEuid));
  params.push_back(log::Param("clientEgid", m_vdqmJob.clientEgid));

  const legacymsg::VmgrTapeInfoMsgBody vmgrTape = m_vmgr.queryTape(vid);
  params.push_back(log::Param("status",
    castor::utils::tapeStatusToString(vmgrTape.status)));
  params.push_back(log::Param("poolName", vmgrTape.poolName));
  m_log(LOG_INFO, "Queried vmgr for the tape for migration", params);

  if(vmgrTape.status & EXPORTED) {
    castor::exception::Exception ex;
    ex.getMessage() << "Cannot migrate files to an EXPORTED tape"
      ": vid=" << vid;
    throw ex;
  }

  if(vmgrTape.status & ARCHIVED) {
    castor::exception::Exception ex;
    ex.getMessage() << "Cannot migrate files to an ARCHIVED tape"
      ": vid=" << vid;
    throw ex;
  }

  if(vmgrTape.status & DISABLED) {
    castor::exception::Exception ex;
    ex.getMessage() << "Cannot migrate files to a DISABLED tape"
      ": vid=" << vid;
    throw ex;
  }

  const legacymsg::VmgrPoolInfoMsgBody vmgrPool =
    m_vmgr.queryPool(vmgrTape.poolName);
  params.push_back(log::Param("poolUid", vmgrPool.poolUid));
  params.push_back(log::Param("poolGid", vmgrPool.poolGid));
  m_log(LOG_INFO, "Queried vmgr for the pool of the tape for migration",
    params);

  // A pool has no owner if either its user or group ID is 0
  //
  // There is no such concept as a pool owned by the user root or the group root
  const bool poolHasOwner = 0 != vmgrPool.poolUid && 0 != vmgrPool.poolGid;

  if(!poolHasOwner) {
    castor::exception::Exception ex;
    ex.getMessage() <<
      "Cannot migrate files to a tape belonging to an owner-less tape-pool"
      ": vid=" << vid;
    throw ex;
  }

  // Only the owner of the pool of a tape can migrate files to that tape
  const bool userIsPoolOwner = m_vdqmJob.clientEuid == vmgrPool.poolUid &&
    m_vdqmJob.clientEgid == vmgrPool.poolGid;
  if(!userIsPoolOwner) {
    castor::exception::Exception ex;
    ex.getMessage() <<
      "Only the owner of the pool of a tape can migrate files to that tape"
      ": vid=" << vid;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getVid
//------------------------------------------------------------------------------
std::string castor::tape::tapeserver::daemon::CatalogueTransferSession::
  getVid() const {
  switch(m_state) {
  case TRANSFERSTATE_WAIT_MOUNTED:
  case TRANSFERSTATE_RUNNING:
    return m_vid;
  default:
    {
      castor::exception::Exception ex;
      ex.getMessage() << "Failed to get VID from catalogue transfer-session"
        ": Catalogue transfer-session is in an incompatible state: "
        " state=" << transferStateToStr(m_state);
      throw ex;
    }
  }
}

//------------------------------------------------------------------------------
// getMode
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::CatalogueTransferSession::
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
pid_t castor::tape::tapeserver::daemon::CatalogueTransferSession::
  getPid() const throw() {
  return m_pid;
}

//-----------------------------------------------------------------------------
// tapeMountedForMigration
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  tapeMountedForMigration(const std::string &vid) {
  if(TRANSFERSTATE_WAIT_MOUNTED != m_state) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for migration"
      ": Catalogue transfer-session state-mismatch: "
      "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_MOUNTED) <<
      " actual=" << transferStateToStr(m_state);
    throw ex;
  }

  // If the volume identifier of the data transfer job does not match the
  // mounted tape
  if(m_vid != vid) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for migration"
      ": VID mismatch: expected=" << m_vid << " actual=" << vid;
    throw ex;
  }

  // If the mount is not for migration
  if(WRITE_ENABLE != m_mode) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for migration"
      ": Data transfer job is not for migration";
    throw ex;
  }

  m_lastTimeSomeBlocksWereMoved = time(0); // Start watchdog timer
  m_state = TRANSFERSTATE_RUNNING;
}

//-----------------------------------------------------------------------------
// tapeMountedForRecall
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  tapeMountedForRecall(const std::string &vid) {

  if(TRANSFERSTATE_WAIT_MOUNTED != m_state) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for recall"
      ": Catalogue transfer-session state-mismatch: "
      "expected=" << transferStateToStr(TRANSFERSTATE_WAIT_MOUNTED) <<
      " actual=" << transferStateToStr(m_state);
    throw ex;
  }

  // If the volume identifier of the data transfer job does not match the
  // mounted tape
  if(m_vid != vid) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for recall"
      ": VID mismatch: expected=" << m_vid << " actual=" << vid;
    throw ex;
  }

  // If the mount is not for recall
  if(WRITE_DISABLE != m_mode) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept tape mounted for recall"
      ": Data transfer job is not for recall";
    throw ex;
  }

  m_lastTimeSomeBlocksWereMoved = time(0); // Start watchdog timer
  m_state = TRANSFERSTATE_RUNNING;
}

//-----------------------------------------------------------------------------
// transferStateToStr
//-----------------------------------------------------------------------------
const char *castor::tape::tapeserver::daemon::CatalogueTransferSession::
  transferStateToStr(const TransferState state) const throw() {
  switch(state) {
  case TRANSFERSTATE_WAIT_JOB    : return "WAIT_JOB";
  case TRANSFERSTATE_WAIT_MOUNTED: return "WAIT_MOUNTED";
  case TRANSFERSTATE_RUNNING     : return "RUNNING";
  case TRANSFERSTATE_KILLED      : return "KILLED";
  default                        : return "UNKNOWN";
  }
}

//-----------------------------------------------------------------------------
// tapeIsBeingMounted
//-----------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::CatalogueTransferSession::
  tapeIsBeingMounted() const throw() {
  return TRANSFERSTATE_WAIT_MOUNTED == m_state;
}

//-----------------------------------------------------------------------------
// receivedHeartbeat
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  receivedHeartbeat(const uint64_t nbBlocksMoved) {
  if(nbBlocksMoved > 0) {
    m_lastTimeSomeBlocksWereMoved = time(0);
  }
}

//-----------------------------------------------------------------------------
// addLogParam
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  addLogParam(const log::Param & param) {
  m_sessionLogContext.pushOrReplace(param);
}

//-----------------------------------------------------------------------------
// deleteLogParam
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueTransferSession::
  deleteLogParam(const std::string & paramName) {
  m_sessionLogContext.erase(paramName);
}

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

#include "castor/messages/ForkCleaner.pb.h"
#include "castor/messages/ForkSucceeded.pb.h"
#include "castor/messages/StopProcessForker.pb.h"
#include "castor/tape/tapeserver/daemon/ProcessForkerMsgType.hpp"
#include "castor/tape/tapeserver/daemon/ProcessForkerProxySocket.hpp"
#include "castor/tape/tapeserver/daemon/ProcessForkerUtils.hpp"
#include "h/serrno.h"

#include <errno.h>
#include <unistd.h>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForkerProxySocket::
  ProcessForkerProxySocket(log::Logger &log, const int socketFd) throw():
  m_log(log), m_socketFd(socketFd) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForkerProxySocket::
  ~ProcessForkerProxySocket() throw() {
  if(-1 == close(m_socketFd)) {
    char message[100];
    sstrerror_r(errno, message, sizeof(message));
    log::Param params[] = {log::Param("socketFd", m_socketFd), 
      log::Param("message", message)};
    m_log(LOG_ERR,
      "Failed to close proxy side of ProcessForker socket pair", params);
  } else {
    m_log(LOG_INFO, "Closed proxy side of ProcessForker socket pair");
  }
}

//------------------------------------------------------------------------------
// stopProcessForker
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForkerProxySocket::
  stopProcessForker(const std::string &reason) {
  messages::StopProcessForker msg;
  msg.set_reason(reason);
  ProcessForkerUtils::writeFrame(m_socketFd, msg);
  ProcessForkerUtils::readAck(m_socketFd);
  m_log(LOG_INFO, "Got ack of graceful shutdown from the ProcessForker");
}

//------------------------------------------------------------------------------
// forkDataTransfer
//------------------------------------------------------------------------------
pid_t castor::tape::tapeserver::daemon::ProcessForkerProxySocket::
  forkDataTransfer(const utils::DriveConfig &driveConfig,
    const legacymsg::RtcpJobRqstMsgBody vdqmJob,
    const DataTransferSession::CastorConf &conf) {

  const messages::ForkDataTransfer rqst = createForkDataTransferMsg(driveConfig,
    vdqmJob, conf);

  // Request the process forker to fork a data-transfer session
  ProcessForkerUtils::writeFrame(m_socketFd, rqst);

  // Read back the reply
  messages::ForkSucceeded reply;
  ProcessForkerUtils::readReplyOrEx(m_socketFd, reply);
  log::Param params[] = {log::Param("pid", reply.pid())};
  m_log(LOG_INFO,
    "Got process ID of the data-transfer session from the ProcessForker",
    params);

  return reply.pid();
}

//------------------------------------------------------------------------------
// createForkDataTransferMsg
//------------------------------------------------------------------------------
castor::messages::ForkDataTransfer
  castor::tape::tapeserver::daemon::ProcessForkerProxySocket::
  createForkDataTransferMsg(const utils::DriveConfig &driveConfig,
    const legacymsg::RtcpJobRqstMsgBody vdqmJob,
    const DataTransferSession::CastorConf &config) {
  messages::ForkDataTransfer msg;

  // Description of the tape drive
  fillMsgWithDriveConfig(msg, driveConfig);

  // Description of the client request
  msg.set_mounttransactionid(vdqmJob.volReqId);
  msg.set_clientport(vdqmJob.clientPort);
  msg.set_clienteuid(vdqmJob.clientEuid);
  msg.set_clientegid(vdqmJob.clientEgid);
  msg.set_clienthost(vdqmJob.clientHost);
  msg.set_clientusername(vdqmJob.clientUserName);

  // Configuration parameters of the session
  msg.set_memblocksize(config.rtcopydBufsz);
  msg.set_nbmemblocks(config.rtcopydNbBufs);
  msg.set_badmirhandling(config.tapeBadMIRHandlingRepair);
  msg.set_bulkrequestmigrationmaxbytes(
    config.tapebridgeBulkRequestMigrationMaxBytes);
  msg.set_bulkrequestmigrationmaxfiles(
    config.tapebridgeBulkRequestMigrationMaxFiles);
  msg.set_bulkrequestrecallmaxbytes(
    config.tapebridgeBulkRequestRecallMaxBytes);
  msg.set_bulkrequestrecallmaxfiles(
    config.tapebridgeBulkRequestRecallMaxFiles);
  msg.set_maxbytesbeforeflush(
    config.tapebridgeMaxBytesBeforeFlush);
  msg.set_maxfilesbeforeflush(
    config.tapebridgeMaxFilesBeforeFlush);
  msg.set_diskthreadpoolsize(
    config.tapeserverdDiskThreads);

  return msg;
}

//------------------------------------------------------------------------------
// forkLabel
//------------------------------------------------------------------------------
pid_t castor::tape::tapeserver::daemon::ProcessForkerProxySocket::
  forkLabel(const utils::DriveConfig &driveConfig,
  const legacymsg::TapeLabelRqstMsgBody &labelJob) {

  const messages::ForkLabel rqst = createForkLabelMsg(driveConfig, labelJob);

  ProcessForkerUtils::writeFrame(m_socketFd, rqst);

  // Read back the reply
  messages::ForkSucceeded reply;
  ProcessForkerUtils::readReplyOrEx(m_socketFd, reply);
  log::Param params[] = {log::Param("pid", reply.pid())};
  m_log(LOG_INFO, "Got process ID of the label session from the ProcessForker",
    params);

  return reply.pid();
}

//------------------------------------------------------------------------------
// createForkLabelMsg
//------------------------------------------------------------------------------
castor::messages::ForkLabel castor::tape::tapeserver::daemon::
  ProcessForkerProxySocket::createForkLabelMsg(
  const utils::DriveConfig &driveConfig,
  const legacymsg::TapeLabelRqstMsgBody &labelJob) {
  messages::ForkLabel msg;

  // Description of the tape drive
  fillMsgWithDriveConfig(msg, driveConfig);

  // Description of the label job
  fillMsgWithLabelJob(msg, labelJob);

  return msg;
}

//------------------------------------------------------------------------------
// forkCleaner
//------------------------------------------------------------------------------
pid_t castor::tape::tapeserver::daemon::ProcessForkerProxySocket::
  forkCleaner(const std::string &unitName, const std::string &vid) {

  // Request the process forker to fork a label session
  messages::ForkCleaner msg;
  msg.set_unitname(unitName);
  msg.set_vid(vid);
  ProcessForkerUtils::writeFrame(m_socketFd, msg);

  // Read back the reply
  messages::ForkSucceeded reply;
  ProcessForkerUtils::readReplyOrEx(m_socketFd, reply);
  log::Param params[] = {log::Param("pid", reply.pid())};
  m_log(LOG_INFO,
    "Got process ID of the cleaner session from the ProcessForker", params);

  return reply.pid();
}

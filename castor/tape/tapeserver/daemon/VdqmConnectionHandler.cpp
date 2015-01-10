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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "castor/exception/Exception.hpp"
#include "castor/tape/tapeserver/daemon/VdqmConnectionHandler.hpp"
#include "castor/utils/utils.hpp"
#include "h/common.h"
#include "h/serrno.h"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::VdqmConnectionHandler::VdqmConnectionHandler(
  const int fd,
  reactor::ZMQReactor &reactor,
  log::Logger &log,
  Catalogue &driveCatalogue,
  const TapeDaemonConfig &tapeDaemonConfig) throw():
  m_fd(fd),
  m_reactor(reactor),
  m_log(log),
  m_driveCatalogue(driveCatalogue),
  m_tapeDaemonConfig(tapeDaemonConfig),
  m_netTimeout(1) { // Timeout in seconds
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::VdqmConnectionHandler::~VdqmConnectionHandler() throw() {
  log::Param params[] = {log::Param("fd", m_fd)};
  m_log(LOG_DEBUG, "Closing vdqm connection", params);
  close(m_fd);
}

//------------------------------------------------------------------------------
// getName
//------------------------------------------------------------------------------
std::string castor::tape::tapeserver::daemon::VdqmConnectionHandler::getName() 
  const throw() {
  return "VdqmConnectionHandler";
}

//------------------------------------------------------------------------------
// fillPollFd
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::fillPollFd(zmq_pollitem_t &fd) throw() {
  fd.fd = m_fd;
  fd.events = ZMQ_POLLIN;
  fd.revents = 0;
  fd.socket = NULL;
}

//------------------------------------------------------------------------------
// handleEvent
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::VdqmConnectionHandler::handleEvent(
  const zmq_pollitem_t &fd)  {
  logConnectionEvent(fd);

  checkHandleEventFd(fd.fd);

  std::list<log::Param> params;
  params.push_back(log::Param("fd", m_fd));

  if(!connectionIsFromTrustedVdqmHost()) {
    return true; // Ask reactor to remove and delete this handler
  }

  try {
    const legacymsg::RtcpJobRqstMsgBody job = readJobMsg();
    logVdqmJobReception(job);
    writeJobReplyMsg();
    CatalogueDrive &drive = m_driveCatalogue.findDrive(job.driveUnit);
    drive.receivedVdqmJob(job);
  } catch(castor::exception::Exception &ex) {
    params.push_back(log::Param("message", ex.getMessage().str()));
    m_log(LOG_ERR, "Failed to handle vdqm-connection event", params);
  }

  m_log(LOG_DEBUG, "Asking reactor to remove and delete"
    " VdqmConnectionHandler", params);
  return true; // Ask reactor to remove and delete this handler
}

//------------------------------------------------------------------------------
// logConnectionEvent 
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  logConnectionEvent(const zmq_pollitem_t &fd)  {
  log::Param params[] = {
  log::Param("fd", fd.fd),
  log::Param("ZMQ_POLLIN", fd.revents & ZMQ_POLLIN ? "true" : "false"),
  log::Param("ZMQ_POLLOUT", fd.revents & ZMQ_POLLOUT ? "true" : "false"),
  log::Param("ZMQ_POLLERR", fd.revents & ZMQ_POLLERR ? "true" : "false")};
  m_log(LOG_DEBUG, "I/O event on vdqm connection", params);
}

//------------------------------------------------------------------------------
// checkHandleEventFd
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  checkHandleEventFd(const int fd)  {
  if(m_fd != fd) {
    castor::exception::Exception ex;
    ex.getMessage() <<
      "VdqmConnectionHandler passed wrong file descriptor"
      ": expected=" << m_fd << " actual=" << fd;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// connectionIsFromTrustedVdqmHost
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  connectionIsFromTrustedVdqmHost() throw() {
  try {
    const std::string vdqmHost = getPeerHostName(m_fd);

    std::list<log::Param> params;
    params.push_back(log::Param("vdqmHost", vdqmHost));
    m_log(LOG_INFO, "Received a vdqm connection" ,params);

    const std::vector<std::string> &vdqmHosts = m_tapeDaemonConfig.vdqmHosts;
    for(std::vector<std::string>::const_iterator itor = vdqmHosts.begin();
      itor != vdqmHosts.end(); itor++) {
      if(vdqmHost == *itor) {
        return true;
      }
    }

    m_log(LOG_WARNING, "Vdqm host is not trusted"
      ": Check the TapeServer:VdqmHosts parameter of castor.conf", params);
  } catch(castor::exception::Exception &ex) {
    log::Param params[] = {log::Param("message", ex.getMessage().str())};
    m_log(LOG_ERR, "Failed to determine if connection is from a trusted vdqm"
      " host", params);
  }

  return false;
}

//------------------------------------------------------------------------------
// getPeerHostName
//------------------------------------------------------------------------------
std::string castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  getPeerHostName(const int sockFd) {
  try {
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    getPeerName(sockFd, (struct sockaddr *)&addr, &addrLen);

    char host[NI_MAXHOST];
    getNameInfo((struct sockaddr *)&addr, addrLen, host, sizeof(host), NULL, 0,
      NI_NAMEREQD);
    host[sizeof(host) - 1] = '\0';

    return host;
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "getPeerHostName() failed: " << ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getPeerName
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::getPeerName(
  const int sockFd, struct sockaddr *const addr, socklen_t *const addrLen) {
  if(getpeername(sockFd, addr, addrLen)) {
    const std::string errMsg = castor::utils::errnoToString(errno);
    castor::exception::Exception ex;
    ex.getMessage() << "getpeername failed: sockFd=" << sockFd << ": " <<
      errMsg;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getNameInfo
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::getNameInfo(
  const struct sockaddr *const sa,
  const socklen_t saLen,
  char *const host,
  const size_t hostLen,
  char *const serv,
  const size_t servLen,
  const int flags) {
  const int rc = getnameinfo(sa, saLen, host, hostLen, serv, servLen, flags);
  if(rc) {
    const std::string errMsg = getNameInfoRcToString(rc);
    castor::exception::Exception ex;
    ex.getMessage() << "getnameinfo failed: " << errMsg;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getNameInfoRcToString
//------------------------------------------------------------------------------
const char *castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  getNameInfoRcToString(const int rc) {
  switch(rc) {
  case 0: return "Success";
  case EAI_AGAIN: return "Try again";
  case EAI_BADFLAGS: return "Invalid flags";
  case EAI_FAIL: return "Non-recoverable error";
  case EAI_FAMILY: return "Invalid address family";
  case EAI_MEMORY: return "Out of memory";
  case EAI_NONAME: return "Name does not resolve";
  case EAI_OVERFLOW: return "Either host of serv buffer was too small";
  case EAI_SYSTEM: return "System error";
  default: return "Unknown";
  }
}

//------------------------------------------------------------------------------
// logVdqmJobReception
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::logVdqmJobReception(const legacymsg::RtcpJobRqstMsgBody &job) const throw() {
  log::Param params[] = {
    log::Param("volReqId", job.volReqId),
    log::Param("clientPort", job.clientPort),
    log::Param("clientEuid", job.clientEuid),
    log::Param("clientEgid", job.clientEgid),
    log::Param("clientHost", job.clientHost),
    log::Param("dgn", job.dgn),
    log::Param("driveUnit", job.driveUnit),
    log::Param("clientUserName", job.clientUserName)};
  m_log(LOG_INFO, "Received job from the vdqmd daemon", params);
}

//------------------------------------------------------------------------------
// readJobMsg
//------------------------------------------------------------------------------
castor::legacymsg::RtcpJobRqstMsgBody
  castor::tape::tapeserver::daemon::VdqmConnectionHandler::readJobMsg()  {
  const legacymsg::MessageHeader header = readMsgHeader();
  const legacymsg::RtcpJobRqstMsgBody body = readJobMsgBody(header.lenOrStatus);

  return body;
}

//------------------------------------------------------------------------------
// readMsgHeader
//------------------------------------------------------------------------------
castor::legacymsg::MessageHeader
  castor::tape::tapeserver::daemon::VdqmConnectionHandler::readMsgHeader()  {
  // Read in the message header
  char buf[3 * sizeof(uint32_t)]; // magic + request type + len
  io::readBytes(m_fd, m_netTimeout, sizeof(buf), buf);

  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::MessageHeader header;
  memset(&header, '\0', sizeof(header));
  legacymsg::unmarshal(bufPtr, bufLen, header);

  if(RTCOPY_MAGIC_OLD0 != header.magic) {
    castor::exception::Exception ex;
    ex.getMessage() << "Invalid vdqm job message: Invalid magic"
      ": expected=0x" << std::hex << RTCOPY_MAGIC_OLD0 << " actual=0x" <<
      header.magic;
    throw ex;
  }

  if(VDQM_CLIENTINFO != header.reqType) {
    castor::exception::Exception ex;
    ex.getMessage() << "Invalid vdqm job message: Invalid request type"
       ": expected=0x" << std::hex << VDQM_CLIENTINFO << " actual=0x" <<
       header.reqType;
    throw ex;
  }

  // The length of the message body is checked later, just before it is read in
  // to memory

  return header;
}

//------------------------------------------------------------------------------
// readJobMsgBody
//------------------------------------------------------------------------------
castor::legacymsg::RtcpJobRqstMsgBody castor::tape::tapeserver::daemon::
  VdqmConnectionHandler::readJobMsgBody(const uint32_t len)  {
  char buf[1024];

  if(sizeof(buf) < len) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to read body of job message"
       ": Maximum body length exceeded"
       ": max=" << sizeof(buf) << " actual=" << len;
    throw ex;
  }

  try {
    io::readBytes(m_fd, m_netTimeout, len, buf);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to read body of job message"
      ": " << ne.getMessage().str();
    throw ex;
  }

  legacymsg::RtcpJobRqstMsgBody body;
  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::unmarshal(bufPtr, bufLen, body);
  return body;
}

//------------------------------------------------------------------------------
// writeJobReplyMsg
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  writeJobReplyMsg()  {
  legacymsg::RtcpJobReplyMsgBody body;
  char buf[1024];
  const size_t len = legacymsg::marshal(buf, body);
  try {
    io::writeBytes(m_fd, m_netTimeout, len, buf);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to write job reply message: " <<
      ne.getMessage().str();
    throw ex;
  }
}

/******************************************************************************
 *         castor/tape/tapeserver/daemon/VdqmConnectionHandler.cpp
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#include "castor/tape/tapeserver/daemon/VdqmConnectionHandler.hpp"
#include "h/common.h"
#include "h/serrno.h"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::VdqmConnectionHandler::VdqmConnectionHandler(
  const int connection,
  io::PollReactor &reactor,
  log::Logger &log,
  Vdqm &vdqm,
  DriveCatalogue &driveCatalogue) throw():
    m_connection(connection),
    m_reactor(reactor),
    m_log(log),
    m_vdqm(vdqm),
    m_driveCatalogue(driveCatalogue),
    m_netTimeout(1) // 1 second
{
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  ~VdqmConnectionHandler() throw() {
  close(m_connection);
}

//------------------------------------------------------------------------------
// getFd
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::VdqmConnectionHandler::getFd() throw() {
  return m_connection;
}

//------------------------------------------------------------------------------
// fillPollFd
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::fillPollFd(
  struct pollfd &fd) throw() {
  fd.fd = m_connection;
  fd.events = POLLRDNORM;
  fd.revents = 0;
}

//------------------------------------------------------------------------------
// handleEvent
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::handleEvent(
  const struct pollfd &fd) throw(castor::exception::Exception) {
  checkHandleEventFd(fd.fd);

  {
    log::Param params[] = {
      log::Param("POLLIN"    , fd.revents & POLLIN     ? "true" : "false"),
      log::Param("POLLRDNORM", fd.revents & POLLRDNORM ? "true" : "false"),
      log::Param("POLLRDBAND", fd.revents & POLLRDBAND ? "true" : "false"),
      log::Param("POLLPRI"   , fd.revents & POLLPRI    ? "true" : "false"),
      log::Param("POLLOUT"   , fd.revents & POLLOUT    ? "true" : "false"),
      log::Param("POLLWRNORM", fd.revents & POLLWRNORM ? "true" : "false"),
      log::Param("POLLWRBAND", fd.revents & POLLWRBAND ? "true" : "false"),
      log::Param("POLLERR"   , fd.revents & POLLERR    ? "true" : "false"),
      log::Param("POLLHUP"   , fd.revents & POLLHUP    ? "true" : "false"),
      log::Param("POLLNVAL"  , fd.revents & POLLNVAL   ? "true" : "false")};
    m_log(LOG_DEBUG, "VdqmConnectionHandler::handleEvent()", params);
  }

  // Do nothing if there is no data to read
  //
  // POLLIN is unfortuntaley not the logical or of POLLRDNORM and POLLRDBAND
  // on SLC 5.  I therefore replaced POLLIN with the logical or.  I also
  // added POLLPRI into the mix to cover all possible types of read event.
  if(0 == (fd.revents & POLLRDNORM) && 0 == (fd.revents & POLLRDBAND) &&
    0 == (fd.revents & POLLPRI)) {
    return;
  }

  if(connectionIsAuthorized()) {
    const legacymsg::RtcpJobRqstMsgBody job = readJobMsg(fd.fd);
    logVdqmJobReception(job);
    writeJobReplyMsg(fd.fd);
  }

  m_reactor.removeHandler(this);
}

//------------------------------------------------------------------------------
// checkHandleEventFd
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  checkHandleEventFd(const int fd) throw (castor::exception::Exception) {
  if(m_connection != fd) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to handle vdqm connection"
      ": Event handler passed wrong file descriptor"
      ": expected=" << m_connection << " actual=" << fd;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// connectionIsAuthorized
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::VdqmConnectionHandler::
  connectionIsAuthorized() throw() {
  // isadminhost fills in peerHost
  char peerHost[CA_MAXHOSTNAMELEN+1];
  memset(peerHost, '\0', sizeof(peerHost));

  // Unfortunately isadminhost can either set errno or serrno
  serrno = 0;
  errno = 0;
  const int rc = isadminhost(m_connection, peerHost);
  if(0 == rc) {
    log::Param params[] = {
      log::Param("host", peerHost[0] != '\0' ? peerHost : "UNKNOWN")};
    m_log(LOG_INFO, "Received connection from authorized admin host", params);
    return true; // Connection is authorized
  } else {
    // Give preference to serrno over errno when it comes to isadminhost()
    const int savedErrno = errno;
    const int savedSerrno = serrno;
    std::string errorMessage;
    if(0 != serrno) {
      errorMessage = sstrerror(savedSerrno);
    } else if(0 != errno) {
      errorMessage = sstrerror(savedErrno);
    } else {
      errorMessage = "UNKNOWN";
    }
    log::Param params[] = {
      log::Param("host", peerHost[0] != '\0' ? peerHost : "UNKNOWN"),
      log::Param("message", errorMessage)};
    m_log(LOG_ERR, "Failed to authorize vdqm host", params);

    return false; // Connection is not authorized
  }
}

//------------------------------------------------------------------------------
// logVdqmJobReception
//------------------------------------------------------------------------------
void
  castor::tape::tapeserver::daemon::VdqmConnectionHandler::logVdqmJobReception(
  const legacymsg::RtcpJobRqstMsgBody &job) const throw() {
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
castor::tape::legacymsg::RtcpJobRqstMsgBody
  castor::tape::tapeserver::daemon::VdqmConnectionHandler::readJobMsg(
    const int connection) throw(castor::exception::Exception) {
  const legacymsg::MessageHeader header = readJobMsgHeader(connection);
  const legacymsg::RtcpJobRqstMsgBody body = readJobMsgBody(connection,
    header.lenOrStatus);

  return body;
}

//------------------------------------------------------------------------------
// readJobMsgHeader
//------------------------------------------------------------------------------
castor::tape::legacymsg::MessageHeader
  castor::tape::tapeserver::daemon::VdqmConnectionHandler::readJobMsgHeader(
    const int connection) throw(castor::exception::Exception) {
  // Read in the message header
  char buf[3 * sizeof(uint32_t)]; // magic + request type + len
  io::readBytes(connection, m_netTimeout, sizeof(buf), buf);

  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::MessageHeader header;
  memset(&header, '\0', sizeof(header));
  legacymsg::unmarshal(bufPtr, bufLen, header);

  if(RTCOPY_MAGIC_OLD0 != header.magic) {
    castor::exception::Internal ex;
    ex.getMessage() << "Invalid vdqm job message: Invalid magic"
      ": expected=0x" << std::hex << RTCOPY_MAGIC_OLD0 << " actual=0x" <<
      header.magic;
    throw ex;
  }

  if(VDQM_CLIENTINFO != header.reqType) {
    castor::exception::Internal ex;
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
castor::tape::legacymsg::RtcpJobRqstMsgBody
  castor::tape::tapeserver::daemon::VdqmConnectionHandler::readJobMsgBody(
    const int connection, const uint32_t len)
    throw(castor::exception::Exception) {
  char buf[1024];

  if(sizeof(buf) < len) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to read body of job message"
       ": Maximum body length exceeded"
       ": max=" << sizeof(buf) << " actual=" << len;
    throw ex;
  }

  try {
    io::readBytes(connection, m_netTimeout, len, buf);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
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
void castor::tape::tapeserver::daemon::VdqmConnectionHandler::writeJobReplyMsg(
  const int connection) throw(castor::exception::Exception) {
  legacymsg::RtcpJobReplyMsgBody body;
  char buf[1024];
  const size_t len = legacymsg::marshal(buf, body);
  try {
    io::writeBytes(connection, m_netTimeout, len, buf);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to write job reply message: " <<
      ne.getMessage().str();
    throw ex;
  }
}

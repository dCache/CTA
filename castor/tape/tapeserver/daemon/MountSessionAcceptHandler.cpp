/******************************************************************************
 *         castor/tape/tapeserver/daemon/MountSessionAcceptHandler.cpp
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
 * @author dkruse@cern.ch
 *****************************************************************************/

#include "castor/exception/BadAlloc.hpp"
#include "castor/exception/Errnum.hpp"
#include "castor/io/io.hpp"
#include "castor/legacymsg/CommonMarshal.hpp"
#include "castor/legacymsg/legacymsg.hpp"
#include "castor/legacymsg/TapeMarshal.hpp"
#include "castor/tape/utils/utils.hpp"
#include "castor/tape/tapeserver/daemon/DriveCatalogue.hpp"
#include "castor/tape/tapeserver/daemon/MountSessionAcceptHandler.hpp"
#include "castor/utils/SmartFd.hpp"
#include "h/common.h"
#include "h/serrno.h"
#include "h/Ctape.h"

#include <vdqm_api.h>

#include <errno.h>
#include <memory>
#include <string.h>
#include <list>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::MountSessionAcceptHandler::MountSessionAcceptHandler(
  const int fd, io::PollReactor &reactor, log::Logger &log,
  DriveCatalogue &driveCatalogue, const std::string &hostName) throw():
  m_fd(fd),
  m_reactor(reactor),
  m_log(log),
  m_driveCatalogue(driveCatalogue),
  m_hostName(hostName),
  m_netTimeout(10) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::MountSessionAcceptHandler::~MountSessionAcceptHandler()
  throw() {
}

//------------------------------------------------------------------------------
// getFd
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::MountSessionAcceptHandler::getFd() throw() {
  return m_fd;
}

//------------------------------------------------------------------------------
// fillPollFd
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::MountSessionAcceptHandler::fillPollFd(
  struct pollfd &fd) throw() {
  fd.fd = m_fd;
  fd.events = POLLRDNORM;
  fd.revents = 0;
}

//------------------------------------------------------------------------------
// handleEvent
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::MountSessionAcceptHandler::handleEvent(
  const struct pollfd &fd)  {
  checkHandleEventFd(fd.fd);

  // Do nothing if there is no data to read
  //
  // POLLIN is unfortunately not the logical or of POLLRDNORM and POLLRDBAND
  // on SLC 5.  I therefore replaced POLLIN with the logical or.  I also
  // added POLLPRI into the mix to cover all possible types of read event.
  if(0 == (fd.revents & POLLRDNORM) && 0 == (fd.revents & POLLRDBAND) &&
    0 == (fd.revents & POLLPRI)) {
    return false; // Stay registered with the reactor
  }

  // Accept the connection from the admin command
  castor::utils::SmartFd connection;
  try {
    const time_t acceptTimeout  = 1; // Timeout in seconds
    connection.reset(io::acceptConnection(fd.fd, acceptTimeout));
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept a connection from the admin command"
      ": " << ne.getMessage().str();
    throw ex;
  }

  // Read in the message header
  const legacymsg::MessageHeader header = readJobMsgHeader(connection.get());
  
  // handleIncomingJob() takes ownership of the client connection because in
  // the good-day scenario it will pass the client connection to the catalogue
  // of tape drives
  handleIncomingJob(header, connection.release());

  return false; // Stay registered with the reactor
}

//------------------------------------------------------------------------------
// checkHandleEventFd
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::MountSessionAcceptHandler::checkHandleEventFd(
  const int fd)  {
  if(m_fd != fd) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to accept connection from the admin command"
      ": Event handler passed wrong file descriptor"
      ": expected=" << m_fd << " actual=" << fd;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// logTapeConfigJobReception
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::MountSessionAcceptHandler::logSetVidJobReception(
  const legacymsg::TapeUpdateDriveRqstMsgBody &job) const throw() {
  log::Param params[] = {
    log::Param("drive", job.drive),
    log::Param("vid", job.vid)};
  m_log(LOG_INFO, "Received message from mount session to set VID of a drive", params);
}

//------------------------------------------------------------------------------
// logLabelJobReception
//------------------------------------------------------------------------------
void
  castor::tape::tapeserver::daemon::MountSessionAcceptHandler::logLabelJobReception(const legacymsg::TapeLabelRqstMsgBody &job) const throw() {
  log::Param params[] = {
    log::Param("drive", job.drive),
    log::Param("vid", job.vid),
    log::Param("dgn", job.dgn),
    log::Param("uid", job.uid),
    log::Param("gid", job.gid)};
  m_log(LOG_INFO, "Received message to label a tape", params);
}

//------------------------------------------------------------------------------
// handleIncomingJob
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::MountSessionAcceptHandler::handleIncomingJob(
  const legacymsg::MessageHeader &header, const int clientConnection) {

  switch(header.reqType) {
  case SETVID:
    // handleIncomingSetVidJob takes ownership of the client connection
    handleIncomingSetVidJob(header, clientConnection);
    break;
  case TPLABEL:
    // handleIncomingSetVidJob takes ownership of the client connection
    handleIncomingLabelJob(header, clientConnection);
    break;
  default:
    {
      // Close the client connection because this method still owns the
      // connection and has not been able to pass it on
      close(clientConnection);

      castor::exception::Exception ex;
      ex.getMessage() << "Unknown request type: " << header.reqType << ". Expected: " << SETVID << "(SETVID)";
      throw ex;
    }
  }
}

//------------------------------------------------------------------------------
// handleIncomingSetVidJob
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::MountSessionAcceptHandler::handleIncomingSetVidJob(const legacymsg::MessageHeader &header, const int clientConnection) {
  castor::utils::SmartFd connection(clientConnection);
  const char *const task = "handle incoming set vid job";
  try {
    // Read message body
    const uint32_t bodyLen = header.lenOrStatus - 3 * sizeof(uint32_t);
    const legacymsg::TapeUpdateDriveRqstMsgBody body = readSetVidMsgBody(connection.get(), bodyLen);
    logSetVidJobReception(body);
  
    m_driveCatalogue.updateVidAssignment(body.vid, body.drive);
    // 0 as return code for the tape config command, as in: "all went fine"
    legacymsg::writeTapeReplyMsg(m_netTimeout, connection.get(), 0, "");
    close(connection.release());
  } catch(castor::exception::Exception &ex) {
    log::Param params[] = {log::Param("message", ex.getMessage().str())};
    m_log(LOG_ERR, "Informing mount session of error", params);

    // Inform the client there was an error
    try {
      legacymsg::writeTapeReplyMsg(m_netTimeout, connection.get(), 1, ex.getMessage().str());
    } catch(castor::exception::Exception &ne) {
      castor::exception::Exception ex;
      ex.getMessage() << "Failed to " << task <<
        ": Failed to inform the client there was an error: " <<
        ne.getMessage().str();
      throw ex;
    }
  }    
}

//------------------------------------------------------------------------------
// handleIncomingLabelJob
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::MountSessionAcceptHandler::handleIncomingLabelJob(
  const legacymsg::MessageHeader &header, const int clientConnection) throw() {
  castor::utils::SmartFd connection(clientConnection);
  const char *const task = "handle incoming label job";

  try {
    // Read message body
    const uint32_t bodyLen = header.lenOrStatus - 3 * sizeof(uint32_t);
    const legacymsg::TapeLabelRqstMsgBody body = readLabelRqstMsgBody(connection.get(), bodyLen);
    logLabelJobReception(body);

    // Try to inform the drive catalogue of the reception of the label job
    //
    // PLEASE NOTE that this method must keep ownership of the client connection
    // whilst informing the drive catalogue of the reception of the label job.
    // If the drive catalogue throws an exception whilst evaluating the drive
    // state-change then the catalogue will NOT have closed the connection.
    m_driveCatalogue.receivedLabelJob(body, connection.get());

    // The drive catalogue will now remember the client connection, therefore it
    // is now safe and necessary for this method to relinquish ownership of the
    // connection
    connection.release();
  } catch(castor::exception::Exception &ex) {
    log::Param params[] = {log::Param("message", ex.getMessage().str())};
    m_log(LOG_ERR, "Informing client label-command of error", params);

    // Inform the client there was an error
    try {
      legacymsg::writeTapeReplyMsg(m_netTimeout, connection.get(), 1, ex.getMessage().str());
    } catch(castor::exception::Exception &ne) {
      castor::exception::Exception ex;
      ex.getMessage() << "Failed to " << task <<
        ": Failed to inform the client there was an error: " <<
        ne.getMessage().str();
      throw ex;
    }
  }
}

//------------------------------------------------------------------------------
// readJobMsgHeader
//------------------------------------------------------------------------------
castor::legacymsg::MessageHeader
  castor::tape::tapeserver::daemon::MountSessionAcceptHandler::readJobMsgHeader(
    const int clientConnection)  {
  castor::utils::SmartFd connection(clientConnection);
  
  // Read in the message header
  char buf[3 * sizeof(uint32_t)]; // magic + request type + len
  io::readBytes(connection.get(), m_netTimeout, sizeof(buf), buf);

  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::MessageHeader header;
  memset(&header, '\0', sizeof(header));
  legacymsg::unmarshal(bufPtr, bufLen, header);

  if(TPMAGIC != header.magic) {
    castor::exception::Exception ex;
    ex.getMessage() << "Invalid admin job message: Invalid magic"
      ": expected=0x" << std::hex << TPMAGIC << " actual=0x" <<
      header.magic;
    throw ex;
  }

  // The length of the message body is checked later, just before it is read in
  // to memory

  connection.release();
  return header;
}

//------------------------------------------------------------------------------
// readSetVidMsgBody
//------------------------------------------------------------------------------
castor::legacymsg::TapeUpdateDriveRqstMsgBody
  castor::tape::tapeserver::daemon::MountSessionAcceptHandler::readSetVidMsgBody(const int connection,
    const uint32_t len)
     {
  char buf[REQBUFSZ];

  if(sizeof(buf) < len) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to read body of job message"
       ": Maximum body length exceeded"
       ": max=" << sizeof(buf) << " actual=" << len;
    throw ex;
  }

  try {
    io::readBytes(connection, m_netTimeout, len, buf);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to read body of job message"
      ": " << ne.getMessage().str();
    throw ex;
  }

  legacymsg::TapeUpdateDriveRqstMsgBody body;
  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::unmarshal(bufPtr, bufLen, body);
  return body;
}

//------------------------------------------------------------------------------
// readLabelRqstMsgBody
//------------------------------------------------------------------------------
castor::legacymsg::TapeLabelRqstMsgBody
  castor::tape::tapeserver::daemon::MountSessionAcceptHandler::readLabelRqstMsgBody(const int connection,
    const uint32_t len)
     {
  char buf[REQBUFSZ];

  if(sizeof(buf) < len) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to read body of job message"
       ": Maximum body length exceeded"
       ": max=" << sizeof(buf) << " actual=" << len;
    throw ex;
  }

  try {
    io::readBytes(connection, m_netTimeout, len, buf);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to read body of job message"
      ": " << ne.getMessage().str();
    throw ex;
  }

  legacymsg::TapeLabelRqstMsgBody body;
  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::unmarshal(bufPtr, bufLen, body);
  return body;
}

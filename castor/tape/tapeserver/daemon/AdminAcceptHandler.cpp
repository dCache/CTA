/******************************************************************************
 *         castor/tape/tapeserver/daemon/AdminAcceptHandler.cpp
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
#include "castor/exception/Internal.hpp"
#include "castor/io/io.hpp"
#include "castor/tape/tapeserver/daemon/AdminAcceptHandler.hpp"
#include "castor/tape/tapeserver/daemon/VdqmConnectionHandler.hpp"
#include "castor/utils/SmartFd.hpp"
#include "h/common.h"
#include "h/serrno.h"
#include "h/Ctape.h"
#include "h/vdqm_api.h"
#include "castor/legacymsg/CommonMarshal.hpp"
#include "castor/legacymsg/TapeMarshal.hpp"
#include "castor/tape/utils/utils.hpp"

#include <errno.h>
#include <memory>
#include <string.h>
#include <list>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::AdminAcceptHandler::AdminAcceptHandler(
  const int fd,
  io::PollReactor &reactor,
  log::Logger &log,
  legacymsg::VdqmProxyFactory &vdqmFactory,
  DriveCatalogue &driveCatalogue,
  const std::string &hostName)
  throw():
    m_fd(fd),
    m_reactor(reactor),
    m_log(log),
    m_vdqmFactory(vdqmFactory),
    m_driveCatalogue(driveCatalogue),
    m_hostName(hostName),
    m_netTimeout(10) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::AdminAcceptHandler::~AdminAcceptHandler()
  throw() {
}

//------------------------------------------------------------------------------
// getFd
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::AdminAcceptHandler::getFd() throw() {
  return m_fd;
}

//------------------------------------------------------------------------------
// fillPollFd
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::AdminAcceptHandler::fillPollFd(
  struct pollfd &fd) throw() {
  fd.fd = m_fd;
  fd.events = POLLRDNORM;
  fd.revents = 0;
}

//-----------------------------------------------------------------------------
// marshalTapeConfigReplyMsg
//-----------------------------------------------------------------------------
size_t castor::tape::tapeserver::daemon::AdminAcceptHandler::marshalTapeRcReplyMsg(char *const dst,
  const size_t dstLen, const int rc)
  throw(castor::exception::Exception) {
  legacymsg::MessageHeader src;
  src.magic = TPMAGIC;
  src.reqType = TAPERC;
  src.lenOrStatus = rc;  
  return legacymsg::marshal(dst, dstLen, src);
}

//------------------------------------------------------------------------------
// writeTapeConfigReplyMsg
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::AdminAcceptHandler::writeTapeRcReplyMsg(const int fd, const int rc) throw(castor::exception::Exception) {
  char buf[REPBUFSZ];
  const size_t len = marshalTapeRcReplyMsg(buf, sizeof(buf), rc);
  try {
    io::writeBytes(fd, m_netTimeout, len, buf);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to write job reply message: " <<
      ne.getMessage().str();
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// marshalTapeStatReplyMsg
//-----------------------------------------------------------------------------
size_t castor::tape::tapeserver::daemon::AdminAcceptHandler::marshalTapeStatReplyMsg(char *const dst,
  const size_t dstLen, const legacymsg::TapeStatReplyMsgBody &body)
  throw(castor::exception::Exception) {

  if(dst == NULL) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to marshal TapeStatReplyMsgBody"
      ": Pointer to destination buffer is NULL";
    throw ex;
  }

  // Calculate the length of the message body
  uint32_t len = sizeof(body.number_of_drives);
  for(uint16_t i = 0; i<body.number_of_drives; i++) {
    len +=
      sizeof(body.drives[i].uid)         +
      sizeof(body.drives[i].jid)         +
      strlen(body.drives[i].dgn) + 1     +
      sizeof(body.drives[i].up)          +
      sizeof(body.drives[i].asn)         +
      sizeof(body.drives[i].asn_time)    +
      strlen(body.drives[i].drive) + 1   +
      sizeof(body.drives[i].mode)        +
      strlen(body.drives[i].lblcode) + 1 +
      sizeof(body.drives[i].tobemounted) +
      strlen(body.drives[i].vid) + 1     +
      strlen(body.drives[i].vsn) + 1     +
      sizeof(body.drives[i].cfseq);
  }

  // Calculate the total length of the message (header + body)
  size_t totalLen =
    sizeof(uint32_t) + // Magic number
    sizeof(uint32_t) + // Request type
    sizeof(uint32_t) + // Length of message body
    len;
  
  // Check that the message header buffer is big enough
  if(totalLen > dstLen) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to marshal TapeStatReplyMsgBody"
      ": Buffer too small: required=" << totalLen << " actual=" << dstLen;
    throw ex;
  }
  
  // Marshal message header
  char *p = dst;
  io::marshalUint32(TPMAGIC, p); // Magic number
  io::marshalUint32(MSG_DATA, p); // Request type
  io::marshalUint32(len, p); // Length of message body
  
  // Marshal message body
  io::marshalUint16(body.number_of_drives, p);  
  
  // Marshal the info of each unit
  for(int i=0; i<body.number_of_drives; i++) {
    io::marshalUint32(body.drives[i].uid, p);
    io::marshalUint32(body.drives[i].jid, p);
    io::marshalString(body.drives[i].dgn, p);
    io::marshalUint16(body.drives[i].up, p);
    io::marshalUint16(body.drives[i].asn, p);
    io::marshalUint32(body.drives[i].asn_time, p);
    io::marshalString(body.drives[i].drive, p);
    io::marshalUint16(body.drives[i].mode, p);
    io::marshalString(body.drives[i].lblcode, p);
    io::marshalUint16(body.drives[i].tobemounted, p);
    io::marshalString(body.drives[i].vid, p);
    io::marshalString(body.drives[i].vsn, p);
    io::marshalUint32(body.drives[i].cfseq, p);
  }

  // Calculate the number of bytes actually marshalled
  const size_t nbBytesMarshalled = p - dst;

  // Check that the number of bytes marshalled was what was expected
  if(totalLen != nbBytesMarshalled) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to marshal TapeStatReplyMsgBody"
      ": Mismatch between expected total length and actual"
      ": expected=" << totalLen << " actual=" << nbBytesMarshalled;
    throw ex;
  }

  return totalLen;
}

//------------------------------------------------------------------------------
// writeTapeStatReplyMsg
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::AdminAcceptHandler::writeTapeStatReplyMsg(const int fd) throw(castor::exception::Exception) {
  legacymsg::TapeStatReplyMsgBody body;
  
  const std::list<std::string> unitNames = m_driveCatalogue.getUnitNames();
  if(unitNames.size()>CA_MAXNBDRIVES) {
    castor::exception::Internal ex;
    ex.getMessage() << "Too many drives in drive catalogue: " << unitNames.size() << ". Max allowed: " << CA_MAXNBDRIVES << ".";
    throw ex;
  }
  body.number_of_drives = unitNames.size();
  int i=0;
  for(std::list<std::string>::const_iterator itor = unitNames.begin(); itor!=unitNames.end() and i<CA_MAXNBDRIVES; itor++) {
    body.drives[i].uid=getuid();
    body.drives[i].jid=m_driveCatalogue.getSessionPid(*itor);
    strncpy(body.drives[i].dgn, m_driveCatalogue.getDgn(*itor).c_str(), CA_MAXDGNLEN);
    body.drives[i].dgn[CA_MAXDGNLEN]='\0'; // this shouldn't be needed since we zero the structure before using it. but you never know...
    (m_driveCatalogue.getState(*itor) == DriveCatalogue::DRIVE_STATE_UP) ? body.drives[i].up=1 : body.drives[i].up=0;
    body.drives[i].asn=m_driveCatalogue.getState(*itor);
    body.drives[i].asn_time=m_driveCatalogue.getAssignmentTime(*itor);
    strncpy(body.drives[i].drive, (*itor).c_str(), CA_MAXUNMLEN);
    body.drives[i].drive[CA_MAXUNMLEN]='\0';
    body.drives[i].mode=WRITE_DISABLE;
    strncpy(body.drives[i].lblcode, "aul", CA_MAXLBLTYPLEN); // only aul format is used
    body.drives[i].lblcode[CA_MAXLBLTYPLEN]='\0';
    body.drives[i].tobemounted=0; // TODO: put 1 if the tape is mounting 0 otherwise
    strncpy(body.drives[i].vid, m_driveCatalogue.getVid(*itor).c_str(), CA_MAXVIDLEN);
    body.drives[i].vid[CA_MAXVIDLEN]='\0';
    strncpy(body.drives[i].vsn, m_driveCatalogue.getVid(*itor).c_str(), CA_MAXVSNLEN);
    body.drives[i].vid[CA_MAXVSNLEN]='\0';
    body.drives[i].cfseq=0; // the fseq is ignored by tpstat, so we leave it empty
    i++;
  }
  
  char buf[REPBUFSZ];
  const size_t len = marshalTapeStatReplyMsg(buf, sizeof(buf), body);
  try {
    io::writeBytes(fd, m_netTimeout, len, buf);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to write job reply message: " <<
      ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// handleEvent
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::AdminAcceptHandler::handleEvent(
  const struct pollfd &fd) throw(castor::exception::Exception) {
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
    connection.reset(io::acceptConnection(fd.fd, 1));
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to accept a connection from the admin command"
      ": " << ne.getMessage().str();
    throw ex;
  }
  
  dispatchJob(connection.get());  
  return false; // Stay registered with the reactor
}

//------------------------------------------------------------------------------
// checkHandleEventFd
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::AdminAcceptHandler::checkHandleEventFd(
  const int fd) throw (castor::exception::Exception) {
  if(m_fd != fd) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to accept connection from the admin command"
      ": Event handler passed wrong file descriptor"
      ": expected=" << m_fd << " actual=" << fd;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// logTapeConfigJobReception
//------------------------------------------------------------------------------
void
  castor::tape::tapeserver::daemon::AdminAcceptHandler::logTapeConfigJobReception(
  const legacymsg::TapeConfigRequestMsgBody &job) const throw() {
  log::Param params[] = {
    log::Param("drive", job.drive),
    log::Param("gid", job.gid),
    log::Param("uid", job.uid),
    log::Param("status", job.status)};
  m_log(LOG_INFO, "Received message from tpconfig command", params);
}

//------------------------------------------------------------------------------
// logTapeStatJobReception
//------------------------------------------------------------------------------
void
  castor::tape::tapeserver::daemon::AdminAcceptHandler::logTapeStatJobReception(
  const legacymsg::TapeStatRequestMsgBody &job) const throw() {
  log::Param params[] = {
    log::Param("gid", job.gid),
    log::Param("uid", job.uid)};
  m_log(LOG_INFO, "Received message from tpstat command", params);
}

//------------------------------------------------------------------------------
// handleTapeConfigJob
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::AdminAcceptHandler::handleTapeConfigJob(const legacymsg::TapeConfigRequestMsgBody &body) throw(castor::exception::Exception) {
  const std::string unitName(body.drive);
  const std::string dgn = m_driveCatalogue.getDgn(unitName);

  std::auto_ptr<legacymsg::VdqmProxy> vdqm(m_vdqmFactory.create());

  if(CONF_UP==body.status) {
    vdqm->setDriveUp(m_hostName, unitName, dgn);
    m_driveCatalogue.configureUp(unitName);
    m_log(LOG_INFO, "Drive is up now");
  }
  else if(CONF_DOWN==body.status) {
    vdqm->setDriveDown(m_hostName, unitName, dgn);
    m_driveCatalogue.configureDown(unitName);
    m_log(LOG_INFO, "Drive is down now");
  }
  else {
    castor::exception::Internal ex;
    ex.getMessage() << "Wrong drive status requested:" << body.status;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// handleTapeStatJob
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::AdminAcceptHandler::handleTapeStatJob(const legacymsg::TapeStatRequestMsgBody &body) throw(castor::exception::Exception) {
  
}

//------------------------------------------------------------------------------
// dispatchJob
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::AdminAcceptHandler::dispatchJob(
    const int connection) throw(castor::exception::Exception) {
  
  const legacymsg::MessageHeader header = readJobMsgHeader(connection);
  if(TPCONF == header.reqType) {
    const legacymsg::TapeConfigRequestMsgBody body = readTapeConfigMsgBody(connection, header.lenOrStatus-sizeof(header));
    logTapeConfigJobReception(body);
    handleTapeConfigJob(body);
    writeTapeRcReplyMsg(connection, 0); // 0 as return code for the tape config command, as in: "all went fine"
  }
  else { //TPSTAT
    const legacymsg::TapeStatRequestMsgBody body = readTapeStatMsgBody(connection, header.lenOrStatus-sizeof(header));
    logTapeStatJobReception(body);
    handleTapeStatJob(body);
    writeTapeStatReplyMsg(connection);
    writeTapeRcReplyMsg(connection, 0); // 0 as return code for the tape config command, as in: "all went fine"
  }
}

//------------------------------------------------------------------------------
// readJobMsgHeader
//------------------------------------------------------------------------------
castor::legacymsg::MessageHeader
  castor::tape::tapeserver::daemon::AdminAcceptHandler::readJobMsgHeader(
    const int connection) throw(castor::exception::Exception) {
  // Read in the message header
  char buf[3 * sizeof(uint32_t)]; // magic + request type + len
  io::readBytes(connection, m_netTimeout, sizeof(buf), buf);

  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::MessageHeader header;
  memset(&header, '\0', sizeof(header));
  legacymsg::unmarshal(bufPtr, bufLen, header);

  if(TPMAGIC != header.magic) {
    castor::exception::Internal ex;
    ex.getMessage() << "Invalid admin job message: Invalid magic"
      ": expected=0x" << std::hex << TPMAGIC << " actual=0x" <<
      header.magic;
    throw ex;
  }

  if(TPCONF != header.reqType and TPSTAT != header.reqType) {
    castor::exception::Internal ex;
    ex.getMessage() << "Invalid admin job message: Invalid request type"
       ": expected= 0x" << std::hex << TPCONF << " or 0x" << TPSTAT << " actual=0x" <<
       header.reqType;
    throw ex;
  }

  // The length of the message body is checked later, just before it is read in
  // to memory

  return header;
}

//------------------------------------------------------------------------------
// readTapeConfigMsgBody
//------------------------------------------------------------------------------
castor::legacymsg::TapeConfigRequestMsgBody
  castor::tape::tapeserver::daemon::AdminAcceptHandler::readTapeConfigMsgBody(
    const int connection, const uint32_t len)
    throw(castor::exception::Exception) {
  char buf[REQBUFSZ];

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

  legacymsg::TapeConfigRequestMsgBody body;
  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::unmarshal(bufPtr, bufLen, body);
  return body;
}

//------------------------------------------------------------------------------
// readTapeStatMsgBody
//------------------------------------------------------------------------------
castor::legacymsg::TapeStatRequestMsgBody
  castor::tape::tapeserver::daemon::AdminAcceptHandler::readTapeStatMsgBody(
    const int connection, const uint32_t len)
    throw(castor::exception::Exception) {
  char buf[REQBUFSZ];

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

  legacymsg::TapeStatRequestMsgBody body;
  const char *bufPtr = buf;
  size_t bufLen = sizeof(buf);
  legacymsg::unmarshal(bufPtr, bufLen, body);
  return body;
}

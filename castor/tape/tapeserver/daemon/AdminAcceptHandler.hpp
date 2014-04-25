/******************************************************************************
 *                castor/tape/tapeserver/daemon/AdminAcceptHandler.hpp
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

#pragma once

#include "castor/io/PollEventHandler.hpp"
#include "castor/io/PollReactor.hpp"
#include "castor/log/Logger.hpp"
#include "castor/tape/tapeserver/daemon/DriveCatalogue.hpp"
#include "castor/legacymsg/MessageHeader.hpp"
#include "castor/legacymsg/TapeConfigRequestMsgBody.hpp"
#include "castor/legacymsg/TapeStatRequestMsgBody.hpp"
#include "castor/legacymsg/TapeStatReplyMsgBody.hpp"
#include "castor/legacymsg/VdqmProxyFactory.hpp"

#include <poll.h>

namespace castor     {
namespace tape       {
namespace tapeserver {
namespace daemon     {

/**
 * Handles the events of the socket listening for connection from the admin
 * commands.
 */
class AdminAcceptHandler: public io::PollEventHandler {
public:

  /**
   * Constructor.
   *
   * @param fd The file descriptor of the socket listening for
   * connections from the vdqmd daemon.
   * @param reactor The reactor to which new Vdqm connection handlers are to
   * be registered.
   * @param log The object representing the API of the CASTOR logging system.
   * @param vdqmFactory Factory to create proxy objects representing the vdqmd
   * daemon.
   * @param driveCatalogue The catalogue of tape drives controlled by the tape
   * server daemon.
   */
  AdminAcceptHandler(
    const int fd,
    io::PollReactor &reactor,
    log::Logger &log,
    legacymsg::VdqmProxyFactory &vdqmFactory,
    DriveCatalogue &driveCatalogue,
    const std::string &hostName) throw();

  /**
   * Returns the integer file descriptor of this event handler.
   */
  int getFd() throw();

  /**
   * Fills the specified poll file-descriptor ready to be used in a call to
   * poll().
   */
  void fillPollFd(struct pollfd &fd) throw();

  /**
   * Handles the specified event.
   *
   * @param fd The poll file-descriptor describing the event.
   * @return true if the event handler should be removed from and deleted by
   * the reactor.
   */
  bool handleEvent(const struct pollfd &fd)
    throw(castor::exception::Exception);

  /**
   * Destructor.
   */
  ~AdminAcceptHandler() throw();

private:
  
  /**
   * Marshals the specified source tape config reply message structure into the
   * specified destination buffer.
   *
   * @param dst    The destination buffer.
   * @param dstLen The length of the destination buffer.
   * @param rc     The return code to reply.
   * @return       The total length of the header.
   */
  size_t marshalTapeRcReplyMsg(char *const dst, const size_t dstLen,
    const int rc) throw(castor::exception::Exception);
  
  /**
   * Writes a job reply message to the tape config command connection.
   *
   * @param fd The file descriptor of the connection with the admin command.
   * @param rc The return code to reply.
   * 
   */
  void writeTapeRcReplyMsg(const int fd, const int rc)
    throw(castor::exception::Exception);
  
  /**
   * Marshals the specified source tape stat reply message structure into the
   * specified destination buffer.
   *
   * @param dst    The destination buffer.
   * @param dstLen The length of the destination buffer.
   * @param rc     The return code to reply.
   * @return       The total length of the header.
   */
  size_t marshalTapeStatReplyMsg(char *const dst, const size_t dstLen,
    const legacymsg::TapeStatReplyMsgBody &body) throw(castor::exception::Exception);
  
  /**
   * Writes a reply message to the tape stat command connection.
   *
   * @param fd The file descriptor of the connection with the admin command.
   * @param rc The return code to reply.
   * 
   */
  void writeTapeStatReplyMsg(const int fd)
    throw(castor::exception::Exception);

  /**
   * Throws an exception if the specified file-descriptor is not that of the
   * socket listening for connections from the vdqmd daemon.
   */
  void checkHandleEventFd(const int fd) throw (castor::exception::Exception);
  
  /**
   * Logs the reception of the specified job message from the tpconfig command.
   */
  void logTapeConfigJobReception(const legacymsg::TapeConfigRequestMsgBody &job)
    const throw();
  
  /**
   * Logs the reception of the specified job message from the tpstat command.
   */
  void logTapeStatJobReception(const legacymsg::TapeStatRequestMsgBody &job)
    const throw();
  /**
   * Replies to the client the status of all registered drives
   * 
   * @param body body of the tape stat message
   */
  void handleTapeStatJob(const legacymsg::TapeStatRequestMsgBody &body) 
    throw(castor::exception::Exception);
  
  /**
   * Carries out the required drive configuration and replies properly to the client
   * 
   * @param body body of the tape config message
   */
  void handleTapeConfigJob(const legacymsg::TapeConfigRequestMsgBody &body) 
    throw(castor::exception::Exception);
  
  /**
   * Reads a job type from the specified connection, dispatches the job to the 
   * correct method, and then closes the connection.
   *
   * @param connection The file descriptor of the connection with admin command
   */
  void dispatchJob(const int connection) throw(castor::exception::Exception);
  
  /**
   * Reads the header of a job message from the specified connection.
   *
   * @param connection The file descriptor of the connection with the vdqm
   * daemon.
   * @return The message header.
   */
  legacymsg::MessageHeader readJobMsgHeader(const int connection)
    throw(castor::exception::Exception);
  
  /**
   * Reads the body of a job message from the specified connection.
   *
   * @param connection The file descriptor of the connection with the command.
   * @param len The length of the message body in bytes.
   * @return The message body.
   */
  legacymsg::TapeConfigRequestMsgBody readTapeConfigMsgBody(const int connection,
    const uint32_t len) throw(castor::exception::Exception);
  
  /**
   * Reads the body of a job message from the specified connection.
   *
   * @param connection The file descriptor of the connection with the command.
   * @param len The length of the message body in bytes.
   * @return The message body.
   */
  legacymsg::TapeStatRequestMsgBody readTapeStatMsgBody(const int connection,
    const uint32_t len) throw(castor::exception::Exception);

  /**
   * The file descriptor of the socket listening for connections from the vdqmd
   * daemon.
   */
  const int m_fd;

  /**
   * The reactor to which new Vdqm connection handlers are to be registered.
   */
  io::PollReactor &m_reactor;

  /**
   * The object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

  /**
   * Factory to create proxy objects representing the vdqmd daemon.
   */
  legacymsg::VdqmProxyFactory &m_vdqmFactory;

  /**
   * The catalogue of tape drives controlled by the tape server daemon.
   */
  DriveCatalogue &m_driveCatalogue;
  
  /**
   * The name of the host on which tape daemon is running.  This name is
   * needed to fill in messages to be sent to the vdqmd daemon.
   */
  const std::string m_hostName;
  
  /**
   * The timeout in seconds to be applied when performing network read and
   * write operations.
   */
  const int m_netTimeout;

}; // class VdqmAcceptHandler

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

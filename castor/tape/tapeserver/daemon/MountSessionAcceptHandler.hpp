/******************************************************************************
 *                castor/tape/tapeserver/daemon/MountSessionAcceptHandler.hpp
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
#include "castor/io/ZMQReactor.hpp"
#include "castor/log/Logger.hpp"
#include "castor/tape/tapeserver/daemon/DriveCatalogue.hpp"
#include "castor/legacymsg/MessageHeader.hpp"
#include "castor/legacymsg/TapeUpdateDriveRqstMsgBody.hpp"
#include "castor/legacymsg/VdqmProxy.hpp"
#include "castor/legacymsg/VmgrProxy.hpp"

namespace castor     {
namespace tape       {
namespace tapeserver {
namespace daemon     {

/**
 * Handles the events of the socket listening for connection from the mount session
 */
class MountSessionAcceptHandler: public io::ZMQPollEventHandler {
public:

  /**
   * Constructor.
   *
   * @param fd The file descriptor of the socket listening for
   * connections from the mount session
   * @param reactor The reactor to which new connection handlers are to be
   * registered.
   * @param log The object representing the API of the CASTOR logging system.
   * @param driveCatalogue The catalogue of tape drives controlled by the tape
   * server daemon.
   */
  MountSessionAcceptHandler(const int fd, io::ZMQReactor &reactor,
    log::Logger &log, DriveCatalogue &driveCatalogue, const std::string &hostName,
    castor::legacymsg::VdqmProxy & vdqm,
    castor::legacymsg::VmgrProxy & vmgr) throw();

  /**
   * Returns the integer file descriptor of this event handler.
   */
  int getFd() throw();

  /**
   * Fills the specified poll file-descriptor ready to be used in a call to
   * poll().
   */
  void fillPollFd(zmq::pollitem_t &fd) throw();

  /**
   * Handles the specified event.
   *
   * @param fd The poll file-descriptor describing the event.
   * @return true if the event handler should be removed from and deleted by
   * the reactor.
   */
  bool handleEvent(const zmq::pollitem_t &fd);

  /**
   * Destructor.
   */
  ~MountSessionAcceptHandler() throw();

private:

  /**
   * Logs the specifed IO event of the mount-session listen-socket.
   */
  void logMountSessionAcceptEvent(const zmq::pollitem_t &fd);
  
  /**
   * Throws an exception if the specified file-descriptor does not match the
   * file-descriptor of this event handler.
   *
   * @param fd The file descriptor to be checked.
   */
  void checkHandleEventFd(const int fd);
  
  /**
   * The file descriptor of the socket listening for mount-session
   * connections.
   */
  const int m_fd;

  /**
   * The reactor to which new Vdqm connection handlers are to be registered.
   */
  io::ZMQReactor &m_reactor;

  /**
   * The object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

  /**
   * The catalogue of tape drives controlled by the tape server daemon.
   */
  DriveCatalogue &m_driveCatalogue;
  
  /**
   * The name of the host on which tape daemon is running.
   */
  const std::string m_hostName;
  
  /** 
   * Reference to the VdqmProxy, allowing reporting of the drive status. It
   * will be used by the StatusReporter 
   */
  castor::legacymsg::VdqmProxy & m_vdqm;

  /** 
   * Reference to the VmgrProxy, allowing reporting and checking tape status.
   * It is also used by the StatusReporter 
   */
  castor::legacymsg::VmgrProxy & m_vmgr;
  
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

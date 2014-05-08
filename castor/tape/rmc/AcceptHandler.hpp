/******************************************************************************
 *                castor/tape/rmc/AcceptHandler.hpp
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

#pragma once

#include "castor/io/PollEventHandler.hpp"
#include "castor/io/PollReactor.hpp"
#include "castor/log/Logger.hpp"

#include <poll.h>

namespace castor {
namespace tape {
namespace rmc {

/**
 * Handles the events of the socket listening for connections from clients of
 * the rmcd daemon.
 */
class AcceptHandler: public io::PollEventHandler {
public:

  /**
   * Constructor.
   *
   * @param fd The file descriptor of the socket listening for connections
   * from clients of the rmcd daemon.
   * @param reactor The reactor to which new connection handlers are to be
   * registered.
   * @param log The object representing the API of the CASTOR logging system.
   */
  AcceptHandler(const int fd, io::PollReactor &reactor, log::Logger &log) throw();

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
   *
   * Closes the listen socket.
   */
  ~AcceptHandler() throw();

private:

  /**
   * Throws an exception if the specified file-descriptor is not that of the
   * socket listening for client connections.
   */
  void checkHandleEventFd(const int fd) throw (castor::exception::Exception);

  /**
   * The file descriptor of the socket listening for client connections.
   */
  const int m_fd;

  /**
   * The reactor to which new connection handlers are to be registered.
   */
  io::PollReactor &m_reactor;

  /**
   * The object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

}; // class AcceptHandler

} // namespace rmc
} // namespace tape
} // namespace castor


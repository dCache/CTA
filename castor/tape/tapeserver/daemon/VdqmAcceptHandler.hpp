/******************************************************************************
 *                castor/tape/tapeserver/daemon/VdqmAcceptHandler.hpp
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
#include "castor/tape/tapeserver/daemon/DriveCatalogue.hpp"
#include "castor/tape/tapeserver/daemon/Vdqm.hpp"

#include <poll.h>

namespace castor     {
namespace tape       {
namespace tapeserver {
namespace daemon     {

/**
 * Handles the events of the socket listening for connection from the vdqmd
 * daemon.
 */
class VdqmAcceptHandler: public io::PollEventHandler {
public:

  /**
   * Constructor.
   *
   * @param listenSock The file descriptor of the socket listening for
   * connections from the vdqmd daemon.
   * @param reactor The reactor to which new Vdqm connection handlers are to be
   * registered.
   * @param log The object representing the API of the CASTOR logging system.
   * @param vdqm The object representing the vdqmd daemon.
   * @param driveCatalogue The catalogue of tape drives controlled by the tape
   * server daemon.
   */
  VdqmAcceptHandler(const int listenSock, io::PollReactor &reactor,
    log::Logger &log, Vdqm &vdqm, DriveCatalogue &driveCatalogue) throw();

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
   */
  void handleEvent(const struct pollfd &fd)
    throw(castor::exception::Exception);

  /**
   * Destructor.
   *
   * Closes the listen socket.
   */
  ~VdqmAcceptHandler() throw();

private:

  /**
   * Throws an exception if the specified file-descriptor is not that of the
   * socket listening for connections from the vdqmd daemon.
   */
  void checkHandleEventFd(const int fd) throw (castor::exception::Exception);

  /**
   * The file descriptor of the socket listening for connections from the vdqmd
   * daemon.
   */
  const int m_listenSock;

  /**
   * The reactor to which new Vdqm connection handlers are to be registered.
   */
  io::PollReactor &m_reactor;

  /**
   * The object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

  /**
   * The object representing the vdqmd daemon.
   */
  Vdqm &m_vdqm;

  /**
   * The catalogue of tape drives controlled by the tape server daemon.
   */
  DriveCatalogue &m_driveCatalogue;

}; // class VdqmAcceptHandler

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor


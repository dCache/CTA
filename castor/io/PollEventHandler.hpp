/******************************************************************************
 *                castor/io/PollEventHandler.hpp
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

#include "castor/exception/Exception.hpp"

#include <poll.h>

namespace castor {
namespace io {

/**
 * Handles the events that occur on a poll() file descriptor.
 *
 * This class is part of an implementation of the Reactor architecture pattern
 * described in the following book:
 *
 *    Pattern-Oriented Software Architecture Volume 2
 *    Patterns for Concurrent and Networked Objects
 *    Authors: Schmidt, Stal, Rohnert and Buschmann
 *    Publication date: 2000
 *    ISBN 0-471-60695-2
 */
class PollEventHandler {
public:

  /**
   * Returns the integer file descriptor of this event handler.
   */
  virtual int getFd() throw() = 0;

  /**
   * Fills the specified poll file-descriptor ready to be used in a call to
   * poll().
   */
  virtual void fillPollFd(struct pollfd &fd) throw() = 0;

  /**
   * Handles the specified event.
   *
   * @param fd The poll file-descriptor describing the event.
   */
  virtual void handleEvent(const struct pollfd &fd)
    throw(castor::exception::Exception) = 0;

  /**
   * Destructor.
   */
  virtual ~PollEventHandler() throw() = 0;

}; // class PollEventHandler

} // namespace io
} // namespace castor


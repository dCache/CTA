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

#pragma once

#include "common/exception/Exception.hpp"
#include "tapeserver/castor/log/Param.hpp"
#include "tapeserver/session/SessionState.hpp"
#include "tapeserver/session/SessionType.hpp"

#include <stdint.h>
#include <string>
#include <list>

namespace cta { namespace tape { namespace daemon {

/**
 * Abstract class defining the interface to a proxy object representing the
 * possible notifications sent back to main tape daemon (taped)
 */
class TapedProxy {
public:

  /**
   * Destructor.
   */
  virtual ~TapedProxy()  = 0;
  
  /**
   * Notifies taped of a state change. Taped will validate the transition and
   * kill the process if it is an unexpected transition.
   * 
   * @param state the new state.
   * @param type the type of the session (archive, retrieve, verify, 
   * @param vid the vid of the tape involved
   */
  virtual void reportState(const cta::tape::session::SessionState state,
    const cta::tape::session::SessionType type, 
    const std::string & vid) = 0;
  
  /**
   * Report a heartbeat to taped. The data counters might or might not have changed
   * as the sending of the heartbeat itself is an information.
   * 
   * @param totalTapeBytesMoved cumulated data transfered to/from tape during the session.
   * @param totalDiskBytesMoved cumulated data transfered to/from disk during the session.
   */
  virtual void reportHeartbeat(uint64_t totalTapeBytesMoved, uint64_t totalDiskBytesMoved) = 0;
  
  /**
   * Sends a new set of parameters, to be logged by the mother process when the
   * transfer session is over.
   * @param params: a vector of log parameters
   */
  virtual void addLogParams(const std::string &unitName,
    const std::list<castor::log::Param> & params) = 0;
  
  /**
   * Sends a list of parameters to remove from the end of session logging.
   */
  virtual void deleteLogParams(const std::string &unitName,
    const std::list<std::string> & paramNames) = 0;

  /**
   * Notifies the tapeserverd daemon that a label session has encountered the
   * specified error.
   *
   * @param unitName The unit name of the tape drive.
   * @param message The error message.
   */
  virtual void labelError(const std::string &unitName,
    const std::string &message) = 0;

}; // class TapeserverProxy

}}} // namespace cta::tape::daemon


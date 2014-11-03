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
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include "castor/log/Logger.hpp"
#include "castor/tape/utils/DriveConfig.hpp"

#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {

/**
 * Abstract base class defining the common interface of a catalogue
 * tape-session.
 */
class CatalogueSession {
public:
    
  /**
   * Destructor.
   */
  virtual ~CatalogueSession() = 0;

  /**
   * enumeration of all the possible types of tape session.
   */
  enum Type {
    SESSION_TYPE_CLEANER,
    SESSION_TYPE_TRANSFER,
    SESSION_TYPE_LABEL};

  /**
   * always returns a string representation of the specified session type.
   * if the session type is unknown then an appropriately worded string
   * representation is returned and no exception is thrown.
   *
   * @param sessiontype the numerical sessiontype.
   * @return the string representation if known else "unknown".
   */
  static const char *sessionTypeToStr(const Type sessionType) throw();

  /**
   * Returns the type of this tape session.
   */
  Type getType() const throw();

  /** 
   * Notifies the catalogue session that it should perform any time related
   * actions such as implementing alarms. 
   *
   * This method does not have to be called at any time precise interval,
   * though it should be called at least twice as fast as the quickest reaction
   * time imposed on the catalogue session.
   */
  virtual void tick() = 0;

  /**
   * To be called when the session has ended with success.
   */
  virtual void sessionSucceeded() = 0;

  /**
   * To be called when the session has ended with failure.
   */
  virtual void sessionFailed() = 0;

  /**
   * Gets the volume identifier of the tape associated with the tape drive.
   *
   * @return The volume identifier of the tape associated with the tape drive.
   */
  virtual std::string getVid() const = 0;

  /**
   * Gets the access mode, either recall (WRITE_DISABLE) or migration
   * (WRITE_ENABLE).
   *
   * @return The access mode.
   */
  virtual int getMode() const = 0;

  /**
   * Gets the process identifier of the session.
   * 
   * @return The process identifier of the session.
   */
  virtual pid_t getPid() const throw() = 0;
  
  /**
   * Gets the time at which a job was assigned to the tape drive.
   *
   * @return The time at which a job was assigned to the tape drive.
   */
  virtual time_t getAssignmentTime() const throw() = 0;

  /**
   * Returns true if a tape is in the process of being mounted.
   *
   * @return True if a tape is in the process of being mounted.
   */
  virtual bool tapeIsBeingMounted() const throw() = 0;

protected:

  /**
   * Protected constructor.
   *
   * Except in the case of unit testing a CatalogueTransferSession object
   * should only be created using the static create() method.  This constructor
   * is protected so that unit tests can go around this restriction for sole
   * purpose of unit testing.
   * 
   * @param sessionType The type of the tape session.
   * @param log Object representing the API of the CASTOR logging system.
   * @param netTimeout Timeout in seconds to be used when performing network
   * I/O.
   * @param pid The process identifier of the session.
   * @param driveConfig The configuration of the tape drive.
   */
  CatalogueSession(
    const Type sessionType,
    log::Logger &log,
    const int netTimeout,
    const pid_t pid,
    const tape::utils::DriveConfig &driveConfig) throw();

  /**
   * The type of the tape session.
   */
  const Type m_sessionType;

  /**
   * Object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

  /**
   * Timeout in seconds to be used when performing network I/O.
   */
  const int m_netTimeout;

  /**
   * The process identifier of the session.
   */
  const pid_t m_pid;

  /**
   * The configuration of the tape drive.
   */
  const tape::utils::DriveConfig &m_driveConfig;

}; // class CatalogueSession

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

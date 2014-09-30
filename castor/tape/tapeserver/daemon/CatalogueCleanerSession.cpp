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

#include "castor/exception/Exception.hpp"
#include "castor/tape/tapeserver/daemon/CatalogueCleanerSession.hpp"
#include "h/Ctape_constants.h"

//------------------------------------------------------------------------------
// create
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::CatalogueCleanerSession *
  castor::tape::tapeserver::daemon::CatalogueCleanerSession::create(
  log::Logger &log,
  const int netTimeout,
  const tape::utils::DriveConfig &driveConfig,
  ProcessForkerProxy &processForker,
  const std::string &vid,
  const unsigned short rmcPort,
  const time_t assignmentTime) {

  const pid_t pid = processForker.forkCleaner(driveConfig, vid, rmcPort);

  return new CatalogueCleanerSession(
    log,
    netTimeout,
    pid,
    driveConfig,
    vid,
    assignmentTime);
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::CatalogueCleanerSession::
  CatalogueCleanerSession(
  log::Logger &log,
  const int netTimeout,
  const pid_t pid,
  const tape::utils::DriveConfig &driveConfig,
  const std::string &vid,
  const time_t assignmentTime) throw():
  CatalogueSession(log, netTimeout, pid, driveConfig),
  m_vid(vid),
  m_assignmentTime(assignmentTime) {
}

//------------------------------------------------------------------------------
// tick
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueCleanerSession::tick() {
}

//------------------------------------------------------------------------------
// sessionSucceeded
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueCleanerSession::
  sessionSucceeded() {
}

//------------------------------------------------------------------------------
// sessionFailed
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::CatalogueCleanerSession::
  sessionFailed() {
}

//------------------------------------------------------------------------------
// getVid
//------------------------------------------------------------------------------
std::string castor::tape::tapeserver::daemon::CatalogueCleanerSession::
  getVid() const {
  // If the volume identifier of the tape drive is not known
  if(m_vid.empty()) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to get VID from cleaner session"
      ": VID not known";
    throw ex;
  }

  return m_vid;
}

//------------------------------------------------------------------------------
// getMode
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::CatalogueCleanerSession::getMode()
  const throw() {
  return WRITE_DISABLE;
}

//-----------------------------------------------------------------------------
// getPid
//-----------------------------------------------------------------------------
pid_t castor::tape::tapeserver::daemon::CatalogueCleanerSession::
  getPid() const throw() {
  return m_pid;
}

//------------------------------------------------------------------------------
// getAssignmentTime
//------------------------------------------------------------------------------
time_t castor::tape::tapeserver::daemon::CatalogueCleanerSession::
  getAssignmentTime() const throw() {
  return m_assignmentTime;
}

//-----------------------------------------------------------------------------
// tapeIsBeingMounted
//-----------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::CatalogueCleanerSession::
  tapeIsBeingMounted() const throw() {
  return false;
}

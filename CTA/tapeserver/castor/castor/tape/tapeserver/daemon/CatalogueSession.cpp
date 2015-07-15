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

#include "castor/tape/tapeserver/daemon/CatalogueSession.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::CatalogueSession::
  CatalogueSession(
  const Type sessionType,
  log::Logger &log,
  const int netTimeout,
  const pid_t pid,
  const DriveConfig &driveConfig) throw():
  m_sessionType(sessionType),
  m_log(log),
  m_netTimeout(netTimeout),
  m_pid(pid),
  m_driveConfig(driveConfig) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::CatalogueSession::
  ~CatalogueSession() {
}

//-----------------------------------------------------------------------------
// sessionTypeToStr
//-----------------------------------------------------------------------------
const char
   *castor::tape::tapeserver::daemon::CatalogueSession::sessionTypeToStr(
  const Type sessionType) throw() {
  switch(sessionType) {
  case SESSION_TYPE_CLEANER : return "CLEANER";
  case SESSION_TYPE_TRANSFER: return "TRANSFER";
  case SESSION_TYPE_LABEL   : return "LABEL";
  default                   : return "UNKNOWN";
  }
}

//-----------------------------------------------------------------------------
// getType
//-----------------------------------------------------------------------------
castor::tape::tapeserver::daemon::CatalogueSession::Type
  castor::tape::tapeserver::daemon::CatalogueSession::getType()
  const throw() {
  return m_sessionType;
}

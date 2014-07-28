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

#include "castor/messages/TapeserverProxyDummy.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::messages::TapeserverProxyDummy::TapeserverProxyDummy() throw() {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::messages::TapeserverProxyDummy::~TapeserverProxyDummy() throw() {
}

//------------------------------------------------------------------------------
// gotRecallJobFromTapeGateway
//------------------------------------------------------------------------------
void castor::messages::TapeserverProxyDummy::gotRecallJobFromTapeGateway(
  const std::string &vid, const std::string &unitName) {
}

//------------------------------------------------------------------------------
// gotRecallJobFromReadTp
//------------------------------------------------------------------------------
void castor::messages::TapeserverProxyDummy::gotRecallJobFromReadTp(
  const std::string &vid, const std::string &unitName) {
}

//------------------------------------------------------------------------------
// gotMigrationJobFromTapeGateway
//------------------------------------------------------------------------------
uint32_t castor::messages::TapeserverProxyDummy::gotMigrationJobFromTapeGateway(
  const std::string &vid, const std::string &unitName) {
  return 0;
}

//------------------------------------------------------------------------------
// gotMigrationJobFromWriteTp
//------------------------------------------------------------------------------
uint32_t castor::messages::TapeserverProxyDummy::gotMigrationJobFromWriteTp(
  const std::string &vid, const std::string &unitName) {
  return 0;
}

//------------------------------------------------------------------------------
// tapeMountedForRecall
//------------------------------------------------------------------------------
void castor::messages::TapeserverProxyDummy::tapeMountedForRecall(
  const std::string &vid, const std::string &unitName) {
}

//------------------------------------------------------------------------------
// tapeMountedForMigration
//------------------------------------------------------------------------------
void castor::messages::TapeserverProxyDummy::tapeMountedForMigration(
  const std::string &vid, const std::string &unitName) {
}

//------------------------------------------------------------------------------
// tapeUnmountStarted
//------------------------------------------------------------------------------
void castor::messages::TapeserverProxyDummy::tapeUnmountStarted(
  const std::string &vid, const std::string &unitName) {
} 

//------------------------------------------------------------------------------
// tapeUnmounted
//------------------------------------------------------------------------------
void castor::messages::TapeserverProxyDummy::tapeUnmounted(
  const std::string &vid, const std::string &unitName) {
} 

//------------------------------------------------------------------------------
// tapeUnmounted
//------------------------------------------------------------------------------
void castor::messages::TapeserverProxyDummy::
notifyHeartbeat(uint64_t) {
} 

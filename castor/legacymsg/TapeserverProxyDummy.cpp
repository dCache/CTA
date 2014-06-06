/******************************************************************************
 *         castor/legacymsg/TapeserverProxyDummy.cpp
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

#include "castor/legacymsg/TapeserverProxyDummy.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::legacymsg::TapeserverProxyDummy::TapeserverProxyDummy() throw() {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::legacymsg::TapeserverProxyDummy::~TapeserverProxyDummy() throw() {
}

//------------------------------------------------------------------------------
// gotReadMountDetailsFromClient
//------------------------------------------------------------------------------
void castor::legacymsg::TapeserverProxyDummy::gotReadMountDetailsFromClient(
  castor::tape::tapeserver::client::ClientProxy::VolumeInfo volInfo,
  const std::string &unitName) {
}

//------------------------------------------------------------------------------
// gotWriteMountDetailsFromClient
//------------------------------------------------------------------------------
uint64_t
  castor::legacymsg::TapeserverProxyDummy::gotWriteMountDetailsFromClient(
  castor::tape::tapeserver::client::ClientProxy::VolumeInfo volInfo,
  const std::string &unitName) {

  return 0;  // Always return 0 files on tape
}

//------------------------------------------------------------------------------
// gotDumpMountDetailsFromClient
//------------------------------------------------------------------------------
void castor::legacymsg::TapeserverProxyDummy::gotDumpMountDetailsFromClient(
  castor::tape::tapeserver::client::ClientProxy::VolumeInfo volInfo,
  const std::string &unitName) {
}

//------------------------------------------------------------------------------
// tapeMountedForRead
//------------------------------------------------------------------------------
void castor::legacymsg::TapeserverProxyDummy::tapeMountedForRead(
  castor::tape::tapeserver::client::ClientProxy::VolumeInfo volInfo,
  const std::string &unitName) {
}

//------------------------------------------------------------------------------
// tapeMountedForWrite
//------------------------------------------------------------------------------
void castor::legacymsg::TapeserverProxyDummy::tapeMountedForWrite(
  castor::tape::tapeserver::client::ClientProxy::VolumeInfo volInfo,
  const std::string &unitName) {
}

//------------------------------------------------------------------------------
// tapeUnmounting
//------------------------------------------------------------------------------
void castor::legacymsg::TapeserverProxyDummy::tapeUnmounting(
  castor::tape::tapeserver::client::ClientProxy::VolumeInfo volInfo,
  const std::string &unitName) {
} 

//------------------------------------------------------------------------------
// tapeUnmounted
//------------------------------------------------------------------------------
void castor::legacymsg::TapeserverProxyDummy::tapeUnmounted(
  castor::tape::tapeserver::client::ClientProxy::VolumeInfo volInfo,
  const std::string &unitName) {
} 

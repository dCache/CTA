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
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "mediachanger/messages.hpp"
#include "mediachanger/ZmqMsg.hpp"

#include <errno.h>
#include <unistd.h>

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
cta::mediachanger::ZmqMsg::ZmqMsg() throw() {
  if(zmq_msg_init(&m_zmqMsg)) {
    const int savedErrno = errno;
    cta::exception::Exception ex;
    ex.getMessage() << "zmq_msg_init() failed: " << zmqErrnoToStr(savedErrno);
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
cta::mediachanger::ZmqMsg::ZmqMsg(const size_t msgSize) throw() {
  if(zmq_msg_init_size(&m_zmqMsg, msgSize)) {
    const int savedErrno = errno;
    cta::exception::Exception ex;
    ex.getMessage() << "zmq_msg_init_size() failed: " <<
      zmqErrnoToStr(savedErrno);
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
cta::mediachanger::ZmqMsg::~ZmqMsg() throw() {
  zmq_msg_close(&m_zmqMsg);
}

//-----------------------------------------------------------------------------
// getZmqMsg
//-----------------------------------------------------------------------------
zmq_msg_t &cta::mediachanger::ZmqMsg::getZmqMsg() throw() {
  return m_zmqMsg;
}

//-----------------------------------------------------------------------------
// getData
//-----------------------------------------------------------------------------
const void* cta::mediachanger::ZmqMsg::getData() const {
  return zmq_msg_data(const_cast<zmq_msg_t*>(&m_zmqMsg));
}

//-----------------------------------------------------------------------------
// getData
//-----------------------------------------------------------------------------
void* cta::mediachanger::ZmqMsg::getData() {
  return zmq_msg_data(&m_zmqMsg);
}

//-----------------------------------------------------------------------------
// size
//-----------------------------------------------------------------------------
size_t cta::mediachanger::ZmqMsg::size() const {
  return zmq_msg_size(const_cast<zmq_msg_t*>(&m_zmqMsg));
}

//-----------------------------------------------------------------------------
// more
//-----------------------------------------------------------------------------
bool cta::mediachanger::ZmqMsg::more() const {
  return zmq_msg_more(const_cast<zmq_msg_t*>(&m_zmqMsg));
}

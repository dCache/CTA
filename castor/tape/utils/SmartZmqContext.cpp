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

#include "castor/tape/utils/SmartZmqContext.hpp"

#include <errno.h>
#include <unistd.h>
#include <zmq.h>

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
castor::tape::utils::SmartZmqContext::SmartZmqContext() throw() :
  m_zmqContext(NULL) {
}

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
castor::tape::utils::SmartZmqContext::SmartZmqContext(void *const zmqContext)
  throw() : m_zmqContext(zmqContext) {
}

//-----------------------------------------------------------------------------
// reset
//-----------------------------------------------------------------------------
void castor::tape::utils::SmartZmqContext::reset(void *const zmqContext)
  throw() {
  // If the new ZMQ context is not the one already owned
  if(zmqContext != m_zmqContext) {

    // If this smart pointer still owns a ZMQ context, then terminate it
    if(m_zmqContext != NULL) {
      zmq_term(m_zmqContext);
    }

    // Take ownership of the new ZMQ context
    m_zmqContext = zmqContext;
  }
}

//-----------------------------------------------------------------------------
// SmartZmqContext assignment operator
//-----------------------------------------------------------------------------
castor::tape::utils::SmartZmqContext
  &castor::tape::utils::SmartZmqContext::operator=(SmartZmqContext& obj) {
  reset(obj.release());
  return *this;
}

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
castor::tape::utils::SmartZmqContext::~SmartZmqContext() throw() {
  reset();
}

//-----------------------------------------------------------------------------
// get
//-----------------------------------------------------------------------------
void *castor::tape::utils::SmartZmqContext::get() const throw() {
  return m_zmqContext;
}

//-----------------------------------------------------------------------------
// release
//-----------------------------------------------------------------------------
void *castor::tape::utils::SmartZmqContext::release() {
  // If this smart pointer does not own a ZMQ context
  if(NULL == m_zmqContext) {
    castor::exception::NotAnOwner ex;
    ex.getMessage() << "Smart pointer does not own a ZMQ context";
    throw ex;
  }

  void *const tmp = m_zmqContext;

  // A NULL value indicates this smart pointer does not own a ZMQ context
  m_zmqContext = NULL;

  return tmp;
}

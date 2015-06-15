/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common/exception/Exception.hpp"
#include "common/SmartFd.hpp"

#include <unistd.h>

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
cta::SmartFd::SmartFd() throw():
  m_fd(-1), m_closedCallback(NULL) {
}

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
cta::SmartFd::SmartFd(const int fd) throw():
  m_fd(fd), m_closedCallback(NULL) {
}

//-----------------------------------------------------------------------------
// setClosedCallback
//-----------------------------------------------------------------------------
void cta::SmartFd::setClosedCallback(ClosedCallback closedCallback)
  throw() {
  m_closedCallback = closedCallback;
}

//-----------------------------------------------------------------------------
// reset
//-----------------------------------------------------------------------------
void cta::SmartFd::reset(const int fd) throw() {
  // If the new file descriptor is not the one already owned
  if(fd != m_fd) {

    // If this SmartFd still owns a file descriptor, then close it
    if(m_fd >= 0) {
      close(m_fd);
      if(m_closedCallback) {
        try {
           (*m_closedCallback)(m_fd);
        } catch(...) {
           // Ignore any exception thrown my the m_closedCallback function
           // because this reset function maybe called by the destructor of
           // SmartFd
        }
      }
    }

    // Take ownership of the new file descriptor
    m_fd = fd;
  }
}

//-----------------------------------------------------------------------------
// SmartFd assignment operator
//-----------------------------------------------------------------------------
cta::SmartFd &cta::SmartFd::operator=(SmartFd& obj) {
  reset(obj.release());
  return *this;
}

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
cta::SmartFd::~SmartFd() {
  reset();
}

//-----------------------------------------------------------------------------
// get
//-----------------------------------------------------------------------------
int cta::SmartFd::get() const throw() {
  return m_fd;
}

//-----------------------------------------------------------------------------
// release
//-----------------------------------------------------------------------------
int cta::SmartFd::release()  {
  // If this SmartFd does not own a file descriptor
  if(m_fd < 0) {
    throw exception::Exception("Smart file-descriptor does not own a"
      " file-descriptor");
  }

  const int tmpFd = m_fd;

  // A negative number indicates this SmartFd does not own a file descriptor
  m_fd = -1;

  return tmpFd;
}

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

#include "castor/utils/SmartFILEPtr.hpp"

#include <errno.h>
#include <unistd.h>

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
castor::utils::SmartFILEPtr::SmartFILEPtr() throw() :
  m_file(NULL) {
}

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
castor::utils::SmartFILEPtr::SmartFILEPtr(FILE *const file) throw() :
  m_file(file) {
}

//-----------------------------------------------------------------------------
// reset
//-----------------------------------------------------------------------------
void castor::utils::SmartFILEPtr::reset(FILE *const file = NULL)
   throw() {
  // If the new pointer is not the one already owned
  if(file != m_file) {

    // If this smart pointer still owns a pointer, then fclose it
    if(m_file != NULL) {
      fclose(m_file);
    }

    // Take ownership of the new pointer
    m_file = file;
  }
}

//-----------------------------------------------------------------------------
// SmartFILEPtr assignment operator
//-----------------------------------------------------------------------------
castor::utils::SmartFILEPtr
  &castor::utils::SmartFILEPtr::operator=(SmartFILEPtr& obj)
   {
  reset(obj.release());
  return *this;
}

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
castor::utils::SmartFILEPtr::~SmartFILEPtr() throw() {
  reset();
}

//-----------------------------------------------------------------------------
// get
//-----------------------------------------------------------------------------
FILE *castor::utils::SmartFILEPtr::get() const throw() {
  return m_file;
}

//-----------------------------------------------------------------------------
// release
//-----------------------------------------------------------------------------
FILE *castor::utils::SmartFILEPtr::release()
   {
  // If this smart pointer does not own a pointer
  if(m_file == NULL) {
    castor::exception::NotAnOwner ex;
    ex.getMessage() << "Smart pointer does not own a FILE pointer";
    throw ex;
  }

  FILE *const tmp = m_file;

  // A NULL value indicates this smart pointer does not own a pointer
  m_file = NULL;

  return tmp;
}

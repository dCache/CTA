/******************************************************************************
 *                      MsgSvc.cpp
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
 * @(#)$RCSfile: MsgSvc.cpp,v $ $Revision: 1.4 $ $Release$ $Date: 2004/07/12 14:19:02 $ $Author: sponcec3 $
 *
 * A message service internaly using a castor log stream
 *
 * @author Sebastien Ponce
 *****************************************************************************/

// Include Files
#include "castor/logstream.h"

// Local Includes
#include "MsgSvc.hpp"

// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
castor::MsgSvc::MsgSvc(const std::string name) throw() :
  BaseSvc(name) {
  // create the inner stream
  m_stream = new castor::logstream(name);
}

// -----------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------
castor::MsgSvc::~MsgSvc() throw() {
  m_stream->close();
  delete m_stream;
}

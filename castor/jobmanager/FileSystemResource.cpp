/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/jobmanager/FileSystemResource.cpp
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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "castor/jobmanager/DiskServerResource.hpp"
#include "castor/jobmanager/FileSystemResource.hpp"
#include "castor/monitoring/AdminStatusCodes.hpp"
#include "castor/stager/FileSystemStatusCodes.hpp"
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::jobmanager::FileSystemResource::FileSystemResource() throw() :
  m_mountPoint(""),
  m_diskPoolName(""),
  m_svcClassName(""),
  m_status(castor::stager::FileSystemStatusCodes(0)),
  m_adminStatus(castor::monitoring::AdminStatusCodes(0)),
  m_diskServer(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::jobmanager::FileSystemResource::~FileSystemResource() throw() {
  if (0 != m_diskServer) {
    m_diskServer->removeFileSystems(this);
  }
}


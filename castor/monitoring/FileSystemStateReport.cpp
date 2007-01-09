/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/monitoring/FileSystemStateReport.cpp
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
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/monitoring/AdminStatusCodes.hpp"
#include "castor/monitoring/DiskServerStateReport.hpp"
#include "castor/monitoring/FileSystemStateReport.hpp"
#include "castor/stager/FileSystemStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::monitoring::FileSystemStateReport::FileSystemStateReport() throw() :
  m_mountPoint(""),
  m_space(0),
  m_id(0),
  m_status(castor::stager::FileSystemStatusCodes(0)),
  m_adminStatus(AdminStatusCodes(0)),
  m_diskServer(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::monitoring::FileSystemStateReport::~FileSystemStateReport() throw() {
  if (0 != m_diskServer) {
    m_diskServer->removeFileSystemStatesReports(this);
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::monitoring::FileSystemStateReport::print(std::ostream& stream,
                                                      std::string indent,
                                                      castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# FileSystemStateReport #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "mountPoint : " << m_mountPoint << std::endl;
  stream << indent << "space : " << m_space << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "status : " << FileSystemStatusCodesStrings[m_status] << std::endl;
  stream << indent << "adminStatus : " << AdminStatusCodesStrings[m_adminStatus] << std::endl;
  stream << indent << "DiskServer : " << std::endl;
  if (0 != m_diskServer) {
    m_diskServer->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::monitoring::FileSystemStateReport::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::monitoring::FileSystemStateReport::TYPE() {
  return OBJ_FileSystemStateReport;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::monitoring::FileSystemStateReport::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::monitoring::FileSystemStateReport::clone() {
  return new FileSystemStateReport(*this);
}


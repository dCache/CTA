/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/Disk2DiskCopyStartRequest.cpp
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
#include "castor/stager/Disk2DiskCopyStartRequest.hpp"
#include "castor/stager/Request.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::Disk2DiskCopyStartRequest::Disk2DiskCopyStartRequest() throw() :
  Request(),
  m_diskCopyId(0),
  m_sourceDiskCopyId(0),
  m_destSvcClass(""),
  m_diskServer(""),
  m_mountPoint(""),
  m_fileId(0),
  m_nsHost(""),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::Disk2DiskCopyStartRequest::~Disk2DiskCopyStartRequest() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::Disk2DiskCopyStartRequest::print(std::ostream& stream,
                                                      std::string indent,
                                                      castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# Disk2DiskCopyStartRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->Request::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "diskCopyId : " << m_diskCopyId << std::endl;
  stream << indent << "sourceDiskCopyId : " << m_sourceDiskCopyId << std::endl;
  stream << indent << "destSvcClass : " << m_destSvcClass << std::endl;
  stream << indent << "diskServer : " << m_diskServer << std::endl;
  stream << indent << "mountPoint : " << m_mountPoint << std::endl;
  stream << indent << "fileId : " << m_fileId << std::endl;
  stream << indent << "nsHost : " << m_nsHost << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::Disk2DiskCopyStartRequest::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::Disk2DiskCopyStartRequest::TYPE() {
  return OBJ_Disk2DiskCopyStartRequest;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::Disk2DiskCopyStartRequest::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::Disk2DiskCopyStartRequest::clone() {
  return new Disk2DiskCopyStartRequest(*this);
}


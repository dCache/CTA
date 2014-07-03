/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

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

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/stager/DiskCopyInfo.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::DiskCopyInfo::DiskCopyInfo() throw() :
  m_diskCopyPath(""),
  m_size(0),
  m_diskCopyStatus(0),
  m_segmentStatus(0),
  m_diskCopyId(0),
  m_fileId(0),
  m_nsHost(""),
  m_diskServer(""),
  m_mountPoint(""),
  m_nbAccesses(0),
  m_lastKnownFileName(""),
  m_svcClass(""),
  m_csumType(""),
  m_csumValue(""),
  m_creationTime(0),
  m_lastAccessTime(0),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::DiskCopyInfo::~DiskCopyInfo() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::DiskCopyInfo::print(std::ostream& stream,
                                         std::string indent,
                                         castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# DiskCopyInfo #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "diskCopyPath : " << m_diskCopyPath << std::endl;
  stream << indent << "size : " << m_size << std::endl;
  stream << indent << "diskCopyStatus : " << m_diskCopyStatus << std::endl;
  stream << indent << "segmentStatus : " << m_segmentStatus << std::endl;
  stream << indent << "diskCopyId : " << m_diskCopyId << std::endl;
  stream << indent << "fileId : " << m_fileId << std::endl;
  stream << indent << "nsHost : " << m_nsHost << std::endl;
  stream << indent << "diskServer : " << m_diskServer << std::endl;
  stream << indent << "mountPoint : " << m_mountPoint << std::endl;
  stream << indent << "nbAccesses : " << m_nbAccesses << std::endl;
  stream << indent << "lastKnownFileName : " << m_lastKnownFileName << std::endl;
  stream << indent << "svcClass : " << m_svcClass << std::endl;
  stream << indent << "csumType : " << m_csumType << std::endl;
  stream << indent << "csumValue : " << m_csumValue << std::endl;
  stream << indent << "creationTime : " << m_creationTime << std::endl;
  stream << indent << "lastAccessTime : " << m_lastAccessTime << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::DiskCopyInfo::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::DiskCopyInfo::TYPE() {
  return OBJ_DiskCopyInfo;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::DiskCopyInfo::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::DiskCopyInfo::clone() {
  return new DiskCopyInfo(*this);
}


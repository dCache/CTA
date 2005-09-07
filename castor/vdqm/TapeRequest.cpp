/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeRequest.cpp
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
#include "castor/stager/ClientIdentification.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/vdqm/DeviceGroupName.hpp"
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::TapeRequest::TapeRequest() throw() :
  m_priority(0),
  m_modificationTime(0),
  m_creationTime(0),
  m_id(0),
  m_tape(0),
  m_tapeAccessSpecification(0),
  m_requestedSrv(0),
  m_tapeDrive(0),
  m_deviceGroupName(0),
  m_client(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::vdqm::TapeRequest::~TapeRequest() throw() {
  if (0 != m_tapeDrive) {
    m_tapeDrive->setRunningTapeReq(0);
  }
  if (0 != m_client) {
    delete m_client;
    m_client = 0;
  }
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeRequest::print(std::ostream& stream,
                                      std::string indent,
                                      castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# TapeRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "priority : " << m_priority << std::endl;
  stream << indent << "modificationTime : " << m_modificationTime << std::endl;
  stream << indent << "creationTime : " << m_creationTime << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "Tape : " << std::endl;
  if (0 != m_tape) {
    m_tape->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "TapeAccessSpecification : " << std::endl;
  if (0 != m_tapeAccessSpecification) {
    m_tapeAccessSpecification->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "RequestedSrv : " << std::endl;
  if (0 != m_requestedSrv) {
    m_requestedSrv->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "TapeDrive : " << std::endl;
  if (0 != m_tapeDrive) {
    m_tapeDrive->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "DeviceGroupName : " << std::endl;
  if (0 != m_deviceGroupName) {
    m_deviceGroupName->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "Client : " << std::endl;
  if (0 != m_client) {
    m_client->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeRequest::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::vdqm::TapeRequest::TYPE() {
  return OBJ_TapeRequest;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::vdqm::TapeRequest::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::vdqm::TapeRequest::clone() {
  return new TapeRequest(*this);
}


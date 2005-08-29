/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeDrive.cpp
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
#include "castor/stager/Tape.hpp"
#include "castor/vdqm/DeviceGroupName.hpp"
#include "castor/vdqm/ErrorHistory.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDriveCompatibility.hpp"
#include "castor/vdqm/TapeDriveDedication.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::TapeDrive::TapeDrive() throw() :
  m_jobID(0),
  m_modificationTime(0),
  m_resettime(0),
  m_usecount(0),
  m_errcount(0),
  m_transferredMB(0),
  m_totalMB(0),
  m_driveName(""),
  m_tapeAccessMode(0),
  m_id(0),
  m_tape(0),
  m_runningTapeReq(0),
  m_deviceGroupName(0),
  m_status(TapeDriveStatusCodes(0)),
  m_tapeServer(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::vdqm::TapeDrive::~TapeDrive() throw() {
  if (0 != m_runningTapeReq) {
    m_runningTapeReq->setTapeDrive(0);
  }
  for (unsigned int i = 0; i < m_errorHistoryVector.size(); i++) {
    m_errorHistoryVector[i]->setTapeDrive(0);
  }
  m_errorHistoryVector.clear();
  for (unsigned int i = 0; i < m_tapeDriveDedicationVector.size(); i++) {
    m_tapeDriveDedicationVector[i]->setTapeDrive(0);
  }
  m_tapeDriveDedicationVector.clear();
  m_tapeDriveCompatibilitesVector.clear();
  if (0 != m_tapeServer) {
    m_tapeServer->removeTapeDrives(this);
  }
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeDrive::print(std::ostream& stream,
                                    std::string indent,
                                    castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# TapeDrive #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "jobID : " << m_jobID << std::endl;
  stream << indent << "modificationTime : " << m_modificationTime << std::endl;
  stream << indent << "resettime : " << m_resettime << std::endl;
  stream << indent << "usecount : " << m_usecount << std::endl;
  stream << indent << "errcount : " << m_errcount << std::endl;
  stream << indent << "transferredMB : " << m_transferredMB << std::endl;
  stream << indent << "totalMB : " << m_totalMB << std::endl;
  stream << indent << "driveName : " << m_driveName << std::endl;
  stream << indent << "tapeAccessMode : " << m_tapeAccessMode << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "Tape : " << std::endl;
  if (0 != m_tape) {
    m_tape->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "RunningTapeReq : " << std::endl;
  if (0 != m_runningTapeReq) {
    m_runningTapeReq->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  {
    stream << indent << "ErrorHistory : " << std::endl;
    int i;
    std::vector<ErrorHistory*>::const_iterator it;
    for (it = m_errorHistoryVector.begin(), i = 0;
         it != m_errorHistoryVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  {
    stream << indent << "TapeDriveDedication : " << std::endl;
    int i;
    std::vector<TapeDriveDedication*>::const_iterator it;
    for (it = m_tapeDriveDedicationVector.begin(), i = 0;
         it != m_tapeDriveDedicationVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  {
    stream << indent << "TapeDriveCompatibilites : " << std::endl;
    int i;
    std::vector<TapeDriveCompatibility*>::const_iterator it;
    for (it = m_tapeDriveCompatibilitesVector.begin(), i = 0;
         it != m_tapeDriveCompatibilitesVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  stream << indent << "DeviceGroupName : " << std::endl;
  if (0 != m_deviceGroupName) {
    m_deviceGroupName->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "status : " << TapeDriveStatusCodesStrings[m_status] << std::endl;
  stream << indent << "TapeServer : " << std::endl;
  if (0 != m_tapeServer) {
    m_tapeServer->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeDrive::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::vdqm::TapeDrive::TYPE() {
  return OBJ_TapeDrive;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::vdqm::TapeDrive::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::vdqm::TapeDrive::clone() {
  return new TapeDrive(*this);
}


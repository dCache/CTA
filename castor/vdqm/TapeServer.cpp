/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeServer.cpp
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
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::TapeServer::TapeServer() throw() :
  m_serverName(""),
  m_id(0),
  m_actingMode(TapeServerStatusCodes(0)) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::vdqm::TapeServer::~TapeServer() throw() {
  for (unsigned int i = 0; i < m_tapeDrivesVector.size(); i++) {
    m_tapeDrivesVector[i]->setTapeServer(0);
    delete m_tapeDrivesVector[i];
  }
  m_tapeDrivesVector.clear();
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeServer::print(std::ostream& stream,
                                     std::string indent,
                                     castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# TapeServer #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "serverName : " << m_serverName << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  {
    stream << indent << "TapeDrives : " << std::endl;
    int i;
    std::vector<TapeDrive*>::const_iterator it;
    for (it = m_tapeDrivesVector.begin(), i = 0;
         it != m_tapeDrivesVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  stream << indent << "actingMode : " << TapeServerStatusCodesStrings[m_actingMode] << std::endl;
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeServer::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::vdqm::TapeServer::TYPE() {
  return OBJ_TapeServer;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::vdqm::TapeServer::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::vdqm::TapeServer::clone() {
  return new TapeServer(*this);
}


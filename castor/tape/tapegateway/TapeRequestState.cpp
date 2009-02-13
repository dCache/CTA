/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/TapeRequestState.cpp
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
#include "castor/stager/Stream.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/tape/tapegateway/TapeRequestState.hpp"
#include "castor/tape/tapegateway/TapeRequestStateCode.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::TapeRequestState::TapeRequestState() throw() :
  m_accessMode(0),
  m_startTime(0),
  m_lastVdqmPingTime(0),
  m_vdqmVolReqId(0),
  m_nbRetry(0),
  m_id(0),
  m_streamMigration(0),
  m_tapeRecall(0),
  m_status(TapeRequestStateCode(0)) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::TapeRequestState::~TapeRequestState() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::TapeRequestState::print(std::ostream& stream,
                                                        std::string indent,
                                                        castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# TapeRequestState #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "accessMode : " << m_accessMode << std::endl;
  stream << indent << "startTime : " << m_startTime << std::endl;
  stream << indent << "lastVdqmPingTime : " << m_lastVdqmPingTime << std::endl;
  stream << indent << "vdqmVolReqId : " << m_vdqmVolReqId << std::endl;
  stream << indent << "nbRetry : " << m_nbRetry << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "StreamMigration : " << std::endl;
  if (0 != m_streamMigration) {
    m_streamMigration->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "TapeRecall : " << std::endl;
  if (0 != m_tapeRecall) {
    m_tapeRecall->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "status : " << TapeRequestStateCodeStrings[m_status] << std::endl;
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::TapeRequestState::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::tape::tapegateway::TapeRequestState::TYPE() {
  return OBJ_TapeRequestState;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::tape::tapegateway::TapeRequestState::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::tape::tapegateway::TapeRequestState::clone() {
  return new TapeRequestState(*this);
}


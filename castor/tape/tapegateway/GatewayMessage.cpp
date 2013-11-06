/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/GatewayMessage.cpp
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
#include "castor/ObjectSet.hpp"
#include "castor/tape/tapegateway/GatewayMessage.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::GatewayMessage::GatewayMessage() throw() :
  m_mountTransactionId(0),
  m_aggregatorTransactionId(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::GatewayMessage::~GatewayMessage() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::GatewayMessage::print(std::ostream& stream,
                                                      std::string indent,
                                                      castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# GatewayMessage #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "mountTransactionId : " << m_mountTransactionId << std::endl;
  stream << indent << "aggregatorTransactionId : " << m_aggregatorTransactionId << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::GatewayMessage::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}


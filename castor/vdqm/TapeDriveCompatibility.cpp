/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeDriveCompatibility.cpp
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
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeDriveCompatibility.hpp"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::TapeDriveCompatibility::TapeDriveCompatibility() throw() :
  m_tapeDriveModel(""),
  m_priorityLevel(0),
  m_id(0),
  m_tapeAccessSpecification(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::vdqm::TapeDriveCompatibility::~TapeDriveCompatibility() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeDriveCompatibility::print(std::ostream& stream,
                                                 std::string indent,
                                                 castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# TapeDriveCompatibility #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "tapeDriveModel : " << m_tapeDriveModel << std::endl;
  stream << indent << "priorityLevel : " << m_priorityLevel << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "TapeAccessSpecification : " << std::endl;
  if (0 != m_tapeAccessSpecification) {
    m_tapeAccessSpecification->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeDriveCompatibility::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::vdqm::TapeDriveCompatibility::TYPE() {
  return OBJ_TapeDriveCompatibility;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::vdqm::TapeDriveCompatibility::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::vdqm::TapeDriveCompatibility::clone() {
  return new TapeDriveCompatibility(*this);
}


/******************************************************************************
 *                      castor/stager/TapeCopyForMigration.cpp
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
#include "castor/stager/TapeCopy.hpp"
#include "castor/stager/TapeCopyForMigration.hpp"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::TapeCopyForMigration::TapeCopyForMigration() throw() :
  TapeCopy(),
  m_diskServer(""),
  m_mountPoint("") {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::TapeCopyForMigration::~TapeCopyForMigration() throw() {
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::TapeCopyForMigration::print(std::ostream& stream,
                                                 std::string indent,
                                                 castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# TapeCopyForMigration #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->TapeCopy::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "diskServer : " << m_diskServer << std::endl;
  stream << indent << "mountPoint : " << m_mountPoint << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::TapeCopyForMigration::print() const {
  ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::TapeCopyForMigration::TYPE() {
  return OBJ_TapeCopyForMigration;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::TapeCopyForMigration::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::TapeCopyForMigration::clone() {
  return new TapeCopyForMigration(*this);
}


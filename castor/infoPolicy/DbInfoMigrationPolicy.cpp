/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/infoPolicy/DbInfoMigrationPolicy.cpp
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
#include "castor/infoPolicy/DbInfoMigrationPolicy.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::infoPolicy::DbInfoMigrationPolicy::DbInfoMigrationPolicy() throw() :
  m_tapeCopyId(0),
  m_copyNb(0),
  m_castorFileName(""),
  m_fileId(0),
  m_tapePoolName(""),
  m_nsHost(""),
  m_tapePoolId(0),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::infoPolicy::DbInfoMigrationPolicy::~DbInfoMigrationPolicy() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::infoPolicy::DbInfoMigrationPolicy::print(std::ostream& stream,
                                                      std::string indent,
                                                      castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# DbInfoMigrationPolicy #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "tapeCopyId : " << m_tapeCopyId << std::endl;
  stream << indent << "copyNb : " << m_copyNb << std::endl;
  stream << indent << "castorFileName : " << m_castorFileName << std::endl;
  stream << indent << "fileId : " << m_fileId << std::endl;
  stream << indent << "tapePoolName : " << m_tapePoolName << std::endl;
  stream << indent << "nsHost : " << m_nsHost << std::endl;
  stream << indent << "tapePoolId : " << m_tapePoolId << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::infoPolicy::DbInfoMigrationPolicy::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::infoPolicy::DbInfoMigrationPolicy::TYPE() {
  return OBJ_DbInfoMigrationPolicy;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::infoPolicy::DbInfoMigrationPolicy::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::infoPolicy::DbInfoMigrationPolicy::clone() {
  return new DbInfoMigrationPolicy(*this);
}


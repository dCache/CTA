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
#include "castor/bwlist/ListPrivileges.hpp"
#include "castor/stager/Request.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::bwlist::ListPrivileges::ListPrivileges() throw() :
  Request(),
  m_userId(0),
  m_groupId(0),
  m_requestType(0),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::bwlist::ListPrivileges::~ListPrivileges() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::bwlist::ListPrivileges::print(std::ostream& stream,
                                           std::string indent,
                                           castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# ListPrivileges #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->castor::stager::Request::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "userId : " << m_userId << std::endl;
  stream << indent << "groupId : " << m_groupId << std::endl;
  stream << indent << "requestType : " << m_requestType << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::bwlist::ListPrivileges::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::bwlist::ListPrivileges::TYPE() {
  return OBJ_ListPrivileges;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::bwlist::ListPrivileges::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::bwlist::ListPrivileges::clone() {
  return new ListPrivileges(*this);
}


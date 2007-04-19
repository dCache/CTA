/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/query/DiskPoolQueryResponse.cpp
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
#include "castor/query/DiskPoolQueryResponse.hpp"
#include "castor/query/DiskServerDescription.hpp"
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::query::DiskPoolQueryResponse::DiskPoolQueryResponse() throw() :
  Response(),
  m_diskPoolName(""),
  m_freeSpace(0),
  m_totalSpace(0),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::query::DiskPoolQueryResponse::~DiskPoolQueryResponse() throw() {
  for (unsigned int i = 0; i < m_diskServersVector.size(); i++) {
    m_diskServersVector[i]->setQuery(0);
    delete m_diskServersVector[i];
  }
  m_diskServersVector.clear();
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::query::DiskPoolQueryResponse::print(std::ostream& stream,
                                                 std::string indent,
                                                 castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# DiskPoolQueryResponse #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->castor::rh::Response::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "diskPoolName : " << m_diskPoolName << std::endl;
  stream << indent << "freeSpace : " << m_freeSpace << std::endl;
  stream << indent << "totalSpace : " << m_totalSpace << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  {
    stream << indent << "DiskServers : " << std::endl;
    int i;
    std::vector<DiskServerDescription*>::const_iterator it;
    for (it = m_diskServersVector.begin(), i = 0;
         it != m_diskServersVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::query::DiskPoolQueryResponse::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::query::DiskPoolQueryResponse::TYPE() {
  return OBJ_DiskPoolQueryResponse;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::query::DiskPoolQueryResponse::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::query::DiskPoolQueryResponse::clone() {
  castor::query::DiskPoolQueryResponse* c = new DiskPoolQueryResponse(*this);
  c->diskServers().clear();
  for (std::vector<DiskServerDescription*>::iterator it =
	 diskServers().begin();
       it != diskServers().end();
       it++) {
    c->addDiskServers(dynamic_cast<DiskServerDescription*>((*it)->clone()));
  }
  return c;
}


/******************************************************************************
 *                      castor/stager/ReqId.cpp
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
 * @(#)$RCSfile: ReqId.cpp,v $ $Revision: 1.1 $ $Release$ $Date: 2004/10/01 14:26:23 $ $Author: sponcec3 $
 *
 * 
 *
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

// Include Files
#include "castor/Constants.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/stager/ReqId.hpp"
#include "castor/stager/ReqIdRequest.hpp"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::ReqId::ReqId() throw() :
  m_value(""),
  m_id(0),
  m_request(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::ReqId::~ReqId() throw() {
  if (0 != m_request) {
    m_request->removeReqids(this);
  }
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::ReqId::print(std::ostream& stream,
                                  std::string indent,
                                  castor::ObjectSet& alreadyPrinted) const {
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "value : " << m_value << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "Request : " << std::endl;
  if (0 != m_request) {
    m_request->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::ReqId::print() const {
  ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::ReqId::TYPE() {
  return OBJ_ReqId;
}

//------------------------------------------------------------------------------
// setId
//------------------------------------------------------------------------------
void castor::stager::ReqId::setId(unsigned long id) {
  m_id = id;
}

//------------------------------------------------------------------------------
// id
//------------------------------------------------------------------------------
unsigned long castor::stager::ReqId::id() const {
  return m_id;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::ReqId::type() const {
  return TYPE();
}


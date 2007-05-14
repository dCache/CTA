/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/MoverCloseRequest.cpp
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
#include "castor/stager/MoverCloseRequest.hpp"
#include "castor/stager/Request.hpp"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::MoverCloseRequest::MoverCloseRequest() throw() :
  Request(),
  m_subReqId(0),
  m_fileSize(0),
  m_timeStamp(0),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::MoverCloseRequest::~MoverCloseRequest() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::MoverCloseRequest::print(std::ostream& stream,
                                              std::string indent,
                                              castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# MoverCloseRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->Request::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "subReqId : " << m_subReqId << std::endl;
  stream << indent << "fileSize : " << m_fileSize << std::endl;
  stream << indent << "timeStamp : " << m_timeStamp << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::MoverCloseRequest::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::MoverCloseRequest::TYPE() {
  return OBJ_MoverCloseRequest;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::MoverCloseRequest::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::MoverCloseRequest::clone() {
  return new MoverCloseRequest(*this);
}


/******************************************************************************
 *                      castor/stager/StagePrepareToPutRequest.cpp
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
#include "castor/ObjectSet.hpp"
#include "castor/stager/FileRequest.hpp"
#include "castor/stager/StagePrepareToPutRequest.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::StagePrepareToPutRequest::StagePrepareToPutRequest() throw() :
  FileRequest(),
  m_id() {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::StagePrepareToPutRequest::~StagePrepareToPutRequest() throw() {
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::StagePrepareToPutRequest::print(std::ostream& stream,
                                                     std::string indent,
                                                     castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# StagePrepareToPutRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->FileRequest::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::StagePrepareToPutRequest::print() const {
  ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::StagePrepareToPutRequest::TYPE() {
  return OBJ_StagePrepareToPutRequest;
}

//------------------------------------------------------------------------------
// setId
//------------------------------------------------------------------------------
void castor::stager::StagePrepareToPutRequest::setId(u_signed64 id) {
  m_id = id;
}

//------------------------------------------------------------------------------
// id
//------------------------------------------------------------------------------
u_signed64 castor::stager::StagePrepareToPutRequest::id() const {
  return m_id;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::StagePrepareToPutRequest::type() const {
  return TYPE();
}


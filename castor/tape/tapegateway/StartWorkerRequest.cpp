/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/StartWorkerRequest.cpp
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
 * @(#)$RCSfile: StartWorkerRequest.cpp,v $ $Revision: 1.2 $ $Release$ $Date: 2009/01/27 09:51:44 $ $Author: gtaur $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/tape/tapegateway/StartWorkerRequest.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::StartWorkerRequest::StartWorkerRequest() throw() :
  m_vdqmVolReqId(0),
  m_side(0),
  m_unit(""),
  m_mode(0),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::StartWorkerRequest::~StartWorkerRequest() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::StartWorkerRequest::print(std::ostream& stream,
                                                          std::string indent,
                                                          castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# StartWorkerRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "vdqmVolReqId : " << m_vdqmVolReqId << std::endl;
  stream << indent << "side : " << m_side << std::endl;
  stream << indent << "unit : " << m_unit << std::endl;
  stream << indent << "mode : " << m_mode << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::StartWorkerRequest::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::tape::tapegateway::StartWorkerRequest::TYPE() {
  return OBJ_StartWorkerRequest;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::tape::tapegateway::StartWorkerRequest::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::tape::tapegateway::StartWorkerRequest::clone() {
  return new StartWorkerRequest(*this);
}


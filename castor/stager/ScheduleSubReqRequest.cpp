/******************************************************************************
 *                      castor/stager/ScheduleSubReqRequest.cpp
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
 * @(#)$RCSfile: ScheduleSubReqRequest.cpp,v $ $Revision: 1.3 $ $Release$ $Date: 2004/12/01 10:41:16 $ $Author: sponcec3 $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/ScheduleSubReqRequest.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::ScheduleSubReqRequest::ScheduleSubReqRequest() throw() :
  Request(),
  m_subreqId(0),
  m_diskServer(""),
  m_fileSystem(""),
  m_id(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::ScheduleSubReqRequest::~ScheduleSubReqRequest() throw() {
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::ScheduleSubReqRequest::print(std::ostream& stream,
                                                  std::string indent,
                                                  castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# ScheduleSubReqRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->Request::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "subreqId : " << m_subreqId << std::endl;
  stream << indent << "diskServer : " << m_diskServer << std::endl;
  stream << indent << "fileSystem : " << m_fileSystem << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::ScheduleSubReqRequest::print() const {
  ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::ScheduleSubReqRequest::TYPE() {
  return OBJ_ScheduleSubReqRequest;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::ScheduleSubReqRequest::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::ScheduleSubReqRequest::clone() {
  return new ScheduleSubReqRequest(*this);
}


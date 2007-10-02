/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/repack/RepackRequest.cpp
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
#include "castor/repack/RepackRequest.hpp"
#include "castor/repack/RepackResponse.hpp"
#include "castor/repack/RepackSubRequest.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::repack::RepackRequest::RepackRequest() throw() :
  m_machine(""),
  m_userName(""),
  m_creationTime(0),
  m_pool(""),
  m_pid(0),
  m_svcclass(""),
  m_command(0),
  m_stager(""),
  m_userId(0),
  m_groupId(0),
  m_retryMax(0),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::repack::RepackRequest::~RepackRequest() throw() {
  for (unsigned int i = 0; i < m_repackresponseVector.size(); i++) {
    m_repackresponseVector[i]->setRepackrequest(0);
  }
  m_repackresponseVector.clear();
  for (unsigned int i = 0; i < m_repacksubrequestVector.size(); i++) {
    m_repacksubrequestVector[i]->setRepackrequest(0);
  }
  m_repacksubrequestVector.clear();
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::repack::RepackRequest::print(std::ostream& stream,
                                          std::string indent,
                                          castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# RepackRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "machine : " << m_machine << std::endl;
  stream << indent << "userName : " << m_userName << std::endl;
  stream << indent << "creationTime : " << m_creationTime << std::endl;
  stream << indent << "pool : " << m_pool << std::endl;
  stream << indent << "pid : " << m_pid << std::endl;
  stream << indent << "svcclass : " << m_svcclass << std::endl;
  stream << indent << "command : " << m_command << std::endl;
  stream << indent << "stager : " << m_stager << std::endl;
  stream << indent << "userId : " << m_userId << std::endl;
  stream << indent << "groupId : " << m_groupId << std::endl;
  stream << indent << "retryMax : " << m_retryMax << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  {
    stream << indent << "Repackresponse : " << std::endl;
    int i;
    std::vector<RepackResponse*>::const_iterator it;
    for (it = m_repackresponseVector.begin(), i = 0;
         it != m_repackresponseVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  {
    stream << indent << "Repacksubrequest : " << std::endl;
    int i;
    std::vector<RepackSubRequest*>::const_iterator it;
    for (it = m_repacksubrequestVector.begin(), i = 0;
         it != m_repacksubrequestVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::repack::RepackRequest::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::repack::RepackRequest::TYPE() {
  return OBJ_RepackRequest;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::repack::RepackRequest::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::repack::RepackRequest::clone() {
  return new RepackRequest(*this);
}


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/jobmanager/JobSubmissionRequest.cpp
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
#include "castor/jobmanager/JobSubmissionRequest.hpp"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::jobmanager::JobSubmissionRequest::JobSubmissionRequest() throw() :
  m_subReqId(""),
  m_protocol(""),
  m_xsize(0),
  m_requestedFileSystems(""),
  m_reqId(""),
  m_clientVersion(0),
  m_fileId(0),
  m_nsHost(""),
  m_svcClass(""),
  m_ipAddress(0),
  m_port(0),
  m_requestType(0),
  m_username(""),
  m_euid(0),
  m_egid(0),
  m_selectTime(0),
  m_submitStartTime(0),
  m_jobId(0),
  m_clientType(0),
  m_openFlags(""),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::jobmanager::JobSubmissionRequest::~JobSubmissionRequest() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::jobmanager::JobSubmissionRequest::print(std::ostream& stream,
                                                     std::string indent,
                                                     castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# JobSubmissionRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "subReqId : " << m_subReqId << std::endl;
  stream << indent << "protocol : " << m_protocol << std::endl;
  stream << indent << "xsize : " << m_xsize << std::endl;
  stream << indent << "requestedFileSystems : " << m_requestedFileSystems << std::endl;
  stream << indent << "reqId : " << m_reqId << std::endl;
  stream << indent << "clientVersion : " << m_clientVersion << std::endl;
  stream << indent << "fileId : " << m_fileId << std::endl;
  stream << indent << "nsHost : " << m_nsHost << std::endl;
  stream << indent << "svcClass : " << m_svcClass << std::endl;
  stream << indent << "ipAddress : " << m_ipAddress << std::endl;
  stream << indent << "port : " << m_port << std::endl;
  stream << indent << "requestType : " << m_requestType << std::endl;
  stream << indent << "username : " << m_username << std::endl;
  stream << indent << "euid : " << m_euid << std::endl;
  stream << indent << "egid : " << m_egid << std::endl;
  stream << indent << "selectTime : " << m_selectTime << std::endl;
  stream << indent << "submitStartTime : " << m_submitStartTime << std::endl;
  stream << indent << "jobId : " << m_jobId << std::endl;
  stream << indent << "clientType : " << m_clientType << std::endl;
  stream << indent << "openFlags : " << m_openFlags << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::jobmanager::JobSubmissionRequest::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::jobmanager::JobSubmissionRequest::TYPE() {
  return OBJ_JobSubmissionRequest;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::jobmanager::JobSubmissionRequest::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::jobmanager::JobSubmissionRequest::clone() {
  return new JobSubmissionRequest(*this);
}


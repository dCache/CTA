/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/FileRecallReportList.cpp
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
#include "castor/tape/tapegateway/FileErrorReportStruct.hpp"
#include "castor/tape/tapegateway/FileRecallReportList.hpp"
#include "castor/tape/tapegateway/FileRecalledNotificationStruct.hpp"
#include "castor/tape/tapegateway/GatewayMessage.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::FileRecallReportList::FileRecallReportList() throw() :
  GatewayMessage(),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::FileRecallReportList::~FileRecallReportList() throw() {
  for (unsigned int i = 0; i < m_successfulRecallsVector.size(); i++) {
    m_successfulRecallsVector[i]->setFileRecallReportList(0);
    delete m_successfulRecallsVector[i];
  }
  m_successfulRecallsVector.clear();
  for (unsigned int i = 0; i < m_failedRecallsVector.size(); i++) {
    m_failedRecallsVector[i]->setFileRecallReportList(0);
    delete m_failedRecallsVector[i];
  }
  m_failedRecallsVector.clear();
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::FileRecallReportList::print(std::ostream& stream,
                                                            std::string indent,
                                                            castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# FileRecallReportList #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->GatewayMessage::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  {
    stream << indent << "SuccessfulRecalls : " << std::endl;
    int i;
    std::vector<FileRecalledNotificationStruct*>::const_iterator it;
    for (it = m_successfulRecallsVector.begin(), i = 0;
         it != m_successfulRecallsVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  {
    stream << indent << "FailedRecalls : " << std::endl;
    int i;
    std::vector<FileErrorReportStruct*>::const_iterator it;
    for (it = m_failedRecallsVector.begin(), i = 0;
         it != m_failedRecallsVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::FileRecallReportList::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::tape::tapegateway::FileRecallReportList::TYPE() {
  return OBJ_FileRecallReportList;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::tape::tapegateway::FileRecallReportList::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::tape::tapegateway::FileRecallReportList::clone() {
  return new FileRecallReportList(*this);
}


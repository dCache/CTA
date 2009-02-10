/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/FileToRecallResponse.cpp
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
 * @(#)$RCSfile: FileToRecallResponse.cpp,v $ $Revision: 1.9 $ $Release$ $Date: 2009/02/10 16:04:22 $ $Author: gtaur $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/tape/tapegateway/ConvertCommandCode.hpp"
#include "castor/tape/tapegateway/FileToRecallResponse.hpp"
#include "castor/tape/tapegateway/NsFileInformation.hpp"
#include "castor/tape/tapegateway/PositionCommandCode.hpp"
#include "castor/tape/tapegateway/RecordFormatCode.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::FileToRecallResponse::FileToRecallResponse() throw() :
  m_errorCode(0),
  m_errorMessage(""),
  m_path(""),
  m_transactionId(0),
  m_concat(false),
  m_deferedAllocation(false),
  m_id(0),
  m_positionCommandCode(PositionCommandCode(0)),
  m_convertCommandCode(ConvertCommandCode(0)),
  m_recordFormatCode(RecordFormatCode(0)),
  m_nsFileInformation(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::tape::tapegateway::FileToRecallResponse::~FileToRecallResponse() throw() {
  if (0 != m_nsFileInformation) {
    delete m_nsFileInformation;
    m_nsFileInformation = 0;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::FileToRecallResponse::print(std::ostream& stream,
                                                            std::string indent,
                                                            castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# FileToRecallResponse #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "errorCode : " << m_errorCode << std::endl;
  stream << indent << "errorMessage : " << m_errorMessage << std::endl;
  stream << indent << "path : " << m_path << std::endl;
  stream << indent << "transactionId : " << m_transactionId << std::endl;
  stream << indent << "concat : " << (m_concat ? "Yes" : "No") << std::endl;
  stream << indent << "deferedAllocation : " << (m_deferedAllocation ? "Yes" : "No") << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "positionCommandCode : " << PositionCommandCodeStrings[m_positionCommandCode] << std::endl;
  stream << indent << "convertCommandCode : " << ConvertCommandCodeStrings[m_convertCommandCode] << std::endl;
  stream << indent << "recordFormatCode : " << RecordFormatCodeStrings[m_recordFormatCode] << std::endl;
  stream << indent << "NsFileInformation : " << std::endl;
  if (0 != m_nsFileInformation) {
    m_nsFileInformation->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::tape::tapegateway::FileToRecallResponse::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::tape::tapegateway::FileToRecallResponse::TYPE() {
  return OBJ_FileToRecallResponse;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::tape::tapegateway::FileToRecallResponse::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::tape::tapegateway::FileToRecallResponse::clone() {
  return new FileToRecallResponse(*this);
}


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/DiskCopyForRecall.cpp
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
#include "castor/stager/DiskCopyForRecall.hpp"
#include "castor/stager/DiskCopyStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::DiskCopyForRecall::DiskCopyForRecall() throw() :
  m_mountPoint(""),
  m_diskServer(""),
  m_fsWeight(0.0),
  m_id(0),
  m_status(DiskCopyStatusCodes(0)) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::DiskCopyForRecall::~DiskCopyForRecall() throw() {
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::DiskCopyForRecall::print(std::ostream& stream,
                                              std::string indent,
                                              castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# DiskCopyForRecall #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "mountPoint : " << m_mountPoint << std::endl;
  stream << indent << "diskServer : " << m_diskServer << std::endl;
  stream << indent << "fsWeight : " << m_fsWeight << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "status : " << DiskCopyStatusCodesStrings[m_status] << std::endl;
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::DiskCopyForRecall::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::DiskCopyForRecall::TYPE() {
  return OBJ_DiskCopyForRecall;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::DiskCopyForRecall::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::DiskCopyForRecall::clone() {
  return new DiskCopyForRecall(*this);
}


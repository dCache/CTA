/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/SvcClass.cpp
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
#include "castor/stager/DiskPool.hpp"
#include "castor/stager/FileClass.hpp"
#include "castor/stager/SvcClass.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::SvcClass::SvcClass() throw() :
  m_name(""),
  m_defaultFileSize(0),
  m_maxReplicaNb(0),
  m_recallerPolicy(""),
  m_gcPolicy(""),
  m_disk1Behavior(false),
  m_replicateOnClose(false),
  m_failJobsWhenNoSpace(false),
  m_lastEditor(""),
  m_lastEditionTime(0),
  m_id(0),
  m_forcedFileClass(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::SvcClass::~SvcClass() throw() {
  for (unsigned int i = 0; i < m_diskPoolsVector.size(); i++) {
    m_diskPoolsVector[i]->removeSvcClasses(this);
  }
  m_diskPoolsVector.clear();
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::SvcClass::print(std::ostream& stream,
                                     std::string indent,
                                     castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# SvcClass #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "name : " << m_name << std::endl;
  stream << indent << "defaultFileSize : " << m_defaultFileSize << std::endl;
  stream << indent << "maxReplicaNb : " << m_maxReplicaNb << std::endl;
  stream << indent << "recallerPolicy : " << m_recallerPolicy << std::endl;
  stream << indent << "gcPolicy : " << m_gcPolicy << std::endl;
  stream << indent << "disk1Behavior : " << (m_disk1Behavior ? "Yes" : "No") << std::endl;
  stream << indent << "replicateOnClose : " << (m_replicateOnClose ? "Yes" : "No") << std::endl;
  stream << indent << "failJobsWhenNoSpace : " << (m_failJobsWhenNoSpace ? "Yes" : "No") << std::endl;
  stream << indent << "lastEditor : " << m_lastEditor << std::endl;
  stream << indent << "lastEditionTime : " << m_lastEditionTime << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  {
    stream << indent << "DiskPools : " << std::endl;
    int i;
    std::vector<DiskPool*>::const_iterator it;
    for (it = m_diskPoolsVector.begin(), i = 0;
         it != m_diskPoolsVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  stream << indent << "ForcedFileClass : " << std::endl;
  if (0 != m_forcedFileClass) {
    m_forcedFileClass->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::SvcClass::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::SvcClass::TYPE() {
  return OBJ_SvcClass;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::SvcClass::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::SvcClass::clone() {
  return new SvcClass(*this);
}


/******************************************************************************
 *                      castor/stager/FileSystem.cpp
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
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/DiskPool.hpp"
#include "castor/stager/DiskServer.hpp"
#include "castor/stager/FileSystem.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::FileSystem::FileSystem() throw() :
  m_free(),
  m_weight(0.0),
  m_fsDeviation(0.0),
  m_mountPoint(""),
  m_id(),
  m_diskPool(0),
  m_diskserver(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::FileSystem::~FileSystem() throw() {
  if (0 != m_diskPool) {
    m_diskPool->removeFileSystems(this);
  }
  for (unsigned int i = 0; i < m_copiesVector.size(); i++) {
    m_copiesVector[i]->setFileSystem(0);
  }
  m_copiesVector.clear();
  if (0 != m_diskserver) {
    m_diskserver->removeFileSystems(this);
  }
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::FileSystem::print(std::ostream& stream,
                                       std::string indent,
                                       castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# FileSystem #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "free : " << m_free << std::endl;
  stream << indent << "weight : " << m_weight << std::endl;
  stream << indent << "fsDeviation : " << m_fsDeviation << std::endl;
  stream << indent << "mountPoint : " << m_mountPoint << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "DiskPool : " << std::endl;
  if (0 != m_diskPool) {
    m_diskPool->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  {
    stream << indent << "Copies : " << std::endl;
    int i;
    std::vector<DiskCopy*>::const_iterator it;
    for (it = m_copiesVector.begin(), i = 0;
         it != m_copiesVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  stream << indent << "Diskserver : " << std::endl;
  if (0 != m_diskserver) {
    m_diskserver->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::FileSystem::print() const {
  ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::FileSystem::TYPE() {
  return OBJ_FileSystem;
}

//------------------------------------------------------------------------------
// setId
//------------------------------------------------------------------------------
void castor::stager::FileSystem::setId(u_signed64 id) {
  m_id = id;
}

//------------------------------------------------------------------------------
// id
//------------------------------------------------------------------------------
u_signed64 castor::stager::FileSystem::id() const {
  return m_id;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::FileSystem::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::FileSystem::clone() {
  return this;
}


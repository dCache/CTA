/******************************************************************************
 *                      castor/stager/CastorFile.cpp
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
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/FileClass.hpp"
#include "castor/stager/SvcClass.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::CastorFile::CastorFile() throw() :
  m_fileId(),
  m_nsHost(""),
  m_fileSize(),
  m_id(),
  m_svcClass(0),
  m_fileClass(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::CastorFile::~CastorFile() throw() {
  for (unsigned int i = 0; i < m_diskCopiesVector.size(); i++) {
    m_diskCopiesVector[i]->setCastorFile(0);
  }
  m_diskCopiesVector.clear();
  for (unsigned int i = 0; i < m_tapeCopiesVector.size(); i++) {
    m_tapeCopiesVector[i]->setCastorFile(0);
    delete m_tapeCopiesVector[i];
  }
  m_tapeCopiesVector.clear();
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::CastorFile::print(std::ostream& stream,
                                       std::string indent,
                                       castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# CastorFile #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "fileId : " << m_fileId << std::endl;
  stream << indent << "nsHost : " << m_nsHost << std::endl;
  stream << indent << "fileSize : " << m_fileSize << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "SvcClass : " << std::endl;
  if (0 != m_svcClass) {
    m_svcClass->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "FileClass : " << std::endl;
  if (0 != m_fileClass) {
    m_fileClass->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  {
    stream << indent << "DiskCopies : " << std::endl;
    int i;
    std::vector<DiskCopy*>::const_iterator it;
    for (it = m_diskCopiesVector.begin(), i = 0;
         it != m_diskCopiesVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  {
    stream << indent << "TapeCopies : " << std::endl;
    int i;
    std::vector<TapeCopy*>::const_iterator it;
    for (it = m_tapeCopiesVector.begin(), i = 0;
         it != m_tapeCopiesVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::CastorFile::print() const {
  ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::CastorFile::TYPE() {
  return OBJ_CastorFile;
}

//------------------------------------------------------------------------------
// setId
//------------------------------------------------------------------------------
void castor::stager::CastorFile::setId(u_signed64 id) {
  m_id = id;
}

//------------------------------------------------------------------------------
// id
//------------------------------------------------------------------------------
u_signed64 castor::stager::CastorFile::id() const {
  return m_id;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::CastorFile::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::CastorFile::clone() {
  return this;
}


/******************************************************************************
 *                      castor/stager/Stream.cpp
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
#include "castor/stager/Stream.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "castor/stager/TapePool.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::Stream::Stream() throw() :
  m_initialSizeToTransfer(0),
  m_id(0),
  m_tape(0),
  m_tapePool(0),
  m_status(StreamStatusCodes(0)) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::Stream::~Stream() throw() {
  for (unsigned int i = 0; i < m_tapeCopyVector.size(); i++) {
    m_tapeCopyVector[i]->removeStream(this);
  }
  m_tapeCopyVector.clear();
  if (0 != m_tape) {
    m_tape->setStream(0);
  }
  if (0 != m_tapePool) {
    m_tapePool->removeStreams(this);
  }
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::Stream::print(std::ostream& stream,
                                   std::string indent,
                                   castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# Stream #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "initialSizeToTransfer : " << m_initialSizeToTransfer << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  {
    stream << indent << "TapeCopy : " << std::endl;
    int i;
    std::vector<TapeCopy*>::const_iterator it;
    for (it = m_tapeCopyVector.begin(), i = 0;
         it != m_tapeCopyVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  stream << indent << "Tape : " << std::endl;
  if (0 != m_tape) {
    m_tape->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "TapePool : " << std::endl;
  if (0 != m_tapePool) {
    m_tapePool->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  stream << indent << "status : " << StreamStatusCodesStrings[m_status] << std::endl;
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::Stream::print() const {
  ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::Stream::TYPE() {
  return OBJ_Stream;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::Stream::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::Stream::clone() {
  return new Stream(*this);
}


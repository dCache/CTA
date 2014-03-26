/******************************************************************************
 *                      MemManagerClient.hpp
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
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include "castor/tape/tapeserver/daemon/MemBlock.hpp"
#include "castor/tape/tapeserver/daemon/Exception.hpp"
#include "castor/exception/Exception.hpp"

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {

class MemoryManagerClient {
public:
  virtual bool endOfWork() throw() = 0;
  virtual bool provideBlock(MemBlock */*mb*/) throw(MemException) { 
    throw MemException(std::string("Tring to provide a memory block to the wrong client")); 
  };
  virtual ~MemoryManagerClient() throw() {}
};

}
}
}
}

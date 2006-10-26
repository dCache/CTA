/******************************************************************************
 *                      ClusterStatus.cpp
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
 * @author Sebastien Ponce
 *****************************************************************************/

#include "castor/monitoring/ClusterStatus.hpp"
#include "castor/monitoring/SharedMemoryAllocator.hpp"
#include <iostream>
#include <iomanip>

//------------------------------------------------------------------------------
// getClusterStatus
//------------------------------------------------------------------------------
castor::monitoring::ClusterStatus*
castor::monitoring::ClusterStatus::getClusterStatus() {
  // declare a singleton containing the cluster Status
  static ClusterStatus* smStatus = 0;
  // If first call, we have to initialize the singleton
  if (0 == smStatus) {
    // Use an Allocator
    castor::monitoring::SharedMemoryAllocator<ClusterStatus> alloc;
    // Create the ClusterStatus Object
    smStatus = new (alloc.allocate(1))ClusterStatus();
  }
  return smStatus;
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::monitoring::ClusterStatus::print
(std::ostream& out, const std::string& indentation) const
  throw() {
  if (0 == size()) {
    out << indentation << "No diskServer registered"
        << std::endl;
  } else {
    std::string dsIndent = indentation + "   ";
    for (const_iterator it = begin(); it != end(); it++) {      
      out << dsIndent << std::setw(20)
          << "name" << ": " << it->first;
      it->second.print(out, dsIndent);
    }
  }
}

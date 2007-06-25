/******************************************************************************
 *                      RmGetNodes.cpp
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
 * gets and displays the list of nodes handles by RmMasterDaemon
 * using shared memory
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#include <iostream>
#include <iomanip>
#include "castor/monitoring/ClusterStatus.hpp"
#include "castor/monitoring/DiskServerStatus.hpp"
#include "castor/exception/Exception.hpp"
#include "Cgetopt.h"

void help(std::string programName) {
  std::cout << "Usage: " << programName
            << " [-h,--help] [-n,--node diskServerName]"
            << std::endl;
}

int main(int argc, char** argv) {
  try {
    // Parse command line
    Coptind = 1; /* Required */
    Coptions_t longopts[] = {
      {"help", NO_ARGUMENT, NULL, 'h'},
      {"all", NO_ARGUMENT, NULL, 'a'},
      {"node", REQUIRED_ARGUMENT, NULL, 'n'},
      {0, 0, 0, 0}
    };
    char c;
    char* node = 0;
    int all = 0;
    while ((c = Cgetopt_long(argc, argv, "han:", longopts, NULL)) != -1) {
      switch (c) {
      case 'n':
        node = strdup(Coptarg);
        break;
      case 'a':
	all = 1;
	break;
      case 'h':
        help(argv[0]);
        exit(0);
      default:
	std::cerr << "Unknown option \n";
        help(argv[0]);
        exit(-1);
      }
    }
    argc -= Coptind;
    if (argc > 0) {
      help(argv[0]);
      exit(-1);
    }
    // Get shared memory object
    bool create = false;
    castor::monitoring::ClusterStatus* cs =
      castor::monitoring::ClusterStatus::getClusterStatus(create);
    if(cs == 0) {
      std::cout << "No shared memory area found. Please start RmMasterDaemon first."
                << std::endl << std::endl;
      return -1;
    }

    // Handle case of empty list
    if (cs->begin() == cs->end()) {
      std::cout << "No diskServer registered" << std::endl;
      return 0;
    }
    
    // Loop over cluster status
    int found = 0;
    std::string dsIndent = "   ";
    for (castor::monitoring::ClusterStatus::const_iterator it =
	   cs->begin();
	 it != cs->end();
	 it++) {
      if (node != 0) {
	if (strcmp(it->first.c_str(), node)) {
	  continue;
	}
      }

      // Don't display all information if diskserver is deleted
      if (it->second.adminStatus() == castor::monitoring::ADMIN_DELETED) {
	if (all) {
	  std::cout << dsIndent << std::setw(24) << "name" << ": " << it->first << "\n";
	  std::cout << dsIndent << std::setw(24)
		    << "status" << ": "
		    << castor::stager::DiskServerStatusCodeStrings[it->second.status()] << "\n";
	  std::cout << dsIndent << std::setw(24)
		    << "adminStatus" << ": "
		    << castor::monitoring::AdminStatusCodesStrings[it->second.adminStatus()] << "\n";
	}
      } else {
	std::cout << dsIndent << std::setw(24) << "name" << ": " << it->first << "\n";	
	it->second.print(std::cout, dsIndent); 
      }
      found++;
    }

    if ((0 == found) && (node != 0)) {
      std::cerr << "No diskServer found with name '"
		<< node << "'. Maybe check the domain."
		<< std::endl;     
    }
  } catch (castor::exception::Exception e) {
    std::cout << e.getMessage().str() << std::endl;
  }
}

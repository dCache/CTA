/**
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CTADirCmd.hpp"

#include "XrdCl/XrdClFileSystem.hh"
#include "XrdCl/XrdClCopyProcess.hh"
#include "XrdOuc/XrdOucString.hh"

#include <getopt.h>
#include <iostream>
#include <string.h>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
CTADirCmd::CTADirCmd() throw(): m_programName("CTA_dircmd") {
}

//------------------------------------------------------------------------------
// usage
//------------------------------------------------------------------------------
void CTADirCmd::usage(std::ostream &os) const throw() {
  os <<
    "Usage:\n"
    "\t" << m_programName << " <directory_name>\n";
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int CTADirCmd::main(const int argc, char **argv) throw() {
//  if(argc < 2) {
//    usage(std::cerr);
//    return 1;
//  }
  int rc = 1;
  
  // Execute the command
  try {
    rc = executeCommand(argc, argv);
  } catch(std::exception &ex) {
    std::cerr << std::endl << "Failed to execute the archive command:\n\n" << ex.what() << std::endl;
    return 1;
  }

  return rc;
}

//------------------------------------------------------------------------------
// executeCommand
//------------------------------------------------------------------------------
int CTADirCmd::executeCommand(const int argc, char **argv)  {
  XrdCl::FileSystem fs(XrdCl::URL("localhost"));
  XrdCl::DirectoryList *response;
  XrdCl::XRootDStatus status = fs.DirList("/", XrdCl::DirListFlags::None, response);
  if(status.IsOK()) {
    for(XrdCl::DirectoryList::ConstIterator it=response->Begin(); it!=response->End(); it++) {
      std::cout << (*it)->GetName() << std::endl;
    }
  }
  else {
    std::cout << "Query error" << std::endl;
  }
  return 0;
}

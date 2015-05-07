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

#include "CTACmd.hpp"

#include "XrdCl/XrdClFileSystem.hh"
#include "XrdOuc/XrdOucString.hh"

#include <getopt.h>
#include <iostream>
#include <string.h>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
CTACmd::CTACmd() throw(): m_programName("CTA_cmd") {
}

//------------------------------------------------------------------------------
// usage
//------------------------------------------------------------------------------
void CTACmd::usage(std::ostream &os) const throw() {
  os <<
    "Usage:\n"
    "\t" << m_programName << " archive <source_file1> [<source_file2> [<source_file3> [...]]] <destination_path>\n"
    "\t" << m_programName << " lsarchivejobs [<tape_pool>]\n"
    "\t" << m_programName << " retrieve <source_file1> [<source_file2> [<source_file3> [...]]] <destination_path>\n"
    "\t" << m_programName << " lsretrievejobs [<vid>]\n"
    "\t" << m_programName << " chdirclass <directory_name> <storage_class_name>\n"
    "\t" << m_programName << " cldirclass <directory_name>\n"
    "\t" << m_programName << " getdirclass <directory_name>\n"
    "\t" << m_programName << " mkclass <storage_class_name> <number_of_tape_copies> <\"comment\">\n"
    "\t" << m_programName << " rmclass <storage_class_name>\n"
    "\t" << m_programName << " lsclass\n"
    "\t" << m_programName << " mkdir <directory_name>\n"
    "\t" << m_programName << " rmdir <directory_name>\n"
    "\t" << m_programName << " ls <directory_name>\n"
    "\t" << m_programName << " mkpool <tapepool_name> <\"comment\">\n"
    "\t" << m_programName << " rmpool <tapepool_name>\n"
    "\t" << m_programName << " lspool\n"
    "\t" << m_programName << " mkroute <storage_class_name> <copy_number> <tapepool_name> <\"comment\">\n"
    "\t" << m_programName << " rmroute <storage_class_name> <copy_number>\n"
    "\t" << m_programName << " lsroute\n"    
    "\t" << m_programName << " mkllib <logical_library_name> <\"comment\">\n"
    "\t" << m_programName << " rmllib <logical_library_name>\n"
    "\t" << m_programName << " lsllib\n"    
    "\t" << m_programName << " mktape <vid> <logical_library_name> <tapepool_name> <capacity_in_bytes> <\"comment\">\n"
    "\t" << m_programName << " rmtape <vid>\n"
    "\t" << m_programName << " lstape\n"
    "\t" << m_programName << " mkadminuser <uid> <gid>\n"
    "\t" << m_programName << " rmadminuser <uid> <gid>\n"
    "\t" << m_programName << " lsadminuser\n"
    "\t" << m_programName << " mkadminhost <host_name>\n"
    "\t" << m_programName << " rmadminhost <host_name>\n"
    "\t" << m_programName << " lsadminhost\n";
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int CTACmd::main(const int argc, char **argv) throw() {
  if(argc < 2) {
    usage(std::cerr);
    return 1;
  }
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
int CTACmd::executeCommand(const int argc, char **argv)  {
  
  XrdCl::FileSystem fs(XrdCl::URL("localhost"));
  std::string queryString = "/";
  queryString += argv[1];
  queryString += "?";
  if(argc > 2) {  
    for(int i=2; i<argc-1; i++) {
      queryString += argv[i];
      queryString += "+";
    }  
    queryString += argv[argc-1];
  }        
  XrdCl::Buffer arg;
  arg.FromString(queryString.c_str());
  XrdCl::Buffer* response = 0;
  XrdCl::XRootDStatus status = fs.Query(XrdCl::QueryCode::Opaque, arg, response);
  if(status.IsOK())
  {
    std::cout << response->GetBuffer() << std::endl;
  }
  else
  {
    std::cout << "Query error" << std::endl;
  }
  delete response;
  return 0;
}

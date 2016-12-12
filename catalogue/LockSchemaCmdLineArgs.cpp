/*
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

#include "catalogue/LockSchemaCmdLineArgs.hpp"
#include "common/exception/UserError.hpp"

#include <getopt.h>
#include <ostream>

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
LockSchemaCmdLineArgs::LockSchemaCmdLineArgs(const int argc, char *const *const argv):
  help(false) {

  static struct option longopts[] = {
    {"help", 0, NULL, 'h'},
    {NULL  , 0, NULL, 0}
  };

  // Prevent getopt() from printing an error message if it does not recognize
  // an option character
  opterr = 0;

  int opt = 0;
  while((opt = getopt_long(argc, argv, ":h", longopts, NULL)) != -1) {
    switch(opt) {
    case 'h':
      help = true;
      break;
    case ':': // Missing parameter
      {
        exception::UserError ex;
        ex.getMessage() << "The -" << (char)opt << " option requires a parameter";
        throw ex;
      }
    case '?': // Unknown option
      {
        exception::UserError ex;
        if(0 == optopt) {
          ex.getMessage() << "Unknown command-line option";
        } else {
          ex.getMessage() << "Unknown command-line option: -" << (char)optopt;
        }
        throw ex;
      }
    default:
      {
        exception::UserError ex;
        ex.getMessage() <<
          "getopt_long returned the following unknown value: 0x" <<
          std::hex << (int)opt;
        throw ex;
      }
    } // switch(opt)
  } // while getopt_long()

  // There is no need to continue parsing when the help option is set
  if(help) {
    return;
  }

  // Calculate the number of non-option ARGV-elements
  const int nbArgs = argc - optind;

  // Check the number of arguments
  if(nbArgs != 1) {
    exception::UserError ex;
    ex.getMessage() << "Wrong number of command-line arguments: excepted=1 actual=" << (argc - 1);
    throw ex;
  }

  dbConfigPath = argv[optind];
}

//------------------------------------------------------------------------------
// printUsage
//------------------------------------------------------------------------------
void LockSchemaCmdLineArgs::printUsage(std::ostream &os) {
  os <<
    "Usage:" << std::endl <<
    "    cta-catalogue-schema-lock databaseConnectionFile" << std::endl <<
    "Where:" << std::endl <<
    "    databaseConnectionFile" << std::endl <<
    "        The path to the file containing the connection details of the CTA" << std::endl <<
    "        catalogue database" << std::endl <<
    "Options:" << std::endl <<
    "    -h,--help" << std::endl <<
    "        Prints this usage message" << std::endl;
}

} // namespace catalogue
} // namespace cta

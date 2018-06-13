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

#include "common/log/log.hpp"
#include "common/log/SyslogLogger.hpp"
#include "mediachanger/acs/Constants.hpp"
#include "AcsDaemon.hpp"
#include "AcsdCmdLine.hpp"
#include "mediachanger/reactor/ZMQReactor.hpp"
#include "AcsdConfiguration.hpp"
#include "common/utils/utils.hpp"
#include "common/exception/Exception.hpp"
#include <iostream>


//------------------------------------------------------------------------------
// exceptionThrowingMain
//
// The main() function delegates the bulk of its implementation to this
// exception throwing version.
//
// @param argc The number of command-line arguments.
// @param argv The command-line arguments.
//------------------------------------------------------------------------------
static int exceptionThrowingMain(cta::log::Logger &log,const int argc,char **const argv);

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(const int argc, char **const argv) {
  using namespace cta;

    const std::string shortHostName = utils::getShortHostname();
    cta::log::SyslogLogger logger(shortHostName, "cta-acsd", log::DEBUG);
  try {
    logger(LOG_INFO, "started ACSD in CTA");
  } catch(cta::exception::Exception &ex) {
    std::cerr <<
      "Failed to instantiate object representing CTA logging system: " <<
      ex.getMessage().str() << std::endl;
    return 1;
  } 
  int programRc = 1; 
  try {
    programRc = exceptionThrowingMain(logger, argc, argv) ;
  } catch(cta::exception::Exception &ex) {
    std::list<log::Param> params = {
      log::Param("message", ex.getMessage().str())};
    logger(LOG_ERR, "Caught an unexpected CTA exception", params);
  } catch(std::exception &se) {
    std::list<log::Param> params = {log::Param("what", se.what())};
    logger(LOG_ERR, "Caught an unexpected standard exception", params);
  } catch(...) {
    logger(LOG_ERR, "Caught an unexpected and unknown exception");
  }

  return programRc;
}

//------------------------------------------------------------------------------
// exceptionThrowingMain
//------------------------------------------------------------------------------
static int exceptionThrowingMain(cta::log::Logger &log, const int argc, char **const argv) {
  

 cta::mediachanger::reactor::ZMQReactor reactor(log);

 const cta::mediachanger::acs::daemon::AcsdConfiguration config = cta::mediachanger::acs::daemon::AcsdConfiguration::createFromCtaConf("/etc/cta/cta-acsd.conf",log);

 
  // Create the main acsd object
  cta::mediachanger::acs::daemon::AcsDaemon daemon(
    argc,
    argv,
    log,
    reactor,
    config);

  // Run the acsd daemon
  return daemon.main();

return 0;
}

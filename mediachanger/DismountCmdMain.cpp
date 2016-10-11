/******************************************************************************
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
 
#include "common/exception/Exception.hpp"
#include "common/utils/utils.hpp"
#include "mediachanger/AcsProxyZmq.hpp"
#include "mediachanger/DismountCmd.hpp"
#include "mediachanger/DismountCmdLine.hpp"
#include "mediachanger/RmcProxyTcpIp.hpp"
#include "mediachanger/MmcProxyNotSupported.hpp"
#include "mediachanger/SmartZmqContext.hpp"

#include <exception>
#include <google/protobuf/stubs/common.h>
#include <iostream>
#include <zmq.h>

/**
 * An exception throwing version of main().
 *
 * @param argc The number of command-line arguments including the program name.
 * @param argv The command-line arguments.
 * @return The exit value of the program.
 */
static int exceptionThrowingMain(const int argc, char *const *const argv);

/**
 * Instantiates a ZMQ context.
 *
 * @param sizeOfIOThreadPoolForZMQ The size of the thread pool used to perform
 * IO.  This is usually 1 thread.
 * @return A pointer to the newly created ZMQ context.
 */
static void *instantiateZmqContext(const int sizeOfIOThreadPoolForZMQ);

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(const int argc, char *const *const argv) {
  using namespace cta;

  std::string errorMessage;

  try {
    const int rc = exceptionThrowingMain(argc, argv);
    google::protobuf::ShutdownProtobufLibrary();
    return rc;
  } catch(cta::exception::Exception &ex) {
    errorMessage = ex.getMessage().str();
  } catch(std::exception &se) {
    errorMessage = se.what();
  } catch(...) {
    errorMessage = "An unknown exception was thrown";
  }

  // Reaching this point means the command has failed, ane exception was throw
  // and errorMessage has been set accordingly

  std::cerr << "Aborting: " << errorMessage << std::endl;
  google::protobuf::ShutdownProtobufLibrary();
  return 1;
}


//------------------------------------------------------------------------------
// exceptionThrowingMain
//------------------------------------------------------------------------------
static int exceptionThrowingMain(const int argc, char *const *const argv) {
  using namespace cta;

  const int sizeOfIOThreadPoolForZMQ = 1;
  mediachanger::SmartZmqContext zmqContext(instantiateZmqContext(
    sizeOfIOThreadPoolForZMQ));
  mediachanger::AcsProxyZmq acs(mediachanger::ACS_PORT, zmqContext.get());

  mediachanger::MmcProxyNotSupported mmc;

  const unsigned short rmcPort = mediachanger::RMC_PORT;

  const unsigned int rmcMaxRqstAttempts = mediachanger::RMC_MAXRQSTATTEMPTS;

  // The network timeout of rmc communications should be several minutes due
  // to the time it takes to mount and unmount tapes
  const int rmcNetTimeout = 600; // Timeout in seconds

  mediachanger::RmcProxyTcpIp rmc(rmcPort, rmcNetTimeout, rmcMaxRqstAttempts);

  mediachanger::MediaChangerFacade mc(acs, mmc, rmc);
  
  mediachanger::DismountCmd cmd(std::cin, std::cout, std::cerr, mc);

  return cmd.exceptionThrowingMain(argc, argv);
}

//------------------------------------------------------------------------------
// instantiateZmqContext
//------------------------------------------------------------------------------
static void *instantiateZmqContext(const int sizeOfIOThreadPoolForZMQ) {
  using namespace cta;
  void *const zmqContext = zmq_init(sizeOfIOThreadPoolForZMQ);
  if(NULL == zmqContext) {
    const std::string message = utils::errnoToString(errno);
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to instantiate ZMQ context: " << message;
    throw ex;
  }
  return zmqContext;
}

/******************************************************************************
 *                      TestMTServer.cpp
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
 * @(#)$RCSfile: TestMTServer.cpp,v $ $Revision: 1.1 $ $Release$ $Date: 2008/09/12 16:54:21 $ $Author: itglp $
 *
 * Base class for a multithreaded client for stress tests
 *
 * @author Giuseppe Lo Presti
 *****************************************************************************/


// Include Files
#include "castor/exception/Exception.hpp"
#include "castor/BaseObject.hpp"
#include "castor/Services.hpp"
//#include "castor/db/DbParamsSvc.hpp"
#include "castor/dlf/Dlf.hpp"
#include "castor/server/SignalThreadPool.hpp"

#include "TestMTServer.hpp"
#include "TestThread.hpp"

#include <iostream>

//------------------------------------------------------------------------------
// main method
//------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
  try {
    TestMTServer server;

    // Create a db parameters service and fill with appropriate defaults
    // (only needed when direct access to the NS database is required)
    /*
    castor::IService* s = castor::BaseObject::sharedServices()->service("DbParamsSvc", castor::SVC_DBPARAMSSVC);
    castor::db::DbParamsSvc* params = dynamic_cast<castor::db::DbParamsSvc*>(s);
    if(params == 0) {
      castor::exception::Exception e(serrno);
      e.getMessage() << "Could not instantiate the parameters service";
      throw e;
    }
    params->setSchemaVersion("2_1_9_0");
    params->setDbAccessConfFile("/etc/castor/ORANSCONFIG");
    */

    //server.addThreadPool(
    //  new castor::server::SignalThreadPool("Test", new TestCnsStatThread(), 10, 20, 300));
    // Create a standard thread pool for the testing code.
    // XXX The last argument is a hack to make all threads start immediately as opposed
    // to the default behavior of Castor daemons whereby only one thread starts
    // at startup and the others follow after the first timeout.
    server.addThreadPool(
      new castor::server::SignalThreadPool("Test", new TestThread(), 1, 5, 100));
    server.setForeground(true);
    server.parseCommandLine(argc, argv);
    server.start();
  }
  catch (castor::exception::Exception e) {
    std::cerr << "Caught castor exception : "
              << sstrerror(e.code()) << std::endl
              << e.getMessage().str() << std::endl;
  }
  catch (...) {
    std::cerr << "Caught general exception!" << std::endl;
  }

  return 0;
}


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
TestMTServer::TestMTServer() :
  castor::server::BaseDaemon("MTTest") {
  
  // Initializes the DLF logging
  castor::dlf::Message nomsgs[] = {{-1, ""}};
  dlfInit(nomsgs);
}

//-----------------------------------------------------------------------------
// help
//-----------------------------------------------------------------------------
void TestMTServer::help(std::string programName)
{
  std::cout << "Usage: " << programName << " [options]\n"
    "\n"
    "where options can be:\n"
    "\n"
    "\t--Tthreads n            or -T n       \tRun <n> threads (default is 5)\n"
    "\t--metrics               or -m         \tEnable metrics collection\n"
    "\t--help                  or -h         \tPrint this help and exit\n"
    "\n"
    "Comments to: Castor.Support@cern.ch\n";
}

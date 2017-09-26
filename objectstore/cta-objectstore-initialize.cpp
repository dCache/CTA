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

/**
 * This program will create a VFS backend for the object store and populate
 * it with the minimum elements (the root entry). The program will then print out
 * the path the backend store and exit
 */

#include "BackendVFS.hpp"
#include "BackendFactory.hpp"
#include "RootEntry.hpp"
#include "Agent.hpp"
#include "AgentReference.hpp"
#include "common/log/StdoutLogger.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char ** argv) {
  try {
    std::unique_ptr<cta::objectstore::Backend> be;
    if (1 == argc) {
      be.reset(new cta::objectstore::BackendVFS);
      
    } else if (2 == argc) {
      be.reset(cta::objectstore::BackendFactory::createBackend(argv[1]).release());
    } else {
      throw std::runtime_error("Wrong number of arguments: expected 0 or 1");
    }
    // If the backend is a VFS, make sure we don't delete it on exit.
    // If not, nevermind.
    try {
      dynamic_cast<cta::objectstore::BackendVFS &>(*be).noDeleteOnExit();
    } catch (std::bad_cast &){}
    cta::objectstore::RootEntry re(*be);
    re.initialize();
    re.insert();
    cta::objectstore::ScopedExclusiveLock rel(re);
    re.fetch();
    cta::log::StdoutLogger logger("cta-objectstore-initialize");
    cta::objectstore::AgentReference agr("cta-objectstore-initialize", logger);
    cta::objectstore::Agent ag(agr.getAgentAddress(), *be);
    ag.initialize();
    cta::objectstore::EntryLogSerDeser el("user0", "systemhost", time(NULL));
    re.addOrGetAgentRegisterPointerAndCommit(agr,el);
    rel.release();
    ag.insertAndRegisterSelf();
    rel.lock(re);
    re.fetch();
    re.addOrGetDriveRegisterPointerAndCommit(agr, el);
    re.addOrGetSchedulerGlobalLockAndCommit(agr,el);
    rel.release();
    std::cout << "New object store path: " << be->getParams()->toURL() << std::endl;
  } catch (std::exception & e) {
    std::cerr << "Failed to initialise the root entry in a new VFS backend store"
        << std::endl << e.what() << std::endl;
  }
}
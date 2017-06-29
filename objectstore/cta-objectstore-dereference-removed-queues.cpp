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
 * This program will make sure every queue listed in the root entry does exist and 
 * will remove reference for the ones that do not. This utility was created to quickly
 * unblock tape servers after changing the ArchiveQueue schema during development.
 */

#include "BackendFactory.hpp"
#include "BackendVFS.hpp"
#include "Agent.hpp"
#include "RootEntry.hpp"
#include "AgentRegister.hpp"
#include <iostream>
#include <stdexcept>

int main(int argc, char ** argv) {
  try {
    std::unique_ptr<cta::objectstore::Backend> be;
    if (2 == argc) {
      be.reset(cta::objectstore::BackendFactory::createBackend(argv[1]).release());
    } else {
      throw std::runtime_error("Wrong number of arguments: expected 1");
    }
    // If the backend is a VFS, make sure we don't delete it on exit.
    // If not, nevermind.
    try {
      dynamic_cast<cta::objectstore::BackendVFS &>(*be).noDeleteOnExit();
    } catch (std::bad_cast &){}
    std::cout << "Object store path: " << be->getParams()->toURL() << std::endl;
    // Open the root entry RW
    cta::objectstore::RootEntry re(*be);
    cta::objectstore::ScopedExclusiveLock rel(re);
    re.fetch();
    std::list<std::string> missingArchiveQueues, missingRetrieveQueues;
    for (auto & aq: re.dumpArchiveQueues()) {
      if (!be->exists(aq.address)) {
        missingArchiveQueues.emplace_back(aq.tapePool);
        std::cout << "The archive queue for tape pool " << aq.tapePool << " at address " << aq.address
            << " is missing and will be dereferenced." << std::endl;
      }
    }
    for (auto & rq: re.dumpRetrieveQueues()) {
      if (!be->exists(rq.address)) {
        missingRetrieveQueues.emplace_back(rq.vid);
        std::cout << "The retrieve queue for vid " << rq.vid << " at address " << rq.address
            << " is missing and will be dereferenced." << std::endl;
      }
    }
    // Actually do the job
    for (auto & tp: missingArchiveQueues) {
      re.removeMissingArchiveQueueReference(tp);
      std::cout << "Archive queue for tape pool " << tp << "dereferenced." << std::endl;
    }
    for (auto & vid: missingRetrieveQueues) {
      re.removeMissingRetrieveQueueReference(vid);
      std::cout << "Retrieve queue for vid " << vid << "dereferenced." << std::endl;
    }
    if (missingArchiveQueues.size() || missingRetrieveQueues.size()) {
      re.commit();
      std::cout << "Root entry committed." << std::endl;
    } else {
      std::cout << "Nothing to clean up from root entry." << std::endl;
    }
  } catch (std::exception & e) {
    std::cerr << "Failed to cleanup root entry: "
        << std::endl << e.what() << std::endl;
  }
}
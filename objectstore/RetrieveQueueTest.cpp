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

#include <gtest/gtest.h>
#include "RetrieveQueue.hpp"
#include "BackendVFS.hpp"
#include "Agent.hpp"

namespace unitTests {
  
TEST(ObjectStore, RetrieveQueueBasicAccess) {
  cta::objectstore::BackendVFS be;
  cta::objectstore::Agent agent(be);
  agent.generateName("unitTest");
  std::string retrieveQueueAddress = agent.nextId("RetrieveQueue");
  { 
    // Try to create the tape entry
    cta::objectstore::RetrieveQueue rq(retrieveQueueAddress, be);
    cta::common::dataStructures::EntryLog el;
    el.user=cta::common::dataStructures::UserIdentity("user0", "group0");
    el.host="unittesthost";
    el.time=time(NULL);
    rq.initialize("V12345", "LIB0", el);
    rq.insert();
  }
  {
    // Try to read back and dump the tape
    cta::objectstore::RetrieveQueue rq(retrieveQueueAddress, be);
    ASSERT_THROW(rq.fetch(), cta::exception::Exception);
    cta::objectstore::ScopedSharedLock lock(rq);
    ASSERT_NO_THROW(rq.fetch());
    rq.dump();
  }
  // Delete the root entry
  cta::objectstore::RetrieveQueue rq(retrieveQueueAddress, be);
  cta::objectstore::ScopedExclusiveLock lock(rq);
  rq.fetch();
  rq.removeIfEmpty();
  ASSERT_FALSE(rq.exists());
}
}
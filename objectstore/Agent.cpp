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

#include "Agent.hpp"
#include "AgentRegister.hpp"
#include "RootEntry.hpp"
#include "GenericObject.hpp"
#include "common/exception/Errnum.hpp"
#include "ProtocolBuffersAlgorithms.hpp"
#include <string>
#include <sstream>
#include <ctime>
#include <cxxabi.h>
#include <google/protobuf/util/json_util.h>

cta::objectstore::Agent::Agent(GenericObject& go):
  ObjectOps<serializers::Agent, serializers::Agent_t>(go.objectStore()) {
  // Here we transplant the generic object into the new object
  go.transplantHeader(*this);
  // And interpret the header.
  getPayloadFromHeader();
}

cta::objectstore::Agent::Agent(const std::string & name, Backend & os): 
  ObjectOps<serializers::Agent, serializers::Agent_t>(os, name), m_nextId(0) {}

void cta::objectstore::Agent::initialize() {
  ObjectOps<serializers::Agent, serializers::Agent_t>::initialize();
  m_payload.set_heartbeat(0);
  m_payload.set_timeout_us(60*1000*1000);
  m_payload.set_description("");
  m_payloadInterpreted = true;
}

void cta::objectstore::Agent::insertAndRegisterSelf() {
  // We suppose initialize was already called, and that the agent name
  // is set.
  // We need to get hold of the agent register, which we suppose is available
  RootEntry re(m_objectStore);
  ScopedSharedLock reLock(re);
  re.fetch();
  AgentRegister ar(re.getAgentRegisterAddress(), m_objectStore);
  reLock.release();
  // Then we should first create a pointer to our agent
  ScopedExclusiveLock arLock(ar);
  ar.fetch();
  ar.addAgent(getAddressIfSet());
  ar.commit();
  // Set the agent register as owner and backup owner
  setBackupOwner(ar.getAddressIfSet());
  setOwner(ar.getAddressIfSet());
  // Create the agent
  insert();
  // And release the agent register's lock
  arLock.release();
}

void cta::objectstore::Agent::removeAndUnregisterSelf() {
  // Check that we own the proper lock
  checkPayloadWritable();
  // Check that we are not empty
  if (!isEmpty()) {
    std::list<std::string> ownershipList = getOwnershipList();
    std::stringstream exSs;
    exSs << "In Agent::deleteAndUnregisterSelf: agent still owns objects. Here's the list:";
    for(auto i=ownershipList.begin(); i!=ownershipList.end(); i++) {
      exSs << " " << *i;
    }
    throw AgentStillOwnsObjects(exSs.str());
  }
  // First delete ourselves
  remove();
  // Then we remove the dangling pointer about ourselves in the agent register.
  // We need to get hold of the agent register, which we suppose is available
  RootEntry re(m_objectStore);
  ScopedSharedLock reLock(re);
  re.fetch();
  AgentRegister ar(re.getAgentRegisterAddress(), m_objectStore);
  reLock.release();
  // Then we should first create a pointer to our agent
  ScopedExclusiveLock arLock(ar);
  ar.fetch();
  ar.removeAgent(getAddressIfSet());
  ar.commit();
  arLock.release();
}

bool cta::objectstore::Agent::isEmpty() {
  checkPayloadReadable();
  if (m_payload.ownedobjects_size())
    return false;
  return true;
}

void cta::objectstore::Agent::garbageCollect(const std::string& presumedOwner, AgentReference & agentReference, log::LogContext & lc,
    cta::catalogue::Catalogue & catalogue) {
  checkPayloadWritable();
  // We are here limited to checking the presumed owner and mark the agent as 
  // untracked in the agent register in case of match, else we do nothing
  if (m_header.owner() == presumedOwner) {
    // We need to get hold of the agent register, which we suppose is available
    RootEntry re(m_objectStore);
    ScopedSharedLock reLock(re);
    re.fetch();
    AgentRegister ar(re.getAgentRegisterAddress(), m_objectStore);
    reLock.release();
    // Then we should first create a pointer to our agent
    ScopedExclusiveLock arLock(ar);
    ar.fetch();
    ar.untrackAgent(getAddressIfSet());
    ar.commit();
    arLock.release();
    // We now mark ourselves as owned by the agent register
    setOwner(ar.getAddressIfSet());
    commit();
  }
}



/*void cta::objectstore::Agent::create() {
  if (!m_setupDone)
    throw SetupNotDone("In Agent::create(): setup() not yet done");
  RootEntry re(m_objectStore);
  AgentRegister ar(re.allocateOrGetAgentRegister(*this), m_objectStore);
  ar.addIntendedElement(selfName(), *this);
  serializers::Agent as;
  as.set_heartbeat(0);
  writeChild(selfName(), as);
  ar.upgradeIntendedElementToActual(selfName(), *this);
  m_creationDone = true;
}*/

void cta::objectstore::Agent::addToOwnership(std::string name) {
  checkPayloadWritable();
  std::string * owned = m_payload.mutable_ownedobjects()->Add();
  *owned = name;
}

void cta::objectstore::Agent::removeFromOwnership(std::string name) {
  checkPayloadWritable();
  serializers::removeString(m_payload.mutable_ownedobjects(), name);
  /*
  bool found;
  do {
    found = false;
    for (int i=0; i<m_payload.mutable_ownedobjects()->size(); i++) {
      if (name == *m_payload.mutable_ownedobjects(i)) {
        found = true;
        m_payload.mutable_ownedobjects()->SwapElements(i, m_payload.mutable_ownedobjects()->size()-1);
        m_payload.mutable_ownedobjects()->RemoveLast();
        break;
      }
    }
  } while (found);*/
}

std::list<std::string> 
  cta::objectstore::Agent::getOwnershipList() {
  checkPayloadReadable();
  std::list<std::string> ret;
  for (int i=0; i<m_payload.ownedobjects_size(); i++) {
    ret.push_back(m_payload.ownedobjects(i));
  }
  return ret;
}

void cta::objectstore::Agent::bumpHeartbeat() {
  checkPayloadWritable();
  auto heartbeat=m_payload.heartbeat()+1;
  m_payload.set_heartbeat(heartbeat);
}

uint64_t cta::objectstore::Agent::getHeartbeatCount() {
  checkPayloadReadable();
  return m_payload.heartbeat();
}

double cta::objectstore::Agent::getTimeout() {
  checkPayloadReadable();
  return 0.000001 * m_payload.timeout_us();
}

void cta::objectstore::Agent::setTimeout_us(uint64_t timeout) {
  checkPayloadWritable();
  m_payload.set_timeout_us(timeout);
}

std::string cta::objectstore::Agent::dump() {
  checkPayloadReadable();
  google::protobuf::util::JsonPrintOptions options;
  options.add_whitespace = true;
  options.always_print_primitive_fields = true;
  std::string headerDump;
  google::protobuf::util::MessageToJsonString(m_payload, &headerDump, options);
  return headerDump;
}



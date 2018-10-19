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

#include "RepackQueue.hpp"
#include "GenericObject.hpp"
#include <google/protobuf/util/json_util.h>

namespace cta { namespace objectstore {

//------------------------------------------------------------------------------
// RepackQueue::RepackQueue()
//------------------------------------------------------------------------------
RepackQueue::RepackQueue(const std::string& address, Backend& os): 
  ObjectOps<serializers::RepackQueue, serializers::RepackQueue_t>(os, address) {}

//------------------------------------------------------------------------------
// RepackQueue::RepackQueue()
//------------------------------------------------------------------------------
RepackQueue::RepackQueue(Backend& os):
  ObjectOps<serializers::RepackQueue, serializers::RepackQueue_t>(os) { }

//------------------------------------------------------------------------------
// RepackQueue::RepackQueue()
//------------------------------------------------------------------------------
RepackQueue::RepackQueue(GenericObject& go):
  ObjectOps<serializers::RepackQueue, serializers::RepackQueue_t>(go.objectStore()) {
  // Here we transplant the generic object into the new object
  go.transplantHeader(*this);
  // And interpret the header.
  getPayloadFromHeader();
}

//------------------------------------------------------------------------------
// RepackQueue::initialize()
//------------------------------------------------------------------------------
void RepackQueue::initialize() {
  // Setup underlying object
  ObjectOps<serializers::RepackQueue, serializers::RepackQueue_t>::initialize();
  // Nothing more to do.
  m_payloadInterpreted = true;
}

//------------------------------------------------------------------------------
// RepackQueue::addRequestsAndCommit()
//------------------------------------------------------------------------------
void RepackQueue::addRequestsAndCommit(const std::list<std::string>& requestAddresses, log::LogContext& lc) {
  checkPayloadWritable();
  // This queue does not need to be sharded
  for (auto &address: requestAddresses) m_payload.add_repackrequestpointers()->set_address(address);
  commit();
}

//------------------------------------------------------------------------------
// RepackQueue::addRequestsIfNecessaryAndCommit()
//------------------------------------------------------------------------------
void RepackQueue::addRequestsIfNecessaryAndCommit(const std::list<std::string>& requestAddresses, log::LogContext& lc) {
  checkPayloadWritable();
  std::set<std::string> existingAddresses;
  for (auto &a: m_payload.repackrequestpointers()) existingAddresses.insert(a.address());
  bool didAdd = false;
  for (auto &a: requestAddresses)
    if (!existingAddresses.count(a)) {
      m_payload.add_repackrequestpointers()->set_address(a);
      didAdd = true;
    }
  if (didAdd) commit();
}

//------------------------------------------------------------------------------
// RepackQueue::garbageCollect()
//------------------------------------------------------------------------------
void RepackQueue::garbageCollect(const std::string& presumedOwner, AgentReference& agentReference, log::LogContext& lc,
    cta::catalogue::Catalogue& catalogue) {
  throw exception::Exception("In RepackQueue::garbageCollect(): not implemented.");
}

//------------------------------------------------------------------------------
// RepackQueue::removeRequestsAndCommit()
//------------------------------------------------------------------------------
void RepackQueue::removeRequestsAndCommit(const std::list<std::string>& requestsAddresses) {
  checkPayloadWritable();
  std::set<std::string> requestsToRemove (requestsAddresses.begin(), requestsAddresses.end());
  bool didRemove=false;
  std::list<std::string> newQueue;
  for (auto &a: m_payload.repackrequestpointers()) {
    if (requestsToRemove.count(a.address())) {
      didRemove=true;
    } else {
      newQueue.emplace_back(a.address());
    }
  }
  if (didRemove) {
    m_payload.mutable_repackrequestpointers()->Clear();
    for (auto &a: newQueue) m_payload.add_repackrequestpointers()->set_address(a);
    commit();
  }
}

//------------------------------------------------------------------------------
// RepackQueue::getRequestsSummary()
//------------------------------------------------------------------------------
auto RepackQueue::getRequestsSummary() -> RequestsSummary {
  checkPayloadReadable();
  RequestsSummary ret;
  ret.requests = m_payload.repackrequestpointers().size();
  return ret;
}

//------------------------------------------------------------------------------
// RepackQueue::isEmpty()
//------------------------------------------------------------------------------
bool RepackQueue::isEmpty() {
  checkPayloadReadable();
  return m_payload.repackrequestpointers().size();
}

//------------------------------------------------------------------------------
// RepackQueue::dump()
//------------------------------------------------------------------------------
std::string RepackQueue::dump() {
  checkPayloadReadable();
  google::protobuf::util::JsonPrintOptions options;
  options.add_whitespace = true;
  options.always_print_primitive_fields = true;
  std::string headerDump;
  google::protobuf::util::MessageToJsonString(m_payload, &headerDump, options);
  return headerDump;
}



}} // namespace cta::objectstore.

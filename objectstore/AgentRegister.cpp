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

#include "AgentRegister.hpp"
#include "ProtocolBuffersAlgorithms.hpp"
#include "GenericObject.hpp"
#include <json-c/json_object.h>

cta::objectstore::AgentRegister::AgentRegister(Backend & os):
ObjectOps<serializers::AgentRegister>(os) {}

cta::objectstore::AgentRegister::AgentRegister(GenericObject& go): 
  ObjectOps<serializers::AgentRegister>(go.objectStore()) {
  // Here we transplant the generic object into the new object
  go.transplantHeader(*this);
  // And interpret the header.
  getPayloadFromHeader();
}

cta::objectstore::AgentRegister::AgentRegister(const std::string & name, Backend & os):
ObjectOps<serializers::AgentRegister>(os, name) {}

void cta::objectstore::AgentRegister::initialize() {
  ObjectOps<serializers::AgentRegister>::initialize();
  // There is nothing to do for the payload.
  m_payloadInterpreted = true;
}

void cta::objectstore::AgentRegister::garbageCollect(const std::string &presumedOwner) {
  checkPayloadWritable();
  if (!isEmpty()) {
    throw (NotEmpty("Trying to garbage collect a non-empty AgentRegister: internal error"));
  }
  remove();
}


bool cta::objectstore::AgentRegister::isEmpty() {
  checkPayloadReadable();
  if (m_payload.untrackedagents_size())
    return false;
  if (m_payload.agents_size())
    return false;
  return true;
}


void cta::objectstore::AgentRegister::addAgent (std::string name) {
  checkPayloadWritable();
  m_payload.add_agents(name);
  m_payload.add_untrackedagents(name);
}

void cta::objectstore::AgentRegister::removeAgent (const std::string  & name) {
  checkPayloadReadable();
  serializers::removeString(m_payload.mutable_agents(), name);
  serializers::removeString(m_payload.mutable_untrackedagents(), name);
}


void cta::objectstore::AgentRegister::trackAgent(std::string name) {
  checkPayloadWritable();
  // Check that the agent is present (next statement throws an exception
  // if the agent is not known)
  serializers::findString(m_payload.mutable_agents(), name);
  serializers::removeString(m_payload.mutable_untrackedagents(), name);
}

void cta::objectstore::AgentRegister::untrackAgent(std::string name) {
  checkPayloadWritable();
  // Check that the agent is present (next statement throws an exception
  // if the agent is not known)
  serializers::findString(m_payload.mutable_agents(), name);
  // Check that the agent is not already in the list to prevent double
  // inserts
  try {
    serializers::findString(m_payload.mutable_untrackedagents(), name);
  } catch (serializers::NotFound &) {
    m_payload.add_untrackedagents(name);
  }
}

std::list<std::string> cta::objectstore::AgentRegister::getAgents() {
  std::list<std::string> ret;
  for (int i=0; i<m_payload.agents_size(); i++) {
    ret.push_back(m_payload.agents(i));
  }
  return ret;
}

std::list<std::string> cta::objectstore::AgentRegister::getUntrackedAgents() {
  std::list<std::string> ret;
  for (int i=0; i<m_payload.untrackedagents_size(); i++) {
    ret.push_back(m_payload.untrackedagents(i));
  }
  return ret;
}

std::string cta::objectstore::AgentRegister::dump() {
  checkPayloadReadable();
  std::stringstream ret;
  ret << "AgentRegister" << std::endl;
  struct json_object * jo;
  jo = json_object_new_object();
  json_object * jal = json_object_new_array();
  for (int i=0; i<m_payload.agents_size(); i++) {
    json_object_array_add(jal, json_object_new_string(m_payload.agents(i).c_str()));
  }
  json_object_object_add(jo, "agents", jal);
  json_object * jual = json_object_new_array();
  for (int i=0; i<m_payload.untrackedagents_size(); i++) {
    json_object_array_add(jual, json_object_new_string(m_payload.untrackedagents(i).c_str()));
  }
  json_object_object_add(jo, "untrackedAgents", jual);
  ret << json_object_to_json_string_ext(jo, JSON_C_TO_STRING_PRETTY) << std::endl;
  json_object_put(jo);
  return ret.str();
}

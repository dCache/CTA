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

#include "common/dataStructures/SecurityIdentity.hpp"
#include "common/exception/Exception.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::common::dataStructures::SecurityIdentity::SecurityIdentity() {  
  m_hostSet = false;
  m_userSet = false;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::common::dataStructures::SecurityIdentity::~SecurityIdentity() throw() {
}

//------------------------------------------------------------------------------
// allFieldsSet
//------------------------------------------------------------------------------
bool cta::common::dataStructures::SecurityIdentity::allFieldsSet() const {
  return m_hostSet
      && m_userSet;
}

//------------------------------------------------------------------------------
// setHost
//------------------------------------------------------------------------------
void cta::common::dataStructures::SecurityIdentity::setHost(const std::string &host) {
  m_host = host;
  m_hostSet = true;
}

//------------------------------------------------------------------------------
// getHost
//------------------------------------------------------------------------------
std::string cta::common::dataStructures::SecurityIdentity::getHost() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the SecurityIdentity have been set!");
  }
  return m_host;
}

//------------------------------------------------------------------------------
// setUser
//------------------------------------------------------------------------------
void cta::common::dataStructures::SecurityIdentity::setUser(const cta::common::dataStructures::UserIdentity &user) {
  m_user = user;
  m_userSet = true;
}

//------------------------------------------------------------------------------
// getUser
//------------------------------------------------------------------------------
cta::common::dataStructures::UserIdentity cta::common::dataStructures::SecurityIdentity::getUser() const {
  if(!allFieldsSet()) {
    throw cta::exception::Exception(std::string(__FUNCTION__)+" Error: not all fields of the SecurityIdentity have been set!");
  }
  return m_user;
}

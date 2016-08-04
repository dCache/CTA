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

#include "Stmt.hpp"

namespace cta {
namespace rdbms {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
Stmt::Stmt(const AutocommitMode autocommitMode): m_autoCommitMode(autocommitMode) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
Stmt::~Stmt() throw() {
}

//------------------------------------------------------------------------------
// bindBool
//------------------------------------------------------------------------------
void Stmt::bindBool(const std::string &paramName, const bool paramValue) {
  bindOptionalBool(paramName, paramValue);
}

//------------------------------------------------------------------------------
// bindOptionalBool
//------------------------------------------------------------------------------
void Stmt::bindOptionalBool(const std::string &paramName, const optional<bool> &paramValue) {
  if(paramValue) {
    bindOptionalUint64(paramName, paramValue.value() ? 1 : 0);
  } else {
    bindOptionalUint64(paramName, nullopt);
  }
}

} // namespace rdbms
} // namespace cta

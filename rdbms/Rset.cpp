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

#include "Rset.hpp"
#include "NullDbValue.hpp"

namespace cta {
namespace rdbms {

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
Rset::~Rset() throw() {
}

//------------------------------------------------------------------------------
// columnString
//------------------------------------------------------------------------------
std::string Rset::columnString(const std::string &colName) const {
  try {
    const optional<std::string> col = columnOptionalString(colName);
    if(col) {
      return col.value();
    } else {
      throw NullDbValue(std::string("Database column ") + colName + " contains a nullptr value");
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// columnUint64
//------------------------------------------------------------------------------
uint64_t Rset::columnUint64(const std::string &colName) const {
  try {
    const optional<uint64_t> col = columnOptionalUint64(colName);
    if(col) {
      return col.value();
    } else {
      throw NullDbValue(std::string("Database column ") + colName + " contains a nullptr value");
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// columnBool
//------------------------------------------------------------------------------
bool Rset::columnBool(const std::string &colName) const {
  try {
    const optional<bool> col = columnOptionalBool(colName);
    if(col) {
      return col.value();
    } else {
      throw NullDbValue(std::string("Database column ") + colName + " contains a nullptr value");
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// columnOptionalBool
//------------------------------------------------------------------------------
optional<bool> Rset::columnOptionalBool(const std::string &colName) const {
  try {
    const auto column = columnOptionalUint64(colName);
    if(column) {
      return optional<bool>(column.value() != 0 ? true : false);
    } else {
      return nullopt;
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

} // namespace rdbms
} // namespace cta

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

#include "catalogue/AutoRollback.hpp"
#include "catalogue/DbConn.hpp"
#include "common/exception/Exception.hpp"

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
AutoRollback::AutoRollback(DbConn *const dbConn): m_dbConn(dbConn) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
AutoRollback::~AutoRollback() {
  try {
    if(NULL != m_dbConn) {
      m_dbConn->rollback();
    }
  } catch(...) {
    // Prevent destructor from throwing
  }
}

//------------------------------------------------------------------------------
// cancel
//------------------------------------------------------------------------------
void AutoRollback::cancel() {
  m_dbConn = NULL;
}

} // namespace catalogue
} // namespace cta

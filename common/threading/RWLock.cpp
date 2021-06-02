/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common/threading/RWLock.hpp"
#include "common/exception/Exception.hpp"
#include "common/utils/utils.hpp"

namespace cta {
namespace threading {
  
//------------------------------------------------------------------------------
//constructor
//------------------------------------------------------------------------------
RWLock::RWLock()  {
  const int rc = pthread_rwlock_init(&m_lock, nullptr);
  if(0 != rc) {
    exception::Exception ex;
    ex.getMessage() << __FUNCTION__ << " failed: Failed to initialise underlying pthread read-write lock: " <<
      utils::errnoToString(rc);
    throw ex;
  }
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RWLock::~RWLock() {
  pthread_rwlock_destroy(&m_lock);
}

//------------------------------------------------------------------------------
// rdlock
//------------------------------------------------------------------------------
void RWLock::rdlock()  {
  const int rc = pthread_rwlock_rdlock(&m_lock);
  if(0 != rc) {
    exception::Exception ex;
    ex.getMessage() << __FUNCTION__ << " failed: Failed to take read lock on underlying pthread read-write lock: "
      << utils::errnoToString(rc);
    throw ex;
  }
}

//------------------------------------------------------------------------------
// wrlock
//------------------------------------------------------------------------------
void RWLock::wrlock()  {
  const int rc = pthread_rwlock_wrlock(&m_lock);
  if(0 != rc) {
    exception::Exception ex;
    ex.getMessage() << __FUNCTION__ << " failed: Failed to take write lock on underlying pthread read-write lock: "
      << utils::errnoToString(rc);
    throw ex;
  }
}

//------------------------------------------------------------------------------
//unlock
//------------------------------------------------------------------------------
void RWLock::unlock()  {
  const int rc = pthread_rwlock_unlock(&m_lock);
  if(0 != rc) {
    exception::Exception ex;
    ex.getMessage() << __FUNCTION__ << " failed: Failed to unlock underlying pthread read-write lock: " <<
      utils::errnoToString(rc);
    throw ex;
  }
}

} // namespace threading
} // namespace cta

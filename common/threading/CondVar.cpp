/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2015-2021 CERN
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

#include "common/exception/Exception.hpp"
#include "common/threading/CondVar.hpp"
#include "common/threading/MutexLocker.hpp"
#include "common/utils/utils.hpp"

namespace cta {
namespace threading {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
CondVar::CondVar() {
  const int initRc = pthread_cond_init(&m_cond, nullptr);
  if(0 != initRc) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: Failed to initialise condition variable");
  }
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CondVar::~CondVar() {
  pthread_cond_destroy(&m_cond);
}

//------------------------------------------------------------------------------
// wait
//------------------------------------------------------------------------------
void CondVar::wait(MutexLocker &locker) {
  if(!locker.m_locked) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: Underlying mutex is not locked.");
  }

  const int waitRc = pthread_cond_wait(&m_cond, &locker.m_mutex.m_mutex);
  if(0 != waitRc) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: pthread_cond_wait failed:" +
      utils::errnoToString(waitRc));
  }
}

//------------------------------------------------------------------------------
// signal
//------------------------------------------------------------------------------
void CondVar::signal() {
  const int signalRc = pthread_cond_signal(&m_cond);
  if(0 != signalRc) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: pthread_cond_signal failed:" +
      utils::errnoToString(signalRc));
  }
}

//------------------------------------------------------------------------------
// broadcast
//------------------------------------------------------------------------------
void CondVar::broadcast() {
  const int broadcastRc = pthread_cond_broadcast(&m_cond);
  if(0 != broadcastRc) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: pthread_cond_broadcast failed:" +
      utils::errnoToString(broadcastRc));
  }
}

} // namespace threading
} // namespace cta

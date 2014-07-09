/******************************************************************************
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "Threading.hpp"
#include <errno.h>
#include <typeinfo>
#include <stdlib.h>
#include <cxxabi.h>
#include "../../../../h/Cthread_api.h"
#include "castor/BaseObject.hpp"

/* Implmentations of the threading primitives */

/* Mutex */

castor::tape::threading::Mutex::Mutex()  {
  pthread_mutexattr_t attr;
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutexattr_init(&attr),
    "Error from pthread_mutexattr_init in castor::tape::threading::Mutex::Mutex()");
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK),
    "Error from pthread_mutexattr_settype in castor::tape::threading::Mutex::Mutex()");
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutex_init(&m_mutex, &attr),
    "Error from pthread_mutex_init in castor::tape::threading::Mutex::Mutex()");
  try {
    castor::exception::Errnum::throwOnReturnedErrno(
      pthread_mutexattr_destroy(&attr),
      "Error from pthread_mutexattr_destroy in castor::tape::threading::Mutex::Mutex()");
  } catch (...) {
    pthread_mutex_destroy(&m_mutex);
    throw;
  }
}

castor::tape::threading::Mutex::~Mutex() {
  pthread_mutex_destroy(&m_mutex);
}

void castor::tape::threading::Mutex::lock()  {
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutex_lock(&m_mutex),
    "Error from pthread_mutex_lock in castor::tape::threading::Mutex::lock()");
}

/* PosixSemaphore */
castor::tape::threading::PosixSemaphore::PosixSemaphore(int initial)
 {
  castor::exception::Errnum::throwOnReturnedErrno(
    sem_init(&m_sem, 0, initial),
    "Error from sem_init in castor::tape::threading::PosixSemaphore::PosixSemaphore()");
}

castor::tape::threading::PosixSemaphore::~PosixSemaphore() {
  /* There is a danger of destroying the semaphore in the consumer
     while the producer is still referring to the object.
     This mutex prevents this from happening. (The release method locks it). */
  MutexLocker ml(&m_mutexPosterProtection);
  sem_destroy(&m_sem);
}

void castor::tape::threading::PosixSemaphore::acquire()
 {
  int ret;
  /* If we receive EINTR, we should just keep trying (signal interruption) */
  while((ret = sem_wait(&m_sem)) && EINTR == errno) {}
  /* If it was not EINTR, it's a failure */
  castor::exception::Errnum::throwOnNonZero(ret,
    "Error from sem_wait in castor::tape::threading::PosixSemaphore::acquire()");
}

bool castor::tape::threading::PosixSemaphore::tryAcquire()
 {
  int ret = sem_trywait(&m_sem);
  if (!ret) return true;
  if (ret && EAGAIN == errno) return false;
  castor::exception::Errnum::throwOnNonZero(ret,
    "Error from sem_trywait in castor::tape::threading::PosixSemaphore::tryAcquire()");
  /* unreacheable, just for compiler happiness */
  return false;
}

void castor::tape::threading::PosixSemaphore::release(int n)
 {
  for (int i=0; i<n; i++) {
    MutexLocker ml(&m_mutexPosterProtection);
    castor::exception::Errnum::throwOnNonZero(sem_post(&m_sem),
      "Error from sem_post in castor::tape::threading::PosixSemaphore::release()");
  }
}

castor::tape::threading::CondVarSemaphore::CondVarSemaphore(int initial)
:
m_value(initial) {
      castor::exception::Errnum::throwOnReturnedErrno(
        pthread_cond_init(&m_cond, NULL),
        "Error from pthread_cond_init in castor::tape::threading::CondVarSemaphore::CondVarSemaphore()");
      castor::exception::Errnum::throwOnReturnedErrno(
        pthread_mutex_init(&m_mutex, NULL),
        "Error from pthread_mutex_init in castor::tape::threading::CondVarSemaphore::CondVarSemaphore()");
    }


castor::tape::threading::CondVarSemaphore::~CondVarSemaphore() {
      /* Barrier protecting the last user */
      pthread_mutex_lock(&m_mutex);
      pthread_mutex_unlock(&m_mutex);
      /* Cleanup */
      pthread_cond_destroy(&m_cond);
      pthread_mutex_destroy(&m_mutex);
    }

void castor::tape::threading::CondVarSemaphore::acquire()
 {
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutex_lock(&m_mutex),
    "Error from pthread_mutex_lock in castor::tape::threading::CondVarSemaphore::acquire()");
  while (m_value <= 0) {
    castor::exception::Errnum::throwOnReturnedErrno(
      pthread_cond_wait(&m_cond, &m_mutex),
      "Error from pthread_cond_wait in castor::tape::threading::CondVarSemaphore::acquire()");
  }
  m_value--;
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutex_unlock(&m_mutex),
    "Error from pthread_mutex_unlock in castor::tape::threading::CondVarSemaphore::acquire()");
}

bool castor::tape::threading::CondVarSemaphore::tryAcquire()
 {
  bool ret;
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutex_lock(&m_mutex),
      "Error from pthread_mutex_lock in castor::tape::threading::CondVarSemaphore::tryAcquire()");
  if (m_value > 0) {
    ret = true;
    m_value--;
  } else {
    ret = false;
  }
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutex_unlock(&m_mutex),
      "Error from pthread_mutex_unlock in castor::tape::threading::CondVarSemaphore::tryAcquire()");
  return ret;
}

void castor::tape::threading::CondVarSemaphore::release(int n)
 {
  for (int i=0; i<n; i++) {
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutex_lock(&m_mutex),
      "Error from pthread_mutex_unlock in castor::tape::threading::CondVarSemaphore::release()");
    m_value++;
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_cond_signal(&m_cond),
      "Error from pthread_cond_signal in castor::tape::threading::CondVarSemaphore::release()");
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_mutex_unlock(&m_mutex),
      "Error from pthread_mutex_unlock in castor::tape::threading::CondVarSemaphore::release()");
  }
}

void castor::tape::threading::Thread::start()
 {
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_create(&m_thread, NULL, pthread_runner, this),
      "Error from pthread_create in castor::tape::threading::Thread::start()");
}

void castor::tape::threading::Thread::wait()
 {
  castor::exception::Errnum::throwOnReturnedErrno(
    pthread_join(m_thread, NULL),
      "Error from pthread_join in castor::tape::threading::Thread::wait()");
  if (m_hadException) {
    std::string w = "Uncaught exception of type \"";
    w += m_type;
    w += "\" in Thread.run():\n>>>>\n";
    w += m_what;
    w += "<<<< End of uncaught exception";
    throw UncaughtExceptionInThread(w);
  }
}

void * castor::tape::threading::Thread::pthread_runner (void * arg) {

  /* static_casting a pointer to and from void* preserves the address. 
   * See https://stackoverflow.com/questions/573294/when-to-use-reinterpret-cast
   */ 
  Thread * _this = static_cast<Thread *>(arg);
   
  // The threading init is needing by many castor components, so better do
  // it all the time (this should not have side effects)
  Cthread_init();
  try {
    _this->run();
  } catch (std::exception & e) {
    _this->m_hadException = true;
    int status = -1;
    char * demangled = abi::__cxa_demangle(typeid(e).name(), NULL, NULL, &status);
    if (!status) {
      _this->m_type += demangled;
    } else {
      _this->m_type = typeid(e).name();
    }
    free(demangled);
    _this->m_what = e.what();
  } catch (...) {
    _this->m_hadException = true;
    _this->m_type = "unknown";
    _this->m_what = "uncaught non-standard exception";
  }
  BaseObject::resetServices();
  return NULL;
}

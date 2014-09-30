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
/* 
 * Author: dcome
 *
 * Created on March 5, 2014, 5:06 PM
 */

#pragma once

#include <queue>
#include <exception>

#include "castor/server/MutexLocker.hpp"
#include "castor/server/Threading.hpp"
#include "castor/server/Semaphores.hpp"

namespace castor {
namespace server {
      
/***
 * This simple class provides a thread-safe blocking queue
 *  
 */
template<class C>
class BlockingQueue {

public:

  typedef typename std::queue<C>::value_type value_type;
  typedef typename std::queue<C>::reference reference;
  typedef typename std::queue<C>::const_reference const_reference;
  typedef struct valueRemainingPair {C value; size_t remaining;} valueRemainingPair;
  
  BlockingQueue(){}
  ~BlockingQueue() {}
  
  /**
   * Copy the concent of e and push into the queue
   * @param e
   */
  void push(const C& e) {
    {
      MutexLocker ml(&m_mutex);
      m_queue.push(e);
    }
    m_sem.release();
  }
  
  /**
   * Return the next value of the queue and remove it
   */
  C pop() {
    m_sem.acquire();
    return popCriticalSection();
  }
  /**
   * Atomically pop the element of the top of the pile AND return it with the 
   * number of remaining elements in the queue 
   * @return a struct holding the popped element (into ret.value) and the number of elements 
   * remaining (into ret.remaining)
   * 
   */
  valueRemainingPair popGetSize () {
    m_sem.acquire();
    valueRemainingPair ret;
    ret.value = popCriticalSection(&ret.remaining);
    return ret;
  }
  
  /**
   * return the number of elements currently in the queue
   */
  size_t size() const { 
    MutexLocker ml(&m_mutex);
    return m_queue.size();
  }
  
private:  
  /** 
   * holds data of the queue 
   */
  std::queue<C> m_queue;
  
  /**
   * Used for blocking a consumer thread as long as the queue is empty 
   */
  Semaphore m_sem;
  
  /**
   * used for locking-operation thus providing thread-safety
   */
  mutable Mutex m_mutex;

  /**
   * Thread and exception safe pop. Optionally atomically extracts the size 
   * of the queue after pop 
   */
  C popCriticalSection(size_t * sz = NULL) {
    MutexLocker ml(&m_mutex);
    C ret = m_queue.front();
    m_queue.pop();
    if (sz)
      *sz = m_queue.size();
    return ret;
  }
  
};

}}

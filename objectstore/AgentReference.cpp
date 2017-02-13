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

#include "AgentReference.hpp"
#include "Agent.hpp"
#include "common/exception/Errnum.hpp"

#include <sstream>
#include <unistd.h>
#include <sys/syscall.h>
#include <iomanip>

namespace cta { namespace objectstore {

AgentReference::AgentReference(const std::string & clientType) {
  m_nextId=0;
  std::stringstream aid;
  // Get time
  time_t now = time(0);
  struct tm localNow;
  localtime_r(&now, &localNow);
  // Get hostname
  char host[200];
  cta::exception::Errnum::throwOnMinusOne(::gethostname(host, sizeof(host)),
    "In AgentId::AgentId:  failed to gethostname");
  // gettid is a safe system call (never fails)
  aid << clientType << "-" << host << "-" << syscall(SYS_gettid) << "-"
    << 1900 + localNow.tm_year
    << std::setfill('0') << std::setw(2) 
    << 1 + localNow.tm_mon
    << std::setw(2) << localNow.tm_mday << "-"
    << std::setw(2) << localNow.tm_hour << ":"
    << std::setw(2) << localNow.tm_min << ":"
    << std::setw(2) << localNow.tm_sec;
  m_agentAddress = aid.str();
  // Initialize the serialization token for queued actions
  m_nextQueueExecutionPromise.reset(new std::promise<void>);
  m_nextQueueExecutionPromise->set_value();
}

std::string AgentReference::getAgentAddress() {
  return m_agentAddress;
}

std::string AgentReference::nextId(const std::string& childType) {
  std::stringstream id;
  id << childType << "-" << m_agentAddress << "-" << m_nextId++;
  return id.str();
}

void AgentReference::addToOwnership(const std::string& objectAddress, objectstore::Backend& backend) {
  Action a{AgentOperation::Add, objectAddress, std::promise<void>()};
  queueAndExecuteAction(a, backend);
}

void AgentReference::removeFromOwnership(const std::string& objectAddress, objectstore::Backend& backend) {
  Action a{AgentOperation::Remove, objectAddress, std::promise<void>()};
  queueAndExecuteAction(a, backend);
}

void AgentReference::queueAndExecuteAction(Action& action, objectstore::Backend& backend) {
  // First, we need to determine if a queue exists or not.
  // If so, we just use it, and if not, we create and serve it.
  std::unique_lock<std::mutex> ulGlobal(m_currentQueueMutex);
  if (m_currentQueue) {
    // There is already a queue
    std::unique_lock<std::mutex> ulQueue(m_currentQueue->mutex);
    m_currentQueue->queue.push_back(&action);
    // If this is time to run, wake up the serving thread
    if (m_currentQueue->queue.size() + 1 >= m_maxQueuedItems) {
      m_currentQueue->promise.set_value();
      m_currentQueue = nullptr;
    }
    // Release the locks and wait for action execution
    ulQueue.unlock();
    ulGlobal.unlock();
    action.promise.get_future().get();
  } else {
    // There is not queue, so we need to create and serve it ourselves
    ActionQueue q;
    // Lock the queue
    std::unique_lock<std::mutex> ulq(q.mutex);
    // Get it referenced
    m_currentQueue = &q;
    // Get our execution promise and leave one behind
    std::unique_ptr<std::promise<void>> promiseForThisQueue(m_nextQueueExecutionPromise.release());
    // Leave a promise behind for the next queue
    m_nextQueueExecutionPromise.reset(new std::promise<void>);
    // Keep a pointer to it, so we will signal our own completion to our successor queue.
    std::promise<void> * promiseForNextQueue = m_nextQueueExecutionPromise.get();
    // We can now unlock the queue and the general lock: queuing is open.
    ulq.unlock();
    ulGlobal.unlock();
    // We wait for time or size of queue
    q.promise.get_future().wait_for(std::chrono::milliseconds(100));
    // Make sure we are not listed anymore a the queue taking jobs (this would happen
    // in case of timeout.
    ulGlobal.lock();
    if (m_currentQueue == &q)
      m_currentQueue = nullptr;
    ulGlobal.unlock();
    // Make sure no leftover thread is still writing to the queue.
    ulq.lock();
    // Off we go! Add the actions to the queue
    try {
      objectstore::Agent ag(m_agentAddress, backend);
      objectstore::ScopedExclusiveLock agl(ag);
      ag.fetch();
      // First we apply our own modification
      appyAction(action, ag);
      // Then those of other threads
      for (auto a: q.queue)
        appyAction(*a, ag);
      // and commit
      ag.commit();
    } catch (...) {
      // Something wend wrong: , we release the next batch of changes
      promiseForNextQueue->set_value();
      // We now pass the exception to all threads
      for (auto a: q.queue)
        a->promise.set_exception(std::current_exception());
      // And to our own caller
      throw;
    }
    // Things went well. We pass the token to the next queue
    promiseForNextQueue->set_value();
    // and release the other threads
    for (auto a: q.queue)
      a->promise.set_value();
  }
}

void AgentReference::appyAction(Action& action, objectstore::Agent& agent) {
  switch (action.op) {
  case AgentOperation::Add:
    agent.addToOwnership(action.objectAddress);
    break;
  case AgentOperation::Remove:
    agent.removeFromOwnership(action.objectAddress);
    break;
  default:
    throw cta::exception::Exception("In AgentReference::appyAction(): unknown operation.");
  }
}


}} // namespace cta::objectstore
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

#include "common/helgrind_annotator.hpp"
#include "AgentReference.hpp"
#include "Agent.hpp"
#include "common/exception/Errnum.hpp"

#include <sstream>
#include <unistd.h>
#include <sys/syscall.h>
#include <iomanip>

namespace cta { namespace objectstore {

AgentReference::AgentReference(const std::string & clientType, log::Logger &logger):
m_logger(logger) {
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
  // Initialize the serialization token for queued actions (lock will make helgrind 
  // happy, but not really needed
  threading::MutexLocker ml(m_currentQueueMutex);
  m_nextQueueExecutionPromise.reset(new std::promise<void>);
  m_nextQueueExecutionFuture = m_nextQueueExecutionPromise->get_future();
  ANNOTATE_HAPPENS_BEFORE(m_nextQueueExecutionPromise.get());
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
  std::shared_ptr<Action> a (new Action(AgentOperation::Add, objectAddress));
  queueAndExecuteAction(a, backend);
}

void AgentReference::addBatchToOwnership(const std::list<std::string>& objectAdresses, objectstore::Backend& backend) {
  objectstore::Agent ag(m_agentAddress, backend);
  log::LogContext lc(m_logger);
  log::ScopedParamContainer params(lc);
  params.add("agentObject", m_agentAddress);
  objectstore::ScopedExclusiveLock agl(ag);
  ag.fetch();
  for (const auto & oa: objectAdresses) {
    ag.addToOwnership(oa);
    log::ScopedParamContainer params(lc);
    params.add("ownedObject", oa);
    lc.log(log::DEBUG, "In AgentReference::addBatchToOwnership(): added object to ownership.");
  }
  ag.commit();
}

void AgentReference::removeFromOwnership(const std::string& objectAddress, objectstore::Backend& backend) {
  std::shared_ptr<Action> a (new Action(AgentOperation::Remove, objectAddress));
  queueAndExecuteAction(a, backend);
}

void AgentReference::removeBatchFromOwnership(const std::list<std::string>& objectAdresses, objectstore::Backend& backend) {
  objectstore::Agent ag(m_agentAddress, backend);
  log::LogContext lc(m_logger);
  log::ScopedParamContainer params(lc);
  params.add("agentObject", m_agentAddress);
  objectstore::ScopedExclusiveLock agl(ag);
  ag.fetch();
  for (const auto & oa: objectAdresses) {
    ag.removeFromOwnership(oa);
    log::ScopedParamContainer params(lc);
    params.add("ownedObject", oa);
    lc.log(log::DEBUG, "In AgentReference::removeBatchFromOwnership(): removed object from ownership.");
  }
  ag.commit();
}

void AgentReference::bumpHeatbeat(objectstore::Backend& backend) {
  std::shared_ptr<Action> a (new Action(AgentOperation::Heartbeat, ""));
  queueAndExecuteAction(a, backend);
}


void AgentReference::queueAndExecuteAction(std::shared_ptr<Action> action, objectstore::Backend& backend) {
  // First, we need to determine if a queue exists or not.
  // If so, we just use it, and if not, we create and serve it.
  threading::MutexLocker ulGlobal(m_currentQueueMutex);
  if (m_currentQueue) {
    // There is already a queue
    threading::MutexLocker ulQueue(m_currentQueue->mutex);
    m_currentQueue->queue.push_back(action);
    // Get hold of the future before the promise gets a chance to be accessed
    auto actionFuture=action->promise.get_future();
    // Release the locks and wait for action execution
    ulQueue.unlock();
    ulGlobal.unlock();
    actionFuture.get();
    ANNOTATE_HAPPENS_AFTER(&action->promise);
    ANNOTATE_HAPPENS_BEFORE_FORGET_ALL(&action->promise);
  } else {
    // There is no queue, so we need to create and serve it ourselves.
    // To make sure there is no lifetime issues, we make it a shared_ptr
    std::shared_ptr<ActionQueue> q(new ActionQueue);
    // Lock the queue
    threading::MutexLocker ulq(q->mutex);
    // Get it referenced
    m_currentQueue = q;
    // Get our execution promise and future and leave one behind.
    std::shared_ptr<std::promise<void>> promiseForThisQueue(m_nextQueueExecutionPromise);
    auto futureForThisQueue = std::move(m_nextQueueExecutionFuture);
    // Leave a promise behind for the next queue, and set the future.
    m_nextQueueExecutionPromise.reset(new std::promise<void>);
    m_nextQueueExecutionFuture=m_nextQueueExecutionPromise->get_future();
    // Keep a pointer to it, so we will signal our own completion to our successor queue.
    std::shared_ptr<std::promise<void>> promiseForNextQueue = m_nextQueueExecutionPromise;
    // We can now unlock the queue and the general lock: queuing is open.
    ulq.unlock();
    ulGlobal.unlock();
    // Wait for previous queue to complete so we will not contend with other threads while
    // updating the object store. 
    futureForThisQueue.get();
    ANNOTATE_HAPPENS_AFTER(promiseForThisQueue.get());
    ANNOTATE_HAPPENS_BEFORE_FORGET_ALL(promiseForThisQueue.get());
    // Make sure we are not listed anymore as the queue taking jobs.
    // We should still be the listed queue
    ulGlobal.lock();
    if (m_currentQueue != q) {
      throw cta::exception::Exception("In AgentReference::queueAndExecuteAction(): our queue is not the listed one as expected.");
    }
    m_currentQueue.reset();
    ulGlobal.unlock();
    // Make sure no leftover thread is still writing to the queue.
    ulq.lock();
    // Off we go! Add the actions to the queue
    try {
      objectstore::Agent ag(m_agentAddress, backend);
      log::LogContext lc(m_logger);
      log::ScopedParamContainer params(lc);
      params.add("agentObject", m_agentAddress);
      objectstore::ScopedExclusiveLock agl(ag);
      ag.fetch();
      // First we apply our own modification
      appyAction(*action, ag, lc);
      // Then those of other threads
      for (auto a: q->queue) {
        threading::MutexLocker ml(a->mutex);
        appyAction(*a, ag, lc);
      }
      // and commit
      ag.commit();
      // We avoid global log (with a count) as we would get one for each heartbeat.
    } catch (...) {
      // Something wend wrong: , we release the next batch of changes
      ANNOTATE_HAPPENS_BEFORE(promiseForNextQueue.get());
      promiseForNextQueue->set_value();
      // We now pass the exception to all threads
      for (auto a: q->queue) {
        threading::MutexLocker ml(a->mutex);
        ANNOTATE_HAPPENS_BEFORE(&a->promise);
        a->promise.set_exception(std::current_exception());
      }
      // And to our own caller
      throw;
    }
    // Things went well. We pass the token to the next queue
    ANNOTATE_HAPPENS_BEFORE(promiseForNextQueue.get());
    promiseForNextQueue->set_value();
    // and release the other threads
    for (auto a: q->queue) {
      threading::MutexLocker ml(a->mutex);
      ANNOTATE_HAPPENS_BEFORE(&a->promise);
      a->promise.set_value();
    }
  }
}

void AgentReference::appyAction(Action& action, objectstore::Agent& agent, log::LogContext &lc) {
  switch (action.op) {
  case AgentOperation::Add:
  {
    agent.addToOwnership(action.objectAddress);
    log::ScopedParamContainer params(lc);
    params.add("ownedObject", action.objectAddress);
    lc.log(log::DEBUG, "In AgentReference::appyAction(): added object to ownership.");
    break;
  }
  case AgentOperation::Remove:
  {
    agent.removeFromOwnership(action.objectAddress);
    log::ScopedParamContainer params(lc);
    params.add("ownedObject", action.objectAddress);
    lc.log(log::DEBUG, "In AgentReference::appyAction(): removed object from ownership.");
    break;
  }
  case AgentOperation::Heartbeat:
    agent.bumpHeartbeat();
    break;
  default:
    throw cta::exception::Exception("In AgentReference::appyAction(): unknown operation.");
  }
}


}} // namespace cta::objectstore
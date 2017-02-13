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

#pragma once

#include "objectstore/Backend.hpp"
#include <atomic>
#include <string>
#include <future>
#include <list>

namespace cta { namespace objectstore {

class Agent;

/**
 * A class allowing the passing of the address of an Agent object, plus a thread safe
 * object name generator, that will allow unique name generation by several threads.
 * This object should be created once and for all per session (as the corresponding
 * Agent object in the object store).
 * A process 
 */
class AgentReference {
public:
  /**
   * Constructor will implicitly generate the address of the Agent object.
   * @param clientType is an indicative string used to generate the agent object's name.
   */
  AgentReference(const std::string &clientType);
  
  /**
   * Generates a unique address for a newly created child object. This function is thread
   * safe.
   * @param childType the name of the child object type.
   * @return a unique address for the child object, derived from the agent's address.
   */
  std::string nextId(const std::string & childType);
  
  /**
   * Adds an object address to the referenced agent. The additions and removals
   * are queued in memory so that several threads can share the same access.
   * The execution order is guaranteed.
   * @param objectAddress
   */
  void addToOwnership(const std::string &objectAddress, objectstore::Backend& backend);
  
  /**
   * Removes an object address from the referenced agent. The additions and removals
   * are queued in memory so that several threads can share the same access.
   * The execution order is guaranteed.
   * @param objectAddress
   */
  void removeFromOwnership(const std::string &objectAddress, objectstore::Backend& backend);

  /**
   * Gets the address of the Agent object generated on construction.
   * @return the agent object address.
   */
  std::string getAgentAddress();
private:
  std::atomic<uint64_t> m_nextId;
  std::string m_agentAddress;
  
  /**
   * An enumeration describing all the queueable operations
   */
  enum class AgentOperation: char {
    Add,
    Remove
  };
  
  /**
   * An operation with its parameter and promise
   */
  struct Action {
    AgentOperation op;
    const std::string & objectAddress;
    std::promise<void> promise;
  };
  
  /**
   * The queue with the lock and flush control 
   */
  struct ActionQueue {
    std::mutex mutex;
    std::list<Action *> queue;
    std::promise<void> promise;
  };
  
  /**
   * Helper function applying the action to the already fetched agent.
   * @param action
   * @param agent
   */
  void appyAction(Action &action, objectstore::Agent & agent);
  
  /**
   * The global function actually doing the job: creates a queue if needed, add
   * the action to it and flushes them based on time and count. Uses an algorithm
   * similar to queueing in ArchiveQueues and RetrieveQeueues.
   * @param action the action
   */
  void queueAndExecuteAction(Action& action, objectstore::Backend& backend);
  
  std::mutex m_currentQueueMutex;
  ActionQueue * m_currentQueue = nullptr;
  /**
   * This pointer holds a promise that will be picked up by the thread managing 
   * the a queue in memory (promise(n)). The same thread will leave a fresh promise 
   * (promise(n+1) in this pointer for the next thread to pick up. The thread will
   * then wait for promise(n) to be fullfilled to flush to queue to the object store
   * and will fullfill promise(n+1) after doing so.
   * This will ensure that the queues will be flushed in order, one at a time.
   * One at a time also minimize contention on the object store.
   */
  std::unique_ptr<std::promise<void>> m_nextQueueExecutionPromise;
  const size_t m_maxQueuedItems = 100;
};

}} 

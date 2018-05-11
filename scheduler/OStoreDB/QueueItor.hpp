/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          Iterator class for Archive/Retrieve job queues
 * @copyright      Copyright 2018 CERN
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

#pragma once

#include "objectstore/Backend.hpp"

namespace cta {

/*!
 * Iterator class for Archive/Retrieve job queues
 *
 * Allows asynchronous access to job queues for streaming responses
 */
template<typename JobQueuesQueue, typename JobQueue>
class QueueItor {
public:
   QueueItor(objectstore::Backend &objectStore, const std::string &tapePoolName = "");
   const std::string &tapePool() const { return m_jobQueuesQueueIt->tapePool; }
   void operator++() { ++m_jobQueueIt; }
   bool end();
   std::pair<bool,typename JobQueue::job_t> getJob();
private:
   void getQueueJobs();

   objectstore::Backend                                           &m_objectStore;         // Reference to ObjectStore Backend
   bool                                                            m_onlyThisTapePool;    // true if a tapePoolName parameter was passed to the constructor
   typename std::list<JobQueuesQueue>                              m_jobQueuesQueue;      // list of Archive or Retrieve Job Queues
   typename std::list<JobQueuesQueue>::const_iterator              m_jobQueuesQueueIt;    // iterator across m_jobQueuesQueue
   typename std::list<typename JobQueue::JobDump>                  m_jobQueue;            // list of Archive or Retrieve Jobs
   typename std::list<typename JobQueue::JobDump>::const_iterator  m_jobQueueIt;          // iterator across m_jobQueue
};

/*!
 * Check if the QueueItor points to a valid object
 *
 * @retval false if internal iterators point to a valid object
 * @retval true if we have passed the last item in the last queue
 */
template<typename JobQueuesQueue, typename JobQueue>
bool QueueItor<JobQueuesQueue, JobQueue>::end()
{
   // Case 1: no more queues
   if(m_jobQueuesQueueIt == m_jobQueuesQueue.end()) return true;

   // Case 2: we are in a queue and haven't reached the end of it
   if(m_jobQueueIt != m_jobQueue.end()) return false;

   // Case 3: we have reached the end of the current queue and this is the only queue we care about
   if(m_onlyThisTapePool) return true;

   // Case 4: we have reached the end of the current queue, try to advance to the next queue
   for(++m_jobQueuesQueueIt; m_jobQueuesQueueIt != m_jobQueuesQueue.end(); ++m_jobQueuesQueueIt) {
      getQueueJobs();
      if(m_jobQueueIt != m_jobQueue.end()) break;
   }
   return m_jobQueueIt == m_jobQueue.end();
}

}

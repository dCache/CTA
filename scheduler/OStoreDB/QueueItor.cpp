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

#include "QueueItor.hpp"
#include <objectstore/RootEntry.hpp>
#include <objectstore/ArchiveQueue.hpp>
#include <objectstore/RetrieveQueue.hpp>

namespace cta {

//------------------------------------------------------------------------------
// QueueItor::getJobQueue (generic)
//------------------------------------------------------------------------------
template<typename JobQueuesQueue, typename JobQueue>
void QueueItor<JobQueuesQueue, JobQueue>::
getJobQueue()
{
std::cerr << "getJobQueue()" << std::endl;
  try {
    JobQueue osq(m_jobQueuesQueueIt->address, m_objectStore);
    objectstore::ScopedSharedLock ostpl(osq);
    osq.fetch();
    m_jobQueue = osq.dumpJobs();
  } catch(...) {
    // Behaviour is racy: it's possible that the queue can disappear before we read it.
    // In this case, we ignore the error and move on.
  }

  // Grab the first batch of jobs from the current queue
  updateJobCache();
}

//------------------------------------------------------------------------------
// QueueItor::QueueItor (Archive specialisation)
//------------------------------------------------------------------------------
template<>
QueueItor<objectstore::RootEntry::ArchiveQueueDump, objectstore::ArchiveQueue>::
QueueItor(objectstore::Backend &objectStore, const std::string &queue_id) :
  m_objectStore(objectStore),
  m_onlyThisQueueId(!queue_id.empty())
{
std::cerr << "ArchiveQueueItor constructor" << std::endl;
  objectstore::RootEntry re(m_objectStore);
  {
    objectstore::ScopedSharedLock rel(re);
    re.fetch();
    m_jobQueuesQueue = re.dumpArchiveQueues();
  }

  // Set queue iterator to the first queue in the list
  m_jobQueuesQueueIt = m_jobQueuesQueue.begin();

  // If we specified a tape pool, advance to the correct queue
  if(m_onlyThisQueueId) {
    for( ; m_jobQueuesQueueIt != m_jobQueuesQueue.end(); ++m_jobQueuesQueueIt) {
      if(m_jobQueuesQueueIt->tapePool == queue_id) break;
    }
    if(m_jobQueuesQueueIt == m_jobQueuesQueue.end()) {
      throw cta::exception::UserError("Archive queue for TapePool " + queue_id + " not found.");
    }
  }

  // Find the first job in the queue
  if(m_jobQueuesQueueIt != m_jobQueuesQueue.end()) {
    getJobQueue();
  }
}

//------------------------------------------------------------------------------
// QueueItor::qid (Archive specialisation)
//------------------------------------------------------------------------------
template<>
const std::string&
QueueItor<objectstore::RootEntry::ArchiveQueueDump, objectstore::ArchiveQueue>::
qid() const
{
std::cerr << "ArchiveQueueItor qid()" << std::endl;
  return m_jobQueuesQueueIt->tapePool;
}

//------------------------------------------------------------------------------
// QueueItor::updateJobCache (Archive specialisation)
//------------------------------------------------------------------------------
template<>
void
QueueItor<objectstore::RootEntry::ArchiveQueueDump, objectstore::ArchiveQueue>::
updateJobCache()
{
std::cerr << "ArchiveQueue updateJobCache()" << std::endl;
}

#if 0
//------------------------------------------------------------------------------
// QueueItor::getJob (Archive specialisation)
//------------------------------------------------------------------------------
template<>
std::pair<bool,objectstore::ArchiveQueue::job_t>
QueueItor<objectstore::RootEntry::ArchiveQueueDump, objectstore::ArchiveQueue>::
getJob() const
{
std::cerr << "ArchiveQueueItor getJob()" << std::endl;
  auto job = cta::common::dataStructures::ArchiveJob();

  try {
    objectstore::ArchiveRequest osar(m_jobQueueIt->address, m_objectStore);
    objectstore::ScopedSharedLock osarl(osar);
    osar.fetch();

    // Find the copy number for this tape pool
    for(auto &j:osar.dumpJobs()) {
      if(j.tapePool == m_jobQueuesQueueIt->tapePool) {
        job.tapePool                 = j.tapePool;
        job.copyNumber               = j.copyNb;
        job.archiveFileID            = osar.getArchiveFile().archiveFileID;
        job.request.checksumType     = osar.getArchiveFile().checksumType;
        job.request.checksumValue    = osar.getArchiveFile().checksumValue;
        job.request.creationLog      = osar.getEntryLog();
        job.request.diskFileID       = osar.getArchiveFile().diskFileId;
        job.request.diskFileInfo     = osar.getArchiveFile().diskFileInfo;
        job.request.fileSize         = osar.getArchiveFile().fileSize;
        job.instanceName             = osar.getArchiveFile().diskInstance;
        job.request.requester        = osar.getRequester();
        job.request.srcURL           = osar.getSrcURL();
        job.request.archiveReportURL = osar.getArchiveReportURL();
        job.request.storageClass     = osar.getArchiveFile().storageClass;
        osarl.release();

        return std::make_pair(true, job);
      }
    }
    osarl.release();
  } catch(...) {}

  // Skip the request if copy number is not found
  return std::make_pair(false, job);
}
#endif

//------------------------------------------------------------------------------
// QueueItor::QueueItor (Retrieve specialisation)
//------------------------------------------------------------------------------
template<>
QueueItor<objectstore::RootEntry::RetrieveQueueDump, objectstore::RetrieveQueue>::
QueueItor(objectstore::Backend &objectStore, const std::string &queue_id) :
  m_objectStore(objectStore),
  m_onlyThisQueueId(!queue_id.empty())
{
std::cerr << "RetrieveQueueItor constructor" << std::endl;
  // Get the list of job queues from the objectstore
  {
    objectstore::RootEntry re(m_objectStore);
    objectstore::ScopedSharedLock rel(re);
    re.fetch();
    m_jobQueuesQueue = re.dumpRetrieveQueues();
  }

  // Find the first queue
  m_jobQueuesQueueIt = m_jobQueuesQueue.begin();

  // If we specified a Volume ID, advance to the correct queue
  if(m_onlyThisQueueId) {
    for( ; m_jobQueuesQueueIt != m_jobQueuesQueue.end() && m_jobQueuesQueueIt->vid != queue_id; ++m_jobQueuesQueueIt) ;
    if(m_jobQueuesQueueIt == m_jobQueuesQueue.end()) {
      throw cta::exception::UserError("Retrieve queue for Volume ID " + queue_id + " not found.");
    }
  }

  // Fill the cache with the first batch of jobs
  for( ; m_jobQueuesQueueIt != m_jobQueuesQueue.end() ; nextJobQueue()) {
    getJobQueue();
    if(!m_jobCache.empty()) break;
  }
}

//------------------------------------------------------------------------------
// QueueItor::qid (Retrieve specialisation)
//------------------------------------------------------------------------------
template<>
const std::string&
QueueItor<objectstore::RootEntry::RetrieveQueueDump, objectstore::RetrieveQueue>::
qid() const
{
std::cerr << "RetrieveQueueItor qid()" << std::endl;
  return m_jobQueuesQueueIt->vid;
}

//------------------------------------------------------------------------------
// QueueItor::updateJobCache (Retrieve specialisation)
//------------------------------------------------------------------------------
template<>
void
QueueItor<objectstore::RootEntry::RetrieveQueueDump, objectstore::RetrieveQueue>::
updateJobCache()
{
std::cerr << "RetrieveQueue updateJobCache(): jobQueue has " << m_jobQueue.size() << " entries." << std::endl;

  while(!m_jobQueue.empty() && m_jobCache.empty())
  {
    // Get a chunk of retrieve jobs from the current retrieve queue

    auto chunksize = m_jobQueue.size() < JOB_CACHE_SIZE ? m_jobQueue.size() : JOB_CACHE_SIZE;
    auto jobQueueChunkEnd = m_jobQueue.begin();
    std::advance(jobQueueChunkEnd, chunksize);

    decltype(m_jobQueue) jobQueueChunk;
    jobQueueChunk.splice(jobQueueChunk.end(), m_jobQueue, m_jobQueue.begin(), jobQueueChunkEnd);
std::cerr << "RetrieveQueue updateJobCache(): jobQueueChunk has " << jobQueueChunk.size() << " entries, jobQueue has " << m_jobQueue.size() << " entries." << std::endl;

    // Populate the jobs cache from the retrieve jobs

    for(auto &j: jobQueueChunk) {
      try {
        auto job = cta::common::dataStructures::RetrieveJob();

        objectstore::RetrieveRequest rr(j.address, m_objectStore);
        objectstore::ScopedSharedLock rrl(rr);
        rr.fetch();
        job.request = rr.getSchedulerRequest();
        for(auto &tf: rr.getArchiveFile().tapeFiles) {
          job.tapeCopies[tf.second.vid].first  = tf.second.copyNb;
          job.tapeCopies[tf.second.vid].second = tf.second;
        }

        m_jobCache.push_back(job);
      } catch(...) {
        // This implementation gives imperfect consistency and is racy. If the queue has gone, move on.
      }
    }
  }

std::cerr << "RetrieveQueue updateJobCache(): jobCache updated with " << m_jobCache.size() << " entries." << std::endl;
}

//auto ArchiveQueue::dumpJobs() -> std::list<JobDump> {
  //checkPayloadReadable();
  // Go read the shards in parallel...
  // std::list<JobDump> ret;
  // std::list<ArchiveQueueShard> shards;
  // std::list<std::unique_ptr<ArchiveQueueShard::AsyncLockfreeFetcher>> shardsFetchers;
  // for (auto & sa: m_payload.archivequeueshards()) {
  // shards.emplace_back(ArchiveQueueShard(sa.address(), m_objectStore));
  // shardsFetchers.emplace_back(shards.back().asyncLockfreeFetch());
  // }
  // auto s = shards.begin();
  // auto sf = shardsFetchers.begin();
  // while (s != shards.end()) {
  // try {
  // (*sf)->wait();
  // } catch (Backend::NoSuchObject & ex) {
  // // We are possibly in read only mode, so we cannot rebuild.
  // // Just skip this shard.
  // goto nextShard;
  // }
  // for (auto & j: s->dumpJobs()) {
  // ret.emplace_back(JobDump{j.size, j.address, j.copyNb});
  // }
  // nextShard:
  // s++; sf++;
  // }
  // return ret;
  // }

} // namespace cta

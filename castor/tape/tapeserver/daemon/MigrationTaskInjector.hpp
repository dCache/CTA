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

#pragma once

#include "castor/tape/tapeserver/daemon/MigrationMemoryManager.hpp"
#include "castor/tape/tapeserver/daemon/TapeWriteSingleThread.hpp"
#include "castor/tape/tapeserver/daemon/TapeWriteTask.hpp"
#include "castor/tape/tapeserver/daemon/DiskReadThreadPool.hpp"
#include "castor/tape/tapeserver/daemon/DiskReadTask.hpp"
#include "castor/tape/tapeserver/client/ClientProxy.hpp"
#include "castor/tape/tapegateway/FileToMigrateStruct.hpp"
#include "castor/tape/tapeserver/client/ClientInterface.hpp"
#include "castor/log/LogContext.hpp"

#include "castor/server/AtomicCounter.hpp"
namespace castor{
namespace tape{
namespace tapeserver{
namespace daemon {

/**
 * This classis responsible for creating the tasks in case of a recall job
 */
class MigrationTaskInjector /*: public TaskInjector*/ {  
public:

  /**
   * Constructor
   * @param mm The memory manager for accessing memory blocks. 
   * The Newly created tapeWriter Tasks will register themselves 
   * as a client to it. 
   * @param diskReader the one object that will hold all the threads which will be executing 
   * disk-reading tasks
   * @param tapeWriter the one object that will hold the thread which will be executing 
   * tape-writing tasks
   * @param client The one that will give us files to migrate 
   * @param maxFiles maximal number of files we may request to the client at once 
   * @param byteSizeThreshold maximal number of cumulated byte 
   * we may request to the client. at once
   * @param lc log context, copied because of the threading mechanism 
   */
  MigrationTaskInjector(MigrationMemoryManager & mm, 
        DiskReadThreadPool & diskReader,
        TapeSingleThreadInterface<TapeWriteTask> & tapeWriter,client::ClientInterface& client,
        uint64_t maxFiles, uint64_t byteSizeThreshold,castor::log::LogContext lc);

  /**
   * Wait for the inner thread to finish
   */
  void waitThreads();
  
  /**
   * Start the inner thread 
   */
  void startThreads();
  /**
   * Function for a feed-back loop purpose between MigrationTaskInjector and 
   * DiskReadThreadPool. When DiskReadThreadPool::popAndRequestMoreJobs detects 
   * it has not enough jobs to do to, it is class to push a request 
   * in order to (try) fill up the queue. 
   * @param lastCall true if we want the new request to be a last call. 
   * See Request::lastCall 
   */
  void requestInjection(bool lastCall);
  
  /**
   * Contact the client to make sure there are really something to do
   * Something = migration at most  maxFiles or at least maxBytes
   * 
   * @return true if there are jobs to be done, false otherwise 
   */
  bool synchronousInjection();
  
  /**
   * Send an end token in the request queue. There should be no subsequent
   * calls to requestInjection.
   */
  void finish();
  
  /**
   * Return the first file to be written's fseq
   * @return 
   */
  uint64_t firstFseqToWrite() const;
private:
  /**
   * Create all the tape-read and write-disk tasks for set of files to retrieve
   * @param jobs the list of FileToMigrateStructs we have to transform in a pair of task
   */
  void injectBulkMigrations(const std::vector<castor::tape::tapegateway::FileToMigrateStruct*>& jobs);
  
  /*Compute how many blocks are needed for a file of fileSize bytes*/
  size_t howManyBlocksNeeded(size_t fileSize,size_t blockCapacity){
    return fileSize/blockCapacity + ((fileSize%blockCapacity==0) ? 0 : 1); 
  }
  
  /**
   * It will signal to the disk read thread  pool, tape write single thread
   * and to the mem manager they have to stop their threads(s)
   */
  void signalEndDataMovement();

   /**
   * A request of files to migrate. We request EITHER
   * - a maximum of nbMaxFiles files
   * - OR at least byteSizeThreshold bytes. 
   * That means we stop as soon as we have nbMaxFiles files or the cumulated size 
   * is equal or above byteSizeThreshold. 
   */
  class Request {
  public:
    Request(uint64_t mf, uint64_t mb, bool lc):
    nbMaxFiles(mf), byteSizeThreshold(mb), lastCall(lc), end(false) {}
    
    Request():
    nbMaxFiles(-1), byteSizeThreshold(-1), lastCall(true),end(true) {}
    
    const uint64_t nbMaxFiles;
    const uint64_t byteSizeThreshold;
    
    /** 
     * True if it is the last call for the set of requests :it means
     *  we don't need to try to get more files to recall 
     *  and can send into all the different threads a signal  .
     */
    const bool lastCall;
    
    /**
     * True indicates the task injector will not receive any more request.
     */
    const bool end;
  };
  
  class WorkerThread: public castor::server::Thread {
  public:
    WorkerThread(MigrationTaskInjector & rji): m_parent(rji) {}
    virtual void run();
  private:
    MigrationTaskInjector & m_parent;
  } m_thread;
  ///The memory manager for accessing memory blocks. 
  MigrationMemoryManager & m_memManager;
  
  ///the one object that will hold the thread which will be executing 
  ///tape-writing tasks
  TapeSingleThreadInterface<TapeWriteTask>& m_tapeWriter;
  
  ///the one object that will hold all the threads which will be executing 
  ///disk-reading tasks
  DiskReadThreadPool & m_diskReader;
  
  /// the client who is sending us jobs
  client::ClientInterface& m_client;
  
  /**
   * utility member to log some pieces of information
   */
  castor::log::LogContext m_lc;
  
  castor::server::Mutex m_producerProtection;
  
  ///all the requests for work we will forward to the client.
  castor::server::BlockingQueue<Request> m_queue;
  
  /** a shared flag among the all tasks related to migration, set as true 
   * as soon a single task encounters a failure. That way we go into a degraded mode
   * where we only circulate memory without writing anything on tape
   */
  castor::server::AtomicFlag m_errorFlag;

  /// The maximum number of files we ask per request. 
  const uint64_t m_maxFiles;
  
  /// Same as m_maxFilesReq for size per request. (in bytes))
  const uint64_t m_maxBytes;
  
  /**The last fseq used on the tape. We should not see this but 
   * IT is computed by subtracting 1 to fSeg  of the first file to migrate we 
   * receive. That part is done by the 
   * MigrationTaskInjector.::synchronousInjection. Thus, we compute it into 
   * that function and retrieve/set it within DataTransferSession executeWrite
   * after we make sure synchronousInjection returned true. To do so, we
   *  need to store it
   */
  uint64_t m_firstFseqToWrite;
};

} //end namespace daemon
} //end namespace tapeserver
} //end namespace tape
} //end namespace castor


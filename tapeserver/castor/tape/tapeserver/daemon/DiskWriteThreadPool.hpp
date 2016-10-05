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

#include "common/threading/BlockingQueue.hpp"
#include "common/threading/Threading.hpp"
#include "common/threading/AtomicCounter.hpp"
#include "common/log/LogContext.hpp"
#include "castor/tape/tapeserver/utils/suppressUnusedVariable.hpp"
#include "castor/tape/tapeserver/daemon/RecallReportPacker.hpp"
#include "castor/tape/tapeserver/daemon/DiskWriteTask.hpp"
#include "castor/tape/tapeserver/daemon/DiskStats.hpp"
#include "castor/tape/tapeserver/daemon/TaskWatchDog.hpp"
#include "castor/tape/tapeserver/file/RadosStriperPool.hpp"
#include "common/Timer.hpp"
#include <vector>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
/**
 * Container for the threads that will execute the disk writes tasks in the 
 * migration.
 */
class DiskWriteThreadPool {
public:
  /**
   * Constructor: we create the thread structures here, but they do not get
   * started yet.
   * @param nbThread Fixed number of threads in the pool
   * @param reportPacker Reference to a previously created recall
   * report packer, to which the tasks will report their results.
   * @param lc reference to a log context object that will be copied at
   * construction time (and then copied further for each thread). There will
   * be no side effect on the caller's logs.
   * @param remoteFileProtocol a string describing how the bare (no URL)
   * pathes to files should be interpreted. Default is RFIO, unless the string
   * is "xroot" (with any case variation) in which case we would use Xroot as a
   * protocol.
   * @param xrootPrivateKeyPath the path to the xroot private key file.
   */
  DiskWriteThreadPool(int nbThread, 
          RecallReportPacker& reportPacker,
          RecallWatchDog& recallWatchDog,
          cta::log::LogContext lc,
          const std::string & remoteFileProtocol,
          const std::string & xrootPrivateKeyPath,
          uint16_t xrootTimeout);
  /**
   * Destructor: we suppose the threads are no running (waitThreads() should
   * be called befor destruction unless the threads were not started.
   */
  virtual ~DiskWriteThreadPool();
  
  /**
   * Starts the thread created at construction time.
   */
  
  void startThreads();
  /**
   * Waits for completion of all the pool's threads.
   */
  void waitThreads();
  
  /**
   * Pushes a pointer to a task. The thread pool owns the task and will
   * de-allocate it.
   * @param t pointer to the task
   */
  void push(DiskWriteTask *t);
  
  /**
   * Signals to the thread pool that there will be no more tasks pushed to it,
   * and that the threads can therefore complete.
   */
  void finish();

private:
  /** Running counter active threads, used to determine which thread is the last. */
  cta::threading::AtomicCounter<int> m_nbActiveThread;
  /** Thread safe counter for failed tasks */
  cta::threading::AtomicCounter<int> m_failedWriteCount;
  
  /**
   * Private class implementing the worker threads.
   */
  class DiskWriteWorkerThread: private cta::threading::Thread {
  public:
    DiskWriteWorkerThread(DiskWriteThreadPool & manager):
    m_threadID(manager.m_nbActiveThread++),m_parentThreadPool(manager),
    m_lc(m_parentThreadPool.m_lc), 
    m_diskFileFactory(manager.m_remoteFileProtocol, manager.m_xrootPrivateKeyPath, 
      manager.m_xrootTimeout, manager.m_striperPool)
    {
      // This thread Id will remain for the rest of the thread's lifetime (and 
      // also context's lifetime) so ne need for a scope.
      m_lc.pushOrReplace(cta::log::Param("threadID", m_threadID));
      m_lc.log(cta::log::INFO,"DiskWrite Thread created");
    }
      
    void start() { cta::threading::Thread::start(); }
    void wait() { cta::threading::Thread::wait(); }
  private:
    void logWithStat(int level, const std::string& message);
    /*
     * For measuring how long  are the the different steps 
     */
    DiskStats m_threadStat;
  
    /**
     * To identify the thread 
     */
    const int m_threadID;
    
    /**
     * The owning thread pool
     */
    DiskWriteThreadPool & m_parentThreadPool;
    
    /**
     * For logging the event
     */
    cta::log::LogContext m_lc;

    /**
     * A disk file factory, that will create the proper type of file access class,
     * depending on the received path
     */
    diskFile::DiskFileFactory m_diskFileFactory;

    virtual void run();
  };
  /**
   * When a thread finishm it call this function to Add its stats to one one of the
   * Threadpool
   * @param threadStats
   */
  void addThreadStats(const DiskStats& threadStats);
  
  
  /**
   * When the last thread finish, we log all m_pooldStat members + message
   * at the given level
   * @param level
   * @param message
   */
  void logWithStat(int level, const std::string& message);
  
  /** The actual container for the thread objects */
  std::vector<DiskWriteWorkerThread *> m_threads;
  /** Mutex protecting the pushers of new tasks from having the object deleted
   * under their feet. */
  cta::threading::Mutex m_pusherProtection;
  
  /**
   To protect addThreadStats from concurrent calls
   */
  cta::threading::Mutex m_statAddingProtection;
protected:
  /** The (thread safe) queue of tasks */
  cta::threading::BlockingQueue<DiskWriteTask*> m_tasks;

  /**
   * Parameter selecting the disk transfer protocol
   */
  std::string m_remoteFileProtocol;
  
  /**
   * Parameter: path to xroot private key
   */
  std::string m_xrootPrivateKeyPath;
  
  /**
   * Parameter: xroot timeout
   */
  uint16_t m_xrootTimeout;

  /**
   * A pool of rados striper connections, to be shared by all threads
   */
  castor::tape::file::RadosStriperPool m_striperPool;

private:
  /**
   * Aggregate all threads' stats 
   */
  DiskStats m_pooldStat;

  /**
   * Measure the thread pool's lifetime
   */
  cta::utils::Timer m_totalTime;
    
  /** Reference to the report packer where tasks report the result of their 
   * individual files and the end of session (for the last thread) */
  RecallReportPacker& m_reporter;
  
  /** Reference to the session watchdog, allowing reporting of errors to it.
   */
  RecallWatchDog& m_watchdog;
  
  /** logging context that will be copied by each thread for individual context */
  cta::log::LogContext m_lc;
};

}}}}

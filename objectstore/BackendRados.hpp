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

#include "Backend.hpp"
#include "rados/librados.hpp"
#include "common/threading/Mutex.hpp"
#include <future>

// RADOS_LOCKING can be NOTIFY or BACKOFF
#define BACKOFF (1)
#define NOTIFY (2)
#define RADOS_LOCKING_STRATEGY BACKOFF

// Define this to get long response times logging.
#define RADOS_SLOW_CALLS_LOGGING
#define RADOS_SLOW_CALLS_LOGGING_FILE "/var/tmp/cta-rados-slow-calls.log"
#define RADOS_SLOW_CALL_TIMEOUT 1

#ifdef RADOS_SLOW_CALLS_LOGGING
#include "common/Timer.hpp"
#include "common/threading/Mutex.hpp"
#include "common/threading/MutexLocker.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <syscall.h>
#endif //RADOS_SLOW_CALLS_LOGGING

namespace cta { namespace objectstore {
/**
 * An implementation of the object store primitives, using Rados.
 */
class BackendRados: public Backend {
public:
  class AsyncUpdater;
  friend class AsyncUpdater;
  /**
   * The constructor, connecting to the storage pool 'pool' using the user id
   * 'userId'
   * @param userId
   * @param pool
   */
  BackendRados(const std::string & userId, const std::string & pool, const std::string &radosNameSpace = "");
  ~BackendRados() override;
  std::string user() {
    return m_user;
  }
  std::string pool() {
    return m_pool;
  }
  

  void create(std::string name, std::string content) override;
  
  void atomicOverwrite(std::string name, std::string content) override;

  std::string read(std::string name) override;
  
  void remove(std::string name) override;
  
  bool exists(std::string name) override;
  
  std::list<std::string> list() override;
      
  enum class LockType {
    Shared,
    Exclusive
  };
  
  class ScopedLock: public Backend::ScopedLock {
    friend class BackendRados;
  public:
    void release() override;
  private:
    inline void releaseBackoff();
    inline void releaseNotify();
  public:
    ~ScopedLock() override;
  private:
    ScopedLock(librados::IoCtx & ioCtx): m_lockSet(false), m_context(ioCtx) {}
    void set(const std::string & oid, const std::string clientId, LockType lockType);
    bool m_lockSet;
    librados::IoCtx & m_context;
    std::string m_clientId;
    std::string m_oid;
    LockType m_lockType;
  };
  
  static const size_t c_maxBackoff;
  static const size_t c_backoffFraction;
  static const uint64_t c_maxWait;
  
private:
  static std::string createUniqueClientId();
  /** This function will lock or die (actually throw, that is) */
  void lock(std::string name, uint64_t timeout_us, LockType lockType, const std::string & clientId);
  inline void lockBackoff(std::string name, uint64_t timeout_us, LockType lockType, const std::string & clientId);
  inline void lockNotify(std::string name, uint64_t timeout_us, LockType lockType, const std::string & clientId);
  
public:  
  ScopedLock * lockExclusive(std::string name, uint64_t timeout_us=0) override;

  ScopedLock * lockShared(std::string name, uint64_t timeout_us=0) override;
private:
  /** 
   * A class for logging the calls to rados taking too long.
   * If RADOS_SLOW_CALLS_LOGGING is not defined, this is just an empty shell.
   */
  class RadosTimeoutLogger {
  public:
    void logIfNeeded(const std::string & radosCall, const std::string & objectName) {
      #ifdef RADOS_SLOW_CALLS_LOGGING
      if (m_timer.secs() >= RADOS_SLOW_CALL_TIMEOUT) {
        cta::threading::MutexLocker ml(g_mutex);
        std::ofstream logFile(RADOS_SLOW_CALLS_LOGGING_FILE, std::ofstream::app);
        std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string date=std::ctime(&end_time);
        // Chomp newline in the end
        date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());
        logFile << date << " pid=" << ::getpid() << " tid=" << syscall(SYS_gettid) << " op=" 
                << radosCall << " obj=" << objectName << " duration=" << m_timer.secs() <<  std::endl;
      }
      #endif //RADOS_SLOW_CALLS_LOGGING
    }
    void reset() {
      #ifdef RADOS_SLOW_CALLS_LOGGING
      m_timer.reset();
      #endif //RADOS_SLOW_CALLS_LOGGING
    }
  private:
    #ifdef RADOS_SLOW_CALLS_LOGGING
    cta::utils::Timer m_timer;
    static cta::threading::Mutex g_mutex;
    #endif //RADOS_SLOW_CALLS_LOGGING
  };

  /**
   * A class handling the watch part when waiting for a lock.
   */
  class LockWatcher {
  public:
    LockWatcher(librados::IoCtx & context, const std::string & name);
    virtual ~LockWatcher();
    typedef std::chrono::microseconds durationUs;
    void wait(const durationUs & timeout);
  private:
    /** An internal class containing the internals exposed to the callback of Rados.
     * The internals are kept separated so we can used asynchronous unwatch and forget
     * about the structure. The callback of aio_unwatch will take care of releasing the
     * object */
    struct Internal: public librados::WatchCtx2 {
      void handle_error(uint64_t cookie, int err) override;
      void handle_notify(uint64_t notify_id, uint64_t cookie, uint64_t notifier_id, librados::bufferlist& bl) override;
      static void deleter(librados::completion_t cb, void * i);
      // We could receive several notifications. The promise should be set only
      // on the first occurrence.
      threading::Mutex m_promiseMutex;
      bool m_promiseSet = false;
      std::promise<void> m_promise;
      std::future<void> m_future;
      RadosTimeoutLogger m_radosTimeoutLogger;
      std::string m_name;
    };
    std::unique_ptr<Internal> m_internal;
    librados::IoCtx & m_context;
    uint64_t m_watchHandle;
  };
  
public:
  /**
   * A class following up the check existence-lock-fetch-update-write-unlock. Constructor implicitly
   * starts the lock step.
   */
  class AsyncUpdater: public Backend::AsyncUpdater {
  public:
    AsyncUpdater(BackendRados & be, const std::string & name, std::function <std::string(const std::string &)> & update);
    void wait() override;
  private:
    /** A reference to the backend */
    BackendRados &m_backend;
    /** The object name */
    const std::string m_name;
    /** The operation on the object */
    std::function <std::string(const std::string &)> & m_update;
    /** Storage for stat operation (date) */
    time_t date;
    /** The promise that will both do the job and allow synchronization with the caller. */
    std::promise<void> m_job;
    /** The future from m_jobs, which will be extracted before any thread gets a chance to play with it. */
    std::future<void> m_jobFuture;
    /** A future used to hold the structure of the lock operation. It will be either empty of complete at 
     destruction time */
    std::unique_ptr<std::future<void>> m_lockAsync;
    /** A string used to identify the locker */
    std::string m_lockClient;
    /** The rados bufferlist used to hold the object data (read+write) */
    ::librados::bufferlist m_radosBufferList;
    /** A future the hole the the structure of the update operation. It will be either empty of complete at 
     destruction time */
    std::unique_ptr<std::future<void>> m_updateAsync;
    /** Async delete in case of zero sized object */
    static void deleteEmptyCallback(librados::completion_t completion, void *pThis);
    /** The second callback operation (after reading) */
    static void fetchCallback(librados::completion_t completion, void *pThis);
    /** The third callback operation (after writing) */
    static void commitCallback(librados::completion_t completion, void *pThis);
    /** The fourth callback operation (after unlocking) */
    static void unlockCallback(librados::completion_t completion, void *pThis);
    /** Instrumentation for rados calls timing */
    RadosTimeoutLogger m_radosTimeoutLogger;
  };
  
  Backend::AsyncUpdater* asyncUpdate(const std::string & name, std::function <std::string(const std::string &)> & update) override;

  /**
   * A class following up the check existence-lock-delete. 
   * Constructor implicitly starts the lock step.
   */
  class AsyncDeleter: public Backend::AsyncDeleter {
  public:
    AsyncDeleter(BackendRados & be, const std::string & name);
    void wait() override;
  private:
    /** A reference to the backend */
    BackendRados &m_backend;
    /** The object name */
    const std::string m_name;
    /** Storage for stat operation (date) */
    time_t date;
    /** The promise that will both do the job and allow synchronization with the caller. */
    std::promise<void> m_job;
    /** The future from m_jobs, which will be extracted before any thread gets a chance to play with it. */
    std::future<void> m_jobFuture;
    /** A future used to hold the structure of the lock operation. It will be either empty of complete at 
     destruction time */
    std::unique_ptr<std::future<void>> m_lockAsync;
    /** A string used to identify the locker */
    std::string m_lockClient;
    /** A future the hole the the structure of the update operation. It will be either empty of complete at 
     destruction time */
    std::unique_ptr<std::future<void>> m_updateAsync;
    /** The second callback operation (after deleting) */
    static void deleteCallback(librados::completion_t completion, void *pThis);
    /** Async delete in case of zero sized object */
    static void deleteEmptyCallback(librados::completion_t completion, void *pThis);
    /** Instrumentation for rados calls timing */
    RadosTimeoutLogger m_radosTimeoutLogger;
  };
  
  Backend::AsyncDeleter* asyncDelete(const std::string & name) override;
  
  class Parameters: public Backend::Parameters {
    friend class BackendRados;
  public:
    /**
     * The standard-issue params to string for logging
     * @return a string representation of the parameters for logging
     */
    std::string toStr() override;
    std::string toURL() override;
  private:
    std::string m_userId;
    std::string m_pool;
    std::string m_namespace;
  };
  
  Parameters * getParams() override;

  std::string typeName() override {
    return "cta::objectstore::BackendRados";
  }
  
private:
  std::string m_user;
  std::string m_pool;
  std::string m_namespace;
  librados::Rados m_cluster;
  librados::IoCtx m_radosCtx;
};

}} // end of cta::objectstore

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

#include "BackendRados.hpp"
#include "common/exception/Errnum.hpp"
#include "common/Timer.hpp"
#include "common/threading/MutexLocker.hpp"
#include "common/log/LogContext.hpp"
#include <rados/librados.hpp>
#include <sys/syscall.h>
#include <errno.h>
#include <unistd.h>
#include <valgrind/helgrind.h>
#include <random>

// This macro should be defined to get printouts to understand timings of locking.
// Usually while running BackendTestRados/BackendAbstractTest.MultithreadLockingInterface
// Also define  TEST_RADOS in objectstore/BackendRadosTestSwitch.hpp.
// Nunber of threads/passes should be reduced in the test for any usefullness.
#undef DEBUG_RADOS_LOCK_TIMINGS
#ifdef DEBUG_RADOS_LOCK_TIMINGS

namespace {
  std::atomic<double> previousSec;
  std::atomic<bool> everReleased{false};
  std::atomic<double> lastReleased;

void timestampedPrint (const char * f, const char *s) {
  struct ::timeval tv;
  ::gettimeofday(&tv, nullptr);
  double localPreviousSec=previousSec;
  double secs=previousSec=tv.tv_sec % 1000 + tv.tv_usec / 1000.0 / 1000;
  uint8_t tid = syscall(__NR_gettid) % 100;
  ::printf ("%03.06f %02.06f %02d %s %s\n", secs, secs - localPreviousSec, tid, f, s);
  ::fflush(::stdout);
}

void notifyReleased() {
  struct ::timeval tv;
  ::gettimeofday(&tv, nullptr);
  lastReleased=tv.tv_sec + tv.tv_usec / 1000.0 / 1000;
  everReleased=true;
}

void notifyLocked() {
  if (everReleased) {
    struct ::timeval tv;
    ::gettimeofday(&tv, nullptr);
    ::printf ("Relocked after %02.06f\n", (tv.tv_sec + tv.tv_usec / 1000.0 / 1000) - lastReleased);
    ::fflush(::stdout);
  }
}

}

#define TIMESTAMPEDPRINT(A) timestampedPrint(__PRETTY_FUNCTION__, (A))
#define NOTIFYLOCKED() notifyLocked()
#define NOTIFYRELEASED() notifyReleased()
#else
#define TIMESTAMPEDPRINT(A)
#define NOTIFYLOCKED()
#define NOTIFYRELEASED()
#endif

namespace cta { namespace objectstore {

cta::threading::Mutex BackendRados::RadosTimeoutLogger::g_mutex;

BackendRados::BackendRados(log::Logger & logger, const std::string & userId, const std::string & pool,
  const std::string &radosNameSpace) :
m_user(userId), m_pool(pool), m_namespace(radosNameSpace), m_cluster(), m_radosCtxPool() {
  log::LogContext lc(logger);
  cta::exception::Errnum::throwOnReturnedErrno(-m_cluster.init(userId.c_str()),
      "In ObjectStoreRados::ObjectStoreRados, failed to m_cluster.init");
  try {
    RadosTimeoutLogger rtl;
    cta::exception::Errnum::throwOnReturnedErrno(-m_cluster.conf_read_file(NULL),
        "In ObjectStoreRados::ObjectStoreRados, failed to m_cluster.conf_read_file");
    rtl.logIfNeeded("In BackendRados::BackendRados(): m_cluster.conf_read_file()", "no object");
    rtl.reset();
    cta::exception::Errnum::throwOnReturnedErrno(-m_cluster.conf_parse_env(NULL),
        "In ObjectStoreRados::ObjectStoreRados, failed to m_cluster.conf_parse_env");
    rtl.logIfNeeded("In BackendRados::BackendRados(): m_cluster.conf_parse_env()", "no object");
    rtl.reset();
    cta::exception::Errnum::throwOnReturnedErrno(-m_cluster.connect(),
        "In ObjectStoreRados::ObjectStoreRados, failed to m_cluster.connect");
    rtl.logIfNeeded("In BackendRados::BackendRados(): m_cluster.connect()", "no object");
    // Create the connection pool. One per CPU hardware thread.
    for (size_t i=0; i<std::thread::hardware_concurrency(); i++) {
      m_radosCtxPool.emplace_back(librados::IoCtx());
      log::ScopedParamContainer params(lc);
      params.add("contextId", i);
      lc.log(log::DEBUG, "BackendRados::BackendRados() about to create a new context");
      rtl.reset();
      cta::exception::Errnum::throwOnReturnedErrno(-m_cluster.ioctx_create(pool.c_str(), m_radosCtxPool.back()),
          "In ObjectStoreRados::ObjectStoreRados, failed to m_cluster.ioctx_create");
      rtl.logIfNeeded("In BackendRados::BackendRados(): m_cluster.ioctx_create()", "no object");
      lc.log(log::DEBUG, "BackendRados::BackendRados() context created. About to set namespace.");
      librados::bufferlist bl;
      rtl.reset();
      // An empty string also sets the namespace to default so no need to filter. This function does not fail.
      m_radosCtxPool.back().set_namespace(radosNameSpace);
      rtl.logIfNeeded("In BackendRados::BackendRados(): m_radosCtxPool.back().set_namespace()", "no object");
      rtl.reset();
lc.log(log::DEBUG, "BackendRados::BackendRados() namespace set. About to test access.");
      // Try to read a non-existing object through the newly created context, in hope this will protect against
      // race conditions(?) when creating the contexts in a tight loop.
      auto rc=m_radosCtxPool.back().read("TestObjectThatDoesNotNeedToExist", bl, 1, 0);
      rtl.logIfNeeded("In BackendRados::BackendRados(): m_radosCtxPool.back().read()", "TestObjectThatDoesNotNeedToExist");
      if (-rc != ENOENT) cta::exception::Errnum::throwOnReturnedErrno(-rc,
          "In ObjectStoreRados::ObjectStoreRados, failed to m_radosCtxPool.back().read(), and error is not ENOENT as expected");
    }
    {
      log::ScopedParamContainer params(lc);
      params.add("radosContexts", m_radosCtxPool.size());
      lc.log(log::DEBUG, "In BackendRados::BackendRados(): created rados contexts");
    }
    // Create the thread pool. One thread per CPU hardware thread.
    for (size_t i=0; i<std::thread::hardware_concurrency(); i++) {
      RadosWorkerThreadAndContext * rwtac = new RadosWorkerThreadAndContext(*this, i, logger);
      m_threads.push_back(rwtac);
      m_threads.back()->start();
    }
    log::ScopedParamContainer params(lc);
    params.add("workThreads", m_threads.size());
    lc.log(log::INFO, "In BackendRados::BackendRados(): created worker threads");
  } catch (...) {
    for (size_t i=0; i<m_threads.size(); i++) m_jobQueue.push(nullptr);
    for (auto &t: m_threads) {
      if (t) t->wait();
      delete t;
    }
    for(auto & c:m_radosCtxPool) {
      RadosTimeoutLogger rtl;
      c.close();
      rtl.logIfNeeded("In BackendRados::BackendRados(): m_radosCtx.close()", "no object");
    }
    RadosTimeoutLogger rtl;
    m_cluster.shutdown();
    rtl.logIfNeeded("In BackendRados::BackendRados(): m_cluster.shutdown()", "no object");
    throw;
  }
}

BackendRados::~BackendRados() {
  for (size_t i=0; i<m_threads.size(); i++) m_jobQueue.push(nullptr);
  for (auto &t: m_threads) {
    t->wait();
  }
  RadosTimeoutLogger rtl;
  for (auto &c: m_radosCtxPool) {
    rtl.reset();
    c.close();
    rtl.logIfNeeded("In BackendRados::~BackendRados(): m_radosCtx.close()", "no object");
  }
  rtl.reset();
  m_cluster.shutdown();
  rtl.logIfNeeded("In BackendRados::~BackendRados(): m_cluster.shutdown()", "no object");
}

librados::IoCtx& BackendRados::getRadosCtx() {
  threading::MutexLocker ml(m_radosCxtIndexMutex);
  auto idx=m_radosCtxIndex++;
  m_radosCtxIndex%=m_radosCtxPool.size();
  ml.unlock();
  return m_radosCtxPool[idx];
}

void BackendRados::create(std::string name, std::string content) {
  librados::ObjectWriteOperation wop;
  const bool createExclusive = true;
  wop.create(createExclusive);
  ceph::bufferlist bl;
  bl.append(content.c_str(), content.size());
  wop.write_full(bl);
  RadosTimeoutLogger rtl;
  cta::exception::Errnum::throwOnReturnedErrno(-getRadosCtx().operate(name, &wop),
      std::string("In ObjectStoreRados::create, failed to create exclusively or write: ")
      + name);
  rtl.logIfNeeded("In BackendRados::create(): m_radosCtx.operate(create+write_full)", name);
}

void BackendRados::atomicOverwrite(std::string name, std::string content) {
  librados::ObjectWriteOperation wop;
  wop.assert_exists();
  ceph::bufferlist bl;
  bl.append(content.c_str(), content.size());
  wop.write_full(bl);
  RadosTimeoutLogger rtl;
  cta::exception::Errnum::throwOnReturnedErrno(-getRadosCtx().operate(name, &wop),
      std::string("In ObjectStoreRados::atomicOverwrite, failed to assert existence or write: ")
      + name);
  rtl.logIfNeeded("In BackendRados::atomicOverwrite(): m_radosCtx.operate(assert_exists+write_full)", name);
}

std::string BackendRados::read(std::string name) {
  std::string ret;
  librados::bufferlist bl;
  RadosTimeoutLogger rtl;
  try { 
    cta::exception::Errnum::throwOnNegativeErrnoIfNegative(getRadosCtx().read(name, bl, std::numeric_limits<int32_t>::max(), 0),
        std::string("In ObjectStoreRados::read,  failed to read: ")
        + name);
  } catch (cta::exception::Errnum & e) {
    // If the object is not present, throw a more detailed exception.
    if (e.errorNumber() == ENOENT) {
      throw Backend::NoSuchObject(e.getMessageValue());
      throw;
    }
  }
  rtl.logIfNeeded("In BackendRados::read(): m_radosCtx.read()", name);
  bl.copy(0, bl.length(), ret);
  return ret;
}

void BackendRados::remove(std::string name) {
  RadosTimeoutLogger rtl;
  cta::exception::Errnum::throwOnReturnedErrno(-getRadosCtx().remove(name));
  rtl.logIfNeeded("In BackendRados::remove(): m_radosCtx.remove()", name);
}

bool BackendRados::exists(std::string name) {
  uint64_t size;
  time_t date;
  RadosTimeoutLogger rtl;
  auto statRet=getRadosCtx().stat(name, &size, &date);
  rtl.logIfNeeded("In BackendRados::exists(): m_radosCtx.stat()", name);
  if (statRet) {
    return false;
  } else {
    return true;
  }
}

std::list<std::string> BackendRados::list() {
  std::list<std::string> ret;
  auto &ctx=getRadosCtx();
  for (auto o=ctx.nobjects_begin(); o!=ctx.nobjects_end(); o++) {
    ret.push_back(o->get_oid());
  }
  return ret;
}

void BackendRados::ScopedLock::release() {
#if RADOS_LOCKING_STRATEGY == NOTIFY
  releaseNotify();
#elif RADOS_LOCKING_STRATEGY == BACKOFF
  releaseBackoff();
#else
#error Wrong value for "RADOS_LOCKING_STRATEGY"
#endif
}

void BackendRados::ScopedLock::releaseNotify() {
  if (!m_lockSet) return;
  // We should be tolerant with unlocking a deleted object, which is part
  // of the lock-remove-(implicit unlock) cycle when we delete an object
  // we hence overlook the ENOENT errors.
  TIMESTAMPEDPRINT("Pre-release");
  RadosTimeoutLogger rtl1;
  int rc=m_context.unlock(m_oid, "lock", m_clientId);
  rtl1.logIfNeeded("In BackendRados::ScopedLock::releaseNotify(): m_context.unlock()", m_oid);
  switch (-rc) {
    case ENOENT:
      break;
    default:
      cta::exception::Errnum::throwOnReturnedErrno(-rc,
        std::string("In cta::objectstore::ScopedLock::release, failed unlock: ") +
        m_oid);
      break;
  }
  NOTIFYRELEASED();
  TIMESTAMPEDPRINT("Post-release/pre-notify");
  // Notify potential waiters to take their chances now on the lock.
  librados::bufferlist bl;
  // We use a fire and forget aio call.
  librados::AioCompletion * completion = librados::Rados::aio_create_completion(nullptr, nullptr, nullptr);
  RadosTimeoutLogger rtl2;
  cta::exception::Errnum::throwOnReturnedErrno(-m_context.aio_notify(m_oid, completion, bl, 10000, nullptr), 
      "In BackendRados::ScopedLock::releaseNotify(): failed to aio_notify()");
  rtl2.logIfNeeded("In BackendRados::ScopedLock::releaseNotify(): m_context.aio_notify()", m_oid);
  completion->release();
  m_lockSet = false;
  TIMESTAMPEDPRINT("Post-notify");
}

void BackendRados::ScopedLock::releaseBackoff() {
  if (!m_lockSet) return;
  // We should be tolerant with unlocking a deleted object, which is part
  // of the lock-remove-(implicit unlock) cycle when we delete an object
  // we hence overlook the ENOENT errors.
  TIMESTAMPEDPRINT("Pre-release");
  RadosTimeoutLogger rtl1;
  int rc=m_context.unlock(m_oid, "lock", m_clientId);
  rtl1.logIfNeeded("In BackendRados::ScopedLock::releaseBackoff(): m_context.unlock()", m_oid);
  switch (-rc) {
    case ENOENT:
      break;
    default:
      cta::exception::Errnum::throwOnReturnedErrno(-rc,
        std::string("In cta::objectstore::ScopedLock::releaseBackoff, failed unlock: ") +
        m_oid);
      break;
  }
  NOTIFYRELEASED();
  TIMESTAMPEDPRINT("Post-release");
}


void BackendRados::ScopedLock::set(const std::string& oid, const std::string clientId, 
    LockType lockType) {
  m_oid = oid;
  m_clientId = clientId;
  m_lockType = lockType;
  m_lockSet = true;
}

BackendRados::ScopedLock::~ScopedLock() {
  release();
}

BackendRados::LockWatcher::LockWatcher(librados::IoCtx& context, const std::string& name):
  m_context(context) {
  m_internal.reset(new Internal);
  m_internal->m_name = name;
  m_internal->m_future = m_internal->m_promise.get_future();
  TIMESTAMPEDPRINT("Pre-watch2");
  RadosTimeoutLogger rtl;
  cta::exception::Errnum::throwOnReturnedErrno(-m_context.watch2(name, &m_watchHandle, m_internal.get()));
  rtl.logIfNeeded("In BackendRados::LockWatcher::LockWatcher(): m_context.watch2()", name);
  TIMESTAMPEDPRINT("Post-watch2");
}

void BackendRados::LockWatcher::Internal::handle_error(uint64_t cookie, int err) {
  threading::MutexLocker ml(m_promiseMutex);
  if (m_promiseSet) return;
  m_promise.set_value();
  TIMESTAMPEDPRINT("Handled notify");
  m_promiseSet = true;
}

void BackendRados::LockWatcher::Internal::handle_notify(uint64_t notify_id, uint64_t cookie, uint64_t notifier_id, librados::bufferlist& bl) {
  threading::MutexLocker ml(m_promiseMutex);
  if (m_promiseSet) return;
  m_promise.set_value();
  TIMESTAMPEDPRINT("Handled notify");
  m_promiseSet = true;
}

void BackendRados::LockWatcher::wait(const durationUs& timeout) {
  TIMESTAMPEDPRINT("Pre-wait"); 
  m_internal->m_future.wait_for(timeout);
  TIMESTAMPEDPRINT("Post-wait");
}

BackendRados::LockWatcher::~LockWatcher() {
  TIMESTAMPEDPRINT("Pre-aio_unwatch2");
  m_internal->m_radosTimeoutLogger.reset();
  auto name=m_internal->m_name;
  librados::AioCompletion *completion = librados::Rados::aio_create_completion(m_internal.release(), nullptr, Internal::deleter);
  RadosTimeoutLogger rtl;
  m_context.aio_unwatch(m_watchHandle, completion);
  completion->release();
  rtl.logIfNeeded("In BackendRados::LockWatcher::~LockWatcher(): m_context.aio_unwatch() call", name);
  TIMESTAMPEDPRINT("Post-aio_unwatch2");
}

void BackendRados::LockWatcher::Internal::deleter(librados::completion_t cb, void* i) {
  TIMESTAMPEDPRINT("Unwatch2 completion");
  std::unique_ptr<Internal> internal((Internal *) i);
  internal->m_radosTimeoutLogger.logIfNeeded("BackendRados::LockWatcher::Internal::deleter(): aio_unwatch callback", internal->m_name);
}

std::string BackendRados::createUniqueClientId() {
  // Build a unique client name: host:thread
  char buff[200];
  cta::exception::Errnum::throwOnMinusOne(gethostname(buff, sizeof (buff)),
      "In ObjectStoreRados::lockExclusive:  failed to gethostname");
  pid_t tid = syscall(SYS_gettid);
  std::stringstream client;
  client << buff << ":" << tid;
  return client.str();
}

void BackendRados::lock(std::string name, uint64_t timeout_us, LockType lockType, const std::string& clientId) {
#if RADOS_LOCKING_STRATEGY == NOTIFY
  lockNotify(name, timeout_us, lockType, clientId, getRadosCtx());
#elif RADOS_LOCKING_STRATEGY == BACKOFF
  lockBackoff(name, timeout_us, lockType, clientId, getRadosCtx());
#else
#error Wrong value for "RADOS_LOCKING_STRATEGY"
#endif
}

void BackendRados::lockWithIoContext(std::string name, uint64_t timeout_us, LockType lockType, const std::string& clientId, librados::IoCtx & radosCtx) {
#if RADOS_LOCKING_STRATEGY == NOTIFY
  lockNotify(name, timeout_us, lockType, clientId, radosCtx);
#elif RADOS_LOCKING_STRATEGY == BACKOFF
  lockBackoff(name, timeout_us, lockType, clientId, radosCtx);
#else
#error Wrong value for "RADOS_LOCKING_STRATEGY"
#endif
}

void BackendRados::lockNotify(std::string name, uint64_t timeout_us, LockType lockType, 
    const std::string & clientId, librados::IoCtx & radosCtx) {
  // In Rados, locking a non-existing object will create it. This is not our intended
  // behavior. We will lock anyway, test the object and re-delete it if it has a size of 0 
  // (while we own the lock).
  struct timeval tv;
  tv.tv_usec = 0;
  tv.tv_sec = 240;
  int rc;
  utils::Timer timeoutTimer;
  while (true) {
    TIMESTAMPEDPRINT(lockType==LockType::Shared?"Pre-lock (shared)":"Pre-lock (exclusive)");
    RadosTimeoutLogger rtl;
    if (lockType==LockType::Shared) {
      rc = radosCtx.lock_shared(name, "lock", clientId, "", "", &tv, 0);
      rtl.logIfNeeded("In BackendRados::lockNotify(): m_radosCtx.lock_shared()", name);
    } else {
      rc = radosCtx.lock_exclusive(name, "lock", clientId, "", &tv, 0);
      rtl.logIfNeeded("In BackendRados::lockNotify(): m_radosCtx.lock_exclusive()", name);
    }
    if (!rc) {
      TIMESTAMPEDPRINT(lockType==LockType::Shared?"Post-lock (shared) (got it)":"Post-lock (exclusive) (got it)");
      NOTIFYLOCKED();
    } else {
      TIMESTAMPEDPRINT("Post-lock");
    }
    if (-EBUSY != rc) break;
    // The lock is taken. Start a watch on it immediately. Inspired from the algorithm listed her:
    // https://zookeeper.apache.org/doc/r3.1.2/recipes.html#sc_recipes_Locks
    TIMESTAMPEDPRINT(lockType==LockType::Shared?"Pre-watch-setup (shared)":"Pre-watch-setup (exclusive)");
    LockWatcher watcher(radosCtx, name);
    TIMESTAMPEDPRINT(lockType==LockType::Shared?"Post-watch-setup/Pre-relock (shared)":"Post-watch-setup/Pre-relock (exclusive)");
    // We need to retry the lock after establishing the watch: it could have been released during that time.
    rtl.reset();
    if (lockType==LockType::Shared) {
      rc = radosCtx.lock_shared(name, "lock", clientId, "", "", &tv, 0);
      rtl.logIfNeeded("In BackendRados::lockNotify(): m_radosCtx.lock_shared()", name);
    } else {
      rc = radosCtx.lock_exclusive(name, "lock", clientId, "", &tv, 0);
      rtl.logIfNeeded("In BackendRados::lockNotify(): m_radosCtx.lock_exclusive()", name);
    }
    if (!rc) {
      TIMESTAMPEDPRINT(lockType==LockType::Shared?"Post-relock (shared) (got it)":"Post-relock (exclusive) (got it)");
      NOTIFYLOCKED();
    } else {
      TIMESTAMPEDPRINT(lockType==LockType::Shared?"Post-relock (shared)":"Post-relock (exclusive)");
    }
    if (-EBUSY != rc) break;
    LockWatcher::durationUs watchTimeout = LockWatcher::durationUs(15L * 1000 * 1000); // We will poll at least every 15 second.
    // If we are dealing with a user-defined timeout, take it into account.
    if (timeout_us) {
      watchTimeout = std::min(watchTimeout, 
          LockWatcher::durationUs(timeout_us) - LockWatcher::durationUs(timeoutTimer.usecs()));
      // Make sure the value makes sense if we just crossed the deadline.
      watchTimeout = std::max(watchTimeout, LockWatcher::durationUs(1));
    }
    watcher.wait(watchTimeout);
    TIMESTAMPEDPRINT(lockType==LockType::Shared?"Post-wait (shared)":"Post-wait (exclusive)");
    if (timeout_us && (timeoutTimer.usecs() > (int64_t)timeout_us)) {
      throw exception::Exception("In BackendRados::lockNotify(): timeout.");
    }
  }
  cta::exception::Errnum::throwOnReturnedErrno(-rc,
      std::string("In ObjectStoreRados::lock(), failed to librados::IoCtx::") + 
      (lockType==LockType::Shared?"lock_shared: ":"lock_exclusive: ") +
      name + "/" + "lock" + "/" + clientId + "//");
  // We could have created an empty object by trying to lock it. We can find this out: if the object is
  // empty, we should delete it and throw an exception.
  // Get the size:
  uint64_t size;
  time_t date;
  cta::exception::Errnum::throwOnReturnedErrno (-radosCtx.stat(name, &size, &date),
      std::string("In ObjectStoreRados::lock, failed to librados::IoCtx::stat: ") +
      name + "/" + "lock" + "/" + clientId + "//");
  if (!size) {
    // The object has a zero size: we probably created it by attempting the locking.
    cta::exception::Errnum::throwOnReturnedErrno (-radosCtx.remove(name),
        std::string("In ObjectStoreRados::lock, failed to librados::IoCtx::remove: ") +
        name + "//");
    throw Backend::NoSuchObject(std::string("In BackendRados::lockWatch(): "
        "trying to lock a non-existing object: ") + name);
  }
}

void BackendRados::lockBackoff(std::string name, uint64_t timeout_us, LockType lockType,
    const std::string& clientId, librados::IoCtx & radosCtx) {
  // In Rados, locking a non-existing object will create it. This is not our intended
  // behavior. We will lock anyway, test the object and re-delete it if it has a size of 0 
  // (while we own the lock).
  struct timeval tv;
  tv.tv_usec = 0;
  tv.tv_sec = 240;
  int rc;
  // Crude backoff: we will measure the RTT of the call and backoff a faction of this amount multiplied
  // by the number of tries (and capped by a maximum). Then the value will be randomized 
  // (betweend and 50-150%)
  size_t backoff=1;
  utils::Timer t, timeoutTimer;
  while (true) {
    TIMESTAMPEDPRINT(lockType==LockType::Shared?"Pre-lock (shared)":"Pre-lock (exclusive)");
    RadosTimeoutLogger rtl;
    if (lockType==LockType::Shared) {
      rc = radosCtx.lock_shared(name, "lock", clientId, "", "", &tv, 0);
      rtl.logIfNeeded("In BackendRados::lockBackoff(): m_radosCtx.lock_shared()", name);
    } else {
      rc = radosCtx.lock_exclusive(name, "lock", clientId, "", &tv, 0);
      rtl.logIfNeeded("In BackendRados::lockBackoff(): m_radosCtx.lock_exclusive()", name);
    }
    if (!rc) {
      TIMESTAMPEDPRINT(lockType==LockType::Shared?"Post-lock (shared) (got it)":"Post-lock (exclusive) (got it)");
      NOTIFYLOCKED();
    } else {
      TIMESTAMPEDPRINT("Post-lock");
    }
    if (-EBUSY != rc) break;
    if (timeout_us && (timeoutTimer.usecs() > (int64_t)timeout_us)) {
      throw exception::Exception("In BackendRados::lockBackoff(): timeout.");
    }
    timespec ts;
    auto wait=t.usecs(utils::Timer::resetCounter)*backoff++/c_backoffFraction;
    wait = std::min(wait, c_maxWait);
    if (backoff>c_maxBackoff) backoff=1;
    // We need to get a random number [50, 150]
    std::default_random_engine dre(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<size_t> distribution(50, 150);
    decltype(wait) randFactor=distribution(dre);
    wait=(wait * randFactor)/100;
    ts.tv_sec = wait/(1000*1000);
    ts.tv_nsec = (wait % (1000*1000)) * 1000;
    nanosleep(&ts, nullptr);
  }
  cta::exception::Errnum::throwOnReturnedErrno(-rc,
      std::string("In ObjectStoreRados::lock(), failed to librados::IoCtx::") + 
      (lockType==LockType::Shared?"lock_shared: ":"lock_exclusive: ") +
      name + "/" + "lock" + "/" + clientId + "//");
  // We could have created an empty object by trying to lock it. We can find this out: if the object is
  // empty, we should delete it and throw an exception.
  // Get the size:
  uint64_t size;
  time_t date;
  cta::exception::Errnum::throwOnReturnedErrno (-radosCtx.stat(name, &size, &date),
      std::string("In ObjectStoreRados::lockBackoff, failed to librados::IoCtx::stat: ") +
      name + "/" + "lock" + "/" + clientId + "//");
  if (!size) {
    // The object has a zero size: we probably created it by attempting the locking.
    cta::exception::Errnum::throwOnReturnedErrno (-radosCtx.remove(name),
        std::string("In ObjectStoreRados::lockBackoff, failed to librados::IoCtx::remove: ") +
        name + "//");
    throw Backend::NoSuchObject(std::string("In BackendRados::lockBackoff(): "
        "trying to lock a non-existing object: ") + name);
  }
}


BackendRados::ScopedLock* BackendRados::lockExclusive(std::string name, uint64_t timeout_us) {
  std::string client = createUniqueClientId();
  lock(name, timeout_us, LockType::Exclusive, client);
  std::unique_ptr<ScopedLock> ret(new ScopedLock(getRadosCtx()));
  ret->set(name, client, LockType::Exclusive);
  return ret.release();
}

BackendRados::ScopedLock* BackendRados::lockShared(std::string name, uint64_t timeout_us) {
   std::string client = createUniqueClientId();
  lock(name, timeout_us, LockType::Shared, client);
  std::unique_ptr<ScopedLock> ret(new ScopedLock(getRadosCtx()));
  ret->set(name, client, LockType::Shared);
  return ret.release();
}

BackendRados::RadosWorkerThreadAndContext::RadosWorkerThreadAndContext(BackendRados & parentBackend, 
  int threadID, log::Logger & logger): 
  m_parentBackend(parentBackend), m_threadID(threadID), m_lc(logger) {
  log::ScopedParamContainer params(m_lc);
  m_lc.pushOrReplace(log::Param("thread", "radosWorker"));
  m_lc.pushOrReplace(log::Param("threadID", threadID));
  m_lc.log(log::INFO, "Started Rados worker thread");
}

BackendRados::RadosWorkerThreadAndContext::~RadosWorkerThreadAndContext() {
  m_lc.log(log::INFO, "Rados worker thread complete");
}

void BackendRados::RadosWorkerThreadAndContext::run() {
  while (1) {
    BackendRados::AsyncJob * j=m_parentBackend.m_jobQueue.pop();
    if (j) {
      j->execute();
    } else {
      break;
}
  }
}

Backend::AsyncUpdater* BackendRados::asyncUpdate(const std::string & name, std::function <std::string(const std::string &)> & update)
{
  return new AsyncUpdater(*this, name, update);
}

BackendRados::AsyncUpdater::AsyncUpdater(BackendRados& be, const std::string& name, std::function<std::string(const std::string&)>& update):
  m_backend(be), m_name(name), m_update(update), m_job(), m_jobFuture(m_job.get_future()) {
  m_updateJob.setParentUpdater(this);
  // At construction time, we just fire a lock.
  try {
    // Rados does not have aio_lock, so we do it in an async.
    // Operation is lock (synchronous), and then launch an async stat, then read.
    // The async function never fails, exceptions go to the promise (as everywhere).
    m_lockAsync.reset(new std::future<void>(std::async(std::launch::async,
        [this](){
          try {
            m_lockClient = BackendRados::createUniqueClientId();
            m_backend.lock(m_name, 300*1000, BackendRados::LockType::Exclusive, m_lockClient);
            // Locking is done, we can launch the read operation (async).
            librados::AioCompletion * aioc = librados::Rados::aio_create_completion(this, fetchCallback, nullptr);
            RadosTimeoutLogger rtl;
            m_radosTimeoutLogger.reset();
            auto rc=m_backend.getRadosCtx().aio_read(m_name, aioc, &m_radosBufferList, std::numeric_limits<int32_t>::max(), 0);
            rtl.logIfNeeded("BackendRados::AsyncUpdater::AsyncUpdater::lock_lambda(): m_radosCtx.aio_read()", m_name);
            aioc->release();
            if (rc) {
              cta::exception::Errnum errnum (-rc, std::string("In BackendRados::AsyncUpdater::AsyncUpdater::lock_lambda(): failed to launch aio_read(): ")+m_name);
              throw Backend::NoSuchObject(errnum.getMessageValue());
            }
          } catch (...) {
            ANNOTATE_HAPPENS_BEFORE(&m_job);
            m_job.set_exception(std::current_exception());
          }
        }
        )));
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&m_job);
    m_job.set_exception(std::current_exception());
  }
}

void BackendRados::AsyncUpdater::deleteEmptyCallback(librados::completion_t completion, void* pThis) {
  AsyncUpdater & au = *((AsyncUpdater *) pThis);
  try {
    // Check that the object could be deleted.
    if (rados_aio_get_return_value(completion)) {
      cta::exception::Errnum errnum(-rados_aio_get_return_value(completion),
          std::string("In BackendRados::AsyncUpdater::deleteEmptyCallback(): could not delete object: ") + au.m_name);
      throw Backend::CouldNotDelete(errnum.getMessageValue());
    }
    // object deleted then throw an exception
    throw Backend::NoSuchObject(std::string("In BackendRados::AsyncUpdater::deleteEmptyCallback(): no such object: ") + au.m_name);
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_exception(std::current_exception());
  }
}

void BackendRados::AsyncUpdater::fetchCallback(librados::completion_t completion, void* pThis) {
  AsyncUpdater & au = *((AsyncUpdater *) pThis);
  au.m_radosTimeoutLogger.logIfNeeded("In BackendRados::AsyncUpdater::fetchCallback(): aio_read callback", au.m_name);
  try {
    // Check that the object could be read.
    __attribute__((unused)) auto rados_ret=rados_aio_get_return_value(completion);
    if (rados_aio_get_return_value(completion)<0) {
      cta::exception::Errnum errnum(-rados_aio_get_return_value(completion),
          std::string("In BackendRados::AsyncUpdater::fetchCallback(): could not read object: ") + au.m_name);
      throw Backend::CouldNotFetch(errnum.getMessageValue());
    }
    // We can now launch the update operation (post to thread pool).
    au.m_backend.m_jobQueue.push(&au.m_updateJob);
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_exception(std::current_exception());
  }
}

void BackendRados::AsyncUpdater::UpdateJob::execute() {
  AsyncUpdater & au = *m_parentUpdater;
  try {
    // The data is in the buffer list.
    std::string value;
    try {
      au.m_radosBufferList.copy(0, au.m_radosBufferList.length(), value);
    } catch (std::exception & ex) {
      throw CouldNotUpdateValue(
          std::string("In In BackendRados::AsyncUpdater::fetchCallback::update_lambda(): failed to read buffer: ") +
          au.m_name + ": "+ ex.what());
    }

    bool updateWithDelete = false;
    try {      
      // Execute the user's callback.
      value=au.m_update(value);
    } catch (AsyncUpdateWithDelete & ex) {
      updateWithDelete = true;               
    } catch (...) {
      // Let exceptions fly through. User knows his own exceptions.
      throw; 
    }

    if(updateWithDelete) {
      try {
        au.m_backend.remove(au.m_name);
        if (au.m_backend.exists(au.m_name)) {
          throw exception::Exception("Object exists after remove");
        }
      } catch (cta::exception::Exception &ex) {
        throw CouldNotUpdateValue(
            std::string("In BackendRados::AsyncUpdater::fetchCallback::update_lambda(): failed to remove value: ") +
            au.m_name + ex.what());
      }
      // Done!
      ANNOTATE_HAPPENS_BEFORE(&au.m_job);
      au.m_job.set_value();
    } else {
      try {
        // Prepare result in buffer list.
        au.m_radosBufferList.clear();
        au.m_radosBufferList.append(value);
      } catch (std::exception & ex) {
        throw CouldNotUpdateValue(
            std::string("In In BackendRados::AsyncUpdater::fetchCallback::update_lambda(): failed to prepare write buffer(): ") + 
            au.m_name + ex.what());
      }
      // Launch the write
      librados::AioCompletion * aioc = librados::Rados::aio_create_completion(m_parentUpdater, commitCallback, nullptr);
      RadosTimeoutLogger rtl;
      au.m_radosTimeoutLogger.reset();
      auto rc=au.m_backend.getRadosCtx().aio_write_full(au.m_name, aioc, au.m_radosBufferList);
      rtl.logIfNeeded("In BackendRados::AsyncUpdater::fetchCallback::update_lambda(): m_radosCtx.aio_write_full() call", au.m_name);
      aioc->release();
      if (rc) {
        cta::exception::Errnum errnum (-rc, 
          "In BackendRados::AsyncUpdater::fetchCallback::update_lambda(): failed to launch aio_write_full()" + au.m_name);
        throw Backend::CouldNotCommit(errnum.getMessageValue());
      }
    }
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_exception(std::current_exception());
  }
}

void BackendRados::AsyncUpdater::commitCallback(librados::completion_t completion, void* pThis) {
  AsyncUpdater & au = *((AsyncUpdater *) pThis);
  au.m_radosTimeoutLogger.logIfNeeded("In BackendRados::AsyncUpdater::commitCallback(): aio_write_full() callback", au.m_name);
  try {
    // Check that the object could be written.
    if (rados_aio_get_return_value(completion)) {
      cta::exception::Errnum errnum(-rados_aio_get_return_value(completion),
          std::string("In BackendRados::AsyncUpdater::commitCallback(): could not write object: ")+au.m_name);
      throw Backend::CouldNotCommit(errnum.getMessageValue());
    }
    // Launch the async unlock.
    librados::AioCompletion * aioc = librados::Rados::aio_create_completion(pThis, unlockCallback, nullptr);
    au.m_radosTimeoutLogger.reset();
    RadosTimeoutLogger rtl;
    auto rc=au.m_backend.getRadosCtx().aio_unlock(au.m_name, "lock", au.m_lockClient, aioc);
    rtl.logIfNeeded("In BackendRados::AsyncUpdater::commitCallback(): m_radosCtx.aio_unlock", au.m_name);
    aioc->release();
    if (rc) {
      cta::exception::Errnum errnum (-rc, std::string("In BackendRados::AsyncUpdater::commitCallback(): failed to launch aio_unlock()")+au.m_name);
      throw Backend::CouldNotUnlock(errnum.getMessageValue());
    }
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_exception(std::current_exception());
  }
}

void BackendRados::AsyncUpdater::unlockCallback(librados::completion_t completion, void* pThis) {
  AsyncUpdater & au = *((AsyncUpdater *) pThis);
  au.m_radosTimeoutLogger.logIfNeeded("In BackendRados::AsyncUpdater::unlockCallback(): aio_unlock() callback", au.m_name);
  try {
    // Check that the object could be unlocked.
    if (rados_aio_get_return_value(completion)) {
      cta::exception::Errnum errnum(-rados_aio_get_return_value(completion),
          std::string("In BackendRados::AsyncUpdater::unlockCallback(): could not unlock object: ")+au.m_name);
      throw Backend::CouldNotUnlock(errnum.getMessageValue());
    }
    // Done!
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_value();
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_exception(std::current_exception());
  }
}

void BackendRados::AsyncUpdater::wait() {
  m_jobFuture.get();
  ANNOTATE_HAPPENS_AFTER(&m_job);
  ANNOTATE_HAPPENS_BEFORE_FORGET_ALL(&m_job);
}


Backend::AsyncDeleter* BackendRados::asyncDelete(const std::string & name)
{
  return new AsyncDeleter(*this, name);
}

BackendRados::AsyncDeleter::AsyncDeleter(BackendRados& be, const std::string& name):
  m_backend(be), m_name(name), m_job(), m_jobFuture(m_job.get_future()) {
  // At construction time, we just fire a lock.
  try {
    // Rados does not have aio_lock, so we do it in an async.
    // Operation is lock (synchronous), and then launch an async stat, then read.
    // The async function never fails, exceptions go to the promise (as everywhere).
    m_lockAsync.reset(new std::future<void>(std::async(std::launch::async,
        [this](){
          try {
            m_lockClient = BackendRados::createUniqueClientId();
            m_backend.lock(m_name, 300*1000, BackendRados::LockType::Exclusive, m_lockClient);
            // Locking is done, we can launch the remove operation (async).
            librados::AioCompletion * aioc = librados::Rados::aio_create_completion(this, deleteCallback, nullptr);
            m_radosTimeoutLogger.reset();
            RadosTimeoutLogger rtl;
            auto rc=m_backend.getRadosCtx().aio_remove(m_name, aioc);
            rtl.logIfNeeded("In BackendRados::AsyncDeleter::AsyncDeleter(): m_radosCtx.aio_remove() call", m_name);
            aioc->release();
            if (rc) {
              cta::exception::Errnum errnum (-rc, std::string("In BackendRados::AsyncUpdater::statCallback(): failed to launch aio_remove(): ")+m_name);
              throw Backend::CouldNotDelete(errnum.getMessageValue());
            }
          } catch (...) {
            ANNOTATE_HAPPENS_BEFORE(&m_job);
            m_job.set_exception(std::current_exception());
          }
        }
        )));
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&m_job);
    m_job.set_exception(std::current_exception());
  }
}

void BackendRados::AsyncDeleter::deleteCallback(librados::completion_t completion, void* pThis) {
  AsyncDeleter & au = *((AsyncDeleter *) pThis);
  au.m_radosTimeoutLogger.logIfNeeded("In BackendRados::AsyncDeleter::deleteCallback(): aio_remove() callback", au.m_name);
  try {
    // Check that the object could be deleted.
    if (rados_aio_get_return_value(completion)) {
      cta::exception::Errnum errnum(-rados_aio_get_return_value(completion),
          std::string("In BackendRados::AsyncDeleter::deleteCallback(): could not delete object: ") + au.m_name);
      throw Backend::CouldNotDelete(errnum.getMessageValue());
    }
    // Done!
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_value();
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_exception(std::current_exception());
  }
}

Backend::AsyncLockfreeFetcher* BackendRados::asyncLockfreeFetch(const std::string& name) {
  return new AsyncLockfreeFetcher(*this, name);
}

void BackendRados::AsyncDeleter::wait() {
  m_jobFuture.get();
  ANNOTATE_HAPPENS_AFTER(&m_job);
  ANNOTATE_HAPPENS_BEFORE_FORGET_ALL(&m_job);
}

BackendRados::AsyncLockfreeFetcher::AsyncLockfreeFetcher(BackendRados& be, const std::string& name):
  m_backend(be), m_name(name), m_job(), m_jobFuture(m_job.get_future()) {
  // At construction time, we just start the read.
  librados::AioCompletion * aioc = librados::Rados::aio_create_completion(this, fetchCallback, nullptr);
  RadosTimeoutLogger rtl;
  m_radosTimeoutLogger.reset();
  auto rc=m_backend.getRadosCtx().aio_read(m_name, aioc, &m_radosBufferList, std::numeric_limits<int32_t>::max(), 0);
  rtl.logIfNeeded("BackendRados::AsyncLockfreeFetcher::AsyncLockfreeFetcher(): m_radosCtx.aio_read()", m_name);
  aioc->release();
  if (rc) {
    cta::exception::Errnum errnum (-rc, std::string("In BackendRados::AsyncLockfreeFetcher::AsyncLockfreeFetcher(): failed to launch aio_read(): ")+m_name);
    throw Backend::NoSuchObject(errnum.getMessageValue());
  }
}

void BackendRados::AsyncLockfreeFetcher::fetchCallback(librados::completion_t completion, void* pThis) {
  AsyncLockfreeFetcher & au = *((AsyncLockfreeFetcher *) pThis);
  au.m_radosTimeoutLogger.logIfNeeded("In BackendRados::AsyncLockfreeFetcher::fetchCallback(): aio_read callback", au.m_name);
  try {
    // Check that the object could be read.
    if (rados_aio_get_return_value(completion)<0) {
      cta::exception::Errnum errnum(-rados_aio_get_return_value(completion),
          std::string("In BackendRados::AsyncLockfreeFetcher::fetchCallback(): could not read object: ") + au.m_name);
      if (errnum.errorNumber() == ENOENT) {
        throw Backend::NoSuchObject(errnum.getMessageValue());
      }
      throw Backend::CouldNotFetch(errnum.getMessageValue());
    }
    // The data is in the buffer list.
    std::string value;
    try {
      au.m_radosBufferList.copy(0, au.m_radosBufferList.length(), value);
    } catch (std::exception & ex) {
      throw CouldNotUpdateValue(
          std::string("In In BackendRados::AsyncUpdater::fetchCallback::update_lambda(): failed to read buffer: ") +
          au.m_name + ": "+ ex.what());
    }
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_value(value);
  } catch (...) {
    ANNOTATE_HAPPENS_BEFORE(&au.m_job);
    au.m_job.set_exception(std::current_exception());
  } 
}

std::string BackendRados::AsyncLockfreeFetcher::wait() {
  auto ret=m_jobFuture.get();
  ANNOTATE_HAPPENS_AFTER(&m_job);
  ANNOTATE_HAPPENS_BEFORE_FORGET_ALL(&m_job);
  return ret;
}


std::string BackendRados::Parameters::toStr() {
  std::stringstream ret;
  ret << "userId=" << m_userId << " pool=" << m_pool;
  return ret.str();
}

std::string BackendRados::Parameters::toURL() {
  std::stringstream ret;
  ret << "rados://" << m_userId << "@" << m_pool;
  if (m_namespace.size())
    ret << ":" << m_namespace;
  return ret.str();
}


BackendRados::Parameters* BackendRados::getParams() {
  std::unique_ptr<Parameters> ret(new Parameters);
  ret->m_pool = m_pool;
  ret->m_userId = m_user;
  ret->m_namespace = m_namespace;
  return ret.release();
}

const size_t BackendRados::c_maxBackoff=32;
const size_t BackendRados::c_backoffFraction=4;
const uint64_t BackendRados::c_maxWait=1000000;

}}

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

#include "SignalHandler.hpp"
#include "ProcessManager.hpp"
#include "common/exception/Errnum.hpp"
#include "common/log/LogContext.hpp"
#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <list>

namespace cta {
namespace tape {
namespace daemon {

SignalHandler::SignalHandler(ProcessManager& pm):
SubprocessHandler("signalHandler"), m_processManager(pm) {
  // Block the signals we want to handle.
  ::sigset_t sigMask;
  ::sigemptyset(&sigMask);
  std::list<int> sigLis = { SIGHUP, SIGINT,SIGQUIT, SIGPIPE, SIGTERM, SIGUSR1, 
    SIGUSR2, SIGCHLD, SIGTSTP, SIGTTIN, SIGTTOU, SIGPOLL, SIGURG, SIGVTALRM };
  for (auto sig: sigLis) ::sigaddset(&sigMask, sig);
  cta::exception::Errnum::throwOnNonZero(::sigprocmask(SIG_BLOCK, &sigMask, nullptr),
      "In SignalHandler::SignalHandler(): sigprocmask() failed");
  // Create the signalfd. We will poll so we should never read uselessly => NONBLOCK will prevent
  // being blocked in case of issue.
  m_sigFd = ::signalfd(-1 ,&sigMask, SFD_NONBLOCK);
  cta::exception::Errnum::throwOnMinusOne(m_sigFd, 
      "In SignalHandler::SignalHandler(): signalfd() failed");
  // We can already register the file descriptor
  m_processManager.addFile(m_sigFd, this);
}

SignalHandler::~SignalHandler() {
  // Deregister the file descriptor from poll
  m_processManager.removeFile(m_sigFd);
  // And close the file
  ::close(m_sigFd);
}

SubprocessHandler::ProcessingStatus SignalHandler::getInitialStatus() {
  // On initiation, we expect nothing but signals, i.e. default status
  // except we are considered shutdown at all times.
  SubprocessHandler::ProcessingStatus ret;
  ret.shutdownComplete = true;
  return ret;
}

SubprocessHandler::ProcessingStatus SignalHandler::processEvent() {
  // We have a signal
  struct ::signalfd_siginfo sigInf;
  int rc=::read(m_sigFd, &sigInf, sizeof(sigInf));
  // We should always get something here. As we set the NONBLOCK option, lack 
  // of signal here will lead to EAGAIN, which is also an error.
  cta::exception::Errnum::throwOnMinusOne(rc);
  // We should get exactly sizeof(sigInf) bytes.
  if (rc!=sizeof(sigInf)) {
    std::stringstream err;
    err << "In SignalHandler::processEvent(): unexpected size for read: "
        "got=" << rc << " expected=" << sizeof(sigInf);
    throw cta::exception::Exception(err.str());
  }
  // Prepare logging the signal
  cta::log::ScopedParamContainer params(m_processManager.logContext());
  if (char * signalName = ::strsignal(sigInf.ssi_signo)) {
    params.add("signal", signalName);
  } else {
    params.add("signal", sigInf.ssi_signo);
  }
  params.add("senderPID", sigInf.ssi_pid)
     .add("senderUID", sigInf.ssi_uid);
  // Handle the signal
  switch (sigInf.ssi_signo) {
  case SIGHUP:
  case SIGQUIT:
  case SIGPIPE:
  case SIGUSR1:
  case SIGUSR2:
  case SIGTSTP:
  case SIGTTIN:
  case SIGTTOU:
  case SIGPOLL:
  case SIGURG:
  case SIGVTALRM:
  {
    
    m_processManager.logContext().log(log::INFO, "In signal handler, ignoring signal");
    break;
  }
  case SIGINT:
  case SIGTERM:
    // We will now require shutdown (if not already done)
    // record the time to define timeout.
    if (!m_shutdownRequested) {
      m_shutdownRequested=true;
      m_shutdownStartTime=std::chrono::steady_clock::now();
      m_processManager.logContext().log(log::INFO, "In signal handler, initiating shutdown");
    } else {
      m_processManager.logContext().log(log::INFO, "In signal handler, shutdown already initiated: ignoring");
    }
    break;
  case SIGCHLD:
    // We will request the processing of sigchild until it is acknowledged (by receiving
    // it ourselves)
    m_sigChildPending = true;
    m_processManager.logContext().log(log::INFO, "In signal handler, received SIGCHLD and propagations to other handlers");
    break;
  }
  SubprocessHandler::ProcessingStatus ret;
  // If the shutdown was not acknowledged (by receiving it ourselves), we ask
  // for it
  ret.shutdownRequested = m_shutdownRequested && !m_shutdownAcknowlegded;
  ret.nextTimeout = m_shutdownStartTime+m_timeoutDuration;
  ret.sigChild = m_sigChildPending;
  ret.shutdownComplete = true; // We are always ready to leave.
  return ret;
}

void SignalHandler::kill() {
  // We have nothing to kill: noop.
}

void SignalHandler::prepareForFork() {
  // We have nothing to do: noop.
}

SubprocessHandler::ProcessingStatus SignalHandler::processTimeout() {
  // If we reach timeout, it means it's time to kill child processes
  SubprocessHandler::ProcessingStatus ret;
  ret.killRequested = true;
  ret.shutdownComplete = true;
  return ret;
}

SubprocessHandler::ProcessingStatus SignalHandler::processSigChild() {
  // Our sigchild is now acknowledged
  m_sigChildPending = false;
  SubprocessHandler::ProcessingStatus ret;
  ret.shutdownRequested = m_shutdownRequested;
  ret.shutdownComplete = true;
  if (m_shutdownRequested) {
    ret.nextTimeout = m_shutdownStartTime+m_timeoutDuration;
  }
  return ret;
}


SubprocessHandler::ProcessingStatus SignalHandler::shutdown() {
  // We received (back) our own shutdown: consider it acknowledged
  m_shutdownAcknowlegded = true;
  SubprocessHandler::ProcessingStatus ret;
  ret.shutdownComplete = true;
  // if we ever asked for shutdown, we have a timeout
  if (m_shutdownRequested) {
    ret.nextTimeout = m_shutdownStartTime+m_timeoutDuration;
  }
  ret.sigChild = m_sigChildPending;
  return ret;
}

SubprocessHandler::ProcessingStatus SignalHandler::fork() {
  throw cta::exception::Exception("Unexpected call to SignalHandler::fork()");
}

int SignalHandler::runChild() {
  throw cta::exception::Exception("Unexpected call to SignalHandler::runChild()");
}



}}} // namespace cta::tape::daemon

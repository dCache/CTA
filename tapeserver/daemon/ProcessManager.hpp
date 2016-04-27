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

#include "SubprocessHandler.hpp"
#include "common/log/LogContext.hpp"
#include <memory>
#include <list>

namespace cta {
namespace tape {
namespace daemon {
/**
 * A class managing several subprocesses, through their handlers. The subprocess
 * handler keeps track of all handlers and owns them.
 */
class ProcessManager {
public:
  ProcessManager(log::LogContext & log);
  ~ProcessManager();
  /** Function passing ownership of a subprocess handler to the manager. */
  void addHandler(std::unique_ptr<SubprocessHandler> && handler);
  /** Function allowing a SubprocessHandler to register a file descriptor to epoll */
  void addFile(int fd, SubprocessHandler * sh);
  /** Function allowing a SubprocessHandler to unregister a file descriptor from epoll */
  void removeFile(int fd);
  /** Infinite loop of the process. This function returns and exit value for 
   * the whole process. Implements the loop described in the header file for
   * SubprocessHandler class. */
  int run();
  /** Get reference to a given handler */
  SubprocessHandler & at(const std::string & name);
  /** Get reference to the log context */
  log::LogContext & logContext();
private:
  int m_epollFd;   ///< The file descriptor for the epoll interface
  log::LogContext & m_logContext; ///< The log context
  /// Structure allowing the follow up of subprocesses
  struct SubprocessAndStatus {
    SubprocessHandler::ProcessingStatus status;
    std::unique_ptr<SubprocessHandler> handler;
  };
  /// The list of process handlers we own and their statuses.
  std::list<SubprocessAndStatus> m_subprocessHandlers;
  /// Status report structs for run parts
  struct RunPartStatus {
    bool doExit=false;
    int exitCode=0;
  };
  /// sub part for run(): handle shutdown
  RunPartStatus runShutdownManagement();
  /// subpart for run(): handle kill
  RunPartStatus runKillManagement();
  /// subpart for run(): handle forking
  RunPartStatus runForkManagement();
  /// subpart for run(): handle SIGCHLD
  RunPartStatus runSigChildManagement();
  /// subpart for run(): handle events and timeouts. Does not return any value.
  void runEventLoop();
};

}}} // namespace cta::tape::daemon
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

#include "castor/exception/Errnum.hpp"
#include "castor/exception/Exception.hpp"
#include <unistd.h>


namespace castor {
namespace server {

  /**
   * A class allowing forking of a child process, and subsequent follow up
   * of the child process. Status check, killing, return code collection.
   */
  class ChildProcess {
  public:
    /**
     * Helper functor for child to clean up unneeded parent resources 
     * after forking.
     */
    class Cleanup {
    public:
      virtual void operator() () = 0;
      virtual ~Cleanup() {}
    };
    /**
     * Exceptions for wrong usage.
     */
    class ProcessStillRunning: public castor::exception::Exception {
    public:
      ProcessStillRunning(const std::string & what = "Process still running"):
      castor::exception::Exception::Exception(what) {}
    };
    
    class ProcessNeverStarted: public castor::exception::Exception {
    public:
      ProcessNeverStarted(const std::string & what = "Process never started"):
      castor::exception::Exception::Exception(what) {}
    };
    
    class ProcessWasKilled: public castor::exception::Exception {
    public:
      ProcessWasKilled(const std::string & what = "Process was killed"):
      castor::exception::Exception::Exception(what) {}
    };
    
    ChildProcess(): m_started(false), m_finished(false), m_exited(false),
      m_wasKilled(false) {}
    /* Clean up leftover child processes (hopefully not useful) */
    virtual ~ChildProcess() { if (m_started && !m_finished) kill(); };
    /** start function, taking as an argument a callback for parent's
     * resources cleanup. A child process can only be fired once. */
    void start(Cleanup & cleanup) ;
    /** Check running status */
    bool running() ;
    /** Wait for completion */
    void wait() ;
    /** collect exit code */
    int exitCode() ;
    /** kill */
    void kill() ;
  private:
    pid_t m_pid;
    /** Was the process started? */
    bool m_started;
    /** As the process finished? */
    bool m_finished;
    /** Did the process exit cleanly? */
    bool m_exited;
    /** Was the process killed? */
    bool m_wasKilled;
    int m_exitCode;
    /** The function actually being run in the child process. The value returned
     * by run() will be the exit code of the process (if we get that far) */
    virtual int run() = 0;
    void parseStatus(int status);
  };
}}

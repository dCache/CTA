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
 * Interface to the CASTOR logging system
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include "common/log/Logger.hpp"
#include "common/threading/Mutex.hpp"

#include <map>
#include <syslog.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

namespace cta {
namespace log {

/**
 * Class implementaing the API of the CASTOR logging system.
 */
class SyslogLogger: public Logger {
public:

  /**
   * Constructor
   *
   * @param socketName The socket to which the logging system should write.
   * @param programName The name of the program to be prepended to every log
   * message.
   * @param logMask The log mask.
   */
  SyslogLogger(const std::string &socketName, const std::string &programName,
    const int logMask);

  /**
   * Destructor.
   */
  ~SyslogLogger();

  /**
   * Prepares the logger object for a call to fork().
   *
   * No further calls to operator() should be made after calling this
   * method until the call to fork() has completed.
   */
  void prepareForFork() ;

  /**
   * Writes a message into the CASTOR logging system. Note that no exception
   * will ever be thrown in case of failure. Failures will actually be
   * silently ignored in order to not impact the processing.
   *
   * Note that this version of operator() implicitly uses the current time as
   * the time stamp of the message.
   *
   * @param priority the priority of the message as defined by the syslog API.
   * @param msg the message.
   * @param params the parameters of the message.
   */
  void operator() (
    const int priority,
    const std::string &msg,
    const std::list<Param> &params = std::list<Param>());

  /**
   * Writes a log message to the specified output stream.
   *
   * This public static (class) permitts other loggers such as the StringLogger to use the same log message format
   *
   * @param logMsg The output stream to which the log message is to be written.
   * @param priority the priority of the message as defined by the syslog API.
   * @param msg the message.
   * @param params the parameters of the message.
   * @param rawParams preprocessed parameters of the message.
   * @param timeStamp the time stamp of the log message.
   * @param programName the program name of the log message.
   * @param pid the pid of the log message.
   */
  static void writeLogMsg(
    std::ostringstream &os,
    const int priority,
    const std::string &priorityText,
    const std::string &msg,
    const std::list<Param> &params,
    const std::string &rawParams,
    const struct timeval &timeStamp,
    const std::string &programName,
    const int pid);

  /**
   * Default size of a syslog message.
   */
  static const size_t DEFAULT_SYSLOG_MSGLEN = 1024;

  /**
   * Default size of a rsyslog message.
   */
  static const size_t DEFAULT_RSYSLOG_MSGLEN = 2000;

  /**
   * Maximum length of a log message.
   */
  static const size_t LOG_MAX_LINELEN = 8192;

  /**
   * Determines the maximum message length that the client syslog server can
   * handle.
   *
   * @return The maximum message length that the client syslog server can
   * handle.
   */
  static size_t determineMaxMsgLen();

  /**
   * Generates and returns the mapping between syslog priorities and their
   * textual representations.
   */
  static std::map<int, std::string> generatePriorityToTextMap();

  /**
   * Generates and returns the mapping between the possible string values
   * of the LogMask parameters of /etc/castor.conf and their equivalent
   * syslog priorities.
   */
  static std::map<std::string, int> generateConfigTextToPriorityMap();

protected:

  /**
   * The socket to which the logging system should write.
   */
  std::string m_socketName;

  /**
   * The log mask.
   */
  int m_logMask;

  /**
   * The maximum message length that the client syslog server can handle.
   */
  const size_t m_maxMsgLen;

  /**
   * Mutex used to protect the critical section of the SyslogLogger
   * object.
   */
  threading::Mutex m_mutex;

  /**
   * The file descriptor of the socket used to send messages to syslog.
   */
  int m_logFile;

  /**
   * Map from syslog integer priority to textual representation.
   */
  const std::map<int, std::string> m_priorityToText;

  /**
   * Map from the possible string values of the LogMask parameters of
   * /etc/castor.conf and their equivalent syslog priorities.
   */
  const std::map<std::string, int> m_configTextToPriority;

  /**
   * Connects to syslog.
   *
   * Please note that this method must be called from within the critical
   * section of the SyslogLogger object.
   *
   * If the connection with syslog is already open then this method does
   * nothing.
   *
   * This method does not throw an exception if the connection cannot be made
   * to syslog.  In this case the internal state of the SyslogLogger
   * object reflects the fact that no connection was made.
   */
  void openLog();

  /**
   * Writes the header of a syslog message to teh specifed output stream.
   *
   * @param os The output stream to which the header will be written.
   * @param priority The priority of the message.
   * @param timeStamp The time stamp of the message.
   * @param programName the program name of the log message.
   * @param pid The process ID of the process logging the message.
   * @return The header of the syslog message.
   */
  static void writeHeader(
    std::ostringstream &os,
    const int priority,
    const struct timeval &timeStamp,
    const std::string &programName,
    const int pid);

  /**
   * Creates a clean version of the specified string ready for use with syslog.
   *
   * @param s The string to be cleaned.
   * @param replaceUnderscores Set to true if spaces should be replaced by
   * underscores.
   * @return A cleaned version of the string.
   */
  static std::string cleanString(const std::string &s,
    const bool replaceUnderscores);

  /**
   * A reduced version of syslog.  This method is able to set the message
   * timestamp.  This is necessary when logging messages asynchronously of there
   * creation, such as when retrieving logs from the DB.
   *
   * @param msg The message to be logged.
   */
  void reducedSyslog(std::string msg);

  /**
   * Closes the connection to syslog.
   *
   * Please note that this method must be called from within the critical
   * section of the SyslogLogger object.
   *
   * If the connection to syslog is already closed then this method does
   * nothing.
   */
  void closeLog();

}; // class SyslogLogger

} // namespace log
} // namespace cta


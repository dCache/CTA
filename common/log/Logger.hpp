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

#include "common/log/Constants.hpp"
#include "common/log/Param.hpp"

// The header file for atomic was is actually called cstdatomic in gcc 4.4
#if __GNUC__ == 4 && (__GNUC_MINOR__ == 4)
    #include <cstdatomic>
#else
  #include <atomic>
#endif

#include <list>
#include <map>

/**
 * It is a convention of CASTOR to use syslog level of LOG_NOTICE to label
 * user errors.  This macro helps enforce that convention and document it in
 * the code.
 */
#define LOG_USERERR LOG_NOTICE

namespace cta {
namespace log {

/**
 * Abstract class representing the API of the CASTOR logging system.
 *
 * The intended way to use the CASTOR logging API is as follows:
 *
 * 1. Keep a reference to a Logger object, for example:
 * \code{.cpp}
 *
 * class MyClassThatWillLog {
 * protected:
 *   Logger & m_log;
 *
 * public:
 *   MyClassThatWillLog(Logger &log): m_log(log) {
 *     ....
 *   }
 * }
 *
 * \endcode
 *
 * 2. To log a message, use the reference to the Logger object like a function.
 *    In other words the Logger object implements operator() and therefore
 *    behaves like a functor:
 * \code{.cpp}
 *
 * void MyClassThatWillLog::aMethodThatWillLog() {
 *   ....
 *   m_log(cta::log::INFO, "My log message");
 *   ....
 * }
 *
 * \endcode
 *
 * The Logger object implements operator() in order to avoid the following long
 * winded syntax (which does not work by the way, so please do NOT copy and
 * paste the following example):
 * \code{.cpp}
 *
 * m_log.logMsg(cta::log::INFO, "My log message");
 *
 * \endcode
 */
class Logger {
public:

  /**
   * Constructor
   *
   * @param programName The name of the program to be prepended to every log
   * @param logMask The log mask.
   * message.
   */
  Logger(const std::string &programName, const int logMask);

  /**
   * Destructor.
   */
  virtual ~Logger() = 0;

  /**
   * Prepares the logger object for a call to fork().
   *
   * No further calls to operator() should be made after calling this
   * method until the call to fork() has completed.
   */
  virtual void prepareForFork() = 0;

  /**
   * Returns the name of the program that is to  be prepended to every log
   * message.
   */
  const std::string &getProgramName() const;

  /**
   * Writes a message into the CTA logging system. Note that no exception
   * will ever be thrown in case of failure. Failures will actually be
   * silently ignored in order to not impact the processing.
   *
   * Note that this version of operator() implicitly uses the current time as
   * the time stamp of the message.
   *
   * @param priority the priority of the message as defined by the syslog API.
   * @param msg the message.
   * @param params optional parameters of the message.
   */
  virtual void operator() (
    const int priority,
    const std::string &msg,
    const std::list<Param> &params = std::list<Param>());

  /**
   * Sets the log mask.
   *
   * @param logMask The log mask.
   */
  void setLogMask(const std::string logMask);

  /**
   * Sets the log mask.
   *
   * @param logMask The log mask.
   */
  void setLogMask(const int logMask);
  
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

protected:

  /**
   * The name of the program to be prepended to every log message.
   */
  const std::string m_programName;
  
  /**
   * A reduced version of syslog.  This method is able to set the message
   * timestamp.  This is necessary when logging messages asynchronously of there
   * creation, such as when retrieving logs from the DB.
   *
   * @param msg The message to be logged.
   */
  virtual void reducedSyslog(const std::string & msg) = 0;

  /**
   * The log mask.
   */
  std::atomic<int> m_logMask;

  /**
   * The maximum message length that the client syslog server can handle.
   */
  const size_t m_maxMsgLen;

  /**
   * Map from syslog integer priority to textual representation.
   */
  const std::map<int, std::string> m_priorityToText;

  /**
   * Map from the possible string values of the LogMask parameters and
   * their equivalent syslog priorities.
   */
  const std::map<std::string, int> m_configTextToPriority;

  /**
   * Writes a log message to the specified output stream.
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
   * of the LogMask parameters their equivalent syslog priorities.
   */
  static std::map<std::string, int> generateConfigTextToPriorityMap();


}; // class Logger

} // namespace log
} // namespace cta


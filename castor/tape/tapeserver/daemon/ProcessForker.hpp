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

#include "castor/legacymsg/TapeLabelRqstMsgBody.hpp"
#include "castor/log/Logger.hpp"
#include "castor/messages/ForkCleaner.pb.h"
#include "castor/messages/ForkDataTransfer.pb.h"
#include "castor/messages/ForkLabel.pb.h"
#include "castor/tape/tapeserver/daemon/DataTransferSession.hpp"
#include "castor/tape/tapeserver/daemon/ProcessForkerFrame.hpp"
#include "castor/tape/tapeserver/daemon/TapeDaemonConfig.hpp"

#include <stdint.h>

namespace castor     {
namespace tape       {
namespace tapeserver {
namespace daemon     {

/**
 * Class responsible for forking processes.
 */
class ProcessForker {
public:

  /**
   * Constructor.
   *
   * This class takes ownership of the socket used to communicate with the
   * client.  The destructor of this class will close the file-descriptor.
   *
   * @param log Object representing the API of the CASTOR logging system.
   * @param cmdSocket The file-descriptor of the socket used to receive commands
   * from the ProcessForker proxy.
   * @param reaperSocket The file-descriptor of the socket used to notify the
   * TapeDaemon parent process of the termination of a process forked by the
   * ProcessForker.
   * @param hostName The name of the host on which the tapeserverd daemon is
   * running.
   * @param argv0 Pointer to argv[0], the command-line.
   * @param config The CASTOR configuration parameters used by the tapeserverd
   * daemon.
   */
  ProcessForker(log::Logger &log, const int cmdSocket, const int reaperSocket,
    const std::string &hostName, char *const argv0,
    const TapeDaemonConfig &config) throw();

  /**
   * Destructor.
   *
   * Closes the  file-descriptor of the socket used to communicate with the
   * client.
   */
  ~ProcessForker() throw();

  /**
   * Executes the main event loop of the ProcessForker.
   */
  void execute() throw();

private:

  /**
   * The maximum permitted size in bytes for the payload of a frame sent between
   * the ProcessForker and its proxy.
   */
  static const ssize_t s_maxPayloadLen = 1024;

  /**
   * Object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

  /**
   * The file-descriptor of the socket used to receive commands from the
   * ProcessForker proxy.
   */
  const int m_cmdSocket;

  /**
   * The file-descriptor of the socket used to notify the TapeDaemon parent
   * process of the termination of a process forked by the ProcessForker.
   */
  const int m_reaperSocket;

  /**
   * The name of the host on which the tapeserverd daemon is running.
   */ 
  const std::string m_hostName;

  /**
   * Pointer to argv[0], the command-line.
   */
  char *const m_argv0;

  /**
   * The CASTOR configuration parameters used by the tapeserverd daemon.
   */
  const TapeDaemonConfig m_config;

  /**
   * Idempotent method that closes the socket used for receving commands
   * from the ProcessForker proxy.
   */
  void closeCmdReceiverSocket() throw();

  /**
   * Structure defining the result of a message handler.
   */
  struct MsgHandlerResult {
    /**
     * True if the main event loop should continue.
     */
    bool continueMainEventLoop;

    /**
     * The reply frame.
     */
    ProcessForkerFrame reply;

    /**
     * Constructor.
     */
    MsgHandlerResult() throw():
      continueMainEventLoop(false) {
    }
  }; // struct MsgHandlerResult
  
  /**
   * Handles any pending events.
   *
   * @return true if the main event loop should continue.
   */
  bool handleEvents() throw ();

  /**
   * Handles a pending message if there is one.
   *
   * @return true if the main event loop should continue.
   */
  bool handlePendingMsgs();

  /**
   * Handles any pending signals.
   *
   * @return true if the main event loop should continue.
   */ 
  bool handlePendingSignals();

  /**
   * Return strue if there is a pending message from the ProcessForker proxy.
   */
  bool thereIsAPendingMsg();

  /**
   * Reads in and handles a single message from the ProcessForker proxy.
   */
  bool handleMsg();

  /**
   * Dispatches the appropriate handler method for the message contained within
   * the specified frame;
   *
   * @param frame The frame containing the message.
   * @return The result of dispatching the message handler.
   */
  MsgHandlerResult dispatchMsgHandler(const ProcessForkerFrame &frame);

  /**
   * Handles a StopProcessForker message.
   *
   * @param frame The frame containing the message.
   * @return The result of the message handler.
   */
  MsgHandlerResult handleStopProcessForkerMsg(const ProcessForkerFrame &frame);

  /**
   * Handles a ForkLabel message.
   *
   * @param frame The frame containing the message.
   * @return The result of the message handler.
   */
  MsgHandlerResult handleForkLabelMsg(const ProcessForkerFrame &frame);

  /**
   * Handles a ForkDataTransfer message.
   *
   * @param frame The frame containing the message.
   * @return The result of the message handler.
   */
  MsgHandlerResult handleForkDataTransferMsg(const ProcessForkerFrame &frame);

  /**
   * Handles a ForkCleaner message.
   *
   * @param frame The frame containing the message.
   * @return The result of the message handler.
   */
  MsgHandlerResult handleForkCleanerMsg(const ProcessForkerFrame &frame);

  /**
   * Runs a cleaner session.  This method is to be called within the child
   * process responsible for running the cleaner session.
   *
   * @param rqst The ForkCleaner message.
   * @return The value to be used when exiting the child process.
   */
  Session::EndOfSessionAction runCleanerSession(const messages::ForkCleaner &rqst);

  /**
   * Runs a data-transfer session.  This method is to be called within the
   * child process responsible for running the data-transfer session.
   *
   * @param rqst The ForkDataTransfer message.
   * @return The value to be used when exiting the child process.
   */
  Session::EndOfSessionAction runDataTransferSession(
    const messages::ForkDataTransfer &rqst);

  /**
   * Gets the drve configuration information from the specified ForkDataTransfer
   * message.
   *
   * @param msg The ForkDataTransfer message.
   * @return The drive configuration.
   */
  template<class T> DriveConfig getDriveConfig(const T &msg) {
    return DriveConfig(
      msg.unitname(),
      msg.dgn(),
      msg.devfilename(),
      msg.libraryslot());
  }

  /**
   * Gets the VDQM job from the specified ForkDataTransfer message.
   *
   * @param msg The ForkDataTransfer message.
   * @return The VDQM job.
   */
  castor::legacymsg::RtcpJobRqstMsgBody getVdqmJob(
    const messages::ForkDataTransfer &msg);

  /**
   * Instantiates a ZMQ context.
   *
   * @param sizeOfIOThreadPoolForZMQ The size of the IO thread pool to be used
   * by ZMQ.
   * @return The ZMQ context.
   */
  void *instantiateZmqContext(const int sizeOfIOThreadPoolForZMQ);

  /**
   * Reaps any zombie processes.
   */
  void reapZombies();

  /**
   * Handles the specified reaped zombie.
   *
   * @param pid The process ID of the reaped zombie.
   * @param waitpidStat The status information given by a call to waitpid().
   */
  void handleReapedZombie(const pid_t pid, const int waitpidStat);

  /**
   * Logs the fact that the specified child process has terminated.
   *
   * @param pid The process ID of the child process.
   * @param waitpidStat The status information given by a call to waitpid().
   */
  void logChildProcessTerminated(const pid_t pid, const int waitpidStat)
    throw();

  /**
   * Notifies the TapeDaemon parent process that a child process of the
   * ProcessForker has terminated.
   *
   * @param pid The process ID of the child process.
   * @param waitpidStat The status information given by a call to waitpid().
   */
  void notifyTapeDaemonOfTerminatedProcess(const pid_t pid,
    const int waitpidStat);

  /**
   * Notifies the TapeDaemon parent process that a child process of the
   * ProcessForker exited.
   *
   * @param pid The process ID of the child process.
   * @param waitpidStat The status information given by a call to waitpid().
   */
  void notifyTapeDaemonOfExitedProcess(const pid_t pid,
    const int waitpidStat);

  /**
   * Notifies the TapeDaemon parent process that a child process of the
   * ProcessForker crashed.
   *
   * @param pid The process ID of the child process.
   * @param waitpidStat The status information given by a call to waitpid().
   */
  void notifyTapeDaemonOfCrashedProcess(const pid_t pid,
    const int waitpidStat);

  /**
   * Runs a label session.  This method is to be called within the child
   * process responsible for running the label session.
   *
   * @param rqst The ForkLabel message.
   * @return The value to be used when exiting the child process.
   */
  Session::EndOfSessionAction runLabelSession(const messages::ForkLabel &rqst);

  /**
   * Gets the label job from the specified ForkLabel message.
   *
   * @param msg The ForkLabel message.
   * @return The label job.
   */
  castor::legacymsg::TapeLabelRqstMsgBody getLabelJob(
    const messages::ForkLabel &msg);

  /**
   * Creates a MsgHandlerResult containing a ForkSucceeded message.
   *
   * @param pid The process identifier of the forked process.
   * @param continueMainEventLoop Set to true if the main event loop should
   * continue.
   * @return The MsgHandlerResult containing a ForkSucceeded message.
   */
  MsgHandlerResult createForkSucceededResult(const pid_t pid,
    const bool continueMainEventLoop);

  /**
   * Creates a MsgHandlerResult containing an Exception message.
   *
   * @param code The error code of the exception.
   * @param message The error message of the exception.
   * @param continueMainEventLoop Set to true if the main event loop should
   * continue.
   * @return The MsgHandlerResult containing an Exception message.
   */
  MsgHandlerResult createExceptionResult(const uint32_t code,
    const std::string& message, const bool continueMainEventLoop);

  /**
   * Creates a MsgHandlerResult containing a ReturnValue message.
   *
   * @param value The return value.
   * @param continueMainEventLoop Set to true if the main event loop should
   * continue.
   * @return The MsgHandlerResult containing a ReturnValue message.
   */
  MsgHandlerResult createReturnValueResult(const uint32_t value, 
    const bool continueMainEventLoop);

}; // class ProcessForker

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor

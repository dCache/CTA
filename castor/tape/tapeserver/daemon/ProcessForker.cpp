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

#include "castor/exception/Exception.hpp"
#include "castor/legacymsg/NsProxy_TapeAlwaysEmpty.hpp"
#include "castor/legacymsg/RmcProxyTcpIp.hpp"
#include "castor/messages/Constants.hpp"
#include "castor/messages/ForkCleaner.pb.h"
#include "castor/messages/ForkDataTransfer.pb.h"
#include "castor/messages/ForkLabel.pb.h"
#include "castor/messages/ForkSucceeded.pb.h"
#include "castor/messages/ProcessCrashed.pb.h"
#include "castor/messages/ProcessExited.pb.h"
#include "castor/messages/ReturnValue.pb.h"
#include "castor/messages/StopProcessForker.pb.h"
#include "castor/messages/TapeserverProxyZmq.hpp"
#include "castor/tape/tapeserver/daemon/Constants.hpp"
#include "castor/tape/tapeserver/daemon/CleanerSession.hpp"
#include "castor/tape/tapeserver/daemon/DataTransferSession.hpp"
#include "castor/tape/tapeserver/daemon/LabelSession.hpp"
#include "castor/tape/tapeserver/daemon/ProcessForker.hpp"
#include "castor/tape/tapeserver/daemon/ProcessForkerUtils.hpp"
#include "castor/tape/utils/DriveConfig.hpp"
#include "castor/messages/SmartZmqContext.hpp"
#include "castor/utils/SmartArrayPtr.hpp"
#include "castor/utils/utils.hpp"
#include "h/serrno.h"

#include <errno.h>
#include <memory>
#include <poll.h>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::ProcessForker(
  log::Logger &log,
  const int cmdSocket,
  const int reaperSocket,
  const std::string &hostName,
  char *const argv0) throw():
  m_log(log),
  m_cmdSocket(cmdSocket),
  m_reaperSocket(reaperSocket),
  m_hostName(hostName),
  m_argv0(argv0) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::~ProcessForker() throw() {
  closeCmdReceiverSocket();
}

//------------------------------------------------------------------------------
// closeCmdReceiverSocket
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForker::closeCmdReceiverSocket()
  throw() {
  if(-1 != m_cmdSocket) {
    std::list<log::Param> params;
    params.push_back(log::Param("cmdSocket", m_cmdSocket));
    if(-1 == close(m_cmdSocket)) {
      char message[100];
      sstrerror_r(errno, message, sizeof(message));
      params.push_back(log::Param("message", message));
      m_log(LOG_ERR, "Failed to close command receiver socket",
        params);
    } else {
      m_log(LOG_INFO, "Closed command receiver socket", params);
    }
  }
}

//------------------------------------------------------------------------------
// execute
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForker::execute() throw() {
  // The main event loop
  while(handleEvents()) {
  }
}

//------------------------------------------------------------------------------
// handleEvents
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::ProcessForker::handleEvents() throw() {
  try {
    return handlePendingMsgs() && handlePendingSignals();
  } catch(castor::exception::Exception &ex) {
    log::Param params[] = {log::Param("message", ex.getMessage().str())};
    m_log(LOG_ERR, "ProcessForker failed to handle events", params);
  } catch(std::exception &se) {
    log::Param params[] = {log::Param("message", se.what())};
    m_log(LOG_ERR, "ProcessForker failed to handle events", params);
  } catch(...) {
    log::Param params[] =
      {log::Param("message", "Caught an unknown exception")};
    m_log(LOG_ERR, "ProcessForker failed to handle events", params);
  }

  // If program execution reached here then an exception was thrown
  m_log(LOG_ERR, "ProcessForker is gracefully shutting down");
  return false;
}

//------------------------------------------------------------------------------
// handlePendingMsgs
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::ProcessForker::handlePendingMsgs() {
  if(thereIsAPendingMsg()) {
    return handleMsg();
  } else {
    return true; // The main event loop should continue
  }
}

//------------------------------------------------------------------------------
// thereIsAPendingMsg
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::ProcessForker::thereIsAPendingMsg() {

  // Call poll() in orer to see if there is any data to be read
  struct pollfd fds;
  fds.fd = m_cmdSocket;
  fds.events = POLLIN;
  fds.revents = 0;
  const int timeout = 100; // Timeout in milliseconds
  const int pollRc = poll(&fds, 1, timeout);

  // Return true of false depending on the result from poll()
  switch(pollRc) {
  case 0: // Timeout
    return false;
  case -1: // Error
    {
      char message[100];
      sstrerror_r(errno, message, sizeof(message));
      log::Param params[] = {log::Param("message", message)};
      m_log(LOG_ERR,
        "Error detected when checking for a pending ProcessForker message",
        params);
      return false;
    }
  case 1: // There is a possibility of a pending message
    return fds.revents & POLLIN ? true : false;
  default: // Unexpected return value
    {
      std::ostringstream message;
      message << "poll returned an unexpected value"
        ": expected=0 or 1 actual=" << pollRc;
      log::Param params[] = {log::Param("message", message.str())};
      m_log(LOG_ERR,
        "Error detected when checking for a pending ProcessForker message",
        params);
      return false;
    }
  }
}

//------------------------------------------------------------------------------
// handleMsg
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::ProcessForker::handleMsg() {
  ProcessForkerFrame frame;
  try {
    const int timeout = 10; // Timeout in seconds
    frame = ProcessForkerUtils::readFrame(m_cmdSocket, timeout);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to handle message: " << ne.getMessage().str();
    throw ex;
  }

  log::Param params[] = {
    log::Param("type", messages::msgTypeToString(frame.type)),
    log::Param("len", frame.payload.length())};
  m_log(LOG_INFO, "ProcessForker handling a ProcessForker message", params);

  MsgHandlerResult result;
  try {
    result = dispatchMsgHandler(frame);
  } catch(castor::exception::Exception &ex) {
    log::Param("message", ex.getMessage().str());
    m_log(LOG_ERR, "ProcessForker::dispatchMsgHandler() threw an exception",
      params);
    messages::Exception msg;
    msg.set_code(ex.code());
    msg.set_message(ex.getMessage().str());
    ProcessForkerUtils::writeFrame(m_cmdSocket, msg);
    return true; // The main event loop should continue
  } catch(std::exception &se) {
    log::Param("message", se.what());
    m_log(LOG_ERR, "ProcessForker::dispatchMsgHandler() threw an exception",
      params);
    messages::Exception msg;
    msg.set_code(SEINTERNAL);
    msg.set_message(se.what());
    ProcessForkerUtils::writeFrame(m_cmdSocket, msg);
    return true; // The main event loop should continue
  } catch(...) {
    m_log(LOG_ERR,
      "ProcessForker::dispatchMsgHandler() threw an unknown exception");
    messages::Exception msg;
    msg.set_code(SEINTERNAL);
    msg.set_message("Caught and unknown and unexpected exception");
    ProcessForkerUtils::writeFrame(m_cmdSocket, msg);
    return true; // The main event loop should continue
  }

  ProcessForkerUtils::writeFrame(m_cmdSocket, result.reply);
  {
    log::Param params[] = {
      log::Param("payloadType",
        messages::msgTypeToString(result.reply.type)),
      log::Param("payloadLen", result.reply.payload.length())};
    m_log(LOG_DEBUG, "ProcessForker wrote reply", params);
  }
  return result.continueMainEventLoop;
}

//------------------------------------------------------------------------------
// dispatchMsgHandler
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::MsgHandlerResult
  castor::tape::tapeserver::daemon::ProcessForker::dispatchMsgHandler(
  const ProcessForkerFrame &frame) {
  switch(frame.type) {
  case messages::MSG_TYPE_FORKCLEANER:
    return handleForkCleanerMsg(frame);
  case messages::MSG_TYPE_FORKDATATRANSFER:
    return handleForkDataTransferMsg(frame);
  case messages::MSG_TYPE_FORKLABEL:
    return handleForkLabelMsg(frame);
  case messages::MSG_TYPE_STOPPROCESSFORKER:
    return handleStopProcessForkerMsg(frame);
  default:
    {
      castor::exception::Exception ex;
      ex.getMessage() << "Failed to dispatch message handler"
        ": Unknown message type: type=" << frame.type;
      throw ex;
    }
  }
}

//------------------------------------------------------------------------------
// handleForkCleanerMsg
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::MsgHandlerResult 
  castor::tape::tapeserver::daemon::ProcessForker::handleForkCleanerMsg(
  const ProcessForkerFrame &frame) {

  // Parse the incoming request
  messages::ForkCleaner rqst;
  ProcessForkerUtils::parsePayload(frame, rqst);

  // Log the contents of the incomming request
  std::list<log::Param> params;
  params.push_back(log::Param("unitName", rqst.unitname()));
  params.push_back(log::Param("vid", rqst.vid()));
  params.push_back(log::Param("rmcPort", rqst.rmcport()));
  m_log(LOG_INFO, "ProcessForker handling ForkCleaner message", params);

  // Fork a label session
  const pid_t forkRc = fork();

  // If fork failed
  if(0 > forkRc) {
    return createExceptionResult(SEINTERNAL,
      "Failed to fork cleaner session for tape drive", true);

  // Else if this is the parent process
  } else if(0 < forkRc) {
    log::Param params[] = {log::Param("pid", forkRc)};
    m_log(LOG_INFO, "ProcessForker forked cleaner session", params);

    return createForkSucceededResult(forkRc, true);

  // Else this is the child process
  } else {
    closeCmdReceiverSocket();

    castor::utils::setProcessNameAndCmdLine(m_argv0, "tpcleaner");

    try {
      exit(runCleanerSession(rqst));
    } catch(castor::exception::Exception &ne) {
      log::Param params[] = {log::Param("message", ne.getMessage().str())};
      m_log(LOG_ERR, "Failed to run cleaner session", params);
    } catch(std::exception &ne) {
      log::Param params[] = {log::Param("message", ne.what())};
      m_log(LOG_ERR, "Failed to run cleaner session", params);
    } catch(...) {
      log::Param params[] = {log::Param("message",
        "Caught an unknown exception")};
      m_log(LOG_ERR, "Failed to run cleaner session", params);
    }
    exit(1);
  }
}

//------------------------------------------------------------------------------
// handleForkDataTransferMsg
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::MsgHandlerResult 
  castor::tape::tapeserver::daemon::ProcessForker::handleForkDataTransferMsg(
  const ProcessForkerFrame &frame) {

  // Parse the incoming request
  messages::ForkDataTransfer rqst;
  ProcessForkerUtils::parsePayload(frame, rqst);

  // Log the contents of the incomming request
  std::list<log::Param> params;
  params.push_back(log::Param("unitName", rqst.unitname()));
  params.push_back(log::Param("rmcPort", rqst.rmcport()));
  m_log(LOG_INFO, "ProcessForker handling ForkDataTransfer message", params);

  // Fork a data-transfer session
  const pid_t forkRc = fork();

  // If fork failed
  if(0 > forkRc) {
    return createExceptionResult(SEINTERNAL,
      "Failed to fork data-transfer session for tape drive", true);
  // Else if this is the parent process
  } else if(0 < forkRc) {
    log::Param params[] = {log::Param("pid", forkRc)};
    m_log(LOG_INFO, "ProcessForker forked data-transfer session", params);

    return createForkSucceededResult(forkRc, true);

  // Else this is the child process
  } else {
    closeCmdReceiverSocket();

    castor::utils::setProcessNameAndCmdLine(m_argv0, "tptransfer");

    try {
      exit(runDataTransferSession(rqst));
    } catch(castor::exception::Exception &ne) {
      log::Param params[] = {log::Param("message", ne.getMessage().str())};
      m_log(LOG_ERR, "Failed to run data-transfer session", params);
    } catch(std::exception &ne) {
      log::Param params[] = {log::Param("message", ne.what())};
      m_log(LOG_ERR, "Failed to run data-transfer session", params);
    } catch(...) {
      log::Param params[] = {log::Param("message",
        "Caught an unknown exception")};
      m_log(LOG_ERR, "Failed to run data-transfer session", params);
    }
    exit(1);
  }
}

//------------------------------------------------------------------------------
// handleForkLabelMsg
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::MsgHandlerResult 
  castor::tape::tapeserver::daemon::ProcessForker::handleForkLabelMsg(
  const ProcessForkerFrame &frame) {
  // Parse the incoming request
  messages::ForkLabel rqst;
  ProcessForkerUtils::parsePayload(frame, rqst);

  // Log the contents of the incomming request
  std::list<log::Param> params;
  params.push_back(log::Param("unitName", rqst.unitname()));
  params.push_back(log::Param("vid", rqst.vid()));
  params.push_back(log::Param("rmcPort", rqst.rmcport()));
  m_log(LOG_INFO, "ProcessForker handling ForkLabel message", params);

  // Fork a label session
  const pid_t forkRc = fork();

  // If fork failed
  if(0 > forkRc) {
    return createExceptionResult(SEINTERNAL,
      "Failed to fork label session for tape drive", true);

  // Else if this is the parent process
  } else if(0 < forkRc) {
    log::Param params[] = {log::Param("pid", forkRc)};
    m_log(LOG_INFO, "ProcessForker forked label session", params);

    return createForkSucceededResult(forkRc, true);

  // Else this is the child process
  } else {
    closeCmdReceiverSocket();

    castor::utils::setProcessNameAndCmdLine(m_argv0, "tplabel");

    try {
      exit(runLabelSession(rqst));
    } catch(castor::exception::Exception &ne) {
      log::Param params[] = {log::Param("message", ne.getMessage().str())};
      m_log(LOG_ERR, "Failed to run label session", params);
    } catch(std::exception &ne) {
      log::Param params[] = {log::Param("message", ne.what())};
      m_log(LOG_ERR, "Failed to run label session", params);
    } catch(...) {
      log::Param params[] = {log::Param("message",
        "Caught an unknown exception")};
      m_log(LOG_ERR, "Failed to run label session", params);
    }
    exit(1);
  }
}

//------------------------------------------------------------------------------
// handleStopProcessForkerMsg
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::MsgHandlerResult 
  castor::tape::tapeserver::daemon::ProcessForker::
  handleStopProcessForkerMsg(const ProcessForkerFrame &frame) {

  // Parse the incoming request
  messages::StopProcessForker rqst;
  ProcessForkerUtils::parsePayload(frame, rqst);

  // Log the fact that the ProcessForker will not gracefully stop
  log::Param params[] = {log::Param("reason", rqst.reason())};
  m_log(LOG_INFO, "Gracefully stopping ProcessForker", params);

  return createReturnValueResult(0, false);
}

//------------------------------------------------------------------------------
// runCleanerSession
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::ProcessForker::runCleanerSession(
  const messages::ForkCleaner &rqst) {
  try {
    const utils::DriveConfig driveConfig = getDriveConfig(rqst);
    std::list<log::Param> params;
    params.push_back(log::Param("unitName", driveConfig.unitName));
    params.push_back(log::Param("vid", rqst.vid()));
    params.push_back(log::Param("rmcPort", rqst.rmcport()));
    m_log(LOG_INFO, "Cleaner-session child-process started", params);

    // The network timeout of rmc communications should be several minutes due
    // to the time it takes to mount and unmount tapes
    const int rmcNetTimeout = 600; // Timeout in seconds
    legacymsg::RmcProxyTcpIp rmc(m_log, rqst.rmcport(), rmcNetTimeout);
    castor::tape::System::realWrapper sWrapper;
    CleanerSession cleanerSession(
      rmc,
      m_log,
      driveConfig,
      sWrapper,
      rqst.vid());
    // execute() returns 0 if drive should be put up or 1 if it should be put down
    return cleanerSession.execute();
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to run cleaner session: " << ne.getMessage().str();
    throw ex;
  } catch(std::exception &se) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to run cleaner session: " << se.what();
    throw ex;
  } catch(...) {
        castor::exception::Exception ex;
    ex.getMessage() << "Failed to run cleaner session"
      ": Caught an unknown exception";
    throw ex;
  }
}

//------------------------------------------------------------------------------
// runDataTransferSession
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::ProcessForker::runDataTransferSession(
  const messages::ForkDataTransfer &rqst) {
  const utils::DriveConfig driveConfig = getDriveConfig(rqst);

  std::list<log::Param> params;
  params.push_back(log::Param("unitName", driveConfig.unitName));
  m_log(LOG_INFO, "Data-transfer child-process started", params);

  const legacymsg::RtcpJobRqstMsgBody vdqmJob = getVdqmJob(rqst);

  castor::server::ProcessCap capUtils;
  const DataTransferSession::CastorConf dataTransferConfig =
    getDataTransferConfig(rqst);

  // The network timeout of rmc communications should be several minutes due
  // to the time it takes to mount and unmount tapes
  const int rmcNetTimeout = 600; // Timeout in seconds
  legacymsg::RmcProxyTcpIp rmc(m_log, rqst.rmcport(), rmcNetTimeout);

  const int sizeOfIOThreadPoolForZMQ = 1;
  messages::SmartZmqContext
    zmqContext(instantiateZmqContext(sizeOfIOThreadPoolForZMQ));

  const int tapeserverNetTimeout = 10; // Timeout in seconds
  messages::TapeserverProxyZmq tapeserver(m_log,
    TAPE_SERVER_INTERNAL_LISTENING_PORT, tapeserverNetTimeout,
    zmqContext.get());

  castor::tape::System::realWrapper sysWrapper;

  // This try bloc will allow us to send a failure notification to the client
  // if we fail before the DataTransferSession has an opportunity to do so.
  std::auto_ptr<DataTransferSession> dataTransferSession;
  try {
    dataTransferSession.reset(new DataTransferSession (
      m_hostName,
      vdqmJob,
      m_log,
      sysWrapper,
      driveConfig,
      rmc,
      tapeserver,
      capUtils,
      dataTransferConfig));
  } catch (castor::exception::Exception & ex) {
    try {
      client::ClientProxy cl(vdqmJob);
      client::ClientInterface::RequestReport rep;
      cl.reportEndOfSessionWithError(ex.getMessageValue(), ex.code(), rep);
    } catch (...) {
      params.push_back(log::Param("errorMessage", ex.getMessageValue()));
      params.push_back(log::Param("errorCode", ex.code()));
      m_log(LOG_ERR, "Failed to notify the client of the failed session"
        " when setting up the data-transfer session", params);
    }
    throw;
  } catch (...) {
    try {
      m_log(LOG_ERR, "Got non castor exception error while constructing"
        " data-transfer session", params);
      client::ClientProxy cl(vdqmJob);
      client::ClientInterface::RequestReport rep;
      cl.reportEndOfSessionWithError(
       "Non-Castor exception when setting up the data-transfer session",
         SEINTERNAL, rep);
    } catch (...) {
      params.push_back(log::Param("errorMessage",
        "Non-Castor exception when setting up the data-transfer session"));
      m_log(LOG_ERR, "Failed to notify the client of the failed session"
        " when setting up the data-transfer session", params);
    }
    throw;
  }
  m_log(LOG_INFO, "Going to execute data-transfer session");
  return dataTransferSession->execute();
}

//------------------------------------------------------------------------------
// getDataTransferConfig
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::DataTransferSession::CastorConf
  castor::tape::tapeserver::daemon::ProcessForker::getDataTransferConfig(
  const messages::ForkDataTransfer &msg) {
  DataTransferSession::CastorConf config;
  config.rtcopydBufsz = msg.memblocksize();
  config.rtcopydNbBufs = msg.nbmemblocks();
  config.tapeBadMIRHandlingRepair = msg.badmirhandling();
  config.tapebridgeBulkRequestMigrationMaxBytes =
    msg.bulkrequestmigrationmaxbytes();
  config.tapebridgeBulkRequestMigrationMaxFiles =
    msg.bulkrequestmigrationmaxfiles();
  config.tapebridgeBulkRequestRecallMaxBytes = msg.bulkrequestrecallmaxbytes();
  config.tapebridgeBulkRequestRecallMaxFiles = msg.bulkrequestrecallmaxfiles();
  config.tapebridgeMaxBytesBeforeFlush = msg.maxbytesbeforeflush();
  config.tapebridgeMaxFilesBeforeFlush = msg.maxfilesbeforeflush();
  config.tapeserverdDiskThreads = msg.diskthreadpoolsize();

  return config;
}

//------------------------------------------------------------------------------
// getVdqmJob
//------------------------------------------------------------------------------
castor::legacymsg::RtcpJobRqstMsgBody
  castor::tape::tapeserver::daemon::ProcessForker::getVdqmJob(
  const messages::ForkDataTransfer &msg) {
  castor::legacymsg::RtcpJobRqstMsgBody job;
  job.volReqId = msg.mounttransactionid();
  job.clientPort = msg.clientport();
  job.clientEuid = msg.clienteuid();
  job.clientEgid = msg.clientegid();
  castor::utils::copyString(job.clientHost, msg.clienthost());
  castor::utils::copyString(job.dgn, msg.dgn());
  castor::utils::copyString(job.driveUnit, msg.unitname());
  castor::utils::copyString(job.clientUserName, msg.clientusername());

  return job;
}

//------------------------------------------------------------------------------
// instantiateZmqContext
//------------------------------------------------------------------------------
void *castor::tape::tapeserver::daemon::ProcessForker::instantiateZmqContext(
  const int sizeOfIOThreadPoolForZMQ) {
  void *const zmqContext = zmq_init(sizeOfIOThreadPoolForZMQ);
  if(NULL == zmqContext) {
    char message[100];
    sstrerror_r(errno, message, sizeof(message));
    castor::exception::Exception ex;
    ex.getMessage() << "Child of ProcessForker failed to instantiate ZMQ"
      " context: " << message;
    throw ex;
  }
  std::ostringstream contextAddr;
  contextAddr << std::hex << zmqContext;
  log::Param params[] = {log::Param("contextAddr", contextAddr.str())};
  m_log(LOG_INFO, "Child of ProcessForker instantiated a ZMQ context", params);

  return zmqContext;
}

//------------------------------------------------------------------------------
// handlePendingSignals
//------------------------------------------------------------------------------
bool castor::tape::tapeserver::daemon::ProcessForker::handlePendingSignals() {
  try {
    // Handle a pending SIGCHLD by reaping the associated zombie(s)
    reapZombies();

    // For now there are no signals that require a gracefully shutdown of the
    // main loop of the ProcessForker
    return true; // The main event loop should continue
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to handle pending signals: " <<
      ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// reapZombies
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForker::reapZombies() {
  pid_t pid = 0;
  int waitpidStat = 0;
  while (0 < (pid = waitpid(-1, &waitpidStat, WNOHANG))) {
    handleReapedZombie(pid, waitpidStat);
  }
}

//------------------------------------------------------------------------------
// handleReapedZombie
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForker::handleReapedZombie(
  const pid_t pid, const int waitpidStat) {
  try {
    logChildProcessTerminated(pid, waitpidStat);
    notifyTapeDaemonOfTerminatedProcess(pid, waitpidStat);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to handle reaped zombie: pid=" << pid <<
      ne.getMessage().str();
    throw ex;
  } catch(std::exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to handle reaped zombie: pid=" << pid <<
      ne.what();
    throw ex;
  } catch(...) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to handle reaped zombie: pid=" << pid <<
      ": Caught an unknown exception";
    throw ex;
  }
} 

//------------------------------------------------------------------------------
// logChildProcessTerminated
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForker::logChildProcessTerminated(
  const pid_t pid, const int waitpidStat) throw() {
  std::list<log::Param> params;
  params.push_back(log::Param("terminatedPid", pid));

  if(WIFEXITED(waitpidStat)) {
    params.push_back(log::Param("WEXITSTATUS", WEXITSTATUS(waitpidStat)));
  }

  if(WIFSIGNALED(waitpidStat)) {
    params.push_back(log::Param("WTERMSIG", WTERMSIG(waitpidStat)));
  }

  if(WCOREDUMP(waitpidStat)) {
    params.push_back(log::Param("WCOREDUMP", "true"));
  } else {
    params.push_back(log::Param("WCOREDUMP", "false"));
  }

  if(WIFSTOPPED(waitpidStat)) {
    params.push_back(log::Param("WSTOPSIG", WSTOPSIG(waitpidStat)));
  }

  if(WIFCONTINUED(waitpidStat)) {
    params.push_back(log::Param("WIFCONTINUED", "true"));
  } else {
    params.push_back(log::Param("WIFCONTINUED", "false"));
  }

  m_log(LOG_INFO, "ProcessForker child process terminated", params);
}

//------------------------------------------------------------------------------
// notifyTapeDaemonOfTerminatedProcess
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForker::
  notifyTapeDaemonOfTerminatedProcess(const pid_t pid, const int waitpidStat) {
  try {
    if(WIFEXITED(waitpidStat)) {
      notifyTapeDaemonOfExitedProcess(pid, waitpidStat);
    } else if(WIFSIGNALED(waitpidStat)) {
      notifyTapeDaemonOfCrashedProcess(pid, waitpidStat);
    } else {
      castor::exception::Exception ex;
      ex.getMessage() << "Process died of unknown causes" << pid;
      throw ex;
    }
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to notify TapeDaemon of process termination"
      ": pid=" << pid << ": " << ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// notifyTapeDaemonOfExitedProcess
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForker::
  notifyTapeDaemonOfExitedProcess(const pid_t pid, const int waitpidStat) {
  try {
    messages::ProcessExited msg;
    msg.set_pid(pid);
    msg.set_exitcode(WEXITSTATUS(waitpidStat));

    log::Param params[] = {
      log::Param("pid", msg.pid()),
      log::Param("exitCode", msg.exitcode()),
      log::Param("payloadLen", msg.ByteSize())};
    m_log(LOG_INFO, "ProcessForker notifying TapeDaemon of process exit",
      params);

    ProcessForkerUtils::writeFrame(m_reaperSocket, msg, &m_log);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to notify TapeDaemon of process exit: " <<
      ne.getMessage().str();
    throw ex;
  } catch(std::exception &se) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to notify TapeDaemon of process exit: " <<
      se.what();
    throw ex;
  } catch(...) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to notify TapeDaemon of process exit: "
      "Caught an unknown exception";
    throw ex;
  }
}

//------------------------------------------------------------------------------
// notifyTapeDaemonOfCrashedProcess
//------------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::ProcessForker::
  notifyTapeDaemonOfCrashedProcess(const pid_t pid, const int waitpidStat) {
  try {
    messages::ProcessCrashed msg;
    msg.set_pid(pid);
    msg.set_signal(WTERMSIG(waitpidStat));

    log::Param params[] = {log::Param("pid", msg.pid()),
      log::Param("signal", msg.signal())};
    m_log(LOG_INFO, "ProcessForker notifying TapeDaemon of process crash",
      params);

    ProcessForkerUtils::writeFrame(m_reaperSocket, msg, &m_log);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to notify TapeDaemon of process crash: " <<
      ne.getMessage().str();
    throw ex;
  } catch(std::exception &se) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to notify TapeDaemon of process crash: " <<
      se.what();
    throw ex;
  } catch(...) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to notify TapeDaemon of process crash: "
      "Caught an unknown exception";
    throw ex;
  }
}

//------------------------------------------------------------------------------
// runLabelSession
//------------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::ProcessForker::runLabelSession(
  const messages::ForkLabel &rqst) {
  try {
    const utils::DriveConfig &driveConfig = getDriveConfig(rqst);
    const legacymsg::TapeLabelRqstMsgBody labelJob = getLabelJob(rqst);

    std::list<log::Param> params;
    params.push_back(log::Param("unitName", driveConfig.unitName));
    params.push_back(log::Param("vid", labelJob.vid));
    params.push_back(log::Param("rmcPort", rqst.rmcport()));
    m_log(LOG_INFO, "Label-session child-process started", params);

    // The network timeout of rmc communications should be several minutes due
    // to the time it takes to mount and unmount tapes
    const int rmcNetTimeout = 600; // Timeout in seconds
    legacymsg::RmcProxyTcpIp rmc(m_log, rqst.rmcport(), rmcNetTimeout);
    legacymsg::NsProxy_TapeAlwaysEmpty ns;
    castor::tape::System::realWrapper sWrapper;
    LabelSession labelsession(
      rmc,
      labelJob,
      m_log,
      sWrapper,
      driveConfig,
      rqst.force());
    labelsession.execute();
    return 0;
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to run label session: " << ne.getMessage().str();
    throw ex;
  } catch(std::exception &se) {
    castor::exception::Exception ex;
    ex.getMessage() << "Failed to run label session: " << se.what();
    throw ex;
  } catch(...) {
        castor::exception::Exception ex;
    ex.getMessage() << "Failed to run label session"
      ": Caught an unknown exception";
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getLabelJob
//------------------------------------------------------------------------------
castor::legacymsg::TapeLabelRqstMsgBody
  castor::tape::tapeserver::daemon::ProcessForker::getLabelJob(
  const messages::ForkLabel &msg) {
  castor::legacymsg::TapeLabelRqstMsgBody job;
  job.force = msg.force() ? 1 : 0;
  job.uid = msg.uid();
  job.gid = msg.gid();
  castor::utils::copyString(job.vid,msg.vid());
  castor::utils::copyString(job.drive, msg.unitname());
  castor::utils::copyString(job.dgn, msg.dgn());
  return job;
}

//------------------------------------------------------------------------------
// createForkSucceededResult
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::MsgHandlerResult 
  castor::tape::tapeserver::daemon::ProcessForker::createForkSucceededResult(
  const pid_t pid, const bool continueMainEventLoop) {
  try {
    messages::ForkSucceeded reply;
    reply.set_pid(pid);

    MsgHandlerResult result;
    result.continueMainEventLoop = continueMainEventLoop;
    ProcessForkerUtils::serializePayload(result.reply, reply);
    
    return result;
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() <<
      "Failed to create MsgHandlerResult containing a ForkSucceeded message:"
      << ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createExceptionResult
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::MsgHandlerResult 
  castor::tape::tapeserver::daemon::ProcessForker::
  createExceptionResult(const uint32_t code, const std::string& message,
    const bool continueMainEventLoop) {
  try {
    messages::Exception reply;
    reply.set_code(code);
    reply.set_message(message);

    MsgHandlerResult result;
    result.continueMainEventLoop = continueMainEventLoop;
    ProcessForkerUtils::serializePayload(result.reply, reply);

    return result;
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() <<
      "Failed to create MsgHandlerResult containing an Exception message:"
      << ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createReturnValueResult
//------------------------------------------------------------------------------
castor::tape::tapeserver::daemon::ProcessForker::MsgHandlerResult
  castor::tape::tapeserver::daemon::ProcessForker::
  createReturnValueResult(const uint32_t value,
    const bool continueMainEventLoop) {
  try {
    messages::ReturnValue reply;
    reply.set_value(value);

    MsgHandlerResult result;
    result.continueMainEventLoop = continueMainEventLoop;
    ProcessForkerUtils::serializePayload(result.reply, reply);

    return result;
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() <<
      "Failed to create MsgHandlerResult containing ReturnValue message:"
      << ne.getMessage().str();
    throw ex;
  }
}

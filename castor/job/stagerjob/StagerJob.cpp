/******************************************************************************
 *                      stagerJob.cpp
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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * this is the job launched by LSF on the diskserver in order to allow some I/O
 *
 * @author castor dev team
 *****************************************************************************/

// Include Files
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <attr/xattr.h>
#include <sstream>
#include <fcntl.h>
#include "common.h"
#include "getconfent.h"
#include "castor/System.hpp"
#include "castor/Services.hpp"
#include "castor/BaseObject.hpp"
#include "castor/dlf/Dlf.hpp"
#include "castor/rh/Client.hpp"
#include "castor/rh/IOResponse.hpp"
#include "castor/rh/EndResponse.hpp"
#include "castor/io/ClientSocket.hpp"
#include "castor/stager/IJobSvc.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/DiskServer.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/job/stagerjob/IPlugin.hpp"
#include "castor/job/stagerjob/StagerJob.hpp"
#include "castor/job/stagerjob/InputArguments.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/exception/Internal.hpp"

// Static map s_plugins
static std::map<std::string, castor::job::stagerjob::IPlugin*> *s_plugins = 0;

// Flag to indicate whether the client has already been sent a callback
static bool clientAnswered = false;

//------------------------------------------------------------------------------
// getPlugin
//------------------------------------------------------------------------------
castor::job::stagerjob::IPlugin*
castor::job::stagerjob::getPlugin(std::string protocol)
  throw (castor::exception::Exception) {
  if (0 != s_plugins && s_plugins->find(protocol) != s_plugins->end()) {
    return s_plugins->operator[](protocol);
  }
  castor::exception::NoEntry e;
  e.getMessage() << "No mover plugin found for protocol "
                 << protocol;
  throw e;
}

//------------------------------------------------------------------------------
// registerPlugin
//------------------------------------------------------------------------------
void castor::job::stagerjob::registerPlugin
(std::string protocol, castor::job::stagerjob::IPlugin* plugin)
  throw () {
  if (0 == s_plugins) {
    s_plugins = new std::map<std::string, castor::job::stagerjob::IPlugin*>();
  }
  s_plugins->operator[](protocol) = plugin;
}

//------------------------------------------------------------------------------
// getJobSvc
//------------------------------------------------------------------------------
castor::stager::IJobSvc* getJobSvc()
  throw (castor::exception::Exception) {
  // Initialize the remote job service
  castor::IService *remsvc =
    castor::BaseObject::services()->service
    ("RemoteJobSvc", castor::SVC_REMOTEJOBSVC);
  if (remsvc == 0) {
    castor::exception::Internal e;
    e.getMessage() << "Unable to get RemoteJobSvc";
    throw e;
  }
  castor::stager::IJobSvc *jobSvc =
    dynamic_cast<castor::stager::IJobSvc *>(remsvc);
  if (jobSvc == 0) {
    castor::exception::Internal e;
    e.getMessage() << "Could not convert newly retrieved service into IJobSvc";
    throw e;
  }
  return jobSvc;
}

//------------------------------------------------------------------------------
// startAndGetPath
//------------------------------------------------------------------------------
std::string startAndGetPath
(castor::job::stagerjob::InputArguments* args,
 castor::job::stagerjob::PluginContext& context)
  throw (castor::exception::Exception) {

  // Create diskserver and filesystem in memory
  castor::stager::DiskServer diskServer;
  diskServer.setName(args->diskServer);
  castor::stager::FileSystem fileSystem;
  fileSystem.setMountPoint(args->fileSystem);
  fileSystem.setDiskserver(&diskServer);
  diskServer.addFileSystems(&fileSystem);

  // Create a subreq in memory and we will just fill its id
  castor::stager::SubRequest subrequest;
  subrequest.setId(args->subRequestId);

  // Get & Update case
  if ((args->accessMode == castor::job::stagerjob::ReadOnly) ||
      (args->accessMode == castor::job::stagerjob::ReadWrite)) {
    bool emptyFile;
    castor::stager::DiskCopy* diskCopy =
      context.jobSvc->getUpdateStart
      (&subrequest, &fileSystem, &emptyFile,
       args->fileId.fileid, args->fileId.server);
    if (diskCopy == NULL) {
      // No DiskCopy return, nothing should be done
      // The job was scheduled for nothing
      // This happens in particular when a diskCopy gets invalidated
      // while the job waits in the scheduler queue
      castor::dlf::Param params[] =
        {castor::dlf::Param("JobId", getenv("LSB_JOBID")),
         castor::dlf::Param(args->subRequestUuid)};
      castor::dlf::dlf_writep
        (args->requestUuid, DLF_LVL_SYSTEM,
         castor::job::stagerjob::JOBNOOP, 2, params, &args->fileId);
      return "";
    }
    std::string fullDestPath = args->fileSystem + diskCopy->path();
    // Deal with recalls of empty files
    if (emptyFile) {
      int thisfd = creat(fullDestPath.c_str(), (S_IRUSR|S_IWUSR));
      if (thisfd < 0) {
        // "Failed to create empty file"
        castor::dlf::Param params[] =
          {castor::dlf::Param("JobId", getenv("LSB_JOBID")),
           castor::dlf::Param("Path", fullDestPath),
           castor::dlf::Param("Error", strerror(errno)),
           castor::dlf::Param(args->subRequestUuid)};
        castor::dlf::dlf_writep
          (args->requestUuid, DLF_LVL_ERROR,
           castor::job::stagerjob::CREATFAILED, 4, params, &args->fileId);
        delete diskCopy;
        castor::exception::Exception e(errno);
        e.getMessage() << "Failed to create empty file";
        throw e;
      }
      if (close(thisfd) != 0) {
        castor::dlf::Param params[] =
          {castor::dlf::Param("JobId", getenv("LSB_JOBID")),
           castor::dlf::Param("Path", fullDestPath),
           castor::dlf::Param("Error", strerror(errno)),
           castor::dlf::Param(args->subRequestUuid)};
        castor::dlf::dlf_writep
          (args->requestUuid, DLF_LVL_ERROR,
           castor::job::stagerjob::FCLOSEFAILED, 4, params, &args->fileId);
      }
    }

    // For Updates we should clear the files extended attributes. There is
    // no need to do something for errors as maybe we have no attributes
    // and we do not have to check RFIOD USE_CHECKSUM
    if (args->accessMode == castor::job::stagerjob::ReadWrite && !emptyFile) {
      removexattr(fullDestPath.c_str(), "user.castor.checksum.value");
      removexattr(fullDestPath.c_str(), "user.castor.checksum.type");
    }

    delete diskCopy;
    return fullDestPath;

    // Put case
  } else {
    // Call putStart
    castor::stager::DiskCopy* diskCopy =
      context.jobSvc->putStart
      (&subrequest, &fileSystem,
       args->fileId.fileid, args->fileId.server);
    std::string fullDestPath = args->fileSystem + diskCopy->path();
    delete diskCopy;
    return fullDestPath;
  }
  // Never reached
  castor::exception::Internal e;
  e.getMessage() << "reached unreachable code !";
  throw e;
}

//------------------------------------------------------------------------------
// switchToCastorSuperuser
//------------------------------------------------------------------------------
void switchToCastorSuperuser(castor::job::stagerjob::InputArguments *args)
  throw (castor::exception::Exception) {

  // "Credentials at start time"
  castor::dlf::Param params[] =
    {castor::dlf::Param("Uid", getuid()),
     castor::dlf::Param("Gid", getgid()),
     castor::dlf::Param("Euid", geteuid()),
     castor::dlf::Param("Egid", getegid()),
     castor::dlf::Param(args->subRequestUuid)};
  castor::dlf::dlf_writep
    (args->requestUuid, DLF_LVL_DEBUG,
     castor::job::stagerjob::JOBORIGCRED, 5, params, &args->fileId);

  // Perform the switch
  castor::System::switchToCastorSuperuser();

  // "Actual credentials used"
  castor::dlf::Param params2[] =
    {castor::dlf::Param("Uid", getuid()),
     castor::dlf::Param("Gid", getgid()),
     castor::dlf::Param("Euid", geteuid()),
     castor::dlf::Param("Egid", getegid()),
     castor::dlf::Param(args->subRequestUuid)};
  castor::dlf::dlf_writep
    (args->requestUuid, DLF_LVL_DEBUG,
     castor::job::stagerjob::JOBACTCRED, 5, params2, &args->fileId);
}

//------------------------------------------------------------------------------
// bindSocketAndListen
//------------------------------------------------------------------------------
void bindSocketAndListen
(castor::job::stagerjob::PluginContext &context,
 std::pair<int,int> &range, int attempts = 0)
  throw (castor::exception::Exception) {
  // Build address
  struct sockaddr_in sin;
  memset(&sin,'\0',sizeof(sin));
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;

  // Set the seed for the new sequence of pseudo-random numbers to be returned
  // by subsequent calls to rand()
  timeval tv;
  gettimeofday(&tv, NULL);
  srand(tv.tv_usec * tv.tv_sec);

  // Loop over all the ports in the specified range starting at a random offset
  int offset = 0;
  int port   = -1;
  int rc     = -1;
  for (offset = (rand() % (range.second - range.first+1)) + range.first;
       port != offset;
       port++) {
    if (port < 0) {
      port = offset;
    }
    // Attempt to bind to the port
    sin.sin_port = htons(port);
    rc = ::bind(context.socket, (struct sockaddr *)&sin, sizeof(sin));
    if (rc == 0) {
      break; // Port successfully bound, doesn't mean listen will succeed!
    }

    // If we reach the maximum allowed port value reset it
    if (port >= range.second) {
      port = 0;
    }
  }
  if (rc != 0) {
    castor::exception::Internal e;
    e.getMessage() << "Unable to bind socket with range ["
                   << range.first << "," << range.second << "]";
    throw e;
  }

  // Listen for the client connection
  if (listen(context.socket, 1) < 0) {
    // If the error is "Address already in use" we try and bind again after 5
    // seconds
    if ((errno == EADDRINUSE) && (attempts < 10)) {
      sleep(5);
      bindSocketAndListen(context, range, attempts + 1);
    }
    castor::exception::Exception e(errno);
    e.getMessage() << "Error caught in call to listen";
    throw e;
  }
  socklen_t len = sizeof(sin);
  if (getsockname(context.socket, (struct sockaddr *)&sin, &len) < 0) {
    castor::exception::Exception e(errno);
    e.getMessage() << "Error caught in call to getsockname";
    throw e;
  }
  context.port = ntohs(sin.sin_port);
}

//------------------------------------------------------------------------------
// process
//------------------------------------------------------------------------------
void process(castor::job::stagerjob::InputArguments* args)
  throw (castor::exception::Exception) {
  // First switch to stage:st privileges
  switchToCastorSuperuser(args);
  // Get an instance of the job service
  castor::stager::IJobSvc* jobSvc = getJobSvc();
  // Get full path of the file we handle
  castor::job::stagerjob::PluginContext context;
  context.host = castor::System::getHostName();
  context.mask = S_IRWXG|S_IRWXO;
  context.jobSvc = jobSvc;
  context.fullDestPath = startAndGetPath(args, context);
  if ("" == context.fullDestPath) {
    // No DiskCopy return, nothing should be done
    // The job was scheduled for nothing
    // This happens in particular when a diskCopy gets invalidated
    // while the job waits in the scheduler queue
    // we've already logged, so just quit
    return;
  }
  // Get proper plugin
  castor::job::stagerjob::IPlugin* plugin =
    castor::job::stagerjob::getPlugin(args->protocol);
  // Create the socket that the user will connect too. Note:
  // xrootd requires no socket as users will connect to the
  // xrd daemon on the machine itself!
  if (args->protocol != "xroot") {
    // Create a socket
    context.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (context.socket < 0) {
      castor::exception::Exception e(errno);
      e.getMessage() << "Error caught in call to socket";
      throw e;
    }
    int rcode = 1;
    int rc = setsockopt(context.socket, SOL_SOCKET, SO_REUSEADDR,
                        (char *)&rcode, sizeof(rcode));
    if (rc < 0) {
      castor::exception::Exception e(errno);
      e.getMessage() << "Error caught in call to setsockopt";
      throw e;
    }
    // Get available port range for the socket
    std::pair<int,int> portRange = plugin->getPortRange(*args);
    // Bind socket and listen for client connection
    bindSocketAndListen(context, portRange);
    // "Mover will use the following port"
    std::ostringstream sPortRange;
    sPortRange << portRange.first << ":" << portRange.second;
    castor::dlf::Param params[] =
      {castor::dlf::Param("Protocol", args->protocol),
       castor::dlf::Param("Available port range", sPortRange.str()),
       castor::dlf::Param("Port used", context.port),
       castor::dlf::Param("JobId", getenv("LSB_JOBID")),
       castor::dlf::Param(args->subRequestUuid)};
    castor::dlf::dlf_writep
      (args->requestUuid, DLF_LVL_DEBUG,
       castor::job::stagerjob::MOVERPORT, 5, params, &args->fileId);
  }
  // Prefork hook for the different movers
  plugin->preForkHook(*args, context);
  // Set our mask to the most restrictive mode
  umask(context.mask);
  // chdir into something else but the root system...
  if (chdir("/tmp") != 0) {
    castor::dlf::Param params[] =
      {castor::dlf::Param("JobId", getenv("LSB_JOBID")),
       castor::dlf::Param(args->subRequestUuid)};
    castor::dlf::dlf_writep
      (args->requestUuid, DLF_LVL_ERROR,
       castor::job::stagerjob::CHDIRFAILED, 2, params, &args->fileId);
    // Not fatal, we just ignore the error
  }
  // Fork and execute the mover
  dlf_prepare();
  context.childPid = fork();
  if (context.childPid < 0) {
    dlf_parent();
    castor::exception::Exception e(errno);
    e.getMessage() << "Error caught in call to fork";
    throw e;
  }
  if (context.childPid == 0) {
    // Child side of the fork
    dlf_child();
    // This call will never come back, since it call execl
    plugin->execMover(*args, context);
    // But in case, let's fail
    dlf_shutdown(5);
    exit(EXIT_FAILURE);
  }
  // Parent side of the fork
  dlf_parent();
  plugin->postForkHook(*args, context);
}

//------------------------------------------------------------------------------
// sendResponse
//------------------------------------------------------------------------------
void castor::job::stagerjob::sendResponse
(castor::IClient *client,
 castor::rh::IOResponse &response)
  throw (castor::exception::Exception) {
  castor::rh::Client* rhc = dynamic_cast<castor::rh::Client*>(client);
  if (0 == rhc) {
    castor::exception::Internal e;
    e.getMessage() << "Unable to reply to client, unknown client type : "
                   << client->type();
    throw e;
  }
  if (clientAnswered == false) {
    clientAnswered = true;
    castor::io::ClientSocket s(rhc->port(), rhc->ipAddress());
    s.connect();
    s.sendObject(response);
    castor::rh::EndResponse endRes;
    s.sendObject(endRes);
  } else {
    // The client has already been sent a response, trying to send a second
    // one will just result in a failure to connect.
  }
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(int argc, char** argv) {

  // Record start time
  timeval tv;
  gettimeofday(&tv, NULL);
  u_signed64 startTime = (tv.tv_sec * 1000000) + tv.tv_usec;

  // Ignore SIGPIPE to avoid being brutally interrupted
  // because of network [write] error
  signal(SIGPIPE,SIG_IGN);
  castor::job::stagerjob::InputArguments* arguments = 0;

  try {
    // Initializing logging
    using namespace castor::job::stagerjob;
    castor::dlf::Message messages[] = {

      // System call errors
      { CREATFAILED,     "Failed to create empty file" },
      { FCLOSEFAILED,    "Failed to close file" },
      { SCLOSEFAILED,    "Failed to close socket" },
      { CHDIRFAILED,     "Failed to change directory to tmp" },
      { DUP2FAILED,      "Failed to duplicate socket" },
      { MOVERNOTEXEC,    "Mover program cannot be executed. Check permissions" },
      { EXECFAILED,      "Failed to exec mover" },

      // Invalid configurations or parameters
      { INVRETRYINT,     "Invalid Job/RetryInterval option, using default" },
      { INVRETRYNBAT,    "Invalid Job/RetryAttempts option, using default" },
      { DOWNRESFILE,     "Downloading resource file" },
      { INVALIDURI,      "Invalid Uniform Resource Indicator, cannot download resource file" },
      { MAXATTEMPTS,     "Exceeded maximum number of attempts trying to download resource file" },
      { DOWNEXCEPT,      "Exception caught trying to download resource file" },
      { INVALRESCONT,    "The content of the resource file is invalid" },

      // Informative logs
      { JOBSTARTED,      "Job Started" },
      { JOBENDED,        "Job finished successfully" },
      { JOBFAILED,       "Job failed" },
      { JOBORIGCRED,     "Credentials at start time" },
      { JOBACTCRED,      "Actual credentials used" },
      { JOBNOOP,         "No operation performed" },
      { FORKMOVER,       "Forking mover" },
      { REQCANCELED,     "Request canceled" },
      { MOVERPORT,       "Mover will use the following port" },
      { MOVERFORK,       "Mover fork uses the following command line" },
      { ACCEPTCONN,      "Client connected" },
      { JOBFAILEDNOANS,  "Job failed before it could send an answer to client" },

      // Errors
      { STAT64FAIL,      "rfio_stat64 error" },
      { CHILDEXITED,     "Child exited" },
      { CHILDSIGNALED,   "Child exited due to uncaught signal" },
      { CHILDSTOPPED,    "Child was stopped" },
      { NOANSWERSENT,    "Could not send answer to client" },
      { GETATTRFAILED,   "Failed to get checksum information from extended attributes" },
      { CSTYPENOTSOP,    "Unsupported checksum type, ignoring checksum information" },

      // Protocol specific. Should not be here if the plugins
      // were properly packaged in separate libs
      { GSIBADPORT,      "Invalid port range for GridFTP in config file. using default" },
      { GSIBADMINPORT,   "Invalid lower bound for GridFTP port range in config file. Using default" },
      { GSIBADMAXPORT,   "Invalid upper bound for GridFTP port range in config file. Using default" },
      { GSIBADMINVAL,    "Lower bound for GridFTP port range not in valid range. Using default" },
      { GSIBADMAXVAL,    "Upper bound for GridFTP port range not in valid range. Using default" },

      { XROOTENOENT,     "Xrootd is not installed" },

      { RFIODBADPORT,    "Invalid port range for RFIOD in config file. using default" },
      { RFIODBADMINPORT, "Invalid lower bound for RFIOD port range in config file. Using default" },
      { RFIODBADMAXPORT, "Invalid upper bound for RFIOD port range in config file. Using default" },
      { RFIODBADMINVAL,  "Lower bound for RFIOD port range not in valid range. Using default" },
      { RFIODBADMAXVAL,  "Upper bound for RFIOD port range not in valid range. Using default" },

      { ROOTDBADPORT,    "Invalid port range for ROOT in config file. using default" },
      { ROOTDBADMINPORT, "Invalid lower bound for ROOT port range in config file. Using default" },
      { ROOTDBADMAXPORT, "Invalid upper bound for ROOT port range in config file. Using default" },
      { ROOTDBADMINVAL,  "Lower bound for ROOT port range not in valid range. Using default" },
      { ROOTDBADMAXVAL,  "Upper bound for ROOT port range not in valid range. Using default" },

      { -1, "" }};
    castor::dlf::dlf_init("Job", messages);

    // stagerJob does not inherit from the BaseDaemon so we must manually
    // trigger the creation of the logging threads.
    dlf_create_threads(0);

    // Parse the command line
    arguments = new castor::job::stagerjob::InputArguments(argc, argv);

  } catch (castor::exception::Exception e) {
    // Something went wrong but we are not yet in a situation
    // where we can inform the client. So log it and return straight
    // "Job failed before it could send an answer to client"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Error", sstrerror(e.code())),
       castor::dlf::Param("Message", e.getMessage().str()),
       castor::dlf::Param("JobId", getenv("LSB_JOBID"))};
    castor::dlf::dlf_writep
      (nullCuuid, DLF_LVL_ERROR,
       castor::job::stagerjob::JOBFAILEDNOANS, 3, params);
    dlf_shutdown(10);
    return -1;
  }

  try {

    // Construct command line
    std::string stagerConcatenatedArgv;
    for (int i = 0; i < argc; i++) {
      stagerConcatenatedArgv += argv[i];
      stagerConcatenatedArgv += " ";
    }

    // Compute waiting time of the request
    struct timeval tv;
    gettimeofday(&tv, NULL);
    double totalWaitTime = tv.tv_usec;
    totalWaitTime = totalWaitTime/1000000 +
      tv.tv_sec - arguments->requestCreationTime;

    // "Job started"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Arguments", stagerConcatenatedArgv),
       castor::dlf::Param("JobId", getenv("LSB_JOBID")),
       castor::dlf::Param("Type", castor::ObjectsIdStrings[arguments->type]),
       castor::dlf::Param("Protocol", arguments->protocol),
       castor::dlf::Param("TotalWaitTime", totalWaitTime),
       castor::dlf::Param(arguments->subRequestUuid)};
    castor::dlf::dlf_writep
      (arguments->requestUuid, DLF_LVL_SYSTEM,
       castor::job::stagerjob::JOBSTARTED, 5, params, &arguments->fileId);

    // Call stagerJobProcess
    process(arguments);

    // Calculate statistics
    gettimeofday(&tv, NULL);
    signed64 elapsedTime =
      (((tv.tv_sec * 1000000) + tv.tv_usec) - startTime);

    // "Job finished successfully"
    castor::dlf::Param params2[] =
      {castor::dlf::Param("JobId", getenv("LSB_JOBID")),
       castor::dlf::Param("ElapsedTime", elapsedTime  * 0.000001),
       castor::dlf::Param(arguments->subRequestUuid)};
    castor::dlf::dlf_writep
      (arguments->requestUuid, DLF_LVL_SYSTEM,
       castor::job::stagerjob::JOBENDED, 3, params2, &arguments->fileId);

    // Memory cleanup
    delete arguments;

  } catch (castor::exception::Exception e) {
    if (e.code() == ESTREQCANCELED) {
      // "manually" catch the RequestCanceled exception
      // these are converted to regular Exception objects
      // by the internal remote procedure call mechanism
      castor::dlf::Param params[] =
        {castor::dlf::Param("JobId", getenv("LSB_JOBID")),
         castor::dlf::Param(arguments->subRequestUuid)};
      castor::dlf::dlf_writep
        (arguments->requestUuid, DLF_LVL_SYSTEM,
         castor::job::stagerjob::REQCANCELED, 2, params, &arguments->fileId);
    } else {
      // "Job failed"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Error", sstrerror(e.code())),
         castor::dlf::Param("Message", e.getMessage().str()),
         castor::dlf::Param("JobId", getenv("LSB_JOBID")),
         castor::dlf::Param(arguments->subRequestUuid)};
      castor::dlf::dlf_writep
        (arguments->requestUuid, DLF_LVL_ERROR,
         castor::job::stagerjob::JOBFAILED, 4, params, &arguments->fileId);
      // Try to answer the client
      try {
        castor::rh::IOResponse ioResponse;
        ioResponse.setErrorCode(e.code());
        ioResponse.setErrorMessage(e.getMessage().str());
        castor::job::stagerjob::sendResponse(arguments->client, ioResponse);
      } catch (castor::exception::Exception e2) {
        // "Could not send answer to client"
        castor::dlf::Param params[] =
          {castor::dlf::Param("Error", sstrerror(e2.code())),
           castor::dlf::Param("Message", e2.getMessage().str()),
           castor::dlf::Param("JobId", getenv("LSB_JOBID")),
           castor::dlf::Param(arguments->subRequestUuid)};
        castor::dlf::dlf_writep
          (arguments->requestUuid, DLF_LVL_ERROR,
           castor::job::stagerjob::NOANSWERSENT, 4, params, &arguments->fileId);
      }
    }
    // Memory cleanup
    if (0 != arguments) delete arguments;
    dlf_shutdown(10);
    return -1;
  }
  dlf_shutdown(10);
}

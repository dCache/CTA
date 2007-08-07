/******************************************************************************
 *                      JobManagerDaemon.cpp
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
 * @(#)$RCSfile: JobManagerDaemon.cpp,v $ $Revision: 1.1 $ $Release$ $Date: 2007/08/07 14:56:32 $ $Author: waldron $
 *
 * @author Dennis Waldron
 *****************************************************************************/

// Include files
#include "castor/jobmanager/JobManagerDaemon.hpp"
#include "castor/jobmanager/CancellationThread.hpp"
#include "castor/jobmanager/DispatchThread.hpp"
#include "castor/jobmanager/SubmissionProcess.hpp"
#include "castor/server/SignalThreadPool.hpp"
#include "castor/server/ForkedProcessPool.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/Constants.hpp"
#include "getconfent.h"
#include "signal.h"

// Definitions
#define DEFAULT_KILLRETRY_INTERVAL 30
#define DEFAULT_DISPATCH_INTERVAL  10
#define DEFAULT_NOTIFICATION_PORT  15011
#define DEFAULT_PREFORKED_WORKERS  20


//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {

  try {
    castor::jobmanager::JobManagerDaemon daemon;
    
    // Attempt to find the LSF cluster and master name for logging purposes.
    // This isn't really required but could be useful for future debugging
    // efforts
    std::string clusterName("Unknown");
    std::string masterName("Unknown");
    char **results = NULL;
    
    // Errors are ignored here!
    lsb_init("JobManager");
    clusterInfo *cInfo = ls_clusterinfo(NULL, NULL, results, 0, 0);
    if (cInfo != NULL) {
      clusterName = cInfo[0].clusterName;
      masterName  = cInfo[0].masterName;
    }

    // "JobManager Daemon started"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Cluster", clusterName),
       castor::dlf::Param("Master", masterName)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 1, 2, params);

    // Create the preforked workers used to submit jobs into LSF. We do this
    // before creating any threads to minimise duplication of thread specific
    // data in the child process. The thread of course will be dead, its data
    // however will not! fork(2)
    char *value = getconfent("JobManager", "PreforkedWorkers", 0);
    int processes = DEFAULT_PREFORKED_WORKERS;
    if (value) {
      processes = std::strtol(value, 0, 10);
      if (processes == 0) {
	processes = DEFAULT_PREFORKED_WORKERS;
      } else if (processes > 200) {
	castor::exception::Exception e(EINVAL);
	e.getMessage() << "Too many PreforkedWorkers configured: " << processes
		       << "- must be < 200" << std::endl;
	throw e;
      }
    }

    // Determine if the job manager should perform sanity checks on the uids
    // that it retrieves from the database. These checks include a verse lookup
    // of uid to username and checks to see if the user belongs to the group
    // reported.
    value = getconfent("JobManager", "ReverseUidLookups", 0);
    bool reverseUidLookups = true;
    if (value) {
      if (!strcasecmp(value, "no")) {
	reverseUidLookups = false;
      } else if (!strcasecmp(value, "yes")) {
	castor::exception::Exception e(EINVAL);
	e.getMessage() << "Invalid option for ReverseUidLookups: " << value
		       << "- must be 'yes' or 'no'" << std::endl;
	throw e;
      }
    }

    // For LSF a shared location is used to store notification files from the
    // scheduler which are read/downloaded by stagerJob. It is mandatory for
    // this to be defined in the configuration file. No default is defined!
    value = getconfent("JobManager", "SharedLSFResource", 0);
    if (!value) {
      castor::exception::Exception e(EINVAL);
      e.getMessage() << "Missing configuration option " 
		     << "JobManager/SharedLSFResource" << std::endl;
      throw e;
    }

    // Forked Process Pool
    daemon.addThreadPool
      (new castor::server::ForkedProcessPool
       ("ForkedProcessPool",
	 new castor::jobmanager::SubmissionProcess(reverseUidLookups, value)));
    daemon.getThreadPool('F')->setNbThreads(processes);

    // Determine the notification port for the job dispatch thread. The port
    // will be used to receive UDP notifications from the stager to wake up
    // the thread when a job requires LSF submission
    int notifyPort = DEFAULT_NOTIFICATION_PORT;
    if ((value = getconfent("JOBMANAGER", "NOTIFYPORT", 0))) {
      notifyPort = std::strtol(value, 0, 10);
      if (notifyPort == 0) {
	notifyPort = DEFAULT_NOTIFICATION_PORT;
      } else if (notifyPort > 65535) {
	castor::exception::Exception e(EINVAL);
	e.getMessage() << "Invalid NOTIFYPORT value configured: " << notifyPort
		       << "- must be < 65535" << std::endl;
	throw e;
      }
    }

    // Dispatch ThreadPool daemon.getThreadPool('F')->getThread()
    daemon.addThreadPool
      (new castor::server::SignalThreadPool
       ("DispatchThread",
	new castor::jobmanager::DispatchThread
	(dynamic_cast<castor::server::ForkedProcessPool*>
	 (daemon.getThreadPool('F'))), notifyPort, DEFAULT_DISPATCH_INTERVAL));
    daemon.getThreadPool('D')->setNbThreads(1);

    // Determine the polling interval for the cancellation thread. This thread
    // communicates with LSF. A polling interval which is too small will most 
    // likely stress the LSF master unnecessarily, maybe even kill it!
    value = getconfent("JobManager", "KillRetryInterval", 0);
    int interval = DEFAULT_KILLRETRY_INTERVAL;
    if (value) {
      interval = std::strtol(value, 0, 10);
      if (interval == 0) {
	interval = DEFAULT_KILLRETRY_INTERVAL;
      } else if (interval < 10) {
	castor::exception::Exception e(EINVAL);
	e.getMessage() << "KillRetryInterval value too small: " << processes
		       << "- must be > 10" << std::endl;
	throw e;
      }
    }

    // Cancellation ThreadPool
    daemon.addThreadPool
      (new castor::server::SignalThreadPool
       ("CancellationThread",
	new castor::jobmanager::CancellationThread
	(interval), 0, interval));
    daemon.getThreadPool('C')->setNbThreads(1);

    // Start daemon
    daemon.parseCommandLine(argc, argv);
    daemon.start();
    return 0;

  } catch (castor::exception::Exception e) {
    std::cerr << "Caught exception: "
	      << sstrerror(e.code()) << std::endl
	      << e.getMessage().str() << std::endl;

    // "Exception caught when starting JobManager"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Code", sstrerror(e.code())),
       castor::dlf::Param("Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 3, 2, params);
  } catch (...) {
    std::cerr << "Caught exception!" << std::endl;
  }

  return 1;
}


//-----------------------------------------------------------------------------
// Cconstructor
//-----------------------------------------------------------------------------
castor::jobmanager::JobManagerDaemon::JobManagerDaemon():
  castor::server::BaseDaemon("JobManager") {

  // Now with predefined messages
  castor::dlf::Message messages[] = {

     // General
     {  1, "JobManager Daemon started" },
     {  2, "Failed to initialize the LSF batch library (LSBLIB)" },
     {  3, "Exception caught when starting JobManager" },

     // Cancellation
     { 20, "Invalid JobManager/PendingTimeout option, ignoring entry" },
     { 21, "Failed to retrieve historical LSF job information from batch master" },
     { 22, "Failed to retrieve current LSF job information from batch master" },
     { 23, "Failed to extract CLEAN_PERIOD value from lsb.params, using default" },
     { 24, "Failed to terminate LSF job" },
     { 25, "Job terminated, timeout occurred" },
     { 26, "Job terminated by service administrator" },
     { 27, "Cancellation thread interval is greater then CLEAN_PERIOD in lsb.params, detection of abnormally EXITING jobs disabled" },
     { 28, "Exception caught in trying to fail subrequest" },
     { 29, "Failed to execute failSchedulerJob procedure" },
    
     // Submission
     { 40, "Invalid JobManager/SubmitRetryAttempts option, using default" },
     { 41, "Invalid JobManager/SubmitRetryInterval option, using default" },
     { 42, "Reverse UID lookup failure. User credentials are invalid, job will not be scheduled" },
     { 43, "Failed to change real and effective user id of process using setreuid, job cannot be submitted into the scheduler as user root " },
     { 44, "Failed to reset the real and effective user id back to root. Terminating further processing through this worker process" },
     { 45, "Job successfully submitted into LSF" },
     { 46, "Failed to submit job into LSF, will try again" },
     { 47, "Exceeded maximum number of attempts trying to submit a job into LSF" },
     { 48, "Failed to submit job into LSF, fatal error encountered" },
     { 49, "Exception caught trying to submit a job into LSF" },
     { 50, "Failed to execute lsfSubmit in SubmissionProcess::run" },
     
     // Dispatch
     { 60, "Job received" },
     { 61, "Exception caught selecting a new job to schedule in DispatchThread::select" },
     { 62, "Failed to execute jobToSchedule procedure in DispatchThread::select" },
     { 63, "Exception caught trying to restart a job in DispatchThread::process, job will remain incorrectly in SUBREQUEST_BEINGSCHED" },
     { 64, "Failed to execute updateSchedulerJob in DispatchThread::process, job will remain incorrectly in SUBREQUEST_BEINGSCHED" },

     { -1, "" }};
  dlfInit(messages);
}

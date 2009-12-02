/******************************************************************************
 *                      ManagementThread.cpp
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
 * @(#)$RCSfile: ManagementThread.cpp,v $ $Revision: 1.14 $ $Release$ $Date: 2009/03/25 13:23:30 $ $Author: waldron $
 *
 * Management thread used to handle processing of jobs that have been PENDING
 * for too long in LSF + synchronization of exiting jobs in LSF.
 *
 * @author Dennis Waldron
 *****************************************************************************/

// Include files
#include "castor/jobmanager/ManagementThread.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/Constants.hpp"
#include "castor/IService.hpp"
#include "castor/Services.hpp"
#include "getconfent.h"
#include "u64subr.h"
#include <sstream>
#include <algorithm>


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
castor::jobmanager::ManagementThread::ManagementThread()
  throw(castor::exception::Exception) :
  m_defaultQueue("castor") {}


//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void castor::jobmanager::ManagementThread::init() {

  // Initialize the oracle job manager service.
  castor::IService *orasvc =
    castor::BaseObject::services()->service
    ("OraJobManagerSvc", castor::SVC_ORAJOBMANAGERSVC);
  if (orasvc == NULL) {
    castor::exception::Internal e;
    e.getMessage() << "Unable to get OraJobManagerSVC for ManagementThread";
    throw e;
  }

  m_jobManagerService =
    dynamic_cast<castor::jobmanager::IJobManagerSvc *>(orasvc);
  if (m_jobManagerService == NULL) {
    castor::exception::Internal e;
    e.getMessage() << "Could not convert newly retrieved service into "
                   << "IJobManagerSvc for ManagementThread" << std::endl;
    throw e;
  }

  // Initialize the LSF API
  //   - This doesn't actually communicate with the LSF master in any way,
  //     it just sets up the environment for later calls
  if (lsb_init((char*)"jobmanagerd") < 0) {
    
    // "Failed to initialize the LSF batch library (LSBLIB)"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Function", "lsb_init"),
       castor::dlf::Param("Error", lsberrno ? lsb_sysmsg() : "no message")};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 2, 2, params);
  }
  
  // Determine the value of DEFAULT_QUEUE as defined in lsb.params. This will
  // be used later to clean up the notification cache.
  parameterInfo *paramInfo = lsb_parameterinfo(NULL, NULL, 0);
  if (paramInfo == NULL) {
    
    // "Failed to extract the DEFAULT_QUEUE value from lsb.params, using
    // defaults"
    castor::dlf::Param params[] =
      {castor::dlf::Param("DefaultQueue", m_defaultQueue)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 23, 1, params);
  } else {
    m_defaultQueue = paramInfo->defaultQueues;
  }
}


//-----------------------------------------------------------------------------
// Run
//-----------------------------------------------------------------------------
void castor::jobmanager::ManagementThread::run(void *param) {

  // LSF doesn't have a built in mechanism to automatically remove jobs from
  // the queue if they spend too much time in a PENDING status. So we must
  // issue an external kill command to achieve this.
  char **results;
  int  count, i;
  m_pendingTimeouts.clear();
  if (!getconfent_multi("JobManager", "PendingTimeouts", 1, &results, &count)) {
    for (i = 0; i < count; i++) {
      std::istringstream iss(results[i]);
      std::string svcclass;
      u_signed64 timeout;

      // Extract the service class and timeout value from the configuration
      // option.
      std::getline(iss, svcclass, ':');
      iss >> timeout;

      if (iss.fail()) {
        // "Invalid JobManager/PendingTimeouts option, ignoring entry"
        castor::dlf::Param params[] =
          {castor::dlf::Param("Value", results[i]),
           castor::dlf::Param("Index", i + 1)};
        castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 20, 2, params);
      } else {
        m_pendingTimeouts[svcclass] = timeout;
      }
      free(results[i]);
    }
    free(results);
  }

  // Extract the DiskCopyPendingTimeout value which defines how long a disk
  // copy transfer can remain pending in LSF.
  m_diskCopyPendingTimeout = 0;
  char *value = getconfent("JobManager", "DiskCopyPendingTimeout", 0);
  if (value != NULL) {
    m_diskCopyPendingTimeout = std::strtol(value, 0, 10);
  }

  // The job manager also has the ability to terminate any job in LSF if the
  // all of the requested filesystems are no longer available. This prevents
  // the queues from accumulating jobs that will never run. For example, when
  // a diskserver is removed from production and never returns.
  m_resReqKill = true;
  if ((value = getconfent("JobManager", "ResReqKill", 0)) != NULL) {
    if (!strcasecmp(value, "no")) {
      m_resReqKill = false;
    }
  }

  // Get a list of all jobs known to the stager database and possible reasons
  // for termination
  std::map<std::string, std::pair<bool, bool> > databaseJobs;
  try {
    databaseJobs = m_jobManagerService->getRunningTransfers();
  } catch (castor::exception::Exception e) {

    // "Exception caught trying to get a list of running transfers from the "
    // stager database"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Type", sstrerror(e.code())),
       castor::dlf::Param("Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 74, 2, params);
    return;
  } catch (...) {

    // "Failed to execute getRunningTransfers"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Message", "General exception caught")};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 75, 1, params);
    return;
  }

  // Get a list of all jobs known to the scheduler
  std::map<std::string, castor::jobmanager::JobRequest> lsfJobs;
  try {
    lsfJobs = getSchedulerJobs();
  } catch (castor::exception::Exception e) {
    // "Exception caught trying to get list of LSF jobs"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 78, 1, params);
    return;
  }

  // A container to hold a list of jobs which have finished or been
  // terminated and the reason why
  std::vector<std::pair<std::string, int> > exitedJobs;

  // Compare the list of jobs known to the stager and LSF to determine those
  // which require termination and cleanup
  for (std::map<std::string, std::pair<bool, bool> >::const_iterator it =
         databaseJobs.begin();
       it != databaseJobs.end();
       it++) {
    std::string subReqId = (*it).first;
    std::map<std::string, castor::jobmanager::JobRequest>::const_iterator
      it2 = lsfJobs.find(subReqId);
    int error = 0;
    if (it2 != lsfJobs.end()) {
      // Process the job
      error = processJob((*it2).second,
                         (*it).second.first,
                         (*it).second.second);
    } else {
      // We have a job in the database which is not in LSF, we assume a
      // scheduling error has occurred
      error = ESTSCHEDERR;
    }
    if (error) {
      exitedJobs.push_back
        (std::pair<std::string, int>(subReqId, error));
      
      // Convert the string representations of the SubRequest uuid to a 
      // Cuuid_t structure
      Cuuid_t subRequestId;
      string2Cuuid(&subRequestId, (char *)subReqId.c_str());
      
      // "Executing cleanup for job"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Error", sstrerror(error)),
         castor::dlf::Param(subRequestId)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 77, 2, params);
    } 
  }

  // Bulk terminate the jobs that exited or were terminated
  try {
    m_jobManagerService->jobFailed(exitedJobs);
  } catch (castor::exception::Exception e) {

    // "Exception caught when trying to fail scheduler job"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Type", sstrerror(e.code())),
       castor::dlf::Param("Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 55, 2, params);
  } catch (...) {
    
    // "Failed to execute jobFailed procedure"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Message", "General exception caught")};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 76, 1, params);
  }
}


//-----------------------------------------------------------------------------
// GetSchedulerJobs
//-----------------------------------------------------------------------------
std::map<std::string, castor::jobmanager::JobRequest> 
castor::jobmanager::ManagementThread::getSchedulerJobs()
  throw(castor::exception::Exception) {

  // Retrieve a list of all jobs
  std::map<std::string, castor::jobmanager::JobRequest> lsfJobs;
  jobInfoEnt *job;
  if (lsb_openjobinfo(0, NULL, (char *)"all", NULL, NULL, CUR_JOB) < 0) {
    if (lsberrno == LSBE_NO_JOB) {
      lsb_closejobinfo(); // No matching job found
    } else {
      lsb_closejobinfo();
      castor::exception::Internal e;
      e.getMessage() << "Failed to retrieve LSF job information from batch "
                     << "master: "
                     << lsberrno ? lsb_sysmsg() : "no message";
      throw e;
    }
  } else {
    
    // Loop over all jobs
    while ((job = lsb_readjobinfo(NULL)) != NULL) {
      try {
        lsfJobs[job->submit.jobName] = extractJobInfo(job);
      } catch (castor::exception::Exception e) {
        // An exception in the extractJobInfo method indicates that the LSF
        // job being read is not a valid CASTOR job so we skip it!
      }
    }
    lsb_closejobinfo();
  }
  return lsfJobs;
}


//-----------------------------------------------------------------------------
// ExtractJobInfo
//-----------------------------------------------------------------------------
castor::jobmanager::JobRequest 
castor::jobmanager::ManagementThread::extractJobInfo(const jobInfoEnt *job)
  throw (castor::exception::Exception) {

  // Parse the jobs external scheduler options to extract the needed key value
  // pairs to process the job further.
  if (job->submit.extsched == NULL) {
    castor::exception::Internal e;
    e.getMessage() << "Invalid job, no external scheduler options";
    throw e;
  }

  // Process the key value pairs.
  std::istringstream extsched(job->submit.extsched);
  std::string buf, key, value;
  castor::jobmanager::JobRequest request;

  while (std::getline(extsched, buf, ';')) {
    std::istringstream iss(buf);
    std::getline(iss, key, '=');
    iss >> value;
    if (!iss.fail()) {
      if (key == "REQUESTID") {
        request.setReqId(value);
      } else if (key == "SUBREQUESTID") {
        request.setSubReqId(value);
      } else if (key == "FILEID") {
        request.setFileId(strutou64((char *)value.c_str()));
      } else if (key == "NSHOST") {
        request.setNsHost(value);
      } else if (key == "TYPE") {
        request.setRequestType(strtou64((char *)value.c_str()));
      } else if (key == "RFS") {
        request.setRequestedFileSystems(value);
      }
    }
  }

  // Extract the job id, username and submission time.
  request.setId(job->jobId);
  request.setUsername(job->user);
  request.setSubmitStartTime(job->submitTime);

  // Determine the name of the service class being used.
  request.setSvcClass(job->submit.queue);
  if (request.svcClass() == m_defaultQueue) {
    request.setSvcClass("default");
  }

  // Check that all the required parameters were extracted.
  if (!request.nsHost().length() || !request.fileId() ||
      !request.reqId().length()  || !request.subReqId().length() ||
      !request.requestType()) {
    castor::exception::Internal e;
    e.getMessage() << "Invalid job, mandatory job parameters missing";
    throw e;
  }
  
  return request;
}


//-----------------------------------------------------------------------------
// ProcessJob
//-----------------------------------------------------------------------------
int castor::jobmanager::ManagementThread::processJob
(const castor::jobmanager::JobRequest job,
 const bool noSpace,
 const bool noFSAvail) {

  // Logging variables
  Cuuid_t requestId, subRequestId;
  Cns_fileid fileId;
  memset(&fileId, 0, sizeof(fileId));
  
  // Check if the job is a candidate for termination if it has exceeded its
  // maximum amount of time in the queue in a PENDING state
  u_signed64 timeout = time(NULL);
  if (job.requestType() != OBJ_StageDiskCopyReplicaRequest) {
    std::map<std::string, u_signed64>::const_iterator it =
      m_pendingTimeouts.find(job.svcClass());

    // If no svcclass entry can be found in the PendingTimeout option check for
    // a default signified by an "all" svcclass name.
    if (it == m_pendingTimeouts.end()) {
      it = m_pendingTimeouts.find("all");
    }
    if (it != m_pendingTimeouts.end()) {
      timeout = (*it).second;
    }
  } else {
    timeout = m_diskCopyPendingTimeout;
  }

  // Convert the string representations of the uuids to Cuuid_t structures
  string2Cuuid(&requestId,    (char *)job.reqId().c_str());
  string2Cuuid(&subRequestId, (char *)job.subReqId().c_str());
  
  // Populate the ns invariant
  strncpy(fileId.server, job.nsHost().c_str(), CA_MAXHOSTNAMELEN);
  fileId.server[CA_MAXHOSTNAMELEN] = '\0';
  fileId.fileid = job.fileId();

  // A timeout was defined ?
  if ((timeout > 0) && 
      ((u_signed64)(time(NULL) - job.submitStartTime()) > timeout)) {

    // "Job terminated, timeout occurred"
    castor::dlf::Param params[] =
      {castor::dlf::Param("JobId", job.id()),
       castor::dlf::Param("Username", job.username()),
       castor::dlf::Param("Type", castor::ObjectsIdStrings[job.requestType()]),
       castor::dlf::Param("Timeout", timeout),
       castor::dlf::Param("SvcClass", job.svcClass()),
       castor::dlf::Param(subRequestId)};
    castor::dlf::dlf_writep(requestId, DLF_LVL_WARNING, 25, 6, params, &fileId);
    if (killJob(job.id())) {
      return ESTJOBTIMEDOUT;
    }
    return 0;
  }

  // All further processing requires resource killing to be active.
  if (!m_resReqKill) {
    return 0;
  }

  // Terminate the job if the target service classes is a diskonly service
  // class which no longer has any space available.
  if ((noSpace) && 
      ((job.requestType() == OBJ_StageDiskCopyReplicaRequest) ||
       (job.requestType() == OBJ_StagePutRequest) ||
       (job.requestType() == OBJ_StageUpdateRequest))) {

    // "Job terminated, svcclass no longer has any space available"
    castor::dlf::Param params[] =
      {castor::dlf::Param("JobId", job.id()),
       castor::dlf::Param("Username", job.username()),
       castor::dlf::Param("Type", castor::ObjectsIdStrings[job.requestType()]),
       castor::dlf::Param("SvcClass", job.svcClass()),
       castor::dlf::Param(subRequestId)};
    castor::dlf::dlf_writep(requestId, DLF_LVL_WARNING, 39, 5, params, &fileId);
    if (killJob(job.id())) {
      return ENOSPC;
    }
    return 0;
  }

  int msg_no = 0;
  if (noFSAvail) {
    if (job.requestType() == OBJ_StageDiskCopyReplicaRequest) {
      // "Job terminated, source filesystem for disk2disk copy is DISABLED, "
      // restarting SubRequest"
      msg_no = 33;
    } else if (job.requestType() == OBJ_StagePutRequest) {
      // "Job terminated, svcclass has no filesystems in PRODUCTION"
      msg_no = 34;
    } else {
      // "Job terminated, all requested filesystems are DRAINING or DISABLED"
      msg_no = 32;
    }
    // Log message
    castor::dlf::Param params[] =
      {castor::dlf::Param("JobId", job.id()),
       castor::dlf::Param("Username", job.username()),
       castor::dlf::Param("Type", castor::ObjectsIdStrings[job.requestType()]),
       castor::dlf::Param("SvcClass", job.svcClass()),
       castor::dlf::Param(subRequestId)};
    castor::dlf::dlf_writep(requestId, DLF_LVL_WARNING, msg_no, 5, params, &fileId);
    if (killJob(job.id())) {
      return ESTNOTAVAIL;
    }
    return 0;
  }

  return 0;
}


//-----------------------------------------------------------------------------
// KillJob
//-----------------------------------------------------------------------------
bool castor::jobmanager::ManagementThread::killJob(const LS_LONG_INT jobId) {

  // LSF has the capacity to kill jobs on bulk (lsb_killbulkjobs). However
  // testing has shown that this call fails to invoke the deletion hook of the
  // scheduler plugin and results in a memory leak in the plugin. (Definitely
  // a LSF bug!)
  if (lsb_forcekilljob(jobId) < 0) {
    if ((lsberrno == LSBE_NO_JOB) || (lsberrno == LSBE_JOB_FINISH)) {
      // No matching job found, most likely killed by another daemon
      return false;
    }

    // "Failed to terminate LSF job"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Function", "lsb_forcekilljob"),
       castor::dlf::Param("Error", lsberrno ? lsb_sysmsg() : "no message"),
       castor::dlf::Param("JobId", (int)jobId)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 24, 3, params);
    return false;
  }
  return true;
}

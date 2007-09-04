/******************************************************************************
 *                      DeletionThread.cpp
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
 * @(#)$RCSfile: DeletionThread.cpp,v $ $Revision: 1.1 $ $Release$ $Date: 2007/09/04 12:38:44 $ $Author: waldron $
 *
 * Deletion thread used to check periodically whether files need to be deleted
 *
 * @author Dennis Waldron
 *****************************************************************************/

// Include files
#include "castor/gc/DeletionThread.hpp"
#include "castor/Services.hpp"
#include "castor/Constants.hpp"
#include "castor/stager/IGCSvc.hpp"
#include "castor/stager/GCLocalFile.hpp"
#include "castor/System.hpp"
#include "getconfent.h"

#include <vector>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

// Definitions
#define DEFAULT_GCINTERVAL 300


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
castor::gc::DeletionThread::DeletionThread(): 
  m_interval(DEFAULT_GCINTERVAL) {};


//-----------------------------------------------------------------------------
// Run
//-----------------------------------------------------------------------------
void castor::gc::DeletionThread::run(void *param) {  

  // "Starting Garbage Collector Daemon" message
  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 1);
  
  // Get RemoteGCSvc
  castor::IService* svc =
    castor::BaseObject::services()->
    service("RemoteGCSvc", castor::SVC_REMOTEGCSVC);
  if (0 == svc) {
    // "Could not get RemoteStagerSvc" message
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 2);
    return;
  }
  
  castor::stager::IGCSvc *stgSvc =
    dynamic_cast<castor::stager::IGCSvc*>(svc);
  if (0 == stgSvc) {
    // "Got a bad RemoteStagerSvc" message
    castor::dlf::Param params[] =
      {castor::dlf::Param("ID", svc->id()),
       castor::dlf::Param("Name", svc->name())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 3, 2, params);
    return;
  }

  // Retrieve DiskServerName, first attempt is environment, second attempt 
  // using castor.conf and finally gethostname
  char *value;
  try {
    if ((value = getenv("GC_TARGET")) || 
	(value = getconfent("GC", "TARGET", 0))) {
      m_diskServerName = value;
    } else {
      m_diskServerName = castor::System::getHostName();
    }
  } catch (castor::exception::Exception e) {
    // "Exception caught trying to getHostName"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Code", strerror(e.code())),
       castor::dlf::Param("Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 17, 2, params);   
    return;
  }
  
  // Get the GC Interval
  if ((value = getenv("GC_INTERVAL")) || 
      (value = getconfent("GC", "INTERVAL", 0))) {
    m_interval = atoi(value);
  } 
  
  // By default the first poll to the stager is deliberately offset by a random
  // interval between 1 and 15 minutes. This randomised delay should prevent all
  // GC's in an instance/setup from deleting files at the same time causing an
  // oscillation in incoming network traffic due to deletions.
  int delay = 1;
  if ((value = getenv("GC_IMMEDIATESTART")) || 
      (value = getconfent("GC", "IMMEDIATESTART", 0))) {
    if (!strcasecmp(value, "yes") || !strcmp(value, "1")) {
      delay = 0;
    }
  }
  if (delay) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec * tv.tv_sec);
    delay = 60 + (int) (900.0 * rand() / (RAND_MAX + 60.0));
  }
  
  // "Garbage Collector started successfully" message
  castor::dlf::Param params[] =
    {castor::dlf::Param("Machine", m_diskServerName),
     castor::dlf::Param("Sleep Interval", m_interval),
     castor::dlf::Param("Startup Delay", delay)};
  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 5, 3, params);

  // Sleep a bit
  if (delay) {
    sleep(delay);
  }

  // Endless loop
  for (;;) {
    // "Checking for garbage" message
    castor::dlf::Param params[] =
      {castor::dlf::Param("Machine", m_diskServerName)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_DEBUG, 6, 1, params);
    
    // Get new sleep interval if the environment has been changed
    int intervalnew;
    if ((value = getenv("GC_INTERVAL")) || 
	(value = getconfent("GC", "INTERVAL", 0))) {
      intervalnew = atoi(value);
      if (intervalnew != m_interval) {
	m_interval = intervalnew;

	// "Sleep interval changed" message
	castor::dlf::Param params[] =
	  {castor::dlf::Param("Machine", m_diskServerName),
	   castor::dlf::Param("Sleep Interval", m_interval)};
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 7, 2, params);
      }
    }
    
    // Retrieve list of files to delete
    std::vector<castor::stager::GCLocalFile*>* files2Delete = 0;
    try {
      files2Delete = stgSvc->selectFiles2Delete(m_diskServerName);
    } catch (castor::exception::Exception e) {

      // "Error caught while looking for garbage files" message
      castor::dlf::Param params[] =
        {castor::dlf::Param("Message", e.getMessage().str())};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 8, 1, params);

      // "Sleeping" message
      castor::dlf::Param params2[] =
        {castor::dlf::Param("Duration", m_interval)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_DEBUG, 9, 1, params2);
      sleep(m_interval);
      continue;
    }
    
    // Delete files
    if (0 < files2Delete->size()) {
      
      // "Found files to garbage. Starting removal" message
      castor::dlf::Param params[] =
        {castor::dlf::Param("Nb files", files2Delete->size())};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 10, 1, params);
      
      // Loop over the files and delete them
      std::vector<u_signed64*> deletedFiles;
      std::vector<u_signed64*> failedFiles;
      u_signed64 removedsize  = 0;
      long filestotal   = 0;
      long filesfailed  = 0;
      long filesremoved = 0;
      for(std::vector<castor::stager::GCLocalFile*>::iterator it =
            files2Delete->begin();
          it != files2Delete->end();
          it++) {
	
	// Construct Cns invariant
	Cns_fileid fileId;
	memset(&fileId, '\0', sizeof(fileId));
	fileId.fileid = (*it)->fileId();
	strncpy(fileId.server, (*it)->nsHost().c_str(), sizeof(fileId.server));	

        try {
          filestotal++;
          u_signed64 filesize = gcRemoveFilePath((*it)->fileName());
          removedsize += filesize;
          filesremoved++;

          // "Removed file successfully" message
          castor::dlf::Param params[] =
            {castor::dlf::Param("File name", (*it)->fileName()),
             castor::dlf::Param("File size", filesize)};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 11, 2, params, &fileId);
          
	  // Add the file to the list of deleted ones
          u_signed64 *fileid = new u_signed64((*it)->diskCopyId());
          deletedFiles.push_back(fileid);
        } catch (castor::exception::Exception e) {
          filesfailed++;
          // "Failed to remove file" message
          castor::dlf::Param params[] =
            {castor::dlf::Param("File name", (*it)->fileName()),
             castor::dlf::Param("Error", e.getMessage().str()),
             castor::dlf::Param("Origin", strerror(e.code()))};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 12, 3, params, &fileId);
          
	  // Add the file to the list of failed ones
          u_signed64 *fileid = new u_signed64((*it)->diskCopyId());
          failedFiles.push_back(fileid);
        }
      } // End of delete files loop
      
      // log to DLF
      if (0 < filestotal) {

        // "Summary of files removed" message
        castor::dlf::Param params[] =
          {castor::dlf::Param("Files removed", filesremoved),
           castor::dlf::Param("Files failed", filesfailed),
           castor::dlf::Param("Files total", filestotal),
           castor::dlf::Param("Space freed", removedsize)};
        castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 13, 4, params);
      }
     
      // Inform stager of the deletion/failure
      if (deletedFiles.size() > 0) {
        try {
          stgSvc->filesDeleted(deletedFiles);
        } catch (castor::exception::Exception e) {
          // "Error caught while informing stager..." message
          castor::dlf::Param params[] =
            {castor::dlf::Param("Error", e.getMessage().str())};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 14, 1, params);
        }
        // release memory
        for (std::vector<u_signed64*>::iterator it =
               deletedFiles.begin();
             it != deletedFiles.end();
             it++) {
          delete *it;
        }
      }
      if (failedFiles.size() > 0) {
        try {
          stgSvc->filesDeletionFailed(failedFiles);
        } catch (castor::exception::Exception e) {
          // "Error caught while informing stager..." message
          castor::dlf::Param params[] =
            {castor::dlf::Param("Error", e.getMessage().str())};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 16, 1, params);
        }
        // release memory
        for (std::vector<u_signed64*>::iterator it =
               failedFiles.begin();
             it != failedFiles.end();
             it++) {
          delete *it;
        }
      }
    } else {
      // "No garbage files found" message
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_DEBUG, 15);
    }
    
    // "Sleeping" message
    castor::dlf::Param params2[] =
      {castor::dlf::Param("Duration", m_interval)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_DEBUG, 9, 1, params2);
    sleep(m_interval);
  } // End of loop
}


//-----------------------------------------------------------------------------
// GCremoveFilePath
//-----------------------------------------------------------------------------
u_signed64 castor::gc::DeletionThread::gcRemoveFilePath
(std::string filepath)
  throw (castor::exception::Exception) {
  u_signed64 filesize = gcGetFileSize(filepath);
  if (unlink(filepath.c_str()) < 0) {
    castor::exception::Exception e(errno);
    e.getMessage() << "gcRemoveFilePath : Cannot unlink file " << filepath;
    throw e;
  }
  return filesize;
}


//-----------------------------------------------------------------------------
// GCgetFileSize
//-----------------------------------------------------------------------------
u_signed64 castor::gc::DeletionThread::gcGetFileSize
(std::string& filepath)
  throw (castor::exception::Exception) {
  struct stat64  filedata;
  if (::stat64(&filepath[0], &filedata) ) {
    castor::exception::Exception e(errno);
    e.getMessage() << "gcGetFileSize : Cannot stat file " << filepath;
    throw e;
  }
  return filedata.st_size;
}

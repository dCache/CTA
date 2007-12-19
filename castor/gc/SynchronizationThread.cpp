/******************************************************************************
 *                      SynchronizationThread.cpp
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
 * @(#)$RCSfile: SynchronizationThread.cpp,v $ $Revision: 1.2 $ $Release$ $Date: 2007/12/19 10:07:26 $ $Author: waldron $
 *
 * Synchronization thread used to check periodically whether files need to be deleted
 *
 * @author castor dev team
 *****************************************************************************/

// Include files
#include "castor/gc/SynchronizationThread.hpp"
#include "castor/Services.hpp"
#include "castor/Constants.hpp"
#include "castor/stager/IGCSvc.hpp"
#include "castor/System.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "getconfent.h"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <map>

// Definitions
#define DEFAULT_GCINTERVAL 300
#define DEFAULT_SYNCINTERVAL 10
#define DEFAULT_CHUNKSIZE 1000


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
castor::gc::SynchronizationThread::SynchronizationThread() {};

//-----------------------------------------------------------------------------
// Run
//-----------------------------------------------------------------------------
void castor::gc::SynchronizationThread::run(void *param) {  

  // "Starting Garbage Collector Daemon" message
  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 18);
  
  // Get the synchronization interval and chunk size
  unsigned int syncInterval = DEFAULT_SYNCINTERVAL;
  unsigned int chunkSize = DEFAULT_CHUNKSIZE;
  readConfigFile(&syncInterval, &chunkSize, true);

  // Endless loop
  for (;;) {
    // get the synchronization interval and chunk size
    // these may have changed since last iteration
    readConfigFile(&syncInterval, &chunkSize);

    // Get the list of filesystem to be checked
    char** fs;
    int nbFs;
    if (getconfent_multi
        ("RmNode", "MountPoints", 1, &fs, &nbFs) < 0) {
      // "Unable to retrieve mountPoints, giving up with synchronization"
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 23);
      sleep(syncInterval);
      continue;
    }
    // initialize random number generator
    srand(time(0));
    // Loop over the fileSystems starting in a random place
    int fsIt = (int) (nbFs * (rand() / (RAND_MAX + 1.0)));
    for (int i = 0; i < nbFs; i++) {
      // list the filesystem directories in random order
      std::vector<std::string> directories;
      DIR *dirs = opendir(fs[fsIt]);
      if (0 == dirs) {
        // "Could not list filesystem directories"
	castor::dlf::Param params[] =
	  {castor::dlf::Param("FileSystem", fs[fsIt]),
           castor::dlf::Param("Error", strerror(errno))};
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 24, 2, params);
        continue;
      }
      struct dirent *dir;
      while ((dir = readdir(dirs))) {
        if (dir->d_type == DT_DIR) {
          int offset = (int) ((1 + directories.size()) *
                              (rand() / (RAND_MAX + 1.0)));
          directories.insert
	    (directories.begin() + offset,
	     (std::string(fs[fsIt])+dir->d_name).c_str());
        }
      }
      closedir(dirs);
      // Loop over the directories
      for (std::vector<std::string>::const_iterator it =
             directories.begin();
           it != directories.end();
           it++) {
        // Loop over files inside a directory
        DIR *files = opendir(it->c_str());
        if (0 == files) {
          // "Could not list filesystem subdirectory"
          castor::dlf::Param params[] =
            {castor::dlf::Param("FileSystem", fs[fsIt]),
             castor::dlf::Param("Directory", *it),
             castor::dlf::Param("Error", strerror(errno))};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 25, 3, params);
          continue;
        }
        // list files in this directory
        struct dirent *file;
        std::map<std::string, std::vector<u_signed64> > fileIds;
        std::map<std::string, std::map<u_signed64, std::string> > paths;
        while ((file = readdir(files))) {
          // ignore non regular files
          if (file->d_type != DT_REG) continue;
          try {
            std::pair<std::string, u_signed64> fid =
              fileIdFromFileName(file->d_name);
            fileIds[fid.first].push_back(fid.second);
            paths[fid.first][fid.second] = *it+"/"+file->d_name;
            // in case of large number of files, synchronize a first chunk
            if (fileIds[fid.first].size() >= chunkSize) {
              synchronizeFiles(fid.first, fileIds[fid.first], paths[fid.first]);
              fileIds[fid.first].clear();
              paths[fid.first].clear();
	      sleep(syncInterval);
            }
          } catch (castor::exception::Exception e) {
            // ignore files badly named, they are probably not ours
          }
        }
	std::map<std::string, std::map<u_signed64, std::string> >::const_iterator it3 =
	  paths.begin();
        // synchronize all files for all nameservers
        for (std::map<std::string, std::vector<u_signed64> >::const_iterator it2 =
               fileIds.begin();
             it2 != fileIds.end();
             it2++, it3++) {
          if (it2->second.size() > 0) {
            synchronizeFiles(it2->first, it2->second, it3->second);
	    sleep(syncInterval);
          }
        }
        closedir(files);
      }
      fsIt = (fsIt + 1) % nbFs;
    }
  }
}

//-----------------------------------------------------------------------------
// readConfigFile
//-----------------------------------------------------------------------------
void castor::gc::SynchronizationThread::readConfigFile
(unsigned int *syncInterval,
 unsigned int * chunkSize,
 bool firstTime) throw() {
  // synchronization interval
  char* value;
  int intervalnew;
  if ((value = getenv("SYNCINTERVAL")) || 
      (value = getconfent("GC", "SYNCINTERVAL", 0))) {
    intervalnew = atoi(value);
    if (intervalnew >= 0) {
      if ((unsigned int)intervalnew != *syncInterval) {
	*syncInterval = intervalnew;
	if (!firstTime) {
	  // "New synchronization interval"
	  castor::dlf::Param params[] =
	    {castor::dlf::Param("New synchronization interval", *syncInterval)};
	  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 21, 1, params);
	}
      }
    } else {
      // "Invalid value for synchronization interval. Used default"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Wrong value", value),
         castor::dlf::Param("Value used", *syncInterval)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 19, 2, params);
    }
  } 
  // chunk size
  int chunkSizenew;
  if ((value = getenv("CHUNKSIZE")) || 
      (value = getconfent("GC", "CHUNKSIZE", 0))) {
    chunkSizenew = atoi(value);
    if (chunkSizenew >= 0) {
      if (*chunkSize != (unsigned int)chunkSizenew) {
	*chunkSize = (unsigned int)chunkSizenew;
	if (!firstTime) {
	  // "New synchronization chunk size"
	  castor::dlf::Param params[] =
	    {castor::dlf::Param("New chunk size", *chunkSize)};
	  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 22, 1, params);
	}
      }
    } else {
      // "Invalid value for chunk size. Used default"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Wrong value", value),
         castor::dlf::Param("Value used", *chunkSize)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 20, 2, params);
    }
  } 
  // logging at start time
  if (firstTime) {
    // "Synchronization configuration"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Interval", *syncInterval),
       castor::dlf::Param("Chunk size", *chunkSize)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 30, 2, params);
  }
}

//-----------------------------------------------------------------------------
// fileIdFromFileName
//-----------------------------------------------------------------------------
std::pair<std::string, u_signed64>
castor::gc::SynchronizationThread::fileIdFromFileName
(char *fileName) throw (castor::exception::Exception) {
  // the filename should start with the numerical fileid
  u_signed64 fid = atoll(fileName);
  if (0 >= fid) {
    castor::exception::Internal e;
    e.getMessage() << "Unable to parse fileName : '"
                   << fileName << "'";
    throw e;
  }
  // then we should have @nshost.diskCopyId
  char* p = index(fileName, '@');
  char* q = index(fileName, '.');
  int len = q - p - 1;
  if (len == 0) {
    castor::exception::Internal e;
    e.getMessage() << "Unable to parse fileName : '"
                   << fileName << "'";
    throw e;
  }
  return std::pair<std::string, u_signed64>
    (std::string(p+1, len), fid);
}

//-----------------------------------------------------------------------------
// synchronizeFiles
//-----------------------------------------------------------------------------
void castor::gc::SynchronizationThread::synchronizeFiles
(std::string nameServer,
 const std::vector<u_signed64> &fileIds,
 const std::map<u_signed64, std::string> &paths) throw() {
  // First call the nameserver to get the list of files
  // that have been already deleted
  int nbFids = fileIds.size();
  u_signed64* cfids = (u_signed64*) malloc(nbFids * sizeof(u_signed64));
  if (0 == cfids) {
    castor::dlf::Param params[] =
      {castor::dlf::Param("function", "synchronizeFiles"),
       castor::dlf::Param("ErrorMessage", strerror(errno)) };
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 29, 2, params);
  }  
  for (int i = 0; i < nbFids; i++) {
    cfids[i] = fileIds[i];
  }
  Cns_bulkExist(nameServer.c_str(), cfids, &nbFids);
  // Stop here if we have no deleted files
  if (0 == nbFids) {
    free(cfids);
    return;
  }
  // Put the returned deleted files into a vector
  std::vector<u_signed64> delFileIds;
  for (int i = 0; i < nbFids; i++) {
    delFileIds.push_back(cfids[i]);
  }
  // release memory
  free(cfids);
  // Get RemoteGCSvc
  castor::IService* svc =
    castor::BaseObject::services()->
    service("RemoteGCSvc", castor::SVC_REMOTEGCSVC);
  castor::stager::IGCSvc* gcSvc =
    dynamic_cast<castor::stager::IGCSvc*>(svc);
  if (0 == gcSvc) {
    // "Could not get RemoteStagerSvc" message
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 2);
    return;
  }
  // call the stager for the files deleted from the nameserver
  // and get back the orphan files that we should locally delete
  std::vector<u_signed64> orphans =
    gcSvc->nsFilesDeleted(delFileIds, nameServer);

  // remove local files that are orphaned
  struct Cns_fileid fid;
  strncpy(fid.server, nameServer.c_str(), sizeof(fid.server));
  for (std::vector<u_signed64>::const_iterator it = orphans.begin();
       it != orphans.end();
       it++) {
    fid.fileid = *it;
    // "Deleting local file that was not in nameserver neither in stager catalog"
    castor::dlf::Param params[] =
      {castor::dlf::Param("fileName", paths.find(*it)->second)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 27, 1, params, &fid);
    if (unlink(paths.find(*it)->second.c_str()) < 0) {
      // "Deletion of orphan local file failed"
      castor::dlf::Param params[] =
	{castor::dlf::Param("fileName", paths.find(*it)->second),
	 castor::dlf::Param("Error", strerror(errno))};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 27, 2, params, &fid);
    }
  }
}

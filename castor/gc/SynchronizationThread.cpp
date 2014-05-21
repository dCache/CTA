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
 *
 * Synchronization thread used to check periodically whether files need to be
 * deleted
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
#include "getconfent.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

// Definitions
#define DEFAULT_CHUNKINTERVAL      1800
#define DEFAULT_CHUNKSIZE          2000
#define DEFAULT_DISABLESTAGERSYNC  false


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
castor::gc::SynchronizationThread::SynchronizationThread(int startDelay) :
  m_startDelay(startDelay), m_chunkInterval(DEFAULT_CHUNKINTERVAL),
  m_chunkSize(DEFAULT_CHUNKSIZE), m_disableStagerSync(DEFAULT_DISABLESTAGERSYNC)
{};

//-----------------------------------------------------------------------------
// syncFile
//-----------------------------------------------------------------------------
bool castor::gc::SynchronizationThread::syncFile
(const std::string &mountPoint,
 const std::string &path,
 const char* fileName,
 std::map<std::string, std::map<u_signed64, std::string> > &paths) {
  // Ignore non regular files and files closed too recently (< 1mn)
  // This protects in particular recently recalled files by giving time
  // to the stager DB to create the associated DiskCopy. Otherwise,
  // we would have a time window where the file exist on disk and can
  // be considered by us, while it does not exist on the stager. Thus
  // we would drop it
  struct stat64 filebuf;
  std::string filepath (path + "/" + fileName);
  if (stat64(filepath.c_str(), &filebuf) < 0) {
    return false;
  } else if (!(filebuf.st_mode & S_IFREG)) {
    return false;  // not a file
  } else if (filebuf.st_mtime > time(NULL) - 600) {
    return false;
  }
  
  // Extract the nameserver host and diskcopy id from the filename
  std::pair<std::string, u_signed64> fid;
  try {
    fid = diskCopyIdFromFileName(fileName);
  } catch (castor::exception::Exception& e) {
    // "Ignoring filename that does not conform to castor naming
    // conventions"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Filename", fileName)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_DEBUG, 39, 1, params);
    return false;
  }

  paths[fid.first][fid.second] = filepath;

  // In the case of a large number of files, synchronize them in
  // chunks so to not overwhelming central services
  return checkAndSyncChunk(fid.first, paths, mountPoint, m_chunkSize);
}

//-----------------------------------------------------------------------------
// Run
//-----------------------------------------------------------------------------
void castor::gc::SynchronizationThread::run(void*) {

  // "Starting synchronization thread"
  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 18);

  // sleep a bit if there is a startDelay
  sleep(m_startDelay);

  // Endless loop
  for (;;) {

    // Get the new configuration (eg. synch interval and chunk)
    // as these may have changed since the last iteration
    readConfigFile();
    if (m_chunkInterval <= 0) {
      // just do nothing if interval = 0
      sleep(300);
      return;
    }

    // Get the list of filesystem to be checked
    char** fs;
    int nbFs;
    if (getconfent_multi("DiskManager", "MountPoints", 1, &fs, &nbFs) < 0) {
      // "Unable to retrieve mountpoints, giving up with synchronization"
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 23);
      sleep(m_chunkInterval);
      continue;
    }

    // Initialize random number generator
    srand(time(0));

    // Loop over the fileSystems starting in a random place
    std::map<std::string, std::map<u_signed64, std::string> > paths;
    int fsIt = (int) (nbFs * (rand() / (RAND_MAX + 1.0)));
    for (int i = 0; i < nbFs; i++) {

      // List the filesystem directories in random order
      std::vector<std::string> directories;
      DIR *dirs = opendir(fs[fsIt]);
      if (0 == dirs) {
        // "Could not list filesystem directories"
        castor::dlf::Param params[] =
          {castor::dlf::Param("FileSystem", fs[fsIt]),
           castor::dlf::Param("Error", strerror(errno))};
        castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 24, 2, params);
        sleep(m_chunkInterval);
        continue;
      }
      struct dirent *dir;
      while ((dir = readdir(dirs))) {
        struct stat64 file;
        std::ostringstream filepath;
        filepath << fs[fsIt] << dir->d_name;
        if (stat64(filepath.str().c_str(), &file) < 0) {
          continue;
        } else if (!(file.st_mode & S_IFDIR)) {
          continue;  // not a directory
        } else if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
          continue;
        } else if (strspn(dir->d_name, "0123456789") != strlen(dir->d_name)
                   || (strlen(dir->d_name) != 2)) {
          continue;  // not a numbered directory name between 00 and 99
        }
        int offset = (int) ((1 + directories.size()) *
                            (rand() / (RAND_MAX + 1.0)));
        directories.insert
          (directories.begin() + offset, filepath.str().c_str());
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
        // List files in this directory
        struct dirent *file;
        while ((file = readdir(files))) {
          bool didSync = syncFile(fs[fsIt], *it, file->d_name, paths);
          if (didSync) {
            // after we've slept, we should flush the hidden cache inside the readdir
            // call. Otherwise, our next files will have as mtime the one before our
            // sleep, and this means that the check on the age is useless
            // As the buffer is hidden, there is no clean way to flush it, but a
            // repositioning of the dir stream to its current place does the trick
            seekdir(files, telldir(files));
          }
        }        
        closedir(files);
      }

      // Synchronize the remaining files not yet checked for this filesystem
      syncAllChunks(fs[fsIt], paths);

      // Go to next filesystem
      free(fs[fsIt]);
      fsIt = (fsIt + 1) % nbFs;

    }

    free(fs);
  }
}

//-----------------------------------------------------------------------------
// syncAllChunks
//-----------------------------------------------------------------------------
void castor::gc::SynchronizationThread::syncAllChunks
(const std::string &mountPoint,
 std::map<std::string, std::map<u_signed64, std::string> > &paths) {
  for (std::map<std::string, std::map<u_signed64, std::string> > ::const_iterator it =
         paths.begin();
       it != paths.end();
       it++) {
    checkAndSyncChunk(it->first, paths, mountPoint, 0);
  }
}

//-----------------------------------------------------------------------------
// checkAndSyncChunk
//-----------------------------------------------------------------------------
bool castor::gc::SynchronizationThread::checkAndSyncChunk
(const std::string &nameServer,
 std::map<std::string, std::map<u_signed64, std::string> > &paths,
 const std::string &mountPoint,
 u_signed64 minimumNbFiles) {
  try {
    if (paths[nameServer].size() >= minimumNbFiles) {
      // "Synchronizing files with nameserver and stager catalog"
      castor::dlf::Param params[] =
        {castor::dlf::Param("NbFiles", paths[nameServer].size()),
         castor::dlf::Param("Nameserver", nameServer)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_DEBUG, 31, 2, params);
      synchronizeFiles(nameServer, paths[nameServer], mountPoint);
      paths[nameServer].clear();
      sleep(m_chunkInterval);
      return true;
    }
  } catch (castor::exception::Exception& e) {
    // "Unexpected exception caught in synchronizeFiles"
    castor::dlf::Param params[] =
      {castor::dlf::Param("ErrorCode", e.code()),
       castor::dlf::Param("ErrorMessage", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 40, 2, params);
    sleep(m_chunkInterval);
  }
  return false;
}

//-----------------------------------------------------------------------------
// ReadConfigFile
//-----------------------------------------------------------------------------
void castor::gc::SynchronizationThread::readConfigFile(bool firstTime) {

  // Synchronization interval
  char* value;
  int intervalnew;

  // Chunk interval
  if ((value = getenv("GC_CHUNKINTERVAL")) ||
      (value = getconfent("GC", "ChunkInterval", 0))) {
    intervalnew = atoi(value);
    if (intervalnew >= 0) {
      if ((unsigned int)intervalnew != m_chunkInterval) {
        m_chunkInterval = intervalnew;
        if (!firstTime) {
          // "New chunk interval"
          castor::dlf::Param params[] =
            {castor::dlf::Param("Interval", m_chunkInterval)};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 37, 1, params);
        }
      }
    } else {
      m_chunkInterval = DEFAULT_CHUNKINTERVAL;
      // "Invalid GC/ChunkInterval option, using default"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Default", m_chunkInterval)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 38, 1, params);
    }
  }

  // Chunk size
  int chunkSizenew;
  if ((value = getenv("GC_CHUNKSIZE")) ||
      (value = getconfent("GC", "ChunkSize", 0))) {
    chunkSizenew = atoi(value);
    if (chunkSizenew >= 0) {
      if (m_chunkSize != (unsigned int)chunkSizenew) {
        m_chunkSize = (unsigned int)chunkSizenew;
        if (!firstTime) {
          // "New synchronization chunk size"
          castor::dlf::Param params[] =
            {castor::dlf::Param("ChunkSize", m_chunkSize)};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 22, 1, params);
        }
      }
    } else {
      m_chunkSize = DEFAULT_CHUNKSIZE;
      // "Invalid GC/ChunkSize option, using default"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Default", m_chunkSize)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 20, 1, params);
    }
  }

  // Disabling of stager synchronization
  if ((value = getenv("GC_DISABLESTAGERSYNC")) ||
      (value = getconfent("GC", "DisableStagerSync", 0))) {
    m_disableStagerSync = DEFAULT_DISABLESTAGERSYNC;
    if (!strcasecmp(value, "yes")) {
      m_disableStagerSync = true;
    } else if (strcasecmp(value, "no")) {
      castor::exception::Exception e(EINVAL);
      e.getMessage() << "Invalid option for DisableStagerSync: '" << value
                     << "' - must be 'yes' or 'no'" << std::endl;
      throw e;
    }
  }

  // Logging at start time
  if (firstTime) {
    // "Synchronization configuration"
    castor::dlf::Param params[] =
      {castor::dlf::Param("ChunkInterval", m_chunkInterval),
       castor::dlf::Param("ChunkSize", m_chunkSize)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 30, 2, params);
  }
}


//-----------------------------------------------------------------------------
// DiskCopyIdFromFileName
//-----------------------------------------------------------------------------
std::pair<std::string, u_signed64>
castor::gc::SynchronizationThread::diskCopyIdFromFileName(std::string fileName)
   {

  // Locate the beginning of the nameserver host in the filename, this gives
  // us the fileid
  std::string::size_type p = fileName.find('@', 0);
  if (p == std::string::npos) {
    castor::exception::Exception e;
    e.getMessage() << "Unable to parse filename : '" << fileName << "'";
    throw e;
  }

  // The fileid should only contain numbers and cannot be empty
  std::string fidStr = fileName.substr(0, p);
  if ((strspn(fidStr.c_str(), "0123456789") != fidStr.length()) ||
      (fidStr.length() <= 0)) {
    castor::exception::Exception e;
    e.getMessage() << "Unable to parse filename : '" << fileName << "'";
    throw e;
  }

  // Now extract the nameserver host, everything up to the last '.'
  std::string::size_type q = fileName.find_last_of('.', fileName.length());
  if (q == std::string::npos) {
    castor::exception::Exception e;
    e.getMessage() << "Unable to parse filename : '" << fileName << "'";
    throw e;
  }
  std::string nsHost = fileName.substr(p + 1, q - p -1);

  // Now the .diskcopyid, again only numbers and cannot be empty
  std::string dcIdStr = fileName.substr(q + 1, fileName.length());
  if ((strspn(dcIdStr.c_str(), "0123456789") != dcIdStr.length()) ||
      (dcIdStr.length() <= 0)) {
    castor::exception::Exception e;
    e.getMessage() << "Unable to parse filename : '" << fileName << "'";
    throw e;
  }
  u_signed64 dcId = atoll(dcIdStr.c_str());

  return std::pair<std::string, u_signed64>(nsHost, dcId);
}


//-----------------------------------------------------------------------------
// FileIdFromFilePath
//-----------------------------------------------------------------------------
u_signed64
castor::gc::SynchronizationThread::fileIdFromFilePath(std::string filePath)
   {

  // Extract the filename
  std::string::size_type f = filePath.find_last_of('/', filePath.length());
  if (f == std::string::npos) {
    castor::exception::Exception e;
    e.getMessage() << "Unable to parse filepath : '" << filePath << "'";
    throw e;
  }
  std::string fileName = filePath.substr(f + 1, filePath.length());

  // Locate the beginning of the nameserver host in the filename, this gives
  // us the fileid
  std::string::size_type p = fileName.find('@', 0);
  if (p == std::string::npos) {
    castor::exception::Exception e;
    e.getMessage() << "Unable to parse filename : '" << fileName << "'";
    throw e;
  }

  // The fileid should only contain numbers and cannot be empty
  std::string fidStr = fileName.substr(0, p);
  if ((strspn(fidStr.c_str(), "0123456789") != fidStr.length()) ||
      (fidStr.length() <= 0)) {
    castor::exception::Exception e;
    e.getMessage() << "Unable to parse filename : '" << fileName << "'";
    throw e;
  }

  return atoll(fidStr.c_str());
}


//-----------------------------------------------------------------------------
// getFilesBeingWrittenTo
//-----------------------------------------------------------------------------
std::set<std::string>
castor::gc::SynchronizationThread::getFilesBeingWrittenTo(const std::string &mountPoint)
   {
  std::set<std::string> files;
  // loop through the /proc/*/fd directories
  DIR *procDir = opendir("/proc");
  if (0 == procDir) {
    castor::exception::Exception e;
    e.getMessage() << "Unable to open /proc directory";
    throw e;
  }
  struct dirent *pidDir;
  while ((pidDir = readdir(procDir))) {
    // disregard non dirs and non numeric dirs (non pids)
    struct stat64 pidDirStat;
    std::ostringstream path;
    path << "/proc/" << pidDir->d_name;
    if ((stat64(path.str().c_str(), &pidDirStat) < 0) ||
        (!(pidDirStat.st_mode & S_IFDIR)) ||
        (strspn(pidDir->d_name, "0123456789") != strlen(pidDir->d_name))) {
      continue;
    }
    // list open files for this pid
    path << "/fd";
    DIR *fdDir = opendir(path.str().c_str());
    if (0 == fdDir) {
      // may have disappeared in the mean time
      continue;
    }
    struct dirent *file;
    while ((file = readdir(fdDir))) {
      // only keep links to castor filesystems that are writable
      struct stat64 fileStat;
      std::ostringstream filePath;
      filePath << path.str() << "/" << file->d_name;
      char buffer[1024];
      if ((lstat64(filePath.str().c_str(), &fileStat) == 0) &&
          S_ISLNK(fileStat.st_mode) &&
          ((fileStat.st_mode & S_IWUSR) == S_IWUSR) &&
          (readlink(filePath.str().c_str(), buffer, sizeof(buffer)) > 0)) {
        if (strncmp(buffer, mountPoint.c_str(), std::min(1024,int(mountPoint.size())))==0) {
          files.insert(std::string(buffer));
        }
      }    
    }
    closedir(fdDir);
  }
  closedir(procDir);
  return files;
}

//-----------------------------------------------------------------------------
// synchronizeFiles
//-----------------------------------------------------------------------------
void castor::gc::SynchronizationThread::synchronizeFiles
(const std::string &nameServer,
 const std::map<u_signed64, std::string> &paths,
 const std::string &mountPoint) throw() {

  // Make a copy of the disk copy id and file path containers so that they can
  // be modified safely
  std::vector<u_signed64> dcIds;
  std::map<u_signed64, std::string> filePaths;

  // Fill the copies ignoring files being written to
  // This in particular targets ongoing recalls as they do not have
  // an associated DiskCopy in the stager DB
  std::set<std::string> filesBeingWrittenTo = getFilesBeingWrittenTo(mountPoint);
  for (std::map<u_signed64, std::string>::const_iterator it = paths.begin();
       it != paths.end();
       it++) {
    if (filesBeingWrittenTo.find(it->second) == filesBeingWrittenTo.end()) {
      dcIds.push_back(it->first);
      filePaths[it->first] = it->second;
    }
  }

  // Get RemoteGCSvc
  castor::stager::IGCSvc *gcSvc = 0;
  try {
    castor::IService* svc =
      castor::BaseObject::services()->service("RemoteGCSvc", castor::SVC_REMOTEGCSVC);
    if (0 == svc) {
      // "Could not get RemoteStagerSvc"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Function", "SynchronizationThread::synchronizeFiles")};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 2, 1, params);
      return;
    }
    gcSvc = dynamic_cast<castor::stager::IGCSvc*>(svc);
    if (0 == gcSvc) {
      // "Got a bad RemoteStagerSvc"
      castor::dlf::Param params[] =
        {castor::dlf::Param("ID", svc->id()),
         castor::dlf::Param("Name", svc->name()),
         castor::dlf::Param("Function", "SynchronizationThread::synchronizeFiles")};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 3, 3, params);
      return;
    }
  } catch (castor::exception::Exception &e) {
    // "Could not get RemoteStagerSvc"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Function", "SynchronizationThread::synchronizeFiles"),
       castor::dlf::Param("ErrorCode", e.code()),
       castor::dlf::Param("ErrorMsg", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 2, 3, params);
    return;
  }

  Cns_fileid fileId;
  memset(&fileId, 0, sizeof(fileId));
  strncpy(fileId.server, nameServer.c_str(), sizeof(fileId.server));
  fileId.server[CA_MAXHOSTNAMELEN] = 0;
  u_signed64 nbOrphanFiles = 0;
  u_signed64 spaceFreed = 0;
  std::vector<u_signed64> orphans;

  // Synchronize the diskcopys with the stager catalog if needed.
  // The stager synchronization exists only to compensate from physical
  // file losses on the diskservers or bugs: as such, we deliberately
  // slow it down by a factor of 10 to not overwhelm the stager database.
  if (!m_disableStagerSync && (rand()/(RAND_MAX + 1.0) < 0.1)) {
    orphans = gcSvc->stgFilesDeleted(dcIds, nameServer);

    // Remove orphaned files
    for (std::vector<u_signed64>::const_iterator it = orphans.begin();
         it != orphans.end();
         it++) {
      try {
        fileId.fileid = fileIdFromFilePath(filePaths.find(*it)->second);
      } catch (castor::exception::Exception &e) {
        // "Could not get fileid from filepath, giving up for this file"
        castor::dlf::Param params[] =
          {castor::dlf::Param("Filename", filePaths.find(*it)->second),
           castor::dlf::Param("Error", e.code()),
           castor::dlf::Param("ErrorMessage", e.getMessage().str())};
        castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 43, 3, params, &fileId);        
        continue;
      }
      // Get information about the file before unlinking
      struct stat64 fileinfo;
      if (stat64(filePaths.find(*it)->second.c_str(), &fileinfo) < 0) {
        if (errno != ENOENT) {
          // "Failed to stat file"
          castor::dlf::Param params[] =
            {castor::dlf::Param("Filename", filePaths.find(*it)->second),
             castor::dlf::Param("Error", strerror(errno))};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 41, 2, params, &fileId);
        }
      }

      if (unlink(filePaths.find(*it)->second.c_str()) < 0) {
        if (errno != ENOENT) {
          // "Deletion of orphaned local file failed"
          castor::dlf::Param params[] =
            {castor::dlf::Param("Filename", filePaths.find(*it)->second),
             castor::dlf::Param("Error", strerror(errno))};
          castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 28, 2, params, &fileId);
        }
      } else {

        // "Deleting local file which is no longer in the stager catalog"
        castor::dlf::Param params[] =
          {castor::dlf::Param("Filename", filePaths.find(*it)->second),
           castor::dlf::Param("FileSize", (u_signed64)fileinfo.st_size),
           castor::dlf::Param("FileAge", time(NULL) - fileinfo.st_ctime)};
        castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 36, 3, params, &fileId);
        spaceFreed += fileinfo.st_size;
        nbOrphanFiles++;
      }

      // Remove the file from the file paths map
      filePaths.erase(filePaths.find(*it));
    }

    // "Summary of files removed by stager synchronization"
    if (nbOrphanFiles) {
      castor::dlf::Param params[] =
        {castor::dlf::Param("NbOrphanFiles", nbOrphanFiles),
         castor::dlf::Param("SpaceFreed", spaceFreed)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 33, 2, params);
    }

    // During the unlinking of the files in the diskserver to stager catalog
    // synchronization we removed the orphaned files from the file path map. As
    // a result the file path map now only contains those files which need to
    // be checked against the nameserver
    if (filePaths.size() == 0) {
      return;
    }

  }

  // Populate the vector and the map with fileids and their corresponding physical paths
  std::map<u_signed64, std::string> cnsFilePaths;
  std::vector<u_signed64> fileIds;
  for (std::map<u_signed64, std::string>::iterator it = filePaths.begin();
       it != filePaths.end();
       it++) {
    try {
      u_signed64 fid = fileIdFromFilePath(it->second);
      cnsFilePaths[fid] = it->second;
      fileIds.push_back(fid);
    } catch (castor::exception::Exception &e) {
      // "Could not get fileid from filepath, giving up for this file"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Filename", it->second),
         castor::dlf::Param("Error", e.code()),
         castor::dlf::Param("ErrorMessage", e.getMessage().str())};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 43, 3, params, &fileId);
    }
  }

  // Notify the stager to the deletion of the orphaned files
  orphans = gcSvc->nsFilesDeleted(fileIds, nameServer);

  // Remove orphaned files
  spaceFreed = 0, nbOrphanFiles = 0;
  for (std::vector<u_signed64>::const_iterator it = orphans.begin();
       it != orphans.end();
       it++) {
    fileId.fileid = *it;

    // Get information about the file before unlinking
    struct stat64 fileinfo;
    if (stat64(cnsFilePaths.find(*it)->second.c_str(), &fileinfo) < 0) {
      if (errno != ENOENT) {
        // "Failed to stat file"
        castor::dlf::Param params[] =
          {castor::dlf::Param("Filename", cnsFilePaths.find(*it)->second),
           castor::dlf::Param("Error", strerror(errno))};
        castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 41, 2, params, &fileId);
      }
    }

    if (unlink(cnsFilePaths.find(*it)->second.c_str()) < 0) {
      if (errno != ENOENT) {
        // "Deletion of orphaned local file failed"
        castor::dlf::Param params[] =
          {castor::dlf::Param("Filename", cnsFilePaths.find(*it)->second),
           castor::dlf::Param("Error", strerror(errno))};
        castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 28, 2, params, &fileId);
      }
    } else {
      // "Deleting local file which is no longer in the nameserver"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Filename", cnsFilePaths.find(*it)->second),
         castor::dlf::Param("FileSize", (u_signed64)fileinfo.st_size),
         castor::dlf::Param("FileAge", time(NULL) - fileinfo.st_ctime)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 27, 3, params, &fileId);
      spaceFreed += fileinfo.st_size;
      nbOrphanFiles++;
    }
  }

  // "Summary of files removed by nameserver synchronization"
  if (nbOrphanFiles) {
    castor::dlf::Param params[] =
      {castor::dlf::Param("NbOrphanFiles", nbOrphanFiles),
       castor::dlf::Param("SpaceFreed", spaceFreed)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 35, 2, params);
  }

}

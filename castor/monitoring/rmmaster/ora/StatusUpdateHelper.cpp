/******************************************************************************
 *                      StatusUpdateHelper.cpp
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2004  CERN
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
 * @(#)$RCSfile: StatusUpdateHelper.cpp,v $ $Author: waldron $
 *
 * Status update helper class. Shared between the OraRmMasterSvc and the
 * Collector thread of the RmMasterDaemon.
 *
 * @author castor-dev team
 *****************************************************************************/

#include "castor/monitoring/rmmaster/ora/StatusUpdateHelper.hpp"
#include "castor/stager/DiskServer.hpp"
#include "castor/stager/FileSystem.hpp"
#include "castor/monitoring/ClusterStatus.hpp"
#include "castor/monitoring/DiskServerStatus.hpp"
#include "castor/monitoring/FileSystemStatus.hpp"
#include "castor/monitoring/DiskServerMetricsReport.hpp"
#include "castor/monitoring/FileSystemMetricsReport.hpp"
#include "castor/monitoring/DiskServerStateReport.hpp"
#include "castor/monitoring/FileSystemStateReport.hpp"
#include "castor/monitoring/admin/DiskServerAdminReport.hpp"
#include "castor/monitoring/admin/FileSystemAdminReport.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/Constants.hpp"
#include <time.h>
#include <errno.h>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::monitoring::rmmaster::ora::StatusUpdateHelper::StatusUpdateHelper
(castor::monitoring::ClusterStatus* clusterStatus) :
  m_clusterStatus(clusterStatus) {
}

//------------------------------------------------------------------------------
// handleStateUpdate
//------------------------------------------------------------------------------
void castor::monitoring::rmmaster::ora::StatusUpdateHelper::handleStateUpdate
(castor::monitoring::DiskServerStateReport* state)
  throw (castor::exception::Exception) {
  // Throw away reports with no name cause the build of
  // a shared memory string fails for empty strings. This
  // is due to an optimization inside the string implementation
  // that tries to delay memory allocation when not needed
  // (typically for empty strings). This could be avoided
  // by recompiling libstdc++ with -D_GLIBCXX_FULLY_DYNAMIC_STRING
  // but the default version distributed does not have this
  // The consequence of accepting this is a seg fault in any
  // process attempting to read it (other than the one that
  // created the empty string).
  if (state->name().size() == 0) {
    // "Ignored state report for machine with empty name"
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 21);
    return;
  }
  // Take care of DiskServer creation
  castor::monitoring::ClusterStatus::iterator it;
  if (!getOrCreateDiskServer(state->name(), it)) {
    return;
  }
  // update DiskServer status
  it->second.setRam(state->ram());
  it->second.setMemory(state->memory());
  it->second.setSwap(state->swap());
  // Announce a diskservers change of status ? (e.g. being re-enabled after
  // heartbeat check failure)
  if ((it->second.adminStatus() == ADMIN_NONE) && 
      (it->second.status() == castor::stager::DISKSERVER_DISABLED) &&
      (state->status() == castor::stager::DISKSERVER_PRODUCTION)) {
    castor::dlf::Param params[] =
      {castor::dlf::Param("Hostname", state->name())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 39, 1, params);
  }
  // Update status if needed
  if (it->second.adminStatus() == ADMIN_NONE ||
      state->adminStatus() != ADMIN_NONE) {
    it->second.setStatus(state->status());
    if (state->adminStatus() == ADMIN_FORCE) {
      it->second.setAdminStatus(ADMIN_FORCE);
    } else {
      it->second.setAdminStatus(ADMIN_NONE);
    }
  }
  // Update lastUpdate
  it->second.setLastStateUpdate(time(0));
  // Update FileSystems
  for (std::vector<castor::monitoring::FileSystemStateReport*>::const_iterator
         itFs = state->FileSystemStatesReports().begin();
       itFs != state->FileSystemStatesReports().end();
       itFs++) {
    // Throw away reports from filesystems with no name cause the build of
    // a shared memory string fails for empty strings. This
    // is due to an optimization inside the string implementation
    // that tries to delay memory allocation when not needed
    // (typically for empty strings). This could be avoided
    // by recompiling libstdc++ with -D_GLIBCXX_FULLY_DYNAMIC_STRING
    // but the default version distributed does not have this
    // The consequence of accepting this is a seg fault in any
    // process attempting to read it (other than the one that
    // created the empty string).
    if ((*itFs)->mountPoint().size() == 0) {
      // "Ignored state report for filesystem with empty name"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Machine", state->name())};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 27, 1, params);
      continue;
    }
    // Take care of the FileSystem creation
    castor::monitoring::DiskServerStatus::iterator it2;
    if (!getOrCreateFileSystem(it->second,
			       (*itFs)->mountPoint(),
			       it2)) {
      return;
    }
    // Update FileSystem status
    it2->second.setSpace((*itFs)->space());
    it2->second.setMinFreeSpace((*itFs)->minFreeSpace());
    it2->second.setMaxFreeSpace((*itFs)->maxFreeSpace());
    it2->second.setMinAllowedFreeSpace((*itFs)->minAllowedFreeSpace());
    // Update status if needed
    if (it2->second.adminStatus() == ADMIN_NONE ||
        (*itFs)->adminStatus() != ADMIN_NONE) {
      it2->second.setStatus((*itFs)->status());
      if ((*itFs)->adminStatus() == ADMIN_FORCE) {
        it2->second.setAdminStatus(ADMIN_FORCE);
      } else {
        it2->second.setAdminStatus(ADMIN_NONE);
      }
    }
    // Update lastUpdate
    it2->second.setLastStateUpdate(time(0));
  }
}

//------------------------------------------------------------------------------
// getOrCreateDiskServer
//------------------------------------------------------------------------------
bool
castor::monitoring::rmmaster::ora::StatusUpdateHelper::getOrCreateDiskServer
(std::string name,
 castor::monitoring::ClusterStatus::iterator& it) throw() {
  // cast normal string into sharedMemory one in order to be able
  // to search for it in the ClusterStatus map.
  // This is safe because the difference in the types is only
  // the allocator and because the 2 allocators have identical
  // members
  castor::monitoring::SharedMemoryString *smName =
    (castor::monitoring::SharedMemoryString*)(&name);
  it = m_clusterStatus->find(*smName);
  if (it == m_clusterStatus->end()) {
    try {
      // here we really have to create a shared memory string,
      // the previous cast won't work since we need to allocate memory
      const castor::monitoring::SharedMemoryString smName2(name.c_str());
      it = m_clusterStatus->insert
	(std::make_pair(smName2,
			castor::monitoring::DiskServerStatus())).first;
    } catch (std::exception e) {
      // "Unable to allocate SharedMemoryString"
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 28);
      // not enough shared memory... let's give up
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// getOrCreateFileSystem
//------------------------------------------------------------------------------
bool
castor::monitoring::rmmaster::ora::StatusUpdateHelper::getOrCreateFileSystem
(castor::monitoring::DiskServerStatus& dss,
 std::string mountPoint,
 castor::monitoring::DiskServerStatus::iterator& it2) throw() {
  // cast normal string into sharedMemory one in order to be able
  // to search for it in the DiskServerStatus map.
  // This is safe because the difference in the types is only
  // the allocator and because the 2 allocators have identical
  // members
  castor::monitoring::SharedMemoryString *smMountPoint =
    (castor::monitoring::SharedMemoryString*)(&mountPoint);
  it2 = dss.find(*smMountPoint);
  if (it2 == dss.end()) {
    try {
      // here we really have to create a shared memory string,
      // the previous cast won't work
      const castor::monitoring::SharedMemoryString smMountPoint2
	(mountPoint.c_str());
      it2 = dss.insert(std::make_pair
		       (smMountPoint2,
			castor::monitoring::FileSystemStatus())).first;
    } catch (std::exception e) {
      // "Unable to allocate SharedMemoryString"
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 28);
      // not enough shared memory... let's give up
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// handleMetricsUpdate
//------------------------------------------------------------------------------
void castor::monitoring::rmmaster::ora::StatusUpdateHelper::handleMetricsUpdate
(castor::monitoring::DiskServerMetricsReport* metrics)
  throw (castor::exception::Exception) {
  // Throw away reports with no name cause the build of
  // a shared memory string fails for empty strings. This
  // is due to an optimization inside the string implementation
  // that tries to delay memory allocation when not needed
  // (typically for empty strings). This could be avoided
  // by recompiling libstdc++ with -D_GLIBCXX_FULLY_DYNAMIC_STRING
  // but the default version distributed does not have this
  // The consequence of accepting this is a seg fault in any
  // process attempting to read it (other than the one that
  // created the empty string).
  if (metrics->name().size() == 0) {
    // "Ignored metrics report for machine with empty name"
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 22);
    return;
  }
  // Take care of DiskServer creation
  castor::monitoring::ClusterStatus::iterator it;
  if (!getOrCreateDiskServer(metrics->name(), it)) {
    return;
  }
  // gather totals
  u_signed64 totalReadRate = 0;
  u_signed64 totalWriteRate = 0;
  unsigned int totalNbReadStreams = 0;
  unsigned int totalNbWriteStreams = 0;
  unsigned int totalNbReadWriteStreams = 0;
  unsigned int totalNbMigratorStreams = 0;
  unsigned int totalNbRecallerStreams = 0;
  // Update FileSystems
  for (std::vector<castor::monitoring::FileSystemMetricsReport*>::const_iterator
         itFs = metrics->fileSystemMetricsReports().begin();
       itFs != metrics->fileSystemMetricsReports().end();
       itFs++) {
    // Throw away reports from filesystems with no name cause the build of
    // a shared memory string fails for empty strings. This
    // is due to an optimization inside the string implementation
    // that tries to delay memory allocation when not needed
    // (typically for empty strings). This could be avoided
    // by recompiling libstdc++ with -D_GLIBCXX_FULLY_DYNAMIC_STRING
    // but the default version distributed does not have this
    // The consequence of accepting this is a seg fault in any
    // process attempting to read it (other than the one that
    // created the empty string).
    if ((*itFs)->mountPoint().size() == 0) {
      // "Ignored metrics report for filesystem with empty name"
      castor::dlf::Param params[] =
        {castor::dlf::Param("Machine", metrics->name())};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 26, 1, params);
      continue;
    }
    // Take care of the FileSystem creation
    castor::monitoring::DiskServerStatus::iterator it2;
    if (!getOrCreateFileSystem(it->second,
			       (*itFs)->mountPoint(),
			       it2)) {
      return;
    }
    // Update FileSystem metrics
    it2->second.setReadRate((*itFs)->readRate());
    it2->second.setDeltaReadRate(0);
    totalReadRate += it2->second.readRate();
    it2->second.setWriteRate((*itFs)->writeRate());
    it2->second.setDeltaWriteRate(0);
    totalWriteRate += it2->second.writeRate();
    it2->second.setNbReadStreams((*itFs)->nbReadStreams());
    it2->second.setDeltaNbReadStreams(0);
    totalNbReadStreams += it2->second.nbReadStreams();
    it2->second.setNbWriteStreams((*itFs)->nbWriteStreams());
    it2->second.setDeltaNbWriteStreams(0);
    totalNbWriteStreams += it2->second.nbWriteStreams();
    it2->second.setNbReadWriteStreams((*itFs)->nbReadWriteStreams());
    it2->second.setDeltaNbReadWriteStreams(0);
    totalNbReadWriteStreams += it2->second.nbReadWriteStreams();
    it2->second.setNbMigratorStreams((*itFs)->nbMigratorStreams());
    it2->second.setDeltaNbMigratorStreams(0);
    totalNbMigratorStreams += it2->second.nbMigratorStreams();
    it2->second.setNbRecallerStreams((*itFs)->nbRecallerStreams());
    it2->second.setDeltaNbRecallerStreams(0);
    totalNbRecallerStreams += it2->second.nbRecallerStreams();
    it2->second.setFreeSpace((u_signed64)((signed64)(*itFs)->freeSpace()));
    it2->second.setDeltaFreeSpace(0);
    // Update lastUpdate
    it2->second.setLastMetricsUpdate(time(0));
  }
  // update DiskServer metrics
  it->second.setFreeRam(metrics->freeRam());
  it->second.setFreeMemory(metrics->freeMemory());
  it->second.setFreeSwap(metrics->freeSwap());
  it->second.setReadRate(totalReadRate);
  it->second.setDeltaReadRate(0);
  it->second.setWriteRate(totalWriteRate);
  it->second.setDeltaWriteRate(0);
  it->second.setNbReadStreams(totalNbReadStreams);
  it->second.setDeltaNbReadStreams(0);
  it->second.setNbWriteStreams(totalNbWriteStreams);
  it->second.setDeltaNbWriteStreams(0);
  it->second.setNbReadWriteStreams(totalNbReadWriteStreams);
  it->second.setDeltaNbReadWriteStreams(0);
  it->second.setNbMigratorStreams(totalNbMigratorStreams);
  it->second.setDeltaNbMigratorStreams(0);
  it->second.setNbRecallerStreams(totalNbRecallerStreams);
  it->second.setDeltaNbRecallerStreams(0);
  // Update lastUpdate
  it->second.setLastMetricsUpdate(time(0));
}

//------------------------------------------------------------------------------
// handleDiskServerAdminUpdate
//------------------------------------------------------------------------------
void castor::monitoring::rmmaster::ora::StatusUpdateHelper::handleDiskServerAdminUpdate
(castor::monitoring::admin::DiskServerAdminReport* admin)
  throw (castor::exception::Exception) {
  // Throw away reports with no name cause the build of
  // a shared memory string fails for empty strings. This
  // is due to an optimization inside the string implementation
  // that tries to delay memory allocation when not needed
  // (typically for empty strings). This could be avoided
  // by recompiling libstdc++ with -D_GLIBCXX_FULLY_DYNAMIC_STRING
  // but the default version distributed does not have this
  // The consequence of accepting this is a seg fault in any
  // process attempting to read it (other than the one that
  // created the empty string).
  if (admin->diskServerName().size() == 0) {
    // "Ignored admin diskserver report for machine with empty name"
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 23);
    return;
  }
  // cast normal string into sharedMemory one in order to be able
  // to search for it in the ClusterStatus map.
  // This is safe because the difference in the types is only
  // the allocator and because the 2 allocators have identical
  // members
  std::string machineName = admin->diskServerName();
  castor::monitoring::SharedMemoryString *smMachineName =
    (castor::monitoring::SharedMemoryString*)(&machineName);
  // Take care of DiskServer creation
  castor::monitoring::ClusterStatus::iterator it =
    m_clusterStatus->find(*smMachineName);
  if (it == m_clusterStatus->end()) {
    // "Ignored admin diskServer report for unknown machine"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Machine name", admin->diskServerName())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 29, 1, params);
    // inform user via the exception mechanism
    castor::exception::NoEntry e;
    e.getMessage() << "Unknown machine '"
		   << admin->diskServerName()
		   << "'. Please check the name and provide the domain.";
    throw e;
  }
  // Update status if needed
  if (it->second.adminStatus() == ADMIN_NONE ||
      admin->adminStatus() != ADMIN_NONE) {
    if (admin->adminStatus() == ADMIN_DELETED) {
      it->second.setStatus(castor::stager::DISKSERVER_DISABLED);
      it->second.setAdminStatus(ADMIN_DELETED);
    } else {
      it->second.setStatus(admin->status());
      if (admin->adminStatus() == ADMIN_FORCE) {
	it->second.setAdminStatus(ADMIN_FORCE);
      } else {
	it->second.setAdminStatus(ADMIN_NONE);
      }
    }
  }
  // Go over the fileSystems if required
  if (admin->recursive()) {
    for (castor::monitoring::DiskServerStatus::iterator it2 =
	   it->second.begin();
	 it2 != it->second.end();
	 it2++) {
      // Update status if needed
      if (it2->second.adminStatus() == ADMIN_NONE ||
	  admin->adminStatus() != ADMIN_NONE) {
	if (admin->adminStatus() == ADMIN_DELETED) {
	  it2->second.setStatus(castor::stager::FILESYSTEM_DISABLED);
	  it2->second.setAdminStatus(ADMIN_DELETED);
	} else {
	  switch (admin->status()) {
	  case castor::stager::DISKSERVER_PRODUCTION :
	    it2->second.setStatus(castor::stager::FILESYSTEM_PRODUCTION);
	    break;
	  case castor::stager::DISKSERVER_DRAINING :
	    it2->second.setStatus(castor::stager::FILESYSTEM_DRAINING);
	    break;
	  case castor::stager::DISKSERVER_DISABLED :
	    it2->second.setStatus(castor::stager::FILESYSTEM_DISABLED);
	  }
	  if (admin->adminStatus() == ADMIN_FORCE) {
	    it2->second.setAdminStatus(ADMIN_FORCE);
	  } else {
	    it2->second.setAdminStatus(ADMIN_NONE);
	  }
	}
      }
    }
  }
}

//------------------------------------------------------------------------------
// handleFileSystemAdminUpdate
//------------------------------------------------------------------------------
void castor::monitoring::rmmaster::ora::StatusUpdateHelper::handleFileSystemAdminUpdate
(castor::monitoring::admin::FileSystemAdminReport* admin)
  throw (castor::exception::Exception) {
  // Throw away reports with no name cause the build of
  // a shared memory string fails for empty strings. This
  // is due to an optimization inside the string implementation
  // that tries to delay memory allocation when not needed
  // (typically for empty strings). This could be avoided
  // by recompiling libstdc++ with -D_GLIBCXX_FULLY_DYNAMIC_STRING
  // but the default version distributed does not have this
  // The consequence of accepting this is a seg fault in any
  // process attempting to read it (other than the one that
  // created the empty string).
  if (admin->diskServerName().size() == 0) {
    // "Ignored admin filesystem report for machine with empty name"
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 24);
    return;
  }
  if (admin->mountPoint().size() == 0) {
    // "Ignored admin filesystem report for filesystem with empty name"
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 25);
    return;
  }
  // cast normal string into sharedMemory one in order to be able
  // to search for it in the ClusterStatus map.
  // This is safe because the difference in the types is only
  // the allocator and because the 2 allocators have identical
  // members
  std::string machineName = admin->diskServerName();
  castor::monitoring::SharedMemoryString *smMachineName =
    (castor::monitoring::SharedMemoryString*)(&machineName);
  // Find out DiskServer
  castor::monitoring::ClusterStatus::iterator it =
    m_clusterStatus->find(*smMachineName);
  if (it == m_clusterStatus->end()) {
    // "Ignored admin fileSystem report for unknown machine"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Machine name", admin->diskServerName())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 30, 1, params);
    // inform user via the exception mechanism
    castor::exception::NoEntry e;
    e.getMessage() << "Unknown machine '"
		   << admin->diskServerName()
		   << "'. Please check the name and provide the domain.";
    throw e;
  }
  // cast normal string into sharedMemory one in order to be able
  // to search for it in the DiskServerStatus map.
  // This is safe because the difference in the types is only
  // the allocator and because the 2 allocators have identical
  // members
  std::string mountPoint = admin->mountPoint();
  castor::monitoring::SharedMemoryString *smMountPoint =
    (castor::monitoring::SharedMemoryString*)(&mountPoint);
  // Find out FileSystem
  castor::monitoring::DiskServerStatus::iterator it2 =
    it->second.find(*smMountPoint);
  if (it2 == it->second.end()) {
    // "Ignored admin fileSystem report for unknown mountPoint"
    castor::dlf::Param params[] =
      {castor::dlf::Param("Machine name", admin->diskServerName()),
       castor::dlf::Param("MountPoint", admin->mountPoint())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 31, 2, params);
    // inform user via the exception mechanism
    castor::exception::NoEntry e;
    e.getMessage() << "Unknown mountPoint '"
		   << admin->mountPoint()
		   << "' on machine '"
		   << admin->diskServerName()
		   << "'.";
    throw e;
  }
  // Update status if needed
  if (it2->second.adminStatus() == ADMIN_NONE ||
      admin->adminStatus() != ADMIN_NONE) {
    if (admin->adminStatus() == ADMIN_DELETED) {
      it2->second.setStatus(castor::stager::FILESYSTEM_DISABLED);
      it2->second.setAdminStatus(ADMIN_DELETED);
    } else {
      it2->second.setStatus(admin->status());
      if (admin->adminStatus() == ADMIN_FORCE) {
	it2->second.setAdminStatus(ADMIN_FORCE);
      } else {
	it2->second.setAdminStatus(ADMIN_NONE);
      }
    }
  }
}

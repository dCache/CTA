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

#include "DriveRegister.hpp"
#include "ProtocolBuffersAlgorithms.hpp"
#include "GenericObject.hpp"
#include "RootEntry.hpp"
#include <set>

cta::objectstore::DriveRegister::DriveRegister(const std::string & address, Backend & os):
  ObjectOps<serializers::DriveRegister>(os, address) { }

cta::objectstore::DriveRegister::DriveRegister(GenericObject& go):
  ObjectOps<serializers::DriveRegister>(go.objectStore()) {
  // Here we transplant the generic object into the new object
  go.transplantHeader(*this);
  // And interpret the header.
  getPayloadFromHeader();
}

void cta::objectstore::DriveRegister::initialize() {
  // Setup underlying object
  ObjectOps<serializers::DriveRegister>::initialize();
  m_payloadInterpreted = true;
}

void cta::objectstore::DriveRegister::garbageCollect(const std::string &presumedOwner) {
  checkPayloadWritable();
  // If the agent is not anymore the owner of the object, then only the very
  // last operation of the drive register creation failed. We have nothing to do.
  if (presumedOwner != m_header.owner())
    return;
  // If the owner is still the agent, we have 2 possibilities:
  // 1) The register is referenced by the root entry. We just need to officialise
  // the ownership on the register (if not, we will either get the NotAllocated 
  // exception) or the address will be different.
  {
    RootEntry re(m_objectStore);
    ScopedSharedLock rel (re);
    re.fetch();
    try {
      if (re.getAgentRegisterAddress() == getAddressIfSet()) {
        setOwner(re.getAddressIfSet());
        commit();
        return;
      }
    } catch (RootEntry::NotAllocated &) {}
  }
  // 2) The tape pool is not referenced in the root entry. We can just clean it up.
  if (!isEmpty()) {
    throw (NotEmpty("Trying to garbage collect a non-empty AgentRegister: internal error"));
  }
  remove();
}

// This operator will be used in the following usage of the templated
// findElement and removeOccurences
namespace {
  bool operator==(const std::string & driveName, 
    const cta::objectstore::serializers::DriveState & ds) {
    return ds.drivename() == driveName;
  }
}

void cta::objectstore::DriveRegister::addDrive(const std::string & driveName,
    const std::string & logicalLibrary, const CreationLog & creationLog) { 
  //add logical library to the parameters
  checkPayloadWritable();
  // Check that we are not trying to duplicate a drive
  try {
    serializers::findElement(m_payload.drives(), driveName);
    throw DuplicateEntry("In DriveRegister::addDrive: entry already exists");
  } catch (serializers::NotFound &) {}
  serializers::DriveState * driveState = m_payload.add_drives();
  driveState->set_drivename(driveName);
  driveState->set_logicallibrary(logicalLibrary);
  driveState->set_lastupdatetime(creationLog.time);
  driveState->set_mounttype(serializers::MountType::NoMount);
  creationLog.serialize(*driveState->mutable_creationlog());
}

cta::objectstore::serializers::DriveStatus 
  cta::objectstore::DriveRegister::serializeDriveStatus(DriveStatus driveStatus) {
  switch (driveStatus) {
    case DriveStatus::Down:
      return serializers::DriveStatus::Down;
    case DriveStatus::Up:
      return serializers::DriveStatus::Up;
    case DriveStatus::Starting:
      return serializers::DriveStatus::Starting;
    case DriveStatus::Mounting:
      return serializers::DriveStatus::Mounting;
    case DriveStatus::Transfering:
      return serializers::DriveStatus::Transfering;
    case DriveStatus::Unloading:
      return serializers::DriveStatus::Unloading;
    case DriveStatus::Unmounting:
      return serializers::DriveStatus::Unmounting;
    case DriveStatus::DrainingToDisk:
      return serializers::DriveStatus::DrainingToDisk;
    case DriveStatus::CleaningUp:
      return serializers::DriveStatus::CleaningUp;
    default:
      throw exception::Exception("In DriveRegister::serializeDriveState: unexpected DriveStatus");
  }
}

cta::objectstore::DriveRegister::DriveStatus
  cta::objectstore::DriveRegister::deserializeDriveStatus(
    serializers::DriveStatus driveStatus) {
  switch (driveStatus) {
    case serializers::DriveStatus::Down:
      return DriveStatus::Down;
    case serializers::DriveStatus::Up:
      return DriveStatus::Up;
    case serializers::DriveStatus::Starting:
      return DriveStatus::Starting;
    case serializers::DriveStatus::Mounting:
      return DriveStatus::Mounting;
    case serializers::DriveStatus::Transfering:
      return DriveStatus::Transfering;
    case serializers::DriveStatus::Unloading:
      return DriveStatus::Unloading;
    case serializers::DriveStatus::Unmounting:
      return DriveStatus::Unmounting;
    case serializers::DriveStatus::DrainingToDisk:
      return DriveStatus::DrainingToDisk;
    case serializers::DriveStatus::CleaningUp:
      return DriveStatus::CleaningUp;
    default:
      throw exception::Exception("In DriveRegister::deserializeDriveStatus: unexpected DriveStatus");
  }
}

cta::objectstore::serializers::MountType
  cta::objectstore::DriveRegister::serializeMountType(MountType mountType) {
  switch (mountType) {
    case MountType::NoMount:
      return serializers::MountType::NoMount;
    case MountType::Archive:
      return serializers::MountType::Archive;
    case MountType::Retrieve:
      return serializers::MountType::Retrieve;
    default:
      throw exception::Exception("In DriveRegister::serializeMountType: unexpected MountType");
  }
}

cta::objectstore::DriveRegister::MountType
  cta::objectstore::DriveRegister::deserializeMountType(serializers::MountType mountType) {
  switch (mountType) {
    case serializers::MountType::NoMount:
      return MountType::NoMount;
    case serializers::MountType::Archive:
      return MountType::Archive;
    case serializers::MountType::Retrieve:
      return MountType::Retrieve;
    default:
      throw exception::Exception("In DriveRegister::serializeMountType: unexpected MountType");
  }
}

namespace {
  template <class C>
  void mergeSecondInfirst(std::set<C> &first, const std::set<C> &second) {
    for (auto e=second.begin(); e!=second.end(); e++)
      first.insert(*e);
  }
}

void cta::objectstore::DriveRegister::reportDriveStatus(const std::string& driveName,
  const std::string& logicalLibary, DriveStatus status, time_t reportTime,
  MountType mountType, uint64_t mountSessionId, uint64_t byteTransfered,
  uint64_t filesTransfered, double latestBandwidth, const std::string & vid,
  const std::string & tapepool) {
  // Wrap all the parameters together for easier manipulation by sub-functions
  ReportDriveStatusInputs inputs (driveName, logicalLibary);
  inputs.mountType = mountType;
  inputs.byteTransfered = byteTransfered;
  inputs.filesTransfered = filesTransfered;
  inputs.latestBandwidth = latestBandwidth;
  inputs.mountSessionId = mountSessionId;
  inputs.reportTime = reportTime;
  inputs.status = status;
  inputs.vid = vid;
  inputs.tapepool = tapepool;
  // Sanity-check the input
  checkReportDriveStatusInputs(inputs);
  // Find the drive entry
  checkPayloadWritable();
  serializers::DriveState * drive(NULL);
  {
    auto dl = m_payload.mutable_drives();
    for (auto dp = dl->begin(); dp != dl->end(); dp++) {
      if (dp->drivename() == driveName)
        drive = &(*dp);
    }
  }
  if (!drive) {
    // If the new status does not imply that there should already be an existing
    // session, we just create the drive.
    std::set<int> alreadyInSessionStatuses = {
      (int)serializers::DriveStatus::Transfering,
      (int)serializers::DriveStatus::Unloading,
      (int)serializers::DriveStatus::Unmounting,
      (int)serializers::DriveStatus::DrainingToDisk      
    };
    if (!alreadyInSessionStatuses.count((int)status)) {
      // TODO: the creation should be moved to another place to better logging
      // Explicitely by scheduler? To be decided.
      CreationLog cl(UserIdentity(0,0), "", reportTime, "Automatic creation of "
          "drive on status report");
      addDrive(driveName, logicalLibary, cl);
    } else
      throw NoSuchDrive("In DriveRegister::reportDriveStatus(): No such drive");
    // Find the drive again
    auto dl = m_payload.mutable_drives();
    for (auto dp = dl->begin(); dp != dl->end(); dp++) {
      if (dp->drivename() == driveName)
        drive = &(*dp);
    }
    if (!drive)
      throw NoSuchDrive("In DriveRegister::reportDriveStatus(): Failed to create the drive");
  }
  switch (status) {
    case DriveStatus::Down:
      setDriveDown(inputs, drive);
      break;
    case DriveStatus::Up:
      setDriveUp(inputs, drive);
      break;
    case DriveStatus::Starting:
      setDriveStarting(inputs, drive);
      break;
    case DriveStatus::Mounting:
      setDriveMounting(inputs, drive);
      break;
    case DriveStatus::Transfering:
      setDriveTransfering(inputs, drive);
      break;
    case DriveStatus::Unloading:
      setDriveUnloading(inputs, drive);
      break;
    case DriveStatus::Unmounting:
      setDriveUnmounting(inputs, drive);
      break;
    case DriveStatus::DrainingToDisk:
      setDriveDrainingToDisk(inputs, drive);
      break;
    case DriveStatus::CleaningUp:
      setDriveCleaningUp(inputs, drive);
      break;
    default:
      throw exception::Exception("Unexpected status in DriveRegister::reportDriveStatus");
  }
}

void cta::objectstore::DriveRegister::setDriveDown(ReportDriveStatusInputs& inputs, 
    serializers::DriveState * drive) {
  // If we were already down, then we only update the last update time.
  if (drive->drivestatus() == serializers::DriveStatus::Down) {
    drive->set_lastupdatetime(inputs.reportTime);
    return;
  }
  // If we are changing state, then all should be reset.
  drive->set_logicallibrary(inputs.logicalLibary);
  drive->set_sessionid(0);
  drive->set_bytestransferedinsession(0);
  drive->set_filestransferedinsession(0);
  drive->set_latestbandwidth(0);
  drive->set_sessionstarttime(0);
  drive->set_mountstarttime(0);
  drive->set_transferstarttime(0);
  drive->set_unloadstarttime(0);
  drive->set_unmountstarttime(0);
  drive->set_drainingstarttime(0);
  drive->set_downorupstarttime(inputs.reportTime);
  drive->set_cleanupstarttime(0);
  drive->set_lastupdatetime(inputs.reportTime);
  drive->set_mounttype(serializers::MountType::NoMount);
  drive->set_drivestatus(serializers::DriveStatus::Down);
  drive->set_currentvid("");
  drive->set_currenttapepool("");
}

void cta::objectstore::DriveRegister::setDriveUp(ReportDriveStatusInputs& inputs, 
    serializers::DriveState * drive) {
  // If we were already up, then we only update the last update time.
  if (drive->drivestatus() == serializers::DriveStatus::Up) {
    drive->set_lastupdatetime(inputs.reportTime);
    return;
  }
  // If we are changing state, then all should be reset.
  drive->set_logicallibrary(inputs.logicalLibary);
  drive->set_sessionid(0);
  drive->set_bytestransferedinsession(0);
  drive->set_filestransferedinsession(0);
  drive->set_latestbandwidth(0);
  drive->set_sessionstarttime(0);
  drive->set_mountstarttime(0);
  drive->set_transferstarttime(0);
  drive->set_unloadstarttime(0);
  drive->set_unmountstarttime(0);
  drive->set_drainingstarttime(0);
  drive->set_downorupstarttime(inputs.reportTime);
  drive->set_cleanupstarttime(0);
  drive->set_lastupdatetime(inputs.reportTime);
  drive->set_mounttype(serializers::MountType::NoMount);
  drive->set_drivestatus(serializers::DriveStatus::Up);
  drive->set_currentvid("");
  drive->set_currenttapepool("");
}

void cta::objectstore::DriveRegister::setDriveStarting(ReportDriveStatusInputs& inputs, 
    serializers::DriveState * drive) {
  // If we were already starting, then we only update the last update time.
  if (drive->drivestatus() == serializers::DriveStatus::Starting) {
    drive->set_lastupdatetime(inputs.reportTime);
    return;
  }
  // If we are changing state, then all should be reset. We are not supposed to
  // know the direction yet.
  drive->set_logicallibrary(inputs.logicalLibary);
  drive->set_sessionid(inputs.mountSessionId);
  drive->set_bytestransferedinsession(0);
  drive->set_filestransferedinsession(0);
  drive->set_latestbandwidth(0);
  drive->set_sessionstarttime(inputs.reportTime);
  drive->set_mountstarttime(0);
  drive->set_transferstarttime(0);
  drive->set_unloadstarttime(0);
  drive->set_unmountstarttime(0);
  drive->set_drainingstarttime(0);
  drive->set_downorupstarttime(0);
  drive->set_cleanupstarttime(0);
  drive->set_lastupdatetime(inputs.reportTime);
  drive->set_mounttype(serializeMountType(inputs.mountType));
  drive->set_drivestatus(serializers::DriveStatus::Mounting);
  drive->set_currentvid(inputs.vid);
  drive->set_currenttapepool(inputs.tapepool);
}

void cta::objectstore::DriveRegister::setDriveMounting(ReportDriveStatusInputs& inputs, 
    serializers::DriveState * drive) {
  // If we were already starting, then we only update the last update time.
  if (drive->drivestatus() == serializers::DriveStatus::Mounting) {
    drive->set_lastupdatetime(inputs.reportTime);
    return;
  }
  // If we are changing state, then all should be reset. We are not supposed to
  // know the direction yet.
  drive->set_logicallibrary(inputs.logicalLibary);
  drive->set_sessionid(inputs.mountSessionId);
  drive->set_bytestransferedinsession(0);
  drive->set_filestransferedinsession(0);
  drive->set_latestbandwidth(0);
  //drive->set_sessionstarttime(inputs.reportTime);
  drive->set_mountstarttime(inputs.reportTime);
  drive->set_transferstarttime(0);
  drive->set_unloadstarttime(0);
  drive->set_unmountstarttime(0);
  drive->set_drainingstarttime(0);
  drive->set_downorupstarttime(0);
  drive->set_cleanupstarttime(0);
  drive->set_lastupdatetime(inputs.reportTime);
  drive->set_mounttype(serializeMountType(inputs.mountType));
  drive->set_drivestatus(serializers::DriveStatus::Mounting);
  drive->set_currentvid(inputs.vid);
  drive->set_currenttapepool(inputs.tapepool);
}

void cta::objectstore::DriveRegister::setDriveTransfering(ReportDriveStatusInputs& inputs, 
    serializers::DriveState * drive) {
  // If we were already transferring, we update the full statistics
  if (drive->drivestatus() == serializers::DriveStatus::Transfering) {
    drive->set_lastupdatetime(inputs.reportTime);
    drive->set_bytestransferedinsession(inputs.byteTransfered);
    drive->set_filestransferedinsession(inputs.filesTransfered);
    drive->set_latestbandwidth(inputs.latestBandwidth);
    return;
  }
  // If we are changing state, we can only come from Mounting.
  if (drive->drivestatus() != serializers::DriveStatus::Mounting)
    throw WrongStateTransition("In DriveRegister::setDriveTransfering, transition from wrong state");
  drive->set_logicallibrary(inputs.logicalLibary);
  drive->set_sessionid(inputs.mountSessionId);
  drive->set_bytestransferedinsession(inputs.byteTransfered);
  drive->set_filestransferedinsession(inputs.filesTransfered);
  drive->set_latestbandwidth(inputs.latestBandwidth);
  //drive->set_sessionstarttime(inputs.reportTime);
  //drive->set_mountstarttime(inputs.reportTime);
  drive->set_transferstarttime(inputs.reportTime);
  drive->set_unloadstarttime(0);
  drive->set_unmountstarttime(0);
  drive->set_drainingstarttime(0);
  drive->set_downorupstarttime(0);
  drive->set_cleanupstarttime(0);
  drive->set_lastupdatetime(inputs.reportTime);
  drive->set_mounttype(serializeMountType(inputs.mountType));
  drive->set_drivestatus(serializers::DriveStatus::Transfering);
  drive->set_currentvid(inputs.vid);
  drive->set_currenttapepool(inputs.tapepool);
}

void cta::objectstore::DriveRegister::setDriveUnloading(ReportDriveStatusInputs& inputs, serializers::DriveState* drive) {
  throw NotImplemented("");
}

void cta::objectstore::DriveRegister::setDriveUnmounting(ReportDriveStatusInputs& inputs, serializers::DriveState* drive) {
  throw NotImplemented("");  
}

void cta::objectstore::DriveRegister::setDriveDrainingToDisk(ReportDriveStatusInputs& inputs, serializers::DriveState* drive) {
  throw NotImplemented("");  
}

void cta::objectstore::DriveRegister::setDriveCleaningUp(ReportDriveStatusInputs& inputs, serializers::DriveState* drive) {
  throw NotImplemented("");
}


void cta::objectstore::DriveRegister::checkReportDriveStatusInputs(
  ReportDriveStatusInputs& inputs) {
    // Depending on the status, we might or might not expect statistics.
  std::set<int> statusesRequiringStats = {
    (int)DriveStatus::Transfering,
    (int)DriveStatus::Unloading,
    (int)DriveStatus::Unmounting,
    (int)DriveStatus::DrainingToDisk
  };
  // A superset of the previous requires a VID and a mount ID
  std::set<int> statusesRequiringVid = {
    (int)DriveStatus::Starting,
    (int)DriveStatus::Mounting
  };
  mergeSecondInfirst(statusesRequiringVid, statusesRequiringStats);
  // Check we have statistics when we should
  if (statusesRequiringStats.count((int)inputs.status) && 
       (inputs.byteTransfered == std::numeric_limits<uint64_t>::max() ||
        inputs.filesTransfered == std::numeric_limits<uint64_t>::max() ||
        inputs.latestBandwidth == std::numeric_limits<double>::max()
       ))
    throw MissingStatistics("In DriveRegister::reportDriveStatus, statistics missing for status requiring it");
  if (statusesRequiringVid.count((int)inputs.status) &&
    (inputs.vid == "" || inputs.tapepool == "" || 
     inputs.mountSessionId == std::numeric_limits<uint64_t>::max()))
    throw MissingTapeInfo("In DriveRegister::reportDriveStatus, tape info or session Id missing for status requiring it");
}


auto cta::objectstore::DriveRegister::dumpDrives() -> std::list<DriveState> {
  checkPayloadReadable();
  auto & dl = m_payload.drives();
  std::list<DriveState> ret;
  for (auto d=dl.begin(); d!=dl.end(); d++) {
    ret.push_back(DriveState());
    auto & di = ret.back();
    di.name = d->drivename();
    di.logicalLibrary = d->logicallibrary();
    di.sessionId = d->sessionid();
    di.bytesTransferedInSession = d->bytestransferedinsession();
    di.filesTransferedInSession = d->filestransferedinsession();
    di.latestBandwidth = d->latestbandwidth();
    di.sessionStartTime = d->sessionstarttime();
    di.mountStartTime = d->mountstarttime();
    di.transferStartTime = d->transferstarttime();
    di.unloadStartTime = d->unloadstarttime();
    di.unmountStartTime = d->unmountstarttime();
    di.drainingStartTime = d->drainingstarttime();
    di.downOrUpStartTime = d->downorupstarttime();
    di.cleanupStartTime = d->cleanupstarttime();
    di.lastUpdateTime = d->lastupdatetime();    
    di.mountType = deserializeMountType(d->mounttype());
    di.status = deserializeDriveStatus(d->drivestatus());
    di.currentVid = d->currentvid();
    di.currentTapePool = d->currenttapepool();
  }
  return ret;
}


bool cta::objectstore::DriveRegister::isEmpty() {
  checkPayloadReadable();
  if (m_payload.drives_size())
    return false;
  return true;
}





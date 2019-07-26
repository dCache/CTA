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

#include "DriveState.hpp"
#include "GenericObject.hpp"
#include <google/protobuf/util/json_util.h>

namespace cta { namespace objectstore {

//------------------------------------------------------------------------------
// DriveState::DriveState())
//------------------------------------------------------------------------------
DriveState::DriveState(GenericObject& go):
ObjectOps<serializers::DriveState, serializers::DriveState_t>(go.objectStore()) {
  // Here we transplant the generic object into the new object
  go.transplantHeader(*this);
  // And interpret the header.
  getPayloadFromHeader();
}

//------------------------------------------------------------------------------
// DriveState::garbageCollect())
//------------------------------------------------------------------------------
void DriveState::garbageCollect(const std::string& presumedOwner, AgentReference& agentReference, log::LogContext& lc, cta::catalogue::Catalogue& catalogue) {
  // The drive state is easily replaceable. We just delete it on garbage collection.
  checkPayloadWritable();
  if (presumedOwner != m_header.owner())
    return;
  remove();
  log::ScopedParamContainer params(lc);
  params.add("driveStateObject", getAddressIfSet());
  lc.log(log::INFO, "In DriveState::garbageCollect(): Garbage collected and removed drive state object.");
}

//------------------------------------------------------------------------------
// DriveState::initialize())
//------------------------------------------------------------------------------
void DriveState::initialize(const std::string & driveName) {
  // Setup underlying object with defaults from dataStructures::DriveState
  ObjectOps<serializers::DriveState, serializers::DriveState_t>::initialize();
  m_payload.set_drivename(driveName);
  cta::common::dataStructures::DriveState driveState;
  driveState.driveName = driveName;
  driveState.downOrUpStartTime = ::time(nullptr);
  setState(driveState);
  // This object is good to go (to storage)
  m_payloadInterpreted = true;
}

//------------------------------------------------------------------------------
// DriveState::DriveState())
//------------------------------------------------------------------------------
DriveState::DriveState(const std::string& address, Backend& os):
  ObjectOps<serializers::DriveState, serializers::DriveState_t>(os, address) { }

//------------------------------------------------------------------------------
// DriveState::DriveState())
//------------------------------------------------------------------------------
DriveState::DriveState(Backend& os):
  ObjectOps<serializers::DriveState, serializers::DriveState_t>(os) { }

//------------------------------------------------------------------------------
// DriveState::getState())
//------------------------------------------------------------------------------
cta::common::dataStructures::DriveState DriveState::getState() {
  cta::common::dataStructures::DriveState ret;
  ret.driveName                   = m_payload.drivename();
  ret.host                        = m_payload.host();
  ret.logicalLibrary              = m_payload.logicallibrary();
  ret.sessionId                   = m_payload.sessionid();
  ret.bytesTransferredInSession   = m_payload.bytestransferedinsession();
  ret.filesTransferredInSession   = m_payload.filestransferedinsession();
  ret.latestBandwidth             = m_payload.latestbandwidth();
  ret.sessionStartTime            = m_payload.sessionstarttime();
  ret.mountStartTime              = m_payload.mountstarttime();
  ret.transferStartTime           = m_payload.transferstarttime();
  ret.unloadStartTime             = m_payload.unloadstarttime();
  ret.unmountStartTime            = m_payload.unmountstarttime();
  ret.drainingStartTime           = m_payload.drainingstarttime();
  ret.downOrUpStartTime           = m_payload.downorupstarttime();
  ret.probeStartTime              = m_payload.probestarttime();
  ret.cleanupStartTime            = m_payload.cleanupstarttime();
  ret.lastUpdateTime              = m_payload.lastupdatetime();
  ret.startStartTime              = m_payload.startstarttime();
  ret.shutdownTime                = m_payload.shutdowntime();
  ret.mountType                   = (common::dataStructures::MountType) m_payload.mounttype();
  ret.driveStatus                 = (common::dataStructures::DriveStatus) m_payload.drivestatus();
  ret.desiredDriveState.up        = m_payload.desiredup();
  ret.desiredDriveState.forceDown = m_payload.desiredforcedown();
  ret.currentVid                  = m_payload.currentvid();
  ret.currentTapePool             = m_payload.currenttapepool();
  ret.currentPriority             = m_payload.current_priority();
  if (m_payload.has_current_activity())
    ret.currentActivityAndWeight = 
      cta::common::dataStructures::DriveState::ActivityAndWeight{
        m_payload.current_activity(), m_payload.current_activity_weight()};
  if (m_payload.has_nextmounttype())
    ret.nextMountType = (common::dataStructures::MountType) m_payload.nextmounttype();
  if (m_payload.has_nexttapepool())
    ret.nextTapepool = m_payload.nexttapepool();
  if (m_payload.has_nextvid())
    ret.nextVid = m_payload.nextvid();
  if (m_payload.has_next_priority())
    ret.nextPriority = m_payload.next_priority();
  if (m_payload.has_next_activity())
    ret.nextActivityAndWeight =
        cta::common::dataStructures::DriveState::ActivityAndWeight{
          m_payload.next_activity(), m_payload.next_activity_weight()};
  return ret;
}

//------------------------------------------------------------------------------
// DriveState::setState())
//------------------------------------------------------------------------------
void DriveState::setState(cta::common::dataStructures::DriveState& state) {
  // There should be no need to set the drive name.
  m_payload.set_host(state.host);
  m_payload.set_logicallibrary(state.logicalLibrary);
  m_payload.set_sessionid(state.sessionId);
  m_payload.set_bytestransferedinsession(state.bytesTransferredInSession);
  m_payload.set_filestransferedinsession(state.filesTransferredInSession);
  m_payload.set_latestbandwidth(state.latestBandwidth);
  m_payload.set_sessionstarttime(state.sessionStartTime);
  m_payload.set_mountstarttime(state.mountStartTime);
  m_payload.set_transferstarttime(state.transferStartTime);
  m_payload.set_unloadstarttime(state.unloadStartTime);
  m_payload.set_unmountstarttime(state.unmountStartTime);
  m_payload.set_drainingstarttime(state.drainingStartTime);
  m_payload.set_downorupstarttime(state.downOrUpStartTime);
  m_payload.set_probestarttime(state.probeStartTime);
  m_payload.set_cleanupstarttime(state.cleanupStartTime);
  m_payload.set_lastupdatetime(state.lastUpdateTime);
  m_payload.set_startstarttime(state.startStartTime);
  m_payload.set_shutdowntime(state.shutdownTime);
  m_payload.set_mounttype((uint32_t)state.mountType);
  m_payload.set_drivestatus((uint32_t)state.driveStatus);
  m_payload.set_desiredup(state.desiredDriveState.up);
  m_payload.set_desiredforcedown(state.desiredDriveState.forceDown);
  m_payload.set_currentvid(state.currentVid);
  m_payload.set_currenttapepool(state.currentTapePool);
  m_payload.set_current_priority(state.currentPriority);
  if (state.currentActivityAndWeight) {
    m_payload.set_current_activity(state.currentActivityAndWeight.value().activity);
    m_payload.set_current_activity_weight(state.currentActivityAndWeight.value().weight);
  } else {
    m_payload.clear_current_activity();
    m_payload.clear_current_activity_weight();
  }
  m_payload.set_nextvid(state.nextVid);
  m_payload.set_nexttapepool(state.nextTapepool);
  m_payload.set_next_priority(state.nextPriority);
  m_payload.set_nextmounttype((uint32_t)state.nextMountType);
  if (state.nextActivityAndWeight) {
    m_payload.set_next_activity(state.nextActivityAndWeight.value().activity);
    m_payload.set_next_activity_weight(state.nextActivityAndWeight.value().weight);
  } else {
    m_payload.clear_next_activity();
    m_payload.clear_next_activity_weight();
  }
}

//------------------------------------------------------------------------------
// DriveState::getDiskSpaceReservations())
//------------------------------------------------------------------------------
std::map<std::string, uint64_t> DriveState::getDiskSpaceReservations() {
  checkHeaderReadable();
  std::map<std::string, uint64_t>  ret;
  for (auto &dsr: m_payload.disk_space_reservations()) {
    ret[dsr.disk_system_name()] = dsr.reserved_bytes();
  }
  return ret;
}

//------------------------------------------------------------------------------
// DriveState::addDiskSpaceReservation())
//------------------------------------------------------------------------------
void DriveState::addDiskSpaceReservation(const std::string& diskSystemName, uint64_t bytes) {
  checkPayloadWritable();
  for (auto dsr: *m_payload.mutable_disk_space_reservations()) {
    if (dsr.disk_system_name() == diskSystemName) {
      dsr.set_reserved_bytes(dsr.reserved_bytes() + bytes);
      return;
    }
  }
  auto * newDsr = m_payload.mutable_disk_space_reservations()->Add();
  newDsr->set_disk_system_name(diskSystemName);
  newDsr->set_reserved_bytes(bytes);
}

//------------------------------------------------------------------------------
// DriveState::substractDiskSpaceReservation())
//------------------------------------------------------------------------------
void DriveState::substractDiskSpaceReservation(const std::string& diskSystemName, uint64_t bytes) {
  checkPayloadWritable();
  size_t index=0;
  for (auto dsr: *m_payload.mutable_disk_space_reservations()) {
    if (dsr.disk_system_name() == diskSystemName) {
      if (bytes > dsr.reserved_bytes())
        throw NegativeDiskSpaceReservationReached(
          "In DriveState::substractDiskSpaceReservation(): we would reach a negative reservation size.");
      dsr.set_reserved_bytes(dsr.reserved_bytes() - bytes);
      if (!dsr.reserved_bytes()) {
        // We can remove this entry from the list.
        auto * mdsr = m_payload.mutable_disk_space_reservations();
        mdsr->SwapElements(index, mdsr->size()-1);
        mdsr->RemoveLast();
      }
      return;
    } else {
      ++index;
    }
  }
  if (bytes)
    throw NegativeDiskSpaceReservationReached(
      "In DriveState::substractDiskSpaceReservation(): Trying to substract bytes without previous reservation.");
}

//------------------------------------------------------------------------------
// DriveState::resetDiskSpaceReservation())
//------------------------------------------------------------------------------
void DriveState::resetDiskSpaceReservation() {
  checkPayloadWritable();
  m_payload.mutable_disk_space_reservations()->Clear();
}

//------------------------------------------------------------------------------
// DriveState::dump())
//------------------------------------------------------------------------------
std::string DriveState::dump() {
  checkPayloadReadable();
  google::protobuf::util::JsonPrintOptions options;
  options.add_whitespace = true;
  options.always_print_primitive_fields = true;
  std::string headerDump;
  google::protobuf::util::MessageToJsonString(m_payload, &headerDump, options);
  return headerDump;
}

}} // namespace cta::objectstore





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

#pragma once

#include "ObjectOps.hpp"
#include <list>
#include <limits>
#include "common/dataStructures/DriveState.hpp"
#include "common/dataStructures/DriveNextState.hpp"

namespace cta { namespace objectstore {
  
class Backend;
class Agent;
class GenericObject;
class EntryLogSerDeser;

class DriveRegister: public ObjectOps<serializers::DriveRegister, serializers::DriveRegister_t> {
  CTA_GENERATE_EXCEPTION_CLASS(DuplicateEntry);
public:
  DriveRegister(const std::string & address, Backend & os);
  DriveRegister(GenericObject & go);
  void initialize();
  CTA_GENERATE_EXCEPTION_CLASS(NotEmpty);
  void garbageCollect(const std::string &presumedOwner, AgentReference & agentReference, log::LogContext & lc,
    cta::catalogue::Catalogue & catalogue) override;
  bool isEmpty();
  
  // Drives management =========================================================
  /**
   * Returns all the drive states stored in the drive registry.
   * @return a list of all the drive states
   */
  std::list<cta::common::dataStructures::DriveState> getAllDrivesState();
  
  /**
   * Query the complete drive state for one drive.
   * @param driveName
   * @return complete drive state, or throws an exception if the drive is not
   * known.
   */
  cta::common::dataStructures::DriveState getDriveState(const std::string &driveName);
  
  /**
   * Set the state of a drive. Either creates or overwrites the entry.
   * @param driveState Full drive state (drive name is part of the structure).
   */
  void setDriveState(const cta::common::dataStructures::DriveState driveState);
  
  /**
   * Set the next state of a drive, following a mount decision (scheduling from
   * idle or preemption).
   * @param driveNextState Partial drive state.
   */
  void setNextDriveState(const cta::common::dataStructures::DriveNextState driveNextState);

  /**
   * Remove the drive from the register.
   * @param name
   */
  void removeDrive (const std::string  & driveName);

  /**
   * JSON dump of the drive 
   * @return 
   */
  std::string dump();
};

}}
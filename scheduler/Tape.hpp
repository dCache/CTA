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

#include "common/UserIdentity.hpp"
#include "scheduler/CreationLog.hpp"

#include <stdint.h>
#include <string>

namespace cta {

/**
 * Class representing a tape.
 */
class Tape {
public:

  /**
   * Constructor.
   */
  Tape();

  /**
   * Destructor.
   */
  ~Tape() throw();

  /**
   * Constructor.
   *
   * @param vid The volume identifier of the tape.
   * @param logicalLibraryName The name of the logical library to which the tape
   * belongs.
   * @param tapePoolName The name of the tape pool to which the tape belongs.
   * @param capacityInBytes The capacity of the tape.
   * @param dataOnTapeInBytes The amount of data currently stored on the tape in
   * bytes.
   * @param creationLog The who, where, when an why of this modification.
   */
  Tape(
    const std::string &vid,
    const std::string &logicalLibraryName,
    const std::string &tapePoolName,
    const uint64_t capacityInBytes,
    const uint64_t dataOnTapeInBytes,
    const CreationLog &creationLog);

  /**
   * Less than operator.
   *
   * @param rhs The right-hand side of the operator.
   */
  bool operator<(const Tape &rhs) const throw();

  /**
   * Returns the volume identifier of the tape.
   *
   * @return The volume identifier of the tape.
   */
  const std::string &getVid() const throw();

  /**
   * Returns the name of the logical library to which the tape belongs.
   *
   * @return The name of the logical library to which the tape belongs.
   */
  const std::string &getLogicalLibraryName() const throw();

  /**
   * Returns the name of the tape pool to which the tape belongs.
   *
   * @return The name of the tape pool to which the tape belongs.
   */
  const std::string &getTapePoolName() const throw();

  /**
   * Returns the capacity of the tape.
   *
   * @return The capacity of the tape.
   */
  uint64_t getCapacityInBytes() const throw();

  /**
   * Returns the amount of data on the tape.
   *
   * @return The amount of data on the tape.
   */
  uint64_t getDataOnTapeInBytes() const throw();

  
  /**
   * Get the creation log
   * @return Reference to the creation log
   */
  const CreationLog & getCreationLog() const throw();
  
private:

  /**
   * The volume identifier of the tape.
   */
  std::string m_vid;

  /**
   * The name of the logical library to which the tape belongs.
   */
  std::string m_logicalLibraryName;

  /**
   * The name of the tape pool to which the tape belongs.
   */
  std::string m_tapePoolName;

  /**
   * The capacity of the tape.
   */
  uint64_t m_capacityInBytes;

  /**
   * The amount of data on the tape.
   */
  uint64_t m_dataOnTapeInBytes;

  /**
   * The record of the entry's creation
   */
  CreationLog m_creationLog;
}; // class Tape

} // namespace cta

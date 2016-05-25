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

#include "common/dataStructures/EntryLog.hpp"

#include <stdint.h>
#include <string>
#include <time.h>

namespace cta {

/**
 * Abstract class representing a user request.
 */
struct UserRequest {

  /**
   * Constructor.
   */
  UserRequest();

  /**
   * Destructor.
   */
  virtual ~UserRequest() throw() = 0;

  /**
   * Constructor.
   *
   * @param priority The priority of the request.
   * @param entryLog log for the creation of the request.
   */
  UserRequest(
    const uint64_t priority,
    const common::dataStructures::EntryLog &entryLog);

  /**
   * The priority of the request.
   */
  uint64_t priority;

  /**
   * The time at which the user request was created.
   */
  common::dataStructures::EntryLog entryLog;

}; // class UserRequest

} // namespace cta

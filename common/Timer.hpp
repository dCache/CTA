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

#include <sys/time.h>
#include <sys/types.h>

namespace cta {
namespace utils {

/**
 * A small timing class.
 * It basically remembers a reference time (by default the time of
 * its construction) and gives the elapsed time since then.
 * The reset method allows to reset the reference time to the current time
 */
class Timer {

public:
        
  enum reset_t {
    keepRunning,
    resetCounter
  };

  /**
   * Constructor.
   */
  Timer();

  /**
   * Destructor.
   */
  virtual ~Timer() {}

  /**
   * Gives elapsed time in microseconds with respect to the reference time
   * optionally resets the counter.
   */
  int64_t usecs(reset_t reset = keepRunning);

  /**
   * Gives elapsed time in seconds (with microsecond precision)
   * with respect to the reference time. Optionally resets the counter.
   */
  double secs(reset_t reset = keepRunning);

  /**
   * Resets the Timer reference's time to the current time.
   */
  void reset();

private:

  /**
   * Reference time for this timeri
   */
  timeval m_reference;

}; // class Timer

} // namespace utils
} // namespace cta


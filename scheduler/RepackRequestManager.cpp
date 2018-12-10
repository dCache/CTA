/**
 * The CERN Tape Archive (CTA) project
 * Copyright © 2018 CERN
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

#include "RepackRequestManager.hpp"

namespace cta {

void RepackRequestManager::runOnePass(log::LogContext& lc) {
  // We give ourselves a budget of 30s for those operations...
  utils::Timer t;
  // First expand any request to expand
  // TODO: implement expansion
  // Next promote requests to ToExpand if needed
  m_scheduler.promoteRepackRequestsToToExpand();
  
}

} // namespace cta
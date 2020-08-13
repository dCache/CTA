/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2019  CERN
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

#include "RandomRAOAlgorithm.hpp"

namespace castor { namespace tape { namespace tapeserver { namespace rao {

RandomRAOAlgorithm::RandomRAOAlgorithm() {
}

std::vector<uint64_t> RandomRAOAlgorithm::performRAO(const std::vector<std::unique_ptr<cta::RetrieveJob> >& jobs) {
  std::vector<uint64_t> raoIndices(jobs.size());
  std::iota(raoIndices.begin(),raoIndices.end(),0);
  std::random_shuffle(raoIndices.begin(), raoIndices.end());
  return raoIndices;
}

RandomRAOAlgorithm::~RandomRAOAlgorithm() {
}

}}}}
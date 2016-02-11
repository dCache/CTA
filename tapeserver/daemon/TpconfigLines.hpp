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

#include "tapeserver/daemon/TpconfigLine.hpp"
#include "common/exception/Exception.hpp"

#include <list>

namespace cta { namespace tape { namespace daemon {

/**
 * A list of lines parsed from a TPCONFIG file.
 */
class TpconfigLines: public std::list<TpconfigLine> {
public:

  CTA_GENERATE_EXCEPTION_CLASS(InvalidArgument);
  /**
   * Parses the specified TPCONFIG file.
   *
   * @param filename The filename of the TPCONFIG file.
   * @return The result of parsing the TPCONFIG file.
   */
  static TpconfigLines parseFile(const std::string &filename);
}; // class TpconfigLines

}}} // namespace cta::tape::daemon

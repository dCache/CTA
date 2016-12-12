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

#include "catalogue/CmdLineTool.hpp"

#include <unistd.h>

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
CmdLineTool::CmdLineTool(
  std::istream &inStream,
  std::ostream &outStream,
  std::ostream &errStream) noexcept:
  m_in(inStream),
  m_out(outStream),
  m_err(errStream) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CmdLineTool::~CmdLineTool() noexcept {
}

//------------------------------------------------------------------------------
// getUsername
//------------------------------------------------------------------------------
std::string CmdLineTool::getUsername() const {
  char buf[256];

  if(getlogin_r(buf, sizeof(buf))) {
    return "UNKNOWN";
  } else {
    return buf;
  }
}

//------------------------------------------------------------------------------
// getHostname
//------------------------------------------------------------------------------
std::string CmdLineTool::getHostname() const {
  char buf[256];

  if(gethostname(buf, sizeof(buf))) {
    return "UNKNOWN";
  } else {
    buf[sizeof(buf) - 1] = '\0';
    return buf;
  }
}

} // namespace catalogue
} // namespace cta

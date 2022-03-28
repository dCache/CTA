/*
 * @project      The CERN Tape Archive (CTA)
 * @copyright    Copyright © 2021-2022 CERN
 * @license      This program is free software, distributed under the terms of the GNU General Public
 *               Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING". You can
 *               redistribute it and/or modify it under the terms of the GPL Version 3, or (at your
 *               option) any later version.
 *
 *               This program is distributed in the hope that it will be useful, but WITHOUT ANY
 *               WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *               PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 *               In applying this licence, CERN does not waive the privileges and immunities
 *               granted to it by virtue of its status as an Intergovernmental Organization or
 *               submit itself to any jurisdiction.
 */

#include "common/log/StdoutLogger.hpp"

namespace cta {
namespace log {
  
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
StdoutLogger::StdoutLogger(const std::string &hostName, const std::string &programName, bool simple):
  Logger(hostName, programName, DEBUG), m_simple(simple) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
StdoutLogger::~StdoutLogger() {
}

//------------------------------------------------------------------------------
// prepareForFork
//------------------------------------------------------------------------------
void StdoutLogger::prepareForFork() {}

//------------------------------------------------------------------------------
// writeMsgToUnderlyingLoggingSystem
//------------------------------------------------------------------------------
void StdoutLogger::writeMsgToUnderlyingLoggingSystem(const std::string &header, const std::string &body) {

  if (m_simple) {
      printf("%s\n", body.c_str());
  }  else {
      const std::string headerPlusBody = header + body;
      printf("%s\n", headerPlusBody.c_str());
  }
}

} // namespace log
} // namespace cta

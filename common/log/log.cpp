/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "castor/exception/Exception.hpp"
#include "castor/log/log.hpp"

/**
 * The logger to be used by the CASTOR logging systsem.
 */
static cta::log::Logger *s_logger = NULL;

//------------------------------------------------------------------------------
// init
//------------------------------------------------------------------------------
void cta::log::init(cta::log::Logger *logger) {
  if(s_logger) {
    throw cta::exception::Exception("Failed to initialise logging system"
      ": Logging system already initialised");
  }

  s_logger = logger;
}

//------------------------------------------------------------------------------
// shutdown
//------------------------------------------------------------------------------
void cta::log::shutdown() {
  delete s_logger;
  s_logger = NULL;
}

//------------------------------------------------------------------------------
// instance
//------------------------------------------------------------------------------
cta::log::Logger &cta::log::instance() {
  if(NULL == s_logger) {
    throw cta::exception::Exception("Failed to get CASTOR logger"
      ": Logger does not exist");
  }
  return *s_logger;
}

//------------------------------------------------------------------------------
// prepareForFork
//------------------------------------------------------------------------------
void cta::log::prepareForFork() {
  try {
    instance().prepareForFork();
  } catch(cta::exception::Exception &ex) {
    throw cta::exception::Exception(
     std::string("Failed to prepare logger for call to fork(): ") +
       ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// write
//------------------------------------------------------------------------------
void cta::log::write(
  const int priority,
  const std::string &msg,
  const std::list<cta::log::Param> &params) {
  if(s_logger) (*s_logger)(priority, msg, params);
}

//------------------------------------------------------------------------------
// write
//------------------------------------------------------------------------------
void cta::log::write(
  const int priority,
  const std::string &msg,
  const std::string &rawParams,
  const struct timeval &timeStamp,
  const std::string &progName,
  const int pid) {
  const std::list<Param> params;
  if(s_logger) (*s_logger)(priority, msg, params, rawParams, timeStamp,
    progName, pid);
}

//------------------------------------------------------------------------------
// getProgramName
//------------------------------------------------------------------------------
std::string cta::log::getProgramName() {
  if(s_logger) {
    return (*s_logger).getProgramName();
  } else {
    return "";
  }
}

//------------------------------------------------------------------------------
// operator<<
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Cuuid_t& uuid) {
  char uuidstr[CUUID_STRING_LEN + 1];
  memset(uuidstr, '\0', CUUID_STRING_LEN + 1);
  Cuuid2string(uuidstr, CUUID_STRING_LEN + 1, &uuid);
  out << uuidstr;
  return out;
}

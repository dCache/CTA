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

#include "OcciConn.hpp"
#include "OcciEnv.hpp"
#include "common/exception/Exception.hpp"

#include <occi.h>

namespace cta {
namespace rdbms {

/**
 * Following the pimpl idiom, this is the class actually implementing OcciEnv.
 */
class OcciEnv::Impl {
public:

  /**
   * Constructor.
   *
   * Creates an OCCI environment with a THREADED_MUTEXED mode.
   */
  Impl() {
    using namespace oracle::occi;
    m_env = Environment::createEnvironment(Environment::THREADED_MUTEXED);
    if(NULL == m_env) {
      throw exception::Exception(std::string(__FUNCTION__) + "failed"
        ": oracle::occi::createEnvironment() returned a NULL pointer");
    }
  }

  /**
   * Destructor.
   *
   * Terminates the underlying OCCI environment.
   */
  ~Impl() throw() {
    using namespace oracle::occi;
    Environment::terminateEnvironment(m_env);
  }

  /**
   * Creates an OCCI connection.
   *
   * This method will throw an exception if either the username, password ori
   * database parameters are NULL pointers.
   *
   * @param username The name of the database user.
   * @param password The database password.
   * @param database The name of the database.
   * @return The newly created OCCI connection.
   */
  DbConn *createConn(
    const std::string &username,
    const std::string &password,
    const std::string &database) {
    try {
      oracle::occi::Connection *const conn = m_env->createConnection(username, password, database);
      if (NULL == conn) {
        throw exception::Exception("oracle::occi::createConnection() returned a NULL pointer");
      }

      return new OcciConn(m_env, conn);
    } catch(exception::Exception &ex) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
    } catch(std::exception &se) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: " + se.what());
    }
  }

private:

  /**
   * The OCCI environment.
   */
  oracle::occi::Environment *m_env;

}; // class Impl

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
OcciEnv::OcciEnv(): m_impl(new Impl()) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OcciEnv::~OcciEnv() = default;

//------------------------------------------------------------------------------
// createConn
//------------------------------------------------------------------------------
DbConn *OcciEnv::createConn(
  const std::string &username,
  const std::string &password,
  const std::string &database) {
  return m_impl->createConn(username, password, database);
}

} // namespace rdbms
} // namespace cta

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

#include "scheduler/UserIdentity.hpp"

#include <string>

namespace cta {

/**
 * Class containing the security information necessary to authorise a user
 * submitting a requests from a specific host.
 */
struct SecurityIdentity {

  /**
   * Constructor.
   */
  SecurityIdentity();

  /**
   * Constructor.
   */
  SecurityIdentity(const UserIdentity &user, const std::string &host);

  /**
   * Returns The identity of the user.
   *
   * @return The identity of the user.
   */
  const UserIdentity &getUser() const throw();

  /**
   * Returns the network name of the host from which they are submitting a
   * request.
   *
   * @return The network name of the host from which they are submitting a
   * request.
   */
  const std::string &getHost() const throw();

private:

  /**
   * The identity of the user.
   */
  UserIdentity m_user;

  /**
   * The network name of the host from which they are submitting a request.
   */
  std::string m_host;

}; // struct SecurityIdentity

} // namespace cta

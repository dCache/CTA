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

#include <list>
#include <map>
#include <stdint.h>
#include <string>


namespace cta {
namespace common {
namespace dataStructures {

class UserIdentity {

public:

  /**
   * Constructor
   */
  UserIdentity();

  /**
   * Destructor
   */
  ~UserIdentity() throw();

  /**
   * Equality operator.
   *
   * @param rhs The right hand side of the operator.
   */
  bool operator==(const UserIdentity &rhs) const;

  void setGid(const uint64_t gid);
  uint64_t getGid() const;

  void setUid(const uint64_t uid);
  uint64_t getUid() const;
  

private:
  
  /**
   * @return true if all fields have been set, false otherwise
   */
  bool allFieldsSet() const;

  uint64_t m_gid;
  bool m_gidSet;

  uint64_t m_uid;
  bool m_uidSet;

  friend std::ostream &operator<<(std::ostream &, const UserIdentity &);

}; // class UserIdentity

/**
 * Output stream operator for UserIdentity.
 *
 * @param os The output stream.
 * @param entryLog The entry log.
 * @return The output stream.
 */
std::ostream &operator<<(std::ostream &os, const UserIdentity &userIdentity);

} // namespace dataStructures
} // namespace common
} // namespace cta

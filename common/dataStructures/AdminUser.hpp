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

#include "common/dataStructures/EntryLog.hpp"
#include "common/dataStructures/UserIdentity.hpp"

namespace cta {
namespace common {
namespace dataStructures {

class AdminUser {

public:

  /**
   * Constructor
   */
  AdminUser();

  /**
   * Destructor
   */
  ~AdminUser() throw();

  void setComment(const std::string &comment);
  std::string getComment() const;

  void setCreationLog(const cta::common::dataStructures::EntryLog &creationLog);
  cta::common::dataStructures::EntryLog getCreationLog() const;

  void setLastModificationLog(const cta::common::dataStructures::EntryLog &lastModificationLog);
  cta::common::dataStructures::EntryLog getLastModificationLog() const;

  void setUser(const cta::common::dataStructures::UserIdentity &user);
  cta::common::dataStructures::UserIdentity getUser() const;
  

private:
  
  /**
   * @return true if all fields have been set, false otherwise
   */
  bool allFieldsSet() const;

  std::string m_comment;
  bool m_commentSet;

  cta::common::dataStructures::EntryLog m_creationLog;
  bool m_creationLogSet;

  cta::common::dataStructures::EntryLog m_lastModificationLog;
  bool m_lastModificationLogSet;

  cta::common::dataStructures::UserIdentity m_user;
  bool m_userSet;

}; // class AdminUser

} // namespace dataStructures
} // namespace common
} // namespace cta

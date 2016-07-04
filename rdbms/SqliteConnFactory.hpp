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

#include "DbConnFactory.hpp"

namespace cta {
namespace rdbms {

/**
 * A concrete factory of DbConn objects.
 */
class SqliteConnFactory: public DbConnFactory {
public:

  /**
   * Constructor.
   *
   * @param filename The filename to be passed to the sqlite3_open() function.
   */
  SqliteConnFactory(const std::string &filename);

  /**
   * Destructor.
   */
  virtual ~SqliteConnFactory() throw();

  /**
   * Returns a newly created database connection.
   *
   * @return A newly created database connection.
   */
  virtual DbConn *create();

private:

  /**
   * The filename to be passed to the sqlite3_open() function.
   */
  std::string m_filename;

}; // class SqliteConnFactory

} // namespace rdbms
} // namespace cta

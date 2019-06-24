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

#include <string>
#include <list>
#include <map>

namespace cta {
namespace catalogue {

/**
 * Structure containing the common schema procedures of the CTA catalogue
 * database.
 */
struct CatalogueSchema {
  /**
   * Constructor.
   *
   * @param sqlSchema The sql for the catalogue schema provided at compilation
   *                  time.
   */
  CatalogueSchema(const std::string &sqlSchema);
  
  /**
   * Constructor.
   *
   * @param sqlSchema The sql for the catalogue schema provided at compilation
   * time.
   * @param sqlTriggerSchema The sql for triggers in the catalogue schema provided
   * at compilation time only for schemas that support triggers.
   */
  CatalogueSchema(const std::string &sqlSchema, const std::string &sqlTriggerSchema);
  
  /**
   * The schema.
   */
  const std::string sql;
  
  /**
   * The trigger.
   */
  const std::string sql_trigger;

  /**
   * Returns the names of all the tables in the catalogue schema.
   *
   * @return The names of all the tables in the catalogue schema.
   */
  std::list<std::string> getSchemaTableNames() const;
  
  /**
   * Returns the names of all the column and their type as a map for the given 
   * table in the catalogue schema.
   *
   * @param tableName The table name to get the columns.
   * @return The map of types by name of all the columns for the given table in the catalogue schema.
   */
  std::map<std::string, std::string> getSchemaColumns(const std::string &tableName) const;
  
  /**
   * Returns the names of all the indexes in the catalogue schema.
   *
   * @return The names of all the indexes in the catalogue schema.
   */
  std::list<std::string> getSchemaIndexNames() const;
  
  /**
   * Returns the names of all the sequences in the catalogue schema.
   * 
   * if the underlying database technologies does not supported sequences then
   * this method simply returns an empty list.
   *
   * @return The names of all the sequences in the catalogue schema.
   */
  std::list<std::string> getSchemaSequenceNames() const;
  
  /**
   * Returns the names of all the triggers in the catalogue schema.
   * 
   * if the underlying database technologies does not supported triggers then
   * this method simply returns an empty list.*
   * 
   * @return The names of all the triggers in the catalogue schema.
   */
  std::list<std::string> getSchemaTriggerNames() const;
  
  /**
   * Returns the map of strings to uint64 for the catalogue SCHEMA_VERSION_MAJOR
   * and SCHEMA_VERSION_MINOR values.
   * 
   * @return The map for SCHEMA_VERSION_MAJOR and SCHEMA_VERSION_MINOR  values.
   */
  std::map<std::string, uint64_t> getSchemaVersion() const;
};

} // namespace catalogue
} // namespace cta
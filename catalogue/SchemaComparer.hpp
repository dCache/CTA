/**
 * The CERN Tape Archive (CTA) project
 * Copyright © 2018 CERN
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
/* 
 * File:   SchemaComparer.hpp
 * Author: cedric
 *
 * Created on December 10, 2019, 10:58 AM
 */

#pragma once

#include "rdbms/ConnPool.hpp"
#include "SchemaComparerResult.hpp"
#include "CatalogueMetadataGetter.hpp"
#include "SchemaSqlStatementsReader.hpp"

namespace cta {
namespace catalogue {

/**
 * This class is used to compare the schema that is running against the database accessible
 * via the connection given in the constructor
 */
class SchemaComparer {
public:
  /**
   * Constructs a SchemaComparer
   * @param catalogueMetadataGetter the catalogue metadata getter to compare the catalogue schema content
   */
  SchemaComparer(CatalogueMetadataGetter &catalogueMetadataGetter);
  /**
   * Destructor
   */
  virtual ~SchemaComparer();
  /**
   * Compare the schema to compare against the database 
   * @return a SchemaComparerResult object that will contain the differences if there are some
   */
  virtual SchemaComparerResult compareAll() = 0;
  /**
   * Compare the tables of the schema against the catalogue database
   * @return a SchemaComparerResult that will contain the differences if there are some
   */
  virtual SchemaComparerResult compareTables() = 0;
  /**
   * Compare the indexes of the schema against the catalogue database
   * @return a SchemaComparerResult that will contain the differences if there are some
   */
  virtual SchemaComparerResult compareIndexes() = 0;
  
  /**
   * Compare only the tables in the list given in parameter
   * @param tableNamesToCompare the tables to compare between the Schema and the database
   * @return a SchemaComparerResult that will contain the differences if there are some
   */
  virtual SchemaComparerResult compareTablesInList(const std::list<std::string> tableNamesToCompare) = 0;

  /**
   * Sets the way the schema sql statements will be read to do the schemas comparison
   * @param schemaSqlStatementsReader the reader used to get the schema sql statements in order to do schema comparison
   */
  void setSchemaSqlStatementsReader(std::unique_ptr<SchemaSqlStatementsReader> schemaSqlStatementsReader);
  
protected:
  cta::catalogue::CatalogueMetadataGetter & m_catalogueMetadataGetter;
  std::unique_ptr<SchemaSqlStatementsReader> m_schemaSqlStatementsReader;
  bool m_compareTableConstraints;
};

}}
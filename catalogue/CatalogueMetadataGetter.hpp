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

#pragma once

#include "rdbms/Conn.hpp"
#include "rdbms/Login.hpp"
#include "SchemaCreatingSqliteCatalogue.hpp"
#include <list>

namespace cta {
namespace catalogue {
  
class CatalogueMetadataGetter {
public:
  protected:
    rdbms::Conn& m_conn;
  public:
    CatalogueMetadataGetter(cta::rdbms::Conn & conn);
    virtual ~CatalogueMetadataGetter();
    std::string getCatalogueVersion();
    virtual std::list<std::string> getIndexNames() = 0;
    virtual std::list<std::string> getTableNames() = 0;
};

class SQLiteCatalogueMetadataGetter: public CatalogueMetadataGetter{
public:
  SQLiteCatalogueMetadataGetter(cta::rdbms::Conn & conn);
  virtual ~SQLiteCatalogueMetadataGetter();
  std::list<std::string> getIndexNames() override;
  std::list<std::string> getTableNames() override;
};

class OracleCatalogueMetadataGetter: public CatalogueMetadataGetter{
  public:
  OracleCatalogueMetadataGetter(cta::rdbms::Conn & conn);
  virtual ~OracleCatalogueMetadataGetter();
  std::list<std::string> getIndexNames() override;
  std::list<std::string> getTableNames() override;
};

class CatalogueMetadataGetterFactory {
public:
  static CatalogueMetadataGetter* create(const rdbms::Login::DbType dbType, cta::rdbms::Conn & conn);
};

}}
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

#include "catalogue/CatalogueFactory.hpp"
#include "catalogue/OcciConn.hpp"
#include "catalogue/OcciEnvSingleton.hpp"
#include "catalogue/RdbmsCatalogue.hpp"
#include "catalogue/SqliteConn.hpp"
#include "common/exception/Exception.hpp"

#include <memory>

namespace cta {
namespace catalogue {

//------------------------------------------------------------------------------
// create
//------------------------------------------------------------------------------
Catalogue *CatalogueFactory::create(const DbLogin &dbLogin) {
  try {
    switch(dbLogin.dbType) {
    case DbLogin::DBTYPE_IN_MEMORY:
      {
        std::unique_ptr<SqliteConn> sqliteConn(new SqliteConn(":memory:"));
        sqliteConn->createCatalogueDatabaseSchema();
        return new RdbmsCatalogue(sqliteConn.release());
      }
    case DbLogin::DBTYPE_ORACLE:
      {
        OcciConn *occiConn = OcciEnvSingleton::instance().createConn(dbLogin.username, dbLogin.password,
          dbLogin.database);
        return new RdbmsCatalogue(occiConn);
      }
    case DbLogin::DBTYPE_NONE:
      throw exception::Exception("Cannot create a catalogue without a database type");
    default:
      {
        exception::Exception ex;
        ex.getMessage() << "Unknown database type: value=" << dbLogin.dbType;
        throw ex;
      }
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

} // namespace catalogue
} // namespace cta

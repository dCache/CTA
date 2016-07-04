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

#include "catalogue/CatalogueTest.hpp"

namespace unitTests {

namespace {

/**
 * Creates DbLogin objects for in-memory catalogue databases.
 */
class InMemoryDbLoginFactory: public cta::rdbms::DbLoginFactory {
public:

  /**
   * Destructor.
   */
  virtual ~InMemoryDbLoginFactory() {
  }

  /**
   * Returns a newly created DbLogin object.
   *
   * @return A newly created DbLogin object.
   */
  virtual cta::rdbms::DbLogin create() {
    using namespace cta::catalogue;
    return cta::rdbms::DbLogin(cta::rdbms::DbLogin::DBTYPE_IN_MEMORY, "", "", "");
  }
}; // class InMemoryDbLoginFactory

InMemoryDbLoginFactory g_inMemoryDbLoginFactory;

} // anonymous namespace

INSTANTIATE_TEST_CASE_P(InMemory, cta_catalogue_CatalogueTest,
  ::testing::Values(dynamic_cast<cta::rdbms::DbLoginFactory*>(&g_inMemoryDbLoginFactory)));

} // namespace unitTests

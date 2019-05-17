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

#include "common/exception/Exception.hpp"
#include "common/make_unique.hpp"
#include "rdbms/ConnPool.hpp"
#include "rdbms/StmtTest.hpp"

#include <gtest/gtest.h>

namespace unitTests {

//------------------------------------------------------------------------------
// Setup
//------------------------------------------------------------------------------
void cta_rdbms_StmtTest::SetUp() {
  using namespace cta::rdbms;

  if(!m_connPool) {
    m_login = GetParam()->create();
    const uint64_t maxNbConns = 1;
    m_connPool = cta::make_unique<ConnPool>(m_login, maxNbConns);
  }

  m_conn = m_connPool->getConn();
  ASSERT_EQ(AutocommitMode::AUTOCOMMIT_ON, m_conn.getAutocommitMode());

  try {
    m_conn.executeNonQuery("DROP TABLE STMT_TEST");
  } catch(...) {
    // Do nothing
  }

  m_conn.executeNonQuery("CREATE TABLE STMT_TEST(DOUBLE_COL FLOAT, UINT64_COL NUMERIC(20, 0))");
}

//------------------------------------------------------------------------------
// TearDown
//------------------------------------------------------------------------------
void cta_rdbms_StmtTest::TearDown() {
  using namespace cta::rdbms;
  try {
    m_conn.executeNonQuery("DROP TABLE STMT_TEST");
  } catch(...) {
    // Do nothing
  }
}

TEST_P(cta_rdbms_StmtTest, insert_with_bindDouble) {
  using namespace cta::rdbms;

  const double insertValue = 1.234;

  // Insert a row into the test table
  {
    const char *const sql =
      "INSERT INTO STMT_TEST("
        "DOUBLE_COL) "
      "VALUES("
        ":DOUBLE_COL)";
    auto stmt = m_conn.createStmt(sql);
    stmt.bindDouble(":DOUBLE_COL", insertValue);
    stmt.executeNonQuery();
  }

  // Select the row back from the table
  {
    const char *const sql =
      "SELECT "
        "DOUBLE_COL AS DOUBLE_COL "
      "FROM "
        "STMT_TEST";
    auto stmt = m_conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    ASSERT_TRUE(rset.next());

    const auto selectValue = rset.columnOptionalDouble("DOUBLE_COL");

    ASSERT_TRUE((bool)selectValue);

    const double diff = insertValue - selectValue.value();
    ASSERT_TRUE(0.000001 > diff);

    ASSERT_FALSE(rset.next());
  }
}

TEST_P(cta_rdbms_StmtTest, insert_with_bindUint64) {
  using namespace cta::rdbms;

  const uint64_t insertValue = 1234;

  // Insert a row into the test table
  {
    const char *const sql =
      "INSERT INTO STMT_TEST("
        "UINT64_COL) "
      "VALUES("
        ":UINT64_COL)";
    auto stmt = m_conn.createStmt(sql);
    stmt.bindDouble(":UINT64_COL", insertValue);
    stmt.executeNonQuery();
  }

  // Select the row back from the table
  {
    const char *const sql =
      "SELECT "
        "UINT64_COL AS UINT64_COL "
      "FROM "
        "STMT_TEST";
    auto stmt = m_conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    ASSERT_TRUE(rset.next());

    const auto selectValue = rset.columnOptionalDouble("UINT64_COL");

    ASSERT_TRUE((bool)selectValue);

    ASSERT_EQ(insertValue,selectValue.value());

    ASSERT_FALSE(rset.next());
  }
}

TEST_P(cta_rdbms_StmtTest, insert_with_bindUint64_2_pow_54_minus_2) {
  using namespace cta::rdbms;

  // The MySql support in CTA cannot store an unsigned integer greater than
  // 2^54-2
  const uint64_t insertValue = 18014398509481982;

  // Insert a row into the test table
  {
    const char *const sql =
      "INSERT INTO STMT_TEST("
        "UINT64_COL) "
      "VALUES("
        ":UINT64_COL)";
    auto stmt = m_conn.createStmt(sql);
    stmt.bindUint64(":UINT64_COL", insertValue);
    stmt.executeNonQuery();
  }

  // Select the row back from the table
  {
    const char *const sql =
      "SELECT "
        "UINT64_COL AS UINT64_COL "
      "FROM "
        "STMT_TEST";
    auto stmt = m_conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    ASSERT_TRUE(rset.next());

    const auto selectValue = rset.columnOptionalUint64("UINT64_COL");

    ASSERT_TRUE((bool)selectValue);

    ASSERT_EQ(insertValue,selectValue.value());

    ASSERT_FALSE(rset.next());
  }
}

TEST_P(cta_rdbms_StmtTest, insert_with_bindUint64_2_pow_64_minus_1_not_mysql) {
  using namespace cta::rdbms;

  if(m_login.dbType == Login::DBTYPE_MYSQL) {
    return;
  }

  // The MySql support in CTA cannot store an unsigned integer greater than
  // 2^54-2
  const uint64_t insertValue = 18446744073709551615U;

  // Insert a row into the test table
  {
    const char *const sql =
      "INSERT INTO STMT_TEST("
        "UINT64_COL) "
      "VALUES("
        ":UINT64_COL)";
    auto stmt = m_conn.createStmt(sql);
    stmt.bindUint64(":UINT64_COL", insertValue);
    stmt.executeNonQuery();
  }

  // Select the row back from the table
  {
    const char *const sql =
      "SELECT "
        "UINT64_COL AS UINT64_COL "
      "FROM "
        "STMT_TEST";
    auto stmt = m_conn.createStmt(sql);
    auto rset = stmt.executeQuery();
    ASSERT_TRUE(rset.next());

    const auto selectValue = rset.columnOptionalUint64("UINT64_COL");

    ASSERT_TRUE((bool)selectValue);

    ASSERT_EQ(insertValue,selectValue.value());

    ASSERT_FALSE(rset.next());
  }
}

} // namespace unitTests

/******************************************************************************
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-cardinalities.h>
#include "Regex.hpp"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;

namespace unitTests {

TEST(Regex, BasicFunctionality) {
  cta::utils::Regex re("a(b)");
  std::vector<std::string> ret1, ret2;
  ret1 = re.exec("1abc");
  ret2 = re.exec("xyz");

  ASSERT_EQ(ret1.size(), 2U);
  ASSERT_EQ(ret1[0], "ab");
  ASSERT_EQ(ret1[1], "b");
  ASSERT_EQ(ret2.size(), 0U);
}

TEST(Regex, OperationalTest) {
  cta::utils::Regex re("^scsi_tape:(st[[:digit:]]+)$");
  std::vector<std::string> ret1, ret2, ret3;
  ret1 = re.exec("scsi_tape:st1");
  ret2 = re.exec("scsi_tape:st124");
  ret3 = re.exec("scsi_tape:st1a");

  ASSERT_EQ(ret1.size(), 2U);
  ASSERT_EQ(ret1[0], "scsi_tape:st1");
  ASSERT_EQ(ret1[1], "st1");
  ASSERT_EQ(ret2.size(), 2U);
  ASSERT_EQ(ret2[0], "scsi_tape:st124");
  ASSERT_EQ(ret2[1], "st124");
  ASSERT_EQ(ret3.size(), 0U);
}

 
TEST(Regex, SubstringMatch) {
  cta::utils::Regex re("^radosstriper:///([^:]+@[^:]+):(.*)$");
  std::vector<std::string> ret1;
  ret1 = re.exec("radosstriper:///user@pool:12345@castorns.7890");

  ASSERT_EQ(ret1.size(), 3U);
  ASSERT_EQ(ret1[1], "user@pool");
  ASSERT_EQ(ret1[2], "12345@castorns.7890");
}

TEST(Regex, NestedMatch) {
  cta::utils::Regex re("^rados://([^@]+)@([^:]+)(|:(.+))$");
  auto ret1 = re.exec("rados://user@pool:namespace");

  ASSERT_EQ(5U, ret1.size());
  ASSERT_EQ("user", ret1[1]);
  ASSERT_EQ("pool", ret1[2]);
  ASSERT_EQ("namespace", ret1[4]);
  
  // The nested match does not show up in the result set if its branch is not met.
  ret1 = re.exec("rados://user1@pool2");
  ASSERT_EQ(4U, ret1.size());
  ASSERT_EQ("user1", ret1[1]);
  ASSERT_EQ("pool2", ret1[2]);
  ASSERT_EQ("", ret1[3]);
}
}


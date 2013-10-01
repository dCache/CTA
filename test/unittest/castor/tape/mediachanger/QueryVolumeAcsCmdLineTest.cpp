/******************************************************************************
 *          test/unittest/castor/tape/dismountacs/QueryVolumeAcsCmdLineTest.hpp
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#include "castor/tape/mediachanger/QueryVolumeAcsCmdLine.hpp"

#include <cppunit/extensions/HelperMacros.h>

namespace castor {
namespace tape {
namespace mediachanger {

class QueryVolumeAcsCmdLineTest: public CppUnit::TestFixture {
public:

  void setUp() {
  }

  void tearDown() {
  }

  void testConstructor() {
    const QueryVolumeAcsCmdLine cmdLine;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Testing debug flag is initialised to FALSE",
      (BOOLEAN)FALSE, cmdLine.debug);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Testing help flag is initialised to FALSE",
      (BOOLEAN)FALSE, cmdLine.help);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Testing timeout option is initialised to 0",
      0, cmdLine.timeout);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Testing query option is initialised to 0",
      0, cmdLine.queryInterval);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "Testing volId is initialised to an empty string",
      '\0', cmdLine.volId.external_label[0]);
  }

  CPPUNIT_TEST_SUITE(QueryVolumeAcsCmdLineTest);
  CPPUNIT_TEST(testConstructor);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(QueryVolumeAcsCmdLineTest);

} // namespace mediachanger
} // namespace tape
} // namespace castor

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

#include <gtest/gtest.h>

#include "tapeserver/daemon/Tpconfig.hpp"
#include "tests/TempFile.hpp"
#include "common/exception/Errnum.hpp"

namespace unitTests {

TEST(cta_Daemon, Tpconfig_base) {
  TempFile tf;
  // Test with empty file
  tf.stringFill("");
  cta::tape::daemon::Tpconfig tpc = cta::tape::daemon::Tpconfig::parseFile(tf.path());
  ASSERT_EQ(0, tpc.size());
  // Test with file full of comments (but no valid line)
  tf.stringFill("# some comment\n"
      "\t   \t # Some non-empty line (spaces)\n"
      "\t\t\t                   \n");
  tpc = cta::tape::daemon::Tpconfig::parseFile(tf.path());
  ASSERT_EQ(0, tpc.size());
  
  // Test with non-existing file
  ASSERT_THROW(cta::tape::daemon::Tpconfig::parseFile("/no/such/file"), cta::exception::Errnum);
  // Check we get the expected Errno.
  try {
    cta::tape::daemon::Tpconfig::parseFile("/no/such/file");
    ASSERT_TRUE(false); // We should never get here.
  } catch (cta::exception::Errnum & ex) {
    ASSERT_NE(std::string::npos, ex.getMessageValue().find("Errno=2:"));
  }
  
  // Test with a line too short
  tf.stringFill("TapeDrive");
  ASSERT_THROW(cta::tape::daemon::Tpconfig::parseFile(tf.path()), cta::exception::Exception);
  try {
    cta::tape::daemon::Tpconfig::parseFile(tf.path());
    ASSERT_TRUE(false); // We should never get here.
  } catch (cta::exception::Exception & ex) {
    ASSERT_NE(std::string::npos, ex.getMessageValue().find("missing"));
  }
  
  // Test with line too long
  tf.stringFill("TapeDrive lib /dev/tape libSlot ExtraArgument");
  ASSERT_THROW(cta::tape::daemon::Tpconfig::parseFile(tf.path()), cta::exception::Exception);
  try {
    cta::tape::daemon::Tpconfig::parseFile(tf.path());
    ASSERT_TRUE(false); // We should never get here.
  } catch (cta::exception::Exception & ex) {
    ASSERT_NE(std::string::npos, ex.getMessageValue().find("extra parameter"));
  }
  
  // Test with several entries (valid file with various extra blanks)
  tf.stringFill("         drive0 lib0 \t\t\t /dev/tape0       lib0slot0\n"
      "drive1 lib0 /dev/tape1 lib0slot1         \n"
  "drive2 lib0 /dev/tape2 lib0slot2");
  tpc = cta::tape::daemon::Tpconfig::parseFile(tf.path());
  ASSERT_EQ(3, tpc.size());
  int i=0;
  for(auto & t: tpc) {
    ASSERT_EQ("drive", t.unitName.substr(0,5));
    ASSERT_EQ("lib0", t.logicalLibrary);
    ASSERT_EQ("/dev/tape", t.devFilename.substr(0,9));
    ASSERT_EQ("lib0slot", t.librarySlot.substr(0,8));
    ASSERT_EQ('0'+i, t.unitName.back());
    ASSERT_EQ('0'+i, t.devFilename.back());
    ASSERT_EQ('0'+i, t.librarySlot.back());
    i++;
  }
}

} // namespace unitTests

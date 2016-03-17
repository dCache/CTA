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

#include "tapeserver/daemon/ConfigurationFile.hpp"
#include "tapeserver/daemon/TapedConfiguration.hpp"
#include "tests/TempFile.hpp"
#include "common/log/StdoutLogger.hpp"

namespace unitTests {

TEST(cta_Daemon, TapedConfiguration) {
  TempFile incompleteConfFile, completeConfFile;
  incompleteConfFile.stringFill(
  "# My incomplete taped configuration file\n"
  );
  completeConfFile.stringFill(
  "#A good enough configuration file for taped\n"
  "general ObjectStoreURL vfsObjectStore:///tmp/dir\n"
  "general FileCatalogURL sqliteFileCatalog:///tmp/dir2\n"
  "taped BufferCount 1\n"
  "taped TpConfigPath ");
  TempFile emptyTpConfig;
  completeConfFile.stringAppend(emptyTpConfig.path());
  ASSERT_THROW(cta::tape::daemon::TapedConfiguration::createFromCtaConf(incompleteConfFile.path()),
      cta::tape::daemon::SourcedParameter<uint64_t>::MandatoryParameterNotDefined);
  auto completeConfig = 
    cta::tape::daemon::TapedConfiguration::createFromCtaConf(completeConfFile.path());
  ASSERT_EQ(completeConfFile.path()+":2", completeConfig.objectStoreURL.source());
  ASSERT_EQ("vfsObjectStore:///tmp/dir", completeConfig.objectStoreURL.value());
  ASSERT_EQ(completeConfFile.path()+":3", completeConfig.fileCatalogURL.source());
  ASSERT_EQ("sqliteFileCatalog:///tmp/dir2", completeConfig.fileCatalogURL.value());
}

TEST(cta_Daemon, TapedConfigurationFull) {
  cta::log::StdoutLogger log("unitTests");
  TempFile completeConfFile;
  completeConfFile.stringFill(
  "#A good enough configuration file for taped\n"
  "general ObjectStoreURL vfsObjectStore:///tmp/dir\n"
  "general FileCatalogURL sqliteFileCatalog:///tmp/dir2\n"
  "taped ArchiveFetchBytesFiles 1,2\n"
  "taped ArchiveFlushBytesFiles              3 , 4 \n"
  "taped RetrieveFetchBytesFiles  5,   6\n"
  "taped BufferCount 1  \n"
  "taped TpConfigPath ");
  TempFile TpConfig;
  TpConfig.stringFill("drive0 lib0 /dev/tape0 lib0slot0\n"
      "drive1 lib0 /dev/tape1 lib0slot1\n"
      "drive2 lib0 /dev/tape2 lib0slot2");
  completeConfFile.stringAppend(TpConfig.path());
  // The log parameter can be uncommented to inspect the result on the output.
  auto completeConfig = 
    cta::tape::daemon::TapedConfiguration::createFromCtaConf(completeConfFile.path()/*, log*/);
  ASSERT_EQ(completeConfFile.path()+":2", completeConfig.objectStoreURL.source());
  ASSERT_EQ("vfsObjectStore:///tmp/dir", completeConfig.objectStoreURL.value());
  ASSERT_EQ(completeConfFile.path()+":3", completeConfig.fileCatalogURL.source());
  ASSERT_EQ("sqliteFileCatalog:///tmp/dir2", completeConfig.fileCatalogURL.value());
  ASSERT_EQ(1, completeConfig.archiveFetchBytesFiles.value().maxBytes);
  ASSERT_EQ(2, completeConfig.archiveFetchBytesFiles.value().maxFiles);
  ASSERT_EQ(3, completeConfig.archiveFlushBytesFiles.value().maxBytes);
  ASSERT_EQ(4, completeConfig.archiveFlushBytesFiles.value().maxFiles);
  ASSERT_EQ(5, completeConfig.retrieveFetchBytesFiles.value().maxBytes);
  ASSERT_EQ(6, completeConfig.retrieveFetchBytesFiles.value().maxFiles);
  ASSERT_EQ(3, completeConfig.driveConfigs.size());
  ASSERT_EQ("/dev/tape1", completeConfig.driveConfigs.at("drive1").value().devFilename);
}

} // namespace unitTests
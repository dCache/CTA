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

#include "castor/acs/AcsQueryVolumeCmdLine.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/MissingOperand.hpp"

#include <gtest/gtest.h>
#include <list>
#include <memory>
#include <sstream>
#include <string.h>

namespace unitTests {

class castor_acs_AcsQueryVolumeCmdLineTest: public ::testing::Test {
protected:

  struct Argcv {
    int argc;
    char **argv;
    Argcv(): argc(0), argv(NULL) {
    }
  };
  typedef std::list<Argcv*> ArgcvList;
  ArgcvList m_argsList;

  /**
   * Creates a duplicate string usin the new operator.
   */
  char *dupString(const char *str) {
    const size_t len = strlen(str);
    char *duplicate = new char[len+1];
    strncpy(duplicate, str, len);
    duplicate[len] = '\0';
    return duplicate;
  }

  virtual void SetUp() {
  }

  virtual void TearDown() {
    // Allow getopt_long to be called again
    optind = 0;

    for(ArgcvList::const_iterator itor = m_argsList.begin();
      itor != m_argsList.end(); itor++) {
      for(int i=0; i < (*itor)->argc; i++) {
        delete[] (*itor)->argv[i];
      }
      delete[] (*itor)->argv;
      delete *itor;
    }
  }
}; // class castor_acs_AcsQueryVolumeCmdLineTest

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, constructor) {
  const castor::acs::AcsQueryVolumeCmdLine cmdLine;
  ASSERT_EQ((BOOLEAN)FALSE, cmdLine.debug);
  ASSERT_EQ((BOOLEAN)FALSE, cmdLine.help);
  ASSERT_EQ(0, cmdLine.timeout);
  ASSERT_EQ(0, cmdLine.queryInterval);
  ASSERT_EQ('\0', cmdLine.volId.external_label[0]);
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithNoArgs) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 1;
  args->argv = new char *[2];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_THROW(AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout),
    castor::exception::MissingOperand);
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithValidVolId) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 2;
  args->argv = new char *[3];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("VIDVID");
  args->argv[2] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ((BOOLEAN)FALSE, cmdLine.debug);
  ASSERT_EQ((BOOLEAN)FALSE, cmdLine.help);
  ASSERT_EQ(defaultQueryInterval, cmdLine.queryInterval);
  ASSERT_EQ(defaultTimeout, cmdLine.timeout);
  ASSERT_EQ(std::string("VIDVID"), std::string(cmdLine.volId.external_label));
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithShortHelp) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 2;
  args->argv = new char *[3];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("-h");
  args->argv[2] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ((BOOLEAN)TRUE, cmdLine.help);
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithLongHelp) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 2;
  args->argv = new char *[3];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("--help");
  args->argv[2] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ((BOOLEAN)TRUE, cmdLine.help);
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithShortDebug) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 3;
  args->argv = new char *[4];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("-d");
  args->argv[2] = dupString("VIDVID");
  args->argv[3] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ((BOOLEAN)TRUE, cmdLine.debug);
  ASSERT_EQ(std::string("VIDVID"), std::string(cmdLine.volId.external_label));
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithLongDebug) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 3;
  args->argv = new char *[4];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("--debug");
  args->argv[2] = dupString("VIDVID");
  args->argv[3] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ((BOOLEAN)TRUE, cmdLine.debug);
  ASSERT_EQ(std::string("VIDVID"), std::string(cmdLine.volId.external_label));
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithTooLongVolId) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 2;
  args->argv = new char *[3];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("VIDVID7");
  args->argv[2] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_THROW(AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout),
    castor::exception::InvalidArgument);
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithShortTimeout) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 4;
  args->argv = new char *[5];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("-t");
  args->argv[2] = dupString("2");
  args->argv[3] = dupString("VIDVID");
  args->argv[4] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ(2, cmdLine.timeout);
  ASSERT_EQ(std::string("VIDVID"), std::string(cmdLine.volId.external_label));
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithLongTimeout) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 4;
  args->argv = new char *[5];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("--timeout");
  args->argv[2] = dupString("2");
  args->argv[3] = dupString("VIDVID");
  args->argv[4] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ(2, cmdLine.timeout);
  ASSERT_EQ(std::string("VIDVID"), std::string(cmdLine.volId.external_label));
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWith0Timeout) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 4;
  args->argv = new char *[5];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("--timeout");
  args->argv[2] = dupString("0");
  args->argv[3] = dupString("VIDVID");
  args->argv[4] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_THROW(AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout),
    castor::exception::InvalidArgument);
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithShortQuery) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 4;
  args->argv = new char *[5];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("-q");
  args->argv[2] = dupString("1");
  args->argv[3] = dupString("VIDVID");
  args->argv[4] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ(1, cmdLine.queryInterval);
  ASSERT_EQ(std::string("VIDVID"), std::string(cmdLine.volId.external_label));
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWithLongQuery) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 4;
  args->argv = new char *[5];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("--query");
  args->argv[2] = dupString("1");
  args->argv[3] = dupString("VIDVID");
  args->argv[4] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  AcsQueryVolumeCmdLine cmdLine;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_NO_THROW(cmdLine = AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout));
  ASSERT_EQ(1, cmdLine.queryInterval);
  ASSERT_EQ(std::string("VIDVID"), std::string(cmdLine.volId.external_label));
}

TEST_F(castor_acs_AcsQueryVolumeCmdLineTest, parseCmdLineWith0Query) {
  using namespace castor::acs;
  Argcv *args = new Argcv();
  m_argsList.push_back(args);
  args->argc = 4;
  args->argv = new char *[5];
  args->argv[0] = dupString("queryvolume");
  args->argv[1] = dupString("--query");
  args->argv[2] = dupString("0");
  args->argv[3] = dupString("VIDVID");
  args->argv[4] = NULL;

  std::istringstream inStream;
  std::ostringstream outStream;
  std::ostringstream errStream;
  const int defaultQueryInterval = 10;
  const int defaultTimeout = 20;
  ASSERT_THROW(AcsQueryVolumeCmdLine(args->argc, args->argv,
    defaultQueryInterval, defaultTimeout),
    castor::exception::InvalidArgument);
}

} // namespace unitTests

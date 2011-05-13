/******************************************************************************
 *                test/castor/tape/utils/TapeUtilsTest.hpp
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

#ifndef TEST_CASTOR_TAPE_UTILS_TAPEUTILSTEST_HPP
#define TEST_CASTOR_TAPE_UTILS_TAPEUTILSTEST_HPP 1

#include "castor/tape/utils/IndexedContainer.hpp"
#include "castor/tape/utils/utils.hpp"

#include <cppunit/extensions/HelperMacros.h>
#include <list>
#include <stdlib.h>
#include <string>
#include <vector>

class test_exception: public std::exception {
private:
  const std::string m_msg;

  test_exception &operator=(const test_exception &tx) {
  }

public:
  test_exception(std::string msg): m_msg(msg) {
  }

  test_exception(const test_exception &tx) : m_msg(tx.m_msg) {
  }

  ~test_exception() throw() {
  }

  const char* what() const throw() {
    return m_msg.c_str();
  }
};

void readFileIntoList_stdException(const char *const filename,
  std::list<std::string> &lines) {
  try {
    castor::tape::utils::readFileIntoList(filename, lines);
  } catch(castor::exception::Exception &ex) {
    test_exception te(ex.getMessage().str());

    throw te;
  }
}

void appendPathToEnvVar_stdException(const std::string &envVarName,
  const std::string &pathToBeAppended) {
  try {
    castor::tape::utils::appendPathToEnvVar(envVarName, pathToBeAppended);
  } catch(castor::exception::Exception &ex) {
    test_exception te(ex.getMessage().str());

    throw te;
  }
}

class TapeUtilsTest: public CppUnit::TestFixture {
public:

  void setUp() {
  }

  void tearDown() {
  }

  void testIndexedContainer() {
    castor::tape::utils::IndexedContainer<const void *> c(3);

    const void *ptr1 = (const void *)0x12;
    const void *ptr2 = (const void *)0x34;
    const void *ptr3 = (const void *)0x56;

    const int ptr1Index = c.insert(ptr1);
    const int ptr2Index = c.insert(ptr2);
    const int ptr3Index = c.insert(ptr3);

    const void *ptr4 = (const void *)0x78;

    CPPUNIT_ASSERT_THROW_MESSAGE("Adding one too many pointers",
      c.insert(ptr4), castor::exception::OutOfMemory);

    CPPUNIT_ASSERT_THROW_MESSAGE(
      "Removing an element using the invalid index of -1",
      c.remove(-1), castor::exception::InvalidArgument);

    CPPUNIT_ASSERT_THROW_MESSAGE(
      "Removing an element using the invalid index of 3",
      c.remove(3), castor::exception::InvalidArgument);

    const void *removedPtr2 = c.remove(1);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Removing element with index 1", ptr2,
      removedPtr2);

    CPPUNIT_ASSERT_THROW_MESSAGE(
      "Illegally removing the free element with index 1",
      c.remove(1), castor::exception::NoEntry);
  }

  void testToHex() {
    const uint32_t number          = 0xdeadface;
    const char     *expectedResult = "deadface";

    {
      char buf[8];
      CPPUNIT_ASSERT_THROW_MESSAGE(
        "Failed to detect a buffer that is too small",
        castor::tape::utils::toHex(number, buf),
        castor::exception::InvalidArgument);
    }

    {
      char buf[9];
      castor::tape::utils::toHex(number, buf);

      CPPUNIT_ASSERT_MESSAGE(
        "toHex did not give the expected result",
        strcmp(expectedResult, buf) == 0);
    }
  }

  void testParseTpconfig() {
    castor::tape::utils::TpconfigLines lines;

    CPPUNIT_ASSERT_THROW_MESSAGE(
      "Failed to detect invalid path",
      castor::tape::utils::parseTpconfigFile("/SillyPath", lines),
      castor::exception::Exception);

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Failed to parse TPCONFIG",
      castor::tape::utils::parseTpconfigFile(castor::tape::TPCONFIGPATH,
      lines));
  }

  void testCopyStringNullDst() {
    char dummy[6] = "Dummy";

    CPPUNIT_ASSERT_THROW_MESSAGE("NULL dst",
      castor::tape::utils::copyString(NULL, 0, dummy),
      castor::exception::Exception);
  }

  void testCopyStringNullSrc() {
    char dummy[6] = "Dummy";

    CPPUNIT_ASSERT_THROW_MESSAGE("NULL src",
      castor::tape::utils::copyString(dummy, sizeof(dummy), NULL),
      castor::exception::Exception);
  }

  void testCopyString() {
    char src[12]  = "Hello World";
    char dst[12];

    castor::tape::utils::copyString(dst, src);
    CPPUNIT_ASSERT_MESSAGE("Copying \"Hello World\"",
      strcmp(dst, src) == 0);
  }

  void testReadFileIntoList_sevenEntryList() {
    const char *const filename = "sevenEntryList.txt";
    std::list<std::string> lines;

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("readFileIntoList_stdException",
      readFileIntoList_stdException(filename, lines));

    size_t expectedNbLines = 7;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("number of lines",
      expectedNbLines, lines.size());

    std::list<std::string>::iterator itor = lines.begin();
    CPPUNIT_ASSERT_MESSAGE("First line"   , *(itor++) == "First line"  );
    CPPUNIT_ASSERT_MESSAGE("Second line"  , *(itor++) == "Second line" );
    CPPUNIT_ASSERT_MESSAGE("Third line"   , *(itor++) == "Third line"  );
    CPPUNIT_ASSERT_MESSAGE("Fourth line"  , *(itor++) == "Fourth line" );
    CPPUNIT_ASSERT_MESSAGE("Fifth line"   , *(itor++) == "Fifth line"  );
    CPPUNIT_ASSERT_MESSAGE("Sixth line"   , *(itor++) == "Sixth line"  );
    CPPUNIT_ASSERT_MESSAGE("Seventh line" , *(itor++) == "Seventh line");
    CPPUNIT_ASSERT_MESSAGE("No more lines", itor == lines.end());
  }

  void testReadFileIntoList_eightEntryListLastThreeEmpty() {
    const char *const filename = "eightEntryListLastThreeEmpty.txt";
    std::list<std::string> lines;

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("readFileIntoList_stdException",
      readFileIntoList_stdException(filename, lines));

    size_t expectedNbLines = 8;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("number of lines",
      expectedNbLines, lines.size());

    std::list<std::string>::iterator itor = lines.begin();
    CPPUNIT_ASSERT_MESSAGE("First line"   , *(itor++) == "First line" );
    CPPUNIT_ASSERT_MESSAGE("Second line"  , *(itor++) == "Second line");
    CPPUNIT_ASSERT_MESSAGE("Third line"   , *(itor++) == "Third line" );
    CPPUNIT_ASSERT_MESSAGE("Fourth line"  , *(itor++) == "Fourth line");
    CPPUNIT_ASSERT_MESSAGE("Fifth line"   , *(itor++) == "Fifth line" );
    CPPUNIT_ASSERT_MESSAGE("Sixth line"   , *(itor++) == ""           );
    CPPUNIT_ASSERT_MESSAGE("Seventh line" , *(itor++) == ""           );
    CPPUNIT_ASSERT_MESSAGE("Eighth line"  , *(itor++) == ""           );
    CPPUNIT_ASSERT_MESSAGE("No more lines", itor == lines.end()       );
  }

  void testReadFileIntoList_twelveEntryListThirdAndFourthEmpty() {
    const char *const filename = "twelveEntryListThirdAndFourthEmpty.txt";
    std::list<std::string> lines;

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("readFileIntoList_stdException",
      readFileIntoList_stdException(filename, lines));

    size_t expectedNbLines = 12;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("number of lines",
      expectedNbLines, lines.size());

    std::list<std::string>::iterator itor = lines.begin();
    CPPUNIT_ASSERT_MESSAGE("First line"   , *(itor++) == "First line"   );
    CPPUNIT_ASSERT_MESSAGE("Second line"  , *(itor++) == "Second line"  );
    CPPUNIT_ASSERT_MESSAGE("Third line"   , *(itor++) == ""             );
    CPPUNIT_ASSERT_MESSAGE("Fourth line"  , *(itor++) == ""             );
    CPPUNIT_ASSERT_MESSAGE("Fifth line"   , *(itor++) == "Fifth line"   );
    CPPUNIT_ASSERT_MESSAGE("Sixth line"   , *(itor++) == "Sixth line"   );
    CPPUNIT_ASSERT_MESSAGE("Seventh line" , *(itor++) == "Seventh line" );
    CPPUNIT_ASSERT_MESSAGE("Eighth line"  , *(itor++) == "Eighth line"  );
    CPPUNIT_ASSERT_MESSAGE("Nineth line"  , *(itor++) == "Nineth line"  );
    CPPUNIT_ASSERT_MESSAGE("Tenth line"   , *(itor++) == "Tenth line"   );
    CPPUNIT_ASSERT_MESSAGE("Eleventh line", *(itor++) == "Eleventh line");
    CPPUNIT_ASSERT_MESSAGE("Twelfth line" , *(itor++) == "Twelfth line" );
    CPPUNIT_ASSERT_MESSAGE("No more lines", itor == lines.end()         );
  }

  void testReadFileIntoList_twelveEntryListFifthAndTenthEmpty() {
    const char *const filename = "twelveEntryListFifthAndTenthEmpty.txt";
    std::list<std::string> lines;

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("readFileIntoList_stdException",
      readFileIntoList_stdException(filename, lines));

    size_t expectedNbLines = 12;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("number of lines",
      expectedNbLines, lines.size());

    std::list<std::string>::iterator itor = lines.begin();
    CPPUNIT_ASSERT_MESSAGE("First line"   , *(itor++) == "First line"   );
    CPPUNIT_ASSERT_MESSAGE("Second line"  , *(itor++) == "Second line"  );
    CPPUNIT_ASSERT_MESSAGE("Third line"   , *(itor++) == "Third line"   );
    CPPUNIT_ASSERT_MESSAGE("Fourth line"  , *(itor++) == "Fourth line"  );
    CPPUNIT_ASSERT_MESSAGE("Fifth line"   , *(itor++) == ""             );
    CPPUNIT_ASSERT_MESSAGE("Sixth line"   , *(itor++) == "Sixth line"   );
    CPPUNIT_ASSERT_MESSAGE("Seventh line" , *(itor++) == "Seventh line" );
    CPPUNIT_ASSERT_MESSAGE("Eighth line"  , *(itor++) == "Eighth line"  );
    CPPUNIT_ASSERT_MESSAGE("Nineth line"  , *(itor++) == "Nineth line"  );
    CPPUNIT_ASSERT_MESSAGE("Tenth line"   , *(itor++) == ""             );
    CPPUNIT_ASSERT_MESSAGE("Eleventh line", *(itor++) == "Eleventh line");
    CPPUNIT_ASSERT_MESSAGE("Twelfth line" , *(itor++) == "Twelfth line" );
    CPPUNIT_ASSERT_MESSAGE("No more lines", itor == lines.end()         );
  }

  void testAppendPathToUnsetEnvVar() {
    const std::string envVarName       = "TESTPATH";
    const std::string pathToBeAppended = "/testing/testing/1/2/3";
    const char        *getenvResult    = NULL;

    CPPUNIT_ASSERT_EQUAL_MESSAGE("unsetenv",
      0, unsetenv(envVarName.c_str()));

    CPPUNIT_ASSERT_EQUAL_MESSAGE("getenv of unset variable",
      (char *)0, getenv(envVarName.c_str()));

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("appendPathToEnvVar_stdException",
      appendPathToEnvVar_stdException(envVarName, pathToBeAppended));

    getenvResult = getenv(envVarName.c_str());
    CPPUNIT_ASSERT_MESSAGE("getenv", NULL != getenvResult);
    CPPUNIT_ASSERT_MESSAGE("path appended", pathToBeAppended == getenvResult);
  }

  void testAppendPathToEmptyEnvVar() {
    const std::string envVarName       = "TESTPATH";
    const std::string pathToBeAppended = "/testing/testing/4/5/6";
    const char        *getenvResult    = NULL;

    const int overwrite = 1;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("setenv variable  to empty string",
      0, setenv(envVarName.c_str(), "", overwrite));

    getenvResult = getenv(envVarName.c_str());
    CPPUNIT_ASSERT_MESSAGE("getenv", NULL != getenvResult);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("getenv value is empty string",
      '\0', *getenvResult);

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("appendPathToEnvVar_stdException",
      appendPathToEnvVar_stdException(envVarName, pathToBeAppended));

    getenvResult = getenv(envVarName.c_str());
    CPPUNIT_ASSERT_MESSAGE("getenv", NULL != getenvResult);
    CPPUNIT_ASSERT_MESSAGE("path appended", pathToBeAppended == getenvResult);
  }

  void testAppendPathToNonEmptyEnvVar() {
    const std::string envVarName       = "TESTPATH";
    const std::string firstPath        = "/the/first/path";
    const std::string pathToBeAppended = "/testing/testing/4/5/6";
    const std::string combinedPaths    = firstPath + ":" + pathToBeAppended;
    const char        *getenvResult    = NULL;

    const int overwrite = 1;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("setenv",
      0, setenv(envVarName.c_str(), firstPath.c_str(), overwrite));

    getenvResult = getenv(envVarName.c_str());
    CPPUNIT_ASSERT_MESSAGE("getenv", NULL != getenvResult);

    CPPUNIT_ASSERT_MESSAGE("getenv value is first path",
      firstPath == getenvResult);

    CPPUNIT_ASSERT_NO_THROW_MESSAGE("appendPathToEnvVar_stdException",
      appendPathToEnvVar_stdException(envVarName, pathToBeAppended));

    getenvResult = getenv(envVarName.c_str());
    CPPUNIT_ASSERT_MESSAGE("getenv", NULL != getenvResult);
    CPPUNIT_ASSERT_MESSAGE("path appended", combinedPaths == getenvResult);
  }

  CPPUNIT_TEST_SUITE(TapeUtilsTest);
  CPPUNIT_TEST(testIndexedContainer);
  CPPUNIT_TEST(testToHex);
  CPPUNIT_TEST(testCopyStringNullDst);
  CPPUNIT_TEST(testCopyStringNullSrc);
  CPPUNIT_TEST(testCopyString);
  CPPUNIT_TEST(testReadFileIntoList_sevenEntryList);
  CPPUNIT_TEST(testReadFileIntoList_eightEntryListLastThreeEmpty);
  CPPUNIT_TEST(testReadFileIntoList_twelveEntryListThirdAndFourthEmpty);
  CPPUNIT_TEST(testReadFileIntoList_twelveEntryListFifthAndTenthEmpty);
  CPPUNIT_TEST(testAppendPathToUnsetEnvVar);
  CPPUNIT_TEST(testAppendPathToEmptyEnvVar);
  CPPUNIT_TEST(testAppendPathToNonEmptyEnvVar);
  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TapeUtilsTest);

#endif // TEST_CASTOR_TAPE_UTILS_TAPEUTILSTEST_HPP

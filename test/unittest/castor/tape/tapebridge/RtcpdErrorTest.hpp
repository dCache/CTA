/******************************************************************************
 *    test/unittest/castor/tape/tapebridge/RtcpdErrorTest.hpp
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

#ifndef TEST_UNITTEST_CASTOR_TAPE_TAPEBRIDGE_RTCPDERRORTEST_HPP
#define TEST_UNITTEST_CASTOR_TAPE_TAPEBRIDGE_RTCPDERRORTEST_HPP 1

#include "castor/tape/tapebridge/RtcpdError.hpp"
#include "h/serrno.h"

#include <cppunit/extensions/HelperMacros.h>
#include <exception>
#include <stdint.h>
#include <stdlib.h>

class RtcpdErrorTest: public CppUnit::TestFixture {
public:

  RtcpdErrorTest() {
  }

  void setUp() {
  }

  void tearDown() {
  }

  void testConstructor() {
    castor::tape::tapebridge::RtcpdError rtcpdError;

    CPPUNIT_ASSERT_EQUAL(
      castor::tape::tapebridge::RtcpdError::UNKNOWN_SCOPE,
      rtcpdError.getErrorScope());

    CPPUNIT_ASSERT_EQUAL(
      SEINTERNAL,
      rtcpdError.getErrorCode());

    CPPUNIT_ASSERT_EQUAL(
      std::string("UNKNOWN"),
      rtcpdError.getErrorMessage());

    CPPUNIT_ASSERT_EQUAL(
      (uint64_t)0,
      rtcpdError.getFileTransactionId());

    CPPUNIT_ASSERT_EQUAL(
      std::string("UNKNOWN"),
      rtcpdError.getNsHost());
  
    CPPUNIT_ASSERT_EQUAL(
      (uint64_t)0,
      rtcpdError.getNsFileId());
  
    CPPUNIT_ASSERT_EQUAL(
      (int32_t)0,
      rtcpdError.getTapeFSeq());
  }

  void testAttributes() {
    castor::tape::tapebridge::RtcpdError rtcpdError;

    rtcpdError.setErrorScope(castor::tape::tapebridge::RtcpdError::FILE_SCOPE);
    CPPUNIT_ASSERT_EQUAL(
      castor::tape::tapebridge::RtcpdError::FILE_SCOPE,
      rtcpdError.getErrorScope());

    rtcpdError.setErrorScope(
      castor::tape::tapebridge::RtcpdError::SESSION_SCOPE);
    CPPUNIT_ASSERT_EQUAL(
      castor::tape::tapebridge::RtcpdError::SESSION_SCOPE,
      rtcpdError.getErrorScope());

    rtcpdError.setErrorScope(
      castor::tape::tapebridge::RtcpdError::UNKNOWN_SCOPE);
    CPPUNIT_ASSERT_EQUAL(
      castor::tape::tapebridge::RtcpdError::UNKNOWN_SCOPE,
      rtcpdError.getErrorScope());

    {
      const castor::tape::tapebridge::RtcpdError::ErrorScope crazyScopeValue =
          (castor::tape::tapebridge::RtcpdError::ErrorScope)12345;

      CPPUNIT_ASSERT_THROW(
        rtcpdError.setErrorScope(crazyScopeValue),
        castor::exception::InvalidArgument);
    }

    {
      const int errorCode = 1;
      rtcpdError.setErrorCode(errorCode);
      CPPUNIT_ASSERT_EQUAL(
        errorCode,
        rtcpdError.getErrorCode());
    }

    {
      const std::string errorMessage("Two");
      rtcpdError.setErrorMessage(errorMessage);
      CPPUNIT_ASSERT_EQUAL(
        errorMessage,
        rtcpdError.getErrorMessage());
    }

    {
      const uint64_t fileTransactionId = 3;
      rtcpdError.setFileTransactionId(fileTransactionId);
      CPPUNIT_ASSERT_EQUAL(
        fileTransactionId,
        rtcpdError.getFileTransactionId());
    }

    {
      const std::string nsHost("Four");
      rtcpdError.setNsHost(nsHost);
      CPPUNIT_ASSERT_EQUAL(
        nsHost,
        rtcpdError.getNsHost());
    }

    {
      const uint64_t nsFileId = 5;
      rtcpdError.setNsFileId(nsFileId);
      CPPUNIT_ASSERT_EQUAL(
        nsFileId,
        rtcpdError.getNsFileId());
    }
  
    {
      const int32_t tapeFSeq = 6;
      rtcpdError.setTapeFSeq(tapeFSeq);
      CPPUNIT_ASSERT_EQUAL(
        tapeFSeq,
        rtcpdError.getTapeFSeq());
    }
  }

  CPPUNIT_TEST_SUITE(RtcpdErrorTest);

  CPPUNIT_TEST(testConstructor);
  CPPUNIT_TEST(testAttributes);

  CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(RtcpdErrorTest);

#endif // TEST_UNITTEST_CASTOR_TAPE_TAPEBRIDGE_RTCPDERRORTEST_HPP

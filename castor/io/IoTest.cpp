/******************************************************************************
 *                castor/io/IoTest.cpp
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

#include "castor/io/io.hpp"
#include "castor/utils/SmartFd.hpp"
#include "h/marshall.h"

#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace unitTests {

class castor_io_IoTest: public ::testing::Test {
protected:

  const char *const m_listenSockPath;

  castor_io_IoTest(): m_listenSockPath("/tmp/listenSockForCastorIoTest") {
    // Do nothing
  }

  void setUp() {
    unlink(m_listenSockPath);
  }

  void tearDown() {
    unlink(m_listenSockPath);
  }

  int createLocalListenSocket(const char *const listenSockPath)
    throw (castor::exception::Exception) {

    // Delete the file to be used for the socket if the file already exists
    unlink(listenSockPath);

    // Create the socket
    castor::utils::SmartFd smartListenSock(socket(PF_LOCAL, SOCK_STREAM, 0));
    if(-1 == smartListenSock.get()) {
      char strErrBuf[256];
      if(0 != strerror_r(errno, strErrBuf, sizeof(strErrBuf))) {
        memset(strErrBuf, '\0', sizeof(strErrBuf));
        strncpy(strErrBuf, "Unknown", sizeof(strErrBuf) - 1);
      }

      std::string errorMessage("Call to socket() failed: ");
      errorMessage += strErrBuf;

      castor::exception::Exception ex(ECANCELED);
      ex.getMessage() << errorMessage;
      throw ex;
    }

    // Bind the socket
    {
      struct sockaddr_un listenAddr;
      memset(&listenAddr, '\0', sizeof(listenAddr));
      listenAddr.sun_family = PF_LOCAL;
      strncpy(listenAddr.sun_path, listenSockPath,
        sizeof(listenAddr.sun_path) - 1);

      if(0 != bind(smartListenSock.get(), (const struct sockaddr *)&listenAddr,
        sizeof(listenAddr))) {
        char strErrBuf[256];
        if(0 != strerror_r(errno, strErrBuf, sizeof(strErrBuf))) {
          memset(strErrBuf, '\0', sizeof(strErrBuf));
          strncpy(strErrBuf, "Unknown", sizeof(strErrBuf) - 1);
        }

        std::string errorMessage("Call to bind() failed: ");
        errorMessage += strErrBuf;

        castor::exception::Exception ex(ECANCELED);
        ex.getMessage() << errorMessage;
        throw ex;
      }
    }

    // Make the socket listen
    {
      const int backlog = 128;
      if(0 != listen(smartListenSock.get(), backlog)) {
        char strErrBuf[256];
        if(0 != strerror_r(errno, strErrBuf, sizeof(strErrBuf))) {
          memset(strErrBuf, '\0', sizeof(strErrBuf));
          strncpy(strErrBuf, "Unknown", sizeof(strErrBuf) - 1);
        }

        std::string errorMessage("Call to listen() failed: ");
        errorMessage += strErrBuf;

        castor::exception::Exception ex(ECANCELED);
        ex.getMessage() << errorMessage;
        throw ex;
      }
    }

    return smartListenSock.release();
  }

}; // class castor_io_IoTest

TEST_F(castor_io_IoTest, connectWithTimeout) {
  castor::utils::SmartFd smartListenSock;
  ASSERT_NO_THROW(smartListenSock.reset(
    createLocalListenSocket(m_listenSockPath)));

  castor::utils::SmartFd smartClientConnectionSock;
  {
    const int             sockDomain   = PF_LOCAL;
    const int             sockType     = SOCK_STREAM;
    const int             sockProtocol = 0;
    struct sockaddr_un    address;
    const socklen_t       address_len  = sizeof(address);
    const int             timeout      = 10; // 10 seconds

    memset(&address, '\0', sizeof(address));
    address.sun_family = PF_LOCAL;
    strncpy(address.sun_path, m_listenSockPath, sizeof(address.sun_path) - 1);
    ASSERT_NO_THROW(
      smartClientConnectionSock.reset(castor::io::connectWithTimeout(
        sockDomain,
        sockType,
        sockProtocol,
        (const struct sockaddr *)&address,
        address_len,
        timeout)));
  }

  castor::utils::SmartFd smartServerConnectionSock;
  {
    const time_t acceptTimeout = 10; // Timeout is in seconds
    ASSERT_NO_THROW(smartServerConnectionSock.reset(
      castor::io::acceptConnection(smartListenSock.get(), acceptTimeout)));
  }

  // Send message from client to server and check it was sent
  char clientToServerMessageBuf[] =
    "Test message from client to server: HELLO";
  const size_t clientToServerMessageBufLen = sizeof(clientToServerMessageBuf);
  {
    const int timeout = 1; // Time out is in seconds
    ASSERT_NO_THROW(castor::io::writeBytes(smartClientConnectionSock.get(),
      timeout, clientToServerMessageBufLen, clientToServerMessageBuf));
  }
  {
    const int timeout = 1; // Time out is in seconds
    char serverInputBuf[clientToServerMessageBufLen];
    memset(serverInputBuf, '\0', sizeof(serverInputBuf));
    ASSERT_NO_THROW(castor::io::readBytes(smartServerConnectionSock.get(),
      timeout, clientToServerMessageBufLen, serverInputBuf));

    ASSERT_EQ(std::string(clientToServerMessageBuf),
      std::string(serverInputBuf));
  }

  // Send message from server to client and check it was sent
  char serverToClientMessageBuf[] =
    "Test message from server to client: BONJOUR";
  const size_t serverToClientMessageBufLen = sizeof(serverToClientMessageBuf);
  {
    const int timeout = 1; // Time out is in seconds
    ASSERT_NO_THROW(castor::io::writeBytes(smartServerConnectionSock.get(),
      timeout, serverToClientMessageBufLen, serverToClientMessageBuf));
  }
  {
    const int timeout = 1; // Time out is in seconds
    char clientInputBuf[serverToClientMessageBufLen];
    memset(clientInputBuf, '\0', sizeof(clientInputBuf));
    ASSERT_NO_THROW(castor::io::readBytes(smartClientConnectionSock.get(),
      timeout, serverToClientMessageBufLen, clientInputBuf));

    ASSERT_EQ( std::string(serverToClientMessageBuf),
      std::string(clientInputBuf));
  }
}

TEST_F(castor_io_IoTest, marshalUint8) {
  const uint8_t v = 0x87;
  char buf[1];
  char *ptr = buf;
    
  memset(buf, '\0', sizeof(buf));
    
  ASSERT_NO_THROW(castor::io::marshalUint8(v, ptr));
  ASSERT_EQ(buf+1, ptr);
  ASSERT_EQ(0x87 & 0xFF, buf[0] & 0xFF);
}

TEST_F(castor_io_IoTest, marshall_BYTE) {
  const uint8_t v = 0x87;
  char buf[1];
  char *ptr = buf;

  memset(buf, '\0', sizeof(buf));

  marshall_BYTE(ptr, v);
  ASSERT_EQ(buf+1, ptr);
  ASSERT_EQ(0x87 & 0xFF, buf[0] & 0xFF);
}

static void check16BitsWereMarshalledBigEndian(const char *const buf) {
  ASSERT_EQ(0x87 & 0xFF, buf[0] & 0xFF);
  ASSERT_EQ(0x65 & 0xFF, buf[1] & 0xFF);
}

TEST_F(castor_io_IoTest, marshalInt16) {
  const int16_t v = 0x8765;
  char buf[2];
  char *ptr = buf;

  memset(buf, '\0', sizeof(buf));

  ASSERT_NO_THROW(castor::io::marshalInt16(v, ptr));
  ASSERT_EQ(buf+2, ptr);
  check16BitsWereMarshalledBigEndian(buf);
}

TEST_F(castor_io_IoTest, marshalUint16) {
  const uint16_t v = 0x8765;
  char buf[2];
  char *ptr = buf;

  memset(buf, '\0', sizeof(buf));

  ASSERT_NO_THROW(castor::io::marshalUint16(v, ptr));
  ASSERT_EQ(buf+2, ptr);
  check16BitsWereMarshalledBigEndian(buf);
}

TEST_F(castor_io_IoTest, marshall_SHORT) {
  const uint16_t v = 0x8765;
  char buf[2];
  char *ptr = buf;

  memset(buf, '\0', sizeof(buf));

  marshall_SHORT(ptr, v);
  ASSERT_EQ(buf+2, ptr);
  check16BitsWereMarshalledBigEndian(buf);
}

static void check32BitsWereMarshalledBigEndian(const char *const buf) {
  ASSERT_EQ(0x87 & 0xFF, buf[0] & 0xFF);
  ASSERT_EQ(0x65 & 0xFF, buf[1] & 0xFF);
  ASSERT_EQ(0x43 & 0xFF, buf[2] & 0xFF);
  ASSERT_EQ(0x21 & 0xFF, buf[3] & 0xFF);
}

TEST_F(castor_io_IoTest, marshalUint32) {
  const uint32_t v = 0x87654321;
  char buf[4];
  char *ptr = buf;

  memset(buf, '\0', sizeof(buf));

  ASSERT_NO_THROW(castor::io::marshalUint32(v, ptr));
  ASSERT_EQ(buf+4, ptr);
  check32BitsWereMarshalledBigEndian(buf);
}

TEST_F(castor_io_IoTest, marshall_LONG) {
  const uint32_t v = 0x87654321;
  char buf[4];
  char *ptr = buf;

  memset(buf, '\0', sizeof(buf));

  marshall_LONG(ptr, v);
  ASSERT_EQ(buf+4, ptr);
  check32BitsWereMarshalledBigEndian(buf);
}

static void check64BitsWereMarshalledBigEndian(const char *const buf) {
  ASSERT_EQ(0x88 & 0xFF, buf[0] & 0xFF);
  ASSERT_EQ(0x77 & 0xFF, buf[1] & 0xFF);
  ASSERT_EQ(0x66 & 0xFF, buf[2] & 0xFF);
  ASSERT_EQ(0x55 & 0xFF, buf[3] & 0xFF);
  ASSERT_EQ(0x44 & 0xFF, buf[4] & 0xFF);
  ASSERT_EQ(0x33 & 0xFF, buf[5] & 0xFF);
  ASSERT_EQ(0x22 & 0xFF, buf[6] & 0xFF);
  ASSERT_EQ(0x11 & 0xFF, buf[7] & 0xFF);
}

TEST_F(castor_io_IoTest, marshalUint64) {
  const uint64_t v = 0x8877665544332211LL;
  char buf[8];
  char *ptr = buf;
    
  memset(buf, '\0', sizeof(buf));
    
  ASSERT_NO_THROW(castor::io::marshalUint64(v, ptr));
  ASSERT_EQ(buf+8, ptr);
  check64BitsWereMarshalledBigEndian(buf);
}

// The following test MUST call check64BitsWereMarshalledBigEndian() like
// the marshalUint64 test above in order to prove that the new C++
// marshalling code of castor::io is compatible with that of the legacy
// code found in h/mashall.h
TEST_F(castor_io_IoTest, marshall_HYPER) {
  const uint64_t v = 0x8877665544332211LL;
  char buf[8];
  char *ptr = buf;

  memset(buf, '\0', sizeof(buf));

  marshall_HYPER(ptr, v);
  ASSERT_EQ(buf+8, ptr);
  check64BitsWereMarshalledBigEndian(buf);
}

static void checkStringWasMarshalled(const char *const buf) {
  ASSERT_EQ('V', buf[0]);
  ASSERT_EQ('a', buf[1]);
  ASSERT_EQ('l', buf[2]);
  ASSERT_EQ('u', buf[3]);
  ASSERT_EQ('e', buf[4]);
  ASSERT_EQ('\0', buf[5]);
  ASSERT_EQ('E', buf[6]);
  ASSERT_EQ('E', buf[7]);
}

TEST_F(castor_io_IoTest, marshalString) {
  const char *const v = "Value";
  char buf[8];
  char *ptr = buf;

  memset(buf, 'E', sizeof(buf));

  ASSERT_NO_THROW(castor::io::marshalString(v, ptr));
  ASSERT_EQ(buf+6, ptr);
  checkStringWasMarshalled(buf);
}

TEST_F(castor_io_IoTest, marshall_STRING) {
  const char *const v = "Value";
  char buf[8];
  char *ptr = buf;

  memset(buf, 'E', sizeof(buf));

  marshall_STRING(ptr, v);
  ASSERT_EQ(buf+6, ptr);
  checkStringWasMarshalled(buf);
}

TEST_F(castor_io_IoTest, unmarshalUint8) {
  char buf[] = {0x87};
  size_t bufLen = sizeof(buf);
  const char *ptr = buf;
  uint8_t v = 0;
  ASSERT_NO_THROW(castor::io::unmarshalUint8(ptr, bufLen, v));
  ASSERT_EQ(buf+1, ptr);
  ASSERT_EQ((size_t)0, bufLen);
  ASSERT_EQ(0x87, v);
}

TEST_F(castor_io_IoTest, unmarshall_BYTE) {
  char buf[] = {0x87};
  const char *ptr = buf;
  uint8_t v = 0;
  unmarshall_BYTE(ptr, v);
  ASSERT_EQ(buf+1, ptr);
  ASSERT_EQ(0x87, v);
}

TEST_F(castor_io_IoTest, unmarshalInt16) {
  char buf[] = {0x87, 0x65};
  size_t bufLen = sizeof(buf);
  const char *ptr = buf;
  int16_t v = 0;
  ASSERT_NO_THROW(castor::io::unmarshalInt16(ptr, bufLen, v));
  ASSERT_EQ(buf+2, ptr);
  ASSERT_EQ((size_t)0, bufLen);
  ASSERT_EQ((int16_t)0x8765, v);
}

TEST_F(castor_io_IoTest, unmarshalUint16) {
  char buf[] = {0x87, 0x65};
  size_t bufLen = sizeof(buf);
  const char *ptr = buf;
  uint16_t v = 0;
  ASSERT_NO_THROW(castor::io::unmarshalUint16(ptr, bufLen, v));
  ASSERT_EQ(buf+2, ptr);
  ASSERT_EQ((size_t)0, bufLen);
  ASSERT_EQ((uint16_t)0x8765, v);
}

TEST_F(castor_io_IoTest, unmarshall_SHORT) {
  char buf[] = {0x87, 0x65};
  const char *ptr = buf;
  uint16_t v = 0;
  unmarshall_SHORT(ptr, v);
  ASSERT_EQ(buf+2, ptr);
  ASSERT_EQ((uint16_t)0x8765, v);
}

TEST_F(castor_io_IoTest, unmarshalUint32) {
  char buf[] = {0x87, 0x65, 0x43, 0x21};
  size_t bufLen = sizeof(buf);
  const char *ptr = buf;
  uint32_t v = 0;
  ASSERT_NO_THROW(castor::io::unmarshalUint32(ptr, bufLen, v));
  ASSERT_EQ(buf+4, ptr);
  ASSERT_EQ((size_t)0, bufLen);
  ASSERT_EQ((uint32_t)0x87654321, v);
}

TEST_F(castor_io_IoTest, unmarshall_LONG) {
  char buf[] = {0x87, 0x65, 0x43, 0x21};
  const char *ptr = buf;
  uint32_t v = 0;
  unmarshall_LONG(ptr, v);
  ASSERT_EQ(buf+4, ptr);
  ASSERT_EQ((uint32_t)0x87654321, v);
}

TEST_F(castor_io_IoTest, unmarshalInt32) {
  char buf[] = {0x87, 0x65, 0x43, 0x21};
  size_t bufLen = sizeof(buf);
  const char *ptr = buf;
  int32_t v = 0;
  ASSERT_NO_THROW(castor::io::unmarshalInt32(ptr, bufLen, v));
  ASSERT_EQ(buf+4, ptr);
  ASSERT_EQ((size_t)0, bufLen);
  ASSERT_EQ((int32_t)0x87654321, v);
}

TEST_F(castor_io_IoTest, unmarshalUint64) {
  char buf[] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
  size_t bufLen = sizeof(buf);
  const char *ptr = buf;
  uint64_t v = 0;
  ASSERT_NO_THROW(castor::io::unmarshalUint64(ptr, bufLen, v));
  ASSERT_EQ(buf+8, ptr);
  ASSERT_EQ((size_t)0, bufLen);
  ASSERT_EQ((uint64_t)0x8877665544332211LL, v);
}

// The following test MUST be the same as the unmarshalUint64 test above in
// order to prove that the new C++ un-marshalling code of castor::io is
// compatible with that of the legacy code found in h/mashall.h
TEST_F(castor_io_IoTest, unmarshall_HYPER) {
  char buf[] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
  const char *ptr = buf;
  uint64_t v = 0;
  unmarshall_HYPER(ptr, v);
  ASSERT_EQ(buf+8, ptr);
  ASSERT_EQ((uint64_t)0x8877665544332211LL, v);
}

TEST_F(castor_io_IoTest, unmarshalString) {
  char src[] = {'V', 'a', 'l', 'u', 'e', '\0', 'E', 'E'};
  size_t srcLen = sizeof(src);
  const char *srcPtr = src;
  char dst[6];
  const size_t dstLen = sizeof(dst);

  ASSERT_NO_THROW(castor::io::unmarshalString(srcPtr, srcLen, dst, dstLen));
  ASSERT_EQ(src+6, srcPtr);
  ASSERT_EQ((size_t)2, srcLen);
  ASSERT_EQ(std::string("Value"), std::string(dst));
}

TEST_F(castor_io_IoTest, unmarshall_STRING) {
  char src[] = {'V', 'a', 'l', 'u', 'e', '\0', 'E', 'E'};
  const char *srcPtr = src;
  char dst[6];

  unmarshall_STRING(srcPtr, dst);
  ASSERT_EQ(src+6, srcPtr);
  ASSERT_EQ(std::string("Value"), std::string(dst));
}

} // namespace unitTests

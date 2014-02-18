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

} // namespace unitTests

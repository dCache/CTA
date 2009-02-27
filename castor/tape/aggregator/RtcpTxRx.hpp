/******************************************************************************
 *                castor/tape/aggregator/RtcpTxRx.hpp
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
 * @author Nicola.Bessone@cern.ch Steven.Murray@cern.ch
 *****************************************************************************/

#ifndef CASTOR_TAPE_AGGREGATOR_RTCPTXRX_HPP
#define CASTOR_TAPE_AGGREGATOR_RTCPTXRX_HPP 1

#include "castor/exception/Exception.hpp"
#include "castor/io/ClientSocket.hpp"
#include "castor/tape/aggregator/RtcpAcknowledgeMsg.hpp"
#include "castor/tape/aggregator/RtcpFileRqstErrMsgBody.hpp"
#include "castor/tape/aggregator/RtcpTapeRqstErrMsgBody.hpp"
#include "castor/tape/aggregator/RtcpTapeRqstMsgBody.hpp"

#include <iostream>
#include <stdint.h>


namespace castor     {
namespace tape       {
namespace aggregator {

/**
 * Provides functions for sending and receiving the messages of the RTCOPY
 * protocol, in other words the protocol used between the VDQM and RTCPD and
 * between RTCPClientD and RTCPD.
 */
class RtcpTxRx {

public:

  /**
   * Gets the request information from RTCPD by sending and receiving the
   * necessary messages.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param reply The request structure to be filled with the reply from
   * RTCPD.
   */
  static void getRequestInfoFromRtcpd(const int socketFd,
    const int netReadWriteTimeout, RtcpTapeRqstErrMsgBody &reply)
    throw(castor::exception::Exception);

  /**
   * Gives information about a volume to RTCPD by sending and receiving the
   * necessary messages.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param request The request to be sent to RTCPD.
   * @param reply The structure to be filled with the reply from
   * RTCPD.
   */
  static void giveVolumeToRtcpd(const int socketFd,
    const int netReadWriteTimeout, RtcpTapeRqstErrMsgBody &request)
    throw(castor::exception::Exception);

  /**
   * Sends the specified RTCPD acknowledge message to RTCPD using the
   * specified socket.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param message The message to be sent.
   */
  static void sendRtcpAcknowledge(const int socketFd,
    const int netReadWriteTimeout, const RtcpAcknowledgeMsg &message)
    throw(castor::exception::Exception);

  /**
   * Pings RTCPD using the specified socket.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   */
  static void pingRtcpd(const int socketFd,
    const int netReadWriteTimeout) throw(castor::exception::Exception);

  /**
   * Receives an RTCP job submission message.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param request The request which will be filled with the contents of the
   * received message.
   */
  static void receiveRcpJobRqst(const int socketFd,
    const int netReadWriteTimeout, RcpJobRqstMsgBody &request)
    throw(castor::exception::Exception);

  /**
   * Asks RTCPD to request more work in the future.
   *
   * A request for more work is infact a file list with one special file
   * request which does not contain any file information, but instead contains
   * a flag indicating a request for more work.
   */ 
  static void askRtcpdToRequestMoreWork(const int socketFd,
    const int netReadWriteTimeout, const uint32_t volReqId)
    throw(castor::exception::Exception);

  /**
   * Gives the specified file list of one and only one actual file description
   * to RTCPD by sending and receiving the necessary messages.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * @param filePath The file path.
   * @param tapePath The tape path.
   * @param recordFormat The record format.
   * @param tapeFileId The tape file ID.
   * @param umask The umask of the file.
   * @param requestMoreWork Set to true if RTCPD should be told that there is
   * a possibility of more work in the future.
   * network read and write operations.
   */
  static void giveFileToRtcpd(const int socketFd,
    const int netReadWriteTimeout, const uint32_t volReqId,
    const char *const filePath, const char *const tapePath,
    const char *const recordFormat, const char *const tapeFileId,
    const uint32_t umask) throw(castor::exception::Exception);

  /**
   * Receives a message header.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param request The request which will be filled with the contents of the
   * received message.
   */
  static void receiveRtcpMsgHeader(const int socketFd,
    const int netReadWriteTimeout, MessageHeader &header)
    throw(castor::exception::Exception);

  /**
   * Receives the body of an RTCPD file request with error appendix message
   * from RTCPD.
   *
   * @param socketFd The socket file desscriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param header The message header which has already been received.
   * @param body The body structure which will be filled with the contents of
   * the received message body.
   */
  static void receiveRtcpFileRqstErrBody(const int socketFd,
    const int netReadWriteTimeout, const MessageHeader &header,
    RtcpFileRqstErrMsgBody &body) throw(castor::exception::Exception);

  /**
   * Receives the body of an RTCPD file request without error appendix message
   * from RTCPD.
   *
   * @param socketFd The socket file desscriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param header The message header which has already been received.
   * @param body The body structure which will be filled with the contents of
   * the received message body.
   */
  static void receiveRtcpFileRqstBody(const int socketFd,
    const int netReadWriteTimeout, const MessageHeader &header,
    RtcpFileRqstMsgBody &body) throw(castor::exception::Exception);

  /**
   * Receives the body of an RTCPD tape request with error appendix message
   * from RTCPD.
   *
   * @param socketFd The socket file desscriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param header The message header which has already been received.
   * @param body The body structure which will be filled with the contents of
   * the received message body.
   */
  static void receiveRtcpTapeRqstErrBody(const int socketFd,
    const int netReadWriteTimeout, const MessageHeader &header,
    RtcpTapeRqstErrMsgBody &body) throw(castor::exception::Exception);

  /**
   * Receives the body of an RTCPD tape request without error appendix message
   * from RTCPD.
   *
   * @param socketFd The socket file desscriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param header The message header which has already been received.
   * @param body The body structure which will be filled with the contents of
   * the received message body.
   */
  static void receiveRtcpTapeRqstBody(const int socketFd,
    const int netReadWriteTimeout, const MessageHeader &header,
    RtcpTapeRqstMsgBody &body) throw(castor::exception::Exception);

  /**
   * Inidcates the end of the current file list to RTCPD by sending and
   * receiving the necessary messages.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   */
  static void tellRtcpdEndOfFileList(const int socketFd,
    const int netReadWriteTimeout) throw(castor::exception::Exception);

  /**
   * Send an abort message to RTCPD
   *
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   */
  static void tellRtcpdToAbort(const int socketFd, const int netReadWriteTimeout)
    throw(castor::exception::Exception);


private:

  /**
   * Private constructor to inhibit instances of this class from being
   * instantiated.
   */
  RtcpTxRx() {}

  /**
   * Gives the description of a file to RTCPD by sending and receiving the
   * necessary messages.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param request The request to be sent to RTCPD.
   * @param reply The structure to be filled with the reply from
   * RTCPD.
   */
  static void giveFileToRtcpd(const int socketFd,
    const int netReadWriteTimeout, RtcpFileRqstErrMsgBody &request)
    throw(castor::exception::Exception);

  /**
   * Receives an acknowledge message from RTCPD and returns the status code
   * embedded within the message.
   *
   * @param socketFd The socket file descriptor of the connection with RTCPD.
   * @param netReadWriteTimeout The timeout to be applied when performing
   * network read and write operations.
   * @param message The message structure to be filled with the acknowledge
   * message.
   */
  static void receiveRtcpAcknowledge(const int socketFd,
    const int netReadWriteTimeout, RtcpAcknowledgeMsg &message)
    throw(castor::exception::Exception);

  /**
   * Throws an exception if the expected magic number is not equal to the
   * actual value.
   *
   * @param expected The expected magic number.
   * @param actual The actual magic number.
   * @param function The name of the caller function.
   */
  static void checkMagic(const uint32_t expected, const uint32_t actual,
    const char *function) throw(castor::exception::Exception);

  /**
   * Throws an exception if the expected RTCOPY_MAGIC request type is not equal
   * to the actual value.
   *
   * @param expected The expected request type.
   * @param received The actual request type.
   * @param function The name of the caller function.
   */
  static void checkRtcopyReqType(const uint32_t expected,
    const uint32_t actual, const char *function)
    throw(castor::exception::Exception);

}; // class RtcpTxRx

} // namespace aggregator
} // namespace tape
} // namespace castor

#endif // CASTOR_TAPE_AGGREGATOR_RTCPTXRX_HPP

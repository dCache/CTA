/******************************************************************************
 *                castor/tape/aggregator/VdqmRequestHandlerThread.cpp
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
 * @author Steven Murray Steven.Murray@cern.ch
 *****************************************************************************/

#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/tape/aggregator/AggregatorDlfMessageConstants.hpp"
#include "castor/tape/aggregator/Constants.hpp"
#include "castor/tape/aggregator/Marshaller.hpp"
#include "castor/tape/aggregator/VdqmRequestHandlerThread.hpp"
#include "castor/tape/aggregator/RcpJobSubmitter.hpp"
#include "castor/tape/aggregator/SocketHelper.hpp"
#include "castor/tape/aggregator/exception/RTCPDErrorMessage.hpp"
#include "h/common.h"
#include "h/rtcp_constants.h"
#include "h/vdqm_constants.h"


//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
castor::tape::aggregator::VdqmRequestHandlerThread::VdqmRequestHandlerThread(
  const int rtcpdListenPort) throw () :
  m_rtcpdListenPort(rtcpdListenPort), m_jobQueue(1) {

  // Create the RTCOPY_MAGIC_OLD0 request handler map
  m_rtcopyMagicOld0Handlers[VDQM_CLIENTINFO] =
    &VdqmRequestHandlerThread::handleJobSubmission;

  // Create the magic number map
  m_magicToHandlers[RTCOPY_MAGIC_OLD0] = &m_rtcopyMagicOld0Handlers;
}


//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
castor::tape::aggregator::VdqmRequestHandlerThread::~VdqmRequestHandlerThread()
  throw () {
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
void castor::tape::aggregator::VdqmRequestHandlerThread::init()
  throw() {
}


//-----------------------------------------------------------------------------
// run
//-----------------------------------------------------------------------------
void castor::tape::aggregator::VdqmRequestHandlerThread::run(void *param)
  throw() {
  Cuuid_t cuuid = nullCuuid;

  // Gives a Cuuid to the request
  Cuuid_create(&cuuid);

  if(param == NULL) {
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_VDQM_REQUEST_HANDLER_SOCKET_IS_NULL);
    return;
  }

  castor::io::ServerSocket *socket = (castor::io::ServerSocket*)param;

  try {

    dispatchRequest(cuuid, *socket);

  } catch(castor::exception::Exception &ex) {

    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_HANDLE_VDQM_REQUEST_EXCEPT, 2, params);
  }

  // Close and de-allocate the socket
  socket->close();
  delete socket;
}


//-----------------------------------------------------------------------------
// stop
//-----------------------------------------------------------------------------
void castor::tape::aggregator::VdqmRequestHandlerThread::stop()
  throw() {
}


//-----------------------------------------------------------------------------
// dispatchRequest
//-----------------------------------------------------------------------------
void castor::tape::aggregator::VdqmRequestHandlerThread::dispatchRequest(
  Cuuid_t &cuuid, castor::io::ServerSocket &socket)
  throw(castor::exception::Exception) {

  // Read and unmarshall the magic number of the request
  uint32_t magic = 0;
  try {
    magic = SocketHelper::readUint32(socket, NETRW_TIMEOUT);
  } catch (castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_FAILED_TO_READ_MAGIC, 2, params);

    return;
  }

  // Find the map of request handlers for the magic number
  MagicToHandlersMap::iterator handlerMapItor = m_magicToHandlers.find(magic);
  if(handlerMapItor == m_magicToHandlers.end()) {
    // Unknown magic number
    castor::dlf::Param params[] = {
      castor::dlf::Param("Magic Number", magic)};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR, AGGREGATOR_UNKNOWN_MAGIC, 1,
      params);

    return;
  }
  HandlerMap *handlers = handlerMapItor->second;

  // Read and unmarshall the type of the request
  uint32_t reqtype = 0;
  try {
    reqtype = SocketHelper::readUint32(socket, NETRW_TIMEOUT);
  } catch (castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_FAILED_TO_READ_REQUEST_TYPE, 2, params);

    return;
  }

  // Find the request handler for the type of request
  HandlerMap::iterator handlerItor = handlers->find(reqtype);
  if(handlerItor == handlers->end()) {
    // Unknown request type
    castor::dlf::Param params[] = {
      castor::dlf::Param("Magic Number", magic),
      castor::dlf::Param("Request Type", reqtype)};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_UNKNOWN_REQUEST_TYPE, 2, params);

    return;
  }
  Handler handler = handlerItor->second;

  // Read and unmarshall the length of the message body of the request
  uint32_t len = 0;
  try {
    len = SocketHelper::readUint32(socket, NETRW_TIMEOUT);
  } catch (castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_FAILED_TO_READ_MESSAGE_BODY_LENGTH, 2, params);

    return;
  }

  // Only need a buffer for the message body, the header has already been read
  // from the socket and unmarshalled
  char body[MSGBUFSIZ - 3 * sizeof(uint32_t)];

  // If the message body is larger than the message body buffer
  if(len > sizeof(body)) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Maximum length", sizeof(body)),
      castor::dlf::Param("Actual length", len)};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_MESSAGE_BODY_LENGTH_TOO_LARGE, 2, params);

    return;
  }

  // Read the message body from the socket
  try {
    SocketHelper::readBytes(socket, NETRW_TIMEOUT, len, body);
  } catch (castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_FAILED_TO_READ_MESSAGE_BODY, 2, params);

    return;
  }

  // Dispatch the request to the appropriate handler
  (this->*handler)(cuuid, magic, reqtype, len, body, socket);
}


//-----------------------------------------------------------------------------
// handleJobSubmission
//-----------------------------------------------------------------------------
void castor::tape::aggregator::VdqmRequestHandlerThread::handleJobSubmission(
  Cuuid_t &cuuid, const uint32_t magic, const uint32_t reqtype,
  const uint32_t len, char *body, castor::io::ServerSocket &socket) throw() {

  // Check that the peer host is authorized
  {
    char peerHost[CA_MAXHOSTNAMELEN+1];


    const int rc = isadminhost(socket.socket(), peerHost);
    if(rc == -1 && serrno != SENOTADMIN) {
      castor::dlf::Param params[] = {
        castor::dlf::Param("Reason", "Failed to lookup connection"),
        castor::dlf::Param("Peer Host", peerHost)};
      castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
        AGGREGATOR_RECEIVED_RCP_JOB_FROM_UNAUTHORIZED_HOST, 2, params);
      return;
    }

    if(*peerHost == '\0' ) {
      castor::dlf::Param params[] = {
        castor::dlf::Param("Reason", "Peer host name is empty"),
        castor::dlf::Param("Peer Host", peerHost)};
      castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
        AGGREGATOR_RECEIVED_RCP_JOB_FROM_UNAUTHORIZED_HOST, 2, params);
      return;
    }

    if(rc != 0) {
      castor::dlf::Param params[] = {
        castor::dlf::Param("Reason", "Unauthorized host"),
        castor::dlf::Param("Peer Host", peerHost)};
      castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
        AGGREGATOR_RECEIVED_RCP_JOB_FROM_UNAUTHORIZED_HOST, 2, params);
      return;
    }
  }

  // If the message body is too small
  if(len < 4 * sizeof(uint32_t) + 4) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Minimum Length", 4 * sizeof(uint32_t) + 4),
      castor::dlf::Param("Actual Length", len)};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_JOB_MESSAGE_BODY_LENGTH_TOO_SHORT, 2, params);
  }

  uint32_t   ui32          = 0; // Used to for casts
  u_signed64 tapeRequestID = 0;
  uint32_t   clientPort    = 0;
  uint32_t   clientEuid    = 0;
  uint32_t   clientEgid    = 0;
  char       clientHost[CA_MAXHOSTNAMELEN+1];
  char       dgn[CA_MAXDGNLEN+1];
  char       driveName[CA_MAXUNMLEN+1];
  char       clientUsername[CA_MAXUSRNAMELEN+1];

  char   *p      = body;
  size_t bodyLen = len;

  Marshaller::unmarshallUint32(p, bodyLen, ui32);
  tapeRequestID = (u_signed64)ui32; // Cast from 32-bits to 64-bits
  Marshaller::unmarshallUint32(p, bodyLen, clientPort);
  Marshaller::unmarshallUint32(p, bodyLen, clientEuid);
  Marshaller::unmarshallUint32(p, bodyLen, clientEgid);
  Marshaller::unmarshallString(p, bodyLen, clientHost, sizeof(clientHost));
  Marshaller::unmarshallString(p, bodyLen, dgn, sizeof(dgn));
  Marshaller::unmarshallString(p, bodyLen, driveName, sizeof(driveName));
  Marshaller::unmarshallString(p, bodyLen, clientUsername,
    sizeof(clientUsername));

  castor::dlf::Param param[] = {
    castor::dlf::Param("tapeRequestID" , tapeRequestID ),
    castor::dlf::Param("clientPort"    , clientPort    ),
    castor::dlf::Param("clientEuid"    , clientEuid    ),
    castor::dlf::Param("clientEgid"    , clientEgid    ),
    castor::dlf::Param("clientHost"    , clientHost    ),
    castor::dlf::Param("dgn"           , dgn           ),
    castor::dlf::Param("driveName"     , driveName     ),
    castor::dlf::Param("clientUsername", clientUsername)};
  castor::dlf::dlf_writep(cuuid, DLF_LVL_SYSTEM,
    AGGREGATOR_HANDLE_JOB_MESSAGE, 8, param);

  // Prepare a positive response which will be overwritten if RTCPD replies to
  // the tape aggregator with an error message
  uint32_t    errorStatusForVdqm  = VDQM_CLIENTINFO; // Strange status code
  std::string errorMessageForVdqm = "";

  // Pass a modified version of the request through to RTCPD, setting the
  // clientHost and clientPort parameters to identify the tape aggregator as
  // being a proxy for RTCPClientD
  try {
    RcpJobSubmitter::submit(
    "localhost",       // host
    RTCOPY_PORT,       // port
    NETRW_TIMEOUT,     // netReadWriteTimeout
    "RTCPD",           // remoteCopyType
    tapeRequestID,
    clientUsername,
    "localhost",       // clientHost
    m_rtcpdListenPort, // clientPort
    clientEuid,
    clientEgid,
    dgn,
    driveName);
  } catch(castor::tape::aggregator::exception::RTCPDErrorMessage &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_RECEIVED_RTCPD_ERROR_MESSAGE, 2, params);

    // Override positive response with the error message from RTCPD
    errorStatusForVdqm  = ex.code();
    errorMessageForVdqm = ex.getMessage().str();
  } catch(castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_FAILED_TO_SUBMIT_JOB_TO_RTCPD, 2, params);

    return;
  }

  // Acknowledge the VDQM - maybe postive or negative depending on reply from
  // RTCPD
  char ackMsg[MSGBUFSIZ];

  size_t ackMsgLen = 0;

  try {
    ackMsgLen = Marshaller::marshallRtcpAckn(ackMsg, sizeof(ackMsg),
      errorStatusForVdqm, errorMessageForVdqm.c_str());
  } catch(castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_FAILED_TO_MARSHALL_RTCP_ACKN, 2, params);
  }

  const int rc = netwrite_timeout(socket.socket(), ackMsg, ackMsgLen,
    NETRW_TIMEOUT);

  try {
    if(rc == -1) {
      castor::exception::Exception ex(SECOMERR);
      ex.getMessage() << __PRETTY_FUNCTION__
        << ": netwrite(REQ) to VDQM: " << neterror();
      throw ex;
    } else if(rc == 0) {
      castor::exception::Exception ex(SECONNDROP);
      ex.getMessage() << __PRETTY_FUNCTION__
        << ": netwrite(REQ) to VDQM: Connection dropped";
      throw ex;
    }
  } catch(castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("Message", ex.getMessage().str()),
      castor::dlf::Param("Code"   , ex.code())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR,
      AGGREGATOR_FAILED_TO_SEND_RTCP_ACKN_TO_VDQM, 2, params);
  }
}

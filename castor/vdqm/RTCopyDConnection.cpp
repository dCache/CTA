/******************************************************************************
 *                      RTCopyDConnection.cpp
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
 * @(#)RCSfile: RTCopyDConnection.cpp  Revision: 1.0  Release Date: Jul 29, 2005  Author: mbraeger
 *
 *
 *
 * @author Matthias Braeger
 *****************************************************************************/

#include <errno.h>
#include <serrno.h>
#include <net.h>
#include <unistd.h>
#include <rtcp_constants.h> /* Definition of RTCOPY_MAGIC   */

#include "castor/exception/InvalidArgument.hpp"
#include "castor/vdqm/ClientIdentification.hpp"
#include "castor/vdqm/DevTools.hpp"
#include "castor/vdqm/DeviceGroupName.hpp"
#include "castor/vdqm/newVdqm.h"
#include "castor/vdqm/RTCopyDConnection.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/VdqmDlfMessageConstants.hpp"
#include "castor/vdqm/vdqmMacros.h"  // Needed for marshalling

#include "osdep.h" //for LONGSIZE


//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::vdqm::RTCopyDConnection::RTCopyDConnection(int socket) throw () :
  AbstractTCPSocket(socket) {}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::vdqm::RTCopyDConnection::RTCopyDConnection(const unsigned short port,
                                                   const std::string host)
  throw (castor::exception::Exception) :
  AbstractTCPSocket(port, host, false) {
  createSocket();
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::vdqm::RTCopyDConnection::RTCopyDConnection(const unsigned short port,
                                                   const unsigned long ip)
  throw (castor::exception::Exception) :
  AbstractTCPSocket(port, ip, false) {
    createSocket();
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::vdqm::RTCopyDConnection::~RTCopyDConnection() throw () {
  ::shutdown(m_socket, SD_BOTH);
  this->close();
}


//------------------------------------------------------------------------------
// connect
//------------------------------------------------------------------------------
void castor::vdqm::RTCopyDConnection::connect()
  throw (castor::exception::Exception) {
  // Connects the socket
  if (::connect(m_socket, (struct sockaddr *)&m_saddr, sizeof(m_saddr)) < 0) {
    int tmpserrno = errno;
    int tmperrno = errno;
    if (errno != ECONNREFUSED) {
      tmpserrno = SECOMERR;
    }
    castor::exception::Exception ex(tmpserrno);
    ex.getMessage() << "Unable to connect socket";
    if (m_saddr.sin_family == AF_INET) {
      unsigned long ip = m_saddr.sin_addr.s_addr;
      ex.getMessage() << " to "
                      << (ip%256) << "."
                      << ((ip >> 8)%256) << "."
                      << ((ip >> 16)%256) << "."
                      << (ip >> 24) << ":"
                      << ntohs(m_saddr.sin_port);
    }
    this->close();
    errno = tmperrno;

    throw ex;
  }
}


//------------------------------------------------------------------------------
// sendJobToRTCP
//------------------------------------------------------------------------------
bool castor::vdqm::RTCopyDConnection::sendJobToRTCPD(
  const u_signed64  tapeRequestID,
  const std::string &clientUserName,
  const std::string &clientMachine,
  const int         clientPort,
  const int         clientEuid,
  const int         clientEgid,
  const std::string &deviceGroupName,
  const std::string &tapeDriveName)
  throw (castor::exception::Exception) {

  bool acknSucc = true; // The return value

  newVdqmDrvReq_t vdqmDrvReq;
  newVdqmVolReq_t vdqmVolReq;

  char buf[VDQM_MSGBUFSIZ];
  int len, rc;
  int magic = RTCOPY_MAGIC_OLD0;
  int reqtype = VDQM_CLIENTINFO;
  char* p;

  unsigned int castValue;
  int intValue;
  char* stringValue;


  // Let's count the length of the message for the header.
  // Please notice: Normally the ID is a 64Bit number!!
  // But for historical reasons, we will do a downcast of the
  // id, which is then still unique, because a tapeRequest has
  // a very short lifetime, compared to the number of new IDs
  len = 4*LONGSIZE + clientUserName.length() +
    clientMachine.length()  +
    deviceGroupName.length()  +
    tapeDriveName.length()  + 4;

  p = buf;

  DO_MARSHALL(LONG, p, magic, SendTo);
  DO_MARSHALL(LONG, p, reqtype, SendTo);
  DO_MARSHALL(LONG, p, len, SendTo);

  /**
   * We must do a downcast because of the old Protocol.
   * Of course we do later the same in case of a comparison
   */
  castValue = (unsigned int)tapeRequestID;
  DO_MARSHALL(LONG, p, castValue, SendTo);

  intValue = clientPort;
  DO_MARSHALL(LONG, p, intValue, SendTo);

  intValue = clientEuid;
  DO_MARSHALL(LONG, p, intValue, SendTo);

  intValue = clientEgid;
  DO_MARSHALL(LONG, p, intValue, SendTo);

  stringValue = (char *)clientMachine.c_str();
  DO_MARSHALL_STRING(p, stringValue, SendTo, sizeof(vdqmVolReq.client_host));

  stringValue = (char *)deviceGroupName.c_str();
  DO_MARSHALL_STRING(p, stringValue, SendTo, sizeof(vdqmDrvReq.dgn));

  stringValue = (char *)tapeDriveName.c_str();
  DO_MARSHALL_STRING(p, stringValue, SendTo, sizeof(vdqmDrvReq.drive));

  stringValue = (char *)clientUserName.c_str();
  DO_MARSHALL_STRING(p, stringValue, SendTo, sizeof(vdqmVolReq.client_name));

  len += 3*LONGSIZE;

  /**
   * After marshalling we can send the informations to RTCP
   */
  rc = netwrite_timeout(m_socket, buf, len, VDQM_TIMEOUT);
#ifdef PRINT_NETWORK_MESSAGES
  castor::vdqm::DevTools::printMessage(std::cout, true, true, m_socket, buf);
#endif

  if (rc == -1) {
    serrno = SECOMERR;
    castor::exception::Exception ex(serrno);
    ex.getMessage() << "RTCopyDConnection::sendJobToRTCP(): "
                    << "netwrite(REQ): "
                    << neterror() << std::endl;
    throw ex;
  }
  else if (rc == 0) {
    serrno = SECONNDROP;
    castor::exception::Exception ex(serrno);
    ex.getMessage() << "RTCopyDConnection::sendJobToRTCP(): "
                    << "netwrite(REQ): connection dropped" << std::endl;
    throw ex;
  }


  acknSucc = readRTCPAnswer();

  return acknSucc;
}


//------------------------------------------------------------------------------
// readRTCPAnswer
//------------------------------------------------------------------------------
bool castor::vdqm::RTCopyDConnection::readRTCPAnswer()
  throw (castor::exception::Exception) {

  int rc, magic, reqtype, len, errmsglen, msglen, status;
  char errmsg[1024];
  char buffer[VDQM_MSGBUFSIZ];
  char* p;

  rc = netread_timeout(m_socket, buffer, LONGSIZE*3, VDQM_TIMEOUT);
#ifdef PRINT_NETWORK_MESSAGES
  castor::vdqm::DevTools::printMessage(std::cout, false, true, m_socket,
    buffer);
#endif

  switch (rc) {
  case -1:
    {
      serrno = SECOMERR;
      castor::exception::Exception ex(serrno);
      ex.getMessage() << "RTCopyDConnection::sendJobToRTCP(): "
                      << "netread(HDR): "
                      << neterror() << std::endl;
      throw ex;
    }
  case 0:
    {
      serrno = SECONNDROP;
      castor::exception::Exception ex(serrno);
      ex.getMessage() << "RTCopyDConnection::sendJobToRTCP(): "
                      << "netread(HDR): connection dropped"
                      << std::endl;
      throw ex;
    }
  }

  p = buffer;

  unmarshall_LONG(p, magic);
  unmarshall_LONG(p, reqtype);
  unmarshall_LONG(p, len);

  rc = 0;
  if ( len > 0 ) {
    if ( len > VDQM_MSGBUFSIZ - 3*LONGSIZE ) {
      // "RTCopyDConnection: Too large errmsg buffer requested" message
      castor::dlf::Param params[] =
        {castor::dlf::Param("valid length", (VDQM_MSGBUFSIZ-3*LONGSIZE)),
         castor::dlf::Param("requested length", len)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, VDQM_RTCOPYDCONNECTION_ERRMSG_TOO_LARGE, 2, params);

      len = VDQM_MSGBUFSIZ - 3*LONGSIZE;
    }

    rc = netread_timeout(m_socket, p, len, VDQM_TIMEOUT);
    switch (rc) {
    case -1:
      {
        serrno = SECOMERR;
        castor::exception::Exception ex(serrno);
        ex.getMessage() << "RTCopyDConnection::sendJobToRTCP(): "
                        << "netread(HDR): "
                        << neterror() << std::endl;
        throw ex;
      }
    case 0:
      {
        serrno = SECONNDROP;
        castor::exception::Exception ex(serrno);
        ex.getMessage() << "RTCopyDConnection::sendJobToRTCP(): "
                        << "netread(HDR): connection dropped"
                        << std::endl;
        throw ex;
      }
    }

    /*
     * Acknowledge message
     */
    p = buffer;
    errmsglen = 1024;
    *errmsg = '\0';
    status = 0;

    DO_MARSHALL(LONG, p, magic, ReceiveFrom);
    DO_MARSHALL(LONG, p, reqtype, ReceiveFrom);
    DO_MARSHALL(LONG, p, len, ReceiveFrom);

    if ( (magic != RTCOPY_MAGIC && magic != RTCOPY_MAGIC_OLD0) ||
         reqtype != VDQM_CLIENTINFO ) return false;

    DO_MARSHALL(LONG, p, status, ReceiveFrom);

    msglen = len - LONGSIZE -1;
    msglen = msglen < errmsglen-1 ? msglen : errmsglen-1;
    strncpy(errmsg, p, msglen);
    errmsg[msglen] = '\0';
    errmsglen = msglen;

    len += 3*LONGSIZE;

    if ( errmsglen > 0 ) {
      // "RTCopyDConnection: rtcopy daemon returned an error" message
      castor::dlf::Param params[] =
        {castor::dlf::Param("status", status),
         castor::dlf::Param("error msg", errmsg)};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, VDQM_RTCOPYDCONNECTION_RTCOPY_ERROR, 2, params);

      return false;
    }
  }

  return true;
}

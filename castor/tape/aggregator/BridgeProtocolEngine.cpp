/******************************************************************************
 *                      BridgeProtocolEngine.cpp
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
 *
 * @author Nicola.Bessone@cern.ch Steven.Murray@cern.ch
 *****************************************************************************/

#include "castor/dlf/Dlf.hpp"
#include "castor/tape/aggregator/AggregatorDlfMessageConstants.hpp"
#include "castor/tape/aggregator/BridgeProtocolEngine.hpp"
#include "castor/tape/aggregator/Constants.hpp"
#include "castor/tape/aggregator/GatewayTxRx.hpp"
#include "castor/tape/aggregator/MessageHeader.hpp"
#include "castor/tape/aggregator/Net.hpp"
#include "castor/tape/aggregator/RtcpTxRx.hpp"
#include "castor/tape/aggregator/SmartFd.hpp"
#include "castor/tape/aggregator/SmartFdList.hpp"
#include "castor/tape/aggregator/Utils.hpp"
#include "h/Ctape_constants.h"
#include "h/rtcp_constants.h"

#include <list>


//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
castor::tape::aggregator::BridgeProtocolEngine::BridgeProtocolEngine(
  const Cuuid_t &cuuid,
  const uint32_t volReqId,
  const char (&gatewayHost)[CA_MAXHOSTNAMELEN+1],
  const unsigned short gatewayPort,
  const int rtcpdCallbackSocketFd,
  const int rtcpdInitialSocketFd,
  const uint32_t mode,
  char (&unit)[CA_MAXUNMLEN+1],
  const char (&vid)[CA_MAXVIDLEN+1],
  char (&vsn)[CA_MAXVSNLEN+1],
  const char (&label)[CA_MAXLBLTYPLEN+1],
  const char (&density)[CA_MAXDENLEN+1]):
  m_cuuid(cuuid),
  m_volReqId(volReqId),
  m_gatewayHost(gatewayHost),
  m_gatewayPort(gatewayPort),
  m_rtcpdCallbackSocketFd(rtcpdCallbackSocketFd),
  m_rtcpdInitialSocketFd(rtcpdInitialSocketFd),
  m_mode(mode),
  m_unit(unit),
  m_vid(vid),
  m_vsn(vsn),
  m_label(label),
  m_density(density),
  m_nbDiskTapeIOConnections(0),
  m_nbReceivedENDOF_REQs(0) {

  // Build the map of message body handlers
  m_handlers[RTCP_FILE_REQ]    = &BridgeProtocolEngine::rtcpFileReqCallback;
  m_handlers[RTCP_FILEERR_REQ] = &BridgeProtocolEngine::rtcpFileErrReqCallback;
  m_handlers[RTCP_TAPE_REQ]    = &BridgeProtocolEngine::rtcpTapeReqCallback;
  m_handlers[RTCP_TAPEERR_REQ] = &BridgeProtocolEngine::rtcpTapeErrReqCallback;
  m_handlers[RTCP_ENDOF_REQ]   = &BridgeProtocolEngine::rtcpEndOfReqCallback;
}


//-----------------------------------------------------------------------------
// acceptRtcpdConnection
//-----------------------------------------------------------------------------
int castor::tape::aggregator::BridgeProtocolEngine::acceptRtcpdConnection()
  throw(castor::exception::Exception) {

  SmartFd connectedSocketFd(Net::acceptConnection(m_rtcpdCallbackSocketFd,
    RTCPDPINGTIMEOUT));

  try {
    unsigned short port = 0; // Client port
    unsigned long  ip   = 0; // Client IP
    char           hostName[HOSTNAMEBUFLEN];

    Net::getPeerIpPort(connectedSocketFd.get(), ip, port);
    Net::getPeerHostName(connectedSocketFd.get(), hostName);

    castor::dlf::Param params[] = {
      castor::dlf::Param("volReqId", m_volReqId                ),
      castor::dlf::Param("IP"      , castor::dlf::IPAddress(ip)),
      castor::dlf::Param("Port"    , port                      ),
      castor::dlf::Param("HostName", hostName                  ),
      castor::dlf::Param("socketFd", connectedSocketFd.get()   ),
      castor::dlf::Param("nbDiskTapeIOConnections",
        m_nbDiskTapeIOConnections)};
    castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
      AGGREGATOR_RTCPD_CALLBACK_WITH_INFO, params);
  } catch(castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("volReqId"             , m_volReqId             ),
      castor::dlf::Param("nbDiskTapeIOConnections", m_nbDiskTapeIOConnections)};
    castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
      AGGREGATOR_RTCPD_CALLBACK_WITHOUT_INFO, params);
  }

  return connectedSocketFd.release();
}


//-----------------------------------------------------------------------------
// processRtcpdSockets
//-----------------------------------------------------------------------------
void castor::tape::aggregator::BridgeProtocolEngine::processRtcpdSockets()
  throw(castor::exception::Exception) {

  SmartFdList readFds;
  int selectRc = 0;
  int selectErrno = 0;
  fd_set readFdSet;
  int maxFd = 0;
  timeval timeout;

  // Append the socket descriptors of the RTCPD callback port and the initial
  // connection from RTCPD to the list of read file descriptors
  readFds.push_back(m_rtcpdCallbackSocketFd);
  readFds.push_back(m_rtcpdInitialSocketFd);
  if(m_rtcpdCallbackSocketFd > m_rtcpdInitialSocketFd) {
    maxFd = m_rtcpdCallbackSocketFd;
  } else {
    maxFd = m_rtcpdInitialSocketFd;
  }

  try {
    // Select loop
    bool continueRtcopySession = true;
    while(continueRtcopySession)
    {
      // Build the file descriptor set ready for the select call
      FD_ZERO(&readFdSet);
      for(std::list<int>::iterator itor = readFds.begin();
        itor != readFds.end(); itor++) {

        FD_SET(*itor, &readFdSet);

        if(*itor > maxFd) {
          maxFd = *itor;
        }
      }

      timeout.tv_sec  = RTCPDPINGTIMEOUT;
      timeout.tv_usec = 0;

      // See if any of the read file descriptors ready?
      selectRc = select(maxFd + 1, &readFdSet, NULL, NULL, &timeout);
      selectErrno = errno;

      switch(selectRc) {
      case 0: // Select timed out

        RtcpTxRx::pingRtcpd(m_cuuid, m_volReqId, m_rtcpdInitialSocketFd,
          RTCPDNETRWTIMEOUT);
        break;

      case -1: // Select encountered an error

        // If select encountered an error other than an interruption
        if(selectErrno != EINTR) {
          char strerrorBuf[STRERRORBUFLEN];
          char *const errorStr = strerror_r(selectErrno, strerrorBuf,
            sizeof(strerrorBuf));

          TAPE_THROW_CODE(selectErrno,
            ": Select encountered an error other than an interruption"
            ": " << errorStr);
        }
        break;

      default: // One or more select file descriptors require attention

        int nbProcessedFds = 0;

        // For each read file descriptor or until all ready file descriptors
        // have been processed
        for(std::list<int>::iterator itor=readFds.begin();
          itor != readFds.end() && nbProcessedFds < selectRc; itor++) {

          // If the read file descriptor is ready
          if(FD_ISSET(*itor, &readFdSet)) {

            processRtcpdSocket(*itor);
            nbProcessedFds++;
          }
        }
      } // switch(selectRc)
    } // while(continueRtcopySession)
  } catch(castor::exception::Exception &ex) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("volReqId", m_volReqId           ),
      castor::dlf::Param("Message" , ex.getMessage().str()),
      castor::dlf::Param("Code"    , ex.code()            )};
    CASTOR_DLF_WRITEPC(m_cuuid, DLF_LVL_ERROR, AGGREGATOR_MAIN_SELECT_FAILED,
      params);
  }
}


//-----------------------------------------------------------------------------
// processRtcpdSocket
//-----------------------------------------------------------------------------
void castor::tape::aggregator::BridgeProtocolEngine::processRtcpdSocket(
  const int socketFd) throw(castor::exception::Exception) {

  // If the file descriptor is that of the callback port
  if(socketFd == m_rtcpdCallbackSocketFd) {

    // Accept the connection and append its socket descriptor to
    // the list of read file descriptors
    m_readFds.push_back(acceptRtcpdConnection());

    m_nbDiskTapeIOConnections++;

  // Else the file descriptor is that of a tape/disk IO connection
  } else {

    // Try to receive the message header which may not be possible; The file
    // descriptor may be ready because RTCPD has closed the connection
    bool connectionClosed = false;
    MessageHeader header;
    utils::setBytes(header, '\0');
    RtcpTxRx::receiveRtcpMsgHeaderFromCloseable(m_cuuid, connectionClosed,
      m_volReqId, socketFd, RTCPDNETRWTIMEOUT, header);

    // If the connection has been closed by RTCPD, then remove the
    // file descriptor from the list of read file descriptors and
    // close it
    if(connectionClosed) {
      close(m_readFds.release(socketFd));

      m_nbDiskTapeIOConnections--;

      castor::dlf::Param params[] = {
        castor::dlf::Param("volReqId", m_volReqId),
        castor::dlf::Param("socketFd", socketFd  ),
        castor::dlf::Param("nbDiskTapeIOConnections",
          m_nbDiskTapeIOConnections)};
      castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
        AGGREGATOR_CONNECTION_CLOSED_BY_RTCPD, params);
    }

    bool receivedENDOF_REQ = false;
    processRtcpdRequest(header, socketFd, receivedENDOF_REQ);

    // If an RTCP_ENDOF_REQ message was received
    if(receivedENDOF_REQ) {
      m_nbReceivedENDOF_REQs++;

      {
        castor::dlf::Param params[] = {
          castor::dlf::Param("volReqId", m_volReqId),
          castor::dlf::Param("socketFd", socketFd  ),
          castor::dlf::Param("nbReceivedENDOF_REQs", m_nbReceivedENDOF_REQs)};
        castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
          AGGREGATOR_RECEIVED_RTCP_ENDOF_REQ, params);
      }

      // Remove the file descriptor from the list of read file descriptors and
      // close it
      close(m_readFds.release(socketFd));

      m_nbDiskTapeIOConnections--;

      {
        castor::dlf::Param params[] = {
          castor::dlf::Param("volReqId", m_volReqId),
          castor::dlf::Param("socketFd", socketFd  ),
          castor::dlf::Param("nbDiskTapeIOConnections",
            m_nbDiskTapeIOConnections)};
        castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
          AGGREGATOR_CLOSED_CONNECTION, params);
      }
    }
  }
}


//-----------------------------------------------------------------------------
// run
//-----------------------------------------------------------------------------
void castor::tape::aggregator::BridgeProtocolEngine::run()
  throw(castor::exception::Exception) {

  RtcpFileRqstErrMsgBody rtcpFileReply;
  utils::setBytes(rtcpFileReply, '\0');

  // Allocate some variables on the stack for information about a possible file
  // to migrate.  These variables will not be used in the case of a recall
  char migrationFilePath[CA_MAXPATHLEN+1];
  utils::setBytes(migrationFilePath, '\0');
  char migrationFileNsHost[CA_MAXHOSTNAMELEN+1];
  utils::setBytes(migrationFileNsHost, '\0');
  uint64_t migrationFileId = 0;
  uint32_t migrationFileTapeFseq = 0;
  uint64_t migrationFileSize = 0;
  char migrationFileLastKnownFileName[CA_MAXPATHLEN+1];
  utils::setBytes(migrationFileLastKnownFileName, '\0');
  uint64_t migrationFileLastModificationTime = 0;
  int32_t positionCommandCode = 0;
  char tapePath[CA_MAXPATHLEN+1];
  utils::setBytes(tapePath, '\0');
  int32_t tapeFseq;		// <-- TEMPORARY
  unsigned char (blockId)[4];  // <-- TEMPORARY
  utils::setBytes(blockId, '\0');

  // If migrating
  if(m_mode == WRITE_ENABLE) {

    // Get first file to migrate from tape gateway
    const bool thereIsAFileToMigrate =
      GatewayTxRx::getFileToMigrateFromGateway(m_cuuid, m_volReqId,
        m_gatewayHost, m_gatewayPort, migrationFilePath, migrationFileNsHost,
        migrationFileId, migrationFileTapeFseq, migrationFileSize,
        migrationFileLastKnownFileName, migrationFileLastModificationTime,
        positionCommandCode);

    // Return if there is no file to migrate
    if(!thereIsAFileToMigrate) {
      return;
    }
  }

  // Give volume to RTCPD
  RtcpTapeRqstErrMsgBody rtcpVolume;
  utils::setBytes(rtcpVolume, '\0');
  utils::copyString(rtcpVolume.vid    , m_vid    );
  utils::copyString(rtcpVolume.vsn    , m_vsn    );
  utils::copyString(rtcpVolume.label  , m_label  );
  utils::copyString(rtcpVolume.density, m_density);
  utils::copyString(rtcpVolume.unit   , m_unit   );
  rtcpVolume.volReqId       = m_volReqId;
  rtcpVolume.mode           = m_mode;
  rtcpVolume.tStartRequest  = time(NULL);
  rtcpVolume.err.severity   =  1;
  rtcpVolume.err.maxTpRetry = -1;
  rtcpVolume.err.maxCpRetry = -1;
  RtcpTxRx::giveVolumeToRtcpd(m_cuuid, m_volReqId, m_rtcpdInitialSocketFd,
    RTCPDNETRWTIMEOUT, rtcpVolume);

  // If migrating
  if(m_mode == WRITE_ENABLE) {

    // Give file to migrate to RTCPD
    char migrationTapeFileId[CA_MAXPATHLEN+1];
    utils::toHex(migrationFileId, migrationTapeFileId);
    RtcpTxRx::giveFileToRtcpd(m_cuuid, m_volReqId, m_rtcpdInitialSocketFd,
      RTCPDNETRWTIMEOUT, rtcpVolume.mode, migrationFilePath, "", RECORDFORMAT,
      migrationTapeFileId, MIGRATEUMASK, positionCommandCode, tapeFseq,
      migrationFileNsHost, migrationFileId, blockId);
  }

  // Ask RTCPD to request more work
  RtcpTxRx::askRtcpdToRequestMoreWork(m_cuuid, m_volReqId, tapePath, 
    m_rtcpdInitialSocketFd, RTCPDNETRWTIMEOUT, m_mode);

  // Tell RTCPD end of file list
  RtcpTxRx::tellRtcpdEndOfFileList(m_cuuid, m_volReqId, m_rtcpdInitialSocketFd,
    RTCPDNETRWTIMEOUT);

  // Spin a select loop processing the RTCPD sockets
  processRtcpdSockets();
}


//-----------------------------------------------------------------------------
// processRtcpdRequest
//-----------------------------------------------------------------------------
void castor::tape::aggregator::BridgeProtocolEngine::processRtcpdRequest(
  const MessageHeader &header, const int socketFd, bool &receivedENDOF_REQ)
  throw(castor::exception::Exception) {

  {
    char magicHex[2 + 17]; // 0 + x + FFFFFFFFFFFFFFFF + '\0'
    magicHex[0] = '0';
    magicHex[1] = 'x';
    utils::toHex(header.magic, &(magicHex[2]), 17);

    const char *magicName = utils::magicToStr(header.magic);

    char reqTypeHex[2 + 17]; // 0 + x + FFFFFFFFFFFFFFFF + '\0'
    reqTypeHex[0] = '0';
    reqTypeHex[1] = 'x';
    utils::toHex(header.reqType, &(reqTypeHex[2]), 17);

    const char *reqTypeName = utils::rtcopyReqTypeToStr(header.reqType);

    castor::dlf::Param params[] = {
      castor::dlf::Param("volReqId"   , m_volReqId ),
      castor::dlf::Param("socketFd"   , socketFd   ),
      castor::dlf::Param("magic"      , magicHex   ),
      castor::dlf::Param("magicName"  , magicName  ),
      castor::dlf::Param("reqType"    , reqTypeHex ),
      castor::dlf::Param("reqTypeName", reqTypeName),
      castor::dlf::Param("len"        , header.len )};
    castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
      AGGREGATOR_PROCESSING_TAPE_DISK_RQST, params);
  }

  // Find the message type's corresponding handler
  MsgBodyCallbackMap::iterator itor = m_handlers.find(header.reqType);
  if(itor == m_handlers.end()) {
    TAPE_THROW_CODE(EBADMSG,
      ": Unknown request type: 0x" << header.reqType);
  }
  const MsgBodyCallback handler = itor->second;

  // Invoke the handler
  return (this->*handler)(header, socketFd, receivedENDOF_REQ);
}


//-----------------------------------------------------------------------------
// rtcpFileReqCallback
//-----------------------------------------------------------------------------
void castor::tape::aggregator::BridgeProtocolEngine::rtcpFileReqCallback(
  const MessageHeader &header, const int socketFd, bool &receivedENDOF_REQ)
  throw(castor::exception::Exception) {

  RtcpFileRqstMsgBody body;

  RtcpTxRx::receiveRtcpFileRqstBody(m_cuuid, m_volReqId, socketFd,
    RTCPDNETRWTIMEOUT, header, body);

  switch(body.procStatus) {
  case RTCP_REQUEST_MORE_WORK:
    // If migrating
    if(m_mode == WRITE_ENABLE) {

      char filePath[CA_MAXPATHLEN+1];
      utils::setBytes(filePath, '\0');
      char nsHost[CA_MAXHOSTNAMELEN+1];
      utils::setBytes(nsHost, '\0');
      char tapePath[CA_MAXPATHLEN+1];
      utils::setBytes(tapePath, '\0');
      uint64_t fileId = 0;
      uint32_t tapeFseq = 0;
      uint64_t fileSize = 0;
      char lastKnownFileName[CA_MAXPATHLEN+1];
      utils::setBytes(lastKnownFileName, '\0');
      uint64_t lastModificationTime = 0;
      int32_t  positionMethod = 0;
       unsigned char blockId[4];
      utils::setBytes(blockId, '\0');

      // If there is a file to migrate
      if(GatewayTxRx::getFileToMigrateFromGateway(m_cuuid, m_volReqId,
        m_gatewayHost, m_gatewayPort, filePath, nsHost, fileId, tapeFseq,
        fileSize, lastKnownFileName, lastModificationTime, positionMethod)) {

        castor::dlf::Param params[] = {
          castor::dlf::Param("volReqId"            , m_volReqId          ),
          castor::dlf::Param("gatewayHost"         , m_gatewayHost       ),
          castor::dlf::Param("gatewayPort"         , m_gatewayPort       ),
          castor::dlf::Param("filePath"            , filePath            ),
          castor::dlf::Param("nsHost"              , nsHost              ),
          castor::dlf::Param("fileId"              , fileId              ),
          castor::dlf::Param("tapeFseq"            , tapeFseq            ),
          castor::dlf::Param("fileSize"            , fileSize            ),
          castor::dlf::Param("lastKnownFileName"   , lastKnownFileName   ),
          castor::dlf::Param("lastModificationTime", lastModificationTime),
          castor::dlf::Param("positionMethod"      , positionMethod      )};
        castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
          AGGREGATOR_FILE_TO_MIGRATE, params);

        char tapeFileId[CA_MAXPATHLEN+1];
        utils::toHex(fileId, tapeFileId);
        RtcpTxRx::giveFileToRtcpd(m_cuuid, m_volReqId, socketFd,
          RTCPDNETRWTIMEOUT, WRITE_ENABLE, filePath, "", RECORDFORMAT,
          tapeFileId, MIGRATEUMASK, tapeFseq, positionMethod, nsHost, fileId,
          blockId);

        RtcpTxRx::askRtcpdToRequestMoreWork(m_cuuid, m_volReqId, tapePath,
          socketFd, RTCPDNETRWTIMEOUT, WRITE_ENABLE);

        RtcpTxRx::tellRtcpdEndOfFileList(m_cuuid, m_volReqId, socketFd,
          RTCPDNETRWTIMEOUT);

      // Else there is no file to migrate
      } else {

        castor::dlf::Param params[] = {
          castor::dlf::Param("volReqId"   , m_volReqId   ),
          castor::dlf::Param("gatewayHost", m_gatewayHost),
          castor::dlf::Param("gatewayPort", m_gatewayPort)};
        castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
          AGGREGATOR_NO_MORE_FILES_TO_MIGRATE, params);

        // Tell RTCPD there is no file by sending an empty file list
        RtcpTxRx::tellRtcpdEndOfFileList(m_cuuid, m_volReqId, socketFd,
          RTCPDNETRWTIMEOUT);
      }

    // Else recalling
    } else {

      char filePath[CA_MAXPATHLEN+1];
      utils::setBytes(filePath, '\0');
      char nsHost[CA_MAXHOSTNAMELEN+1];
      utils::setBytes(nsHost, '\0');
      uint64_t fileId = 0;
      uint32_t tapeFseq = 0;
      unsigned char blockId[4];
      utils::setBytes(blockId, '\0');
      int32_t  positionCommandCode = 0;

      // If there is a file to recall
      if(GatewayTxRx::getFileToRecallFromGateway(m_cuuid, m_volReqId,
        m_gatewayHost, m_gatewayPort, filePath, nsHost, fileId, tapeFseq,
        blockId, positionCommandCode)) {

        castor::dlf::Param params[] = {
          castor::dlf::Param("volReqId"           , m_volReqId         ),
          castor::dlf::Param("gatewayHost"        , m_gatewayHost      ),
          castor::dlf::Param("gatewayPort"        , m_gatewayPort      ),
          castor::dlf::Param("filePath"           , filePath           ),
          castor::dlf::Param("nsHost"             , nsHost             ),
          castor::dlf::Param("fileId"             , fileId             ),
          castor::dlf::Param("tapeFseq"           , tapeFseq           ),
          castor::dlf::Param("blockId[0]"         , blockId[0]         ),
          castor::dlf::Param("blockId[1]"         , blockId[1]         ),
          castor::dlf::Param("blockId[2]"         , blockId[2]         ),
          castor::dlf::Param("blockId[3]"         , blockId[3]         ),
          castor::dlf::Param("positionCommandCode", positionCommandCode)};
        castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
          AGGREGATOR_FILE_TO_RECALL, params);

        char tapeFileId[CA_MAXPATHLEN+1];
        utils::setBytes(tapeFileId, '\0');
        RtcpTxRx::giveFileToRtcpd(m_cuuid, m_volReqId, socketFd,
          RTCPDNETRWTIMEOUT, WRITE_DISABLE, filePath, body.tapePath,
          RECORDFORMAT, tapeFileId, RECALLUMASK, positionCommandCode, tapeFseq,
          nsHost, fileId, blockId);

        RtcpTxRx::askRtcpdToRequestMoreWork(m_cuuid, m_volReqId, body.tapePath,
          socketFd, RTCPDNETRWTIMEOUT, WRITE_DISABLE);

        RtcpTxRx::tellRtcpdEndOfFileList(m_cuuid, m_volReqId, socketFd,
          RTCPDNETRWTIMEOUT);

        // Asynchronus Request More Work ACK
        RtcpAcknowledgeMsg ackMsg;
        ackMsg.magic   = RTCOPY_MAGIC;
        ackMsg.reqType = RTCP_FILE_REQ;
        ackMsg.status  = 0;
        RtcpTxRx::sendRtcpAcknowledge(m_cuuid, m_volReqId, socketFd,
          RTCPDNETRWTIMEOUT,ackMsg);

      // Else there is no file to recall
      } else {

        castor::dlf::Param params[] = {
          castor::dlf::Param("volReqId"   , m_volReqId   ),
          castor::dlf::Param("gatewayHost", m_gatewayHost),
          castor::dlf::Param("gatewayPort", m_gatewayPort)};
        castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
          AGGREGATOR_NO_MORE_FILES_TO_RECALL, params);

        // Tell RTCPD there is no file by sending an empty file list
        RtcpTxRx::tellRtcpdEndOfFileList(m_cuuid, m_volReqId, socketFd,
          RTCPDNETRWTIMEOUT);

        // Asynchronus Request More Work ACK
        RtcpAcknowledgeMsg ackMsg;
        ackMsg.magic   = RTCOPY_MAGIC;
        ackMsg.reqType = RTCP_FILE_REQ;
        ackMsg.status  = 0;
        RtcpTxRx::sendRtcpAcknowledge(m_cuuid, m_volReqId, socketFd,
          RTCPDNETRWTIMEOUT,ackMsg);
      }
    } // Else recalling
    break;
  case RTCP_POSITIONED:
    {
      castor::dlf::Param params[] = {
        castor::dlf::Param("volReqId", m_volReqId),
        castor::dlf::Param("filePath", body.filePath),
        castor::dlf::Param("tapePath", body.tapePath)};
      castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
        AGGREGATOR_TAPE_POSITIONED_FILE_REQ, params);

      RtcpAcknowledgeMsg ackMsg;
      ackMsg.magic   = RTCOPY_MAGIC;
      ackMsg.reqType = RTCP_FILE_REQ;
      ackMsg.status  = 0;
      RtcpTxRx::sendRtcpAcknowledge(m_cuuid, m_volReqId, socketFd,
        RTCPDNETRWTIMEOUT, ackMsg);
    }
    break;
  case RTCP_FINISHED:
    {
      castor::dlf::Param params[] = {
        castor::dlf::Param("volReqId", m_volReqId),
        castor::dlf::Param("filePath", body.filePath),
        castor::dlf::Param("tapePath", body.tapePath)};
      castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
        AGGREGATOR_FILE_TRANSFERED, params);

      RtcpAcknowledgeMsg ackMsg;
      ackMsg.magic   = RTCOPY_MAGIC;
      ackMsg.reqType = RTCP_FILE_REQ;
      ackMsg.status  = 0;
      RtcpTxRx::sendRtcpAcknowledge(m_cuuid, m_volReqId, socketFd,
        RTCPDNETRWTIMEOUT, ackMsg);

      // Connect to Gateway and tell "finish"
    }
    break;
  default:
    {
      TAPE_THROW_CODE(EBADMSG,
           ": Received unexpected file request process status 0x"
        << std::hex << body.procStatus
        << "(" << utils::procStatusToStr(body.procStatus) << ")");
    }
  }
}


//-----------------------------------------------------------------------------
// rtcpFileErrReqCallback
//-----------------------------------------------------------------------------
void castor::tape::aggregator::BridgeProtocolEngine::rtcpFileErrReqCallback(
  const MessageHeader &header, const int socketFd, bool &receivedENDOF_REQ)
  throw(castor::exception::Exception) {

  RtcpFileRqstErrMsgBody body;

  RtcpTxRx::receiveRtcpFileRqstErrBody(m_cuuid, m_volReqId, socketFd,
    RTCPDNETRWTIMEOUT, header, body);

  RtcpAcknowledgeMsg ackMsg;
  ackMsg.magic   = RTCOPY_MAGIC;
  ackMsg.reqType = RTCP_FILEERR_REQ;
  ackMsg.status  = 0;
  RtcpTxRx::sendRtcpAcknowledge(m_cuuid, m_volReqId, socketFd,
    RTCPDNETRWTIMEOUT, ackMsg);

  TAPE_THROW_CODE(body.err.errorCode,
    ": Received an error from RTCPD: " << body.err.errorMsg);
}


//-----------------------------------------------------------------------------
// rtcpTapeReqCallback
//-----------------------------------------------------------------------------
void castor::tape::aggregator::BridgeProtocolEngine::rtcpTapeReqCallback(
  const MessageHeader &header, const int socketFd, bool &receivedENDOF_REQ)
  throw(castor::exception::Exception) {

  RtcpTapeRqstMsgBody body;

  RtcpTxRx::receiveRtcpTapeRqstBody(m_cuuid, m_volReqId, socketFd,
    RTCPDNETRWTIMEOUT, header, body);

  castor::dlf::Param params[] = {
    castor::dlf::Param("volReqId", m_volReqId)};
  castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
    AGGREGATOR_TAPE_POSITIONED_TAPE_REQ, params);

  // Acknowledge tape request
  RtcpAcknowledgeMsg ackMsg;
  ackMsg.magic   = RTCOPY_MAGIC;
  ackMsg.reqType = RTCP_TAPE_REQ;
  ackMsg.status  = 0;
  RtcpTxRx::sendRtcpAcknowledge(m_cuuid, m_volReqId, socketFd,
    RTCPDNETRWTIMEOUT, ackMsg);
}


//-----------------------------------------------------------------------------
// rtcpTapeErrReqCallback
//-----------------------------------------------------------------------------
void
  castor::tape::aggregator::BridgeProtocolEngine::rtcpTapeErrReqCallback(
  const MessageHeader &header, const int socketFd, bool &receivedENDOF_REQ)
  throw(castor::exception::Exception) {

  RtcpTapeRqstErrMsgBody body;

  RtcpTxRx::receiveRtcpTapeRqstErrBody(m_cuuid, m_volReqId, socketFd,
    RTCPDNETRWTIMEOUT, header, body);

  if(body.err.errorCode == 0) {
    castor::dlf::Param params[] = {
      castor::dlf::Param("volReqId", m_volReqId)};
    castor::dlf::dlf_writep(m_cuuid, DLF_LVL_SYSTEM,
      AGGREGATOR_TAPE_POSITIONED_TAPE_REQ, params);
  } else {
    char codeStr[STRERRORBUFLEN];
    sstrerror_r(body.err.errorCode, codeStr, sizeof(codeStr));

    TAPE_THROW_CODE(body.err.errorCode,
      ": Received an error from RTCPD"
      ": errorCode=" << body.err.errorCode <<
      ": errorMsg=" << body.err.errorMsg  <<
      ": sstrerror_r=" << codeStr);
  }

  RtcpAcknowledgeMsg ackMsg;
  ackMsg.magic   = RTCOPY_MAGIC;
  ackMsg.reqType = RTCP_TAPEERR_REQ;
  ackMsg.status  = 0;
  RtcpTxRx::sendRtcpAcknowledge(m_cuuid, m_volReqId, socketFd,
    RTCPDNETRWTIMEOUT, ackMsg);
}


//-----------------------------------------------------------------------------
// rtcpEndOfReqCallback
//-----------------------------------------------------------------------------
void castor::tape::aggregator::BridgeProtocolEngine::rtcpEndOfReqCallback(
  const MessageHeader &header, const int socketFd, bool &receivedENDOF_REQ)
  throw(castor::exception::Exception) {

  receivedENDOF_REQ = true;

  // Acknowledge RTCP_ENDOF_REQ message
  RtcpAcknowledgeMsg ackMsg;
  ackMsg.magic   = RTCOPY_MAGIC;
  ackMsg.reqType = RTCP_ENDOF_REQ;
  ackMsg.status  = 0;
  RtcpTxRx::sendRtcpAcknowledge(m_cuuid, m_volReqId, socketFd,
    RTCPDNETRWTIMEOUT, ackMsg);
}

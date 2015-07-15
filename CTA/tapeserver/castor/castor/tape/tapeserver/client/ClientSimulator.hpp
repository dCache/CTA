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

#pragma once

#include "castor/tape/tpcp/TpcpCommand.hpp"
#include "castor/tape/tapegateway/ClientType.hpp"
#include "castor/tape/tapegateway/FileToRecallStruct.hpp"
#include "castor/tape/tapegateway/FileToMigrateStruct.hpp"
#include "castor/tape/tapegateway/VolumeMode.hpp"
#include <queue>

namespace castor {
namespace tape {
namespace tapeserver {
namespace client {
  /**
   * A class, reusing code of TpcpCommand which simulates the tape server
   * part of the client communication with the server. The constructor
   * will hence setup a client callback socket and wait. All the tape mounting
   * logic is hence skipped. We will then in parallel start a tape session.
   */
  class ClientSimulator: public tpcp::TpcpCommand {
  public:
    typedef enum {
      NotEmpty,
      EmptyOnVolReq,
      EmptyOnGetMoreWork
    } EmptyMount_t;
    ClientSimulator(uint32_t volReqId, const std::string & vid, 
            const std::string & density, tapegateway::ClientType clientType,
            tapegateway::VolumeMode volumeMode, EmptyMount_t emptyMount = NotEmpty);
    
    virtual ~ClientSimulator() throw() {}
    
    struct ipPort {
      ipPort(uint32_t i, uint16_t p): ip(i), port(p) {}
      union {
        uint32_t ip;
        struct {
          uint8_t a;
          uint8_t b;
          uint8_t c;
          uint8_t d;
        };
      };
      uint16_t port;
    };
    struct ipPort getCallbackAddress() {
      unsigned short port = 0;
      /* This is a workaround for the usage of unsigned long for ips in castor
       * (it's not fine anymore on 64 bits systems).
       */
      unsigned long ip = 0;
      m_callbackSock.getPortIp(port, ip);
      return ipPort(ip,port);
    }
    void sessionLoop() {
      processFirstRequest();
      while (processOneRequest());
      m_callbackSock.close();
    }
    void addFileToRecall(tapegateway::FileToRecallStruct & ftr, uint64_t size) {
      m_filesToRecall.push(ftr);
      m_recallSizes.push(size);
    }
    void addFileToMigrate(tapegateway::FileToMigrateStruct & ftm) {
      m_filesToMigrate.push(ftm);
    }
    
    /**
     * Container where the migration result pairs (fseq, checksum) are 
     * stored.
     */
    std::map<uint64_t, uint64_t> m_receivedChecksums;
    /**
     * Container where the error codes pairs (fseq, errorCode) are 
     * stored.
     */
    std::map<uint64_t, uint64_t> m_receivedErrorCodes;
    /**
     * The resulting error code of the session.
     */
    int m_sessionErrorCode;
  protected:
    // Place holders for pure virtual members of TpcpCommand we don't
    // use in the simulator
    virtual void usage(std::ostream &) const throw() {}
    virtual void parseCommandLine(const int, char **)
       {}
    virtual void checkAccessToDisk()
      const  {}
    virtual void checkAccessToTape()
      const  {}
    virtual void requestDriveFromVdqm(char *const)
       {}
    virtual void performTransfer()  {}
    
    
    // The functions we actually implement in the simulator
    virtual void sendVolumeToTapeServer(
      const tapegateway::VolumeRequest &volumeRequest,
      castor::io::AbstractTCPSocket    &connection)
      const  {}
    virtual bool dispatchMsgHandler(castor::IObject *const obj,
      castor::io::AbstractSocket &sock)  {
      return false;
    }
    
  private:
    // Process the first request which should be getVolume
    void processFirstRequest() ;
    // Process requests (recall or migration) until we receive and end of session
    // This helper function will process one request and return true if there is 
    // still more to process (that is if the end session is not signaled yet)
    bool processOneRequest() ;
    // Notify the client
    void sendEndNotificationErrorReport(
    const uint64_t             transactionId,
    const int                  errorCode,
    const std::string          &errorMessage,
    castor::io::AbstractSocket &sock)
    throw();
    const std::string m_vid;
    const std::string m_volLabel;
    const std::string m_density;
    std::queue<tapegateway::FileToRecallStruct> m_filesToRecall;
    std::queue<uint64_t> m_recallSizes;
    std::queue<tapegateway::FileToMigrateStruct> m_filesToMigrate;
    const castor::tape::tapegateway::ClientType m_clientType;
    const castor::tape::tapegateway::VolumeMode m_volumeMode;
    EmptyMount_t m_emptyMount;
  };
}
}
}
}

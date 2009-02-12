/******************************************************************************
 *                castor/tape/aggregator/VdqmRequestHandlerThread.hpp
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

#ifndef CASTOR_TAPE_AGGREGATOR_VDQMREQUESTHANDLERTHREAD_HPP
#define CASTOR_TAPE_AGGREGATOR_VDQMREQUESTHANDLERTHREAD_HPP 1

#include "castor/io/ServerSocket.hpp"
#include "castor/server/IThread.hpp"
#include "castor/tape/aggregator/MessageHeader.hpp"
#include "castor/tape/aggregator/RcpJobRqstMsgBody.hpp"
#include "castor/tape/aggregator/TapeDiskRqstHandler.hpp"

#include <list>


namespace castor {
namespace tape {
namespace aggregator {

  /**
   * Handles the requests from the VDQM server.
   */
  class VdqmRequestHandlerThread : public castor::server::IThread {

  public:

    /**
     * Destructor
     */
    ~VdqmRequestHandlerThread() throw();

    /**
     * Initialization of the thread.
     */
    virtual void init() throw();

    /**
     * Main work for this thread.
     */
    virtual void run(void *param) throw();

    /**
     * Convenience method to stop the thread.
     */
    virtual void stop() throw();


  private:

    /**
     * Handler of requests from the tape and disk IO threads of RTCPD.
     */
    TapeDiskRqstHandler m_tapeDiskRqstHandler;

    /**
     * Processes a job submission request message from the specified
     * connection.
     *
     * If the processing of the request is successfull then this function will
     * have passed the request onto RTCPD and will have given as output the
     * contents of the job submission request.
     *
     * @param cuuid The ccuid to be used for logging.
     * @param vdqmSocketFd The socket file descriptor of the connection from
     * which the request is to be processed.
     * @param jobRequest Out parameter.  The job request message structure
     * which will be filled by this function.
     * @param rtcpdCallbackSocketFd The file descriptor of the listener socket
     * to be used to accept callback connections from RTCPD.
     */
    void processJobSubmissionRequest(const Cuuid_t &cuuid,
      const int vdqmSocketFd, RcpJobRqstMsgBody &jobRequest,
      const int rtcpdCallbackSocketFd) throw(castor::exception::Exception);

    /**
     * Processes an incomming error message on the initial RTCPD connection.
     *
     * @param cuuid The ccuid to be used for logging.
     * @param jobRequest The job request message received from the VDQM.  To be
     * used for logging purposes.
     * @param rtcpdInitialSocketFd The socket file descriptor of initial RTCPD
     * connection.
     */
    void processErrorOnInitialRtcpdConnection(const Cuuid_t &cuuid,
      const RcpJobRqstMsgBody &vdqmJobRequest,
      const int rtcpdInitialSocketFd) throw(castor::exception::Exception);

    /**
     * Accepts an RTCPD connection using the specified listener socket and adds
     * the newly created connected socket to the specified list of connected
     * sockets.
     *
     * @param cuuid The ccuid to be used for logging.
     * @param vdqmJobRequest The job request message received from the VDQM.
     * @param rtcpdCallbackSocketFd The file descriptor of the listener socket
     * to be used to accept callback connections from RTCPD.
     * @param connectedSocketFds In/out parameter - The list of connected
     * socket file descriptors.
     */
    void acceptRtcpdConnection(const Cuuid_t &cuuid,
      const RcpJobRqstMsgBody &vdqmJobRequest,
      const int rtcpdCallbackSocketFd, std::list<int> &connectedSocketFds)
      throw(castor::exception::Exception);

    /**
     * Processes the following RTCPD sockets:
     * <ul>
     * <li>The connected socket of the initial RTCPD connection
     * <li>The RTCPD callback listener socket
     * <li>The connected sockets of the tape and disk I/O threads
     * </ul>
     *
     * @param cuuid The ccuid to be used for logging.
     * @param vdqmJobRequest The job request message received from the VDQM.
     * @param rtcpdCallbackSocketFd The file descriptor of the listener socket
     * to be used to accept callback connections from RTCPD.
     * @param rtcpdInitialSocketFd The socket file descriptor of initial RTCPD
     * connection.
     */
    void processRtcpdSockets(const Cuuid_t &cuuid,
      const RcpJobRqstMsgBody &vdqmJobRequest,
      const int rtcpdCallbackSocketFd, const int rtcpdInitialSocketFd)
      throw(castor::exception::Exception);

    /**
     * Exchanges the request and volume information between the RTCPD and the
     * tape gateway by carrying out the* following steps in order:
     * <ol>
     * <li> Get request information from RTCPD.
     * <li> Give request information to tape gateway and get volume information
     * in return.
     * <li> Give volume information to RTCPD.
     * </ol>
     *
     * @param cuuid The ccuid to be used for logging.
     * @param jobRequest The job request message received from the VDQM.
     * @param rtcpdInitialSocketFd The socket file descriptor of initial RTCPD
     * connection.
     * @param mode Out parameter: The tape access mode.
     */
    void exchangeRequestAndVolumeInfo(const Cuuid_t &cuuid,
      const RcpJobRqstMsgBody &vdqmJobRequest, const int rtcpdInitialSocketFd,
      uint32_t &mode) throw(castor::exception::Exception);

    /**
     * Coordinates the remote copy operations by sending and recieving the
     * necessary messages.
     *
     * @param cuuid The ccuid to be used for logging.
     * @param jobRequest The job request message received from the VDQM.
     * @param rtcpdCallbackSocketFd The file descriptor of the listener socket
     * to be used to accept callback connections from RTCPD.
     */
    void coordinateRemoteCopy(const Cuuid_t &cuuid,
      const RcpJobRqstMsgBody &vdqmJobRequest,
      const int rtcpdCallbackSocketFd) throw(castor::exception::Exception);

    /**
     * Throws an exception if the peer host associated with the specified
     * socket is not an authorised RCP job submitter.
     *
     * @param socketFd The socket file descriptor.
     */
    void checkRcpJobSubmitterIsAuthorised(const int socketFd)
      throw(castor::exception::Exception);

  }; // class VdqmRequestHandlerThread

} // namespace aggregator
} // namespace tape
} // namespace castor

#endif // CASTOR_TAPE_AGGREGATOR_VDQMREQUESTHANDLERTHREAD_HPP

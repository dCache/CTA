/******************************************************************************
 *                castor/tape/aggregator/RtcpdHandlerThread.hpp
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

#ifndef CASTOR_TAPE_AGGREGATOR_RTCPDREQUESTHANDLERTHREAD_HPP
#define CASTOR_TAPE_AGGREGATOR_RTCPDREQUESTHANDLERTHREAD_HPP 1

#include "castor/io/ServerSocket.hpp"
#include "castor/server/IThread.hpp"
#include "castor/server/Queue.hpp"

#include <map>


namespace castor {
namespace tape {
namespace aggregator {

  /**
   * Handles the RTCPD->VDQM protocol.
   */
  class RtcpdHandlerThread : public castor::server::IThread {

  public:

    /**
     * Constructor
     */
    RtcpdHandlerThread() throw();

    /**
     * Destructor
     */
    ~RtcpdHandlerThread() throw();

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
     * Get the VID associated with the remote copy job from RTCPD.
     *
     * @param socket The socket of the connection with RTCPD.
     * @return The VID.
     */
    std::string getVidFromRtcpd(castor::io::AbstractTCPSocket &socket)
      throw(castor::exception::Exception);

  }; // class RtcpdHandlerThread

} // namespace aggregator
} // namespace tape
} // namespace castor

#endif // CASTOR_TAPE_AGGREGATOR_RTCPDREQUESTHANDLERTHREAD_HPP

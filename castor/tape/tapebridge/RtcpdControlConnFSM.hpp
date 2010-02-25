/******************************************************************************
 *                      castor/tape/tapebridge/RtcpdControlConnFSM.hpp
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
#ifndef _CASTOR_TAPE_TAPEBRIDGE_RTCPDCONTROLCONNFSM_HPP_
#define _CASTOR_TAPE_TAPEBRIDGE_RTCPDCONTROLCONNFSM_HPP_

namespace castor     {
namespace tape       {
namespace tapebridge {
    
/**
 */
class RtcpdControlConnFSM {
public:

  enum State {
    IDLE,
    SENDING_REQUEST_FOR_FILE_TO_CLIENT,
    WAITING_FOR_FILE_FROM_CLIENT,
    SENDING_FILE_TO_RTCPD,
    SENDING_NO_MORE_FILES_TO_RTCPD,
    SENDING_ACK_OF_TRANSFERED_TO_RTCPD,
    SENDING_TRANSFERED_TO_CLIENT,
    WAITING_FOR_ACK_FROM_CLIENT
  };

  /**
   * operator() which takes no parameters and returns a boolean.
   */
  virtual bool operator()() = 0;

  /**
   * Virtual destructor.
   */
  virtual ~RtcpdControlConnFSM() throw();

}; // class RtcpdControlConnFSM

} // namespace tapebridge
} // namespace tape
} // namespace castor      

#endif // _CASTOR_TAPE_TAPEBRIDGE_RTCPDCONTROLCONNFSM_HPP_

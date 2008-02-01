/******************************************************************************
 *                castor/vdqm/RTCPJobSubmitterThread.hpp
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
 * @author castor dev team
 *****************************************************************************/

#ifndef CASTOR_VDQM_RTCPJOBSUBMITTERTHREAD_HPP
#define CASTOR_VDQM_RTCPJOBSUBMITTERTHREAD_HPP 1

#include "castor/server/SelectProcessThread.hpp"


namespace castor {

  namespace vdqm {

    // Forward declarations
    class IVdqmSvc;
    class TapeDrive;
    class TapeRequest;


    /**
     * Instances of this class submit remote tape copy jobs to RTPCD based on
     * the matched "tape request / free drive" pairs they find in the VDQM
     * database.
     */
    class RTCPJobSubmitterThread :
    public virtual castor::server::SelectProcessThread {

    public:

      /**
       * Constructor
       */
      RTCPJobSubmitterThread() throw();

      /**
       * Destructor
       */
      ~RTCPJobSubmitterThread() throw();

      /**
       * Performs the select query that tries to find a matched
       * "tape request / free drive" pair.
       */
      virtual castor::IObject* select() throw();

      /**
       * Processes the results of the select.
       *
       * @param param The IObject returned by select
       */
      virtual void process(castor::IObject* param) throw();


    private:

      /**
       * Submits the corresponding remote tape job of the specified tape
       * request to the RTCPD.
       *
       * WARNING: The following links should be present:
       * <ul>
       * <li>Tape request -> set of client indentification data</li>
       * <li>Tape request -> tape drive</li>
       * <li>Tape drive of tape request -> device group name</li>
       * <li>Tape drive of tape request -> tape server</li>
       * </ul>
       */
      void submitJobToRTCPD(castor::vdqm::TapeRequest* request)
        throw(castor::exception::Exception);

      /**
       * Returns a pointer to the DbVdqmSvc or throws an exception if it cannot.
       *
       * Please note that this method never returns NULL.  The method returns a
       * non-zero pointer or it throws an exception.
       */
      castor::vdqm::IVdqmSvc *getDbVdqmSvc()
        throw(castor::exception::Exception);

    }; // class RTCPJobSubmitterThread

  } // end namespace vdqm

} //end namespace castor

#endif // CASTOR_VDQM_RTCPJOBSUBMITTERTHREAD_HPP

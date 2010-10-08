
/******************************************************************************
 *                     VdqmRequestsCheckerThread.hpp
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
 * @(#)$RCSfile: VdqmRequestsCheckerThread.hpp,v $ $Author: gtaur $
 *
 *
 *
 * @author Giulia Taurelli
 *****************************************************************************/

#ifndef VDQMREQUESTSCHECKER_THREAD_HPP
#define VDQMREQUESTSCHECKER_THREAD_HPP 1

#include <u64subr.h>
#include "castor/tape/tapegateway/TapeGatewayRequest.hpp"
#include "castor/BaseObject.hpp"
#include "castor/server/IThread.hpp"

namespace castor     {
namespace tape       {
namespace tapegateway{
  
    /**
     *  VdqmRequestsChecker tread.
     */
    
  class VdqmRequestsCheckerThread : public virtual castor::server::IThread,
                                    public castor::BaseObject { 
    u_signed64 m_timeOut;
  public:
	
    VdqmRequestsCheckerThread(u_signed64 timeOut);
    virtual ~VdqmRequestsCheckerThread() throw() {};

    /**
     * Initialization of the thread.
     */
    virtual void init() {}

    /**
     * Main work for this thread
     */
    virtual void run(void* param);

    /**
     * Stop of the thread
     */
    virtual void stop() {}

  };

} // end of tapegateway
} // end of namespace tape
} // end of namespace castor

#endif // VDQMREQUESTSCHECKER_THREAD_HPP

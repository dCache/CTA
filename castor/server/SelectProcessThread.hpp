/******************************************************************************
 *                      SelectProcessThread.hpp
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
 * @(#)$RCSfile: SelectProcessThread.hpp,v $ $Revision: 1.9 $ $Release$ $Date: 2007/10/22 17:57:17 $ $Author: itglp $
 *
 * Base thread for the select/process model: it loops until select() returns
 * something to do. If stop() is called, the underlying database connection is dropped.
 *
 * @author Giuseppe Lo Presti
 *****************************************************************************/

#ifndef CASTOR_SERVER_SELECTPROCESSTHREAD_HPP
#define CASTOR_SERVER_SELECTPROCESSTHREAD_HPP 1

#include <iostream>
#include <string>
#include "castor/IObject.hpp"
#include "castor/server/IThread.hpp"
#include "castor/BaseObject.hpp"

namespace castor {

 namespace server {

  /**
   * Basic select/process thread for internal stager services.
   */
  class SelectProcessThread : public virtual IThread, public castor::BaseObject {
  public:

    /**
     * Standard constructor
     */
    SelectProcessThread() : m_stopped(false) {};

    /**
     * Thread initialization, empty in this case
     */
    virtual void init() {};

    /**
     * Select part of the service
     */
    virtual castor::IObject* select() throw() = 0;

    /**
     * Process part of the service
     */
    virtual void process(castor::IObject* param) throw() = 0;

    /**
     * Main work for this thread
     */
    virtual void run(void* param);

    /**
     * Mark the threads as to be stopped
     */
    virtual void stop();
    
  private:
  
    /// flag to stop the activity of all threads based on this class
    bool m_stopped;

  };

 } // end of namespace server

} // end of namespace castor


#endif // CASTOR_SERVER_SELECTPROCESSTHREAD_HPP

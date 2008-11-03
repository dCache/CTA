/******************************************************************************
 *                      VdqmServer.hpp
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
 * @author Matthias Braeger
 *****************************************************************************/

#ifndef CASTOR_VDQM_VDQMSERVER_HPP
#define CASTOR_VDQM_VDQMSERVER_HPP 1

#include "castor/server/BaseDaemon.hpp"
#include "castor/vdqm/exception/InvalidConfigEntry.hpp"
#include <string>

namespace castor {

  namespace vdqm {
  	
    /**
     * The Volume and Drive Queue Manager.
     */
    class VdqmServer : public castor::server::BaseDaemon {
    public:
    
      /**
       * Default number of threads in the server thread pool
       */    
      static const int DEFAULT_THREAD_NUMBER = 20;

      /**
       * Constructor
       */
      VdqmServer() throw();

      /**
       * Parses the command line and sets the server options accordingly.
       *
       * In case of an error this method writes the appriopriate error messages
       * to both standard error and DLF and then calls exit with a value of 1.
       *
       * @param cuuid the cuuid to be used for logging
       * @param argc the number of command-line arguments
       * @param argv the command-line arguments
       */
      void parseCommandLine(Cuuid_t &cuuid, int argc, char *argv[]) throw();

      /**
       * Initialises the database service.
       *
       * In case of an error this method writes the appriopriate error messages
       * to both standard error and DLF and then calls exit with a value of 1.
       *
       * @param cuuid the cuuid to be used for logging
       */
      void initDatabaseService(Cuuid_t &cuuid);

      /**
       * Returns the scheduler timeout, in other words the time a scheduler
       * thread will sleep when there is no work to be done.
       */
      int getSchedulerTimeout()
        throw(castor::vdqm::exception::InvalidConfigEntry);

      /**
       * Returns the RTCP job submitter timeout, in other words the time an
       * RTCP job submitter thread will sleep when there is no work to be done.
       */
      int getRTCPJobSubmitterTimeout()
        throw(castor::vdqm::exception::InvalidConfigEntry);

      /**
       * Returns the port on which the server will listen.
       */
      int getListenPort() throw(castor::vdqm::exception::InvalidConfigEntry);

      /**
       * Returns the UPD port on which the server will listen fori
       * notifications.
       */
      int getNotifyPort()
        throw(castor::vdqm::exception::InvalidConfigEntry);

      /**
       * Returns the number of threads in the request handler thread pool.
       */
      int getRequestHandlerThreadNumber();

      /**
       * Returns the number of threads in the remote tape copy job submitter
       * thread pool.
       */
      int getRTCPJobSubmitterThreadNumber();

      /**
       * Returns the number of threads in the scheduler thread pool.
       */
      int getSchedulerThreadNumber();
      

    private:

      /**
       * DLF message strings.
       */
      static castor::dlf::Message s_dlfMessages[];

      /**
       * Number of request handler threads.
       */
      int m_requestHandlerThreadNumber;

      /**
       * Number of remote tape copy job submitter threads.
       */
      int m_RTCPJobSubmitterThreadNumber;

      /**
       * Number of scheduler threads.
       */
      int m_schedulerThreadNumber;
    
      /**
       * Prints out the command-line usage message for the VDQM server
       * application.
       */
      void usage(std::string programName) throw();
    
    }; // class VdqmServer

  } // namespace vdqm

} // namespace castor


#endif // CASTOR_VDQM_VDQMSERVER_HPP

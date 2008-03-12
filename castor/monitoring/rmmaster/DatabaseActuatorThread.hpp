/******************************************************************************
 *                      DatabaseActuatorThread.hpp
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
 * @(#)$RCSfile: DatabaseActuatorThread.hpp,v $ $Author: waldron $
 *
 * The DatabaseActuator thread of the RmMaster daemon. It updates the stager
 * database with monitoring data
 *
 * @author castor-dev team
 *****************************************************************************/

#ifndef RMMASTER_DATABASEACTUATORTHREAD_HPP
#define RMMASTER_DATABASEACTUATORTHREAD_HPP 1

// Include files
#include "castor/server/IThread.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/monitoring/rmmaster/LSFStatus.hpp"

namespace castor {

  namespace monitoring {

    // Forward declarations
    class ClusterStatus;

    namespace rmmaster {

      // Forward declarations
      class IRmMasterSvc;

      /**
       * DatabaseActuator  tread.
       */
      class DatabaseActuatorThread : public castor::server::IThread {

      public:

        /**
         * Constructor
         * @param clusterStatus pointer to the status of the cluster
         */
        DatabaseActuatorThread(ClusterStatus* clusterStatus)
          throw (castor::exception::Exception);

        /**
         * Destructor
         */
        virtual ~DatabaseActuatorThread() throw();

	/// Not implemented
        virtual void init() {};

	/**
	 * Method called to synchornize the shared memory with the stager
	 * database.
	 */
	virtual void run(void*) throw();

	/// Not implemented
        virtual void stop() throw() {};

      private:

        /// rmmaster service to call the oracle procedures
        castor::monitoring::rmmaster::IRmMasterSvc* m_rmMasterService;

        /// Machine Status List
        castor::monitoring::ClusterStatus* m_clusterStatus;

	/// The previous recorded name of the LSF master
	std::string m_prevMasterName;

      };

    } // end of namespace rmmaster

  } // end of namespace monitoring

} // end of namespace castor

#endif // RMMASTER_DATABASEACTUATORTHREAD_HPP

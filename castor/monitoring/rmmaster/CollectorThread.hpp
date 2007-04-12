/******************************************************************************
 *                      CollectorThread.hpp
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
 * @(#)$RCSfile$ $Author$
 *
 * The Collector thread of the RmMaster daemon.
 * It collects the data from the different nodes and updates a shared
 * memory representation of it
 *
 * @author castor-dev team
 *****************************************************************************/

#ifndef RMMASTER_COLLECTORTHREAD_HPP
#define RMMASTER_COLLECTORTHREAD_HPP 1

#include "castor/server/IThread.hpp"
#include "castor/monitoring/ClusterStatus.hpp"

namespace castor {

  namespace monitoring {

    // Forward declarations
    class DiskServerStateReport;
    class DiskServerMetricsReport;
    class ClusterStatus;
    namespace admin {
      class DiskServerAdminReport;
      class FileSystemAdminReport;
    }

    namespace rmmaster {

      /**
       * Collector  tread.
       */
      class CollectorThread : public castor::server::IThread {

      public:

        /**
         * constructor
         * @param clusterStatus pointer to the status of the cluster
         */
        CollectorThread(castor::monitoring::ClusterStatus* clusterStatus);

        /**
         * Method called once per request, where all the code resides
         * @param param the socket obtained from the calling thread pool
         */
        virtual void run(void *param) throw();

        /// not implemented
        virtual void stop() {};

      private:

        /**
         * handles state updates
         * @param state the new state
         */
        void handleStateUpdate
        (castor::monitoring::DiskServerStateReport* state)
          throw (castor::exception::Exception);

        /**
         * handles metrics updates
         * @param metrics the new metrics
         */
        void handleMetricsUpdate
        (castor::monitoring::DiskServerMetricsReport* metrics)
          throw (castor::exception::Exception);

        /**
         * handles DiskServer admin updates
         * @param admin the new admin report
         */
        void handleDiskServerAdminUpdate
        (castor::monitoring::admin::DiskServerAdminReport* admin)
          throw (castor::exception::Exception);

        /**
         * handles FileSystem admin updates
         * @param admin the new admin report
         */
        void handleFileSystemAdminUpdate
        (castor::monitoring::admin::FileSystemAdminReport* admin)
          throw (castor::exception::Exception);

	/*
	 * gets an iterator on a specific machine from the
	 * ClusterStatus map or create one if the machine is
	 * not existing in the map yet
	 * @param name name of the machine
	 * @param it the returned iterator
	 * @return true if the machine was found or created successfully,
	 * false if the creation failed
	 */
	bool getOrCreateDiskServer
	(std::string name,
	 castor::monitoring::ClusterStatus::iterator& it) throw();

	/*
	 * gets an iterator on a specific fileSystem from a
	 * DiskServerStatus map or create one if the fileSystem is
	 * not existing in the map yet
	 * @param it dss the DiskServerStatus map
	 * @param mountPoint the mountPoint of the fileSystem
	 * @param it2 the returned iterator
	 * @return true if the fileSystem was found or created successfully,
	 * false if the creation failed
	 */
	bool getOrCreateFileSystem
	(castor::monitoring::DiskServerStatus& dss,
	 std::string mountPoint,
	 castor::monitoring::DiskServerStatus::iterator& it2) throw();

      private:

        // Machine Status List
        castor::monitoring::ClusterStatus* m_clusterStatus;

      };

    } // end of namespace rmmaster

  } // end of namespace monitoring

} // end of namespace castor

#endif // RMMASTER_COLLECTORTHREAD_HPP

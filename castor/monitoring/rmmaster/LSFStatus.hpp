/******************************************************************************
 *                      LSFStatus.hpp
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
 * @(#)$RCSfile: LSFStatus.hpp,v $ $Revision: 1.1 $ $Release$ $Date: 2008/03/12 10:42:14 $ $Author: waldron $
 *
 * Singleton used to access LSF related information
 *
 * @author Dennis Waldron
 *****************************************************************************/

#ifndef RMMASTER_LSF_STATUS_HPP
#define RMMASTER_LSF_STATUS_HPP 1

// Include files
#include "castor/exception/Exception.hpp"

// LSF headers
extern "C" {
  #ifndef LSBATCH_H
    #include "lsf/lssched.h"
    #include "lsf/lsbatch.h"
  #endif
}

namespace castor {

  namespace monitoring {

    namespace rmmaster {

      /**
       * LSFStatus singleton class
       */
      class LSFStatus {

      public:

	/**
	 * Static method to get this singleton's instance
	 */
	static castor::monitoring::rmmaster::LSFStatus* getInstance()
	  throw(castor::exception::Exception);

	/**
	 * Get information about the current status of LSF.
	 * @param production pointer to a bool value to update with a flag
	 * to indicate whether we are the production resource master.
	 * @param update flag to indicate whether this call is allowed to
	 * call LSF to update the information currently cached in memory.
	 * @exception Exception in case of error
	 */
	void getLSFStatus(bool &production,
			  bool update)
	  throw(castor::exception::Exception);

	/**
	 * Get information about the current status of LSF.
	 * @param production pointer to a boolean value to update with a flag
	 * to indicate whether we are the production resource master.
	 * @param masterName pointer to a string to store the current LSF
	 * master name.
	 * @param hostName pointer to a string to store the current hostname.
	 * @param update flag to indicate whether this call is allowed to
	 * call LSF to update the information currently cached in memory.
	 * @exception Exception in case of error
	 */
	void getLSFStatus(bool &production,
			  std::string &masterName,
			  std::string &hostName,
			  bool update)
	  throw(castor::exception::Exception);

      private:

	/// This singleton's instance
	static castor::monitoring::rmmaster::LSFStatus *s_instance;

	/**
	 * Default constructor. Initializes the LSF API
	 * @exception Exception in case of error
	 */
	LSFStatus() throw (castor::exception::Exception);

	/**
	 * Default destructor
	 */
	virtual ~LSFStatus() throw() {};

      private:

	/// The name of the LSF master
	std::string m_masterName;

	/// The previous name of the LSF master
	std::string m_prevMasterName;

	/// The last time the LSF master name was refreshed
	u_signed64 m_lastUpdate;

      };

    } // End of namespace rmmaster

  } // End of namespace monitoring

} // End of namespace castor

#endif // RMMASTER_LSF_STATUS_HPP

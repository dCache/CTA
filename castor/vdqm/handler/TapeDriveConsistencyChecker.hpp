/******************************************************************************
 *                      TapeDriveConsistencyChecker.hpp
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
 * @(#)RCSfile: TapeDriveConsistencyChecker.hpp  Revision: 1.0  Release Date: Jul 4, 2005  Author: mbraeger 
 *
 *
 *
 * @author Matthias Braeger
 *****************************************************************************/

#ifndef _TAPEDRIVECONSISTENCYCHECKER_HPP_
#define _TAPEDRIVECONSISTENCYCHECKER_HPP_

#include "castor/exception/Exception.hpp"
#include "BaseRequestHandler.hpp"
//#include "castor/vdqm/TapeDrive.hpp"

typedef struct vdqmdDrvReq vdqmDrvReq_t;

namespace castor {
	//Forward declaration

  namespace vdqm {

		//Forward declaration
		class TapeDrive;

		namespace handler {
	    /**
	     * The TapeDriveConsistencyChecker is only used by the TapeDriveHandler
	     * class. It verifies that new unit status is consistent with the
		   * current status of the drive.
		   * Note that this class is not used outside of this namespace!
	     */
	    class TapeDriveConsistencyChecker : public BaseRequestHandler {
				
				/**
				 * Like a real friend, TapeDriveHandler will respect the privacy 
				 * wishes of this class ;-)
				 */
				friend class TapeDriveHandler;
	
				public:
				
				  /**
				   * Does the consistency check. Please note, that this function can
				   * manipulate the TapeDrive object.
				   * 
				   * return false, if the consistency of the tape drive in the db
				   * and its unit is not given.
				   */
					bool checkConsistency() throw (castor::exception::Exception);
	
	
				protected:
	
		      /**
		       * Constructor
		       * 
		       * @param tapeDrive The tape Drive, which needs a consistency check
					 * @param driveRequest The TapeDriveRequest from the old protocol
					 * @param cuuid The unique id of the request. Needed for dlf
					 * @exception In case of error
		       */
					TapeDriveConsistencyChecker(TapeDrive* tapeDrive, 
													 vdqmDrvReq_t* driveRequest, Cuuid_t cuuid) throw();
					
		      /**
		       * Destructor
		       */
					virtual ~TapeDriveConsistencyChecker() throw();
						
				private:
					// Private variables
					TapeDrive* ptr_tapeDrive;
					vdqmDrvReq_t* ptr_driveRequest;
					Cuuid_t m_cuuid;
						
	    }; // class TapeDriveConsistencyChecker
    
  	} // end of namespace handler

  } // end of namespace vdqm

} // end of namespace castor

#endif //_TAPEDRIVECONSISTENCYCHECKER_HPP_

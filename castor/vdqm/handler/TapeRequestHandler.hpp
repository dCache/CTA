/******************************************************************************
 *                      TapeRequestHandler.hpp
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
 * @(#)RCSfile: TapeRequestHandler.hpp  Revision: 1.0  Release Date: Apr 19, 2005  Author: mbraeger 
 *
 *
 *
 * @author Matthias Braeger
 *****************************************************************************/

#ifndef _TAPEREQUESTHANDLER_HPP_
#define _TAPEREQUESTHANDLER_HPP_

// Local include
#include "BaseRequestHandler.hpp"

typedef struct newVdqmHdr newVdqmHdr_t;
typedef struct newVdqmVolReq newVdqmVolReq_t;

namespace castor {

  namespace vdqm {
  	
  	//Forward declaration
  	class OldProtocolInterpreter;
  	
  	namespace handler {
	
	    /**
	     * The TapeRequestHandler provides functions to handle all vdqm related
	     * tape request issues. It handles for example the VDQM_PING, VDQM_VOL_REQ
	     * and VDQM_DEL_VOLREQ
	     */
	    class TapeRequestHandler : public BaseRequestHandler {
	
				public:
	
		      /**
		       * Constructor
		       */
					TapeRequestHandler() throw(castor::exception::Exception);
					
		      /**
		       * Destructor
		       */
					virtual ~TapeRequestHandler() throw();
					
					/**
					 * This function replaces the old vdqm_NewVolReq() C-function. It stores
					 * the request into the data Base
					 * 
					 * @param header The header of the old Protocol
					 * @param volumeRequest The TapeRequest in the old protocol
					 * @param cuuid The unique id of the request. Needed for dlf
					 * @exception In case of error
					 */
					void newTapeRequest(newVdqmHdr_t *header, 
															newVdqmVolReq_t *volumeRequest,
															Cuuid_t cuuid) 
						throw (castor::exception::Exception);
						
					/**
					 * Deletes a TapeRequest from the DB. The request must have the same
					 * id like the VolReq  
					 * 
					 * @param volumeRequest The TapeRequest in the old protocol
					 * @param cuuid The unique id of the request. Needed for dlf
					 * @exception In case of error
					 */
					void deleteTapeRequest(newVdqmVolReq_t *volumeRequest, Cuuid_t cuuid) 
						throw (castor::exception::Exception);
						
					
						
					/**
					 * This function replaces the old vdqm_GetQueuePos() C-function. It returns
					 * in fact the queue position in the db.
					 *
					 * @param volumeRequest The TapeRequest in the old protocol
					 * @param cuuid The unique id of the request. Needed for dlf 
					 * @return 0<: The row number, 
		    	 *         0 : The request is handled at the moment from a TapeDrive, 
		    	 *         -1: if there is no entry for it.
					 * @exception In case of error
					 */
					int getQueuePosition(newVdqmVolReq_t *VolReq, Cuuid_t cuuid) 
						throw (castor::exception::Exception);

						
					/**
					 * This function replaces the old vdqm_GetVolQueue() C-function. 
					 * I looks into the data base for all stored tapeRequests and sends
					 * them back to the client via the OldProtocolInterpreter interface.
					 *
					 * @param volumeRequest The TapeRequest in the old protocol
					 * @param oldProtInterpreter The interface to send the queue to the client
					 * @param cuuid The unique id of the request. Needed for dlf 
					 * @exception In case of error
					 */
					void sendTapeRequestQueue(newVdqmHdr_t *header, 
																	 newVdqmVolReq_t *volumeRequest,
																	 newVdqmDrvReq_t *driveRequest, 
					                         OldProtocolInterpreter* oldProtInterpreter, 
					                         Cuuid_t cuuid) 
						throw (castor::exception::Exception);						
	
	    }; // class TapeRequestHandler
    
  	} // end of namespace handler

  } // end of namespace vdqm

} // end of namespace castor

#endif //_TAPEREQUESTHANDLER_HPP_

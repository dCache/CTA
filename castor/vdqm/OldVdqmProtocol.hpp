/******************************************************************************
 *                      OldVdqmProtocol.hpp
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
 * @(#)RCSfile: OldVdqmProtocol.hpp  Revision: 1.0  Release Date: Apr 18, 2005  Author: mbraeger 
 *
 * 
 *
 * @author Matthias Braeger
 *****************************************************************************/

#ifndef _OLDVDQMPROTOCOL_HPP_
#define _OLDVDQMPROTOCOL_HPP_

//Include files
#include "castor/BaseObject.hpp"
#include "castor/exception/Exception.hpp"


//Forward declarations
typedef struct vdqmVolReq vdqmVolReq_t;
typedef struct vdqmdDrvReq vdqmDrvReq_t;
typedef struct vdqmHdr vdqmHdr_t;
typedef struct vdqmnw vdqmnw_t;

namespace castor {

  namespace vdqm {

    /**
     * This class provides functions to handle the old VDQM protocol.
     * It is used from the VdqmServer class.
     */
    class OldVdqmProtocol : public BaseObject {

    	public:
       
      /**
			 * Constructor
			 */
       OldVdqmProtocol(vdqmVolReq_t *volumeRequest,
												vdqmDrvReq_t *driveRequest,
										  	vdqmHdr_t *header,
												vdqmnw_t *client_connection,
												int reqtype);
			
       
       /**
        * Calls the right function for the request.
        */
       void handleRequestType(Cuuid_t cuuid) 
       	throw (castor::exception::Exception);
       
       /**
        * Checks the reqtype and returns an error, if it is a 
        * wrong number. Throws an exception, if an error occures.
        */
       bool checkRequestType(Cuuid_t cuuid) 
       	throw (castor::exception::Exception);
      
      private:
     	/**
       * Default Constructor
       */
       OldVdqmProtocol();
      
      	vdqmVolReq_t *ptr_volumeRequest;
		  	vdqmDrvReq_t *ptr_driveRequest;
  			vdqmHdr_t *ptr_header;
  			vdqmnw_t *ptr_client_connection;
				int m_reqtype;
				
		}; // class VdqmServer

  } // end of namespace vdqm

} // end of namespace castor

#endif //_OLDVDQMPROTOCOL_HPP_

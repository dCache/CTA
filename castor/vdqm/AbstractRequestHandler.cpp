/******************************************************************************
 *                      AbstractRequestHandler.cpp
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
 * @(#)RCSfile: AbstractRequestHandler.cpp  Revision: 1.0  Release Date: Apr 29, 2005  Author: mbraeger 
 *
 *
 *
 * @author Matthias Braeger
 *****************************************************************************/
 

#include "castor/IObject.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/Constants.hpp"
#include "castor/Services.hpp"
#include "castor/IService.hpp"

#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
 
//Local Includes
#include "AbstractRequestHandler.hpp"
#include "TapeRequest.hpp"
#include "TapeDrive.hpp"
#include "IVdqmSvc.hpp"


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::AbstractRequestHandler::AbstractRequestHandler() 
	throw() {

  castor::IService* svc;

  /**
   * The IVdqmService Objects has some important fuctions
   * to handle db queries.
   */
  
  ptr_svcs = services();
	
	/**
	 * Getting DbVdqmService
	 */
	svc = ptr_svcs->service("DbVdqmSvc", castor::SVC_DBVDQMSVC);
  if (0 == svc) {
    castor::exception::Internal ex;
    ex.getMessage() << "Could not get DbVdqmSvc" << std::endl;
    throw ex;
  }
  
  ptr_IVdqmService = dynamic_cast<IVdqmSvc*>(svc);
  if (0 == ptr_IVdqmService) {
    castor::exception::Internal ex;
    ex.getMessage() << "Got a bad DbVdqmSvc: "
    								<< "ID=" << svc->id()
    								<< ", Name=" << svc->name()
    								<< std::endl;
    throw ex;
  }
}


//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::vdqm::AbstractRequestHandler::~AbstractRequestHandler() 
	throw() {
	ptr_IVdqmService->release();
	
	//Reset the pointer
	ptr_svcs = 0;
	ptr_IVdqmService = 0;
}


//------------------------------------------------------------------------------
// handleRequest
//------------------------------------------------------------------------------
void castor::vdqm::AbstractRequestHandler::handleRequest
(castor::IObject* fr, Cuuid_t cuuid)
  throw (castor::exception::Exception) {
  // Stores it into DB
  castor::BaseAddress ad;
  ad.setCnvSvcName("DbCnvSvc");
  ad.setCnvSvcType(castor::SVC_DBCNV);
  try {
  	//Create a new entry in the table
    svcs()->createRep(&ad, fr, false);
    
    // Store files for taoe requests
    castor::vdqm::TapeRequest *tapeRequest =
      dynamic_cast<castor::vdqm::TapeRequest*>(fr);
    
    if (0 != tapeRequest) {
//    	if (NULL != tapeRequest->reqExtDevGrp()) { 
//    		svcs()->createRep(&ad, (IObject *)tapeRequest->reqExtDevGrp(), false);
//    		svcs()->fillRep(&ad, fr, OBJ_ExtendedDeviceGroup, false);
//    	}
//    	
//    	if (0 != tapeRequest->requestedSrv()) {
//	    	svcs()->createRep(&ad, (IObject *)tapeRequest->requestedSrv(), false);
//	    	svcs()->fillRep(&ad, fr, OBJ_TapeServer, false);
//    	}
    	
    	svcs()->createRep(&ad, (IObject *)tapeRequest->client(), false);
      svcs()->fillRep(&ad, fr, OBJ_ClientIdentification, false);
      svcs()->fillRep(&ad, fr, OBJ_Tape, false);
    }
    
    castor::vdqm::TapeDrive *tapeDrive =
      dynamic_cast<castor::vdqm::TapeDrive*>(fr);
    
    if (0 != tapeDrive) {
      svcs()->fillRep(&ad, fr, OBJ_TapeDrive, false);
    }

    svcs()->commit(&ad);
    // "Request stored in DB" message
    castor::dlf::Param params[] =
      {castor::dlf::Param("ID", fr->id())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_USAGE, 12, 1, params);

  } catch (castor::exception::Exception e) {
    svcs()->rollback(&ad);
    throw e;
  }

  // TODO: Send an UDP message to the vdqm client
}

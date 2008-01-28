/******************************************************************************
 *                      TapeRequestHandler.cpp
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
 * @(#)RCSfile: TapeRequestHandler.cpp  Revision: 1.0  Release Date: Apr 19, 2005  Author: mbraeger 
 *
 *
 *
 * @author Matthias Braeger
 *****************************************************************************/
 
#include <string> 
#include <vector>
#include <time.h>

#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/vdqm/ClientIdentification.hpp"
#include "castor/vdqm/VdqmTape.hpp"

#include "castor/IObject.hpp"
#include "castor/Constants.hpp"
#include "castor/Services.hpp"
#include "castor/BaseAddress.hpp"

#include "castor/vdqm/DatabaseHelper.hpp"
#include "castor/vdqm/DeviceGroupName.hpp"
#include "castor/vdqm/newVdqm.h"
#include "castor/vdqm/OldProtocolInterpreter.hpp"
#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDriveStatusCodes.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "castor/vdqm/VdqmDlfMessageConstants.hpp"
#include "castor/vdqm/handler/TapeRequestHandler.hpp"

#define VDQMSERV 1

#include <net.h>
#include <vdqm_constants.h>
#include "h/Ctape_constants.h"
#include <common.h> //for getconfent

/**
 * includes to talk to VMGR
 */
#include <vmgr_struct.h>
#include <sys/types.h>
#include "vmgr_api.h"
 

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::handler::TapeRequestHandler::TapeRequestHandler() 
  throw(castor::exception::Exception) {
}


//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::vdqm::handler::TapeRequestHandler::~TapeRequestHandler() throw() {
}

//------------------------------------------------------------------------------
// newTapeRequest
//------------------------------------------------------------------------------
void castor::vdqm::handler::TapeRequestHandler::newTapeRequest(newVdqmHdr_t *header, 
                                                  newVdqmVolReq_t *volumeRequest,
                                                  Cuuid_t cuuid) 
  throw (castor::exception::Exception) {
  
  //The db related informations
  TapeRequest *newTapeReq = NULL;
  castor::vdqm::ClientIdentification *clientData = NULL;
  castor::vdqm::VdqmTape *tape = NULL;
  
  struct vmgr_tape_info tape_info; // used to get information about the tape
  
  TapeServer *reqTapeServer = NULL;
  DeviceGroupName *dgName = NULL;
  TapeAccessSpecification *tapeAccessSpec = NULL;
  
  int rowNumber = 0; 
  int rc = 0; // return value from vmgr_querytape
  char *p;


  if ( header == NULL || volumeRequest == NULL ) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "One of the arguments is NULL"
                    << std::endl;
    throw ex;
  }
  
  
  //The parameters of the old vdqm VolReq Request
  castor::dlf::Param params[] =
    {castor::dlf::Param("client_name", volumeRequest->client_name),
     castor::dlf::Param("clientUID", volumeRequest->clientUID),
     castor::dlf::Param("clientGID", volumeRequest->clientGID),
     castor::dlf::Param("client_host", volumeRequest->client_host),
     castor::dlf::Param("client_port", volumeRequest->client_port),
     castor::dlf::Param("volid", volumeRequest->volid),
     castor::dlf::Param("mode", volumeRequest->mode),
     castor::dlf::Param("dgn", volumeRequest->dgn),
     castor::dlf::Param("drive", (*volumeRequest->drive == '\0' ? "***" : volumeRequest->drive)),
     castor::dlf::Param("server", (*volumeRequest->server == '\0' ? "***" : volumeRequest->server))};
  castor::dlf::dlf_writep(cuuid, DLF_LVL_DEBUG, VDQM_OLD_VDQM_VOLREQ_PARAMS, 10, params);
  
  try {
    //------------------------------------------------------------------------
    //The Tape related informations
    newTapeReq = new TapeRequest();
    newTapeReq->setCreationTime(time(NULL));
    /**
     * The modification time will only be changed,
     * in case that an error occures during the tape assignment
     * procedure with RTCPD
     */
     newTapeReq->setModificationTime(newTapeReq->creationTime());
     /*
     * We don't allow client to set priority
     */
     newTapeReq->setPriority(VDQM_PRIORITY_NORMAL);
     
     //The client related informations
     clientData = new castor::vdqm::ClientIdentification();
     clientData->setMachine(volumeRequest->client_host);
     clientData->setUserName(volumeRequest->client_name);
     clientData->setPort(volumeRequest->client_port);
     clientData->setEuid(volumeRequest->clientUID);
     clientData->setEgid(volumeRequest->clientGID);
     clientData->setMagic(header->magic);
     
     /**
      * Annotation: The side of the Tape is not necesserally needed
      * by the vdqmDaemon. Normaly the RTCopy daemon should already 
      * have created an entry to the Tape table. So, we just give 0 as parameter 
      * at this place.
      */
     tape = ptr_IVdqmService->selectTape(volumeRequest->volid, 
                                         0, 
                                         volumeRequest->mode);                                     
    
    //The requested tape server: Return value is NULL if the server name is empty
    reqTapeServer = ptr_IVdqmService->selectTapeServer(volumeRequest->server, false);
    
    memset(&tape_info,'\0',sizeof(vmgr_tape_info));
    
    // "Try to get information about the tape from the VMGR daemon" message
    castor::dlf::dlf_writep(cuuid, DLF_LVL_DEBUG, VDQM_GET_TAPE_INFO_FROM_VMGR);     
    
    /**
     * Create a connection to vmgr daemon, to obtain more information about the
     * tape, like its density and its tapeModel.
     */
    rc = vmgr_querytape (tape->vid().c_str(), 0, &tape_info, volumeRequest->dgn);    
    
    if ( rc == -1) {
      castor::exception::Exception ex(EVQDGNINVL);
      ex.getMessage() << "Errors, while using to vmgr_querytape: "
                      << "serrno = " << serrno
                      << std::endl;
      throw ex;      
    }
    
    /**
     * Valids the requested tape Access for the specific tape model.
     * In case of errors, the return value is NULL
     */
    tapeAccessSpec = 
      ptr_IVdqmService->selectTapeAccessSpecification(volumeRequest->mode, 
                                                      tape_info.density, 
                                                      tape_info.model);
    
    if (0 == tapeAccessSpec) {
      castor::exception::Exception ex(EVQDGNINVL);
      ex.getMessage() << "The specified tape access mode doesn't exist in the db"
                      << std::endl;
      throw ex;
    }
    
     /**
     * The requested device group name. If the entry doesn't yet exist, 
     * it will be created.
     */
     dgName = ptr_IVdqmService->selectDeviceGroupName(volumeRequest->dgn);
    
    if ( 0 == dgName) {
      castor::exception::Exception ex(EVQDGNINVL);
      ex.getMessage() << "DGN " <<  volumeRequest->dgn
                      << " does not exist in the db" << std::endl;
      throw ex;
    }
    
    
    //Connect the tapeRequest with the additional information
    newTapeReq->setClient(clientData);
    newTapeReq->setTapeAccessSpecification(tapeAccessSpec);
    newTapeReq->setRequestedSrv(reqTapeServer);
    newTapeReq->setTape(tape);
    newTapeReq->setDeviceGroupName(dgName);
    
  
    /*
     * Set priority for tpwrite
     */
    if ( (tapeAccessSpec->accessMode() == WRITE_ENABLE) &&
         ((p = getconfent("VDQM","WRITE_PRIORITY",0)) != NULL) ) {
      if ( strcmp(p,"YES") == 0 ) {
        newTapeReq->setPriority(VDQM_PRIORITY_MAX);
      }
    }
    
    // Request priority changed
    castor::dlf::Param params2[] =
      {castor::dlf::Param("priority", newTapeReq->priority())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_SYSTEM, VDQM_REQUEST_PRIORITY_CHANGED, 1, params2);
    
    
    // Verify that the request doesn't exist, 
    // by calling IVdqmSvc->checkTapeRequest
    castor::dlf::dlf_writep(cuuid, DLF_LVL_DEBUG, VDQM_VERIFY_REQUEST_NON_EXISTANT);
    /*
     * Verify that the request doesn't (yet) exist. If it doesn't exist,
     * the return value should be true.
     */
    bool requestNotExists = ptr_IVdqmService->checkTapeRequest(newTapeReq);
    if ( requestNotExists == false ) {
      castor::exception::Exception ex(EVQALREADY);
      ex.getMessage() << "Input request already queued: " 
                      << "Position = " << rowNumber << std::endl;
      throw ex;
    }
    
    // Try to store Request into the data base
     castor::dlf::dlf_writep(cuuid, DLF_LVL_DEBUG, VDQM_STORE_REQUEST_IN_DB);
    /*
     * Add the record to the volume queue
     */
    castor::vdqm::DatabaseHelper::storeRepresentation(newTapeReq, cuuid);
    
    /**
     * Now the newTapeReq has the id of its 
     * row representatioon in the db table.
     * 
     * Please note: Because of the restrictions of the old
     * protocol, we must convert the 64bit id into a 32bit number.
     * This is still sufficient to have a unique TapeRequest ID, because
     * the requests don't stay for long time in the db.
     */
    volumeRequest->VolReqID = (unsigned int)newTapeReq->id();  
  } catch(castor::exception::Exception e) {
     if (newTapeReq)
       delete newTapeReq; //also deletes clientData, because of composition
     if (tape)
       delete tape;
    if (tapeAccessSpec)
      delete tapeAccessSpec;
    if (reqTapeServer)
      delete reqTapeServer;
    if (dgName)
      delete dgName;
  
    throw e;
  }
  
  // Delete all Objects
  delete newTapeReq; //also deletes clientData, because of composition 
  delete tape;
  delete tapeAccessSpec;
  
  if (reqTapeServer)
    delete reqTapeServer;
  
  delete dgName;
}


//------------------------------------------------------------------------------
// deleteTapeRequest
//------------------------------------------------------------------------------
void castor::vdqm::handler::TapeRequestHandler::deleteTapeRequest(
                                                  newVdqmVolReq_t *volumeRequest,
                                                  Cuuid_t cuuid) 
  throw (castor::exception::Exception) {
   //The db related informations
  TapeRequest *tapeReq = NULL;
  castor::BaseAddress ad;
  
  // Get the tapeReq from its id
  try {
    tapeReq = ptr_IVdqmService->selectTapeRequest(volumeRequest->VolReqID);
    
    if ( tapeReq == NULL ) {
      /**
       * If we don't find the tapeRequest in the db it is normally not a big deal,
       * because the assigned tape drive has probably already finished the transfer.
       */
      
      //"Couldn't find the tape request in db. Maybe it is already deleted?" message
      castor::dlf::Param params[] =
        {castor::dlf::Param("tapeRequest ID", volumeRequest->VolReqID),
         castor::dlf::Param("function", "castor::vdqm::handler::TapeRequestHandler::deleteTapeRequest()")};
      castor::dlf::dlf_writep(cuuid, DLF_LVL_WARNING, VDQM_TAPE_REQUEST_NOT_FOUND_IN_DB, 2, params);
      
      return;
    }
  } catch (castor::exception::Exception e) {
    /**
     * If we don't find the tapeRequest in the db it is normally not a big deal,
     * because the assigned tape drive has probably already finished the transfer.
     */
    
    //"Couldn't find the tape request in db. Maybe it is already deleted?" message
    castor::dlf::Param params[] =
      {castor::dlf::Param("tapeRequest ID", volumeRequest->VolReqID),
       castor::dlf::Param("function", "castor::vdqm::handler::TapeRequestHandler::deleteTapeRequest()"),
       castor::dlf::Param("Message", e.getMessage().str().c_str())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_WARNING, VDQM_TAPE_REQUEST_NOT_FOUND_IN_DB, 2, params);
    
    if ( tapeReq ) {
      if ( tapeReq->tapeDrive() ) {
        delete tapeReq->tapeDrive();
        tapeReq->setTapeDrive(0);
      }
      
      delete tapeReq;
      tapeReq = 0;
    }
    
    return;
  }
    
  /**
   * OK, now we have the tapeReq and its client :-)
   */
  
  try {  
    if ( tapeReq->tapeDrive() ) {
      // If we are here, the TapeRequest is already assigned to a TapeDrive
      //TODO:
      /*
       * PROBLEM:
       * Can only remove request, if it is not assigned to a drive. Otherwise
       * it should be cleanup by resetting the drive status to RELEASE + FREE.
       * Mark it as UNKNOWN until an unit status clarifies what has happened.
       */
       TapeDrive* tapeDrive = tapeReq->tapeDrive();
      
      // Set new TapeDriveStatusCode
      tapeDrive->setStatus(STATUS_UNKNOWN);
      
      tapeReq->setModificationTime(time(NULL));
        
      /**
       * Update the data base. To avoid a deadlock, the tape drive has to be 
       * updated first!
       */
       castor::vdqm::DatabaseHelper::updateRepresentation(
                          tapeDrive, cuuid);
      castor::vdqm::DatabaseHelper::updateRepresentation(
                          tapeReq, cuuid);

      delete tapeDrive;
      tapeDrive=0;
      tapeReq->setTapeDrive(0);
      delete tapeReq;
      tapeReq = 0;
          
      castor::exception::Exception ex(EVQREQASS);
      ex.getMessage() << "TapeRequest is assigned to a TapeDrive. "
                      << "Can't delete it at the moment" << std::endl;
      throw ex;
    }
    else {
      // Remove the TapeRequest from the db (queue)
      castor::vdqm::DatabaseHelper::deleteRepresentation(tapeReq, cuuid);
    
      // "TapeRequest and its ClientIdentification removed" message
      castor::dlf::Param params[] =
        {castor::dlf::Param("TapeRequest ID", tapeReq->id())};
      castor::dlf::dlf_writep(cuuid, DLF_LVL_DEBUG, VDQM_TAPE_REQUEST_ANDCLIENT_ID_REMOVED, 1, params);
    }
  } catch(castor::exception::Exception e) {
     if (tapeReq) {
       delete tapeReq;
       tapeReq = 0;
     }
  
    throw e;
  }
  
  // Delete all Objects
  delete tapeReq;
  tapeReq = 0;
}


//------------------------------------------------------------------------------
// getQueuePosition
//------------------------------------------------------------------------------
int castor::vdqm::handler::TapeRequestHandler::getQueuePosition(
                                                  newVdqmVolReq_t *volumeRequest,
                                                  Cuuid_t cuuid) 
  throw (castor::exception::Exception) {
    
  //To store the db related informations
  castor::vdqm::TapeRequest *tapeReq = NULL;
  int queuePosition = 0;
  
  try {
    tapeReq = ptr_IVdqmService->selectTapeRequest(volumeRequest->VolReqID);
    
    if ( tapeReq == NULL ) {
      return -1;
    }
    else if ( tapeReq->tapeDrive() ) {
      delete tapeReq->tapeDrive();
      tapeReq->setTapeDrive(0);
    }
    else {
      queuePosition = ptr_IVdqmService->getQueuePosition(tapeReq);
    }
    
    // Queue position of TapeRequest
    castor::dlf::Param params[] =
      {castor::dlf::Param("Queue position", queuePosition),
       castor::dlf::Param("TapeRequest id", tapeReq->id())};
    castor::dlf::dlf_writep(cuuid, DLF_LVL_DEBUG, VDQM_QUEUE_POS_OF_TAPE_REQUEST, 2, params);
    
  } catch(castor::exception::Exception e) {
     if (tapeReq) {
       delete tapeReq;
       tapeReq = 0;
     }
  
    throw e;
  }
  // Delete the tapeRequest Object
  delete tapeReq;
  tapeReq = 0;
  
  return queuePosition;
}


//------------------------------------------------------------------------------
// sendTapeRequestQueue
//------------------------------------------------------------------------------
void castor::vdqm::handler::TapeRequestHandler::sendTapeRequestQueue(
  newVdqmHdr_t *header, 
  newVdqmVolReq_t *volumeRequest,
  newVdqmDrvReq_t *driveRequest, 
  castor::vdqm::OldProtocolInterpreter* oldProtInterpreter,
  Cuuid_t cuuid) 
  throw (castor::exception::Exception) {

  // The result of the search in the database.
  std::vector<castor::vdqm::TapeRequest*>* result = 0;
  
  std::string dgn, server;
    
  dgn = "";
  server = "";

  if ( *(volumeRequest->dgn) != '\0' ) dgn = volumeRequest->dgn;
  if ( *(volumeRequest->server) != '\0' ) server = volumeRequest->server;

  try {
    /**
     * With this function call we get the requested queue out of the db.
     * The result depends on the parameters. If the paramters are not specified,
     * then we get all tapeRequests back.
     */
    result = ptr_IVdqmService->selectTapeRequestQueue(dgn, server);
  
    if ( result != NULL && result->size() > 0 ) {
      //If we are here, then we got a result which we can send to the client
      for(std::vector<castor::vdqm::TapeRequest*>::iterator it = result->begin();
          it != result->end();
          it++) {
            
        volumeRequest->VolReqID = (unsigned int)(*it)->id();
        
        TapeDrive* tapeDrive = (*it)->tapeDrive();
        if ( tapeDrive != NULL ) {
          strcpy(volumeRequest->drive, tapeDrive->driveName().c_str());
          volumeRequest->DrvReqID = (unsigned int)tapeDrive->id();
          
          delete tapeDrive;
          tapeDrive = 0;
          (*it)->setTapeDrive(0);
        } 
        else { 
          strcpy(volumeRequest->drive, "");
          volumeRequest->DrvReqID = 0;
        }
        
        volumeRequest->priority = (*it)->priority();
        volumeRequest->client_port = (*it)->client()->port();
        volumeRequest->clientUID = (*it)->client()->euid();
        volumeRequest->clientGID = (*it)->client()->egid();
        volumeRequest->mode = (*it)->tapeAccessSpecification()->accessMode();
        volumeRequest->recvtime = (int)(*it)->modificationTime();
        strcpy(volumeRequest->client_host, (*it)->client()->machine().c_str());
        strcpy(volumeRequest->volid, (*it)->tape()->vid().c_str());
        
        TapeServer* requestedSrv = (*it)->requestedSrv();
        if ( requestedSrv != NULL ) {
          strcpy(volumeRequest->server, requestedSrv->serverName().c_str());
         
          delete requestedSrv;
          requestedSrv = 0;
          (*it)->setRequestedSrv(0);
        }
        else 
          strcpy(volumeRequest->server, "");
            
        
        strcpy(volumeRequest->dgn, (*it)->deviceGroupName()->dgName().c_str());
        strcpy(volumeRequest->client_name, (*it)->client()->userName().c_str());
            
        
        //free memory
        delete (*it)->tape();
        (*it)->setTape(0);
        delete (*it)->deviceGroupName();
        (*it)->setDeviceGroupName(0);
        delete (*it)->tapeAccessSpecification();
        (*it)->setTapeAccessSpecification(0);
        delete (*it);
        (*it) = 0;
        
        //"Send information for showqueues command" message
        castor::dlf::Param param[] =
          {castor::dlf::Param("message", "TapeRequest info"),
           castor::dlf::Param("TapeRequest ID", volumeRequest->VolReqID)};
        castor::dlf::dlf_writep(cuuid, DLF_LVL_DEBUG, VDQM_SEND_SHOWQUEUES_INFO, 2, param);
        
        //Send informations to the client
        oldProtInterpreter->sendToOldClient(
                    header, volumeRequest, driveRequest);
      }
    }
  } catch (castor::exception::Exception ex) {
    
    if ( result != NULL ) {
      //free memory
      for(std::vector<castor::vdqm::TapeRequest*>::iterator it = result->begin();
            it != result->end();
            it++) {
        TapeDrive* tapeDrive = (*it)->tapeDrive();
        if ( tapeDrive != NULL ) {
          delete tapeDrive;
          tapeDrive = 0;
          (*it)->setTapeDrive(0);
        }
        
        TapeServer* requestedSrv = (*it)->requestedSrv();
        if ( requestedSrv != NULL ) {
          delete requestedSrv;
          requestedSrv = 0;
          (*it)->setRequestedSrv(0);
        }
        
        delete (*it)->tape();
        (*it)->setTape(0);
        delete (*it)->deviceGroupName();
        (*it)->setDeviceGroupName(0);
        delete (*it)->tapeAccessSpecification();
        (*it)->setTapeAccessSpecification(0);
        delete (*it);
        (*it) = 0;             
      }
      
      // deletion of the vector
      delete result;
    }

    /**
     * To inform the client about the end of the queue, we send again a 
     * volumeRequest with the VolReqID = -1
     */
    volumeRequest->VolReqID = -1;
    
    oldProtInterpreter->sendToOldClient(header, volumeRequest, driveRequest);
    
    throw ex;
  }
  
  // deletion of the vector
  delete result;  

  /**
   * To inform the client about the end of the queue, we send again a 
   * volumeRequest with the VolReqID = -1
   */
  volumeRequest->VolReqID = -1;
  
  oldProtInterpreter->sendToOldClient(header, volumeRequest, driveRequest);
}

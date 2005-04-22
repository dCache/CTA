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
#include <time.h>
 
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"

#include "castor/stager/ClientIdentification.hpp"
#include "castor/stager/Tape.hpp"

#include "h/vdqm.h"
#include "h/vdqm_constants.h"
#include "h/getconfent.h"

//Local includes
#include "TapeRequestHandler.hpp"
#include "ExtendedDeviceGroup.hpp"
#include "TapeRequest.hpp"
#include "ClientIdentification.hpp"
#include "IVdqmSvc.hpp"
#include "TapeServer.hpp"


 

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::TapeRequestHandler::TapeRequestHandler() {
  initLog("vdqmRequestHandler", SVC_DLFMSG);
  // Initializes the DLF logging. This includes
  // defining the predefined messages
  castor::dlf::Message messages[] =
    {{ 0, " - "},
     { 1, "The parameters of the old vdqm VolReq Request"},
     { 2, "Request priority changed"},
     {-1, ""}};
  castor::dlf::dlf_init("vdqmRequestHandler", messages);
}


//------------------------------------------------------------------------------
// newTapeRequest
//------------------------------------------------------------------------------
void castor::vdqm::RequestHandler::newTapeRequest(vdqmHdr_t *header, 
																									vdqmVolReq_t *volumeRequest) 
	throw (castor::exception::Exception) {
  
  //The db related inforamtions
  castor::vdqm::TapeRequest *newTapeReq;
  castor::stager::ClientIdentification *clientData;
  castor::stager::Tape *tape;
  castor::vdqm::TapeServer *reqTapeServer;
  castor::vdqm::ExtendedDeviceGroup *reqExtDevGrp;
  
  //The IService for vdqm
  castor::vdqm::IVdqmSvc *ptr_IVdqmService;
  
  bool exist = false;
  
  dgn_element_t 										*dgn_context; //ExtendedDeviceGroup
  vdqm_volrec_t 										*volrec, *newvolrec; //TapeRequest 
  vdqm_drvrec_t 										*drvrec; //TapeDriveRequest
  char 															*p;
  int 															rc;
  
  //TODO: ptr_IVdqmService instanziieren!!!
  

  if ( header == NULL || volumeRequest == NULL ) {
  	castor::exception::InvalidArgument ex;
    ex.getMessage() << "One of the arguments is NULL";
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
  castor::dlf::dlf_writep(cuuid, DLF_LVL_USAGE, 1, 10, params);
  
  //The Tape related informations
 	newTapeReq->setCreationTime(time(NULL));
 	/*
   * We don't allow client to set priority
   */
 	newTapeReq->setPriority(VDQM_PRIORITY_NORMAL);
 	
 	//The client related informations
 	clientData->setMachine(volumeRequest->client_host);
 	clientData->setUserName(volumeRequest->client_name);
 	clientData->setPort(volumeRequest->client_port);
 	clientData->setEuid(volumeRequest->clientUID);
 	clientData->setEgid(volumeRequest->clientGID);
 	clientData->setMagic(header->magic);
 
  //The requested ExtendedDeviceGroup
  reqExtDevGrp->setdgName(volumeRequest->dgn);
  reqExtDevGrp->setMode(volumeRequest->mode);
  
  //The requested tape server
  reqTapeServer->setServerName(volumeRequest->server);
  
//  tape->set
	//TODO: Tape aus volumeRequest
  
  /*
   * Check that the requested device exists.
   */
  exist = ptr_IVdqmService->checkDgn(reqExtDevGrp);
  
  if ( !exist ) {
  	castor::exception::Internal ex;
    ex.getMessage() << "DGN " <<  volumeRequest->dgn
    								<< " does not exist" << std::endl;
    throw ex;
  }

//------------------------------------------------------------------------------*
//  /*
//   * Reset Device Group Name context
//   */
//  rc = SetDgnContext(&dgn_context,volumeRequest->dgn);
//  if ( rc == -1 ) {
//      log(LOG_ERR,"vdqm_NewVolReq() cannot set Dgn context for %s\n",
//          volumeRequest->dgn);
//      return(-1);
//  }
  
  /*
   * Verify that the request doesn't (yet) exist
   */
  exist = ptr_IVdqmService->checkTapeRequest(newTapeReq);
  if ( exist ) {
    castor::exception::Internal ex;
    ex.getMessage() << "Input request already queued" << std::endl;
    throw ex;
  }
  

  /*
   * Fill in the new information
   */
  volrec->magic = header->magic;

  /*
   * Set priority for tpwrite
   */
  if ( (reqExtDevGrp->mode() == WRITE_ENABLE) &&
       ((p = getconfent("VDQM","WRITE_PRIORITY",0)) != NULL) ) {
    if ( strcmp(p,"YES") == 0 ) {
      newTapeReq->setPriority(VDQM_PRIORITY_MAX);
    }
  }
  
  newTapeReq->setClient(clientData);
  newTapeReq->setReqExtDevGrp(reqExtDevGrp);
  
  //The parameters of the old vdqm VolReq Request
  castor::dlf::Param params[] =
  	{castor::dlf::Param("priority", newTapeReq->priority())};
  castor::dlf::dlf_writep(cuuid, DLF_LVL_USAGE, 2, 1, params);
  
  /*
   * Remember the new volume record to assure replica is updated about it.
   */
   newvolrec = volrec;

  
  /*
   * Add the record to the volume queue
   */
  rc = AddVolRecord(dgn_context,volrec);
  volumeRequest->VolReqID = volrec->vol.VolReqID;
  if ( rc < 0 ) {
      log(LOG_ERR,"vdqm_NewVolReq(): AddVolRecord() returned error\n");
      FreeDgnContext(&dgn_context);
      return(-1);
  }
  log(LOG_INFO,"vdqm_NewVolReq() Assigned volume request ID=%d\n",
      volumeRequest->VolReqID);
 
  rc = AnyDrvFreeForDgn(dgn_context);
  if ( rc < 0 ) {
      log(LOG_ERR,"vdqm_NewVolReq(): AnyFreeDrvForDgn() returned error\n");
      FreeDgnContext(&dgn_context);
      return(0);
  }
  /*
   * If there was a free drive, start a new job
   */
  if ( rc == 1 ) {
      /* 
       * Loop until either the volume queue is empty or
       * there are no more suitable drives
       */
      for (;;) {
          rc = SelectVolAndDrv(dgn_context,&volrec,&drvrec);
          if ( rc == -1 || volrec == NULL || drvrec == NULL ) {
              log(LOG_ERR,"vdqm_NewVolReq(): SelectVolAndDrv() returned rc=%d\n",
                  rc);
              break;
          }
          /*
           * Free memory allocated for previous request for this drive
           */
          if ( drvrec->vol != NULL ) free(drvrec->vol);
          drvrec->vol = volrec;
          volrec->drv = drvrec;
          drvrec->drv.VolReqID = volrec->vol.VolReqID;
          volrec->vol.DrvReqID = drvrec->drv.DrvReqID;
          drvrec->drv.jobID = 0;
          /*
           * Reset the drive status
           */
          drvrec->drv.status = drvrec->drv.status & 
              ~(VDQM_UNIT_RELEASE | VDQM_UNIT_BUSY | VDQM_VOL_MOUNT |
              VDQM_VOL_UNMOUNT | VDQM_UNIT_ASSIGN);
          drvrec->drv.recvtime = (int)time(NULL);
      
          /*
           * Start the job
           */
          rc = vdqm_StartJob(volrec);
          if ( rc < 0 ) {
              log(LOG_ERR,"vdqm_NewVolReq(): vdqm_StartJob() returned error\n");
              volrec->vol.DrvReqID = 0;
              drvrec->drv.VolReqID = 0;
              volrec->drv = NULL;
              drvrec->vol = NULL;
              break;
          }
     } /* end of for (;;) */
  }
  /*
   * Always update replica for this volume record (update status may have
   * been temporary reset by SelectVolAndDrv()).
   */
  newvolrec->update = 1;
  FreeDgnContext(&dgn_context);
  return(0);
}
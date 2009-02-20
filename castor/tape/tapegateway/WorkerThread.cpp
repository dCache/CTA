/******************************************************************************
 *                      WorkerThread.cpp
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2004  CERN
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
 * @(#)$RCSfile: WorkerThread.cpp,v $ $Author: gtaur $
 *
 *
 *
 * @author Giulia Taurelli
 *****************************************************************************/

#include "castor/tape/tapegateway/WorkerThread.hpp"

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include "castor/Services.hpp"
#include "castor/Constants.hpp"
#include "castor/IService.hpp"

#include "castor/tape/tapegateway/VolumeRequest.hpp"
#include "castor/tape/tapegateway/Volume.hpp"

#include "castor/tape/tapegateway/FileToRecallRequest.hpp"
#include "castor/tape/tapegateway/FileToRecall.hpp"
#include "castor/tape/tapegateway/FileRecalledNotification.hpp"

#include "castor/tape/tapegateway/FileToMigrateRequest.hpp"
#include "castor/tape/tapegateway/FileToMigrate.hpp"
#include "castor/tape/tapegateway/FileMigratedNotification.hpp"

#include "castor/tape/tapegateway/EndNotification.hpp"
#include "castor/tape/tapegateway/EndNotificationErrorReport.hpp"
#include "castor/tape/tapegateway/NotificationAcknowledge.hpp"
#include "castor/tape/tapegateway/ErrorReport.hpp"
#include "castor/tape/tapegateway/NoMoreFiles.hpp"

#include <getconfent.h>
#include <u64subr.h>
#include <sstream>
#include <errno.h>

#include "castor/tape/tapegateway/VmgrTapeGatewayHelper.hpp"
#include "castor/tape/tapegateway/NsTapeGatewayHelper.hpp"
#include "castor/tape/tapegateway/TapeFileNsAttribute.hpp"



//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

castor::tape::tapegateway::WorkerThread::WorkerThread(castor::tape::tapegateway::ITapeGatewaySvc* dbSvc):BaseDbThread(){ 
  m_dbSvc=dbSvc; 
  // populate the map with the different handlers
  m_handlerMap[OBJ_VolumeRequest] = &WorkerThread::handleStartWorker;
  m_handlerMap[OBJ_FileRecalledNotification] = &WorkerThread::handleRecallUpdate;
  m_handlerMap[OBJ_FileMigratedNotification] = &WorkerThread::handleMigrationUpdate;
  m_handlerMap[OBJ_FileToMigrateRequest] = &WorkerThread::handleRecallMoreWork;
  m_handlerMap[OBJ_FileToRecallRequest] = &WorkerThread::handleMigrationMoreWork;
  m_handlerMap[OBJ_EndNotification] = &WorkerThread::handleEndWorker;

}



//------------------------------------------------------------------------------
// runs the thread
//------------------------------------------------------------------------------
void castor::tape::tapegateway::WorkerThread::run(void* arg)
{

  // to comunicate with the tape aggragator

  try{

    unsigned short port=0;
    unsigned long ip=0;
    
    // create a socket

    std::auto_ptr<castor::io::ServerSocket> sock((castor::io::ServerSocket*)arg); 

    // Retrieve info on the client for logging 

    try {
      sock->getPeerIp(port, ip);
    } catch(castor::exception::Exception e) {
      // "Exception caught : ignored" message
      castor::dlf::Param params[] =
	{castor::dlf::Param("Standard Message", sstrerror(e.code())),
	 castor::dlf::Param("Precise Message", e.getMessage().str())};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 34, 2, params);
      
    }
    
    castor::dlf::Param params[] =
      {castor::dlf::Param("Client IP", ip), 
       castor::dlf::Param("Client port",port)
      };

    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 33, 2, params);
    
    // get the incoming request

    try {
      std::auto_ptr<castor::IObject>  obj(sock->readObject());
    
      if (obj.get() == NULL){

	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 35, 0, NULL);
	return;

      }

      //let's  call the proper handler 

      HandlerMap::iterator handlerItor = m_handlerMap.find(obj->type());
      
      if(handlerItor == m_handlerMap.end()) {
      
	//object not valid
   
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 36, 0, NULL);
	return;
	
      }
	
      Handler handler = handlerItor->second;
    
      // Dispatch the request to the appropriate handler
      
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 41, 0, NULL);

      std::auto_ptr<castor::IObject> response( (this->*handler)(*obj,*m_dbSvc));

      // send back the proper response

      try {

	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 38, 0, NULL);

	sock->sendObject(*response);

      }catch (castor::exception::Exception e){
	castor::dlf::Param params[] =
	  {castor::dlf::Param("errorCode",sstrerror(e.code())),
	   castor::dlf::Param("errorMessage",e.getMessage().str())
	  };
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 37, 2, params);
      }

    } catch (castor::exception::Exception e) {
      
      // "Unable to read Request from socket" message
      
      castor::dlf::Param params[] =
	{castor::dlf::Param("errorCode",sstrerror(e.code())),
	 castor::dlf::Param("errorMessage",e.getMessage().str())
	};
	
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 39, 2, params);
      return;
    }
    
  } catch(...) {
    //castor one are trapped before 
    
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 40, 0, NULL);
    
  }

  // Deliberately slow down processing times to force thread pool expansion.
  sleep(1);

}


castor::IObject* castor::tape::tapegateway::WorkerThread::handleStartWorker( castor::IObject&  obj, castor::tape::tapegateway::ITapeGatewaySvc& m_dbSvc ) throw(){

  // I received a start worker request
  Volume* response=NULL;

  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 42, 0, NULL);

  try{
    
    VolumeRequest &startRequest = dynamic_cast<VolumeRequest&>(obj);
    
    castor::dlf::Param params[] =
      {castor::dlf::Param("transactionId", startRequest.vdqmVolReqId())
      };
    
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 43, 1, params);

    try {
    
      response = m_dbSvc.updateDbStartTape(startRequest); 
      
    } catch (castor::exception::Exception e){
    
      EndNotificationErrorReport* errorReport=new EndNotificationErrorReport();
      errorReport->setErrorCode(e.code());
      errorReport->setErrorMessage(e.getMessage().str());
     
      castor::dlf::Param params[] =
	{castor::dlf::Param("errorCode",sstrerror(e.code())),
	 castor::dlf::Param("errorMessage",e.getMessage().str())
	};

      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 44, 2, params);
      return errorReport;
    }
     
    if (response  == NULL ) {
      // I don't have anything to recall I send a NoMoreFiles
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 73, 1, params);
    
      NoMoreFiles* noMore= new NoMoreFiles();
      noMore->setTransactionId(startRequest.vdqmVolReqId());
      return noMore;
    }
    
  } catch( std::bad_cast &ex) {
    // "Invalid Request" message
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 36, 0, NULL);
    EndNotificationErrorReport* errorReport=new EndNotificationErrorReport();
    errorReport->setErrorCode(EINVAL);
    errorReport->setErrorMessage("invalid object");
    return errorReport;
  }

  return response;
}


castor::IObject* castor::tape::tapegateway::WorkerThread::handleRecallUpdate( castor::IObject& obj, castor::tape::tapegateway::ITapeGatewaySvc& m_dbSvc ) throw(){
      // I received a FileRecalledResponse
  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 45, 0, NULL);
  NotificationAcknowledge* response = NULL;

  try {

    FileRecalledNotification &fileRecalled = dynamic_cast<FileRecalledNotification&>(obj);

    castor::dlf::Param params[] =
	    {castor::dlf::Param("transactionId",fileRecalled.transactionId()),
	     castor::dlf::Param("NSHOST",fileRecalled.nshost()),
	     castor::dlf::Param("FILEID",fileRecalled.fileid()),
	     castor::dlf::Param("VID",fileRecalled.tapeFileNsAttribute()->vid())
	    };
       
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 46, 4, params);

    response = new NotificationAcknowledge();
    response->setTransactionId(fileRecalled.transactionId());

    if ( !fileRecalled.errorCode()){
      try {
	// check the nameserver if no error was faced already happened
	NsTapeGatewayHelper nsHelper;
	nsHelper.checkRecalledFile(fileRecalled);
 

      } catch (castor::exception::Exception e){
	// nameserver error
	fileRecalled.setErrorCode(e.code());	
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 47, 4, params); 
      }
    }

    try {
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 48, 4, params); 
      m_dbSvc.fileRecallUpdate(fileRecalled);
      
    } catch (castor::exception::Exception e) {
      castor::dlf::Param params[] =
	{ castor::dlf::Param("transactionId",fileRecalled.transactionId()),
	  castor::dlf::Param("NSHOST",fileRecalled.nshost()),
	  castor::dlf::Param("FILEID",fileRecalled.fileid()),
	  castor::dlf::Param("VID",fileRecalled.tapeFileNsAttribute()->vid()),
	  castor::dlf::Param("errorCode",sstrerror(e.code())),
	  castor::dlf::Param("errorMessage",e.getMessage().str())
	};
       
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 49, 6, params);
       
    }
    
  } catch  (std::bad_cast &ex) {
    // "Invalid Request" message
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 36, 0, NULL);
    
    ErrorReport* errorReport=new ErrorReport();
    errorReport->setErrorCode(EINVAL);
    errorReport->setErrorMessage("invalid object");
    return errorReport;
	
  }
  
  return response;
}

castor::IObject*  castor::tape::tapegateway::WorkerThread::handleMigrationUpdate(  castor::IObject& obj, castor::tape::tapegateway::ITapeGatewaySvc& m_dbSvc ) throw(){
  
  // I received a FileMigratedResponse
  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 50, 0, NULL);

  
  VmgrTapeGatewayHelper vmgrHelper;
  NsTapeGatewayHelper nsHelper;
  NotificationAcknowledge* response=NULL;

  try{

    FileMigratedNotification& fileMigrated = dynamic_cast<FileMigratedNotification&>(obj);
  
    response = new NotificationAcknowledge();
    response->setTransactionId(fileMigrated.transactionId());
    
    castor::dlf::Param params[] =
      {castor::dlf::Param("transactionId",fileMigrated.transactionId()),
       castor::dlf::Param("NSHOST",fileMigrated.nshost()),
       castor::dlf::Param("FILEID",fileMigrated.fileid()),
       castor::dlf::Param("VID",fileMigrated.tapeFileNsAttribute()->vid())
      };
       
    

    if (!fileMigrated.errorCode()) {
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 51, 4, params);
      // update vmgr
      try {  
      
	vmgrHelper.updateTapeInVmgr(fileMigrated);

	// check if it is repack case
	std::string repackVid;
	try {
	  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 52, 4, params);
	  repackVid=m_dbSvc.getRepackVid(fileMigrated);
      
	  try {
	    if (repackVid.empty()) {
      
	      // update the name server (standard migration)
	      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 53, 4, params);
	      nsHelper.updateMigratedFile(fileMigrated);
	    } else {
	    
	      // update the name server (repacked file)
	      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 54, 4, params);
	      nsHelper.updateRepackedFile(fileMigrated,repackVid);
	    }
	    
	  } catch (castor::exception::Exception e) {
	    fileMigrated.setErrorCode(e.code());
	    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 55, 4, params);
	  }
      
	} catch (castor::exception::Exception e){
	  castor::dlf::Param params[] =
	    {
	      castor::dlf::Param("transactionId",fileMigrated.transactionId()),
	      castor::dlf::Param("NSHOST",fileMigrated.nshost()),
	      castor::dlf::Param("FILEID",fileMigrated.fileid()),
	      castor::dlf::Param("VID",fileMigrated.tapeFileNsAttribute()->vid()),
	      castor::dlf::Param("errorCode",sstrerror(e.code())),
	     castor::dlf::Param("errorMessage",e.getMessage().str())
	    };
      
	  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 56, 6, params);
	  fileMigrated.setErrorCode(e.code());
	
	}
    

      } catch (castor::exception::Exception e){
      // vmgr exception
	castor::dlf::Param params[] =
	  {
	    castor::dlf::Param("transactionId",fileMigrated.transactionId()),
	    castor::dlf::Param("NSHOST",fileMigrated.nshost()),
	    castor::dlf::Param("FILEID",fileMigrated.fileid()),
	    castor::dlf::Param("VID",fileMigrated.tapeFileNsAttribute()->vid()),
	    castor::dlf::Param("errorCode",sstrerror(e.code())),
	    castor::dlf::Param("errorMessage",e.getMessage().str())
	  };
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 57, 6, params);
	fileMigrated.setErrorCode(e.code());
      }

    }
  // update the db
    
    try {
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 58, 4, params);
      m_dbSvc.fileMigrationUpdate(fileMigrated);
    }catch (castor::exception::Exception e) {
      
      castor::dlf::Param params[] =
	  {
	    castor::dlf::Param("transactionId",fileMigrated.transactionId()),
	    castor::dlf::Param("NSHOST",fileMigrated.nshost()),
	    castor::dlf::Param("FILEID",fileMigrated.fileid()),
	    castor::dlf::Param("VID",fileMigrated.tapeFileNsAttribute()->vid()),
	    castor::dlf::Param("errorCode",sstrerror(e.code())),
	    castor::dlf::Param("errorMessage",e.getMessage().str())
	  };
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 59, 6, params);
    
    }
  } catch (std::bad_cast &ex) {
   
    // "Invalid Request" message
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 36, 0, NULL);

    ErrorReport* errorReport=new ErrorReport();
    errorReport->setErrorCode(EINVAL);
    errorReport->setErrorMessage("invalid object");
    return errorReport;

  }

  return response;
    
}

castor::IObject*  castor::tape::tapegateway::WorkerThread::handleRecallMoreWork(castor::IObject& obj,castor::tape::tapegateway::ITapeGatewaySvc& m_dbSvc ) throw(){

  // I received FileToRecallRequest
  FileToRecall* response=NULL;

 castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 60, 0, NULL);

 try {
    FileToRecallRequest& fileToRecall = dynamic_cast<FileToRecallRequest&>(obj);
    castor::dlf::Param params[] =
      {castor::dlf::Param("transactionId",fileToRecall.transactionId())
      };
    
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 61, 1, params);
    

    try {
      response = m_dbSvc.fileToRecall(fileToRecall);
    } catch (castor::exception::Exception e) {
      castor::dlf::Param params[] =
	{castor::dlf::Param("transactionId",fileToRecall.transactionId()),
	 castor::dlf::Param("errorCode",sstrerror(e.code())),
	 castor::dlf::Param("errorMessage",e.getMessage().str())
	};
      
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 62, 3, params);
   
      ErrorReport* errorReport=new ErrorReport();
      errorReport->setErrorCode(e.code());
      errorReport->setErrorMessage(e.getMessage().str());
      errorReport->setTransactionId(fileToRecall.transactionId());
      return errorReport;
     
    }
    
    if (response  == NULL ) {
      // I don't have anything to recall I send a NoMoreFiles
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 62, 1, params);
    
      NoMoreFiles* noMore= new NoMoreFiles();
      noMore->setTransactionId(fileToRecall.transactionId());
      return noMore;
    }
  } catch (std::bad_cast &ex) {
    // "Invalid Request" message

    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 36, 0, NULL);
    
    ErrorReport* errorReport=new ErrorReport();
    errorReport->setErrorCode(EINVAL);
    errorReport->setErrorMessage("invalid object");
    return errorReport;
  }
  return response;
}

castor::IObject* castor::tape::tapegateway::WorkerThread::handleMigrationMoreWork( castor::IObject& obj, castor::tape::tapegateway::ITapeGatewaySvc& m_dbSvc) throw(){
       
  // I received FileToMigrateRequest
  // I get a file from the db
  // send a new file

  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 64, 0, NULL);

  FileToMigrate* response=NULL;
  
  try {
    
     FileToMigrateRequest& fileToMigrate = dynamic_cast<FileToMigrateRequest&>(obj);
     castor::dlf::Param params[] =
      {castor::dlf::Param("transactionId",fileToMigrate.transactionId())
      };
     
     castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 65, 1, params);
     
     try {
      
       response=m_dbSvc.fileToMigrate(fileToMigrate);

     } catch (castor::exception::Exception e) {
       castor::dlf::Param params[] =
	 {castor::dlf::Param("transactionId",fileToMigrate.transactionId()),
	  castor::dlf::Param("errorCode",sstrerror(e.code())),
	  castor::dlf::Param("errorMessage",e.getMessage().str())
	 };

       castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 66, 3, params);
    
       ErrorReport* errorReport=new ErrorReport();
       errorReport->setErrorCode(e.code());
       errorReport->setErrorMessage(e.getMessage().str());
       return errorReport;
     }
      
     if ( response == NULL ) {
       castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 67, 1, params);
       // I don't have anything to migrate I send an NoMoreFiles

       NoMoreFiles* noMore= new NoMoreFiles();
       noMore->setTransactionId(fileToMigrate.transactionId());
       return noMore;
     } 
  } catch (std::bad_cast &ex) {
    // "Invalid Request" message
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 36, 0, NULL);
    ErrorReport* errorReport=new ErrorReport();
    errorReport->setErrorCode(EINVAL);
    errorReport->setErrorMessage("invalid object");
    return errorReport;
  }

  return response; // check went fine

}


castor::IObject*  castor::tape::tapegateway::WorkerThread::handleEndWorker( castor::IObject&  obj, castor::tape::tapegateway::ITapeGatewaySvc&  m_dbSvc ) throw(){
  	// I received an EndTransferRequest, I send back an EndTransferResponse
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 68, 0, NULL);

	NotificationAcknowledge* response=NULL;

	try {
	  EndNotification& endRequest = dynamic_cast<EndNotification&>(obj);
	  castor::dlf::Param params[] =
	    {castor::dlf::Param("transactionId", endRequest.transactionId())
	    };
    
	  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 69, 1, params);
	  response = new NotificationAcknowledge();
	  response->setTransactionId(endRequest.transactionId());

	  try {
    
	    std::auto_ptr<castor::stager::Tape> tape(m_dbSvc.updateDbEndTape(endRequest)); 

	    // release busy tape

	    if (tape.get() != NULL && tape->tpmode() == 1) {
	      castor::dlf::Param params[] =
		{castor::dlf::Param("transactionId", endRequest.transactionId()),
		 castor::dlf::Param("VID", tape->vid())
		};
    
	      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 70, 2, params);
	      try {
		VmgrTapeGatewayHelper vmgrHelper;
		vmgrHelper.resetBusyTape(*tape);
	  
	      } catch (castor::exception::Exception e) {
		castor::dlf::Param params[] =
		  {castor::dlf::Param("transactionId", endRequest.transactionId()),
		   castor::dlf::Param("VID", tape->vid()),
		   castor::dlf::Param("errorCode",sstrerror(e.code())),
		   castor::dlf::Param("errorMessage",e.getMessage().str())
		  };

		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 71, 4, params);
	      }
	    }

    
	  } catch (castor::exception::Exception e){
    
	    castor::dlf::Param params[] =
	      {castor::dlf::Param("transactionId", endRequest.transactionId()),
	       castor::dlf::Param("errorCode",sstrerror(e.code())),
	       castor::dlf::Param("errorMessage",e.getMessage().str())
	      };
	    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 72, 3, params);
    
	  }


	} catch (std::bad_cast){

	  // "Invalid Request" message
	  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 36, 0, NULL);
	  
	  ErrorReport* errorReport=new ErrorReport();
	  errorReport->setErrorCode(EINVAL);
	  errorReport->setErrorMessage("invalid object");
	  return errorReport;
	  
	}
	
	return  response;

}

/******************************************************************************
*                      TapeGatewayDaemon.cpp
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
* @(#)$RCSfile: TapeGatewayDaemon.cpp,v $ $Author: gtaur $
*
*
*
* @author Giulia Taurelli
*****************************************************************************/

// Include Files

#include <Cgetopt.h>
#include <iostream>
#include <string>
#include <u64subr.h>

#include "castor/Constants.hpp"
#include "castor/PortNumbers.hpp"
#include "castor/Services.hpp"

#include "castor/dlf/Dlf.hpp"

#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"

#include "castor/infoPolicy/TapeRetryPySvc.hpp"

#include "castor/server/SignalThreadPool.hpp"
#include "castor/server/TCPListenerThreadPool.hpp"

#include "castor/tape/tapegateway/DlfCodes.hpp"
#include "castor/tape/tapegateway/ITapeGatewaySvc.hpp"
#include "castor/tape/tapegateway/MigratorErrorHandlerThread.hpp"
#include "castor/tape/tapegateway/RecallerErrorHandlerThread.hpp"
#include "castor/tape/tapegateway/TapeGatewayDaemon.hpp"
#include "castor/tape/tapegateway/TapeStreamLinkerThread.hpp"
#include "castor/tape/tapegateway/VdqmRequestsCheckerThread.hpp"
#include "castor/tape/tapegateway/VdqmRequestsProducerThread.hpp"
#include "castor/tape/tapegateway/WorkerThread.hpp"



#define  DEFAULT_SLEEP_INTERVAL   10
#define  VDQM_TIME_OUT_INTERVAL 600 // Timeout between two polls on a VDQM request 
#define  MIN_WORKER_THREADS 5
#define  MAX_WORKER_THREADS 240

extern "C" {
  char* getconfent(const char *, const char *, int);
}
 

//------------------------------------------------------------------------------
// main method
//------------------------------------------------------------------------------

int main(int argc, char* argv[]){
 
  castor::tape::tapegateway::TapeGatewayDaemon tgDaemon;

  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, castor::tape::tapegateway::DAEMON_START, 0, NULL);

  // load the TapeGateway service to check that everything is fine with it

  castor::IService* dbSvc = castor::BaseObject::services()->service("OraTapeGatewaySvc", castor::SVC_ORATAPEGATEWAYSVC);
  castor::tape::tapegateway::ITapeGatewaySvc* oraSvc = dynamic_cast<castor::tape::tapegateway::ITapeGatewaySvc*>(dbSvc);
  if (0 == oraSvc) {
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, castor::tape::tapegateway::FATAL_ERROR , 0, NULL);

    std::cerr << "Couldn't load the oracle tapegateway service, check the castor.conf for DynamicLib entries"
	      << std::endl;
    exit(-1);
  }
  
  // create the retry policies
  try{
    char* prm=NULL;
    char* prr=NULL;
    std::string retryMigrationPolicyName;
    std::string retryRecallPolicyName;
    std::string migrationFunctionName;
    std::string recallFunctionName;
    
    // get migration retry policy 

    if ( (prm = getconfent("Policy","RetryMigration",0)) != NULL ){ 
      retryMigrationPolicyName=prm;
    } else {
       castor::dlf::Param params[] =
	  {castor::dlf::Param("message","No policy for migration retry in castor.conf")};
       castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ALERT, castor::tape::tapegateway::NO_RETRY_POLICY_FOUND, 1, params);

    }
    prm=NULL;
    if ( (prm = getconfent("Policy","RetryMigrationFunction",0)) != NULL ){ 
      migrationFunctionName=prm;
    } else {
        castor::dlf::Param params[] =
	  {castor::dlf::Param("message","No global function name for migration retry policy  in castor.conf")};
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ALERT,castor::tape::tapegateway:: NO_RETRY_POLICY_FOUND, 1, params);
    }


    // get recall retry  policy 

    if ( (prr = getconfent("Policy","RetryRecall",0)) != NULL ){ 
      retryRecallPolicyName=prr;
    } else {
      castor::dlf::Param params[] =
	  {castor::dlf::Param("message","No policy for recall retry  in castor.conf")};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ALERT, castor::tape::tapegateway::NO_RETRY_POLICY_FOUND, 1, params);

    }
    prr=NULL;
    if ( (prr = getconfent("Policy","RetryRecallFunction",0)) != NULL ){ 
      recallFunctionName=prr;
    } else {
        castor::dlf::Param params[] =
	  {castor::dlf::Param("message","No global function name for recall retry policy  in castor.conf")};
	castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ALERT, castor::tape::tapegateway::NO_RETRY_POLICY_FOUND, 1, params);
    }


    castor::infoPolicy::TapeRetryPySvc* retryMigrationSvc=NULL;
    castor::infoPolicy::TapeRetryPySvc* retryRecallSvc=NULL;
    
    // migration
    if (!retryMigrationPolicyName.empty() && !migrationFunctionName.empty())
      retryMigrationSvc = new castor::infoPolicy::TapeRetryPySvc(retryMigrationPolicyName,migrationFunctionName);

    //recaller
    if (!retryRecallPolicyName.empty() && !recallFunctionName.empty())
      retryRecallSvc = new castor::infoPolicy::TapeRetryPySvc(retryRecallPolicyName,recallFunctionName);

    // Get the min and max number of thread used by the Worker
    
    int minThreadsNumber = MIN_WORKER_THREADS;
    int maxThreadsNumber = MAX_WORKER_THREADS;

    char* tmp=NULL;
    if ( (tmp= getconfent("TAPEGATEWAY","MINWORKERTHREADS",0)) != NULL ){ 
      char* dp = tmp;
      minThreadsNumber= strtoul(tmp, &dp, 0);      
      if (*dp != 0 || minThreadsNumber <=0 ) {
        minThreadsNumber = MIN_WORKER_THREADS;
      }
    }

    if ( (tmp= getconfent("TAPEGATEWAY","MAXWORKERTHREADS",0)) != NULL ){ 
      char* dp = tmp;
      maxThreadsNumber= strtoul(tmp, &dp, 0);      
      if (*dp != 0 || maxThreadsNumber <=0 ) {
        maxThreadsNumber = MAX_WORKER_THREADS;
      }
    }

    tgDaemon.parseCommandLine(argc, argv);

    // run as stage st

    tgDaemon.runAsStagerSuperuser();

     // send request to vdmq
         
    tgDaemon.addThreadPool(
			   new castor::server::SignalThreadPool("ProducerOfVdqmRequestsThread", new castor::tape::tapegateway::VdqmRequestsProducerThread(tgDaemon.listenPort()), DEFAULT_SLEEP_INTERVAL)); // port used just to be sent to vdqm
    tgDaemon.getThreadPool('P')->setNbThreads(1);
			   
     // check requests for vdmq
			       
    tgDaemon.addThreadPool(
			    new castor::server::SignalThreadPool("CheckerOfVdqmRequestsThread", new castor::tape::tapegateway::VdqmRequestsCheckerThread( VDQM_TIME_OUT_INTERVAL), DEFAULT_SLEEP_INTERVAL ));
    tgDaemon.getThreadPool('C')->setNbThreads(1); 

    
    // query vmgr for tape and tapepools
   
    tgDaemon.addThreadPool(
      new castor::server::SignalThreadPool("TapeStreamLinkerThread", new castor::tape::tapegateway::TapeStreamLinkerThread(), DEFAULT_SLEEP_INTERVAL));
      tgDaemon.getThreadPool('T')->setNbThreads(1);
    
    // migration error handler

    tgDaemon.addThreadPool(
      new castor::server::SignalThreadPool("MigrationErrorHandlerThread", new castor::tape::tapegateway::MigratorErrorHandlerThread(retryMigrationSvc),  DEFAULT_SLEEP_INTERVAL));
    tgDaemon.getThreadPool('M')->setNbThreads(1);
    
    // recaller error handler

    tgDaemon.addThreadPool(
      new castor::server::SignalThreadPool("RecallerErrorHandlerThread", new castor::tape::tapegateway::RecallerErrorHandlerThread(retryRecallSvc), DEFAULT_SLEEP_INTERVAL ));
      tgDaemon.getThreadPool('R')->setNbThreads(1); 
    
    // recaller/migration dynamic thread pool

    tgDaemon.addThreadPool(
			   new castor::server::TCPListenerThreadPool("WorkerThread", new castor::tape::tapegateway::WorkerThread(),tgDaemon.listenPort(),true, minThreadsNumber, maxThreadsNumber )); 
			   
    // start the daemon

    tgDaemon.start();


  }// end try block
  catch (castor::exception::Exception e) {
    std::cerr << "Caught castor exception : "
     << sstrerror(e.code()) << std::endl
     << e.getMessage().str() << std::endl;
    return -1;
  }
  catch (...) {
    
    std::cerr << "Caught general exception!" << std::endl;    
    return -1;
    
  } 

  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, castor::tape::tapegateway::DAEMON_STOP, 0, NULL);
  return 0;
}

//------------------------------------------------------------------------------
// TapeGatewayDaemon Constructor
// also initialises the logging facility
//------------------------------------------------------------------------------

castor::tape::tapegateway::TapeGatewayDaemon::TapeGatewayDaemon() : castor::server::BaseDaemon("TapeGateway") 
{
 
  // Initializes the DLF logging. This includes
  // registration of the predefined messages
  // Initializes the DLF logging. This includes
  // defining the predefined messages

  castor::dlf::Message messages[] =
    { {DAEMON_START, "Service startup"},
      {DAEMON_STOP, "Service shoutdown"},
      {NO_RETRY_POLICY_FOUND, "Incomplete parameters for retry policy"},
      {FATAL_ERROR, "Fatal error"}, 
      {PRODUCER_GETTING_TAPES, "VdqmRequestsProducer: getting tapes to submit"},
      {PRODUCER_NO_TAPE, "VdqmRequestsProducer: no tape to submit"},
      {PRODUCER_CANNOT_UPDATE_DB,"VdqmRequestsProducer: cannot update db"},
      {PRODUCER_QUERYING_VMGR,"VdqmRequestsProducer: querying vmgr" },
      {PRODUCER_VMGR_ERROR,"VdqmRequestsProducer: vmgr error" },
      {PRODUCER_SUBMITTING_VDQM , "VdqmRequestsProducer: submitting to vdqm"},
      {PRODUCER_VDQM_ERROR,"VdqmRequestsProducer: vdqm error"},
      {CHECKER_GETTING_TAPES, "VdqmRequestsChecker: getting tapes to check"},
      {CHECKER_NO_TAPE, "VdqmRequestsChecker: no tape to check"},
      {CHECKER_QUERYING_VDQM, "VdqmRequestsChecker: querying vdqm"},
      {CHECKER_LOST_VDQM_REQUEST, "VdqmRequestsChecker: request was lost or out of date"},
      {CHECKER_CANNOT_UPDATE_DB, "VdqmRequestsChecker: cannot update db"},
      {CHECKER_RELEASING_UNUSED_TAPE, "VdqmRequestsChecker: releasing BUSY tape"},
      {CHECKER_VMGR_ERROR, "VdqmRequestsChecker: vmgr error, impossible to reset BUSY state"},
      {LINKER_GETTING_STREAMS,"TapeStreamLinker: getting streams to resole"},
      {LINKER_NO_STREAM, "TapeStreamLinker: no stream to resolve" },
      {LINKER_QUERYING_VMGR,"TapeStreamLinker: querying vmgr"},
      {LINKER_LINKING_TAPE_STREAM, "TapeStreamLinker: association tape-stream done"},
      {LINKER_CANNOT_UPDATE_DB,"TapeStreamLinker: cannot update db"},
      {LINKER_RELEASED_BUSY_TAPE,"TapeStreamLinker: released BUSY tape"},
      {LINKER_NO_TAPE_AVAILABLE, "No tape available in such tapepool"},
      {MIG_ERROR_GETTING_FILES, "MigratorErrorHandlerThread: getting failed tapecopy"},
      {MIG_ERROR_NO_FILE, "MigratorErrorHandlerThread: no failed tapecopy"},
      {MIG_ERROR_RETRY,"MigratorErrorHandlerThread: retry this migration"},
      {MIG_ERROR_FAILED,"MigratorErrorHandlerThread: fail this migration"},
      {MIG_ERROR_RETRY_BY_DEFAULT, "MigratorErrorHandlerThread: retry this migration without applying the policy"},
      {MIG_ERROR_CANNOT_UPDATE_DB, "MigratorErrorHandlerThread: cannot update db"},
      {REC_ERROR_GETTING_FILES,"RecallerErrorHandlerThread: getting failed tapecopy"},
      {REC_ERROR_NO_FILE,"RecallerErrorHandlerThread: no failed tapecopy" },
      {REC_ERROR_RETRY,"RecallerErrorHandlerThread: retry this recall"},
      {REC_ERROR_FAILED,"RecallerErrorHandlerThread: fail this recall"},
      {REC_ERROR_RETRY_BY_DEFAULT,"RecallerErrorHandlerThread: retry this recall without applying the policy" },
      {REC_ERROR_CANNOT_UPDATE_DB,"RecallerErrorHandlerThread: cannot update db" },
      {WORKER_MESSAGE_RECEIVED,"Worker: received a message"},
      {WORKER_UNKNOWN_CLIENT, "Worker: unknown client"},
      {WORKER_INVALID_REQUEST, "Worker: invalid request"},
      {WORKER_INVALID_CAST, "Worker: invalid cast"},
      {WORKER_DISPATCHING, "Worker: dispatching request"},
      {WORKER_RESPONDING, "Worker: responding to the aggregator"},
      {WORKER_CANNOT_RESPOND, "Worker: cannot respond to the aggregator"},
      {WORKER_CANNOT_RECEIVE, "Worker: cannot receive message"},
      {WORKER_UNKNOWN_EXCEPTION, "Worker: unexpected exception"},
      {WORKER_VOLUME_REQUESTED, "Worker: received volume request"},
      {WORKER_GETTING_VOLUME, "Worker: getting volume from db"},
      {WORKER_NO_VOLUME, "Worler: no volume found"},
      {WORKER_NO_FILE, "Worker: no file found for such volume"},
      {WORKER_VOLUME_FOUND, "Worker: volume found"},
      {WORKER_RECALL_NOTIFIED,"Worker: received recall notification"},
      {WORKER_RECALL_GET_DB_INFO,"Worker: getting data from db for recalled file" },
      {WORKER_RECALL_FILE_NOT_FOUND,"Worker: recalled file not found"},
      {WORKER_RECALL_NS_CHECK,"Worker: checking nameserver"},
      {WORKER_RECALL_NS_FAILURE,"Worker: nameserver error for recalled file"},
      {WORKER_RECALL_DB_UPDATE,"Worker: updating db after recall notification"},
      {WORKER_RECALL_CANNOT_UPDATE_DB,"Worker: cannot update db for recalled"},
      {WORKER_RECALL_CHECK_FILE_SIZE,"Worker: checking file size of recalled file"},
      {WORKER_RECALL_WRONG_FILE_SIZE, "Worker: wrong file size for recalled file"}, 
      {WORKER_RECALL_COMPLETED_UPDATE_DB, "Worker: update the db after full recall completed"},
      {WORKER_MIGRATION_NOTIFIED,"Worker: received migration notification"},
      {WORKER_MIGRATION_GET_DB_INFO, "Worker: getting data from db for migrated file"},
      {WORKER_MIGRATION_FILE_NOT_FOUND, "Worker: migrated file not found"},
      {WORKER_MIGRATION_VMGR_UPDATE, "Worker: updating vmgr for migrated file"},
      {WORKER_MIGRATION_NS_UPDATE, "Worker: updating nameserver for migrated file"},
      {WORKER_REPACK_NS_UPDATE, "Worker: updating nameserver for repacked file"},
      {WORKER_MIGRATION_NS_FAILURE, "Worker: nameserver error for migrated/repacked file"},
      {WORKER_MIGRATION_VMGR_FAILURE, "Worker: vmgr error for migrated/repacked file"},
      {WORKER_MIGRATION_DB_UPDATE,"Worker: updating db for migrated file"},
      {WORKER_MIGRATION_CANNOT_UPDATE_DB,"Worker: cannot update db for migrated file"},
      {WORKER_RECALL_REQUESTED, "Worker: file to recall requested"},
      {WORKER_RECALL_RETRIEVED , "Worker: file to recall retrieved from db"},
      {WORKER_RECALL_RETRIEVING_DB_ERROR, "Worker: db error while retrieving file to recall"},
      {WORKER_NO_FILE_TO_RECALL, "Worker: no more file to recall"},
      {WORKER_MIGRATION_REQUESTED,"Worker: file to migrate requested"},
      {WORKER_MIGRATION_RETRIEVED,"Worker: file to migrate retrieved from db"},
      {WORKER_MIGRATION_RETRIEVING_DB_ERROR,"Worker: db error while retrieving file to migrate"},
      {WORKER_NO_FILE_TO_MIGRATE,"Worker: no more file to migrate" },
      {WORKER_END_NOTIFICATION,"Worker: received end transaction notification"},
      {WORKER_END_DB_UPDATE,"Worker: updating db after end transaction"},
      {WORKER_END_DB_ERROR, "Worker: db error while updating for end transaction"},
      {WORKER_END_RELEASE_TAPE,"Worker: releasing BUSY tape after end transaction"},
      {WORKER_CANNOT_RELEASE_TAPE, "Worker: cannot release BUSY tape after end transaction"},
      {WORKER_FILE_ERROR_NOTIFIED, "Worker: received file error notification"},
      {WORKER_FILE_ERROR_DB_UPDATE,"Worker: updating db after file error notification"},
      {WORKER_FILE_ERROR_CANNOT_UPDATE_DB,"Worker: db error while updating for file error notification"},
      {WORKER_TAPE_MAKED_FULL,"Worker: set tape as FULL"},
      {WORKER_CANNOT_MARK_TAPE_FULL,"Worker: cannot set the tape as FULL"},
      {-1, ""}
    };
  dlfInit(messages);


  // get the port

 				      
  char* tmp=NULL;

  // This let's try to  the tapegateway port in castor.conf 
   
  if ( (tmp= getconfent("TAPEGATEWAY","PORT",0)) != NULL ){ 
    char* dp = tmp;
    m_listenPort = strtoul(tmp, &dp, 0);      
    if (*dp != 0) {
        castor::exception::Internal ex;
        ex.getMessage() << "Bad port value in enviroment variable " 
                        << tmp << std::endl;
        throw ex;
    }
    if ( m_listenPort > 65535 ){
        castor::exception::Internal ex;
        ex.getMessage() << "Given port no. in enviroment variable "
                        << "exceeds 65535 !"<< std::endl;
        throw ex;
    }
  } else {
     m_listenPort= castor::TAPEGATEWAY_DEFAULT_NOTIFYPORT;
  }

}


void castor::tape::tapegateway::TapeGatewayDaemon::parseCommandLine(int argc, char* argv[]){
  if (argc < 1 ) {
    usage();
    return;
    }
  Coptind = 1;
  Copterr = 1;
  int c;
  while ( (c = Cgetopt(argc,argv,"fh")) != -1 ) {
    switch (c) {
    case 'f':
      m_foreground = true;
      break;
    case 'h':
      usage();
      exit(0);
    default:
      usage();
      exit(0);
    }
  }

}

void castor::tape::tapegateway::TapeGatewayDaemon::usage(){
  std::cout << "\nUsage: " << "tapegateway [-f][-h]\n" 
	    << "-f     : to run in foreground\n"
	    <<std::endl; 
}

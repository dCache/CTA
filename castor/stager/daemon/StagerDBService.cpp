/*********************************************************************************************************/
/* cpp version of the "stager_db_service.c" represented by a thread calling the right request's handler */
/*******************************************************************************************************/

#include "castor/stager/dbService/StagerRequestHelper.hpp"
#include "castor/stager/dbService/StagerCnsHelper.hpp"
#include "castor/stager/dbService/StagerReplyHelper.hpp"
#include "castor/stager/dbService/StagerRequestHandler.hpp"
#include "castor/stager/dbService/StagerJobRequestHandler.hpp"
#include "castor/stager/dbService/StagerDBService.hpp"

#include "castor/stager/dbService/StagerGetHandler.hpp"
#include "castor/stager/dbService/StagerRepackHandler.hpp"
#include "castor/stager/dbService/StagerPrepareToGetHandler.hpp"
#include "castor/stager/dbService/StagerPrepareToPutHandler.hpp"
#include "castor/stager/dbService/StagerPutHandler.hpp"
#include "castor/stager/dbService/StagerPutDoneHandler.hpp"
#include "castor/stager/dbService/StagerPrepareToUpdateHandler.hpp"
#include "castor/stager/dbService/StagerUpdateHandler.hpp"
#include "castor/stager/dbService/StagerRmHandler.hpp"
#include "castor/stager/dbService/StagerSetGCHandler.hpp"




#include "castor/server/SelectProcessThread.hpp"
#include "castor/BaseObject.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"

#include "stager_constants.h"
#include "Cns_api.h"
#include "expert_api.h"
#include "serrno.h"
#include "dlf_api.h"

#include "castor/dlf/Dlf.hpp"
#include "castor/dlf/Param.hpp"


#include "osdep.h"
#include "Cnetdb.h"
#include "Cpwd.h"
#include "Cgrp.h"
#include "stager_uuid.h"
#include "Cuuid.h"
#include "u64subr.h"
#include "marshall.h"
#include "net.h"
#include "getconfent.h"

#include "castor/IAddress.hpp"
#include "castor/IObject.hpp"
#include "castor/Services.hpp"
#include "castor/stager/IStagerSvc.hpp"
#include "castor/BaseObject.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/Constants.hpp"

#include "castor/exception/Exception.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"


#include <iostream>
#include <string>

#define STAGER_OPTIONS 10

/* (from the latest stager_db_service.cpp) copied from the JobManagerDaemon.cpp */
#define DEFAULT_NOTIFICATION_PORT 15011






namespace castor{
  namespace stager{
    namespace dbService{

     
      /****************/
      /* constructor */
      /**************/
      StagerDBService::StagerDBService() throw()
      {
	
	this->types.resize(STAGER_OPTIONS);
	ObjectsIds auxTypes[] = {OBJ_StageGetRequest,
				 OBJ_StagePrepareToGetRequest,
				 OBJ_StageRepackRequest,
				 OBJ_StagePutRequest,
				 OBJ_StagePrepareToPutRequest,
				 OBJ_StageUpdateRequest,
				 OBJ_StagePrepareToUpdateRequest,
				 OBJ_StageRmRequest,
				 OBJ_SetFileGCWeight,
				 OBJ_StagePutDoneRequest};
	
	for(int i= 0; i< STAGER_OPTIONS; i++){
	  this->types.at(i) = auxTypes[i];
	}
	
	
	/* Initializes the DLF logging */
	/*	castor::dlf::Messages messages[]={{1, "Starting StagerDBService Thread"},{2, "StagerRequestHelper"},{3, "StagerCnsHelper"},{4, "StagerReplyHelper"},{5, "StagerRequestHelper failed"},{6, "StagerCnsHelper failed"},{7, "StagerReplyHelper failed"},{8, "StagerHandler"}, {9, "StagerHandler successfully finished"},{10,"StagerHandler failed finished"},{11, "StagerDBService Thread successfully finished"},{12, "StagerDBService Thread failed finished"}};
		castor::dlf::dlf_init("StagerDBService", messages);*/
      }

      /****************/
      /* destructor  */
      /**************/
      StagerDBService::~StagerDBService() throw(){};



      /*************************************************************/
      /* Method to get a subrequest to do using the StagerService */
      /***********************************************************/
      castor::IObject* StagerDBService::select() throw(castor::exception::Exception){
	castor::stager::IStagerSvc* stgService;

	castor::IService* svc =
	  castor::BaseObject::services()->
	  service("DbStagerSvc", castor::SVC_DBSTAGERSVC);
	if (0 == svc) {
	  castor::exception::Exception ex(SEINTERNAL);
	  ex.getMessage()<<"(StagerDBService) Impossible to get the stgService"<<std::endl;
	  throw ex;
	}
	stgService = dynamic_cast<castor::stager::IStagerSvc*>(svc);
	if (0 == stgService) {
	  castor::exception::Exception ex(SEINTERNAL);
	  ex.getMessage()<<"(StagerDBService) Got a bad stgService"<<std::endl;
	  throw ex;
	}
	
	castor::stager::SubRequest* subrequestToProcess = stgService->subRequestToDo(this->types);
	
	return(subrequestToProcess);
      }


      /*********************************************************/
      /* Thread calling the specific request's handler        */
      /***************************************************** */
      void StagerDBService::process(castor::IObject* subRequestToProcess) throw(castor::exception::Exception){
	try {


	  /*******************************************/
	  /* We create the helpers at the beginning */
	  /*****************************************/
	   
	  stgCnsHelper = new StagerCnsHelper();
	  if(stgCnsHelper == NULL){
	    castor::exception::Exception ex(SEINTERNAL);
	    ex.getMessage()<<"(StagerDBService) Impossible to create the StagerCnsHelper"<<std::endl;
	    throw ex;
	  }
	  
	  stgRequestHelper = new StagerRequestHelper();
	  if(stgRequestHelper == NULL){
	    castor::exception::Exception ex(SEINTERNAL);
	    ex.getMessage()<< "(StagerDBService) Impossible to create the StagerRequestHelper"<<std::endl;
	    throw ex;
	  }


	  /* we get the Cuuid_t fileid (needed to logging in dl)  */
	  castor::dlf::Param param[]= {castor::dlf::Param("Standard Message","Helpers successfully created")};
	  castor::dlf::dlf_writep( nullCuuid, DLF_LVL_USAGE, 1, 1, param);/*   */
	  
	 

	  /* set the subrequest on the helper(obtained on the select() thread method) */
	  stgRequestHelper->setSubrequest(dynamic_cast<castor::stager::SubRequest*>(subRequestToProcess));
	 

	  /* settings BaseAddress */
	  stgRequestHelper->settingBaseAddress();
	 	 
	  /* get the uuid subrequest string version and check if it is valid */
	  /* we can create one !*/
	  stgRequestHelper->setSubrequestUuid();
	  
	  /* obtain all the fileRequest associated to the subrequest using the dbService to get the foreign representation */
	  stgRequestHelper->getFileRequest();/* get from subrequest */
	  int type = stgRequestHelper->fileRequest->type();

	  /* get the associated client and set the iClientAsString variable */
	  stgRequestHelper->getIClient();
	 

	  /* get the stgCnsHelper->fileid needed for the logging in dlf */
	  stgCnsHelper->getFileid();


	  /* get the uuid request string version and check if it is valid */
	  stgRequestHelper->setRequestUuid();

	 
	  /* get the svcClass */
	  stgRequestHelper->getSvcClass();
	  
	 

	  /* create and fill request->svcClass link on DB */
	  stgRequestHelper->linkRequestToSvcClassOnDB();
	  	
	  
	  /* ask sebastien about this!:  */
	  /* #ifdef HAVE_REQUEST_UUID */
	  /* requestUuid = stgRequestHelper->getOrCreateRequest()->uuid();*/
	  /* #endif */ 
	  /* otherwise, requestUuid = (from string to Cuuid_t )request->reqId(); */
	  

	  mode_t mask = (mode_t) stgRequestHelper->fileRequest->mask();
	  stgCnsHelper->cnsSettings(stgRequestHelper->fileRequest->euid(), stgRequestHelper->fileRequest->egid(), mask);//to do!!!
	  stgRequestHelper->setUsernameAndGroupname();

	  /* get the castorFile of the subrequest */
	  /*  stgRequestHelper->getCastorFile(); */
	 
	  
	  /* for the required file: check existence (and create if necessary), and permission */
	  stgCnsHelper->setSubrequestFileName(stgRequestHelper->subrequest->fileName());
	  bool fileExist = stgCnsHelper->createCnsFileIdAndStat_setFileExist();
	  if(!fileExist){

	    /* depending on fileExist and type, check if the file needed is to be created or throw exception */
	    if(stgRequestHelper->isFileToCreateOrException(fileExist)){

	      mode_t mode = (mode_t) stgRequestHelper->subrequest->modeBits();
	      /* using Cns_creatx and Cns_stat c functions, create the file and update Cnsfileid and Cnsfilestat structures */
	      stgCnsHelper->createFileAndUpdateCns(mode, (stgRequestHelper->svcClass));
	    }
	    
	  }

	  /* check if the user (euid,egid) has the right permission for the request's type. otherwise-> throw exception  */
	  stgRequestHelper->checkFilePermission();


                    
	  castor::dlf::Param parameter[] = {castor::dlf::Param("Standard Message","Starting specific subrequestHandler")};
	  castor::dlf::dlf_writep( nullCuuid, DLF_LVL_USAGE, 1, 1, parameter);
	  

	  /**************************************************/
	  /* "from now on we can log the invariants in DLF"*/
	  /*  fileidPointer = &cnsFileid  */
	  /*  fileid_ts = cnsFileid "and we remember these settings in the thread-specific version of the fileid" */ 
	 
	  switch(type){
	 
	  case OBJ_StageGetRequest:
	    {
	      StagerGetHandler *stgGetHandler = new StagerGetHandler(stgRequestHelper, stgCnsHelper);
	      if(stgGetHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerGetHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]= {castor::dlf::Param("Standard Message","StagerGetHandler starting")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);
		stgGetHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerGetHandler successfully finished")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);

		/* for the GET and UPDATE we have to notify the Job Manager*/
		if(stgGetHandler->getNewSubrequestStatus() == SUBREQUEST_READYFORSCHED){
		  char jobManagerHost[CA_MAXHOSTNAMELEN+1];
		  int jobManagerPort = DEFAULT_NOTIFICATION_PORT;
		   
		  char* value = getconfent("JOBMANAGER", "NOTIFYPORT", 0);
		  if(value != NULL ){
		    jobManagerPort = std::strtol(value, 0, 10);
		    if (jobManagerPort == 0) {
		      jobManagerPort = DEFAULT_NOTIFICATION_PORT;
		    } else if (jobManagerPort > 65535) {
		      castor::exception::Exception e(EINVAL);
		      e.getMessage() << "Invalid NOTIFYPORT value configured: " << jobManagerPort
				     << "- must be < 65535" << std::endl;
		      throw e;
		    }
		  }else{
		    castor::exception::Exception e(EINVAL);
		    e.getMessage() << "Null JobManager NOTIFYPORT value configured: " << jobManagerPort << std::endl;
		    throw e;
		  }
		  
		  value = getconfent("JOBMANAGER", "HOST", 0);
		  if(value != NULL ){
		    strncpy(jobManagerHost, value, CA_MAXHOSTNAMELEN);
		    jobManagerHost[CA_MAXHOSTNAMELEN] = '\0';
		  }else{
		    castor::exception::Exception e(EINVAL);
		    e.getMessage() << "Null JobManager HOST value configured: "<< jobManagerHost<<std::endl;
		    throw e;
		  }

		  sendNotification(jobManagerHost, jobManagerPort, 1);
		}
		
		delete stgGetHandler;
	      }catch(castor::exception::Exception ex){
		delete stgGetHandler;
		throw ex;
	      }
	     
	    }
	    break;

	  
	  case OBJ_StagePrepareToGetRequest:
	    {
	      StagerPrepareToGetHandler *stgPrepareToGetHandler = new StagerPrepareToGetHandler(stgRequestHelper, stgCnsHelper);
	      if(stgPrepareToGetHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerPrepareToGetHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerPrepareToGetHandler starting")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);
		stgPrepareToGetHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerPrepareToGetHandler successfully finished")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);
		delete stgPrepareToGetHandler;
	      }catch(castor::exception::Exception ex){
		delete stgPrepareToGetHandler;
		throw ex;
	      }
	     
	    }
	    break;


	  case OBJ_StageRepackRequest:
	    {
	      StagerRepackHandler *stgRepackHandler = new StagerRepackHandler(stgRequestHelper, stgCnsHelper);
	      if(stgRepackHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerRepackHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerRepackHandler starting")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);
		stgRepackHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerRepackHandler successfully finished")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);
		delete stgRepackHandler;
	      }catch(castor::exception::Exception ex){
		delete stgRepackHandler;
		throw ex;
	      }
	      
 
	    }
	    break;

	  case OBJ_StagePrepareToPutRequest:
	    {
	      StagerPrepareToPutHandler *stgPrepareToPutHandler = new StagerPrepareToPutHandler(stgRequestHelper, stgCnsHelper);
	      if(stgPrepareToPutHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerPrepareToPutHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerPrepareToPutHandler starting")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);      
		stgPrepareToPutHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerPutHandler successfully finished")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);
		delete stgPrepareToPutHandler;
	      }catch(castor::exception::Exception ex){
		delete stgPrepareToPutHandler;
		throw ex;
	      }
	     
	    }
	    break;

	  case OBJ_StagePrepareToUpdateRequest:
	    {
	      bool toRecreateCastorFile = !(fileExist && (((stgRequestHelper->subrequest->flags()) & O_TRUNC) == 0));

	      StagerPrepareToUpdateHandler *stgPrepareToUpdateHandler = new StagerPrepareToUpdateHandler(stgRequestHelper, stgCnsHelper, toRecreateCastorFile);
	      if(stgPrepareToUpdateHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerPrepareToUpdateHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerPrepareToUpdateHandler starting")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param); 
		stgPrepareToUpdateHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerPrepareToUpdate successfully finished")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);
		delete stgPrepareToUpdateHandler;
	      }catch(castor::exception::Exception ex){
		delete stgPrepareToUpdateHandler;
		throw ex;
	      }
	      
	    }
	    break;
	    
	  case OBJ_StagePutRequest:
	    {
	      StagerPutHandler *stgPutHandler = new StagerPutHandler(stgRequestHelper, stgCnsHelper);
	      if(stgPutHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerPutHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerPutHandler starting")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);
		stgPutHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerPutHandler successfully finished")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);
		delete stgPutHandler;
	      }catch(castor::exception::Exception ex){
		delete stgPutHandler;
		throw ex;
	      }
	      
	    }
	    break;

	  case OBJ_StageUpdateRequest:
	    {
	      bool toRecreateCastorFile = !(fileExist && (((stgRequestHelper->subrequest->flags()) & O_TRUNC) == 0));
	      StagerUpdateHandler *stgUpdateHandler = new StagerUpdateHandler(stgRequestHelper, stgCnsHelper, toRecreateCastorFile);
	      if(stgUpdateHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerUpdateHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerUpdateHandler starting")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);
		stgUpdateHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerUpdateHandler successfully finished")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);

		/* for the GET and UPDATE we have to notify the Job Manager*/
		if(stgUpdateHandler->getNewSubrequestStatus() == SUBREQUEST_READYFORSCHED){
		  
		  char jobManagerHost[CA_MAXHOSTNAMELEN+1];
		  int jobManagerPort = DEFAULT_NOTIFICATION_PORT;
		  
		  char* value = getconfent("JOBMANAGER", "NOTIFYPORT", 0);
		  if(value != NULL ){
		    jobManagerPort = std::strtol(value, 0, 10);
		    if (jobManagerPort == 0) {
		      jobManagerPort = DEFAULT_NOTIFICATION_PORT;
		    } else if (jobManagerPort > 65535) {
		      castor::exception::Exception e(EINVAL);
		      e.getMessage() << "Invalid NOTIFYPORT value configured: " << jobManagerPort
				     << "- must be < 65535" << std::endl;
		      throw e;
		    }
		  }else{
		    castor::exception::Exception e(EINVAL);
		    e.getMessage() << "Null JobManager NOTIFYPORT value configured: " << jobManagerPort << std::endl;
		    throw e;
		  }
		  
		  value = getconfent("JOBMANAGER", "HOST", 0);
		  if(value != NULL ){
		    strncpy(jobManagerHost, value, CA_MAXHOSTNAMELEN);
		    jobManagerHost[CA_MAXHOSTNAMELEN] = '\0';
		  }else{
		    castor::exception::Exception e(EINVAL);
		    e.getMessage() << "Null JobManager HOST value configured: "<< jobManagerHost<<std::endl;
		    throw e;
		  }
		  
		  sendNotification(jobManagerHost, jobManagerPort, 1);
		}

		delete stgUpdateHandler;
	      }catch(castor::exception::Exception ex){
		delete stgUpdateHandler;
		throw ex;
	      }
	    }
	    break;

	  case OBJ_StagePutDoneRequest:
	    {
	      StagerPutDoneHandler *stgPutDoneHandler = new StagerPutDoneHandler(stgRequestHelper, stgCnsHelper);
	      if(stgPutDoneHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerPutDoneHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerPutDoneHandler starting")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);
		stgPutDoneHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerPutDone successfully finished")};
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);
		delete stgPutDoneHandler;
	      // stgPutDoneHandler.jobOriented;
	      }catch(castor::exception::Exception ex){
		delete stgPutDoneHandler;
		throw ex;
	      }
	    }
	    break;
	      
	  case OBJ_StageRmRequest:
	    {
	      StagerRmHandler *stgRmHandler = new StagerRmHandler(stgRequestHelper, stgCnsHelper);
	      if(stgRmHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerRmHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerRmHandler starting")};/* 414 */
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);     
		stgRmHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerRmHandler successfully finished")};/* 417 */
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);
		delete stgRmHandler;
	      }catch(castor::exception::Exception ex){
		delete stgRmHandler;
		throw ex;
	      }
	    }
	    break;

	  case OBJ_SetFileGCWeight:
	    {
	      StagerSetGCHandler *stgSetGCHandler = new StagerSetGCHandler(stgRequestHelper, stgCnsHelper);
	      if(stgSetGCHandler == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService) Impossible to execute the StagerSetFileGCWeightHandler"<<std::endl;
		throw ex;
	      }
	      try{
		castor::dlf::Param param[]={castor::dlf::Param("Standard Message","StagerSetFileGCWeightHandler starting")};/* 436 */
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param);
		stgSetGCHandler->handle();/**/
		castor::dlf::Param param2[]={castor::dlf::Param("Standard Message","StagerSetFileGCWeightHandler successfully finished")};/* 439*/
		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_USAGE, 1, 1, param2);
		delete stgSetGCHandler;
	      }catch(castor::exception::Exception ex){
		delete stgSetGCHandler;
		throw (ex);
	      }
		
	    }
	    break;
	      
	  }


	  if(stgRequestHelper != NULL){
	    if(stgRequestHelper->baseAddr) delete stgRequestHelper->baseAddr;
	    delete stgRequestHelper;
	  }
	  
	  if(stgCnsHelper) delete stgCnsHelper;
	 
	  
	  /* we have to process the exception and reply to the client in case of error  */
	}catch(castor::exception::Exception ex){

	  castor::dlf::Param params[] = {castor::dlf::Param("Standard Message",sstrerror(ex.code())),castor::dlf::Param("Precise Message",ex.getMessage().str())};/* 459 */
	  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 2, 1, params);

	  /* we have to set the subrequest status to SUBREQUEST_FAILED_FINISHED */
	  /* but not setGetNextSubrequestStatus!!  */
	  if(stgRequestHelper!=NULL){
	    /* update subrequest status */
	    if(stgRequestHelper->subrequest != NULL){
	      stgRequestHelper->updateSubrequestStatus(SUBREQUEST_FAILED_FINISHED);
	    }
	    /* reply to the client in case of error*/
	    if(stgRequestHelper->iClient != NULL){
	      StagerReplyHelper *stgReplyHelper = new StagerReplyHelper;
	      if(stgReplyHelper == NULL){
		castor::exception::Exception ex(SEINTERNAL);
		ex.getMessage()<<"(StagerDBService catch exception) Impossible to get the stgReplyHelper"<<std::endl;
		throw ex;
	      }
	      stgReplyHelper->setAndSendIoResponse(stgRequestHelper,stgCnsHelper->fileid,ex.code(),ex.getMessage().str());
	      stgReplyHelper->endReplyToClient(stgRequestHelper);
	      delete stgReplyHelper->ioResponse;
	      delete stgReplyHelper;
	    }else{
	      if((stgRequestHelper->dbService)&&(stgRequestHelper->subrequest)){
		stgRequestHelper->dbService->updateRep(stgRequestHelper->baseAddr, stgRequestHelper->subrequest, true);
	      }
	    }

	    if(stgRequestHelper != NULL){
	      if(stgRequestHelper->baseAddr) delete stgRequestHelper->baseAddr;
	      delete stgRequestHelper;
	    }
	    
	    if(stgCnsHelper != NULL){
	      delete stgCnsHelper;/* 567 */
	    }
	  }  
 
	}catch (...){
	  
	  castor::dlf::Param params[] = {castor::dlf::Param("Standard Message","Caught general exception in StagerDBService")}; /* 485 */
	  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 2, 1, params);
	  
	  /* we have to set the subrequest status to SUBREQUEST_FAILED */
	  /* but not setGetNextSubrequestStatus!!  */
	  if(stgRequestHelper != NULL){
	    if(stgRequestHelper->subrequest != NULL){
	      stgRequestHelper->subrequest->setStatus(SUBREQUEST_FAILED_FINISHED);
	    }
	    /* reply to the client in case of error*/
	    if(stgRequestHelper->iClient != NULL){
	      StagerReplyHelper *stgReplyHelper = new StagerReplyHelper;	  
	      if(stgReplyHelper != NULL){
		stgReplyHelper->setAndSendIoResponse(stgRequestHelper,stgCnsHelper->fileid, SEINTERNAL, "General Exception");	      
		stgReplyHelper->endReplyToClient(stgRequestHelper);
		delete stgReplyHelper->ioResponse;
		delete stgReplyHelper;
	      }
	    }else{
	      if((stgRequestHelper->dbService)&&(stgRequestHelper->subrequest)){
		stgRequestHelper->dbService->updateRep(stgRequestHelper->baseAddr, stgRequestHelper->subrequest, true);
	      }
	    }
	  }
	 
	  if(stgRequestHelper != NULL){
	    if(stgRequestHelper->baseAddr) delete stgRequestHelper->baseAddr;
	    delete stgRequestHelper;
	  }
	  
	  if(stgCnsHelper != NULL){
	    delete stgCnsHelper;
	  }
	}


      }/* end StagerDBService::process */



      /* --------------------------------------------------- */
      /* sendNotification()                                  */
      /*                                                     */
      /* Send a notification message to another CASTOR2      */
      /* daemon using the NotificationThread model. This     */
      /* will wake up the process and trigger it to perform  */
      /* a dedicated action.                                 */
      /*                                                     */
      /* This function is copied from BaseServer.cpp. We     */
      /* could have wrapped the C++ call to be callable in   */
      /* C. However, as the stager is being re-written in    */
      /* C++ anyway we take this shortcut.                   */
      /*                                                     */
      /* Input:  (const char *) host - host to notify        */
      /*         (const int) port - notification por         */
      /*         (const int) nbThreads - number of threads   */
      /*                      to wake up on the server       */
      /*                                                     */
      /* Return: nothing (void). All errors ignored          */
      /* --------------------------------------------------- */
      void StagerDBService::sendNotification(const char *host, const int port, const int nbThreads) {
	struct hostent *hp;
	struct sockaddr_in sin;
	int s;
	char buf[HYPERSIZE + LONGSIZE];
	char *p;
	
	/* Check arguments */
	if ((host[0] == '\0') || !port) {
	  return;
	}
	
	/* Resolve host address */
	if ((hp = Cgethostbyname(host)) == NULL) {
	  return;
	}
	
	/* Prepare the request */
	p = buf;
	const long NOTIFY_MAGIC = 0x44180876;
	marshall_LONG(p, NOTIFY_MAGIC);
	marshall_LONG(p, nbThreads);
	
	// Create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	  return;
	}
	
	/* Send packet containing notification magic number + service number */
	memset((char *) &sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	sendto(s, buf, sizeof(buf), 0, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
	netclose(s);
      }
   
    }//end namespace dbService
  }//end namespace stager
}//end namespace castor
      
    





    







   

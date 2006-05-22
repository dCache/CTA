/******************************************************************************
 *                      RepackFileStager.cpp
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
 * @(#)$RCSfile: RepackFileStager.cpp,v $ $Revision: 1.5 $ $Release$ $Date: 2006/05/22 06:47:22 $ $Author: felixehm $
 *
 *
 *
 * @author Felix Ehm
 *****************************************************************************/
 
#include "RepackFileStager.hpp"
 
namespace castor {
	namespace repack {
		
	
		
//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
RepackFileStager::RepackFileStager(RepackServer* srv) throw ()
{
	/*
  char *tmp = (char*) malloc(CA_MAXHOSTNAMELEN*sizeof(char));
	
	if ( !(tmp = getconfent((char*)CNS_CAT, (char*)CNS_HOST,0)) ){
		castor::exception::Internal ex;
		ex.getMessage() << "Unable to initialise FileListHelper with nameserver "
		                << "entry in castor config file";
		throw ex;	
	}
	m_ns = new std::string(tmp);
  */
  ptr_server = srv;
	m_filehelper = new FileListHelper(ptr_server->getNsName());
  m_dbhelper = new DatabaseHelper();	
	//m_filehelper = new FileListHelper((*m_ns));
}


//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
RepackFileStager::~RepackFileStager() throw() {
	delete m_dbhelper;
	delete m_filehelper;
}


//------------------------------------------------------------------------------
// run
//------------------------------------------------------------------------------
void RepackFileStager::run(void *param) throw() {
	
		/* Lets see, if good old pal DB has a Job for us */
		RepackSubRequest* sreq = m_dbhelper->requestToDo();
		
		if ( sreq != NULL ){
	    	
			stage_trace(2,"New Job! %s , %s ",sreq->vid().c_str(), sreq->cuuid().c_str());
			castor::dlf::Param params[] =
			{castor::dlf::Param("VID", sreq->vid()),
			 castor::dlf::Param("ID", sreq->id()),
			 castor::dlf::Param("STATUS", sreq->status())};
			castor::dlf::dlf_writep(stringtoCuuid(sreq->cuuid()), DLF_LVL_SYSTEM, 22, 3, params);

			/* main decision for further handling */
			try {
				stage_files(sreq);
			/* stage the files, and we forget errors, the logs are written anyway */
			}
			catch (castor::exception::Internal e){
				delete sreq;
			}
			catch (castor::exception::Exception ex){
				delete sreq;
			}
			delete sreq;
		}
}


//------------------------------------------------------------------------------
// stop
//------------------------------------------------------------------------------
void RepackFileStager::stop() throw() {
}



//------------------------------------------------------------------------------
// stage_files
//------------------------------------------------------------------------------
void RepackFileStager::stage_files(RepackSubRequest* sreq) throw() {

	int rc=0;				// the return code for the stager_prepareToGet call.
	int i,j;
	std::string reqId = "";
	_Cuuid_t cuuid = stringtoCuuid(sreq->cuuid());
	castor::stager::StagePrepareToGetRequest req;
	
	stage_trace(3,"Updating SubRequest with its segs");
	castor::dlf::dlf_writep(cuuid, DLF_LVL_SYSTEM, 29, 0, NULL);
	/* get the Segs for this tape !*/
	if ( m_filehelper->getFileListSegs(sreq,cuuid) )
		return;


  // ---------------------------------------------------------------
	// This part has to be removed, if the stager also accepts only fileid
	// as parameter. For now we have to get the paths first.
	stage_trace(3,"Now, get the paths.");
	std::vector<std::string>* filelist = m_filehelper->getFilePathnames(sreq,cuuid);
	std::vector<std::string>::iterator filename = filelist->begin();
	// ---------------------------------------------------------------
//	std::vector<u_signed64>* filelist = m_filehelper->getFileList(sreq,cuuid);
//	std::vector<u_signed64>::iterator fileid;
	
	/* here we build the Request for the stager */
	while (filename != filelist->end()) {
		castor::stager::SubRequest *subreq = new castor::stager::SubRequest();
		subreq->setFileName((*filename));
		subreq->setRepackVid(sreq->vid());	// this marks the Request as a 'special' one.
		subreq->setProtocol(ptr_server->getProtocol());	// we have to specify a protocol, otherwise our call is not scheduled!
		subreq->setRequest(&req);
		req.addSubRequests(subreq);
		filename++;
	}

	stage_trace(3,"Sending the stager request.");
	castor::dlf::dlf_writep(cuuid, DLF_LVL_SYSTEM, 26, 0, NULL);
	try {
		if (filelist->size() > 0 )
			sendStagerRepackRequest(&req, &reqId);
	
	}catch (castor::exception::Exception ex){
		castor::dlf::Param params[] =
		{castor::dlf::Param("Standard Message", sstrerror(ex.code())),
		 castor::dlf::Param("Precise Message", ex.getMessage().str())};
		castor::dlf::dlf_writep(cuuid, DLF_LVL_ERROR, 21, 2, params);
	}
	
	/* Setting the SubRequest's Cuuid to the stager one for better follow */
	stage_trace(3," Stagerrequest (now new RepackSubRequest CUUID ): %s",(char*)reqId.c_str());
	castor::dlf::Param param[] = {castor::dlf::Param("New reqID", reqId)};
	castor::dlf::dlf_writep(cuuid, DLF_LVL_SYSTEM, 33, 1, param);
	cuuid = stringtoCuuid(reqId);
	sreq->setCuuid(reqId);

	stage_trace(2,"File are sent to stager, repack request updated.");
	castor::dlf::dlf_writep(cuuid, DLF_LVL_SYSTEM, 25, 0, NULL);
	sreq->setFilesStaging(filelist->size());
        sreq->setFiles(filelist->size());
        sreq->setStatus(SUBREQUEST_STAGING);
	m_dbhelper->updateSubRequest(sreq,true, cuuid);

	filelist->clear();
	delete filelist;
      

	// the Request has now status SUBREQUEST_STAGING
}
		


void RepackFileStager::sendStagerRepackRequest(castor::stager::StagePrepareToGetRequest* req, std::string *reqId) throw ()
{
	struct stage_options opts;
  char *tmp;
	opts.stage_host = (char*)ptr_server->getStagerName().c_str();	
	opts.service_class = (char*)ptr_server->getServiceClass().c_str();

	/** Uses a BaseClient to handle the request */
	castor::client::BaseClient client(stage_getClientTimeout());
	client.setOption(NULL);	/** to initialize the RH,stager, etc. */
	
	castor::stager::RequestHelper reqh(req);
	reqh.setOptions(&opts);

	/** Send the Request */
	std::vector<castor::rh::Response*>respvec;
	castor::client::VectorResponseHandler rh(&respvec);
	
	
	*reqId = client.sendRequest(req, &rh);

	/** like the normale api call, we have to check for errors in the answer*/
	for (int i=0; i<respvec.size(); i++) {

      // Casting the response into a FileResponse !
      castor::rh::FileResponse* fr = dynamic_cast<castor::rh::FileResponse*>(respvec[i]);
      if (0 == fr) {
        castor::exception::Exception e(SEINTERNAL);
        e.getMessage() << "Error in dynamic cast, response was NOT a file response.\n"
                       << "Object type is "
                       << castor::ObjectsIdStrings[respvec[i]->type()];
        throw e;
      }
		if ( fr->errorCode() || fr->status() != 6 ){
			/** TODO: DLF logging for errors on files*/
			std::cerr 
					<< fr->castorFileName() << " " << fr->fileId() << " "
					<<"(size "<< fr->fileSize() << ", "
					<<"status "<< fr->status() << ") "
					<<":"<< fr->errorMessage() << "(" << fr->errorCode() << ")"
					<< std::endl;
		}
		
		delete fr;
	}
	respvec.clear();
}



//------------------------------------------------------------------------------
// createSvcClass, for repacking to different tapepool than origin
//------------------------------------------------------------------------------	
/*
void RepackFileStager::createSvcClass(RepackSubRequest *rsub)
{
	// 1. enter new SvcClass into DB 
	
	std::ofstream myfile;
	struct vmgr_tape_info tape_info;

	if (vmgr_querytape (vid, 0, &tape_info, NULL) < 0) {
		// The check was done before !
		return;
	}
	
	myfile.open ("migratorPolicy.pl");
	myfile << "";
	myfile.close();
}
		
*/	
		
	} //END NAMESPACE REPACK
}//END NAMESPACE CASTOR

/******************************************************************************
 *                      RepackWorker.cpp
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
 * @(#)$RCSfile: RepackWorker.cpp,v $ $Revision: 1.14 $ $Release$ $Date: 2006/03/14 17:44:20 $ $Author: felixehm $
 *
 *
 *
 * @author Felix Ehm
 *****************************************************************************/



#include "RepackWorker.hpp"




namespace castor {


 namespace repack {
  
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
RepackWorker::RepackWorker()
{
  m_databasehelper = new castor::repack::DatabaseHelper();
}



//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RepackWorker::~RepackWorker() throw()
{
  delete m_databasehelper;
}



//------------------------------------------------------------------------------
// runs the htread starts by threadassign()
//------------------------------------------------------------------------------
void RepackWorker::run(void* param) throw()
{
  stage_trace(3, "RepackWorker started and Request is now handled!");
  // Response for client
  RepackAck ack;

  castor::io::ServerSocket* sock = (castor::io::ServerSocket*) param;

  // Retrieve info on the client
  unsigned short port;
  unsigned long ip;
  try {
    sock->getPeerIp(port, ip);
  } catch(castor::exception::Exception e) {
    // "Exception caught : ignored" message
    castor::dlf::Param params[] =
      {castor::dlf::Param("Standard Message", sstrerror(e.code())),
       castor::dlf::Param("Precise Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 4, 2, params);
    return;
  }
  
  
  // "New Request Arrival" message
  castor::dlf::Param params[] =
    {castor::dlf::Param("IP", castor::dlf::IPAddress(ip)),
     castor::dlf::Param("Port", port)};
  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 1, 2, params);
  
  stage_trace(2, "Request from %3d.%3d.%3d.%3d:%d", 
  				 (ip%265),((ip >> 8)%256),((ip >> 16)%256),(ip >> 24), port);

  

  castor::repack::RepackRequest* rreq = 0;

  // get the incoming request
  try {
    castor::IObject* obj = sock->readObject();
    rreq = dynamic_cast<castor::repack::RepackRequest*>(obj);
    if (0 == rreq) {
      delete obj; 
      // "Invalid Request" message
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 6);
      ack.setErrorCode(EINVAL);
      ack.setErrorMessage("Invalid Request!");
    }
  } catch (castor::exception::Exception e) {
    // "Unable to read Request from socket" message
    castor::dlf::Param params[] =
      {castor::dlf::Param("Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 7, 1, params);
    ack.setErrorCode(EINVAL);
    ack.setErrorMessage(e.getMessage().str());
    stage_trace(3, (char*)e.getMessage().str().c_str());
    return;
  }
  
  
/****************************************************************************/
  try{
      

	switch ( rreq->command() ){
	
	 case REMOVE_TAPE:	removeRequest(rreq);break;
	 case REPACK: 			handleRepack(rreq);break;
	 case GET_STATUS: 	
								getStatus(rreq);
								ack.addRequest(rreq);
								break;
	 case GET_STATUS_ALL:getStatusAll(rreq);
								ack.addRequest(rreq);
								break;
	 default:break;	/** Do nothing by default */
	}

  }catch (castor::exception::Internal e) {
    ack.setErrorCode(e.code());
    ack.setErrorMessage(e.getMessage().str());
    stage_trace(2,"%s\n%s",sstrerror(e.code()), e.getMessage().str().c_str() );
  }
/****************************************************************************/  
  
  try {
    sock->sendObject(ack);
  } catch (castor::exception::Exception e) {
  	
    // "Unable to send Ack to client" message
    castor::dlf::Param params[] =
      {castor::dlf::Param("Standard Message", sstrerror(e.code())),
       castor::dlf::Param("Precise Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 11, 2, params);
    std::cout << e.getMessage();
  }
  
  delete rreq;
  delete sock;  // originally created from RepackServer

}



//------------------------------------------------------------------------------
// Stops the Thread
//------------------------------------------------------------------------------
void RepackWorker::stop() throw()
{


}

//------------------------------------------------------------------------------
// Retrieves the subrequests for client answer
//------------------------------------------------------------------------------
void RepackWorker::getStatus(RepackRequest* rreq) throw()
{
	std::vector<RepackSubRequest*>::iterator tape = rreq->subRequest().begin();
	while ( tape != rreq->subRequest().end() ){
		
		RepackSubRequest* tmp = 
								m_databasehelper->getSubRequestByVid( (*tape)->vid());
		
		if ( tmp != NULL ) {
			rreq->subRequest().erase((tape)); /** we don't need the subrequest from 
															the client, we replace it */
         rreq->subRequest().insert(tape,tmp);
      }
		tape++;
	}
}


//------------------------------------------------------------------------------
// Retrieves all subrequests in the repack system
//------------------------------------------------------------------------------
void RepackWorker::getStatusAll(RepackRequest* rreq) throw()
{
	std::vector<castor::repack::RepackSubRequest*>* result = 
											m_databasehelper->getAllSubRequests();
	
	std::vector<RepackSubRequest*>::iterator tape = result->begin();
	
	while ( tape != result->end() ){
		rreq->subRequest().push_back((*tape));
		tape++;
	}
}



//------------------------------------------------------------------------------
// Removes a request from repack
//------------------------------------------------------------------------------
void RepackWorker::removeRequest(RepackRequest* rreq) throw ()
{
	std::vector<RepackSubRequest*>::iterator tape = rreq->subRequest().begin();
	while ( tape != rreq->subRequest().end() ){
		
		RepackSubRequest* tmp = m_databasehelper->getSubRequestByVid( (*tape)->vid());
		if ( tmp != NULL ) {
			m_databasehelper->remove(tmp);
			tape++;
		}
	}
	
}



//------------------------------------------------------------------------------
// handle RepackRequest
//------------------------------------------------------------------------------
void RepackWorker::handleRepack(RepackRequest* rreq) throw()
{
	int tapecnt =0;
	/* check if the tape(s)/pool exist */
	getPoolInfo(rreq);
 
	
	for ( tapecnt = 0; tapecnt < rreq->subRequest().size() ; tapecnt++ ) 
	{
		RepackSubRequest* subRequest = rreq->subRequest().at(tapecnt);
		// set the status
		subRequest->setStatus(SUBREQUEST_READYFORSTAGING);
		// and for each subrequest a own cuuid, for DLF logging
		Cuuid_t cuuid = nullCuuid;
		Cuuid_create(&cuuid);
		char buf[CUUID_STRING_LEN+1];
		Cuuid2string(buf, CUUID_STRING_LEN+1, &cuuid);
		std::string tmp (buf,CUUID_STRING_LEN);
		subRequest->setCuuid(tmp);
 }

	/* Go to DB, but only if tapes were found !*/
	if ( tapecnt )
	  	m_databasehelper->storeRequest(rreq);
}


//------------------------------------------------------------------------------
// Retrieves Information about the Pool of the Request
//------------------------------------------------------------------------------
int RepackWorker::getPoolInfo(castor::repack::RepackRequest* rreq) throw()
{
	char *pool_name;
	int flags;
	vmgr_list list;
	struct vmgr_tape_info *lp;
	unsigned int nbvol = 0;
	std::vector<RepackSubRequest*>::iterator tape;
	

	if ( rreq != NULL )	{

		if ( rreq->pool().length() > 0 ) {
			/* pool name given , check if exists and get the tapes for it*/
			pool_name = (char*)rreq->pool().c_str();
			if (vmgr_querypool (pool_name, NULL, NULL, NULL, NULL) < 0) {
				castor::exception::Internal ex;
				ex.getMessage() << "RepackWorker::getPoolInfo(..): "
								<< "No such Pool %s" << pool_name << std::endl;
				castor::dlf::Param params[] =
				{castor::dlf::Param("POOL", pool_name),
		         castor::dlf::Param("ERRORTEXT", sstrerror(serrno))};
		  		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 19, 2, params);
				throw ex;
			}
			/* get the tapes, check them and add them to the Request */
			flags = VMGR_LIST_BEGIN;
			castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 20, 0, NULL);
			while ((lp = vmgr_listtape (NULL, pool_name, flags, &list)) != NULL) {
				flags = VMGR_LIST_CONTINUE;
				if ( checkTapeForRepack(lp->vid) ){
					RepackSubRequest* tmp = new RepackSubRequest();
					tmp->setVid(lp->vid);
					tmp->setRequestID(rreq);
					//tmp->setOriginPool(lp->poolname);
					rreq->addSubRequest(tmp);
					nbvol++;
				}
				/* if the tape is unvalid for repacking, a message is already
				 * written to DLF by checkTapeforRepack(..)*/
			}
			vmgr_listtape (NULL, pool_name, VMGR_LIST_END, &list);
			return nbvol;
		}


		/* No tape pool given, so check the given tapes */
		else {
			lp   = (struct vmgr_tape_info*)(malloc(sizeof(vmgr_tape_info)));
			tape = rreq->subRequest().begin();
			while ( tape != rreq->subRequest().end() ){
				if ( checkTapeForRepack((*tape)->vid()) ){
					memset((void*)lp,'\0',sizeof(vmgr_tape_info));
					vmgr_querytape ((char*)(*tape)->vid().c_str(), 0, lp, NULL);
					//(*tape)->setOriginPool(lp->poolname);
					nbvol++;
					tape++;
				}
				else
					rreq->subRequest().erase(tape);
			}
			free(lp);
		}
		return nbvol;
	}
	return -1;
}



bool RepackWorker::checkTapeForRepack(std::string tapename)
{
	return ( (getTapeStatus(tapename)>0) && !m_databasehelper->is_stored(tapename) );
}


//------------------------------------------------------------------------------
// Gets the Tape information
//------------------------------------------------------------------------------
int RepackWorker::getTapeStatus(std::string tapename){
	
	std::string p_stat = "?";
	struct vmgr_tape_info tape_info;
	typedef std::map<int,std::string> TapeDriveStatus;
	TapeDriveStatus statusname;
	char *vid = (char*)tapename.c_str();
	
	

	/* just to have a nice access for output */
	statusname[DISABLED] 	= "Disabled";
	statusname[EXPORTED] 	= "Exported";
	statusname[TAPE_BUSY] 	= "Busy";
	statusname[TAPE_FULL] 	= "Full";
	statusname[TAPE_RDONLY] = "TAPE_RDONLY";
	statusname[ARCHIVED] 	= "Archived";
	statusname[0]			= "Free";
	
	/* check if the volume exists and get its status */
	if (vmgr_querytape (vid, 0, &tape_info, NULL) < 0) {
		castor::exception::Internal ex;
		ex.getMessage() << sstrerror(serrno);
		castor::dlf::Param params[] =
        {castor::dlf::Param("VID", vid),
         castor::dlf::Param("ErrorText", sstrerror(serrno))};
  		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 16, 1, params);
		return -1;
	}

	/* Tapestatus is unkown - nothing to be done !*/
	if ( statusname[tape_info.status] == "" ){
		castor::exception::Internal ex;
		ex.getMessage() << 	"Tape "<< vid << " has unkown status, repack abort for this tape!";
		castor::dlf::Param params[] =
        {castor::dlf::Param("VID", vid),
         castor::dlf::Param("Status", tape_info.status)};
  		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 17, 2, params);
  		return -1;
	}
	/* Tape is Free - nothing to be done !*/
	else if ( tape_info.status == 0) {
		castor::exception::Internal ex;
		ex.getMessage() << 	"Tape "<< vid << " is marked as FREE, no repack to be done!";
		castor::dlf::Param params[] =
        {castor::dlf::Param("VID", vid)};
  		castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 18, 1, params);
  		return 1; //TODO: remove, we don't watn to repack free tapes - just for testing purpose
	}
	/* Ok,we got a valid status. fine, we can proceed*/
	else
		p_stat = statusname[tape_info.status];
	
	stage_trace(3, "Volume %s in Pool :%s ==> %s (%d)<==!", 
				vid, tape_info.poolname, p_stat.c_str(), tape_info.status);

	return tape_info.status;
}




   }  // END Namespace Repack
}  // END Namespace Castor


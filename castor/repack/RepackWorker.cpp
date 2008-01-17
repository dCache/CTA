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
 * @(#)$RCSfile: RepackWorker.cpp,v $ $Revision: 1.43 $ $Release$ $Date: 2008/01/17 14:11:19 $ $Author: gtaur $
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
RepackWorker::RepackWorker(RepackServer* pserver)
{
  m_databasehelper =NULL;  
  ptr_server = pserver;
}



//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RepackWorker::~RepackWorker() throw()
{
  delete m_databasehelper;
}



//------------------------------------------------------------------------------
// runs the thread starts by threadassign()
//------------------------------------------------------------------------------
void RepackWorker::run(void* param) 
{
  if  (m_databasehelper == NULL){
    m_databasehelper = new castor::repack::DatabaseHelper();
  }

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
  
  // "New Request Arrival" message
  castor::dlf::Param params[] =
    {castor::dlf::Param("IP", castor::dlf::IPAddress(ip)),
     castor::dlf::Param("Port", port),
     castor::dlf::Param("Command", rreq->command()),
     castor::dlf::Param("User", rreq->userName()),
     castor::dlf::Param("Machine", rreq->machine()),};
  castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 1, 5, params);

  

  switch ( rreq->command() ){

  case REMOVE_TAPE:       removeRequest(rreq);
                          break;
  
  case RESTART:           restart(rreq);
                          break;

  case REPACK:            handleRepack(rreq);
                          ack.setRepackrequest(rreq);
                          break;

  case GET_STATUS:
                          getStatus(rreq);
			  queryForErrors(rreq); 
                          //rreq->setCommand(GET_STATUS);
                          ack.setRepackrequest(rreq);
                          break;
      
  case GET_STATUS_NS:             
             /** the old RepackRequest is removed */
                          getStatus(rreq);
                          //rreq->setCommand(GET_STATUS);
                          ack.setRepackrequest(rreq);
                          break;

  case GET_STATUS_ALL:    getStatusAll(rreq);
                          ack.setRepackrequest(rreq);
                          break;
  case ARCHIVE:           archiveSubRequests(rreq);
                          ack.setRepackrequest(rreq);
                          break;
  case ARCHIVE_ALL:       archiveAllSubRequests(rreq);
                          ack.setRepackrequest(rreq);
                          break;
  case GET_ERRORS:        queryForErrors(rreq); 
                          ack.setRepackrequest(rreq);
                          break;

	  default: break;	/** Do nothing by default */
	}

  }catch (castor::exception::Internal e) {
    ack.setErrorCode(e.code());
    ack.setErrorMessage(e.getMessage().str());
    stage_trace(2,"%s\n%s",sstrerror(e.code()), e.getMessage().str().c_str() );
   
    // Added some log in DLF

    castor::dlf::Param params[] =
      {castor::dlf::Param("Standard Message", sstrerror(e.code())),
       castor::dlf::Param("Precise Message", e.getMessage().str())};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 9, 2, params);

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
    std::cerr << e.getMessage();
  }


  /** Clean everything. */ 
 
  if(rreq !=NULL) freeRepackObj(rreq);
  rreq = NULL;
  delete sock;  // originally created from RepackServer

}



//------------------------------------------------------------------------------
// Stops the Thread
//------------------------------------------------------------------------------
void RepackWorker::stop() 
{


}

//------------------------------------------------------------------------------
// Retrieves the subrequest for client answer
//------------------------------------------------------------------------------
void  RepackWorker::getStatus(RepackRequest* rreq) throw (castor::exception::Exception)
{
  /** this method takes only 1! subrequest, this is normaly ensured by the 
    * repack client 
    */

  if ( rreq== NULL || rreq->repacksubrequest().size()==0 ) {
    castor::exception::Internal ex;
    ex.getMessage() << "RepackWorker::getStatus(..) : Invalid Request" << std::endl;
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 6, 0, NULL);
    throw ex;
  }

  /** Get the SubRequest. We query by VID and receive a full subrequest */

  // we give all the segments to gather information from the name server.

  RepackSubRequest* tmp= m_databasehelper->getSubRequestByVid(rreq->repacksubrequest().at(0)->vid(), true );
  if ( tmp != NULL ) {
      //  we don't need the request from the client, we replace it with the one from DB 

      rreq->repacksubrequest().clear();
      rreq->addRepacksubrequest(tmp);
      //tmp->setRequest(rreq);
  }
}


//------------------------------------------------------------------------------
// Retrieves all the not archived subrequests in the repack system 
//------------------------------------------------------------------------------
void RepackWorker::getStatusAll(RepackRequest* rreq) throw (castor::exception::Exception)
{
  std::vector<castor::repack::RepackSubRequest*>* result = 
						m_databasehelper->getAllSubRequests();
	
	std::vector<RepackSubRequest*>::iterator tape = result->begin();
	
	while ( tape != result->end() ){
		rreq->repacksubrequest().push_back((*tape));
		tape++;
	}
	
	if (result != NULL) delete result;
}


//------------------------------------------------------------------------------
// Archives the finished tapes
//------------------------------------------------------------------------------
void RepackWorker::archiveSubRequests(RepackRequest* rreq) throw (castor::exception::Exception)
{ ///get all finished repack tape

  std::vector<castor::repack::RepackSubRequest*>* result=NULL;
  std::vector<RepackSubRequest*>::iterator tapeToBeArchived;

  if ( rreq== NULL || rreq->repacksubrequest().size()==0 ) {
    castor::exception::Internal ex;
    ex.getMessage() << "RepackWorker::archiveSubRequests(..) : Invalid Request" << std::endl;
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 6, 0, NULL);
    throw ex;
  }

  std::vector<RepackSubRequest*>::iterator tape= rreq->repacksubrequest().begin();

  // Get the All SubRequest  
  while(tape != rreq->repacksubrequest().end()){ 
     result=m_databasehelper->getAllSubRequestsVid((*tape)->vid());
     tapeToBeArchived = result->begin();
     while ( tapeToBeArchived != result->end() ){
       if ((*tapeToBeArchived)->status() == SUBREQUEST_DONE || (*tapeToBeArchived)->status() == SUBREQUEST_FAILED){
         m_databasehelper->archive((*tapeToBeArchived)->vid());
     
	 // For the result
	(*tape)->setStatus(SUBREQUEST_ARCHIVED); 
        (*tape)->setCuuid((*tapeToBeArchived)->cuuid());
        (*tape)->setFiles((*tapeToBeArchived)->files());
        (*tape)->setFilesStaging((*tapeToBeArchived)->filesStaging());
        (*tape)->setFilesMigrating((*tapeToBeArchived)->filesMigrating());
	(*tape)->setFilesFailed((*tapeToBeArchived)->filesFailed());
	(*tape)->setFilesStaged((*tapeToBeArchived)->filesStaged());
        (*tape)->setXsize((*tapeToBeArchived)->xsize());         
        break; 
       }    
       tapeToBeArchived++;       
     }
     tape++;
     if (result !=NULL) delete result;
  }

}


//------------------------------------------------------------------------------
// Archives all finished tapes
//------------------------------------------------------------------------------
void RepackWorker::archiveAllSubRequests(RepackRequest* rreq) throw (castor::exception::Exception)
{
  std::vector<castor::repack::RepackSubRequest*>* result = 
            m_databasehelper->getAllSubRequestsStatus(SUBREQUEST_DONE);
  
  std::vector<RepackSubRequest*>::iterator tape = result->begin();

  /// for the answer we set the status to archived
  while ( tape != result->end() ){
    m_databasehelper->archive((*tape)->vid());
    (*tape)->setStatus(SUBREQUEST_ARCHIVED);
    rreq->repacksubrequest().push_back((*tape));
    tape++;
  }
  delete result;

}



//------------------------------------------------------------------------------
// Restart a  repack subrequest
//------------------------------------------------------------------------------
void RepackWorker::restart(RepackRequest* rreq) throw (castor::exception::Exception)
{   
    /** more than one tape can be restarted at once */
    std::vector<RepackSubRequest*>::iterator tape = rreq->repacksubrequest().begin();
    
    while ( tape != rreq->repacksubrequest().end() ){
      if ( (*tape) == NULL )
        continue;
      /// this DB method returns only running processes (no archived ones)
      RepackSubRequest* tmp = 
        m_databasehelper->getSubRequestByVid( (*tape)->vid(), false );

      if (tmp != NULL ) {
        Cuuid_t cuuid = stringtoCuuid(tmp->cuuid());
        if ( tmp->status() == SUBREQUEST_ARCHIVED){
          castor::exception::Internal ex;
          ex.getMessage() << "This Tape is already archived (" << tmp->vid() << ")" 
                          << std::endl;
          delete tmp;
          throw ex;
        }
        if ( tmp->status() != SUBREQUEST_DONE && tmp->status() != SUBREQUEST_FAILED
              && ( tmp->filesMigrating() || tmp->filesStaging() )
           ) {
          castor::exception::Internal ex;
          ex.getMessage() << "There are still files staging/migrating, Restart abort." <<std::endl;
          delete tmp;
          throw ex;
        }
        tmp->setStatus(SUBREQUEST_RESTART);
 
	tmp->setFilesStaging(0);
	tmp->setFilesMigrating(0);
	tmp->setFilesFailed(0);
	tmp->setFilesFailedSubmit(0);
        m_databasehelper->updateSubRequest(tmp,false,cuuid);
        freeRepackObj(tmp);
      }
      tape++;
    }
}



//------------------------------------------------------------------------------
// Removes a request from repack
//------------------------------------------------------------------------------
void RepackWorker::removeRequest(RepackRequest* rreq) throw (castor::exception::Exception)
{
	
  std::vector<RepackSubRequest*>::iterator tape = rreq->repacksubrequest().begin();
  while ( tape != rreq->repacksubrequest().end() ){
  /** if the vid is not in the repack system, a exception in thrown and
   * send to the client
   */
    RepackSubRequest* tmp = m_databasehelper->getSubRequestByVid( (*tape)->vid(), true );
    
    if ( tmp != NULL ) {
      Cuuid_t cuuid = stringtoCuuid(tmp->cuuid());
      if ( tmp->status() == SUBREQUEST_TOBESTAGED || tmp->status() == SUBREQUEST_READYFORSTAGING )
        tmp->setStatus(SUBREQUEST_READYFORCLEANUP);
      else
        tmp->setStatus(SUBREQUEST_TOBEREMOVED);
      m_databasehelper->updateSubRequest(tmp,false,cuuid);
      freeRepackObj(tmp);
      //m_databasehelper->unlock();
    }
    tape++;
   }
}



//------------------------------------------------------------------------------
// handleRepack
//------------------------------------------------------------------------------
void RepackWorker::handleRepack(RepackRequest* rreq) throw (castor::exception::Exception)
{
  unsigned int tapecnt =0;
  /* check if the tape(s)/pool exist  and if all tapes are valid for repacking*/
  if ( !getPoolInfo(rreq) ) {
    return;
  }
 
  /// set the default serviceclass if none is given
  if ( rreq->svcclass().length() == 0 ){
    rreq->setSvcclass(ptr_server->getServiceClass());
  }

  /// set the default stager if none is given 
  if ( rreq->stager().length() == 0 ){
    rreq->setStager(ptr_server->getStagerName());
  }
  
  for ( tapecnt = 0; tapecnt < rreq->repacksubrequest().size() ; tapecnt++ ) 
  {
    RepackSubRequest* subRequest = rreq->repacksubrequest().at(tapecnt);
    // set the status
    subRequest->setStatus(SUBREQUEST_READYFORSTAGING); // started
    // and for each subrequest a own cuuid, for DLF logging
    Cuuid_t cuuid = nullCuuid;
    Cuuid_create(&cuuid);
    char buf[CUUID_STRING_LEN+1];
    Cuuid2string(buf, CUUID_STRING_LEN+1, &cuuid);
    std::string tmp (buf,CUUID_STRING_LEN);
    subRequest->setCuuid(tmp);
    subRequest->setRetryNb(rreq->retryMax());
  }

  /* Go to DB, but only if tapes were found !*/
  if ( tapecnt ){
    m_databasehelper->storeRequest(rreq);
    //m_databasehelper->unlock(); 
  }
		
}


//------------------------------------------------------------------------------
// Retrieves Information about the Pool of the Request
//------------------------------------------------------------------------------
int RepackWorker::getPoolInfo(castor::repack::RepackRequest* rreq) throw (castor::exception::Exception)
{
	char *pool_name;
	int flags;
	vmgr_list list;
	struct vmgr_tape_info *lp;
	unsigned int nbvol = 0;
	std::vector<RepackSubRequest*>::iterator tape;
	

	if ( rreq != NULL )	{
	   
		// Pool Handling 
		if ( rreq->pool().length() > 0 ) {
			//check if exists 
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
			
			// get the tapes, check them and add them to the Request 
			flags = VMGR_LIST_BEGIN;
			castor::dlf::dlf_writep(nullCuuid, DLF_LVL_SYSTEM, 20, 0, NULL);
			
			while ((lp = vmgr_listtape (NULL, pool_name, flags, &list)) != NULL) {
				flags = VMGR_LIST_CONTINUE;
        /** we don't throw an exception if we find a tape in this pool not 
            in RDONLY/FULL because they are not taken in account for repacking.
          */
				if ( checkTapeForRepack(lp->vid) ){
					RepackSubRequest* tmp = new RepackSubRequest();
					tmp->setVid(lp->vid);
					tmp->setRepackrequest(rreq);
					rreq->addRepacksubrequest(tmp);
					nbvol++;
				}
				/** if the tape is unvalid for repacking, a message is already
				    written to DLF by checkTapeforRepack(..) 
            The lp is just a pointer to a vmgr_list struct, which we allocated,
            therefore no need for freeing 
         */
			}
			vmgr_listtape (NULL, pool_name, VMGR_LIST_END, &list);
			return nbvol;
		}


		/** No tape pool given, so check the given tapes */ 
		else {
      tape = rreq->repacksubrequest().begin();
      while ( tape != rreq->repacksubrequest().end() ){
      /** throws exception if tape is already in queue or has invalid status, as
          well as in any other error cases 
        */
        if ( !checkTapeForRepack((*tape)->vid()) ){
          castor::exception::Internal ex;
          ex.getMessage() << " Tape " << (*tape)->vid() 
                          << " is not marked as FULL or RDONLY !" 
                          << std::endl;
          throw ex;
        }
        nbvol++;
        tape++;
      }
    }
    return nbvol;
  }
  return -1;
}


//------------------------------------------------------------------------------
// checkTapeForRepack
//------------------------------------------------------------------------------
bool RepackWorker::checkTapeForRepack(std::string tapename) throw (castor::exception::Exception)
{
  /** checks if already in queue */
  if  ( m_databasehelper->isStored(tapename)  ){
    castor::exception::Internal ex;
    ex.getMessage() << "Tape " << tapename << " already in repack queue." << std::endl;
    throw ex;
  }
  
  /** checks for valid status */
  return ( getTapeStatus(tapename) );
}


//------------------------------------------------------------------------------
// Validates the status of the tape
//------------------------------------------------------------------------------
int RepackWorker::getTapeStatus(std::string tapename) throw (castor::exception::Exception)
{
	
	struct vmgr_tape_info tape_info;
	char *vid = (char*)tapename.c_str();
	
	/* check if the volume exists and get its status */
  if (vmgr_querytape (vid, 0, &tape_info, NULL) < 0) {
    castor::exception::Internal ex;
    ex.getMessage() << "VMGR returns \"" << sstrerror(serrno) 
                    << "\" for Tape "
                    << tapename 
                    << " (wrong volume name?) " << std::endl;
    castor::dlf::Param params[] =
        {castor::dlf::Param("VID", vid),
         castor::dlf::Param("ErrorText", sstrerror(serrno))};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 16, 1, params);
    throw ex;
  }

  if ( !(tape_info.status & TAPE_FULL) && !(tape_info.status & TAPE_RDONLY)) {
    castor::dlf::Param params[] =
      {castor::dlf::Param("VID", vid)};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_WARNING, 18, 1, params);
    return 0;
  }

	return 1;
}


//------------------------------------------------------------------------------
// Query for Errors 
//------------------------------------------------------------------------------

  void RepackWorker::queryForErrors(RepackRequest* rreq) throw (castor::exception::Exception){
    std::vector<castor::repack::RepackSubRequest*> sreqs=rreq->repacksubrequest();
    std::vector<castor::repack::RepackSubRequest*>::iterator sreq=sreqs.begin();
    castor::repack::RepackMonitor monitor(ptr_server);
    
    while (sreq != sreqs.end()){ 
      RepackResponse* resp=new RepackResponse();
      resp->setVid((*sreq)->vid());
      castor::repack::RepackFileQry* fileQry;

      std::vector<castor::rh::FileQryResponse*> fr;
      std::vector<castor::rh::FileQryResponse*>::iterator stagerResponse;
      try {

    /** get the stats by quering the stager */
    
       monitor.getStats((*sreq), &fr);

      }catch (castor::exception::InvalidArgument inval){
      }catch (castor::exception::Exception ex){
        fileQry= new castor::repack::RepackFileQry();
	fileQry->setErrorCode(ex.code());
	fileQry->setErrorMessage( ex.getMessage().str());
	resp->addRepackfileqry(fileQry);
	continue;
      }
    
      /** get the information for repack **/
      
       /// see, in which status the files are 
  
      stagerResponse = fr.begin();
      while ( stagerResponse != fr.end() ) {
	fileQry= new castor::repack::RepackFileQry();
	if ((*stagerResponse)->errorCode() >0) {
	  fileQry->setErrorCode((*stagerResponse)->errorCode());
	  fileQry->setErrorMessage((*stagerResponse)->errorMessage());
	  fileQry->setFileid((*stagerResponse)->fileId());
	  fileQry->setFilename((*stagerResponse)->fileName());
	  fileQry->setStatus((*stagerResponse)->status());
	  resp->addRepackfileqry(fileQry);
	}
	stagerResponse++;
      }
      rreq->addRepackresponse(resp);
      sreq++;
    }
 }

 }  // END Namespace Repack
}  // END Namespace Castor


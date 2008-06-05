/******************************************************************************
 *                      OraRepackSvc.cpp
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
 * @(#)OraPolicySvc.cpp,v 1.20 $Release$ 2007/04/13 11:58:53 sponcec3
 *
 * Implementation of the IRepackSvc for Oracle
 *
 * @author Giulia Taurelli
 *****************************************************************************/

// Include Files
#include "castor/IAddress.hpp"
#include "castor/IObject.hpp"
#include "castor/IFactory.hpp"
#include "castor/SvcFactory.hpp"
#include "castor/repack/ora/OraRepackSvc.hpp"
#include "castor/Services.hpp"
#include "castor/repack/RepackResponse.hpp"
#include "castor/repack/RepackUtility.hpp"
#include "castor/BaseAddress.hpp"

#include <iostream>
#include <string>
#include "occi.h"

// -----------------------------------------------------------------------
// Instantiation of a static factory class
// -----------------------------------------------------------------------

static castor::SvcFactory<castor::repack::ora::OraRepackSvc>* s_factoryOraRepackSvc =
  new castor::SvcFactory<castor::repack::ora::OraRepackSvc>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------

/// SQL to store a new request
const std::string castor::repack::ora::OraRepackSvc::s_storeRequestStatementString =
  "BEGIN storeRequest(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13);END;";

/// SQL to store all the segment for a request
const std::string castor::repack::ora::OraRepackSvc::s_updateSubRequestSegmentsStatementString=
  "BEGIN updateSubRequestSegments(:1,:2,:3,:4);END;";

/// SQL optimized fillObj
const std::string castor::repack::ora::OraRepackSvc::s_getSegmentsForSubRequestStatementString =
  "BEGIN getSegmentsForSubRequest(:1,:2);END;";

/// SQL to get RepackSubRequest with a certain vid
const std::string castor::repack::ora::OraRepackSvc::s_getSubRequestByVidStatementString =
  "BEGIN getSubRequestByVid(:1,:2);END;";

/// SQL to get RepackSubRequests attached to a request  by status
const std::string castor::repack::ora::OraRepackSvc::s_getSubRequestsByStatusStatementString =
  "BEGIN getSubRequestsByStatus(:1,:2);END;"; 

/// SQL to get all  RepackSubRequests (which are not archived)
const std::string castor::repack::ora::OraRepackSvc::s_getAllSubRequestsStatementString =
  "BEGIN getAllSubRequests(:1); END;"; 

/// SQL to validate a RepackSubrequest to submit to the stager
const std::string castor::repack::ora::OraRepackSvc::s_validateRepackSubRequestStatementString =
  "BEGIN validateRepackSubRequest(:1,:2); END;"; 

/// SQL to resurrect a RepackSubrequest to submit to the stager
const std::string castor::repack::ora::OraRepackSvc::s_resurrectTapesOnHoldStatementString =
  "BEGIN resurrectTapesOnHold(); END;"; 

/// SQL to restart a failed RepackSubRequest

const std::string castor::repack::ora::OraRepackSvc::s_restartSubRequestStatementString="BEGIN restartSubRequest(:1); END;";

/// SQL to changeSubRequestsStatus

const std::string castor::repack::ora::OraRepackSvc::s_changeSubRequestsStatusStatementString="BEGIN changeSubRequestsStatus(:1,:2,:3); END;";

/// SQL to changeAllSubRequestsStatus

const std::string castor::repack::ora::OraRepackSvc::s_changeAllSubRequestsStatusStatementString="BEGIN changeAllSubRequestsStatus(:1,:2); END;";


/// SQL for getting lattest information of a tapy copy to do a repack undo TODO properly 

const std::string castor::repack::ora::OraRepackSvc::s_selectLastSegmentsSituationStatementString=
"select repacksubrequest.id from repacksubrequest,repackrequest where repacksubrequest.requestid=repackrequest.id and repackrequest.creationtime in (select max(creationtime) from (select * from repackrequest where id in (select requestid from repacksubrequest where vid=:1))) and repacksubrequest.vid=:1";


// -----------------------------------------------------------------------
// OraRepackSvc
// -----------------------------------------------------------------------

castor::repack::ora::OraRepackSvc::OraRepackSvc(const std::string name) :
  IRepackSvc(),  
  OraCommonSvc(name),
  m_storeRequestStatement(0),
  m_updateSubRequestSegmentsStatement(0),
  m_getSegmentsForSubRequestStatement(0),
  m_getSubRequestByVidStatement(0),
  m_getSubRequestsByStatusStatement(0),
  m_getAllSubRequestsStatement(0),
  m_validateRepackSubRequestStatement(0),
  m_resurrectTapesOnHoldStatement(0),	
  m_restartSubRequestStatement(0),
  m_changeSubRequestsStatusStatement(0),
  m_changeAllSubRequestsStatusStatement(0),
  m_selectLastSegmentsSituationStatement(0){
}

// -----------------------------------------------------------------------
// ~OraRepackSvc
// -----------------------------------------------------------------------
castor::repack::ora::OraRepackSvc::~OraRepackSvc() throw() {
  reset();
}

// -----------------------------------------------------------------------
// id
// -----------------------------------------------------------------------
const unsigned int castor::repack::ora::OraRepackSvc::id() const {
  return ID();
}

// -----------------------------------------------------------------------
// ID
// -----------------------------------------------------------------------
const unsigned int castor::repack::ora::OraRepackSvc::ID() {
  return castor::SVC_ORAREPACKSVC;
}

//-----------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::repack::ora::OraRepackSvc::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it

  OraCommonSvc::reset();

  try {
    if (m_storeRequestStatement)  deleteStatement(m_storeRequestStatement);
    if (m_updateSubRequestSegmentsStatement)  deleteStatement(m_updateSubRequestSegmentsStatement);
    if (m_getSegmentsForSubRequestStatement) deleteStatement(m_getSegmentsForSubRequestStatement);
    if (m_getSubRequestByVidStatement) deleteStatement(m_getSubRequestByVidStatement);
    if (m_getSubRequestsByStatusStatement) deleteStatement(m_getSubRequestsByStatusStatement);
    if (m_getAllSubRequestsStatement) deleteStatement(m_getAllSubRequestsStatement);
    if (m_validateRepackSubRequestStatement) deleteStatement(m_validateRepackSubRequestStatement);
    if (m_resurrectTapesOnHoldStatement)deleteStatement(m_resurrectTapesOnHoldStatement); 	
    if (m_restartSubRequestStatement) deleteStatement(m_restartSubRequestStatement);
    if (m_changeSubRequestsStatusStatement) deleteStatement(m_changeSubRequestsStatusStatement);
    if (m_changeAllSubRequestsStatusStatement)deleteStatement(m_changeAllSubRequestsStatusStatement);
    if (m_selectLastSegmentsSituationStatement)deleteStatement(m_selectLastSegmentsSituationStatement);     
  } catch (castor::exception::SQLError e) {};
  // Now reset all pointers to 0
  m_storeRequestStatement=0; 
  m_updateSubRequestSegmentsStatement=0;
  m_getSegmentsForSubRequestStatement=0;
  m_getSubRequestByVidStatement=0; 
  m_getSubRequestsByStatusStatement=0;
  m_getAllSubRequestsStatement=0;
  m_validateRepackSubRequestStatement=0;
  m_resurrectTapesOnHoldStatement=0; 	
  m_restartSubRequestStatement=0;
  m_changeSubRequestsStatusStatement=0;
  m_changeAllSubRequestsStatusStatement=0;
  m_selectLastSegmentsSituationStatement=0;
 
}

//------------------------------------------------------------------------------
// private: endTransation
//------------------------------------------------------------------------------

void castor::repack::ora::OraRepackSvc::endTransation() throw (){
  try {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV); 
      svcs()->commit(&ad);
  } catch (castor::exception::Exception e){
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 58, 0, 0);
  }


}



//------------------------------------------------------------------------------
// storeRequest 
//------------------------------------------------------------------------------

castor::repack::RepackAck* castor::repack::ora::OraRepackSvc::storeRequest(castor::repack::RepackRequest* rreq)
						throw ()
{
  RepackAck* ack=NULL;
  oracle::occi::ResultSet *rset =NULL;
  ub2 *lens = NULL;
  char *buffer = NULL;

  stage_trace(2,"Storing Request in DB" );
  if (rreq != NULL) {
    try {

      if ( m_storeRequestStatement == NULL ) {
	m_storeRequestStatement = 
	  createStatement(s_storeRequestStatementString);
        m_storeRequestStatement->registerOutParam
        (13, oracle::occi::OCCICURSOR);
     
      }
     
      // Give all the request information
     
      m_storeRequestStatement->setString(1,rreq->machine());
      m_storeRequestStatement->setString(2,rreq->userName());
      m_storeRequestStatement->setDouble(3,(double)rreq->creationTime());
      m_storeRequestStatement->setString(4,rreq->pool());
      m_storeRequestStatement->setDouble(5,(double)rreq->pid());
      m_storeRequestStatement->setString(6,rreq->svcclass());
      m_storeRequestStatement->setInt(7,rreq->command());
      m_storeRequestStatement->setString(8,rreq->stager());
      m_storeRequestStatement->setDouble(9,rreq->userId());
      m_storeRequestStatement->setDouble(10,(double)rreq->groupId());
      m_storeRequestStatement->setDouble(11,(double)rreq->retryMax());

     // DataBuffer with all the vid (one for each subrequest)

     // loop 

      std::vector<std::string> listOfVids;
      
      std::vector<RepackSubRequest*> listOfSubRequest=rreq->repacksubrequest();
      std::vector<RepackSubRequest*>::iterator subReq=listOfSubRequest.begin();
      std::map<std::string,int> hitMissMap;
      unsigned int maxLen=0;
      unsigned int numTapes=0;

      while (subReq != listOfSubRequest.end()) {
	if (*subReq) {
	  listOfVids.push_back((*subReq)->vid());
          hitMissMap[(*subReq)->vid()]=0;
	  maxLen=maxLen>((*subReq)->vid()).length()?maxLen:((*subReq)->vid()).length();
	  numTapes++;
	}
	subReq++;
      }
      
      if (numTapes==0 || maxLen==0){
	ack=new RepackAck();
	RepackResponse* resp=new RepackResponse();
	resp->setErrorCode(-1);
	resp->setErrorMessage("No Tape given");
	ack->addRepackresponse(resp);
	return ack;
      }

      unsigned int bufferCellSize = maxLen * sizeof(char);

      lens = (ub2*) malloc(maxLen * sizeof(ub2));
      buffer =
	(char*) malloc(numTapes * bufferCellSize);

      // Fill in the structure

      std::vector<std::string>::iterator vid= listOfVids.begin();
      int i=0;

      while (vid != listOfVids.end()){
	lens[i]=(*vid).length();
        strncpy(buffer+(bufferCellSize*i),(*vid).c_str(),lens[i]);
	vid++;
	i++;
      }

      ub4 len=numTapes;
      m_storeRequestStatement->setDataBufferArray
      (12, buffer, oracle::occi::OCCI_SQLT_CHR,
       len, &len, maxLen, lens);

      m_storeRequestStatement->executeUpdate();

      rset=m_storeRequestStatement->getCursor(13);

    // Run through the cursor
      
      ack=new RepackAck(); // to answer back
        
      oracle::occi::ResultSet::Status status = rset->next();
      
      while (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
	RepackResponse* resp = new castor::repack::RepackResponse();
	RepackSubRequest* sub=new RepackSubRequest();
	sub->setVid(rset->getString(1));
	sub->setRepackrequest(NULL);

	resp->setErrorCode(0); // if it is here it has been inserted

	resp->setRepacksubrequest(sub);
	ack->addRepackresponse(resp);

        hitMissMap[sub->vid()]=1; // to optimize the research of failure
	status = rset->next();
      }
      
      if (ack->repackresponse().size() !=  listOfVids.size()) {
	// add miss response	
        std::vector<std::string>::iterator elem=listOfVids.begin();
	while (elem !=listOfVids.end()){
	  if (hitMissMap[*elem] == 0){
	    // failure the request exists
	    RepackResponse* resp = new RepackResponse();
	    RepackSubRequest* sub= new RepackSubRequest(); 
	    sub->setVid(*elem);
	    sub->setRepackrequest(NULL);
	    resp->setRepacksubrequest(sub);
	    resp->setErrorCode(-1);
	    resp->setErrorMessage("An old repack process is still active");
            ack->addRepackresponse(resp);
	  }
	  elem++;
	}

      }
      endTransation();

    } catch (oracle::occi::SQLException ex) {

      // log the error in Dlf

      castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage())
	};
      castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 46, 1, params);
      
      //handle the exception 

      handleException(ex);
      freeRepackObj(ack);
      ack=new RepackAck();
      RepackResponse* resp=new RepackResponse();
      resp->setErrorCode(-1);
      resp->setErrorMessage("Oracle Exception");
      ack->addRepackresponse(resp);
    }
    
  }  
  
  if (rset) delete rset;
  rset=NULL;
  if (buffer) free(buffer);
  buffer=NULL;
  if (lens) free(lens);
  lens=NULL;
  return ack;
}



//------------------------------------------------------------------------------
// updateSubRequest
//------------------------------------------------------------------------------
void castor::repack::ora::OraRepackSvc::updateSubRequest(
        castor::repack::RepackSubRequest* obj) 
        throw ()
{
  
  try {
    /// stores it into the database

    castor::BaseAddress ad;
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    
    svcs()->updateRep(&ad, obj, true);

  /// Exception handling

  } catch (oracle::occi::SQLException  ex) {
    // log the error in Dlf

    castor::dlf::Param params[] =
      {
       castor::dlf::Param("Precise Message", ex.getMessage()),
       castor::dlf::Param("VID", obj->vid())
      };
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 47, 2, params);

    handleException(ex);
  }
}


//------------------------------------------------------------------------------
// updateSubRequestSegments
//------------------------------------------------------------------------------

void castor::repack::ora::OraRepackSvc::updateSubRequestSegments(castor::repack::RepackSubRequest* obj,std::vector<RepackSegment*> listToUpdate) throw ()
{

  if (listToUpdate.empty()) { 
    updateSubRequest(obj);
    return;
  }

  ub4 nb=listToUpdate.size();

  unsigned char (*bufferFileId)[21]=(unsigned char(*)[21]) calloc((nb) * 21, sizeof(unsigned char));
  unsigned char (*bufferErrorCode)[21]=(unsigned char(*)[21]) calloc((nb) * 21, sizeof(unsigned char));
  char * bufferErrorMessage=NULL;

  ub2 *lensFileId=(ub2 *)malloc (sizeof(ub2)*nb);

  ub2 *lensErrorCode=(ub2 *)malloc (sizeof(ub2)*nb);
  ub2 *lensErrorMessage=NULL;

  try {
    /// stores it into the database

    castor::BaseAddress ad;
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);    
    svcs()->updateRep(&ad, obj, false);
    
    if ( m_updateSubRequestSegmentsStatement == NULL ) {
      m_updateSubRequestSegmentsStatement = 
	createStatement(s_updateSubRequestSegmentsStatementString);
    }

    // DataBuffer
    std::vector<RepackSegment*>::iterator elem=listToUpdate.begin();
    unsigned int maxLen=1;

    while (elem != listToUpdate.end()) {
      // errorMessageLenCheck
      if ((*elem)->errorCode()!=0)
	  maxLen=(*elem)->errorMessage().length()>maxLen?(*elem)->errorMessage().length():maxLen;
      elem++;
    }
   
    // now I can allocate the memory for the strings

    unsigned int bufferCellSize = maxLen * sizeof(char);

    lensErrorMessage = (ub2*) malloc(nb * sizeof(ub2));
    bufferErrorMessage = (char*) malloc(nb * bufferCellSize);

    elem=listToUpdate.begin();
    unsigned int i=0;
    while (elem != listToUpdate.end()){ 

      // fileid

      oracle::occi::Number n = (double)((*elem)->fileid());
      oracle::occi::Bytes b = n.toBytes();
      b.getBytes(bufferFileId[i],b.length());
      lensFileId[i] = b.length();

      // errorCode

      n = (double)((*elem)->errorCode());
      b = n.toBytes();
      b.getBytes(bufferErrorCode[i],b.length());
      lensErrorCode[i] = b.length();

      // errorMessage
      
      lensErrorMessage[i]=(*elem)->errorMessage().length();
      if (lensErrorMessage[i] != 0){
	strncpy(bufferErrorMessage+(bufferCellSize*i),(*elem)->errorMessage().c_str(),lensErrorMessage[i]);
      
      } else {
	strncpy(bufferErrorMessage+(bufferCellSize*i),"",1);
	lensErrorMessage[i]=1;
      }

      i++;
      elem++;
    }

    ub4 unused = nb;
    m_updateSubRequestSegmentsStatement->setDouble(1,(double)obj->id()); // subrequest id

    m_updateSubRequestSegmentsStatement->setDataBufferArray(2,bufferFileId, oracle::occi::OCCI_SQLT_NUM, nb, &unused, 21, lensFileId); // fileid
    m_updateSubRequestSegmentsStatement->setDataBufferArray(3,bufferErrorCode, oracle::occi::OCCI_SQLT_NUM,nb, &unused, 21, lensErrorCode); // errorCode
    
    ub4 len=nb;
    m_updateSubRequestSegmentsStatement->setDataBufferArray(4,bufferErrorMessage, oracle::occi::OCCI_SQLT_CHR, len, &len, maxLen, lensErrorMessage); // errorMessage

    m_updateSubRequestSegmentsStatement->executeUpdate();

  } catch (oracle::occi::SQLException  ex) {

    // log the error in Dlf
    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage()),
	 castor::dlf::Param("VID", obj->vid())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 48, 2, params);
    handleException(ex);
  }
  
  // CLEAN UP

  if (bufferFileId) free(bufferFileId);
  bufferFileId=NULL;
  if (bufferErrorCode) free(bufferErrorCode);
  bufferErrorCode=NULL;
  if (bufferErrorMessage) free(bufferErrorMessage);
  bufferErrorMessage=NULL;
  
  if (lensFileId) free(lensFileId);
  lensFileId=NULL;
  if (lensErrorCode) free(lensErrorCode);
  lensErrorCode=NULL;
  if (lensErrorMessage) free(lensErrorMessage);
  lensErrorMessage=NULL;

}


//------------------------------------------------------------------------------
// insertSubRequestSegments
//------------------------------------------------------------------------------

void castor::repack::ora::OraRepackSvc::insertSubRequestSegments(
        castor::repack::RepackSubRequest* obj) 
        throw ()
{
  
  try {
    /// stores it into the database

    castor::BaseAddress ad;
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);   
    svcs()->updateRep(&ad, obj, false);
    svcs()->fillRep(&ad,obj,OBJ_RepackSegment,false); // segments too 

  } catch (oracle::occi::SQLException ex) {
    
     // log the error in Dlf
    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage()),
	 castor::dlf::Param("VID", obj->vid())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 49, 2, params);
    handleException(ex);
    
  }
}

//------------------------------------------------------------------------------
// private:  getSegmentsForSubRequest
//------------------------------------------------------------------------------

void castor::repack::ora::OraRepackSvc::getSegmentsForSubRequest(RepackSubRequest* rsub) throw (castor::exception::Exception){

  oracle::occi::ResultSet * rset=NULL;
  try{
    if ( m_getSegmentsForSubRequestStatement == NULL ) {
      m_getSegmentsForSubRequestStatement = 
	createStatement(s_getSegmentsForSubRequestStatementString);
      m_getSegmentsForSubRequestStatement->registerOutParam
        (2, oracle::occi::OCCICURSOR); 
    }

    m_getSegmentsForSubRequestStatement->setDouble(1, rsub->id());
    m_getSegmentsForSubRequestStatement->executeUpdate();
    rset = m_getSegmentsForSubRequestStatement->getCursor(2);
    oracle::occi::ResultSet::Status status = rset->next();
      
    while (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
	
	RepackSegment* seg= new RepackSegment();
	seg->setFileid((u_signed64)rset->getDouble(1));
	seg->setSegsize((u_signed64)rset->getDouble(2));
	seg->setCompression(rset->getInt(3));
	seg->setFilesec(rset->getInt(4));
	seg->setCopyno(rset->getInt(5));
	seg->setBlockid((u_signed64)rset->getDouble(6));
	seg->setFileseq((u_signed64)rset->getDouble(7));
	seg->setErrorCode(rset->getInt(8));
	seg->setErrorMessage(rset->getString(9));
	seg->setId((u_signed64)rset->getDouble(10));

	// attach it
	seg->setRepacksubrequest(rsub);
	rsub->addRepacksegment(seg);
	status = rset->next();
    }
    
  } catch(oracle::occi::SQLException e){
    if (rset) delete rset;
    rset=NULL;
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getSegmentsForSubRequest"
      << std::endl << e.what();
    throw ex;
  }

}


//------------------------------------------------------------------------------
// getSubRequestByVid 
//------------------------------------------------------------------------------
castor::repack::RepackResponse* 
         castor::repack::ora::OraRepackSvc::getSubRequestByVid(std::string vid, bool fill) throw () { 
  RepackResponse* result = new RepackResponse();
  RepackSubRequest* sub = new RepackSubRequest();
  sub->setVid(vid);
  result->setRepacksubrequest(sub);

  oracle::occi::ResultSet *rset =NULL;
  try{
    if ( m_getSubRequestByVidStatement == NULL ) {
      m_getSubRequestByVidStatement = 
	createStatement(s_getSubRequestByVidStatementString);
      m_getSubRequestByVidStatement->registerOutParam
        (2, oracle::occi::OCCICURSOR); 
    }

    m_getSubRequestByVidStatement->setString(1, vid);
    m_getSubRequestByVidStatement->executeUpdate();

    // RepackSubRequest

    rset = m_getSubRequestByVidStatement->getCursor(2);

    // Run through the cursor
    oracle::occi::ResultSet::Status status = rset->next();

    // we took just the first

    if (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      result->setErrorCode(0);
      sub->setVid(rset->getString(1));
      sub->setXsize((u_signed64)rset->getDouble(2));
      sub->setStatus((castor::repack::RepackSubRequestStatusCode)rset->getInt(3));
      sub->setFilesMigrating((u_signed64)rset->getDouble(4));
      sub->setFilesStaging((u_signed64)rset->getDouble(5));
      sub->setFiles((u_signed64)rset->getDouble(6));
      sub->setFilesFailed((u_signed64)rset->getDouble(7));
      sub->setCuuid(rset->getString(8));
      sub->setSubmitTime((u_signed64)rset->getDouble(9));
      sub->setFilesStaged((u_signed64)rset->getDouble(10));
      sub->setFilesFailedSubmit((u_signed64)rset->getDouble(11));
      sub->setRetryNb((u_signed64)rset->getDouble(12));
      sub->setId((u_signed64)rset->getDouble(13));
      sub->setRepackrequest(NULL); // don't used
    
      if (rset) delete rset;
      rset=NULL;

      // Get the segment and attach it 

      if (fill)  getSegmentsForSubRequest(sub);

     
    } else {
      
    //No tape

      result->setErrorCode(-1);
      result->setErrorMessage("Unknown vid");
      
    }

  } catch (oracle::occi::SQLException ex) {
    
    // log the error in Dlf

    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage()),
	 castor::dlf::Param("VID", vid)
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 50, 2, params);
    result->setErrorCode(-1);
    result->setErrorMessage("repacksever not available");
    handleException(ex);
  
  }catch (castor::exception::Exception  e) {
    
    // log the error in Dlf

    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", e.getMessage().str()),
	 castor::dlf::Param("VID", vid)
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 17, 2, params);
    result->setErrorCode(-1);
    result->setErrorMessage("repacksever not available");
  
  }

  if ( rset ) delete rset;
  rset=NULL;
  return result;

}


//------------------------------------------------------------------------------
// getSubRequestsByStatus
//------------------------------------------------------------------------------

std::vector<castor::repack::RepackSubRequest*> 
		castor::repack::ora::OraRepackSvc::getSubRequestsByStatus( castor::repack::RepackSubRequestStatusCode st, bool fill)throw ()
{
  std::vector<RepackSubRequest*> subs;
  
  RepackSubRequest* resp=NULL;
  oracle::occi::ResultSet *rset = NULL;
  
  try{
    /// Check whether the statements are ok
    if ( m_getSubRequestsByStatusStatement == NULL ) {
      m_getSubRequestsByStatusStatement = createStatement(s_getSubRequestsByStatusStatementString);
      m_getSubRequestsByStatusStatement->registerOutParam
        (2, oracle::occi::OCCICURSOR); // to get a valid repack subrequest request 
    }	
    m_getSubRequestsByStatusStatement->setInt(1,st);
    m_getSubRequestsByStatusStatement->executeUpdate();
    oracle::occi::ResultSet *rs =
      m_getSubRequestsByStatusStatement->getCursor(2);
    // Run through the cursor
    oracle::occi::ResultSet::Status status = rs->next();
    while (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      resp = new castor::repack::RepackSubRequest();
      resp->setVid(rs->getString(1));
      resp->setXsize((u_signed64)rs->getDouble(2));
      resp->setStatus((castor::repack::RepackSubRequestStatusCode) rs->getInt(3));
      resp->setFilesMigrating((u_signed64)rs->getDouble(4));
      resp->setFilesStaging((u_signed64)rs->getDouble(5));
      resp->setFiles((u_signed64)rs->getDouble(6));
      resp->setFilesFailed((u_signed64)rs->getDouble(7));
      resp->setCuuid(rs->getString(8));
      resp->setSubmitTime((u_signed64)rs->getDouble(9));
      resp->setFilesStaged((u_signed64)rs->getDouble(10));
      resp->setFilesFailedSubmit((u_signed64)rs->getDouble(11));
      resp->setRetryNb((u_signed64)rs->getDouble(12));
      resp->setId((u_signed64)rs->getDouble(13));
      
      // GET REQUEST

       castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      svcs()->fillObj(&ad,resp,OBJ_RepackRequest); //get the request

      // SEGMENTS NEEDED
      
      if (fill) getSegmentsForSubRequest(resp);
      
      subs.push_back(resp);
      status = rs->next();

    }

  }catch (oracle::occi::SQLException ex) {

     // log the error in Dlf

    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 51, 1, params);

    handleException(ex);    
    
    //clean up for errors

    std::vector<RepackSubRequest*>::iterator toDelete= subs.begin();
    while (toDelete != subs.end()){
      if (*toDelete) freeRepackObj(*toDelete);
      *toDelete=NULL;
      toDelete++;
    }
    subs.clear();
  } 

  if ( rset ) delete rset;
  rset=NULL;

  return subs;	
}


//------------------------------------------------------------------------------
// getAllSubRequests
//------------------------------------------------------------------------------

castor::repack::RepackAck* 
		castor::repack::ora::OraRepackSvc::getAllSubRequests()
					throw ()
{
  castor::repack::RepackAck* ack=new castor::repack::RepackAck();
  castor::repack::RepackResponse* resp = NULL;
  castor::repack::RepackSubRequest* sub=NULL;
  
  oracle::occi::ResultSet *rset = NULL;
  try{
    /// Check whether the statements are ok
    if ( m_getAllSubRequestsStatement == NULL ) {
      m_getAllSubRequestsStatement = createStatement(s_getAllSubRequestsStatementString);
      m_getAllSubRequestsStatement->registerOutParam
        (1, oracle::occi::OCCICURSOR); // to get a valid repack subrequest request 
    }	
    m_getAllSubRequestsStatement->executeUpdate();
    oracle::occi::ResultSet *rs =
      m_getAllSubRequestsStatement->getCursor(1);
    // Run through the cursor
    oracle::occi::ResultSet::Status status = rs->next();
    while (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      resp = new RepackResponse();
      resp->setErrorCode(0);

      sub = new RepackSubRequest();
      sub->setVid(rs->getString(1));
      sub->setXsize((u_signed64)rs->getDouble(2));
      sub->setStatus((castor::repack::RepackSubRequestStatusCode) rs->getInt(3));
      sub->setFilesMigrating((u_signed64)rs->getDouble(4));
      sub->setFilesStaging((u_signed64)rs->getDouble(5));
      sub->setFiles((u_signed64)rs->getDouble(6));
      sub->setFilesFailed((u_signed64)rs->getDouble(7));
      sub->setCuuid(rs->getString(8));
      sub->setSubmitTime((u_signed64)rs->getDouble(9));
      sub->setFilesStaged((u_signed64)rs->getDouble(10));
      sub->setFilesFailedSubmit((u_signed64)rs->getDouble(11));
      sub->setRetryNb((u_signed64)rs->getDouble(12));
      sub->setId((u_signed64)rs->getDouble(13));
      sub->setRepackrequest(NULL); // not used
      
      resp->setRepacksubrequest(sub);
      ack->addRepackresponse(resp);
      status = rs->next();

    }

  }catch (oracle::occi::SQLException ex) {

     // log the error in Dlf

    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 52, 1, params);

    handleException(ex); 
    freeRepackObj(ack);
    ack=new RepackAck();
    RepackResponse* resp=new RepackResponse();
    resp->setErrorCode(-1);
    resp->setErrorMessage("Oracle Exception");
    ack->addRepackresponse(resp);
  }

  if (rset) delete rset;  
  return ack;	
}



//------------------------------------------------------------------------------
// validateRepackSubRequest
//------------------------------------------------------------------------------

bool  castor::repack::ora::OraRepackSvc::validateRepackSubRequest( RepackSubRequest* tape)
  throw (){
  stage_trace(3,"Validate tape  %s",tape->vid().c_str());
  try{
    /// Check whether the statements are ok
    if ( m_validateRepackSubRequestStatement == NULL ) {
      m_validateRepackSubRequestStatement = createStatement(s_validateRepackSubRequestStatementString);
      m_validateRepackSubRequestStatement->registerOutParam(2, oracle::occi::OCCIINT); // to get a valid repack subrequest request 
   
    }	
    m_validateRepackSubRequestStatement->setDouble(1, tape->id());
    m_validateRepackSubRequestStatement->executeUpdate();
    
    /// get the request we found
    bool ret =  (u_signed64)  m_validateRepackSubRequestStatement->getInt(2) == 1?true:false;
    return ret;
  }catch (oracle::occi::SQLException ex) {

    // log the error in Dlf

    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 53, 1, params);   

    handleException(ex);

  }
  return false;
}


//------------------------------------------------------------------------------
// resurrectTapesOnHold
//------------------------------------------------------------------------------

void  castor::repack::ora::OraRepackSvc::resurrectTapesOnHold()
  throw (){
  stage_trace(3,"Resurrecting  tapes");
  try{
    /// Check whether the statements are ok
    if ( m_resurrectTapesOnHoldStatement == NULL ) {
      m_resurrectTapesOnHoldStatement = createStatement(s_resurrectTapesOnHoldStatementString);
   
    }	
    m_resurrectTapesOnHoldStatement->executeUpdate();
    

  }catch (oracle::occi::SQLException ex) {

    // log the error in Dlf

    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 54, 1, params);   

    handleException(ex);
  }  
}


//------------------------------------------------------------------------------
// restartSubRequest
//------------------------------------------------------------------------------

void  castor::repack::ora::OraRepackSvc::restartSubRequest(u_signed64 srId)
  throw (){
  stage_trace(3,"Restarting Tape");
  try{
    /// Check whether the statements are ok
    if ( m_restartSubRequestStatement == NULL ) {
      m_restartSubRequestStatement = createStatement(s_restartSubRequestStatementString);
   
    }	
    m_restartSubRequestStatement->setDouble(1, srId);
    m_restartSubRequestStatement->executeUpdate();
    

  }catch (oracle::occi::SQLException ex) {

    // log the error in Dlf

    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 55, 1, params); 
    
    handleException(ex);
   
  }  
}


//------------------------------------------------------------------------------
// changeSubRequestsStatus
//------------------------------------------------------------------------------

castor::repack::RepackAck*  castor::repack::ora::OraRepackSvc::changeSubRequestsStatus(std::vector<castor::repack::RepackSubRequest*> srs,  castor::repack::RepackSubRequestStatusCode st)
  throw (){
  RepackAck* ack=new castor::repack::RepackAck();
  oracle::occi::ResultSet *rset =NULL;
  ub2 *lens = NULL;
  char *buffer = NULL;

  try{
    /// Check whether the statements are ok
    if ( m_changeSubRequestsStatusStatement == NULL ) {
      m_changeSubRequestsStatusStatement = createStatement(s_changeSubRequestsStatusStatementString);
      m_changeSubRequestsStatusStatement->registerOutParam
        (3, oracle::occi::OCCICURSOR);
   
    }

    // loop 
    std::map<std::string,int> hitMissMap;

    std::vector<std::string> listOfVids;
    std::vector<RepackSubRequest*>::iterator subReq=srs.begin();
    unsigned int maxLen=0;
    unsigned int numTapes=0;

    while (subReq !=srs.end()) {
      if (*subReq) {
	listOfVids.push_back((*subReq)->vid());
	hitMissMap[(*subReq)->vid()]=0;
	maxLen=maxLen>((*subReq)->vid()).length()?maxLen:((*subReq)->vid()).length();
	numTapes++;
      }
      subReq++;
    }
    
    if (numTapes==0 || maxLen==0){
      ack=new RepackAck();
      RepackResponse* resp=new RepackResponse();
      resp->setErrorCode(-1);
      resp->setErrorMessage("no tape given");
      ack->addRepackresponse(resp);
      return ack;
    }
    
    unsigned int bufferCellSize = maxLen * sizeof(char);

    lens = (ub2*) malloc(maxLen * sizeof(ub2));
    buffer =
      (char*) malloc(numTapes * bufferCellSize);

    // Fill in the structure

    std::vector<std::string>::iterator vid= listOfVids.begin();
    int i=0;
    while (vid != listOfVids.end()){
      lens[i]=(*vid).length();
      strncpy(buffer+(i * bufferCellSize),(*vid).c_str(),lens[i]);
      vid++;
      i++;
    }

    ub4 len=numTapes;
    m_changeSubRequestsStatusStatement->setDataBufferArray
      (1, buffer, oracle::occi::OCCI_SQLT_CHR,
       len, &len, maxLen, lens);

    m_changeSubRequestsStatusStatement->setInt(2,(int)st);
    m_changeSubRequestsStatusStatement->executeUpdate();
    
    // RETURN
    rset = m_changeSubRequestsStatusStatement->getCursor(3);
    // Run through the cursor
    oracle::occi::ResultSet::Status status = rset->next();
    while (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      RepackResponse* resp = new RepackResponse();
      RepackSubRequest* sub = new RepackSubRequest();
      sub->setVid(rset->getString(1));
      sub->setXsize((u_signed64)rset->getDouble(2));
      sub->setStatus((castor::repack::RepackSubRequestStatusCode) rset->getInt(3));
      sub->setFilesMigrating((u_signed64)rset->getDouble(4));
      sub->setFilesStaging((u_signed64)rset->getDouble(5));
      sub->setFiles((u_signed64)rset->getDouble(6));
      sub->setFilesFailed((u_signed64)rset->getDouble(7));
      sub->setCuuid(rset->getString(8));
      sub->setSubmitTime((u_signed64)rset->getDouble(9));
      sub->setFilesStaged((u_signed64)rset->getDouble(10));
      sub->setFilesFailedSubmit((u_signed64)rset->getDouble(11));
      sub->setRetryNb((u_signed64)rset->getDouble(12));
      sub->setId((u_signed64)rset->getDouble(13));
      sub->setRepackrequest(NULL);
      
      resp->setRepacksubrequest(sub);
      ack->addRepackresponse(resp);
      hitMissMap[sub->vid()]=1;    
      status = rset->next();
    } 
   

    if (ack->repackresponse().size() !=  listOfVids.size()) {
      // add miss response	
      std::vector<std::string>::iterator elem=listOfVids.begin();
      while (elem !=listOfVids.end()){
	if (hitMissMap[*elem] == 0){
	  // failure the request exists
	  RepackResponse* resp = new RepackResponse();
	  RepackSubRequest* sub= new RepackSubRequest(); 
	  sub->setVid(*elem);
	  sub->setRepackrequest(NULL);
	  resp->setRepacksubrequest(sub);
	  resp->setErrorCode(-1);
	  resp->setErrorMessage("operation not allowed");
	  ack->addRepackresponse(resp);
	}
	elem++;
      }
    }

    endTransation();

  }catch (oracle::occi::SQLException ex) {

     // log the error in Dlf
    
    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 56, 1, params);

    handleException(ex);
    freeRepackObj(ack);
    ack=new RepackAck();
    RepackResponse* resp=new RepackResponse();
    resp->setErrorCode(-1);
    resp->setErrorMessage("Oracle Exception");
    ack->addRepackresponse(resp);
  }

  if (rset) delete rset;
  rset=NULL;
    
  if (buffer) free(buffer);
  buffer=NULL;
  if (lens)   free(lens);
  lens=NULL;

  return ack;  
}


//------------------------------------------------------------------------------
// changeAllSubRequestsStatus
//------------------------------------------------------------------------------

castor::repack::RepackAck*  castor::repack::ora::OraRepackSvc::changeAllSubRequestsStatus(castor::repack::RepackSubRequestStatusCode st)
  throw (){
  castor::repack::RepackAck* ack=new castor::repack::RepackAck();
  oracle::occi::ResultSet *rset =NULL;

  try{
    /// Check whether the statements are ok
    if ( m_changeAllSubRequestsStatusStatement == NULL ) {
      m_changeAllSubRequestsStatusStatement =createStatement(s_changeAllSubRequestsStatusStatementString);
      m_changeAllSubRequestsStatusStatement->registerOutParam(2, oracle::occi::OCCICURSOR); 
   
    }	
    m_changeAllSubRequestsStatusStatement->setInt(1,(int)st);
    m_changeAllSubRequestsStatusStatement->executeUpdate();
    
    // RETURN
    rset = m_changeAllSubRequestsStatusStatement->getCursor(2);
	// Run through the cursor
    oracle::occi::ResultSet::Status status = rset->next();

    if (status != oracle::occi::ResultSet::DATA_AVAILABLE){
      RepackResponse* resp = new RepackResponse();
      resp->setErrorCode(-1);
      resp->setErrorMessage("No finished tapes to archive");
      ack->addRepackresponse(resp);    
      return ack;
    }

    while (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      RepackResponse* resp = new RepackResponse();
      resp->setErrorCode(0);
      RepackSubRequest* sub= new RepackSubRequest();
      sub->setVid(rset->getString(1));
      sub->setXsize((u_signed64)rset->getDouble(2));
      sub->setStatus((castor::repack::RepackSubRequestStatusCode) rset->getInt(3));
      sub->setFilesMigrating((u_signed64)rset->getDouble(4));
      sub->setFilesStaging((u_signed64)rset->getDouble(5));
      sub->setFiles((u_signed64)rset->getDouble(6));
      sub->setFilesFailed((u_signed64)rset->getDouble(7));
      sub->setCuuid(rset->getString(8));
      sub->setSubmitTime((u_signed64)rset->getDouble(9));
      sub->setFilesStaged((u_signed64)rset->getDouble(10));
      sub->setFilesFailedSubmit((u_signed64)rset->getDouble(11));
      sub->setRetryNb((u_signed64)rset->getDouble(12));
      sub->setId((u_signed64)rset->getDouble(13));
      sub->setRepackrequest(NULL);
      resp->setRepacksubrequest(sub);
      ack->addRepackresponse(resp);
      status = rset->next();
    } 
    endTransation();

  }catch (oracle::occi::SQLException ex) {

     // log the error in Dlf

    castor::dlf::Param params[] =
	{
	 castor::dlf::Param("Precise Message", ex.getMessage())
	};
    castor::dlf::dlf_writep(nullCuuid, DLF_LVL_ERROR, 57, 1, params);    

    handleException(ex);

    freeRepackObj(ack);

    ack=new RepackAck();
    RepackResponse* resp=new RepackResponse();
    resp->setErrorCode(-1);
    resp->setErrorMessage("Oracle Exception");
    ack->addRepackresponse(resp);
  }

  if (rset) delete rset;
  rset=NULL ;
  return ack;
}



//------------------------------------------------------------------------------
//  getLastTapeInformation 
//------------------------------------------------------------------------------

castor::repack::RepackRequest* 
 castor::repack::ora::OraRepackSvc::getLastTapeInformation(std::string vidName)
                    throw ()
{
 
  // TODO AGAIN
  castor::repack::RepackRequest* result=NULL;
  /* try {

    if (vidName.length() ==0) {
      castor::exception::Internal ex;
      ex.getMessage() << "passed Parameter is NULL" << std::endl;
      throw ex;
    }
  
    if ( m_selectLastSegmentsSituationStatement == NULL ) {
      m_selectLastSegmentsSituationStatement = 
                      createStatement(s_selectLastSegmentsSituationStatementString);
    }

    m_selectLastSegmentsSituationStatement->setString(1,vidName);
    oracle::occi::ResultSet *rset = m_selectLastSegmentsSituationStatement->executeQuery();

    if ( rset->next() ){
      /// get the request we found
      u_signed64 id = (u_signed64)rset->getDouble(1);
      result = getSubRequest(id);

      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);

      svcs()->fillObj(&ad,result,OBJ_RepackRequest);
      svcs()->fillObj(&ad,result,OBJ_RepackSegment);
    }
    delete rset;
  }catch (oracle::occi::SQLException ex) {
		castor::exception::Internal ex;
		ex.getMessage()
		<< "OraRepackSvc::getLastTapeInformation(...):"
		<< "Unable to get all RepackSubRequests" 
		<< std::endl << e.getMessage();
		throw ex;
  }
  return  (result==NULL)? NULL:(result->repackrequest());
  */
  return result;
}

 


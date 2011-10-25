/******************************************************************************
 *                castor/tape/tapegateway/ora/OraTapeGateway.hpp
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
 * @(#)$RCSfile: OraTapeGatewaySvc.hpp,v $ $Revision: 1.19 $ $Release$ $Date: 2009/08/10 09:48:17 $ $Author: gtaur $
 *
 * Implementation of the ITapeGatewaySvc for Oracle
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef ORA_ORATAPEGATEWAYSVC_HPP
#define ORA_ORATAPEGATEWAYSVC_HPP 1

// Include Files

#include <u64subr.h>
#include <list>

#include "occi.h"

#include "castor/BaseSvc.hpp"

#include "castor/db/newora/OraCommonSvc.hpp"

#include "castor/tape/tapegateway/daemon/ITapeGatewaySvc.hpp"


namespace castor      {
namespace tape        {    
namespace tapegateway {
namespace ora         {
  
  /**
   * Implementation of the ITapeGatewaySvc for Oracle
   */
  class OraTapeGatewaySvc : 
    public castor::db::ora::OraCommonSvc,
    public virtual castor::tape::tapegateway::ITapeGatewaySvc {
    
  public:
    
    OraTapeGatewaySvc(const std::string name); 
    virtual ~OraTapeGatewaySvc() throw();
    virtual inline unsigned int id() const;
    static unsigned int ID();
    void reset() throw ();
    
  public:

    // To get all the stream without a Tape associated to it
    virtual void  getMigrationMountsWithoutTapes(std::list<castor::tape::tapegateway::ITapeGatewaySvc::Stream>& streams)
      throw (castor::exception::Exception);

    // To create the db link between a Tape and a Stream
    virtual void attachTapesToStreams(
			   const std::list<u_signed64>& strIds,
			   const std::list<std::string>& vids,
			   const std::list<int>& fseqs)
     throw (castor::exception::Exception);

    
    // To get a Tape for which we need to send a request to VDQM 
    virtual void  getTapeWithoutDriveReq (
		    castor::tape::tapegateway::VdqmTapeGatewayRequest& request)
      throw (castor::exception::Exception);

    // To update the db with the information retrieved by VDQM 
    // after we have sent a request

    virtual void attachDriveReqToTape(
	  const castor::tape::tapegateway::VdqmTapeGatewayRequest& tapeRequest,
					  const castor::stager::Tape& tape)
      throw (castor::exception::Exception); 

    // To get all the tapes for which there is a VDQM request     
    virtual void  getTapesWithDriveReqs(
	    std::list<castor::tape::tapegateway::TapeGatewayRequest>& requests,
	    std::list<std::string>& vids,
	    const u_signed64& timeOut) 
      throw (castor::exception::Exception);

    // To restart a request lost by VDQM or a request which was processed 
    // while the tapegateway was down
    virtual void restartLostReqs(
	 const std::list<castor::tape::tapegateway::TapeGatewayRequest>& tapes)
      throw (castor::exception::Exception);
    
    //  To get the best file to migrate when the function is called
    virtual void  getFileToMigrate(
		   const castor::tape::tapegateway::FileToMigrateRequest& req,
		   castor::tape::tapegateway::FileToMigrate& file)
      throw (castor::exception::Exception);
    
    // To update the db for a file which is migrated successfully 
    virtual  void  setFileMigrated(
	      const castor::tape::tapegateway::FileMigratedNotification& resp)
      throw (castor::exception::Exception);

    // To update the db for a file which can't be referenced in the
    // name server anymore after a successful migration (file changed in the mean time)
    virtual  void  setFileStaleInMigration(
              const castor::tape::tapegateway::FileMigratedNotification& resp)
      throw (castor::exception::Exception);

    //  To get the best file to recall when the function is called
    virtual void getFileToRecall(
	      const castor::tape::tapegateway::FileToRecallRequest&  req,
	      castor::tape::tapegateway::FileToRecall& file )
      throw (castor::exception::Exception);

    // To update the db for a file which has been recalled successfully 
    virtual void  setFileRecalled(
	       const castor::tape::tapegateway::FileRecalledNotification& resp)
      throw (castor::exception::Exception);

    // To get the tapecopies which faced a migration failure
    virtual void  getFailedMigrations(
	  std::list<castor::tape::tapegateway::RetryPolicyElement>& candidates)
      throw (castor::exception::Exception);

    // To update the db using the retry migration policy returned values
    virtual void  setMigRetryResult(
				 const std::list<u_signed64>& mjToRetry,
				 const std::list<u_signed64>& mjToFail ) 
      throw (castor::exception::Exception);

    // To get the tapecopies which faced a recall failure 
    virtual void  getFailedRecalls(
	  std::list<castor::tape::tapegateway::RetryPolicyElement>& candidates)
      throw (castor::exception::Exception);
	
    // To update the db using the retry recall policy returned values
    virtual void  setRecRetryResult(
				    const std::list<u_signed64>& rjToRetry,
				    const std::list<u_signed64>& rjToFail) 
      throw (castor::exception::Exception);

    // To access the db to retrieve the information about a completed migration
    virtual void getRepackVidAndFileInfo(
	       const castor::tape::tapegateway::FileMigratedNotification& file,
	       std::string& vid,
	       int& copyNumber,
	       u_signed64& lastModificationTime,
	       std::string& repackVid,
	       std::string& fileClass)
      throw (castor::exception::Exception);

    // To update the database when the tapebridge allows
    // us to serve a request
    virtual void  startTapeSession( 
		     const castor::tape::tapegateway::VolumeRequest& startReq,
		     castor::tape::tapegateway::Volume& volume) 
      throw (castor::exception::Exception); 

    // To update the database when the tape request has been served 
    virtual void endTapeSession(
		  const castor::tape::tapegateway::EndNotification& endRequest)
      throw (castor::exception::Exception); 

    // To access the db to retrieve the information about a completed recall 
    virtual void getSegmentInfo(
			   const FileRecalledNotification &fileRecalled,
			   std::string& vid,
			   int& copyNb)
      throw (castor::exception::Exception); 

    // To update the db after a major failure
    virtual void failTapeSession(
      	 const castor::tape::tapegateway::EndNotificationErrorReport& failure)
      throw (castor::exception::Exception);

    // To update the db after a file failure
    virtual void failFileTransfer(const FileErrorReport& failure)
      throw (castor::exception::Exception);

    // To get tapes to release in vmgr */
    virtual void  getTapeToRelease(const u_signed64& mountTransactionId,
      castor::tape::tapegateway::ITapeGatewaySvc::TapeToReleaseInfo& tape)
      throw (castor::exception::Exception);

    // To commit a  transaction 
    virtual void  endTransaction() 
      throw (castor::exception::Exception);

    // To delete stream with wrong tapepool 
    virtual void deleteMigrationMountWithBadTapePool(
				    const u_signed64 streamId) 
      throw (castor::exception::Exception);

    // To delete taperequest 
    virtual void deleteTapeRequest(const u_signed64& tapeRequestId)
      throw (castor::exception::Exception);

    // Mark tape full for the tape session.
    // This is typically called when a file migration gets a tape full
    // error so that we remember to make the tape as full at the end of
    // the session. Session is passed by VDQM request id (like for end/failSession).
    virtual void flagTapeFullForMigrationSession(const u_signed64& tapeRequestId)
      throw (castor::exception::Exception);

    // To directly commit 
    virtual void commit()
      throw (castor::exception::Exception);

    // To direcly rollback
    virtual void rollback()
      throw (castor::exception::Exception);

  private:

    // To invalidate file
    
    virtual void invalidateFile(const FileErrorReport& failure)
      throw (castor::exception::Exception);

    oracle::occi::Statement *m_getMigrationMountsWithoutTapesStatement;
    oracle::occi::Statement *m_attachTapesToStreamsStatement;
    oracle::occi::Statement *m_getTapeWithoutDriveReqStatement;
    oracle::occi::Statement *m_attachDriveReqToTapeStatement;
    oracle::occi::Statement *m_getTapesWithDriveReqsStatement;
    oracle::occi::Statement *m_restartLostReqsStatement;
    oracle::occi::Statement *m_getFileToMigrateStatement;
    oracle::occi::Statement *m_setFileMigratedStatement;
    oracle::occi::Statement *m_setFileStaleInMigrationStatement;
    oracle::occi::Statement *m_getFileToRecallStatement;
    oracle::occi::Statement *m_setFileRecalledStatement;
    oracle::occi::Statement *m_getFailedMigrationsStatement;
    oracle::occi::Statement *m_setMigRetryResultStatement;
    oracle::occi::Statement *m_getFailedRecallsStatement;
    oracle::occi::Statement *m_setRecRetryResultStatement;
    oracle::occi::Statement *m_getRepackVidAndFileInfoStatement;
    oracle::occi::Statement *m_startTapeSessionStatement;
    oracle::occi::Statement *m_endTapeSessionStatement;
    oracle::occi::Statement *m_getSegmentInfoStatement;
    oracle::occi::Statement *m_failFileTransferStatement;
    oracle::occi::Statement *m_invalidateFileStatement;
    oracle::occi::Statement *m_getTapeToReleaseStatement;
    oracle::occi::Statement *m_deleteMigrationMountWithBadTapePoolStatement;
    oracle::occi::Statement *m_deleteTapeRequestStatement;
    oracle::occi::Statement *m_flagTapeFullForMigrationSession;
  }; // end of class OraTapeGateway
  
} // end of namespace ora
} // end of namespace tapegateway  
}  // end of namespace tape
} // end of namespace castor

#endif // ORA_ORATAPEGATEWAYSVC_HPP

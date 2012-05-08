/******************************************************************************
 *                      OraTapeGatewaySvc.cpp
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
 * Implementation of the ITapeGatewaySvc for Oracle
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files

#include <serrno.h>
#include <string>
#include <list>

#include "errno.h"

#include "occi.h"

#include "castor/BaseAddress.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IObject.hpp"
#include "castor/IFactory.hpp"
#include "castor/SvcFactory.hpp"

#include "castor/exception/Internal.hpp"
#include "castor/exception/OutOfMemory.hpp"

#include "castor/tape/tapegateway/ClientType.hpp"
#include "castor/tape/tapegateway/PositionCommandCode.hpp"
#include "castor/tape/tapegateway/RetryPolicyElement.hpp"
#include "castor/tape/tapegateway/TapeGatewayDlfMessageConstants.hpp"
#include "castor/tape/tapegateway/VolumeMode.hpp"
#include "castor/db/ora/SmartOcciResultSet.hpp"

#include "castor/tape/tapegateway/daemon/NsTapeGatewayHelper.hpp"
#include "castor/tape/tapegateway/daemon/ora/OraTapeGatewaySvc.hpp"


//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::SvcFactory<castor::tape::tapegateway::ora::OraTapeGatewaySvc>* s_factoryOraTapeGatewaySvc =
  new castor::SvcFactory<castor::tape::tapegateway::ora::OraTapeGatewaySvc>();

#if __GNUC__ >= 2
#define USE(var) void use_##var (void) {var = var;}
USE (s_factoryOraTapeGatewaySvc);
#endif

//------------------------------------------------------------------------------
// OraTapeGatewaySvc
//------------------------------------------------------------------------------
castor::tape::tapegateway::ora::OraTapeGatewaySvc::OraTapeGatewaySvc(const std::string name) :
  ITapeGatewaySvc(),
  OraCommonSvc(name),
  m_getMigrationMountsWithoutTapesStatement(0),
  m_attachTapesToMigMountsStatement(0),
  m_getTapeWithoutDriveReqStatement(0),
  m_attachDriveReqStatement(0),
  m_getTapesWithDriveReqsStatement(0),
  m_restartLostReqsStatement(0),
  m_getFileToMigrateStatement(0),
  m_setFileMigratedStatement(0),
  m_setFileStaleInMigrationStatement(0),
  m_getFileToRecallStatement(0),
  m_setFileRecalledStatement(0),
  m_getFailedMigrationsStatement(0),
  m_setMigRetryResultStatement(0),
  m_getRepackVidAndFileInfoStatement(0),
  m_startTapeSessionStatement(0),
  m_endTapeSessionStatement(0),
  m_failFileTransferStatement(0),
  m_getTapeToReleaseStatement(0),
  m_cancelMigrationOrRecallStatement(0),
  m_deleteMigrationMountWithBadTapePoolStatement(0),
  m_flagTapeFullForMigrationSession(0),
  m_getMigrationMountVid(0),
  m_dropSuperfluousSegmentStatement(0)
{
}

//------------------------------------------------------------------------------
// ~OraTapeGatewaySvc
//------------------------------------------------------------------------------
castor::tape::tapegateway::ora::OraTapeGatewaySvc::~OraTapeGatewaySvc() throw() {
  reset();
}

//------------------------------------------------------------------------------
// id
//------------------------------------------------------------------------------
unsigned int castor::tape::tapegateway::ora::OraTapeGatewaySvc::id() const {
  return ID();
}

//------------------------------------------------------------------------------
// ID
//------------------------------------------------------------------------------
unsigned int castor::tape::tapegateway::ora::OraTapeGatewaySvc::ID() {
  return castor::SVC_ORATAPEGATEWAYSVC;
}

//-----------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  OraCommonSvc::reset();
  try {
    if ( m_getMigrationMountsWithoutTapesStatement ) deleteStatement(m_getMigrationMountsWithoutTapesStatement);
    if ( m_attachTapesToMigMountsStatement ) deleteStatement(m_attachTapesToMigMountsStatement);
    if ( m_getTapeWithoutDriveReqStatement ) deleteStatement(m_getTapeWithoutDriveReqStatement); 
    if ( m_attachDriveReqStatement ) deleteStatement(m_attachDriveReqStatement);
    if ( m_getTapesWithDriveReqsStatement ) deleteStatement(m_getTapesWithDriveReqsStatement);
    if ( m_restartLostReqsStatement ) deleteStatement( m_restartLostReqsStatement);
    if ( m_getFileToMigrateStatement ) deleteStatement( m_getFileToMigrateStatement);
    if ( m_setFileMigratedStatement ) deleteStatement( m_setFileMigratedStatement);
    if ( m_setFileStaleInMigrationStatement ) deleteStatement( m_setFileStaleInMigrationStatement);
    if ( m_getFileToRecallStatement ) deleteStatement( m_getFileToRecallStatement);
    if ( m_setFileRecalledStatement ) deleteStatement(m_setFileRecalledStatement);
    if ( m_getFailedMigrationsStatement ) deleteStatement(m_getFailedMigrationsStatement);
    if ( m_setMigRetryResultStatement ) deleteStatement(m_setMigRetryResultStatement);
    if ( m_getRepackVidAndFileInfoStatement ) deleteStatement( m_getRepackVidAndFileInfoStatement);
    if ( m_startTapeSessionStatement ) deleteStatement(m_startTapeSessionStatement);
    if ( m_endTapeSessionStatement ) deleteStatement(m_endTapeSessionStatement);
    if ( m_failFileTransferStatement ) deleteStatement(m_failFileTransferStatement);
    if ( m_getTapeToReleaseStatement ) deleteStatement(m_getTapeToReleaseStatement);
    if ( m_cancelMigrationOrRecallStatement ) deleteStatement(m_cancelMigrationOrRecallStatement);
    if ( m_deleteMigrationMountWithBadTapePoolStatement) deleteStatement(m_deleteMigrationMountWithBadTapePoolStatement);    
    if ( m_flagTapeFullForMigrationSession) deleteStatement(m_flagTapeFullForMigrationSession);
    if ( m_getMigrationMountVid) deleteStatement(m_getMigrationMountVid);
    if ( m_dropSuperfluousSegmentStatement) deleteStatement(m_dropSuperfluousSegmentStatement);
  } catch (castor::exception::Exception& ignored) {};
  // Now reset all pointers to 0
  m_getMigrationMountsWithoutTapesStatement= 0; 
  m_attachTapesToMigMountsStatement = 0;
  m_getTapeWithoutDriveReqStatement= 0;
  m_attachDriveReqStatement= 0;
  m_getTapesWithDriveReqsStatement = 0;
  m_restartLostReqsStatement = 0;
  m_getFileToMigrateStatement= 0;
  m_setFileMigratedStatement= 0;
  m_setFileStaleInMigrationStatement = 0;
  m_getFileToRecallStatement= 0;
  m_setFileRecalledStatement= 0;
  m_getFailedMigrationsStatement = 0;
  m_setMigRetryResultStatement= 0;
  m_getRepackVidAndFileInfoStatement = 0;
  m_startTapeSessionStatement = 0;
  m_endTapeSessionStatement = 0;
  m_failFileTransferStatement= 0;
  m_getTapeToReleaseStatement=0;
  m_cancelMigrationOrRecallStatement=0;
  m_deleteMigrationMountWithBadTapePoolStatement=0;
  m_flagTapeFullForMigrationSession=0;
  m_getMigrationMountVid=0;
  m_dropSuperfluousSegmentStatement=0;
}

//----------------------------------------------------------------------------
// getMigrationMountsWithoutTapes
//----------------------------------------------------------------------------

void castor::tape::tapegateway::ora::OraTapeGatewaySvc::getMigrationMountsWithoutTapes
(std::list<castor::tape::tapegateway::ITapeGatewaySvc::migrationMountParameters>& migrationMounts)
  throw (castor::exception::Exception){
  oracle::occi::ResultSet *rs = NULL;
  try {
    // Check whether the statements are ok
    if (!m_getMigrationMountsWithoutTapesStatement) {
      m_getMigrationMountsWithoutTapesStatement =
        createStatement("BEGIN tg_getMigMountsWithoutTapes(:1);END;");
      m_getMigrationMountsWithoutTapesStatement->registerOutParam
        (1, oracle::occi::OCCICURSOR);
    }
    // execute the statement and see whether we found something
    unsigned int nb = m_getMigrationMountsWithoutTapesStatement->executeUpdate();
    if (0 == nb) {
      cnvSvc()->commit(); 
      return;
    }
    rs = m_getMigrationMountsWithoutTapesStatement->getCursor(1);
    // Identify the columns of the cursor
    resultSetIntrospector resIntros (rs);
    int idIdx       = resIntros.findColumnIndex(                   "ID", oracle::occi::OCCI_SQLT_NUM);
    int TPNameIdx   = resIntros.findColumnIndex(                 "NAME", oracle::occi::OCCI_SQLT_CHR);
    int ReqIdIdx    = resIntros.findColumnIndex( "TAPEGATEWAYREQUESTID", oracle::occi::OCCI_SQLT_NUM);
    // Run through the cursor
    while( rs->next() == oracle::occi::ResultSet::DATA_AVAILABLE) {
      castor::tape::tapegateway::ITapeGatewaySvc::migrationMountParameters item;
      item.migrationMountId     = (u_signed64) (double) rs->getNumber(idIdx);
      item.tapegatewayRequestID = (u_signed64) (double) rs->getNumber(ReqIdIdx);
      // Note that we hardcode 1 for the initialSizeToTransfer. This parameter should actually
      // be dropped and the call to VMGR changed accordingly.
      item.initialSizeToTransfer = 1;
      item.tapePoolName = rs->getString(TPNameIdx);
      migrationMounts.push_back(item);
    }
    m_getMigrationMountsWithoutTapesStatement->closeResultSet(rs);
  } catch (oracle::occi::SQLException& e) {
    if (rs) m_getMigrationMountsWithoutTapesStatement->closeResultSet(rs);
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getMigrationMountsWithoutTapes"
      << std::endl << e.what();
    throw ex;
  } catch (std::exception& e) {
    if (rs) m_getMigrationMountsWithoutTapesStatement->closeResultSet(rs);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getMigrationMountsWithoutTapes"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// attachTapesToMigMounts
//----------------------------------------------------------------------------

void castor::tape::tapegateway::ora::OraTapeGatewaySvc::attachTapesToMigMounts(const std::list<u_signed64>& MMIds,const std::list<std::string>& vids, const std::list<int>& fseqs)
          throw (castor::exception::Exception){
  unsigned char (*bufferFseqs)[21]=NULL;
  ub2 *lensFseqs=NULL;
  unsigned char (*bufferMigrationMountIds)[21]=NULL;
  ub2 *lensMMIds=NULL;
  char * bufferVids=NULL;
  ub2 *lensVids=NULL;
  try {
    if ( !MMIds.size() || !vids.size() || !fseqs.size() ) {
      // just release the lock no result
      cnvSvc()->commit();
      return;
    }
    if (MMIds.size() != vids.size() || MMIds.size() != fseqs.size()) {
      // just release the lock no result
      cnvSvc()->commit();
      castor::exception::Exception e(EINVAL);
      throw e;
    }
    // Check whether the statements are ok
    if (0 == m_attachTapesToMigMountsStatement) {
      m_attachTapesToMigMountsStatement =
        createStatement("BEGIN tg_attachTapesToMigMounts(:1,:2,:3);END;");
      m_attachTapesToMigMountsStatement->setAutoCommit(true);
    }
    // input
    ub4 nb=MMIds.size();
    // fseq
    bufferFseqs=(unsigned char(*)[21]) calloc((nb) * 21, sizeof(unsigned char));
    lensFseqs=(ub2 *)malloc (sizeof(ub2)*nb);
    if ( lensFseqs  == 0 || bufferFseqs == 0 ) {
      if (lensFseqs != 0 ) free(lensFseqs);
      if (bufferFseqs != 0 ) free(bufferFseqs);
      castor::exception::OutOfMemory e; 
      throw e;
    }
    // MMIds
    bufferMigrationMountIds =(unsigned char(*)[21]) calloc((nb) * 21, sizeof(unsigned char));
    lensMMIds=(ub2 *)malloc (sizeof(ub2)*nb);
    if ( lensMMIds  == 0 || bufferMigrationMountIds == 0 ) {
      if (lensMMIds != 0 ) free(lensMMIds);
      if (bufferMigrationMountIds != 0) free(bufferMigrationMountIds);
      if (lensFseqs != 0 ) free(lensFseqs);
      if (bufferFseqs != 0 ) free(bufferFseqs);
      castor::exception::OutOfMemory e; 
      throw e;
    }
    // vids
    // get the maximum cell size
    unsigned int maxLen=0;
    for (std::list<std::string>::const_iterator vid = vids.begin();
         vid != vids.end();
         vid++){
      maxLen=maxLen > (*vid).length()?maxLen:(*vid).length();
    }
    if (maxLen == 0) {
      if (lensMMIds != 0 ) free(lensMMIds);
      if (bufferMigrationMountIds != 0) free(bufferMigrationMountIds);
      if (lensFseqs != 0 ) free(lensFseqs);
      if (bufferFseqs != 0 ) free(bufferFseqs);
      castor::exception::Internal ex;
      ex.getMessage() << "invalid VID in attachTapesToMigMounts"
                      << std::endl;
      throw ex;
    }
    unsigned int bufferCellSize = maxLen * sizeof(char);
    lensVids = (ub2*) malloc(nb * sizeof(ub2));
    bufferVids =
      (char*) malloc(nb * bufferCellSize);
    if ( lensVids  == 0 || bufferVids == 0 ) {
      if (lensMMIds != 0 ) free(lensMMIds);
      if (bufferMigrationMountIds != 0) free(bufferMigrationMountIds);
      if (lensVids != 0 ) free(lensVids);
      if (bufferVids != 0) free(bufferVids);
      if (lensFseqs != 0 ) free(lensFseqs);
      if (bufferFseqs != 0 ) free(bufferFseqs);
      castor::exception::OutOfMemory e; 
      throw e;
    }
    // DataBuffer with all the vid (one for each subrequest)
    // Fill in the structure
    std::list<std::string>::const_iterator vid;
    std::list<u_signed64>::const_iterator strId;
    std::list<int>::const_iterator  fseq;
    int i=0;
    for(vid = vids.begin(),
          strId = MMIds.begin(),
          fseq = fseqs.begin();
        strId != MMIds.end();
        vid++,strId++,fseq++,i++ ){
      // fseq
      oracle::occi::Number n = (double)(*fseq);
      oracle::occi::Bytes b = n.toBytes();
      b.getBytes(bufferFseqs[i],b.length());
      lensFseqs[i] = b.length();
      // Migration mount Ids
      n = (double)(*strId);
      b = n.toBytes();
      b.getBytes(bufferMigrationMountIds[i],b.length());
      lensMMIds[i] = b.length();
      // vids
      lensVids[i]= (*vid).length();
      strncpy(bufferVids+(bufferCellSize*i),(*vid).c_str(),lensVids[i]);
    }
    ub4 unused=nb;
    /* Attach buffer to inStartFseqs */
    m_attachTapesToMigMountsStatement->setDataBufferArray(1,bufferFseqs, oracle::occi::OCCI_SQLT_NUM, nb, &unused, 21, lensFseqs);
    /* Attach array to inMountIds */
    m_attachTapesToMigMountsStatement->setDataBufferArray(2,bufferMigrationMountIds, oracle::occi::OCCI_SQLT_NUM, nb, &unused, 21, lensMMIds);
    ub4 len=nb;
    /* Attach array to inTapeVids */
    m_attachTapesToMigMountsStatement->setDataBufferArray(3, bufferVids, oracle::occi::OCCI_SQLT_CHR,len, &len, maxLen, lensVids);
    // execute the statement and see whether we found something
    m_attachTapesToMigMountsStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    if (lensMMIds != 0 ) free(lensMMIds);
    if (bufferMigrationMountIds != 0) free(bufferMigrationMountIds);
    if (lensVids != 0 ) free(lensVids);
    if (bufferVids != 0) free(bufferVids);
    if (lensFseqs != 0 ) free(lensFseqs);
    if (bufferFseqs != 0 ) free(bufferFseqs);
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in attachTapesToMigMounts"
      << std::endl << e.what();
    throw ex;
  }
  if (lensMMIds != 0 ) free(lensMMIds);
  if (bufferMigrationMountIds != 0) free(bufferMigrationMountIds);
  if (lensVids != 0 ) free(lensVids);
  if (bufferVids != 0) free(bufferVids);
  if (lensFseqs != 0 ) free(lensFseqs);
  if (bufferFseqs != 0 ) free(bufferFseqs);
}


//----------------------------------------------------------------------------
// getTapeWithoutDriveReq 
//----------------------------------------------------------------------------

void castor::tape::tapegateway::ora::OraTapeGatewaySvc::getTapeWithoutDriveReq
(std::vector<std::string> &vidsForMigr,
 std::vector<std::pair<std::string, int> > &tapesForRecall)
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_getTapeWithoutDriveReqStatement) {
      m_getTapeWithoutDriveReqStatement =
        createStatement("BEGIN tg_getTapeWithoutDriveReq(:1,:2);END;");
      m_getTapeWithoutDriveReqStatement->registerOutParam
        (1, oracle::occi::OCCICURSOR);
      m_getTapeWithoutDriveReqStatement->registerOutParam
        (2, oracle::occi::OCCICURSOR);
    }
    // execute the statement
    m_getTapeWithoutDriveReqStatement->executeUpdate();
    // deal with migrations
    oracle::occi::ResultSet *rs = m_getTapeWithoutDriveReqStatement->getCursor(1);
    while(rs->next()  == oracle::occi::ResultSet::DATA_AVAILABLE) {
      std::string vid(rs->getString(1));
      vidsForMigr.push_back(vid);
    }
    m_getTapeWithoutDriveReqStatement->closeResultSet(rs);
    // deal with recalls
    rs = m_getTapeWithoutDriveReqStatement->getCursor(2);
    while(rs->next()  == oracle::occi::ResultSet::DATA_AVAILABLE) {
      std::string vid(rs->getString(1));
      int vdqmPriority = rs->getInt(2);
      tapesForRecall.push_back(std::pair<std::string, int>(vid, vdqmPriority));
    }
    m_getTapeWithoutDriveReqStatement->closeResultSet(rs);
  } catch (oracle::occi::SQLException e) {    
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage() << "Error caught in getTapeWithoutDriveReq"
                    << std::endl << e.what();
    throw ex;
  }
}
 
//----------------------------------------------------------------------------
// attachDriveReq
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::attachDriveReq
(const std::string &vid, const u_signed64 mountTransactionId, const int mode,
 const char *label, const char *density)
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_attachDriveReqStatement) {
      m_attachDriveReqStatement =
        createStatement("BEGIN tg_attachDriveReq(:1,:2,:3,:4,:5);END;");
    }
    // execute the statement
    m_attachDriveReqStatement->setString(1, vid);
    m_attachDriveReqStatement->setDouble(2, (double)mountTransactionId);
    m_attachDriveReqStatement->setInt(3, mode);
    m_attachDriveReqStatement->setString(4, label);
    m_attachDriveReqStatement->setString(5, density);
    m_attachDriveReqStatement->executeUpdate(); 
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in attachDriveReq"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// getTapesWithDriveReqs
//----------------------------------------------------------------------------

void  castor::tape::tapegateway::ora::OraTapeGatewaySvc::getTapesWithDriveReqs
(std::list<TapeRequest>& requests,
 const u_signed64& timeOut) 
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_getTapesWithDriveReqsStatement) {
      m_getTapesWithDriveReqsStatement =
        createStatement("BEGIN tg_getTapesWithDriveReqs(:1,:2);END;");
      // This cursor has 3 columns : mode, mountTransactionId, VID
      m_getTapesWithDriveReqsStatement->registerOutParam(2, oracle::occi::OCCICURSOR);
    }
    // execute the statement and see whether we found something
    m_getTapesWithDriveReqsStatement->setDouble(1,(double)timeOut);
    unsigned int nb = m_getTapesWithDriveReqsStatement->executeUpdate();
    if (0 == nb) {
      // just release the lock no result
      cnvSvc()->commit();
      return;
    }
    // Run through the results
    oracle::occi::ResultSet *rs = m_getTapesWithDriveReqsStatement->getCursor(2);
    while(rs->next()  == oracle::occi::ResultSet::DATA_AVAILABLE) {
      TapeRequest tr;
      tr.mode = rs->getInt(1);
      tr.mountTransactionId = (u_signed64)rs->getDouble(2);
      tr.vid = rs->getString(3);
      requests.push_back(tr);
    }
    m_getTapesWithDriveReqsStatement->closeResultSet(rs);
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getTapesWithDriveReqs"
      << std::endl << e.what();
    throw ex;
  }

}

//----------------------------------------------------------------------------
// restartLostReqs 
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::restartLostReqs
(const std::list<int>& mountTransactionIds)
  throw (castor::exception::Exception){
  unsigned char (*bufferMountTransactionIds)[21]=NULL;
  ub2 *lensMountTransactionIds=NULL;
  try {
    if (!mountTransactionIds.size()) {
      cnvSvc()->commit();
      return;
    }
    // Check whether the statements are ok
    if (0 == m_restartLostReqsStatement) {
      m_restartLostReqsStatement = createStatement("BEGIN tg_restartLostReqs(:1);END;");
      m_restartLostReqsStatement->setAutoCommit(true);
    }
    // input
    ub4 nb = mountTransactionIds.size();
    bufferMountTransactionIds=(unsigned char(*)[21]) calloc((nb) * 21, sizeof(unsigned char));
    lensMountTransactionIds=(ub2 *)malloc (sizeof(ub2)*nb);
    if ( lensMountTransactionIds == 0 || bufferMountTransactionIds == 0 ) {
      if (lensMountTransactionIds != 0 ) free(lensMountTransactionIds);
      if (bufferMountTransactionIds != 0) free(bufferMountTransactionIds);
      castor::exception::OutOfMemory e; 
      throw e;
     }
    // Fill in the structure
    int i=0;
    for (std::list<int>::const_iterator mountTransactionId = mountTransactionIds.begin();
	 mountTransactionId != mountTransactionIds.end();
	 mountTransactionId++,i++){
      oracle::occi::Number n = (double)(*mountTransactionId);
      oracle::occi::Bytes b = n.toBytes();
      b.getBytes(bufferMountTransactionIds[i],b.length());
      lensMountTransactionIds[i] = b.length();
    }
    ub4 unused=nb;
    m_restartLostReqsStatement->setDataBufferArray
      (1,bufferMountTransactionIds, oracle::occi::OCCI_SQLT_NUM, nb, &unused, 21, lensMountTransactionIds);
    // execute the statement
    m_restartLostReqsStatement->executeUpdate();
    // fre memory
    if (lensMountTransactionIds != 0 ) free(lensMountTransactionIds);
    if (bufferMountTransactionIds != 0) free(bufferMountTransactionIds);
  } catch (oracle::occi::SQLException e) {
    // free momery
    if (lensMountTransactionIds != 0 ) free(lensMountTransactionIds);
    if (bufferMountTransactionIds != 0) free(bufferMountTransactionIds);
    // handle exception
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in restartLostReqs "
      << std::endl << e.what();
    throw ex;
  }
}


//----------------------------------------------------------------------------
// getFileToMigrate  
//----------------------------------------------------------------------------

void castor::tape::tapegateway::ora::OraTapeGatewaySvc::getFileToMigrate(const castor::tape::tapegateway::FileToMigrateRequest& req,castor::tape::tapegateway::FileToMigrate& file) throw (castor::exception::Exception){

  try {

    while (1) { // until we get a valid file
     
      std::string diskserver;
      std::string mountpoint;
      file.setMountTransactionId(0);
      // Check whether the statements are ok
      if (0 == m_getFileToMigrateStatement) {
        m_getFileToMigrateStatement =
          createStatement("BEGIN tg_getFileToMigrate(:1,:2,:3,:4);END;");
        m_getFileToMigrateStatement->registerOutParam
          (2, oracle::occi::OCCIINT);
        m_getFileToMigrateStatement->registerOutParam
          (3, oracle::occi::OCCISTRING, 2048 );
        m_getFileToMigrateStatement->registerOutParam
          (4, oracle::occi::OCCICURSOR);
      }

      m_getFileToMigrateStatement->setDouble(1,(double)req.mountTransactionId());
      m_getFileToMigrateStatement->executeUpdate();

      int ret = m_getFileToMigrateStatement->getInt(2);
      if (ret == -1 ) {
        cnvSvc()->commit();
        return;
      }

      if (ret == -2 ) {// UNKNOWN request
        cnvSvc()->commit();
        castor::exception::Exception e(EINVAL);
        throw e;
      }


      if (ret == -3 ) {// no diskserver available
        cnvSvc()->commit();
        castor::exception::Internal ex;
        ex.getMessage()
          << "no diskserver available"
          << std::endl; 
        
        throw ex;
        
      }

      std::string vid=m_getFileToMigrateStatement->getString(3); 

      oracle::occi::ResultSet *rs =
        m_getFileToMigrateStatement->getCursor(4);

      // Run through the cursor 
      
      oracle::occi::ResultSet::Status status = rs->next();
      
      // one at the moment

      if  (status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      
        file.setFileid((u_signed64)rs->getDouble(1));
        file.setNshost(rs->getString(2));
        file.setLastModificationTime((u_signed64)rs->getDouble(3));
        diskserver=rs->getString(4);
        mountpoint=rs->getString(5);
        file.setPath(diskserver.append(":").append(mountpoint).append(rs->getString(6))); 
        file.setLastKnownFilename(rs->getString(7));
        file.setFseq(rs->getInt(8));
        file.setFileSize((u_signed64)rs->getDouble(9));
        file.setFileTransactionId((u_signed64)rs->getDouble(10));
        file.setMountTransactionId(req.mountTransactionId());
        file.setPositionCommandCode(TPPOSIT_FSEQ);
      }
        
      m_getFileToMigrateStatement->closeResultSet(rs);
      break; // end the loop

    }      
    // here we have a valid candidate or NULL
    cnvSvc()->commit();

  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getFileToMigrate"
      << std::endl << e.what();
    throw ex;
  }
  // hardcoded umask
  file.setUmask(022);  
 
}

//----------------------------------------------------------------------------
// setFileMigrated  
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::setFileMigrated
(const castor::tape::tapegateway::FileMigratedNotification& resp)
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_setFileMigratedStatement) {
      m_setFileMigratedStatement =
        createStatement("BEGIN tg_setFileMigrated(:1,:2,:3,:4);END;");
    }
    m_setFileMigratedStatement->setDouble(1,(double)resp.fileid());
    m_setFileMigratedStatement->setString(2,resp.nshost());
    m_setFileMigratedStatement->setInt(3,resp.fseq());
    m_setFileMigratedStatement->setDouble(4,(double)resp.fileTransactionId());
    m_setFileMigratedStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in setFileMigrated"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// dropSuperfluousSegment
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::dropSuperfluousSegment
(const castor::tape::tapegateway::FileMigratedNotification& resp)
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_dropSuperfluousSegmentStatement) {
      m_dropSuperfluousSegmentStatement =
        createStatement("BEGIN tg_dropSuperfluousSegment(:2,:3,:4,:5);END;");
    }
    m_dropSuperfluousSegmentStatement->setDouble(1,(double)resp.fileid());
    m_dropSuperfluousSegmentStatement->setString(2,resp.nshost());
    m_dropSuperfluousSegmentStatement->setInt(3,resp.fseq());
    m_dropSuperfluousSegmentStatement->setDouble(4,(double)resp.fileTransactionId());
    m_dropSuperfluousSegmentStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in dropSuperfluousSegment"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// setFileStaleInMigration
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::setFileStaleInMigration
(const castor::tape::tapegateway::FileMigratedNotification& resp)
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_setFileStaleInMigrationStatement) {
      m_setFileStaleInMigrationStatement =
        createStatement("BEGIN tg_setFileStaleInMigration(:1,:2,:3,:4,:5);END;");
    }
    m_setFileStaleInMigrationStatement->setDouble(1,(double)resp.mountTransactionId());
    m_setFileStaleInMigrationStatement->setDouble(2,(double)resp.fileid());
    m_setFileStaleInMigrationStatement->setString(3,resp.nshost());
    m_setFileStaleInMigrationStatement->setInt(4,resp.fseq());
    m_setFileStaleInMigrationStatement->setDouble(5,(double)resp.fileTransactionId());
    m_setFileStaleInMigrationStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in setFileStaleInMigration"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// getFileToRecall 
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::getFileToRecall
 (const castor::tape::tapegateway::FileToRecallRequest& req,
  castor::tape::tapegateway::FileToRecall& file)
  throw (castor::exception::Exception) {
  try {
    while (1) {
      std::string diskserver;
      std::string mountpoint;
      file.setMountTransactionId(0);
      // Check whether the statements are ok
      if (0 == m_getFileToRecallStatement) {
        m_getFileToRecallStatement =
          createStatement("BEGIN tg_getFileToRecall(:1,:2,:3,:4);END;");
        m_getFileToRecallStatement->registerOutParam
          (2, oracle::occi::OCCIINT);
        m_getFileToRecallStatement->registerOutParam
          (3, oracle::occi::OCCISTRING,  2048);
        m_getFileToRecallStatement->registerOutParam
          (4, oracle::occi::OCCICURSOR);
      }
      m_getFileToRecallStatement->setDouble(1,(double)req.mountTransactionId());

      // execute the statement and see whether we found something
      m_getFileToRecallStatement->executeUpdate();
      int ret = m_getFileToRecallStatement->getInt(2);
      if (ret == -1 ) return;
      if (ret == -2 ) {// UNKNOWN request
        castor::exception::Exception e(ENOENT);
        throw e;
      }
      if (ret == -3 ) {// no diskserver available
        castor::exception::Internal ex;
        ex.getMessage()
          << "no diskserver available"
          << std::endl; 
        throw ex;
      }

      // Extract the result
      std::string vid= m_getFileToRecallStatement->getString(3);
      oracle::occi::ResultSet *rs =
        m_getFileToRecallStatement->getCursor(4);
      // one at the moment (the cusror is expected to return one element only
      // XXX This is a trivial place to insert bulk transactions.
      if  (rs->next() == oracle::occi::ResultSet::DATA_AVAILABLE) {
        file.setFileid((u_signed64)rs->getDouble(1));
        file.setNshost(rs->getString(2));
        diskserver=rs->getString(3);
        mountpoint=rs->getString(4);
        file.setPath(diskserver.append(":").append(mountpoint).append(rs->getString(5)));
        file.setFseq(rs->getInt(6));
        file.setFileTransactionId((u_signed64)rs->getDouble(7));
        file.setMountTransactionId(req.mountTransactionId());
	file.setPositionCommandCode(TPPOSIT_BLKID);
        oracle::occi::Bytes blockIdBytes = rs->getBytes(8);
        file.setBlockId0(blockIdBytes.byteAt(0));
        file.setBlockId1(blockIdBytes.byteAt(1));
        file.setBlockId2(blockIdBytes.byteAt(2));
        file.setBlockId3(blockIdBytes.byteAt(3));
        if ((file.blockId0() == '\0') && (file.blockId1() == '\0') &&
            (file.blockId2() == '\0') && (file.blockId3() == '\0')) {
          file.setPositionCommandCode(TPPOSIT_FSEQ); // magic things for the first file
        }
	// here we have a valid candidate
      }
      m_getFileToRecallStatement->closeResultSet(rs);
      break;
    }
    cnvSvc()->commit();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getFileToRecall"
      << std::endl << e.what();
    throw ex;
  }
  // hardcoded umask
  file.setUmask(077);  
}
 
//----------------------------------------------------------------------------
// setFileRecalled  
//----------------------------------------------------------------------------
void  castor::tape::tapegateway::ora::OraTapeGatewaySvc::setFileRecalled
(const castor::tape::tapegateway::FileRecalledNotification& fileRecalled)
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_setFileRecalledStatement) {
      m_setFileRecalledStatement = createStatement("BEGIN tg_setFileRecalled(:1,:2,:3,:4,:5);END;");
      m_setFileRecalledStatement->setAutoCommit(true);
    }
    // Call procedure
    m_setFileRecalledStatement->setDouble(1,(double)fileRecalled.mountTransactionId());
    m_setFileRecalledStatement->setInt(2,fileRecalled.fseq());
    m_setFileRecalledStatement->setString(3,fileRecalled.path());
    m_setFileRecalledStatement->setString(4,fileRecalled.checksumName());
    m_setFileRecalledStatement->setDouble(5,(double)fileRecalled.checksum());
    m_setFileRecalledStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage() << "Error caught in setFileRecalled" << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// getFailedMigrations
//----------------------------------------------------------------------------

void  castor::tape::tapegateway::ora::OraTapeGatewaySvc::getFailedMigrations(
    std::list<castor::tape::tapegateway::RetryPolicyElement>& candidates)
          throw (castor::exception::Exception)
{
  oracle::occi::ResultSet *rs = NULL;
  try {
    // Check whether the statements are ok
    if (!m_getFailedMigrationsStatement) {
      m_getFailedMigrationsStatement =
        createStatement("BEGIN tg_getFailedMigrations(:1);END;");
      m_getFailedMigrationsStatement->registerOutParam
        (1, oracle::occi::OCCICURSOR);
    }
    // execute the statement and see whether we found something
    unsigned int nb = m_getFailedMigrationsStatement->executeUpdate();
    if (0 == nb) {
      cnvSvc()->commit(); 
      return;
    }
    rs = m_getFailedMigrationsStatement->getCursor(1);
    // Find columns in the cursor
    resultSetIntrospector resIntros (rs);
    int MigrationJobIdIndex = resIntros.findColumnIndex(       "ID", oracle::occi::OCCI_SQLT_NUM);
    int ErrorCodeIndex  =     resIntros.findColumnIndex("ERRORCODE", oracle::occi::OCCI_SQLT_NUM);
    int NbRetryIndex    =     resIntros.findColumnIndex(  "NBRETRY", oracle::occi::OCCI_SQLT_NUM);
    int NsHostIndex     =     resIntros.findColumnIndex(   "NSHOST", oracle::occi::OCCI_SQLT_CHR);
    int FileIdIndex     =     resIntros.findColumnIndex(   "FILEID", oracle::occi::OCCI_SQLT_NUM);
    // Run through the cursor
    while (rs->next() == oracle::occi::ResultSet::DATA_AVAILABLE) {
      castor::tape::tapegateway::RetryPolicyElement item;
      item.migrationOrRecallJobId = (u_signed64) (double) occiNumber(rs->getNumber(MigrationJobIdIndex));
      item.errorCode              =                 (int) occiNumber(rs->getNumber(ErrorCodeIndex));
      item.nbRetry                =                 (int) occiNumber(rs->getNumber(NbRetryIndex));
      item.nsHost                 =                                  rs->getString(NsHostIndex);
      item.fileId                 = (u_signed64) (double) occiNumber(rs->getNumber(FileIdIndex));
      item.tape                   = "";
      item.fSeq                   = 0;
      candidates.push_back(item);
    }
    m_getFailedMigrationsStatement->closeResultSet(rs);
  } catch (oracle::occi::SQLException& e) {
    if (rs) m_getFailedMigrationsStatement->closeResultSet(rs);
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getFailedMigrations"
      << std::endl << e.what();
    throw ex;
  } catch (std::exception &e) { // This case is almost identical to the previous one, but does not call handleException
    if (rs) m_getFailedMigrationsStatement->closeResultSet(rs);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getFailedMigrations"
      << std::endl << e.what();
    throw ex;
  }
}


//----------------------------------------------------------------------------
// setMigRetryResult
//----------------------------------------------------------------------------

void  castor::tape::tapegateway::ora::OraTapeGatewaySvc::setMigRetryResult(const std::list<u_signed64>& mjToRetry, const std::list<u_signed64>&  mjToFail ) throw (castor::exception::Exception) {

  
 unsigned char (*bufferRetry)[21]=NULL;
 ub2 *lensRetry=NULL;
 unsigned char (*bufferFail)[21]=NULL;
 ub2 *lensFail = NULL;

  try { 
    // Check whether the statements are ok
    if (0 == m_setMigRetryResultStatement) {
      m_setMigRetryResultStatement =
        createStatement("BEGIN tg_setMigRetryResult(:1,:2);END;");
    }

    // success

    ub4 nbRetry= mjToRetry.size();
    nbRetry=nbRetry==0?1:nbRetry;
    bufferRetry=(unsigned char(*)[21]) calloc((nbRetry) * 21, sizeof(unsigned char));
    lensRetry=(ub2 *)malloc (sizeof(ub2)*nbRetry);

    if ( lensRetry == 0 || bufferRetry == 0 ) {
      if (lensRetry != 0 ) free(lensRetry);
      if (bufferRetry != 0) free(bufferRetry);
      castor::exception::OutOfMemory e; 
      throw e;
    }

    int i=0;
    
    for (std::list<u_signed64>::const_iterator elem= mjToRetry.begin(); 
         elem != mjToRetry.end();
         elem++, i++){
        oracle::occi::Number n = (double)(*elem);
        oracle::occi::Bytes b = n.toBytes();
        b.getBytes(bufferRetry[i],b.length());
        lensRetry[i] = b.length();
    }

    // if there where no successfull migration
    if (mjToRetry.size() == 0){
      //let's put -1
      oracle::occi::Number n = (double)(-1);
      oracle::occi::Bytes b = n.toBytes();
      b.getBytes(bufferRetry[0],b.length());
      lensRetry[0] = b.length();
    }

    ub4 unusedRetry =nbRetry;
    m_setMigRetryResultStatement->setDataBufferArray(1,bufferRetry, oracle::occi::OCCI_SQLT_NUM, nbRetry, &unusedRetry, 21, lensRetry);

    // failures

    ub4 nbFail = mjToFail.size();
    nbFail = nbFail == 0 ? 1 : nbFail; 
    bufferFail=(unsigned char(*)[21]) calloc((nbFail) * 21, sizeof(unsigned char));
    lensFail = (ub2 *)malloc (sizeof(ub2)*nbFail);

    if ( lensFail == 0 || bufferFail == 0 ) {
      if (lensFail != 0 ) free(lensFail);
      if (bufferFail != 0) free(bufferFail);
      if (lensRetry != 0 ) free(lensRetry);
      if (bufferRetry != 0) free(bufferRetry);
      castor::exception::OutOfMemory e; 
      throw e;
    }
    
    i=0;

    for (std::list<u_signed64>::const_iterator elem=mjToFail.begin();
          elem != mjToFail.end();
          elem++,i++){
      
        oracle::occi::Number n = (double)(*elem);
        oracle::occi::Bytes b = n.toBytes();
        b.getBytes(bufferFail[i],b.length());
        lensFail[i] = b.length();

    }

    // if there where no failed migration

    if (mjToFail.size() == 0){
      //let's put -1
      oracle::occi::Number n = (double)(-1);
      oracle::occi::Bytes b = n.toBytes();
      b.getBytes(bufferFail[0],b.length());
      lensFail[0] = b.length();
    }

    ub4 unusedFail = nbFail;
    m_setMigRetryResultStatement->setDataBufferArray(2,bufferFail, oracle::occi::OCCI_SQLT_NUM, nbFail, &unusedFail, 21, lensFail);

    m_setMigRetryResultStatement->executeUpdate();

  } catch (oracle::occi::SQLException e) {

    if (lensFail != 0 ) free(lensFail);
    if (bufferFail != 0) free(bufferFail);
    if (lensRetry != 0 ) free(lensRetry);
    if (bufferRetry != 0) free(bufferRetry);

    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in setMigRetryResult"
      << std::endl << e.what();
    throw ex;
  }

  if (lensFail != 0 ) free(lensFail);
  if (bufferFail != 0) free(bufferFail);
  if (lensRetry != 0 ) free(lensRetry);
  if (bufferRetry != 0) free(bufferRetry);

}

//----------------------------------------------------------------------------
// getRepackVidAndFileInfo
//----------------------------------------------------------------------------
  
void  castor::tape::tapegateway::ora::OraTapeGatewaySvc::getMigratedFileInfo
(const castor::tape::tapegateway::FileMigratedNotification& resp,
 std::string& vid, int& copyNumber, u_signed64& lastModificationTime,
 std::string& originalVid, int& originalCopyNumber, std::string& fileClass)
  throw (castor::exception::Exception){
  
  try {
    // Check whether the statements are ok

    if (0 == m_getRepackVidAndFileInfoStatement) {
      m_getRepackVidAndFileInfoStatement =
        createStatement("BEGIN tg_getRepackVidAndFileInfo(:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12);END;");
      m_getRepackVidAndFileInfoStatement->registerOutParam
        (6, oracle::occi::OCCISTRING, 2048 );
      m_getRepackVidAndFileInfoStatement->registerOutParam
        (7, oracle::occi::OCCIINT);
      m_getRepackVidAndFileInfoStatement->registerOutParam
        (8, oracle::occi::OCCISTRING, 2048 );
      m_getRepackVidAndFileInfoStatement->registerOutParam
        (9, oracle::occi::OCCIINT);
      m_getRepackVidAndFileInfoStatement->registerOutParam
        (10, oracle::occi::OCCIDOUBLE);
      m_getRepackVidAndFileInfoStatement->registerOutParam
        (11, oracle::occi::OCCISTRING, 2048 );
      m_getRepackVidAndFileInfoStatement->registerOutParam
        (12, oracle::occi::OCCIINT);
    }

    m_getRepackVidAndFileInfoStatement->setDouble(1,(double)resp.fileid());
    m_getRepackVidAndFileInfoStatement->setString(2,resp.nshost());
    m_getRepackVidAndFileInfoStatement->setInt(3,resp.fseq());
    m_getRepackVidAndFileInfoStatement->setDouble(4,resp.mountTransactionId());
    m_getRepackVidAndFileInfoStatement->setDouble(5,resp.fileSize());
    
    // execute the statement 

    m_getRepackVidAndFileInfoStatement->executeUpdate();
    
    int ret=m_getRepackVidAndFileInfoStatement->getInt(12);
    if (ret==-1){
      //wrong file size
      castor::exception::Exception ex(ERTWRONGSIZE);
      ex.getMessage()<<"wrong file size given: "<<resp.fileSize();
      throw ex;
    }
      
    originalVid = m_getRepackVidAndFileInfoStatement->getString(6);
    originalCopyNumber = m_getRepackVidAndFileInfoStatement->getInt(7);
    vid = m_getRepackVidAndFileInfoStatement->getString(8);
    copyNumber = m_getRepackVidAndFileInfoStatement->getInt(9);
    lastModificationTime= (u_signed64)m_getRepackVidAndFileInfoStatement->getDouble(10);
    fileClass = m_getRepackVidAndFileInfoStatement->getString(11);

  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getRepackVid"
      << std::endl << e.what();
    throw ex;
  }
  
}

//--------------------------------------------------------------------------
// startTapeSession
//--------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::startTapeSession(const castor::tape::tapegateway::VolumeRequest& startRequest, castor::tape::tapegateway::Volume& volume ) throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_startTapeSessionStatement) {
      m_startTapeSessionStatement =
        createStatement("BEGIN tg_startTapeSession(:1,:2,:3,:4,:5,:6);END;");
      m_startTapeSessionStatement->registerOutParam
        (2, oracle::occi::OCCISTRING, 2048 );
      m_startTapeSessionStatement->registerOutParam
        (3, oracle::occi::OCCIINT);
      m_startTapeSessionStatement->registerOutParam
        (4, oracle::occi::OCCIINT);
      m_startTapeSessionStatement->registerOutParam
        (5, oracle::occi::OCCISTRING, 2048 );
      m_startTapeSessionStatement->registerOutParam
        (6, oracle::occi::OCCISTRING, 2048 );
      m_startTapeSessionStatement->setAutoCommit(true);
    }
    m_startTapeSessionStatement->setDouble(1,(double)startRequest.mountTransactionId());
    // execute the statement
    m_startTapeSessionStatement->executeUpdate();
    int ret = m_startTapeSessionStatement->getInt(4);
    if (ret == -1) {
      // No more files
      return;
    }
    if (ret == -2) {
      // Unknown request
      castor::exception::Exception e(EINVAL);
      throw e;
    }
    volume.setClientType(TAPE_GATEWAY);
    volume.setVid(m_startTapeSessionStatement->getString(2));
    volume.setMode((castor::tape::tapegateway::VolumeMode)m_startTapeSessionStatement->getInt(3));
    volume.setDensity(m_startTapeSessionStatement->getString(5));
    volume.setLabel(m_startTapeSessionStatement->getString(6));
    volume.setMountTransactionId(startRequest.mountTransactionId());
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in startTapeSession"
      << std::endl << e.what();
    throw ex;
  }
} 

//----------------------------------------------------------------------------
// endTapeSession
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::endTapeSession
(const u_signed64 mountTransactionId, const int errorCode)
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_endTapeSessionStatement) {
      m_endTapeSessionStatement = createStatement("BEGIN tg_endTapeSession(:1,:2);END;");
      m_endTapeSessionStatement->setAutoCommit(true);
    }
    // run statement
    m_endTapeSessionStatement->setDouble(1, (double)mountTransactionId); 
    m_endTapeSessionStatement->setInt(2, errorCode);
    m_endTapeSessionStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage() << "Error caught in failTapeSession" << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// failFileTransfer
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::failFileTransfer
(const u_signed64 mountTransactionId, const u_signed64 fileId,
 const std::string &nsHost, const int errorCode)
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_failFileTransferStatement) {
      m_failFileTransferStatement =
        createStatement("BEGIN tg_failFileTransfer(:1,:2,:3,:4);END;");
      m_failFileTransferStatement->setAutoCommit(true);
    }
    m_failFileTransferStatement->setDouble(1,(double)mountTransactionId); 
    m_failFileTransferStatement->setDouble(2,(double)fileId);
    m_failFileTransferStatement->setString(3,nsHost);
    m_failFileTransferStatement->setInt(4,errorCode);
    m_failFileTransferStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in failFileTransfer"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// getTapeToRelease
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::getTapeToRelease
(const u_signed64& mountTransactionId, 
 castor::tape::tapegateway::ITapeGatewaySvc::TapeToReleaseInfo& tape)
  throw (castor::exception::Exception){  
  try {
    // Check whether the statements are ok
    if (0 == m_getTapeToReleaseStatement) {
      m_getTapeToReleaseStatement =
        createStatement("BEGIN tg_getTapeToRelease(:1,:2,:3,:4);END;");
      m_getTapeToReleaseStatement->registerOutParam(2, oracle::occi::OCCISTRING, 2048 ); 
      m_getTapeToReleaseStatement->registerOutParam(3, oracle::occi::OCCIINT);
      m_getTapeToReleaseStatement->registerOutParam(4, oracle::occi::OCCIINT);
    }
    m_getTapeToReleaseStatement->setDouble(1,(double)mountTransactionId); 
    m_getTapeToReleaseStatement->executeUpdate();
    tape.vid = m_getTapeToReleaseStatement->getString(2);
    tape.mode = m_getTapeToReleaseStatement->getInt(3);
    tape.full = m_getTapeToReleaseStatement->getInt(4);
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getTapeToRelease"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// cancelMigrationOrRecall
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::cancelMigrationOrRecall
(const int mode,
 const std::string &vid,
 const int errorCode,
 const std::string &errorMsg)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_cancelMigrationOrRecallStatement) {
      m_cancelMigrationOrRecallStatement =
        createStatement("BEGIN cancelMigrationOrRecall(:1,:2,:3,:4);END;");
      m_cancelMigrationOrRecallStatement->setAutoCommit(true);
    }
    m_cancelMigrationOrRecallStatement->setInt(1, mode);
    m_cancelMigrationOrRecallStatement->setString(2, vid);
    m_cancelMigrationOrRecallStatement->setInt(3, errorCode);
    m_cancelMigrationOrRecallStatement->setString(4, errorMsg);
    // execute the statement and see whether we found something
    m_cancelMigrationOrRecallStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in deleteMigrationMountWithBadTapePool"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// deleteMigrationMountWithBadTapePool
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::deleteMigrationMountWithBadTapePool(const u_signed64 migrationMountId) 
  throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok
    if (0 == m_deleteMigrationMountWithBadTapePoolStatement) {
      m_deleteMigrationMountWithBadTapePoolStatement =
        createStatement("BEGIN tg_deleteMigrationMount(:1);END;");
    }
    m_deleteMigrationMountWithBadTapePoolStatement->setDouble(1,(double)migrationMountId);
    // execute the statement and see whether we found something
    m_deleteMigrationMountWithBadTapePoolStatement->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in deleteMigrationMountWithBadTapePool"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// flagTapeFullForMigrationSession
//----------------------------------------------------------------------------

void castor::tape::tapegateway::ora::OraTapeGatewaySvc::flagTapeFullForMigrationSession(const u_signed64& tapeRequestId)throw (castor::exception::Exception){
  try {
    // Check whether the statements are ok

    /* The name of the SQL procedure is shorter, but not really ambiguous:
       only migration session can lead to a full tape (obviously) */
    if (!m_flagTapeFullForMigrationSession)
      m_flagTapeFullForMigrationSession =
        createStatement("BEGIN tg_flagTapeFull(:1);END;");

    m_flagTapeFullForMigrationSession->setDouble(1,(double)tapeRequestId);
    // execute the statement
    m_flagTapeFullForMigrationSession->executeUpdate();
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in flagTapeFullForMigrationSession"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// getMigrationMountVid
//----------------------------------------------------------------------------
void castor::tape::tapegateway::ora::OraTapeGatewaySvc::getMigrationMountVid(FileMigratedNotification & fileMigrated,
    std::string &vid, std::string &tapePool)
{
  try {
    if (!m_getMigrationMountVid) {
      m_getMigrationMountVid =
          createStatement("BEGIN tg_getMigrationMountVid(:1,:2,:3);END;");
      m_getMigrationMountVid->registerOutParam
      (2, oracle::occi::OCCISTRING, 2048 );
      m_getMigrationMountVid->registerOutParam
      (3, oracle::occi::OCCISTRING, 2048 );
    }
    m_getMigrationMountVid->setDouble(1,(double)fileMigrated.mountTransactionId());
    m_getMigrationMountVid->executeUpdate();
    vid      = m_getMigrationMountVid->getString(2);
    tapePool = m_getMigrationMountVid->getString(3);
  } catch (oracle::occi::SQLException e) {
    handleException(e);
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in getMigrationMountVid"
      << std::endl << e.what();
    throw ex;
  }
}

//----------------------------------------------------------------------------
// commit
//----------------------------------------------------------------------------

void castor::tape::tapegateway::ora::OraTapeGatewaySvc::commit()
  throw (castor::exception::Exception)
{
  DbBaseObj::commit();
}

//----------------------------------------------------------------------------
// rollback
//----------------------------------------------------------------------------

void castor::tape::tapegateway::ora::OraTapeGatewaySvc::rollback()
  throw (castor::exception::Exception)
{
  DbBaseObj::rollback();
}


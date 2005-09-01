/******************************************************************************
 *                      OraVdqmSvc.cpp
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
 * @(#)RCSfile: OraVdqmSvc.cpp  Revision: 1.0  Release Date: May 31, 2005  Author: mbraeger 
 *
 *
 *
 * @author Matthias Braeger
 *****************************************************************************/

#include "castor/IFactory.hpp"
#include "castor/SvcFactory.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/Constants.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/stager/Tape.hpp"

#include "castor/vdqm/TapeAccessSpecification.hpp"
#include "castor/vdqm/DeviceGroupName.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeRequest.hpp"
#include "castor/vdqm/TapeServer.hpp"
#include "castor/vdqm/newVdqm.h"

#include "occi.h"
#include <Cuuid.h>
#include <vdqm_constants.h>
#include <net.h>

// Local includes
#include "OraVdqmSvc.hpp"


 
// -----------------------------------------------------------------------
// Instantiation of a static factory class
// -----------------------------------------------------------------------
static castor::SvcFactory<castor::db::ora::OraVdqmSvc> s_factoryOraVdqmSvc;
const castor::IFactory<castor::IService>&
OraVdqmSvcFactory = s_factoryOraVdqmSvc;


//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for function getTapeServer
const std::string castor::db::ora::OraVdqmSvc::s_selectTapeServerStatementString =
  "SELECT id FROM TapeServer WHERE serverName = :1 FOR UPDATE";


/// SQL statement for function checkTapeRequest
const std::string castor::db::ora::OraVdqmSvc::s_checkTapeRequestStatement1String =
  "SELECT tapeDrive FROM TapeRequest WHERE id = :1";

/// SQL statement for function checkTapeRequest
const std::string castor::db::ora::OraVdqmSvc::s_checkTapeRequestStatement2String =
  "SELECT count(*) FROM TapeRequest WHERE id <= :1";
  

/// SQL statement for function selectFreeTapeDrive
const std::string castor::db::ora::OraVdqmSvc::s_selectFreeTapeDriveStatementString =
  "SELECT id FROM Tape WHERE status = :1";
  
/// SQL statement for function deleteAllTapeDrvsFromSrv
const std::string castor::db::ora::OraVdqmSvc::s_selectTapeDriveStatementString =
  "SELECT id FROM TapeDrive WHERE driveName = :1 AND tapeServer = :2 FOR UPDATE";
  
/// SQL statement for function existTapeDriveWithTapeInUse
const std::string castor::db::ora::OraVdqmSvc::s_existTapeDriveWithTapeInUseStatementString =
  "SELECT id FROM TapeDrive WHERE runningTapeReq = (SELECT id FROM TapeRequest WHERE tape = (SELECT id FROM Tape WHERE vid = :1))";
  
/// SQL statement for function existTapeDriveWithTapeMounted
const std::string castor::db::ora::OraVdqmSvc::s_existTapeDriveWithTapeMountedStatementString =
  "SELECT id FROM TapeDrive WHERE tape = (SELECT id FROM Tape WHERE vid = :1)";
  
/// SQL statement for function selectTapeByVid
const std::string castor::db::ora::OraVdqmSvc::s_selectTapeByVidStatementString =
  "SELECT id FROM Tape WHERE vid = :1";      
  
/// SQL statement for function selectTapeReqForMountedTape
const std::string castor::db::ora::OraVdqmSvc::s_selectTapeReqForMountedTapeStatementString =
  "SELECT id FROM TapeRequest WHERE tapeDrive = 0 AND tape = :1 AND (requestedSrv = :2 OR requestedSrv = 0) FOR UPDATE";
  
/// SQL statement for function selectTapeAccessSpecification
const std::string castor::db::ora::OraVdqmSvc::s_selectTapeAccessSpecificationStatementString =
  "SELECT id FROM TapeAccessSpecification WHERE accessMode = :1 AND density = :2 AND tapeModel = :3";
  
/// SQL statement for function selectDeviceGroupName
const std::string castor::db::ora::OraVdqmSvc::s_selectDeviceGroupNameStatementString =
  "SELECT id FROM DeviceGroupName WHERE dgName = :1";
  
/// SQL statement for function selectTapeRequestQueue
const std::string castor::db::ora::OraVdqmSvc::s_selectTapeRequestQueueStatementString =
  "BEGIN selectTapeRequestQueue(:1, :2, :3); END;";

/**
 * This is the main select statement to dedicate a tape to a tape drive.
 * It respects the old and of course the new way to select a tape for a 
 * tape drive.
 * The old way is to look, if the tapeDrive and the tapeRequest have the same
 * dgn.
 * The new way is to look if the TapeAccessSpecification can be served by a 
 * specific tapeDrive. The tape Request are then orderd by the priorityLevel (for 
 * the new way) and by the modification time.
 */  
/// SQL statement for function matchTape2TapeDrive
const std::string castor::db::ora::OraVdqmSvc::s_matchTape2TapeDriveStatementString =
  "BEGIN testMatchTape2TapeDrive(:1, :2); END;";        
  

// -----------------------------------------------------------------------
// OraVdqmSvc
// -----------------------------------------------------------------------
castor::db::ora::OraVdqmSvc::OraVdqmSvc(const std::string name) :
  OraCommonSvc(name),
  m_selectTapeServerStatement(0),
  m_checkTapeRequestStatement1(0),
  m_checkTapeRequestStatement2(0),
  m_selectFreeTapeDriveStatement(0),
  m_selectTapeDriveStatement(0),
  m_existTapeDriveWithTapeInUseStatement(0),
  m_existTapeDriveWithTapeMountedStatement(0),
  m_selectTapeByVidStatement(0),
  m_selectTapeReqForMountedTapeStatement(0),
  m_selectTapeAccessSpecificationStatement(0),
  m_selectDeviceGroupNameStatement(0),
  m_selectTapeRequestQueueStatement(0),
  m_matchTape2TapeDriveStatement(0) {
}


// -----------------------------------------------------------------------
// ~OraVdqmSvc
// -----------------------------------------------------------------------
castor::db::ora::OraVdqmSvc::~OraVdqmSvc() throw() {
  reset();
}

// -----------------------------------------------------------------------
// id
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraVdqmSvc::id() const {
  return ID();
}

// -----------------------------------------------------------------------
// ID
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraVdqmSvc::ID() {
  return castor::SVC_ORAVDQMSVC;
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraVdqmSvc::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  OraCommonSvc::reset();
  try {
    deleteStatement(m_selectTapeServerStatement);
    deleteStatement(m_checkTapeRequestStatement1);
    deleteStatement(m_checkTapeRequestStatement2);
    deleteStatement(m_selectFreeTapeDriveStatement);
    deleteStatement(m_selectTapeDriveStatement);
    deleteStatement(m_existTapeDriveWithTapeInUseStatement);
    deleteStatement(m_existTapeDriveWithTapeMountedStatement);
    deleteStatement(m_selectTapeByVidStatement);
    deleteStatement(m_selectTapeReqForMountedTapeStatement);
    deleteStatement(m_selectTapeAccessSpecificationStatement);
    deleteStatement(m_selectDeviceGroupNameStatement);
    deleteStatement(m_selectTapeRequestQueueStatement);
    deleteStatement(m_matchTape2TapeDriveStatement);
  } catch (oracle::occi::SQLException e) {};
  
  // Now reset all pointers to 0
  m_selectTapeServerStatement = 0;
  m_checkTapeRequestStatement1 = 0;
  m_checkTapeRequestStatement2 = 0;
  m_selectFreeTapeDriveStatement = 0;
  m_selectTapeDriveStatement = 0;
  m_existTapeDriveWithTapeInUseStatement = 0;
  m_existTapeDriveWithTapeMountedStatement = 0;
  m_selectTapeByVidStatement = 0;
  m_selectTapeReqForMountedTapeStatement = 0;
  m_selectTapeAccessSpecificationStatement = 0;
  m_selectDeviceGroupNameStatement = 0;
  m_selectTapeRequestQueueStatement = 0;
  m_matchTape2TapeDriveStatement = 0;
}


// -----------------------------------------------------------------------
// selectTapeServer
// -----------------------------------------------------------------------
castor::vdqm::TapeServer* 
	castor::db::ora::OraVdqmSvc::selectTapeServer(const std::string serverName)
  throw (castor::exception::Exception) {
  
  //Check, if the parameter isn't empty 
  if ( std::strlen(serverName.c_str()) == 0 )
  	return NULL;
  	
  // Check whether the statements are ok
  if (0 == m_selectTapeServerStatement) {
    m_selectTapeServerStatement = createStatement(s_selectTapeServerStatementString);
  }
  // Execute statement and get result
  u_signed64 id;
  try {
    m_selectTapeServerStatement->setString(1, serverName);
    oracle::occi::ResultSet *rset = m_selectTapeServerStatement->executeQuery();
    
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_selectTapeServerStatement->closeResultSet(rset);
      
      // we found nothing, so let's create the tape sever
      castor::vdqm::TapeServer* tapeServer = new castor::vdqm::TapeServer();
      tapeServer->setServerName(serverName);
      tapeServer->setStatus(VDQM_TPD_STARTED);
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      try {
        cnvSvc()->createRep(&ad, tapeServer, false);
        return tapeServer;
      } catch (oracle::occi::SQLException e) {
        delete tapeServer;
        if (1 == e.getErrorCode()) {
          // if violation of unique constraint, ie means that
          // some other thread was quicker than us on the insertion
          // So let's select what was inserted
          rset = m_selectTapeServerStatement->executeQuery();
          if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
            // Still nothing ! Here it's a real error
            m_selectTapeServerStatement->closeResultSet(rset);
            castor::exception::Internal ex;
            ex.getMessage()
              << "Unable to select tapeServer while inserting "
              << "violated unique constraint :"
              << std::endl << e.getMessage();
            throw ex;
          }
        }
        m_selectTapeServerStatement->closeResultSet(rset);
        // Else, "standard" error, throw exception
        castor::exception::Internal ex;
        ex.getMessage()
          << "Exception while inserting new tapeServer in the DB :"
          << std::endl << e.getMessage();
        throw ex;
      }
    }
    
    // If we reach this point, then we selected successfully
    // a tapeServer and it's id is in rset
    id = (u_signed64)rset->getDouble(1);
    m_selectTapeServerStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tapeServer by vid, side and tpmode :"
      << std::endl << e.getMessage();
    throw ex;
  }
  
  // Now get the tapeServer from its id
  try {
    castor::BaseAddress ad;
    ad.setTarget(id);
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    castor::IObject* obj = cnvSvc()->createObj(&ad);
    
    castor::vdqm::TapeServer* tapeServer =
      dynamic_cast<castor::vdqm::TapeServer*> (obj);
    if (0 == tapeServer) {
      castor::exception::Internal e;
      e.getMessage() << "createObj return unexpected type "
                     << obj->type() << " for id " << id;
      delete obj;
      throw e;
    }
    return tapeServer;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tapeServer for id " << id  << " :"
      << std::endl << e.getMessage();
    throw ex;
  }
  // We should never reach this point
}


// -----------------------------------------------------------------------
// checkTapeRequest
// -----------------------------------------------------------------------
int castor::db::ora::OraVdqmSvc::checkTapeRequest(
	const castor::vdqm::TapeRequest *tapeRequest) 
	throw (castor::exception::Exception) {
  	
  try {
    // Check whether the statements are ok
    if (0 == m_checkTapeRequestStatement1) {
      m_checkTapeRequestStatement1 =
        createStatement(s_checkTapeRequestStatement1String);
    }
    
    if (0 == m_checkTapeRequestStatement2) {
      m_checkTapeRequestStatement2 =
        createStatement(s_checkTapeRequestStatement2String);
    }
    
    // execute the statement1 and see whether we found something   
    m_checkTapeRequestStatement1->setDouble(1, tapeRequest->id());
    oracle::occi::ResultSet *rset = m_checkTapeRequestStatement1->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      // Nothing found, return -1
      m_checkTapeRequestStatement1->closeResultSet(rset);
      return -1;
    }
    else if ((int)rset->getInt(1)) {
    	//The request is already beeing handled from  a tape Drive
    	// In this case we return the queue position 0
    	return 0;
    }
    
    // If we come up to this line, the job is not handled till now, but it exists
    // execute the statement2 and see whether we found something
    m_checkTapeRequestStatement2->setDouble(1, tapeRequest->id());
    rset = m_checkTapeRequestStatement2->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      // Nothing found, return -1
      //Normally, the second statement should always find something!
      m_checkTapeRequestStatement2->closeResultSet(rset);
      return -1;
    }
    
    // Return the TapeRequest queue position
    // TODO: Maybe in future the return value should be double!
    return (int)rset->getInt(1);
  } catch (oracle::occi::SQLException e) {
    rollback();
    castor::exception::Internal ex;
    ex.getMessage()
      << "Error caught in checkTapeRequest."
      << std::endl << e.what();
    throw ex;
  }
  // We should never reach this point.
}

//// -----------------------------------------------------------------------
//// selectFreeTapeDrive
//// -----------------------------------------------------------------------
//castor::vdqm::TapeDrive* 
//	castor::db::ora::OraVdqmSvc::selectFreeTapeDrive
//	(const castor::vdqm::ExtendedDeviceGroup *extDevGrp)
//  throw (castor::exception::Exception) {
// return NULL; 	
//}


// -----------------------------------------------------------------------
// selectTapeDrive
// -----------------------------------------------------------------------
castor::vdqm::TapeDrive* 
	castor::db::ora::OraVdqmSvc::selectTapeDrive(
	const newVdqmDrvReq_t* driveRequest,
	castor::vdqm::TapeServer* tapeServer)
  throw (castor::exception::Exception) {
  	
  // Check whether the statements are ok
  if (0 == m_selectTapeDriveStatement) {
    m_selectTapeDriveStatement = createStatement(s_selectTapeDriveStatementString);
  }
  // Execute statement and get result
  u_signed64 id;
  try {
    m_selectTapeDriveStatement->setString(1, driveRequest->drive);
    m_selectTapeDriveStatement->setDouble(2, tapeServer->id());
    oracle::occi::ResultSet *rset = m_selectTapeDriveStatement->executeQuery();
    
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_selectTapeDriveStatement->closeResultSet(rset);
      
      // we found nothing, so return NULL
			return NULL;
    }
    
    // If we reach this point, then we selected successfully
    // a tapeDrive and it's id is in rset
    id = (u_signed64)rset->getDouble(1);
    m_selectTapeDriveStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tapeDrive by driveName and tapeServer id: "
      << std::endl << e.getMessage();
    throw ex;
  }
  
  // Now get the tapeDrive from its id
  try {
		castor::BaseAddress ad;
    ad.setTarget(id);
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    castor::IObject* obj = cnvSvc()->createObj(&ad);
    
    castor::vdqm::TapeDrive* tapeDrive =
      dynamic_cast<castor::vdqm::TapeDrive*> (obj);
    if (0 == tapeDrive) {
      castor::exception::Internal e;
      e.getMessage() << "createObj return unexpected type "
                     << obj->type() << " for id " << id;
      delete obj;
      throw e;
    }
    
    //Now we get the foreign related objects

    cnvSvc()->fillObj(&ad, obj, castor::OBJ_TapeRequest);
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_Tape);	
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_DeviceGroupName);	
    
    tapeDrive->setTapeServer(tapeServer);

    /**
     * If there is already an assigned tapeRequest, we want also its objects
     */
    castor::vdqm::TapeRequest* tapeRequest = tapeDrive->runningTapeReq();
    if (tapeRequest != NULL) {
//        cnvSvc()->fillObj(&ad, tapeRequest, castor::OBJ_ClientIdentification);
        cnvSvc()->fillObj(&ad, tapeRequest, castor::OBJ_Tape);
        cnvSvc()->fillObj(&ad, tapeRequest, castor::OBJ_DeviceGroupName);
        cnvSvc()->fillObj(&ad, tapeRequest, castor::OBJ_TapeAccessSpecification);        
        cnvSvc()->fillObj(&ad, tapeRequest, castor::OBJ_TapeServer);
    }
     
    //Reset pointer
    obj = 0;
    tapeRequest = 0;
    
    return tapeDrive;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tapeDrive for id " << id  << " :"
      << std::endl << e.getMessage();
    throw ex;
  }
  // We should never reach this point
}


// -----------------------------------------------------------------------
// existTapeDriveWithTapeInUse
// -----------------------------------------------------------------------
bool
	castor::db::ora::OraVdqmSvc::existTapeDriveWithTapeInUse(
	const std::string volid)
  throw (castor::exception::Exception) {
  
  // Execute statement and get result
  u_signed64 id;
  	
  // Check whether the statements are ok
  if (0 == m_existTapeDriveWithTapeInUseStatement) {
    m_existTapeDriveWithTapeInUseStatement = 
    	createStatement(s_existTapeDriveWithTapeInUseStatementString);
  }

  try {
    m_existTapeDriveWithTapeInUseStatement->setString(1, volid);
    oracle::occi::ResultSet *rset = m_existTapeDriveWithTapeInUseStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_existTapeDriveWithTapeInUseStatement->closeResultSet(rset);
      // we found nothing, so let's return false
			return false;
    }
    // If we reach this point, then we selected successfully
    // a tape drive and it's id is in rset
    id = (u_signed64)rset->getDouble(1);
    m_existTapeDriveWithTapeInUseStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tape by vid: "
      << std::endl << e.getMessage();
    throw ex;
  }  
  
  return true;
}


// -----------------------------------------------------------------------
// existTapeDriveWithTapeMounted
// -----------------------------------------------------------------------
bool
	castor::db::ora::OraVdqmSvc::existTapeDriveWithTapeMounted(
	const std::string volid)
  throw (castor::exception::Exception) {

  // Execute statement and get result
  u_signed64 id;
  	
  // Check whether the statements are ok
  if (0 == m_existTapeDriveWithTapeMountedStatement) {
    m_existTapeDriveWithTapeMountedStatement = 
    	createStatement(s_existTapeDriveWithTapeMountedStatementString);
  }

  try {
    m_existTapeDriveWithTapeMountedStatement->setString(1, volid);
    oracle::occi::ResultSet *rset = m_existTapeDriveWithTapeMountedStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_existTapeDriveWithTapeMountedStatement->closeResultSet(rset);
      // we found nothing, so let's return false
			return false;
    }
    // If we reach this point, then we selected successfully
    // a tape drive and it's id is in rset
    id = (u_signed64)rset->getDouble(1);
    m_existTapeDriveWithTapeMountedStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tape by vid: "
      << std::endl << e.getMessage();
    throw ex;
  }
 	
 	return true;  	
}


// -----------------------------------------------------------------------
// selectTapeByVid
// -----------------------------------------------------------------------
castor::stager::Tape* 
	castor::db::ora::OraVdqmSvc::selectTapeByVid(
	const std::string volid)
  throw (castor::exception::Exception) {
  	
  // Check whether the statements are ok
  if (0 == m_selectTapeByVidStatement) {
    m_selectTapeByVidStatement = createStatement(s_selectTapeByVidStatementString);
  }
  // Execute statement and get result
  u_signed64 id;
  try {
    m_selectTapeByVidStatement->setString(1, volid);
    oracle::occi::ResultSet *rset = m_selectTapeByVidStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_selectTapeByVidStatement->closeResultSet(rset);
      // we found nothing, so let's return NULL
			return NULL;
    }
    // If we reach this point, then we selected successfully
    // a tape and it's id is in rset
    id = (u_signed64)rset->getDouble(1);
    m_selectTapeByVidStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tape by vid: "
      << std::endl << e.getMessage();
    throw ex;
  }
  // Now get the tape from its id
  try {
    castor::BaseAddress ad;
    ad.setTarget(id);
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    castor::IObject* obj = cnvSvc()->createObj(&ad);
    castor::stager::Tape* tape =
      dynamic_cast<castor::stager::Tape*> (obj);
    if (0 == tape) {
      castor::exception::Internal e;
      e.getMessage() << "createObj return unexpected type "
                     << obj->type() << " for id " << id;
      delete obj;
      throw e;
    }
    return tape;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tape for id " << id  << " :"
      << std::endl << e.getMessage();
    throw ex;
  }
  // We should never reach this point	
}


// -----------------------------------------------------------------------
// selectTapeReqForMountedTape
// -----------------------------------------------------------------------
castor::vdqm::TapeRequest* 
	castor::db::ora::OraVdqmSvc::selectTapeReqForMountedTape(
	const castor::vdqm::TapeDrive* tapeDrive)
  throw (castor::exception::Exception) {
	
  // Check whether the statements are ok
  if (0 == m_selectTapeReqForMountedTapeStatement) {
    m_selectTapeReqForMountedTapeStatement = 
    	createStatement(s_selectTapeReqForMountedTapeStatementString);
  }
  // Execute statement and get result
  u_signed64 id;
  try {
    m_selectTapeReqForMountedTapeStatement->setDouble(1, tapeDrive->tape()->id());
    m_selectTapeReqForMountedTapeStatement->setDouble(2, tapeDrive->tapeServer()->id());
    oracle::occi::ResultSet *rset = m_selectTapeReqForMountedTapeStatement->executeQuery();
    
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_selectTapeReqForMountedTapeStatement->closeResultSet(rset);
      
      // we found nothing, so return NULL
			return NULL;
    }
    
    // If we reach this point, then we selected successfully
    // a tapeDrive and it's id is in rset
    id = (u_signed64)rset->getDouble(1);
    m_selectTapeReqForMountedTapeStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tapeDrive by vid, side and tpmode :"
      << std::endl << e.getMessage();
    throw ex;
  }
  
  // Now get the tapeRequest from its id
  try {
		castor::BaseAddress ad;
    ad.setTarget(id);
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    castor::IObject* obj = cnvSvc()->createObj(&ad);
    
    castor::vdqm::TapeRequest* tapeRequest =
      dynamic_cast<castor::vdqm::TapeRequest*> (obj);
    if (0 == tapeRequest) {
      castor::exception::Internal e;
      e.getMessage() << "createObj return unexpected type "
                     << obj->type() << " for id " << id;
      delete obj;
      throw e;
    }
    
    //Now we get the foreign related objects
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_ClientIdentification);
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_TapeServer);
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_Tape);
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_DeviceGroupName);
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_TapeAccessSpecification);    

    //Reset pointer
    obj = 0;
    
    return tapeRequest;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select tapeRequest for id " << id  << " :"
      << std::endl << e.getMessage();
    throw ex;
  }
  // We should never reach this point	
}


// -----------------------------------------------------------------------
// selectTapeAccessSpecification
// -----------------------------------------------------------------------
castor::vdqm::TapeAccessSpecification* 
	castor::db::ora::OraVdqmSvc::selectTapeAccessSpecification
	(const int accessMode,
	 const std::string density,
	 const std::string tapeModel)
  throw (castor::exception::Exception) {
  	
//  std::cout << "accessMode: " << accessMode << std::endl;
//  std::cout << "density: " << density << std::endl;
//  std::cout << "tapeModel: " << tapeModel << std::endl;  	

  // Check whether the statements are ok
  if (0 == m_selectTapeAccessSpecificationStatement) {
    m_selectTapeAccessSpecificationStatement = 
    	createStatement(s_selectTapeAccessSpecificationStatementString);
  }
  // Execute statement and get result
  u_signed64 id;
  try {
    m_selectTapeAccessSpecificationStatement->setInt(1, accessMode);
    m_selectTapeAccessSpecificationStatement->setString(2, density);
    m_selectTapeAccessSpecificationStatement->setString(3, tapeModel);    
    oracle::occi::ResultSet *rset = m_selectTapeAccessSpecificationStatement->executeQuery();
    
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_selectTapeAccessSpecificationStatement->closeResultSet(rset);
      // we found nothing, so let's return the NULL pointer  
			return NULL;
    }
    
    // If we reach this point, then we selected successfully
    // a dgName and it's id is in rset
    id = (u_signed64)rset->getDouble(1);
    m_selectTapeAccessSpecificationStatement->closeResultSet(rset);
  
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select TapeAccessSpecification by accessMode, density, tapeModel:"
      << std::endl << e.getMessage();
    throw ex;
  }
  // Now get the DeviceGroupName from its id
  try {
    castor::BaseAddress ad;
    ad.setTarget(id);
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    castor::IObject* obj = cnvSvc()->createObj(&ad);
    castor::vdqm::TapeAccessSpecification* tapeAccessSpec =
      dynamic_cast<castor::vdqm::TapeAccessSpecification*> (obj);
    if (0 == tapeAccessSpec) {
      castor::exception::Internal e;
      e.getMessage() << "createObj return unexpected type "
                     << obj->type() << " for id " << id;
      delete obj;
      throw e;
    }
    return tapeAccessSpec;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select TapeAccessSpecification for id " << id  << " :"
      << std::endl << e.getMessage();
    throw ex;
  }
  // We should never reach this point 	
}


// -----------------------------------------------------------------------
// selectDeviceGroupName
// -----------------------------------------------------------------------
castor::vdqm::DeviceGroupName* 
	castor::db::ora::OraVdqmSvc::selectDeviceGroupName
	(const std::string dgName)
  throw (castor::exception::Exception) {

  // Check whether the statements are ok
  if (0 == m_selectDeviceGroupNameStatement) {
    m_selectDeviceGroupNameStatement = createStatement(s_selectDeviceGroupNameStatementString);
  }
  // Execute statement and get result
  u_signed64 id;
  try {
    m_selectDeviceGroupNameStatement->setString(1, dgName);
    oracle::occi::ResultSet *rset = m_selectDeviceGroupNameStatement->executeQuery();
    
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      m_selectDeviceGroupNameStatement->closeResultSet(rset);
      // we found nothing, so let's return NULL
      
      return NULL;
    }
    // If we reach this point, then we selected successfully
    // a DeviceGroupName and it's id is in rset
    id = (u_signed64)rset->getDouble(1);
    m_selectDeviceGroupNameStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select DeviceGroupName by dgName :"
      << std::endl << e.getMessage();
    throw ex;
  }
  // Now get the DeviceGroupName from its id
  try {
    castor::BaseAddress ad;
    ad.setTarget(id);
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    castor::IObject* obj = cnvSvc()->createObj(&ad);
    castor::vdqm::DeviceGroupName* deviceGroupName =
      dynamic_cast<castor::vdqm::DeviceGroupName*> (obj);
    if (0 == deviceGroupName) {
      castor::exception::Internal e;
      e.getMessage() << "createObj return unexpected type "
                     << obj->type() << " for id " << id;
      delete obj;
      throw e;
    }
    return deviceGroupName;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to select DeviceGroupName for id " << id  << " :"
      << std::endl << e.getMessage();
    throw ex;
  }
  // We should never reach this point 	
}


// -----------------------------------------------------------------------
// selectTapeRequestQueue
// -----------------------------------------------------------------------
std::vector<castor::vdqm::TapeRequest*>* 
	castor::db::ora::OraVdqmSvc::selectTapeRequestQueue (
	const std::string dgn, 
	const std::string requestedSrv)
	throw (castor::exception::Exception) {

  u_signed64 idTapeRequest;
	castor::vdqm::TapeRequest* tapeRequest = NULL;
  
  // Check whether the statements are ok
  if (0 == m_selectTapeRequestQueueStatement) {
    m_selectTapeRequestQueueStatement = 
    	createStatement(s_selectTapeRequestQueueStatementString);
    
    m_selectTapeRequestQueueStatement->registerOutParam
        (3, oracle::occi::OCCIDOUBLE);
  }
  
  // Execute statement and get result
  try {
  	m_selectTapeRequestQueueStatement->setString(1, dgn);
  	m_selectTapeRequestQueueStatement->setString(2, requestedSrv);
    unsigned int nb = m_selectTapeRequestQueueStatement->executeUpdate();
    
    if (0 == nb) {
      castor::exception::Internal ex;
      ex.getMessage() << "Result is empty";
      throw ex;
    }    
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "OraVdqmSvc::selectTapeRequestQueue(): "
      << "Unable to find TapeRequests for queue: "
      << std::endl << e.getMessage();
    throw ex;
  }	
  
  
  // Now get the the Objects from their id's
  try {
    // create result
    std::vector<castor::vdqm::TapeRequest*>* result =
      new std::vector<castor::vdqm::TapeRequest*>;
      
    // Run through the cursor
    oracle::occi::ResultSet *rs =
      m_selectTapeRequestQueueStatement->getCursor(3);
    oracle::occi::ResultSet::Status status = rs->next();
    
    
    /**
     * For each id, we create a TapeRequest object and the needed linked objects
     * from the other tables.
     */
    while(status == oracle::occi::ResultSet::DATA_AVAILABLE) {
    	idTapeRequest = 0;
    	tapeRequest = 0;
    	
      // Fill result
      idTapeRequest = (u_signed64)rs->getDouble(3);
      
      if ( idTapeRequest != 0 ) {
	      castor::BaseAddress ad;
		    ad.setTarget(idTapeRequest);
		    ad.setCnvSvcName("DbCnvSvc");
		    ad.setCnvSvcType(castor::SVC_DBCNV);
				
				castor::IObject* obj = cnvSvc()->createObj(&ad);
		    tapeRequest = dynamic_cast<castor::vdqm::TapeRequest*> (obj);
		    if (0 == tapeRequest) {
		      castor::exception::Internal e;
		      e.getMessage() << "createObj return unexpected type "
		                     << obj->type() << " for id " << idTapeRequest;
		      delete obj;
		      
		      throw e;
		    }  
		    
		    // Get the foreign related objects
		    cnvSvc()->fillObj(&ad, obj, castor::OBJ_DeviceGroupName);
		    cnvSvc()->fillObj(&ad, obj, castor::OBJ_TapeAccessSpecification);
		    cnvSvc()->fillObj(&ad, obj, castor::OBJ_TapeServer);
		    cnvSvc()->fillObj(&ad, obj, castor::OBJ_Tape);
		    cnvSvc()->fillObj(&ad, obj, castor::OBJ_TapeDrive);
		    cnvSvc()->fillObj(&ad, obj, castor::OBJ_ClientIdentification);
	      
	      
	      result->push_back(tapeRequest);
	      status = rs->next();
      }
    }
    
    return result;  	
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to find a TapeRequest for id "
      << idTapeRequest << " :"
      << std::endl << e.getMessage();
  }  
}


// -----------------------------------------------------------------------
// matchTape2TapeDrive
// -----------------------------------------------------------------------
void castor::db::ora::OraVdqmSvc::matchTape2TapeDrive (
	castor::vdqm::TapeDrive** freeTapeDrive, 
	castor::vdqm::TapeRequest** waitingTapeRequest)
  throw (castor::exception::Exception) {
  
  u_signed64 idTapeDrive, idTapeRequest;
  
  castor::vdqm::TapeDrive* tapeDrive = NULL;
	castor::vdqm::TapeRequest* tapeRequest = NULL;
  
  // Check whether the statements are ok
  if (0 == m_matchTape2TapeDriveStatement) {
    m_matchTape2TapeDriveStatement = createStatement(s_matchTape2TapeDriveStatementString);
    
    m_matchTape2TapeDriveStatement->registerOutParam
        (1, oracle::occi::OCCIDOUBLE);
    m_matchTape2TapeDriveStatement->registerOutParam
        (2, oracle::occi::OCCIDOUBLE);
  }
  
  // Execute statement and get result
  try {
    m_matchTape2TapeDriveStatement->executeUpdate();
    
    // If we reach this point, then we selected successfully
    // a TapeDrive + TapeRequest and their id's are in rset
    idTapeDrive = (u_signed64)m_matchTape2TapeDriveStatement->getDouble(1);
    idTapeRequest = (u_signed64)m_matchTape2TapeDriveStatement->getDouble(2);
    
    if ( idTapeDrive == 0 || idTapeRequest == 0 ) {
      // we found nothing, so let's just return two NULL Pointers
      
      *freeTapeDrive = NULL;      
      *waitingTapeRequest = NULL;
      
      return;    	
    }
    
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to find a TapeRequest for a free TapeDrive: "
      << std::endl << e.getMessage();
    throw ex;
  }
  
  // Now get the the two Objects from their id's
  try {
    castor::BaseAddress ad;
    ad.setTarget(idTapeDrive);
    ad.setCnvSvcName("DbCnvSvc");
    ad.setCnvSvcType(castor::SVC_DBCNV);
    
    castor::IObject* obj = cnvSvc()->createObj(&ad);
    tapeDrive = dynamic_cast<castor::vdqm::TapeDrive*> (obj);
    if (0 == tapeDrive) {
      castor::exception::Internal e;
      e.getMessage() << "createObj return unexpected type "
                     << obj->type() << " for id " << idTapeDrive;
      delete obj;
      throw e;
    }
    
    // Get the foreign related objects
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_DeviceGroupName);
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_TapeRequest);
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_Tape);		
    cnvSvc()->fillObj(&ad, obj, castor::OBJ_TapeServer);		
    
    
    castor::BaseAddress ad2;
    ad2.setTarget(idTapeRequest);
    ad2.setCnvSvcName("DbCnvSvc");
    ad2.setCnvSvcType(castor::SVC_DBCNV);
		
		obj = cnvSvc()->createObj(&ad2);
    tapeRequest = dynamic_cast<castor::vdqm::TapeRequest*> (obj);
    if (0 == tapeRequest) {
      castor::exception::Internal e;
      e.getMessage() << "createObj return unexpected type "
                     << obj->type() << " for id " << idTapeRequest;
      delete obj;
      delete tapeDrive;
      throw e;
    }  
    
    // Get the foreign related objects
    cnvSvc()->fillObj(&ad2, obj, castor::OBJ_TapeDrive);
    cnvSvc()->fillObj(&ad2, obj, castor::OBJ_ClientIdentification);		
    
    /**
     * If we are here, everything went fine and we found a new 
     * tape <--> tape drive couple 
     */    
    *freeTapeDrive = tapeDrive;
    *waitingTapeRequest = tapeRequest;      
    
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage()
      << "Unable to find a TapeRequest or a TapeDrive for id(TapeDrive) " 
      << idTapeDrive  
      << " or for id(TapeRequest) "
      << idTapeRequest << " :"
      << std::endl << e.getMessage();
    throw ex;
  }
}

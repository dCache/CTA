/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbRepackSubRequestCnv.cpp
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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "DbRepackSubRequestCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/repack/RepackRequest.hpp"
#include "castor/repack/RepackSegment.hpp"
#include "castor/repack/RepackSubRequest.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbRepackSubRequestCnv>* s_factoryDbRepackSubRequestCnv =
  new castor::CnvFactory<castor::db::cnv::DbRepackSubRequestCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_insertStatementString =
"INSERT INTO RepackSubRequest (vid, xsize, status, cuuid, filesMigrating, filesStaging, files, filesFailed, submitTime, filesStaged, id, requestID) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,ids_seq.nextval,:11) RETURNING id INTO :12";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_deleteStatementString =
"DELETE FROM RepackSubRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_selectStatementString =
"SELECT vid, xsize, status, cuuid, filesMigrating, filesStaging, files, filesFailed, submitTime, filesStaged, id, requestID FROM RepackSubRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_updateStatementString =
"UPDATE RepackSubRequest SET vid = :1, xsize = :2, status = :3, cuuid = :4, filesMigrating = :5, filesStaging = :6, files = :7, filesFailed = :8, submitTime = :9, filesStaged = :10 WHERE id = :11";

/// SQL statement for type storage
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member segment
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_selectRepackSegmentStatementString =
"SELECT id FROM RepackSegment WHERE vid = :1 FOR UPDATE";

/// SQL delete statement for member segment
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_deleteRepackSegmentStatementString =
"UPDATE RepackSegment SET vid = 0 WHERE id = :1";

/// SQL remote update statement for member segment
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_remoteUpdateRepackSegmentStatementString =
"UPDATE RepackSegment SET vid = :1 WHERE id = :2";

/// SQL existence statement for member requestID
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_checkRepackRequestExistStatementString =
"SELECT id FROM RepackRequest WHERE id = :1";

/// SQL update statement for member requestID
const std::string castor::db::cnv::DbRepackSubRequestCnv::s_updateRepackRequestStatementString =
"UPDATE RepackSubRequest SET requestID = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbRepackSubRequestCnv::DbRepackSubRequestCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectRepackSegmentStatement(0),
  m_deleteRepackSegmentStatement(0),
  m_remoteUpdateRepackSegmentStatement(0),
  m_checkRepackRequestExistStatement(0),
  m_updateRepackRequestStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbRepackSubRequestCnv::~DbRepackSubRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_deleteRepackSegmentStatement) delete m_deleteRepackSegmentStatement;
    if(m_selectRepackSegmentStatement) delete m_selectRepackSegmentStatement;
    if(m_remoteUpdateRepackSegmentStatement) delete m_remoteUpdateRepackSegmentStatement;
    if(m_checkRepackRequestExistStatement) delete m_checkRepackRequestExistStatement;
    if(m_updateRepackRequestStatement) delete m_updateRepackRequestStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectRepackSegmentStatement = 0;
  m_deleteRepackSegmentStatement = 0;
  m_remoteUpdateRepackSegmentStatement = 0;
  m_checkRepackRequestExistStatement = 0;
  m_updateRepackRequestStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbRepackSubRequestCnv::ObjType() {
  return castor::repack::RepackSubRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbRepackSubRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::fillRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     unsigned int type,
                                                     bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackSubRequest* obj = 
    dynamic_cast<castor::repack::RepackSubRequest*>(object);
  try {
    switch (type) {
    case castor::OBJ_RepackSegment :
      fillRepRepackSegment(obj);
      break;
    case castor::OBJ_RepackRequest :
      fillRepRepackRequest(obj);
      break;
    default :
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "fillRep called for type " << type 
                      << " on object of type " << obj->type() 
                      << ". This is meaningless.";
      throw ex;
    }
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.getMessage().str() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepRepackSegment
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::fillRepRepackSegment(castor::repack::RepackSubRequest* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectRepackSegmentStatement) {
    m_selectRepackSegmentStatement = createStatement(s_selectRepackSegmentStatementString);
  }
  // Get current database data
  std::set<int> segmentList;
  m_selectRepackSegmentStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectRepackSegmentStatement->executeQuery();
  while (rset->next()) {
    segmentList.insert(rset->getInt(1));
  }
  delete rset;
  // update segment and create new ones
  for (std::vector<castor::repack::RepackSegment*>::iterator it = obj->segment().begin();
       it != obj->segment().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_RepackSubRequest);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateRepackSegmentStatement) {
        m_remoteUpdateRepackSegmentStatement = createStatement(s_remoteUpdateRepackSegmentStatementString);
      }
      // Update remote object
      m_remoteUpdateRepackSegmentStatement->setUInt64(1, obj->id());
      m_remoteUpdateRepackSegmentStatement->setUInt64(2, (*it)->id());
      m_remoteUpdateRepackSegmentStatement->execute();
      std::set<int>::iterator item;
      if ((item = segmentList.find((*it)->id())) != segmentList.end()) {
        segmentList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = segmentList.begin();
       it != segmentList.end();
       it++) {
    if (0 == m_deleteRepackSegmentStatement) {
      m_deleteRepackSegmentStatement = createStatement(s_deleteRepackSegmentStatementString);
    }
    m_deleteRepackSegmentStatement->setUInt64(1, *it);
    m_deleteRepackSegmentStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepRepackRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::fillRepRepackRequest(castor::repack::RepackSubRequest* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->requestID()) {
    // Check checkRepackRequestExist statement
    if (0 == m_checkRepackRequestExistStatement) {
      m_checkRepackRequestExistStatement = createStatement(s_checkRepackRequestExistStatementString);
    }
    // retrieve the object from the database
    m_checkRepackRequestExistStatement->setUInt64(1, obj->requestID()->id());
    castor::db::IDbResultSet *rset = m_checkRepackRequestExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->requestID(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateRepackRequestStatement) {
    m_updateRepackRequestStatement = createStatement(s_updateRepackRequestStatementString);
  }
  // Update local object
  m_updateRepackRequestStatement->setUInt64(1, 0 == obj->requestID() ? 0 : obj->requestID()->id());
  m_updateRepackRequestStatement->setUInt64(2, obj->id());
  m_updateRepackRequestStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::fillObj(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     unsigned int type,
                                                     bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackSubRequest* obj = 
    dynamic_cast<castor::repack::RepackSubRequest*>(object);
  switch (type) {
  case castor::OBJ_RepackSegment :
    fillObjRepackSegment(obj);
    break;
  case castor::OBJ_RepackRequest :
    fillObjRepackRequest(obj);
    break;
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillObj called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
  if (autocommit) {
    cnvSvc()->commit();
  }
}
//------------------------------------------------------------------------------
// fillObjRepackSegment
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::fillObjRepackSegment(castor::repack::RepackSubRequest* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectRepackSegmentStatement) {
    m_selectRepackSegmentStatement = createStatement(s_selectRepackSegmentStatementString);
  }
  // retrieve the object from the database
  std::set<int> segmentList;
  m_selectRepackSegmentStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectRepackSegmentStatement->executeQuery();
  while (rset->next()) {
    segmentList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::repack::RepackSegment*> toBeDeleted;
  for (std::vector<castor::repack::RepackSegment*>::iterator it = obj->segment().begin();
       it != obj->segment().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = segmentList.find((*it)->id())) == segmentList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      segmentList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::repack::RepackSegment*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeSegment(*it);
    (*it)->setVid(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = segmentList.begin();
       it != segmentList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::repack::RepackSegment* remoteObj = 
      dynamic_cast<castor::repack::RepackSegment*>(item);
    obj->addSegment(remoteObj);
    remoteObj->setVid(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjRepackRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::fillObjRepackRequest(castor::repack::RepackSubRequest* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
  if (!rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 requestIDId = rset->getInt64(12);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->requestID() &&
      (0 == requestIDId ||
       obj->requestID()->id() != requestIDId)) {
    obj->requestID()->removeSubRequest(obj);
    obj->setRequestID(0);
  }
  // Update object or create new one
  if (0 != requestIDId) {
    if (0 == obj->requestID()) {
      obj->setRequestID
        (dynamic_cast<castor::repack::RepackRequest*>
         (cnvSvc()->getObjFromId(requestIDId)));
    } else {
      cnvSvc()->updateObj(obj->requestID());
    }
    obj->requestID()->addSubRequest(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::createRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool autocommit,
                                                       unsigned int type)
  throw (castor::exception::Exception) {
  castor::repack::RepackSubRequest* obj = 
    dynamic_cast<castor::repack::RepackSubRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(12, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->vid());
    m_insertStatement->setUInt64(2, obj->xsize());
    m_insertStatement->setInt(3, obj->status());
    m_insertStatement->setString(4, obj->cuuid());
    m_insertStatement->setInt(5, obj->filesMigrating());
    m_insertStatement->setInt(6, obj->filesStaging());
    m_insertStatement->setInt(7, obj->files());
    m_insertStatement->setInt(8, obj->filesFailed());
    m_insertStatement->setUInt64(9, obj->submitTime());
    m_insertStatement->setUInt64(10, obj->filesStaged());
    m_insertStatement->setUInt64(11, (type == OBJ_RepackRequest && obj->requestID() != 0) ? obj->requestID()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(12));
    m_storeTypeStatement->setUInt64(1, obj->id());
    m_storeTypeStatement->setUInt64(2, obj->type());
    m_storeTypeStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  vid : " << obj->vid() << std::endl
                    << "  xsize : " << obj->xsize() << std::endl
                    << "  status : " << obj->status() << std::endl
                    << "  cuuid : " << obj->cuuid() << std::endl
                    << "  filesMigrating : " << obj->filesMigrating() << std::endl
                    << "  filesStaging : " << obj->filesStaging() << std::endl
                    << "  files : " << obj->files() << std::endl
                    << "  filesFailed : " << obj->filesFailed() << std::endl
                    << "  submitTime : " << obj->submitTime() << std::endl
                    << "  filesStaged : " << obj->filesStaged() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  requestID : " << obj->requestID() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::updateRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackSubRequest* obj = 
    dynamic_cast<castor::repack::RepackSubRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->vid());
    m_updateStatement->setUInt64(2, obj->xsize());
    m_updateStatement->setInt(3, obj->status());
    m_updateStatement->setString(4, obj->cuuid());
    m_updateStatement->setInt(5, obj->filesMigrating());
    m_updateStatement->setInt(6, obj->filesStaging());
    m_updateStatement->setInt(7, obj->files());
    m_updateStatement->setInt(8, obj->filesFailed());
    m_updateStatement->setUInt64(9, obj->submitTime());
    m_updateStatement->setUInt64(10, obj->filesStaged());
    m_updateStatement->setUInt64(11, obj->id());
    m_updateStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::deleteRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackSubRequest* obj = 
    dynamic_cast<castor::repack::RepackSubRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_deleteStatement) {
      m_deleteStatement = createStatement(s_deleteStatementString);
    }
    if (0 == m_deleteTypeStatement) {
      m_deleteTypeStatement = createStatement(s_deleteTypeStatementString);
    }
    // Now Delete the object
    m_deleteTypeStatement->setUInt64(1, obj->id());
    m_deleteTypeStatement->execute();
    m_deleteStatement->setUInt64(1, obj->id());
    m_deleteStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in delete request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_deleteStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::db::cnv::DbRepackSubRequestCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::BaseAddress* ad = 
    dynamic_cast<castor::BaseAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setUInt64(1, ad->target());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << ad->target();
      throw ex;
    }
    // create the new Object
    castor::repack::RepackSubRequest* object = new castor::repack::RepackSubRequest();
    // Now retrieve and set members
    object->setVid(rset->getString(1));
    object->setXsize(rset->getUInt64(2));
    object->setStatus(rset->getInt(3));
    object->setCuuid(rset->getString(4));
    object->setFilesMigrating(rset->getInt(5));
    object->setFilesStaging(rset->getInt(6));
    object->setFiles(rset->getInt(7));
    object->setFilesFailed(rset->getInt(8));
    object->setSubmitTime(rset->getUInt64(9));
    object->setFilesStaged(rset->getUInt64(10));
    object->setId(rset->getUInt64(11));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in select request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_selectStatementString << std::endl
                    << "and id was " << ad->target() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbRepackSubRequestCnv::updateObj(castor::IObject* obj)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setUInt64(1, obj->id());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << obj->id();
      throw ex;
    }
    // Now retrieve and set members
    castor::repack::RepackSubRequest* object = 
      dynamic_cast<castor::repack::RepackSubRequest*>(obj);
    object->setVid(rset->getString(1));
    object->setXsize(rset->getUInt64(2));
    object->setStatus(rset->getInt(3));
    object->setCuuid(rset->getString(4));
    object->setFilesMigrating(rset->getInt(5));
    object->setFilesStaging(rset->getInt(6));
    object->setFiles(rset->getInt(7));
    object->setFilesFailed(rset->getInt(8));
    object->setSubmitTime(rset->getUInt64(9));
    object->setFilesStaged(rset->getUInt64(10));
    object->setId(rset->getUInt64(11));
    delete rset;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


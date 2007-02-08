/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbSubRequestCnv.cpp
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
#include "DbSubRequestCnv.hpp"
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
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/FileRequest.hpp"
#include "castor/stager/SubRequest.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbSubRequestCnv>* s_factoryDbSubRequestCnv =
  new castor::CnvFactory<castor::db::cnv::DbSubRequestCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbSubRequestCnv::s_insertStatementString =
"INSERT INTO SubRequest (retryCounter, fileName, protocol, xsize, priority, subreqId, flags, modeBits, creationTime, lastModificationTime, answered, id, diskcopy, castorFile, parent, status, request, getNextStatus) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,ids_seq.nextval,:12,:13,:14,:15,:16,:17) RETURNING id INTO :18";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbSubRequestCnv::s_deleteStatementString =
"DELETE FROM SubRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbSubRequestCnv::s_selectStatementString =
"SELECT retryCounter, fileName, protocol, xsize, priority, subreqId, flags, modeBits, creationTime, lastModificationTime, answered, id, diskcopy, castorFile, parent, status, request, getNextStatus FROM SubRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbSubRequestCnv::s_updateStatementString =
"UPDATE SubRequest SET retryCounter = :1, fileName = :2, protocol = :3, xsize = :4, priority = :5, subreqId = :6, flags = :7, modeBits = :8, lastModificationTime = :9, answered = :10, status = :11, getNextStatus = :12 WHERE id = :13";

/// SQL statement for type storage
const std::string castor::db::cnv::DbSubRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbSubRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member diskcopy
const std::string castor::db::cnv::DbSubRequestCnv::s_checkDiskCopyExistStatementString =
"SELECT id FROM DiskCopy WHERE id = :1";

/// SQL update statement for member diskcopy
const std::string castor::db::cnv::DbSubRequestCnv::s_updateDiskCopyStatementString =
"UPDATE SubRequest SET diskcopy = :1 WHERE id = :2";

/// SQL existence statement for member castorFile
const std::string castor::db::cnv::DbSubRequestCnv::s_checkCastorFileExistStatementString =
"SELECT id FROM CastorFile WHERE id = :1";

/// SQL update statement for member castorFile
const std::string castor::db::cnv::DbSubRequestCnv::s_updateCastorFileStatementString =
"UPDATE SubRequest SET castorFile = :1 WHERE id = :2";

/// SQL existence statement for member parent
const std::string castor::db::cnv::DbSubRequestCnv::s_checkSubRequestExistStatementString =
"SELECT id FROM SubRequest WHERE id = :1";

/// SQL update statement for member parent
const std::string castor::db::cnv::DbSubRequestCnv::s_updateSubRequestStatementString =
"UPDATE SubRequest SET parent = :1 WHERE id = :2";

/// SQL update statement for member request
const std::string castor::db::cnv::DbSubRequestCnv::s_updateFileRequestStatementString =
"UPDATE SubRequest SET request = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbSubRequestCnv::DbSubRequestCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkDiskCopyExistStatement(0),
  m_updateDiskCopyStatement(0),
  m_checkCastorFileExistStatement(0),
  m_updateCastorFileStatement(0),
  m_checkSubRequestExistStatement(0),
  m_updateSubRequestStatement(0),
  m_updateFileRequestStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbSubRequestCnv::~DbSubRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    delete m_insertStatement;
    delete m_deleteStatement;
    delete m_selectStatement;
    delete m_updateStatement;
    delete m_storeTypeStatement;
    delete m_deleteTypeStatement;
    delete m_checkDiskCopyExistStatement;
    delete m_updateDiskCopyStatement;
    delete m_checkCastorFileExistStatement;
    delete m_updateCastorFileStatement;
    delete m_checkSubRequestExistStatement;
    delete m_updateSubRequestStatement;
    delete m_updateFileRequestStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkDiskCopyExistStatement = 0;
  m_updateDiskCopyStatement = 0;
  m_checkCastorFileExistStatement = 0;
  m_updateCastorFileStatement = 0;
  m_checkSubRequestExistStatement = 0;
  m_updateSubRequestStatement = 0;
  m_updateFileRequestStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbSubRequestCnv::ObjType() {
  return castor::stager::SubRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbSubRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::SubRequest* obj = 
    dynamic_cast<castor::stager::SubRequest*>(object);
  try {
    switch (type) {
    case castor::OBJ_DiskCopy :
      fillRepDiskCopy(obj);
      break;
    case castor::OBJ_CastorFile :
      fillRepCastorFile(obj);
      break;
    case castor::OBJ_SubRequest :
      fillRepSubRequest(obj);
      break;
    case castor::OBJ_FileRequest :
      fillRepFileRequest(obj);
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
// fillRepDiskCopy
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillRepDiskCopy(castor::stager::SubRequest* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->diskcopy()) {
    // Check checkDiskCopyExist statement
    if (0 == m_checkDiskCopyExistStatement) {
      m_checkDiskCopyExistStatement = createStatement(s_checkDiskCopyExistStatementString);
    }
    // retrieve the object from the database
    m_checkDiskCopyExistStatement->setUInt64(1, obj->diskcopy()->id());
    castor::db::IDbResultSet *rset = m_checkDiskCopyExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->diskcopy(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateDiskCopyStatement) {
    m_updateDiskCopyStatement = createStatement(s_updateDiskCopyStatementString);
  }
  // Update local object
  m_updateDiskCopyStatement->setUInt64(1, 0 == obj->diskcopy() ? 0 : obj->diskcopy()->id());
  m_updateDiskCopyStatement->setUInt64(2, obj->id());
  m_updateDiskCopyStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepCastorFile
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillRepCastorFile(castor::stager::SubRequest* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->castorFile()) {
    // Check checkCastorFileExist statement
    if (0 == m_checkCastorFileExistStatement) {
      m_checkCastorFileExistStatement = createStatement(s_checkCastorFileExistStatementString);
    }
    // retrieve the object from the database
    m_checkCastorFileExistStatement->setUInt64(1, obj->castorFile()->id());
    castor::db::IDbResultSet *rset = m_checkCastorFileExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->castorFile(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateCastorFileStatement) {
    m_updateCastorFileStatement = createStatement(s_updateCastorFileStatementString);
  }
  // Update local object
  m_updateCastorFileStatement->setUInt64(1, 0 == obj->castorFile() ? 0 : obj->castorFile()->id());
  m_updateCastorFileStatement->setUInt64(2, obj->id());
  m_updateCastorFileStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillRepSubRequest(castor::stager::SubRequest* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->parent()) {
    // Check checkSubRequestExist statement
    if (0 == m_checkSubRequestExistStatement) {
      m_checkSubRequestExistStatement = createStatement(s_checkSubRequestExistStatementString);
    }
    // retrieve the object from the database
    m_checkSubRequestExistStatement->setUInt64(1, obj->parent()->id());
    castor::db::IDbResultSet *rset = m_checkSubRequestExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->parent(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateSubRequestStatement) {
    m_updateSubRequestStatement = createStatement(s_updateSubRequestStatementString);
  }
  // Update local object
  m_updateSubRequestStatement->setUInt64(1, 0 == obj->parent() ? 0 : obj->parent()->id());
  m_updateSubRequestStatement->setUInt64(2, obj->id());
  m_updateSubRequestStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepFileRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillRepFileRequest(castor::stager::SubRequest* obj)
  throw (castor::exception::Exception) {
  // Check update statement
  if (0 == m_updateFileRequestStatement) {
    m_updateFileRequestStatement = createStatement(s_updateFileRequestStatementString);
  }
  // Update local object
  m_updateFileRequestStatement->setUInt64(1, 0 == obj->request() ? 0 : obj->request()->id());
  m_updateFileRequestStatement->setUInt64(2, obj->id());
  m_updateFileRequestStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillObj(castor::IAddress* address,
                                               castor::IObject* object,
                                               unsigned int type,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::SubRequest* obj = 
    dynamic_cast<castor::stager::SubRequest*>(object);
  switch (type) {
  case castor::OBJ_DiskCopy :
    fillObjDiskCopy(obj);
    break;
  case castor::OBJ_CastorFile :
    fillObjCastorFile(obj);
    break;
  case castor::OBJ_SubRequest :
    fillObjSubRequest(obj);
    break;
  case castor::OBJ_FileRequest :
    fillObjFileRequest(obj);
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
// fillObjDiskCopy
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillObjDiskCopy(castor::stager::SubRequest* obj)
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
  u_signed64 diskcopyId = rset->getInt64(13);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->diskcopy() &&
      (0 == diskcopyId ||
       obj->diskcopy()->id() != diskcopyId)) {
    obj->diskcopy()->removeSubRequests(obj);
    obj->setDiskcopy(0);
  }
  // Update object or create new one
  if (0 != diskcopyId) {
    if (0 == obj->diskcopy()) {
      obj->setDiskcopy
        (dynamic_cast<castor::stager::DiskCopy*>
         (cnvSvc()->getObjFromId(diskcopyId)));
    } else {
      cnvSvc()->updateObj(obj->diskcopy());
    }
    obj->diskcopy()->addSubRequests(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjCastorFile
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillObjCastorFile(castor::stager::SubRequest* obj)
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
  u_signed64 castorFileId = rset->getInt64(14);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->castorFile() &&
      (0 == castorFileId ||
       obj->castorFile()->id() != castorFileId)) {
    obj->setCastorFile(0);
  }
  // Update object or create new one
  if (0 != castorFileId) {
    if (0 == obj->castorFile()) {
      obj->setCastorFile
        (dynamic_cast<castor::stager::CastorFile*>
         (cnvSvc()->getObjFromId(castorFileId)));
    } else {
      cnvSvc()->updateObj(obj->castorFile());
    }
  }
}

//------------------------------------------------------------------------------
// fillObjSubRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillObjSubRequest(castor::stager::SubRequest* obj)
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
  u_signed64 parentId = rset->getInt64(15);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->parent() &&
      (0 == parentId ||
       obj->parent()->id() != parentId)) {
    obj->parent()->removeChild(obj);
    obj->setParent(0);
  }
  // Update object or create new one
  if (0 != parentId) {
    if (0 == obj->parent()) {
      obj->setParent
        (dynamic_cast<castor::stager::SubRequest*>
         (cnvSvc()->getObjFromId(parentId)));
    } else {
      cnvSvc()->updateObj(obj->parent());
    }
    obj->parent()->addChild(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjFileRequest
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::fillObjFileRequest(castor::stager::SubRequest* obj)
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
  u_signed64 requestId = rset->getInt64(17);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->request() &&
      (0 == requestId ||
       obj->request()->id() != requestId)) {
    obj->request()->removeSubRequests(obj);
    obj->setRequest(0);
  }
  // Update object or create new one
  if (0 != requestId) {
    if (0 == obj->request()) {
      obj->setRequest
        (dynamic_cast<castor::stager::FileRequest*>
         (cnvSvc()->getObjFromId(requestId)));
    } else {
      cnvSvc()->updateObj(obj->request());
    }
    obj->request()->addSubRequests(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::createRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit,
                                                 unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::SubRequest* obj = 
    dynamic_cast<castor::stager::SubRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(18, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->retryCounter());
    m_insertStatement->setString(2, obj->fileName());
    m_insertStatement->setString(3, obj->protocol());
    m_insertStatement->setUInt64(4, obj->xsize());
    m_insertStatement->setInt(5, obj->priority());
    m_insertStatement->setString(6, obj->subreqId());
    m_insertStatement->setInt(7, obj->flags());
    m_insertStatement->setInt(8, obj->modeBits());
    m_insertStatement->setInt(9, time(0));
    m_insertStatement->setInt(10, time(0));
    m_insertStatement->setInt(11, obj->answered());
    m_insertStatement->setUInt64(12, (type == OBJ_DiskCopy && obj->diskcopy() != 0) ? obj->diskcopy()->id() : 0);
    m_insertStatement->setUInt64(13, (type == OBJ_CastorFile && obj->castorFile() != 0) ? obj->castorFile()->id() : 0);
    m_insertStatement->setUInt64(14, (type == OBJ_SubRequest && obj->parent() != 0) ? obj->parent()->id() : 0);
    m_insertStatement->setInt(15, (int)obj->status());
    m_insertStatement->setUInt64(16, (type == OBJ_FileRequest && obj->request() != 0) ? obj->request()->id() : 0);
    m_insertStatement->setInt(17, (int)obj->getNextStatus());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(18));
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
                    << "  retryCounter : " << obj->retryCounter() << std::endl
                    << "  fileName : " << obj->fileName() << std::endl
                    << "  protocol : " << obj->protocol() << std::endl
                    << "  xsize : " << obj->xsize() << std::endl
                    << "  priority : " << obj->priority() << std::endl
                    << "  subreqId : " << obj->subreqId() << std::endl
                    << "  flags : " << obj->flags() << std::endl
                    << "  modeBits : " << obj->modeBits() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  lastModificationTime : " << obj->lastModificationTime() << std::endl
                    << "  answered : " << obj->answered() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  diskcopy : " << obj->diskcopy() << std::endl
                    << "  castorFile : " << obj->castorFile() << std::endl
                    << "  parent : " << obj->parent() << std::endl
                    << "  status : " << obj->status() << std::endl
                    << "  request : " << obj->request() << std::endl
                    << "  getNextStatus : " << obj->getNextStatus() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbSubRequestCnv::updateRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::SubRequest* obj = 
    dynamic_cast<castor::stager::SubRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->retryCounter());
    m_updateStatement->setString(2, obj->fileName());
    m_updateStatement->setString(3, obj->protocol());
    m_updateStatement->setUInt64(4, obj->xsize());
    m_updateStatement->setInt(5, obj->priority());
    m_updateStatement->setString(6, obj->subreqId());
    m_updateStatement->setInt(7, obj->flags());
    m_updateStatement->setInt(8, obj->modeBits());
    m_updateStatement->setInt(9, time(0));
    m_updateStatement->setInt(10, obj->answered());
    m_updateStatement->setInt(11, (int)obj->status());
    m_updateStatement->setInt(12, (int)obj->getNextStatus());
    m_updateStatement->setUInt64(13, obj->id());
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
void castor::db::cnv::DbSubRequestCnv::deleteRep(castor::IAddress* address,
                                                 castor::IObject* object,
                                                 bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::SubRequest* obj = 
    dynamic_cast<castor::stager::SubRequest*>(object);
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
castor::IObject* castor::db::cnv::DbSubRequestCnv::createObj(castor::IAddress* address)
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
    castor::stager::SubRequest* object = new castor::stager::SubRequest();
    // Now retrieve and set members
    object->setRetryCounter(rset->getInt(1));
    object->setFileName(rset->getString(2));
    object->setProtocol(rset->getString(3));
    object->setXsize(rset->getUInt64(4));
    object->setPriority(rset->getInt(5));
    object->setSubreqId(rset->getString(6));
    object->setFlags(rset->getInt(7));
    object->setModeBits(rset->getInt(8));
    object->setCreationTime(rset->getUInt64(9));
    object->setLastModificationTime(rset->getUInt64(10));
    object->setAnswered(rset->getInt(11));
    object->setId(rset->getUInt64(12));
    object->setStatus((enum castor::stager::SubRequestStatusCodes)rset->getInt(16));
    object->setGetNextStatus((enum castor::stager::SubRequestGetNextStatusCodes)rset->getInt(18));
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
void castor::db::cnv::DbSubRequestCnv::updateObj(castor::IObject* obj)
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
    castor::stager::SubRequest* object = 
      dynamic_cast<castor::stager::SubRequest*>(obj);
    object->setRetryCounter(rset->getInt(1));
    object->setFileName(rset->getString(2));
    object->setProtocol(rset->getString(3));
    object->setXsize(rset->getUInt64(4));
    object->setPriority(rset->getInt(5));
    object->setSubreqId(rset->getString(6));
    object->setFlags(rset->getInt(7));
    object->setModeBits(rset->getInt(8));
    object->setCreationTime(rset->getUInt64(9));
    object->setLastModificationTime(rset->getUInt64(10));
    object->setAnswered(rset->getInt(11));
    object->setId(rset->getUInt64(12));
    object->setStatus((enum castor::stager::SubRequestStatusCodes)rset->getInt(16));
    object->setGetNextStatus((enum castor::stager::SubRequestGetNextStatusCodes)rset->getInt(18));
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


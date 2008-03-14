/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbNsFilesDeletedCnv.cpp
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
#include "DbNsFilesDeletedCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/GCFile.hpp"
#include "castor/stager/NsFilesDeleted.hpp"
#include "castor/stager/SvcClass.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbNsFilesDeletedCnv>* s_factoryDbNsFilesDeletedCnv =
  new castor::CnvFactory<castor::db::cnv::DbNsFilesDeletedCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_insertStatementString =
"INSERT INTO NsFilesDeleted (flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, nsHost, id, svcClass, client) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,ids_seq.nextval,:14,:15) RETURNING id INTO :16";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_deleteStatementString =
"DELETE FROM NsFilesDeleted WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_selectStatementString =
"SELECT flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, nsHost, id, svcClass, client FROM NsFilesDeleted WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_updateStatementString =
"UPDATE NsFilesDeleted SET flags = :1, userName = :2, euid = :3, egid = :4, mask = :5, pid = :6, machine = :7, svcClassName = :8, userTag = :9, reqId = :10, lastModificationTime = :11, nsHost = :12 WHERE id = :13";

/// SQL statement for type storage
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL statement for request insertion into newRequests table
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_insertNewReqStatementString =
"INSERT INTO newRequests (id, type, creation) VALUES (:1, :2, SYSDATE)";

/// SQL select statement for member files
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_selectGCFileStatementString =
"SELECT id FROM GCFile WHERE request = :1 FOR UPDATE";

/// SQL delete statement for member files
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_deleteGCFileStatementString =
"UPDATE GCFile SET request = 0 WHERE id = :1";

/// SQL remote update statement for member files
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_remoteUpdateGCFileStatementString =
"UPDATE GCFile SET request = :1 WHERE id = :2";

/// SQL existence statement for member svcClass
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_checkSvcClassExistStatementString =
"SELECT id FROM SvcClass WHERE id = :1";

/// SQL update statement for member svcClass
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_updateSvcClassStatementString =
"UPDATE NsFilesDeleted SET svcClass = :1 WHERE id = :2";

/// SQL update statement for member client
const std::string castor::db::cnv::DbNsFilesDeletedCnv::s_updateIClientStatementString =
"UPDATE NsFilesDeleted SET client = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbNsFilesDeletedCnv::DbNsFilesDeletedCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_insertNewReqStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectGCFileStatement(0),
  m_deleteGCFileStatement(0),
  m_remoteUpdateGCFileStatement(0),
  m_checkSvcClassExistStatement(0),
  m_updateSvcClassStatement(0),
  m_updateIClientStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbNsFilesDeletedCnv::~DbNsFilesDeletedCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_insertNewReqStatement) delete m_insertNewReqStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_deleteGCFileStatement) delete m_deleteGCFileStatement;
    if(m_selectGCFileStatement) delete m_selectGCFileStatement;
    if(m_remoteUpdateGCFileStatement) delete m_remoteUpdateGCFileStatement;
    if(m_checkSvcClassExistStatement) delete m_checkSvcClassExistStatement;
    if(m_updateSvcClassStatement) delete m_updateSvcClassStatement;
    if(m_updateIClientStatement) delete m_updateIClientStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_insertNewReqStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectGCFileStatement = 0;
  m_deleteGCFileStatement = 0;
  m_remoteUpdateGCFileStatement = 0;
  m_checkSvcClassExistStatement = 0;
  m_updateSvcClassStatement = 0;
  m_updateIClientStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbNsFilesDeletedCnv::ObjType() {
  return castor::stager::NsFilesDeleted::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbNsFilesDeletedCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::fillRep(castor::IAddress* address,
                                                   castor::IObject* object,
                                                   unsigned int type,
                                                   bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::NsFilesDeleted* obj = 
    dynamic_cast<castor::stager::NsFilesDeleted*>(object);
  try {
    switch (type) {
    case castor::OBJ_GCFile :
      fillRepGCFile(obj);
      break;
    case castor::OBJ_SvcClass :
      fillRepSvcClass(obj);
      break;
    case castor::OBJ_IClient :
      fillRepIClient(obj);
      break;
    default :
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "fillRep called for type " << type 
                      << " on object of type " << obj->type() 
                      << ". This is meaningless.";
      throw ex;
    }
    if (endTransaction) {
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
// fillRepGCFile
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::fillRepGCFile(castor::stager::NsFilesDeleted* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectGCFileStatement) {
    m_selectGCFileStatement = createStatement(s_selectGCFileStatementString);
  }
  // Get current database data
  std::set<int> filesList;
  m_selectGCFileStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectGCFileStatement->executeQuery();
  while (rset->next()) {
    filesList.insert(rset->getInt(1));
  }
  delete rset;
  // update files and create new ones
  for (std::vector<castor::stager::GCFile*>::iterator it = obj->files().begin();
       it != obj->files().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false, OBJ_GCFileList);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateGCFileStatement) {
        m_remoteUpdateGCFileStatement = createStatement(s_remoteUpdateGCFileStatementString);
      }
      // Update remote object
      m_remoteUpdateGCFileStatement->setUInt64(1, obj->id());
      m_remoteUpdateGCFileStatement->setUInt64(2, (*it)->id());
      m_remoteUpdateGCFileStatement->execute();
      std::set<int>::iterator item;
      if ((item = filesList.find((*it)->id())) != filesList.end()) {
        filesList.erase(item);
      }
    }
  }
  // Delete old links
  for (std::set<int>::iterator it = filesList.begin();
       it != filesList.end();
       it++) {
    if (0 == m_deleteGCFileStatement) {
      m_deleteGCFileStatement = createStatement(s_deleteGCFileStatementString);
    }
    m_deleteGCFileStatement->setUInt64(1, *it);
    m_deleteGCFileStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepSvcClass
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::fillRepSvcClass(castor::stager::NsFilesDeleted* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->svcClass()) {
    // Check checkSvcClassExist statement
    if (0 == m_checkSvcClassExistStatement) {
      m_checkSvcClassExistStatement = createStatement(s_checkSvcClassExistStatementString);
    }
    // retrieve the object from the database
    m_checkSvcClassExistStatement->setUInt64(1, obj->svcClass()->id());
    castor::db::IDbResultSet *rset = m_checkSvcClassExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->svcClass(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateSvcClassStatement) {
    m_updateSvcClassStatement = createStatement(s_updateSvcClassStatementString);
  }
  // Update local object
  m_updateSvcClassStatement->setUInt64(1, 0 == obj->svcClass() ? 0 : obj->svcClass()->id());
  m_updateSvcClassStatement->setUInt64(2, obj->id());
  m_updateSvcClassStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepIClient
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::fillRepIClient(castor::stager::NsFilesDeleted* obj)
  throw (castor::exception::Exception) {
  // Check update statement
  if (0 == m_updateIClientStatement) {
    m_updateIClientStatement = createStatement(s_updateIClientStatementString);
  }
  // Update local object
  m_updateIClientStatement->setUInt64(1, 0 == obj->client() ? 0 : obj->client()->id());
  m_updateIClientStatement->setUInt64(2, obj->id());
  m_updateIClientStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::fillObj(castor::IAddress* address,
                                                   castor::IObject* object,
                                                   unsigned int type,
                                                   bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::NsFilesDeleted* obj = 
    dynamic_cast<castor::stager::NsFilesDeleted*>(object);
  switch (type) {
  case castor::OBJ_GCFile :
    fillObjGCFile(obj);
    break;
  case castor::OBJ_SvcClass :
    fillObjSvcClass(obj);
    break;
  case castor::OBJ_IClient :
    fillObjIClient(obj);
    break;
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillObj called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
  if (endTransaction) {
    cnvSvc()->commit();
  }
}
//------------------------------------------------------------------------------
// fillObjGCFile
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::fillObjGCFile(castor::stager::NsFilesDeleted* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectGCFileStatement) {
    m_selectGCFileStatement = createStatement(s_selectGCFileStatementString);
  }
  // retrieve the object from the database
  std::set<int> filesList;
  m_selectGCFileStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectGCFileStatement->executeQuery();
  while (rset->next()) {
    filesList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::GCFile*> toBeDeleted;
  for (std::vector<castor::stager::GCFile*>::iterator it = obj->files().begin();
       it != obj->files().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = filesList.find((*it)->id())) == filesList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      filesList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::GCFile*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeFiles(*it);
    (*it)->setRequest(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = filesList.begin();
       it != filesList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::GCFile* remoteObj = 
      dynamic_cast<castor::stager::GCFile*>(item);
    obj->addFiles(remoteObj);
    remoteObj->setRequest(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjSvcClass
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::fillObjSvcClass(castor::stager::NsFilesDeleted* obj)
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
  u_signed64 svcClassId = rset->getInt64(15);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->svcClass() &&
      (0 == svcClassId ||
       obj->svcClass()->id() != svcClassId)) {
    obj->setSvcClass(0);
  }
  // Update object or create new one
  if (0 != svcClassId) {
    if (0 == obj->svcClass()) {
      obj->setSvcClass
        (dynamic_cast<castor::stager::SvcClass*>
         (cnvSvc()->getObjFromId(svcClassId)));
    } else {
      cnvSvc()->updateObj(obj->svcClass());
    }
  }
}

//------------------------------------------------------------------------------
// fillObjIClient
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::fillObjIClient(castor::stager::NsFilesDeleted* obj)
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
  u_signed64 clientId = rset->getInt64(16);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->client() &&
      (0 == clientId ||
       obj->client()->id() != clientId)) {
    obj->setClient(0);
  }
  // Update object or create new one
  if (0 != clientId) {
    if (0 == obj->client()) {
      obj->setClient
        (dynamic_cast<castor::IClient*>
         (cnvSvc()->getObjFromId(clientId)));
    } else {
      cnvSvc()->updateObj(obj->client());
    }
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::createRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     bool endTransaction,
                                                     unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::NsFilesDeleted* obj = 
    dynamic_cast<castor::stager::NsFilesDeleted*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(16, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_insertNewReqStatement) {
      m_insertNewReqStatement = createStatement(s_insertNewReqStatementString);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setUInt64(1, obj->flags());
    m_insertStatement->setString(2, obj->userName());
    m_insertStatement->setInt(3, obj->euid());
    m_insertStatement->setInt(4, obj->egid());
    m_insertStatement->setInt(5, obj->mask());
    m_insertStatement->setInt(6, obj->pid());
    m_insertStatement->setString(7, obj->machine());
    m_insertStatement->setString(8, obj->svcClassName());
    m_insertStatement->setString(9, obj->userTag());
    m_insertStatement->setString(10, obj->reqId());
    m_insertStatement->setInt(11, time(0));
    m_insertStatement->setInt(12, time(0));
    m_insertStatement->setString(13, obj->nsHost());
    m_insertStatement->setUInt64(14, (type == OBJ_SvcClass && obj->svcClass() != 0) ? obj->svcClass()->id() : 0);
    m_insertStatement->setUInt64(15, (type == OBJ_IClient && obj->client() != 0) ? obj->client()->id() : 0);
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(16));
    m_storeTypeStatement->setUInt64(1, obj->id());
    m_storeTypeStatement->setUInt64(2, obj->type());
    m_storeTypeStatement->execute();
    m_insertNewReqStatement->setUInt64(1, obj->id());
    m_insertNewReqStatement->setUInt64(2, obj->type());
    m_insertNewReqStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { if (endTransaction) cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  flags : " << obj->flags() << std::endl
                    << "  userName : " << obj->userName() << std::endl
                    << "  euid : " << obj->euid() << std::endl
                    << "  egid : " << obj->egid() << std::endl
                    << "  mask : " << obj->mask() << std::endl
                    << "  pid : " << obj->pid() << std::endl
                    << "  machine : " << obj->machine() << std::endl
                    << "  svcClassName : " << obj->svcClassName() << std::endl
                    << "  userTag : " << obj->userTag() << std::endl
                    << "  reqId : " << obj->reqId() << std::endl
                    << "  creationTime : " << obj->creationTime() << std::endl
                    << "  lastModificationTime : " << obj->lastModificationTime() << std::endl
                    << "  nsHost : " << obj->nsHost() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  svcClass : " << obj->svcClass() << std::endl
                    << "  client : " << obj->client() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbNsFilesDeletedCnv::updateRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::NsFilesDeleted* obj = 
    dynamic_cast<castor::stager::NsFilesDeleted*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setUInt64(1, obj->flags());
    m_updateStatement->setString(2, obj->userName());
    m_updateStatement->setInt(3, obj->euid());
    m_updateStatement->setInt(4, obj->egid());
    m_updateStatement->setInt(5, obj->mask());
    m_updateStatement->setInt(6, obj->pid());
    m_updateStatement->setString(7, obj->machine());
    m_updateStatement->setString(8, obj->svcClassName());
    m_updateStatement->setString(9, obj->userTag());
    m_updateStatement->setString(10, obj->reqId());
    m_updateStatement->setInt(11, time(0));
    m_updateStatement->setString(12, obj->nsHost());
    m_updateStatement->setUInt64(13, obj->id());
    m_updateStatement->execute();
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { if (endTransaction) cnvSvc()->rollback(); }
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
void castor::db::cnv::DbNsFilesDeletedCnv::deleteRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::NsFilesDeleted* obj = 
    dynamic_cast<castor::stager::NsFilesDeleted*>(object);
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
    for (std::vector<castor::stager::GCFile*>::iterator it = obj->files().begin();
         it != obj->files().end();
         it++) {
      cnvSvc()->deleteRep(0, *it, false);
    }
    if (obj->client() != 0) {
      cnvSvc()->deleteRep(0, obj->client(), false);
    }
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { if (endTransaction) cnvSvc()->rollback(); }
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
castor::IObject* castor::db::cnv::DbNsFilesDeletedCnv::createObj(castor::IAddress* address)
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
    castor::stager::NsFilesDeleted* object = new castor::stager::NsFilesDeleted();
    // Now retrieve and set members
    object->setFlags(rset->getUInt64(1));
    object->setUserName(rset->getString(2));
    object->setEuid(rset->getInt(3));
    object->setEgid(rset->getInt(4));
    object->setMask(rset->getInt(5));
    object->setPid(rset->getInt(6));
    object->setMachine(rset->getString(7));
    object->setSvcClassName(rset->getString(8));
    object->setUserTag(rset->getString(9));
    object->setReqId(rset->getString(10));
    object->setCreationTime(rset->getUInt64(11));
    object->setLastModificationTime(rset->getUInt64(12));
    object->setNsHost(rset->getString(13));
    object->setId(rset->getUInt64(14));
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
void castor::db::cnv::DbNsFilesDeletedCnv::updateObj(castor::IObject* obj)
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
    castor::stager::NsFilesDeleted* object = 
      dynamic_cast<castor::stager::NsFilesDeleted*>(obj);
    object->setFlags(rset->getUInt64(1));
    object->setUserName(rset->getString(2));
    object->setEuid(rset->getInt(3));
    object->setEgid(rset->getInt(4));
    object->setMask(rset->getInt(5));
    object->setPid(rset->getInt(6));
    object->setMachine(rset->getString(7));
    object->setSvcClassName(rset->getString(8));
    object->setUserTag(rset->getString(9));
    object->setReqId(rset->getString(10));
    object->setCreationTime(rset->getUInt64(11));
    object->setLastModificationTime(rset->getUInt64(12));
    object->setNsHost(rset->getString(13));
    object->setId(rset->getUInt64(14));
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


/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbTapeCopyCnv.cpp
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
#include "DbTapeCopyCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/VectorAddress.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/Segment.hpp"
#include "castor/stager/Stream.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "castor/stager/TapeCopyStatusCodes.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbTapeCopyCnv>* s_factoryDbTapeCopyCnv =
  new castor::CnvFactory<castor::db::cnv::DbTapeCopyCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbTapeCopyCnv::s_insertStatementString =
"INSERT INTO TapeCopy (copyNb, id, castorFile, status) VALUES (:1,ids_seq.nextval,:2,:3) RETURNING id INTO :4";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbTapeCopyCnv::s_deleteStatementString =
"DELETE FROM TapeCopy WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbTapeCopyCnv::s_selectStatementString =
"SELECT copyNb, id, castorFile, status FROM TapeCopy WHERE id = :1";

/// SQL statement for bulk request selection
const std::string castor::db::cnv::DbTapeCopyCnv::s_bulkSelectStatementString =
"DECLARE \
   TYPE CurType IS REF CURSOR RETURN TapeCopy%ROWTYPE; \
   PROCEDURE bulkSelect(ids IN castor.\"cnumList\", \
                        objs OUT CurType) AS \
   BEGIN \
     FORALL i IN ids.FIRST..ids.LAST \
       INSERT INTO bulkSelectHelper VALUES(ids(i)); \
     OPEN objs FOR SELECT copyNb, id, castorFile, status \
                     FROM TapeCopy t, bulkSelectHelper h \
                    WHERE t.id = h.objId; \
     DELETE FROM bulkSelectHelper; \
   END; \
 BEGIN \
   bulkSelect(:1, :2); \
 END;";

/// SQL statement for request update
const std::string castor::db::cnv::DbTapeCopyCnv::s_updateStatementString =
"UPDATE TapeCopy SET copyNb = :1, status = :2 WHERE id = :3";

/// SQL statement for type storage
const std::string castor::db::cnv::DbTapeCopyCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbTapeCopyCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL insert statement for member stream
const std::string castor::db::cnv::DbTapeCopyCnv::s_insertStreamStatementString =
"INSERT INTO Stream2TapeCopy (Child, Parent) VALUES (:1, :2)";

/// SQL delete statement for member stream
const std::string castor::db::cnv::DbTapeCopyCnv::s_deleteStreamStatementString =
"DELETE FROM Stream2TapeCopy WHERE Child = :1 AND Parent = :2";

/// SQL select statement for member stream
// The FOR UPDATE is needed in order to avoid deletion
// of a segment after listing and before update/remove
const std::string castor::db::cnv::DbTapeCopyCnv::s_selectStreamStatementString =
"SELECT Parent FROM Stream2TapeCopy WHERE Child = :1 FOR UPDATE";

/// SQL select statement for member segments
const std::string castor::db::cnv::DbTapeCopyCnv::s_selectSegmentStatementString =
"SELECT id FROM Segment WHERE copy = :1 FOR UPDATE";

/// SQL delete statement for member segments
const std::string castor::db::cnv::DbTapeCopyCnv::s_deleteSegmentStatementString =
"UPDATE Segment SET copy = 0 WHERE id = :1";

/// SQL remote update statement for member segments
const std::string castor::db::cnv::DbTapeCopyCnv::s_remoteUpdateSegmentStatementString =
"UPDATE Segment SET copy = :1 WHERE id = :2";

/// SQL existence statement for member castorFile
const std::string castor::db::cnv::DbTapeCopyCnv::s_checkCastorFileExistStatementString =
"SELECT id FROM CastorFile WHERE id = :1";

/// SQL update statement for member castorFile
const std::string castor::db::cnv::DbTapeCopyCnv::s_updateCastorFileStatementString =
"UPDATE TapeCopy SET castorFile = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeCopyCnv::DbTapeCopyCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_bulkSelectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_insertStreamStatement(0),
  m_deleteStreamStatement(0),
  m_selectStreamStatement(0),
  m_selectSegmentStatement(0),
  m_deleteSegmentStatement(0),
  m_remoteUpdateSegmentStatement(0),
  m_checkCastorFileExistStatement(0),
  m_updateCastorFileStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeCopyCnv::~DbTapeCopyCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_bulkSelectStatement) delete m_bulkSelectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_insertStreamStatement) delete m_insertStreamStatement;
    if(m_deleteStreamStatement) delete m_deleteStreamStatement;
    if(m_selectStreamStatement) delete m_selectStreamStatement;
    if(m_deleteSegmentStatement) delete m_deleteSegmentStatement;
    if(m_selectSegmentStatement) delete m_selectSegmentStatement;
    if(m_remoteUpdateSegmentStatement) delete m_remoteUpdateSegmentStatement;
    if(m_checkCastorFileExistStatement) delete m_checkCastorFileExistStatement;
    if(m_updateCastorFileStatement) delete m_updateCastorFileStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_bulkSelectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_insertStreamStatement = 0;
  m_deleteStreamStatement = 0;
  m_selectStreamStatement = 0;
  m_selectSegmentStatement = 0;
  m_deleteSegmentStatement = 0;
  m_remoteUpdateSegmentStatement = 0;
  m_checkCastorFileExistStatement = 0;
  m_updateCastorFileStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeCopyCnv::ObjType() {
  return castor::stager::TapeCopy::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeCopyCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::fillRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::TapeCopy* obj = 
    dynamic_cast<castor::stager::TapeCopy*>(object);
  try {
    switch (type) {
    case castor::OBJ_Stream :
      fillRepStream(obj);
      break;
    case castor::OBJ_Segment :
      fillRepSegment(obj);
      break;
    case castor::OBJ_CastorFile :
      fillRepCastorFile(obj);
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
// fillRepStream
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::fillRepStream(castor::stager::TapeCopy* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectStreamStatement) {
    m_selectStreamStatement = createStatement(s_selectStreamStatementString);
  }
  // Get current database data
  std::set<u_signed64> streamList;
  m_selectStreamStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStreamStatement->executeQuery();
  while (rset->next()) {
    streamList.insert(rset->getUInt64(1));
  }
  delete rset;
  // update stream and create new ones
  for (std::vector<castor::stager::Stream*>::iterator it = obj->stream().begin();
       it != obj->stream().end();
       it++) {
    if (0 == (*it)->id()) {
      cnvSvc()->createRep(0, *it, false);
    }
    std::set<u_signed64>::iterator item;
    if ((item = streamList.find((*it)->id())) != streamList.end()) {
      streamList.erase(item);
    } else {
      if (0 == m_insertStreamStatement) {
        m_insertStreamStatement = createStatement(s_insertStreamStatementString);
      }
      m_insertStreamStatement->setUInt64(1, obj->id());
      m_insertStreamStatement->setUInt64(2, (*it)->id());
      m_insertStreamStatement->execute();
    }
  }
  // Delete old links
  for (std::set<u_signed64>::iterator it = streamList.begin();
       it != streamList.end();
       it++) {
    if (0 == m_deleteStreamStatement) {
      m_deleteStreamStatement = createStatement(s_deleteStreamStatementString);
    }
    m_deleteStreamStatement->setUInt64(1, obj->id());
    m_deleteStreamStatement->setUInt64(2, *it);
    m_deleteStreamStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepSegment
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::fillRepSegment(castor::stager::TapeCopy* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectSegmentStatement) {
    m_selectSegmentStatement = createStatement(s_selectSegmentStatementString);
  }
  // Get current database data
  std::set<u_signed64> segmentsList;
  m_selectSegmentStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectSegmentStatement->executeQuery();
  while (rset->next()) {
    segmentsList.insert(rset->getUInt64(1));
  }
  delete rset;
  // update segments and create new ones
  std::vector<castor::IObject*> toBeCreated;
  for (std::vector<castor::stager::Segment*>::iterator it = obj->segments().begin();
       it != obj->segments().end();
       it++) {
    if (0 == (*it)->id()) {
      toBeCreated.push_back(*it);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateSegmentStatement) {
        m_remoteUpdateSegmentStatement = createStatement(s_remoteUpdateSegmentStatementString);
      }
      // Update remote object
      m_remoteUpdateSegmentStatement->setUInt64(1, obj->id());
      m_remoteUpdateSegmentStatement->setUInt64(2, (*it)->id());
      m_remoteUpdateSegmentStatement->execute();
      std::set<u_signed64>::iterator item;
      if ((item = segmentsList.find((*it)->id())) != segmentsList.end()) {
        segmentsList.erase(item);
      }
    }
  }
  // create new objects
  cnvSvc()->bulkCreateRep(0, toBeCreated, false, OBJ_TapeCopy);
  // Delete old links
  for (std::set<u_signed64>::iterator it = segmentsList.begin();
       it != segmentsList.end();
       it++) {
    if (0 == m_deleteSegmentStatement) {
      m_deleteSegmentStatement = createStatement(s_deleteSegmentStatementString);
    }
    m_deleteSegmentStatement->setUInt64(1, *it);
    m_deleteSegmentStatement->execute();
  }
}

//------------------------------------------------------------------------------
// fillRepCastorFile
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::fillRepCastorFile(castor::stager::TapeCopy* obj)
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
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::fillObj(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::TapeCopy* obj = 
    dynamic_cast<castor::stager::TapeCopy*>(object);
  switch (type) {
  case castor::OBJ_Stream :
    fillObjStream(obj);
    break;
  case castor::OBJ_Segment :
    fillObjSegment(obj);
    break;
  case castor::OBJ_CastorFile :
    fillObjCastorFile(obj);
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
// fillObjStream
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::fillObjStream(castor::stager::TapeCopy* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectStreamStatement) {
    m_selectStreamStatement = createStatement(s_selectStreamStatementString);
  }
  // retrieve the object from the database
  std::vector<u_signed64> streamList;
  m_selectStreamStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStreamStatement->executeQuery();
  while (rset->next()) {
    streamList.push_back(rset->getUInt64(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::Stream*> toBeDeleted;
  for (std::vector<castor::stager::Stream*>::iterator it = obj->stream().begin();
       it != obj->stream().end();
       it++) {
    std::vector<u_signed64>::iterator item =
      std::find(streamList.begin(), streamList.end(), (*it)->id());
    if (item == streamList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      streamList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::Stream*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeStream(*it);
    (*it)->removeTapeCopy(obj);
  }
  // Create new objects
  std::vector<castor::IObject*> newStream =
    cnvSvc()->getObjsFromIds(streamList, OBJ_Stream);
  for (std::vector<castor::IObject*>::iterator it = newStream.begin();
       it != newStream.end();
       it++) {
    castor::stager::Stream* remoteObj = 
      dynamic_cast<castor::stager::Stream*>(*it);
    obj->addStream(remoteObj);
    remoteObj->addTapeCopy(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjSegment
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::fillObjSegment(castor::stager::TapeCopy* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectSegmentStatement) {
    m_selectSegmentStatement = createStatement(s_selectSegmentStatementString);
  }
  // retrieve the object from the database
  std::vector<u_signed64> segmentsList;
  m_selectSegmentStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectSegmentStatement->executeQuery();
  while (rset->next()) {
    segmentsList.push_back(rset->getUInt64(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::Segment*> toBeDeleted;
  for (std::vector<castor::stager::Segment*>::iterator it = obj->segments().begin();
       it != obj->segments().end();
       it++) {
    std::vector<u_signed64>::iterator item =
      std::find(segmentsList.begin(), segmentsList.end(), (*it)->id());
    if (item == segmentsList.end()) {
      toBeDeleted.push_back(*it);
    } else {
      segmentsList.erase(item);
      cnvSvc()->updateObj((*it));
    }
  }
  // Delete old objects
  for (std::vector<castor::stager::Segment*>::iterator it = toBeDeleted.begin();
       it != toBeDeleted.end();
       it++) {
    obj->removeSegments(*it);
    (*it)->setCopy(0);
  }
  // Create new objects
  std::vector<castor::IObject*> newSegments =
    cnvSvc()->getObjsFromIds(segmentsList, OBJ_Segment);
  for (std::vector<castor::IObject*>::iterator it = newSegments.begin();
       it != newSegments.end();
       it++) {
    castor::stager::Segment* remoteObj = 
      dynamic_cast<castor::stager::Segment*>(*it);
    obj->addSegments(remoteObj);
    remoteObj->setCopy(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjCastorFile
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::fillObjCastorFile(castor::stager::TapeCopy* obj)
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
  u_signed64 castorFileId = rset->getInt64(3);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->castorFile() &&
      (0 == castorFileId ||
       obj->castorFile()->id() != castorFileId)) {
    obj->castorFile()->removeTapeCopies(obj);
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
    obj->castorFile()->addTapeCopies(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool endTransaction,
                                               unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::TapeCopy* obj = 
    dynamic_cast<castor::stager::TapeCopy*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(4, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->copyNb());
    m_insertStatement->setUInt64(2, (type == OBJ_CastorFile && obj->castorFile() != 0) ? obj->castorFile()->id() : 0);
    m_insertStatement->setInt(3, (int)obj->status());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(4));
    m_storeTypeStatement->setUInt64(1, obj->id());
    m_storeTypeStatement->setUInt64(2, obj->type());
    m_storeTypeStatement->execute();
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
                    << "Statement was : " << std::endl
                    << s_insertStatementString << std::endl
                    << " and parameters' values were :" << std::endl
                    << "  copyNb : " << obj->copyNb() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  castorFile : " << obj->castorFile() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::bulkCreateRep(castor::IAddress* address,
                                                   std::vector<castor::IObject*> &objects,
                                                   bool endTransaction,
                                                   unsigned int type)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::stager::TapeCopy*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::stager::TapeCopy*>(objects[i]));
  }
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(4, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for copyNb
    int* copyNbBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* copyNbBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      copyNbBuffer[i] = objs[i]->copyNb();
      copyNbBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (1, copyNbBuffer, DBTYPE_INT, sizeof(copyNbBuffer[0]), copyNbBufLens);
    // build the buffers for castorFile
    double* castorFileBuffer = (double*) malloc(nb * sizeof(double));
    unsigned short* castorFileBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      castorFileBuffer[i] = (type == OBJ_CastorFile && objs[i]->castorFile() != 0) ? objs[i]->castorFile()->id() : 0;
      castorFileBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (2, castorFileBuffer, DBTYPE_UINT64, sizeof(castorFileBuffer[0]), castorFileBufLens);
    // build the buffers for status
    int* statusBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* statusBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      statusBuffer[i] = objs[i]->status();
      statusBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (3, statusBuffer, DBTYPE_INT, sizeof(statusBuffer[0]), statusBufLens);
    // build the buffers for returned ids
    double* idBuffer = (double*) calloc(nb, sizeof(double));
    unsigned short* idBufLens = (unsigned short*) calloc(nb, sizeof(unsigned short));
    m_insertStatement->setDataBuffer
      (4, idBuffer, DBTYPE_UINT64, sizeof(double), idBufLens);
    m_insertStatement->execute(nb);
    for (int i = 0; i < nb; i++) {
      objects[i]->setId((u_signed64)idBuffer[i]);
    }
    // release the buffers for copyNb
    free(copyNbBuffer);
    free(copyNbBufLens);
    // release the buffers for castorFile
    free(castorFileBuffer);
    free(castorFileBufLens);
    // release the buffers for status
    free(statusBuffer);
    free(statusBufLens);
    // reuse idBuffer for bulk insertion into Id2Type
    m_storeTypeStatement->setDataBuffer
      (1, idBuffer, DBTYPE_UINT64, sizeof(idBuffer[0]), idBufLens);
    // build the buffers for type
    int* typeBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* typeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      typeBuffer[i] = objs[i]->type();
      typeBufLens[i] = sizeof(int);
    }
    m_storeTypeStatement->setDataBuffer
      (2, typeBuffer, DBTYPE_INT, sizeof(typeBuffer[0]), typeBufLens);
    m_storeTypeStatement->execute(nb);
    // release the buffers for type
    free(typeBuffer);
    free(typeBufLens);
    // release the buffers for returned ids
    free(idBuffer);
    free(idBufLens);
    if (endTransaction) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { if (endTransaction) cnvSvc()->rollback(); }
    catch(castor::exception::Exception ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in bulkInsert request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << " was called in bulk with "
                    << nb << " items." << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::updateRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::TapeCopy* obj = 
    dynamic_cast<castor::stager::TapeCopy*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->copyNb());
    m_updateStatement->setInt(2, (int)obj->status());
    m_updateStatement->setUInt64(3, obj->id());
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
                    << "Statement was : " << std::endl
                    << s_updateStatementString << std::endl
                    << " and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::deleteRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::TapeCopy* obj = 
    dynamic_cast<castor::stager::TapeCopy*>(object);
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
    for (std::vector<castor::stager::Segment*>::iterator it = obj->segments().begin();
         it != obj->segments().end();
         it++) {
      cnvSvc()->deleteRep(0, *it, false);
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
                    << "Statement was : " << std::endl
                    << s_deleteStatementString << std::endl
                    << " and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::db::cnv::DbTapeCopyCnv::createObj(castor::IAddress* address)
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
    castor::stager::TapeCopy* object = new castor::stager::TapeCopy();
    // Now retrieve and set members
    object->setCopyNb(rset->getInt(1));
    object->setId(rset->getUInt64(2));
    object->setStatus((enum castor::stager::TapeCopyStatusCodes)rset->getInt(4));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in select request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was : " << std::endl
                    << s_selectStatementString << std::endl
                    << " and id was " << ad->target() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateObj
//------------------------------------------------------------------------------
std::vector<castor::IObject*>
castor::db::cnv::DbTapeCopyCnv::bulkCreateObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  // Prepare result
  std::vector<castor::IObject*> res;
  // check whether something needs to be done
  castor::VectorAddress* ad = 
    dynamic_cast<castor::VectorAddress*>(address);
  int nb = ad->target().size();
  if (0 == nb) return res;
  try {
    // Check whether the statement is ok
    if (0 == m_bulkSelectStatement) {
      m_bulkSelectStatement = createStatement(s_bulkSelectStatementString);
      m_bulkSelectStatement->registerOutParam(2, castor::db::DBTYPE_CURSOR);
    }
    // set the buffer for input ids
    m_bulkSelectStatement->setDataBufferUInt64Array(1, ad->target());
    // Execute statement
    m_bulkSelectStatement->execute();
    // get the result, that is a cursor on the selected rows
    castor::db::IDbResultSet *rset =
      m_bulkSelectStatement->getCursor(2);
    // loop and create the new objects
    bool status = rset->next();
    while (status) {
      // create the new Object
      castor::stager::TapeCopy* object = new castor::stager::TapeCopy();
      // Now retrieve and set members
      object->setCopyNb(rset->getInt(1));
      object->setId(rset->getUInt64(2));
      object->setStatus((enum castor::stager::TapeCopyStatusCodes)rset->getInt(4));
      // store object in results and loop;
      res.push_back(object);
      status = rset->next();
    }
    delete rset;
    return res;
  } catch (castor::exception::SQLError e) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in bulkSelect request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << " was called in bulk with "
                    << nb << " items." << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCopyCnv::updateObj(castor::IObject* obj)
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
    castor::stager::TapeCopy* object = 
      dynamic_cast<castor::stager::TapeCopy*>(obj);
    object->setCopyNb(rset->getInt(1));
    object->setId(rset->getUInt64(2));
    object->setStatus((enum castor::stager::TapeCopyStatusCodes)rset->getInt(4));
    delete rset;
  } catch (castor::exception::SQLError e) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was : " << std::endl
                    << s_updateStatementString << std::endl
                    << " and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


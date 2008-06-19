/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbTapeCnv.cpp
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
#include "DbTapeCnv.hpp"
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
#include "castor/stager/Segment.hpp"
#include "castor/stager/Stream.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/stager/TapeStatusCodes.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbTapeCnv>* s_factoryDbTapeCnv =
  new castor::CnvFactory<castor::db::cnv::DbTapeCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbTapeCnv::s_insertStatementString =
"INSERT INTO Tape (vid, side, tpmode, errMsgTxt, errorCode, severity, vwAddress, id, stream, status) VALUES (:1,:2,:3,:4,:5,:6,:7,ids_seq.nextval,:8,:9) RETURNING id INTO :10";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbTapeCnv::s_deleteStatementString =
"DELETE FROM Tape WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbTapeCnv::s_selectStatementString =
"SELECT vid, side, tpmode, errMsgTxt, errorCode, severity, vwAddress, id, stream, status FROM Tape WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbTapeCnv::s_updateStatementString =
"UPDATE Tape SET vid = :1, side = :2, tpmode = :3, errMsgTxt = :4, errorCode = :5, severity = :6, vwAddress = :7, status = :8 WHERE id = :9";

/// SQL statement for type storage
const std::string castor::db::cnv::DbTapeCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbTapeCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member stream
const std::string castor::db::cnv::DbTapeCnv::s_selectStreamStatementString =
"SELECT id FROM Stream WHERE tape = :1 FOR UPDATE";

/// SQL delete statement for member stream
const std::string castor::db::cnv::DbTapeCnv::s_deleteStreamStatementString =
"UPDATE Stream SET tape = 0 WHERE id = :1";

/// SQL remote update statement for member stream
const std::string castor::db::cnv::DbTapeCnv::s_remoteUpdateStreamStatementString =
"UPDATE Stream SET tape = :1 WHERE id = :2";

/// SQL existence statement for member stream
const std::string castor::db::cnv::DbTapeCnv::s_checkStreamExistStatementString =
"SELECT id FROM Stream WHERE id = :1";

/// SQL update statement for member stream
const std::string castor::db::cnv::DbTapeCnv::s_updateStreamStatementString =
"UPDATE Tape SET stream = :1 WHERE id = :2";

/// SQL select statement for member segments
const std::string castor::db::cnv::DbTapeCnv::s_selectSegmentStatementString =
"SELECT id FROM Segment WHERE tape = :1 FOR UPDATE";

/// SQL delete statement for member segments
const std::string castor::db::cnv::DbTapeCnv::s_deleteSegmentStatementString =
"UPDATE Segment SET tape = 0 WHERE id = :1";

/// SQL remote update statement for member segments
const std::string castor::db::cnv::DbTapeCnv::s_remoteUpdateSegmentStatementString =
"UPDATE Segment SET tape = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeCnv::DbTapeCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_selectStreamStatement(0),
  m_deleteStreamStatement(0),
  m_remoteUpdateStreamStatement(0),
  m_checkStreamExistStatement(0),
  m_updateStreamStatement(0),
  m_selectSegmentStatement(0),
  m_deleteSegmentStatement(0),
  m_remoteUpdateSegmentStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::cnv::DbTapeCnv::~DbTapeCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    if(m_insertStatement) delete m_insertStatement;
    if(m_deleteStatement) delete m_deleteStatement;
    if(m_selectStatement) delete m_selectStatement;
    if(m_updateStatement) delete m_updateStatement;
    if(m_storeTypeStatement) delete m_storeTypeStatement;
    if(m_deleteTypeStatement) delete m_deleteTypeStatement;
    if(m_deleteStreamStatement) delete m_deleteStreamStatement;
    if(m_selectStreamStatement) delete m_selectStreamStatement;
    if(m_remoteUpdateStreamStatement) delete m_remoteUpdateStreamStatement;
    if(m_checkStreamExistStatement) delete m_checkStreamExistStatement;
    if(m_updateStreamStatement) delete m_updateStreamStatement;
    if(m_deleteSegmentStatement) delete m_deleteSegmentStatement;
    if(m_selectSegmentStatement) delete m_selectSegmentStatement;
    if(m_remoteUpdateSegmentStatement) delete m_remoteUpdateSegmentStatement;
  } catch (castor::exception::Exception ignored) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_selectStreamStatement = 0;
  m_deleteStreamStatement = 0;
  m_remoteUpdateStreamStatement = 0;
  m_checkStreamExistStatement = 0;
  m_updateStreamStatement = 0;
  m_selectSegmentStatement = 0;
  m_deleteSegmentStatement = 0;
  m_remoteUpdateSegmentStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeCnv::ObjType() {
  return castor::stager::Tape::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbTapeCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCnv::fillRep(castor::IAddress* address,
                                         castor::IObject* object,
                                         unsigned int type,
                                         bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
  try {
    switch (type) {
    case castor::OBJ_Stream :
      fillRepStream(obj);
      break;
    case castor::OBJ_Segment :
      fillRepSegment(obj);
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
void castor::db::cnv::DbTapeCnv::fillRepStream(castor::stager::Tape* obj)
  throw (castor::exception::Exception) {
  // Check selectStream statement
  if (0 == m_selectStreamStatement) {
    m_selectStreamStatement = createStatement(s_selectStreamStatementString);
  }
  // retrieve the object from the database
  m_selectStreamStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStreamStatement->executeQuery();
  if (rset->next()) {
    u_signed64 streamId = rset->getInt64(1);
    if (0 != streamId &&
        (0 == obj->stream() ||
         obj->stream()->id() != streamId)) {
      if (0 == m_deleteStreamStatement) {
        m_deleteStreamStatement = createStatement(s_deleteStreamStatementString);
      }
      m_deleteStreamStatement->setUInt64(1, streamId);
      m_deleteStreamStatement->execute();
    }
  }
  // Close resultset
  delete rset;
  if (0 != obj->stream()) {
    // Check checkStreamExist statement
    if (0 == m_checkStreamExistStatement) {
      m_checkStreamExistStatement = createStatement(s_checkStreamExistStatementString);
    }
    // retrieve the object from the database
    m_checkStreamExistStatement->setUInt64(1, obj->stream()->id());
    castor::db::IDbResultSet *rset = m_checkStreamExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->stream(), false, OBJ_Tape);
    } else {
      // Check remote update statement
      if (0 == m_remoteUpdateStreamStatement) {
        m_remoteUpdateStreamStatement = createStatement(s_remoteUpdateStreamStatementString);
      }
      // Update remote object
      m_remoteUpdateStreamStatement->setUInt64(1, obj->id());
      m_remoteUpdateStreamStatement->setUInt64(2, obj->stream()->id());
      m_remoteUpdateStreamStatement->execute();
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateStreamStatement) {
    m_updateStreamStatement = createStatement(s_updateStreamStatementString);
  }
  // Update local object
  m_updateStreamStatement->setUInt64(1, 0 == obj->stream() ? 0 : obj->stream()->id());
  m_updateStreamStatement->setUInt64(2, obj->id());
  m_updateStreamStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepSegment
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCnv::fillRepSegment(castor::stager::Tape* obj)
  throw (castor::exception::Exception) {
  // check select statement
  if (0 == m_selectSegmentStatement) {
    m_selectSegmentStatement = createStatement(s_selectSegmentStatementString);
  }
  // Get current database data
  std::set<int> segmentsList;
  m_selectSegmentStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectSegmentStatement->executeQuery();
  while (rset->next()) {
    segmentsList.insert(rset->getInt(1));
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
      std::set<int>::iterator item;
      if ((item = segmentsList.find((*it)->id())) != segmentsList.end()) {
        segmentsList.erase(item);
      }
    }
  }
  // create new objects
  cnvSvc()->bulkCreateRep(0, toBeCreated, false, OBJ_Tape);
  // Delete old links
  for (std::set<int>::iterator it = segmentsList.begin();
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
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCnv::fillObj(castor::IAddress* address,
                                         castor::IObject* object,
                                         unsigned int type,
                                         bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
  switch (type) {
  case castor::OBJ_Stream :
    fillObjStream(obj);
    break;
  case castor::OBJ_Segment :
    fillObjSegment(obj);
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
void castor::db::cnv::DbTapeCnv::fillObjStream(castor::stager::Tape* obj)
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
  u_signed64 streamId = rset->getInt64(9);
  // Close ResultSet
  delete rset;
  // Check whether something should be deleted
  if (0 != obj->stream() &&
      (0 == streamId ||
       obj->stream()->id() != streamId)) {
    obj->stream()->setTape(0);
    obj->setStream(0);
  }
  // Update object or create new one
  if (0 != streamId) {
    if (0 == obj->stream()) {
      obj->setStream
        (dynamic_cast<castor::stager::Stream*>
         (cnvSvc()->getObjFromId(streamId)));
    } else {
      cnvSvc()->updateObj(obj->stream());
    }
    obj->stream()->setTape(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjSegment
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCnv::fillObjSegment(castor::stager::Tape* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectSegmentStatement) {
    m_selectSegmentStatement = createStatement(s_selectSegmentStatementString);
  }
  // retrieve the object from the database
  std::set<int> segmentsList;
  m_selectSegmentStatement->setUInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectSegmentStatement->executeQuery();
  while (rset->next()) {
    segmentsList.insert(rset->getInt(1));
  }
  // Close ResultSet
  delete rset;
  // Update objects and mark old ones for deletion
  std::vector<castor::stager::Segment*> toBeDeleted;
  for (std::vector<castor::stager::Segment*>::iterator it = obj->segments().begin();
       it != obj->segments().end();
       it++) {
    std::set<int>::iterator item;
    if ((item = segmentsList.find((*it)->id())) == segmentsList.end()) {
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
    (*it)->setTape(0);
  }
  // Create new objects
  for (std::set<int>::iterator it = segmentsList.begin();
       it != segmentsList.end();
       it++) {
    castor::IObject* item = cnvSvc()->getObjFromId(*it);
    castor::stager::Segment* remoteObj = 
      dynamic_cast<castor::stager::Segment*>(item);
    obj->addSegments(remoteObj);
    remoteObj->setTape(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCnv::createRep(castor::IAddress* address,
                                           castor::IObject* object,
                                           bool endTransaction,
                                           unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(10, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->vid());
    m_insertStatement->setInt(2, obj->side());
    m_insertStatement->setInt(3, obj->tpmode());
    m_insertStatement->setString(4, obj->errMsgTxt());
    m_insertStatement->setInt(5, obj->errorCode());
    m_insertStatement->setInt(6, obj->severity());
    m_insertStatement->setString(7, obj->vwAddress());
    m_insertStatement->setUInt64(8, (type == OBJ_Stream && obj->stream() != 0) ? obj->stream()->id() : 0);
    m_insertStatement->setInt(9, (int)obj->status());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getUInt64(10));
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
                    << "  vid : " << obj->vid() << std::endl
                    << "  side : " << obj->side() << std::endl
                    << "  tpmode : " << obj->tpmode() << std::endl
                    << "  errMsgTxt : " << obj->errMsgTxt() << std::endl
                    << "  errorCode : " << obj->errorCode() << std::endl
                    << "  severity : " << obj->severity() << std::endl
                    << "  vwAddress : " << obj->vwAddress() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  stream : " << obj->stream() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// bulkCreateRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCnv::bulkCreateRep(castor::IAddress* address,
                                               std::vector<castor::IObject*> &objects,
                                               bool endTransaction,
                                               unsigned int type)
  throw (castor::exception::Exception) {
  // check whether something needs to be done
  int nb = objects.size();
  if (0 == nb) return;
  // Casts all objects
  std::vector<castor::stager::Tape*> objs;
  for (int i = 0; i < nb; i++) {
    objs.push_back(dynamic_cast<castor::stager::Tape*>(objects[i]));
  }
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(10, castor::db::DBTYPE_UINT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // build the buffers for vid
    unsigned int vidMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->vid().length()+1 > vidMaxLen)
        vidMaxLen = objs[i]->vid().length()+1;
    }
    char* vidBuffer = (char*) calloc(nb, vidMaxLen);
    unsigned short* vidBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      strncpy(vidBuffer+(i*vidMaxLen), objs[i]->vid().c_str(), vidMaxLen);
      vidBufLens[i] = objs[i]->vid().length()+1; // + 1 for the trailing \0
    }
    m_insertStatement->setDataBuffer
      (1, vidBuffer, DBTYPE_STRING, vidMaxLen, vidBufLens);
    // build the buffers for side
    int* sideBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* sideBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      sideBuffer[i] = objs[i]->side();
      sideBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (2, sideBuffer, DBTYPE_INT, sizeof(sideBuffer[0]), sideBufLens);
    // build the buffers for tpmode
    int* tpmodeBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* tpmodeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      tpmodeBuffer[i] = objs[i]->tpmode();
      tpmodeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (3, tpmodeBuffer, DBTYPE_INT, sizeof(tpmodeBuffer[0]), tpmodeBufLens);
    // build the buffers for errMsgTxt
    unsigned int errMsgTxtMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->errMsgTxt().length()+1 > errMsgTxtMaxLen)
        errMsgTxtMaxLen = objs[i]->errMsgTxt().length()+1;
    }
    char* errMsgTxtBuffer = (char*) calloc(nb, errMsgTxtMaxLen);
    unsigned short* errMsgTxtBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      strncpy(errMsgTxtBuffer+(i*errMsgTxtMaxLen), objs[i]->errMsgTxt().c_str(), errMsgTxtMaxLen);
      errMsgTxtBufLens[i] = objs[i]->errMsgTxt().length()+1; // + 1 for the trailing \0
    }
    m_insertStatement->setDataBuffer
      (4, errMsgTxtBuffer, DBTYPE_STRING, errMsgTxtMaxLen, errMsgTxtBufLens);
    // build the buffers for errorCode
    int* errorCodeBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* errorCodeBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      errorCodeBuffer[i] = objs[i]->errorCode();
      errorCodeBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (5, errorCodeBuffer, DBTYPE_INT, sizeof(errorCodeBuffer[0]), errorCodeBufLens);
    // build the buffers for severity
    int* severityBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* severityBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      severityBuffer[i] = objs[i]->severity();
      severityBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (6, severityBuffer, DBTYPE_INT, sizeof(severityBuffer[0]), severityBufLens);
    // build the buffers for vwAddress
    unsigned int vwAddressMaxLen = 0;
    for (int i = 0; i < nb; i++) {
      if (objs[i]->vwAddress().length()+1 > vwAddressMaxLen)
        vwAddressMaxLen = objs[i]->vwAddress().length()+1;
    }
    char* vwAddressBuffer = (char*) calloc(nb, vwAddressMaxLen);
    unsigned short* vwAddressBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      strncpy(vwAddressBuffer+(i*vwAddressMaxLen), objs[i]->vwAddress().c_str(), vwAddressMaxLen);
      vwAddressBufLens[i] = objs[i]->vwAddress().length()+1; // + 1 for the trailing \0
    }
    m_insertStatement->setDataBuffer
      (7, vwAddressBuffer, DBTYPE_STRING, vwAddressMaxLen, vwAddressBufLens);
    // build the buffers for stream
    double* streamBuffer = (double*) malloc(nb * sizeof(double));
    unsigned short* streamBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      streamBuffer[i] = (type == OBJ_Stream && objs[i]->stream() != 0) ? objs[i]->stream()->id() : 0;
      streamBufLens[i] = sizeof(double);
    }
    m_insertStatement->setDataBuffer
      (8, streamBuffer, DBTYPE_UINT64, sizeof(streamBuffer[0]), streamBufLens);
    // build the buffers for status
    int* statusBuffer = (int*) malloc(nb * sizeof(int));
    unsigned short* statusBufLens = (unsigned short*) malloc(nb * sizeof(unsigned short));
    for (int i = 0; i < nb; i++) {
      statusBuffer[i] = objs[i]->status();
      statusBufLens[i] = sizeof(int);
    }
    m_insertStatement->setDataBuffer
      (10, statusBuffer, DBTYPE_INT, sizeof(statusBuffer[0]), statusBufLens);
    // build the buffers for returned ids
    double* idBuffer = (double*) calloc(nb, sizeof(double));
    unsigned short* idBufLens = (unsigned short*) calloc(nb, sizeof(unsigned short));
    m_insertStatement->setDataBuffer
      (11, idBuffer, DBTYPE_UINT64, sizeof(double), idBufLens);
    m_insertStatement->execute(nb);
    for (int i = 0; i < nb; i++) {
      objects[i]->setId((u_signed64)idBuffer[i]);
    }
    // release the buffers for vid
    free(vidBuffer);
    free(vidBufLens);
    // release the buffers for side
    free(sideBuffer);
    free(sideBufLens);
    // release the buffers for tpmode
    free(tpmodeBuffer);
    free(tpmodeBufLens);
    // release the buffers for errMsgTxt
    free(errMsgTxtBuffer);
    free(errMsgTxtBufLens);
    // release the buffers for errorCode
    free(errorCodeBuffer);
    free(errorCodeBufLens);
    // release the buffers for severity
    free(severityBuffer);
    free(severityBufLens);
    // release the buffers for vwAddress
    free(vwAddressBuffer);
    free(vwAddressBufLens);
    // release the buffers for stream
    free(streamBuffer);
    free(streamBufLens);
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
void castor::db::cnv::DbTapeCnv::updateRep(castor::IAddress* address,
                                           castor::IObject* object,
                                           bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setString(1, obj->vid());
    m_updateStatement->setInt(2, obj->side());
    m_updateStatement->setInt(3, obj->tpmode());
    m_updateStatement->setString(4, obj->errMsgTxt());
    m_updateStatement->setInt(5, obj->errorCode());
    m_updateStatement->setInt(6, obj->severity());
    m_updateStatement->setString(7, obj->vwAddress());
    m_updateStatement->setInt(8, (int)obj->status());
    m_updateStatement->setUInt64(9, obj->id());
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
void castor::db::cnv::DbTapeCnv::deleteRep(castor::IAddress* address,
                                           castor::IObject* object,
                                           bool endTransaction)
  throw (castor::exception::Exception) {
  castor::stager::Tape* obj = 
    dynamic_cast<castor::stager::Tape*>(object);
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
castor::IObject* castor::db::cnv::DbTapeCnv::createObj(castor::IAddress* address)
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
    castor::stager::Tape* object = new castor::stager::Tape();
    // Now retrieve and set members
    object->setVid(rset->getString(1));
    object->setSide(rset->getInt(2));
    object->setTpmode(rset->getInt(3));
    object->setErrMsgTxt(rset->getString(4));
    object->setErrorCode(rset->getInt(5));
    object->setSeverity(rset->getInt(6));
    object->setVwAddress(rset->getString(7));
    object->setId(rset->getUInt64(8));
    object->setStatus((enum castor::stager::TapeStatusCodes)rset->getInt(10));
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
// updateObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbTapeCnv::updateObj(castor::IObject* obj)
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
    castor::stager::Tape* object = 
      dynamic_cast<castor::stager::Tape*>(obj);
    object->setVid(rset->getString(1));
    object->setSide(rset->getInt(2));
    object->setTpmode(rset->getInt(3));
    object->setErrMsgTxt(rset->getString(4));
    object->setErrorCode(rset->getInt(5));
    object->setSeverity(rset->getInt(6));
    object->setVwAddress(rset->getString(7));
    object->setId(rset->getUInt64(8));
    object->setStatus((enum castor::stager::TapeStatusCodes)rset->getInt(10));
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


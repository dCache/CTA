/******************************************************************************
 *                      castor/db/ora/OraSegmentCnv.cpp
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
#include "OraSegmentCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvFactory.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/Segment.hpp"
#include "castor/stager/SegmentStatusCodes.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/stager/TapeCopy.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraSegmentCnv> s_factoryOraSegmentCnv;
const castor::ICnvFactory& OraSegmentCnvFactory = 
  s_factoryOraSegmentCnv;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraSegmentCnv::s_insertStatementString =
"INSERT INTO Segment (fseq, offset, bytes_in, bytes_out, host_bytes, segmCksumAlgorithm, segmCksum, errMsgTxt, errorCode, severity, blockId0, blockId1, blockId2, blockId3, id, tape, copy, status) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,ids_seq.nextval,:15,:16,:17) RETURNING id INTO :18";

/// SQL statement for request deletion
const std::string castor::db::ora::OraSegmentCnv::s_deleteStatementString =
"DELETE FROM Segment WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraSegmentCnv::s_selectStatementString =
"SELECT fseq, offset, bytes_in, bytes_out, host_bytes, segmCksumAlgorithm, segmCksum, errMsgTxt, errorCode, severity, blockId0, blockId1, blockId2, blockId3, id, tape, copy, status FROM Segment WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraSegmentCnv::s_updateStatementString =
"UPDATE Segment SET fseq = :1, offset = :2, bytes_in = :3, bytes_out = :4, host_bytes = :5, segmCksumAlgorithm = :6, segmCksum = :7, errMsgTxt = :8, errorCode = :9, severity = :10, blockId0 = :11, blockId1 = :12, blockId2 = :13, blockId3 = :14, status = :15 WHERE id = :16";

/// SQL statement for type storage
const std::string castor::db::ora::OraSegmentCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraSegmentCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member tape
const std::string castor::db::ora::OraSegmentCnv::s_checkTapeExistStatementString =
"SELECT id from Tape WHERE id = :1";

/// SQL update statement for member tape
const std::string castor::db::ora::OraSegmentCnv::s_updateTapeStatementString =
"UPDATE Segment SET tape = :1 WHERE id = :2";

/// SQL existence statement for member copy
const std::string castor::db::ora::OraSegmentCnv::s_checkTapeCopyExistStatementString =
"SELECT id from TapeCopy WHERE id = :1";

/// SQL update statement for member copy
const std::string castor::db::ora::OraSegmentCnv::s_updateTapeCopyStatementString =
"UPDATE Segment SET copy = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraSegmentCnv::OraSegmentCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkTapeExistStatement(0),
  m_updateTapeStatement(0),
  m_checkTapeCopyExistStatement(0),
  m_updateTapeCopyStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraSegmentCnv::~OraSegmentCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_checkTapeExistStatement);
    deleteStatement(m_updateTapeStatement);
    deleteStatement(m_checkTapeCopyExistStatement);
    deleteStatement(m_updateTapeCopyStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkTapeExistStatement = 0;
  m_updateTapeStatement = 0;
  m_checkTapeCopyExistStatement = 0;
  m_updateTapeCopyStatement = 0;
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraSegmentCnv::ObjType() {
  return castor::stager::Segment::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraSegmentCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::fillRep(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type,
                                             bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Segment* obj = 
    dynamic_cast<castor::stager::Segment*>(object);
  try {
    switch (type) {
    case castor::OBJ_Tape :
      fillRepTape(obj);
      break;
    case castor::OBJ_TapeCopy :
      fillRepTapeCopy(obj);
      break;
    default :
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "fillRep called for type " << type 
                      << " on object of type " << obj->type() 
                      << ". This is meaningless.";
      throw ex;
    }
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepTape
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::fillRepTape(castor::stager::Segment* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->tape()) {
    // Check checkTapeExist statement
    if (0 == m_checkTapeExistStatement) {
      m_checkTapeExistStatement = createStatement(s_checkTapeExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeExistStatement->setDouble(1, obj->tape()->id());
    oracle::occi::ResultSet *rset = m_checkTapeExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("OraCnvSvc");
      ad.setCnvSvcType(castor::SVC_ORACNV);
      cnvSvc()->createRep(&ad, obj->tape(), false);
    }
    // Close resultset
    m_checkTapeExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateTapeStatement) {
    m_updateTapeStatement = createStatement(s_updateTapeStatementString);
  }
  // Update local object
  m_updateTapeStatement->setDouble(1, 0 == obj->tape() ? 0 : obj->tape()->id());
  m_updateTapeStatement->setDouble(2, obj->id());
  m_updateTapeStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillRepTapeCopy
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::fillRepTapeCopy(castor::stager::Segment* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->copy()) {
    // Check checkTapeCopyExist statement
    if (0 == m_checkTapeCopyExistStatement) {
      m_checkTapeCopyExistStatement = createStatement(s_checkTapeCopyExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeCopyExistStatement->setDouble(1, obj->copy()->id());
    oracle::occi::ResultSet *rset = m_checkTapeCopyExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("OraCnvSvc");
      ad.setCnvSvcType(castor::SVC_ORACNV);
      cnvSvc()->createRep(&ad, obj->copy(), false);
    }
    // Close resultset
    m_checkTapeCopyExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateTapeCopyStatement) {
    m_updateTapeCopyStatement = createStatement(s_updateTapeCopyStatementString);
  }
  // Update local object
  m_updateTapeCopyStatement->setDouble(1, 0 == obj->copy() ? 0 : obj->copy()->id());
  m_updateTapeCopyStatement->setDouble(2, obj->id());
  m_updateTapeCopyStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::fillObj(castor::IAddress* address,
                                             castor::IObject* object,
                                             unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::Segment* obj = 
    dynamic_cast<castor::stager::Segment*>(object);
  switch (type) {
  case castor::OBJ_Tape :
    fillObjTape(obj);
    break;
  case castor::OBJ_TapeCopy :
    fillObjTapeCopy(obj);
    break;
  default :
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "fillObj called on type " << type 
                    << " on object of type " << obj->type() 
                    << ". This is meaningless.";
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillObjTape
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::fillObjTape(castor::stager::Segment* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
  if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 tapeId = (u_signed64)rset->getDouble(16);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->tape() &&
      (0 == tapeId ||
       obj->tape()->id() != tapeId)) {
    obj->tape()->removeSegments(obj);
    obj->setTape(0);
  }
  // Update object or create new one
  if (0 != tapeId) {
    if (0 == obj->tape()) {
      obj->setTape
        (dynamic_cast<castor::stager::Tape*>
         (cnvSvc()->getObjFromId(tapeId)));
    } else {
      cnvSvc()->updateObj(obj->tape());
    }
    obj->tape()->addSegments(obj);
  }
}

//------------------------------------------------------------------------------
// fillObjTapeCopy
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::fillObjTapeCopy(castor::stager::Segment* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
  if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 copyId = (u_signed64)rset->getDouble(17);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->copy() &&
      (0 == copyId ||
       obj->copy()->id() != copyId)) {
    obj->copy()->removeSegments(obj);
    obj->setCopy(0);
  }
  // Update object or create new one
  if (0 != copyId) {
    if (0 == obj->copy()) {
      obj->setCopy
        (dynamic_cast<castor::stager::TapeCopy*>
         (cnvSvc()->getObjFromId(copyId)));
    } else {
      cnvSvc()->updateObj(obj->copy());
    }
    obj->copy()->addSegments(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::createRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit,
                                               unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::Segment* obj = 
    dynamic_cast<castor::stager::Segment*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(18, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->fseq());
    m_insertStatement->setDouble(2, obj->offset());
    m_insertStatement->setDouble(3, obj->bytes_in());
    m_insertStatement->setDouble(4, obj->bytes_out());
    m_insertStatement->setDouble(5, obj->host_bytes());
    m_insertStatement->setString(6, obj->segmCksumAlgorithm());
    m_insertStatement->setInt(7, obj->segmCksum());
    m_insertStatement->setString(8, obj->errMsgTxt());
    m_insertStatement->setInt(9, obj->errorCode());
    m_insertStatement->setInt(10, obj->severity());
    m_insertStatement->setInt(11, obj->blockId0());
    m_insertStatement->setInt(12, obj->blockId1());
    m_insertStatement->setInt(13, obj->blockId2());
    m_insertStatement->setInt(14, obj->blockId3());
    m_insertStatement->setDouble(15, (type == OBJ_Tape && obj->tape() != 0) ? obj->tape()->id() : 0);
    m_insertStatement->setDouble(16, (type == OBJ_TapeCopy && obj->copy() != 0) ? obj->copy()->id() : 0);
    m_insertStatement->setInt(17, (int)obj->status());
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(18));
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_insertStatementString << std::endl
                    << "and parameters' values were :" << std::endl
                    << "  fseq : " << obj->fseq() << std::endl
                    << "  offset : " << obj->offset() << std::endl
                    << "  bytes_in : " << obj->bytes_in() << std::endl
                    << "  bytes_out : " << obj->bytes_out() << std::endl
                    << "  host_bytes : " << obj->host_bytes() << std::endl
                    << "  segmCksumAlgorithm : " << obj->segmCksumAlgorithm() << std::endl
                    << "  segmCksum : " << obj->segmCksum() << std::endl
                    << "  errMsgTxt : " << obj->errMsgTxt() << std::endl
                    << "  errorCode : " << obj->errorCode() << std::endl
                    << "  severity : " << obj->severity() << std::endl
                    << "  blockId0 : " << obj->blockId0() << std::endl
                    << "  blockId1 : " << obj->blockId1() << std::endl
                    << "  blockId2 : " << obj->blockId2() << std::endl
                    << "  blockId3 : " << obj->blockId3() << std::endl
                    << "  id : " << obj->id() << std::endl
                    << "  tape : " << obj->tape() << std::endl
                    << "  copy : " << obj->copy() << std::endl
                    << "  status : " << obj->status() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::updateRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Segment* obj = 
    dynamic_cast<castor::stager::Segment*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  try {
    // Check whether the statements are ok
    if (0 == m_updateStatement) {
      m_updateStatement = createStatement(s_updateStatementString);
    }
    // Update the current object
    m_updateStatement->setInt(1, obj->fseq());
    m_updateStatement->setDouble(2, obj->offset());
    m_updateStatement->setDouble(3, obj->bytes_in());
    m_updateStatement->setDouble(4, obj->bytes_out());
    m_updateStatement->setDouble(5, obj->host_bytes());
    m_updateStatement->setString(6, obj->segmCksumAlgorithm());
    m_updateStatement->setInt(7, obj->segmCksum());
    m_updateStatement->setString(8, obj->errMsgTxt());
    m_updateStatement->setInt(9, obj->errorCode());
    m_updateStatement->setInt(10, obj->severity());
    m_updateStatement->setInt(11, obj->blockId0());
    m_updateStatement->setInt(12, obj->blockId1());
    m_updateStatement->setInt(13, obj->blockId2());
    m_updateStatement->setInt(14, obj->blockId3());
    m_updateStatement->setInt(15, (int)obj->status());
    m_updateStatement->setDouble(16, obj->id());
    m_updateStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::deleteRep(castor::IAddress* address,
                                               castor::IObject* object,
                                               bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::Segment* obj = 
    dynamic_cast<castor::stager::Segment*>(object);
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
    m_deleteTypeStatement->setDouble(1, obj->id());
    m_deleteTypeStatement->executeUpdate();
    m_deleteStatement->setDouble(1, obj->id());
    m_deleteStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->getConnection()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in delete request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_deleteStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::db::ora::OraSegmentCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::BaseAddress* ad = 
    dynamic_cast<castor::BaseAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setDouble(1, ad->target());
    oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << ad->target();
      throw ex;
    }
    // create the new Object
    castor::stager::Segment* object = new castor::stager::Segment();
    // Now retrieve and set members
    object->setFseq(rset->getInt(1));
    object->setOffset((u_signed64)rset->getDouble(2));
    object->setBytes_in((u_signed64)rset->getDouble(3));
    object->setBytes_out((u_signed64)rset->getDouble(4));
    object->setHost_bytes((u_signed64)rset->getDouble(5));
    object->setSegmCksumAlgorithm(rset->getString(6));
    object->setSegmCksum(rset->getInt(7));
    object->setErrMsgTxt(rset->getString(8));
    object->setErrorCode(rset->getInt(9));
    object->setSeverity(rset->getInt(10));
    object->setBlockId0(rset->getInt(11));
    object->setBlockId1(rset->getInt(12));
    object->setBlockId2(rset->getInt(13));
    object->setBlockId3(rset->getInt(14));
    object->setId((u_signed64)rset->getDouble(15));
    object->setStatus((enum castor::stager::SegmentStatusCodes)rset->getInt(18));
    m_selectStatement->closeResultSet(rset);
    return object;
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in select request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_selectStatementString << std::endl
                    << "and id was " << ad->target() << std::endl;;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::db::ora::OraSegmentCnv::updateObj(castor::IObject* obj)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setDouble(1, obj->id());
    oracle::occi::ResultSet *rset = m_selectStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << obj->id();
      throw ex;
    }
    // Now retrieve and set members
    castor::stager::Segment* object = 
      dynamic_cast<castor::stager::Segment*>(obj);
    object->setFseq(rset->getInt(1));
    object->setOffset((u_signed64)rset->getDouble(2));
    object->setBytes_in((u_signed64)rset->getDouble(3));
    object->setBytes_out((u_signed64)rset->getDouble(4));
    object->setHost_bytes((u_signed64)rset->getDouble(5));
    object->setSegmCksumAlgorithm(rset->getString(6));
    object->setSegmCksum(rset->getInt(7));
    object->setErrMsgTxt(rset->getString(8));
    object->setErrorCode(rset->getInt(9));
    object->setSeverity(rset->getInt(10));
    object->setBlockId0(rset->getInt(11));
    object->setBlockId1(rset->getInt(12));
    object->setBlockId2(rset->getInt(13));
    object->setBlockId3(rset->getInt(14));
    object->setId((u_signed64)rset->getDouble(15));
    object->setStatus((enum castor::stager::SegmentStatusCodes)rset->getInt(18));
    m_selectStatement->closeResultSet(rset);
  } catch (oracle::occi::SQLException e) {
    try {
      // Always try to rollback
      cnvSvc()->getConnection()->rollback();
      if (3114 == e.getErrorCode() || 28 == e.getErrorCode()) {
        // We've obviously lost the ORACLE connection here
        cnvSvc()->dropConnection();
      }
    } catch (oracle::occi::SQLException e) {
      // rollback failed, let's drop the connection for security
      cnvSvc()->dropConnection();
    }
    castor::exception::InvalidArgument ex; // XXX Fix it, depending on ORACLE error
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.what() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


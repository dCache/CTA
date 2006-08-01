/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "DbSegmentCnv.hpp"
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
#include "castor/stager/SegmentStatusCodes.hpp"
#include "castor/stager/Tape.hpp"
#include "castor/stager/TapeCopy.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::cnv::DbSegmentCnv>* s_factoryDbSegmentCnv =
  new castor::CnvFactory<castor::db::cnv::DbSegmentCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::cnv::DbSegmentCnv::s_insertStatementString =
"INSERT INTO Segment (fseq, offset, bytes_in, bytes_out, host_bytes, segmCksumAlgorithm, segmCksum, errMsgTxt, errorCode, severity, blockId0, blockId1, blockId2, blockId3, id, tape, copy, status) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,ids_seq.nextval,:15,:16,:17) RETURNING id INTO :18";

/// SQL statement for request deletion
const std::string castor::db::cnv::DbSegmentCnv::s_deleteStatementString =
"DELETE FROM Segment WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::cnv::DbSegmentCnv::s_selectStatementString =
"SELECT fseq, offset, bytes_in, bytes_out, host_bytes, segmCksumAlgorithm, segmCksum, errMsgTxt, errorCode, severity, blockId0, blockId1, blockId2, blockId3, id, tape, copy, status FROM Segment WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::cnv::DbSegmentCnv::s_updateStatementString =
"UPDATE Segment SET fseq = :1, offset = :2, bytes_in = :3, bytes_out = :4, host_bytes = :5, segmCksumAlgorithm = :6, segmCksum = :7, errMsgTxt = :8, errorCode = :9, severity = :10, blockId0 = :11, blockId1 = :12, blockId2 = :13, blockId3 = :14, status = :15 WHERE id = :16";

/// SQL statement for type storage
const std::string castor::db::cnv::DbSegmentCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::cnv::DbSegmentCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member tape
const std::string castor::db::cnv::DbSegmentCnv::s_checkTapeExistStatementString =
"SELECT id FROM Tape WHERE id = :1";

/// SQL update statement for member tape
const std::string castor::db::cnv::DbSegmentCnv::s_updateTapeStatementString =
"UPDATE Segment SET tape = :1 WHERE id = :2";

/// SQL existence statement for member copy
const std::string castor::db::cnv::DbSegmentCnv::s_checkTapeCopyExistStatementString =
"SELECT id FROM TapeCopy WHERE id = :1";

/// SQL update statement for member copy
const std::string castor::db::cnv::DbSegmentCnv::s_updateTapeCopyStatementString =
"UPDATE Segment SET copy = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::cnv::DbSegmentCnv::DbSegmentCnv(castor::ICnvSvc* cnvSvc) :
  DbBaseCnv(cnvSvc),
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
castor::db::cnv::DbSegmentCnv::~DbSegmentCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::cnv::DbSegmentCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    delete m_insertStatement;
    delete m_deleteStatement;
    delete m_selectStatement;
    delete m_updateStatement;
    delete m_storeTypeStatement;
    delete m_deleteTypeStatement;
    delete m_checkTapeExistStatement;
    delete m_updateTapeStatement;
    delete m_checkTapeCopyExistStatement;
    delete m_updateTapeCopyStatement;
  } catch (castor::exception::SQLError ignored) {};
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
const unsigned int castor::db::cnv::DbSegmentCnv::ObjType() {
  return castor::stager::Segment::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::cnv::DbSegmentCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::cnv::DbSegmentCnv::fillRep(castor::IAddress* address,
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
// fillRepTape
//------------------------------------------------------------------------------
void castor::db::cnv::DbSegmentCnv::fillRepTape(castor::stager::Segment* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->tape()) {
    // Check checkTapeExist statement
    if (0 == m_checkTapeExistStatement) {
      m_checkTapeExistStatement = createStatement(s_checkTapeExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeExistStatement->setInt64(1, obj->tape()->id());
    castor::db::IDbResultSet *rset = m_checkTapeExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->tape(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateTapeStatement) {
    m_updateTapeStatement = createStatement(s_updateTapeStatementString);
  }
  // Update local object
  m_updateTapeStatement->setInt64(1, 0 == obj->tape() ? 0 : obj->tape()->id());
  m_updateTapeStatement->setInt64(2, obj->id());
  m_updateTapeStatement->execute();
}

//------------------------------------------------------------------------------
// fillRepTapeCopy
//------------------------------------------------------------------------------
void castor::db::cnv::DbSegmentCnv::fillRepTapeCopy(castor::stager::Segment* obj)
  throw (castor::exception::Exception) {
  if (0 != obj->copy()) {
    // Check checkTapeCopyExist statement
    if (0 == m_checkTapeCopyExistStatement) {
      m_checkTapeCopyExistStatement = createStatement(s_checkTapeCopyExistStatementString);
    }
    // retrieve the object from the database
    m_checkTapeCopyExistStatement->setInt64(1, obj->copy()->id());
    castor::db::IDbResultSet *rset = m_checkTapeCopyExistStatement->executeQuery();
    if (!rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->copy(), false);
    }
    // Close resultset
    delete rset;
  }
  // Check update statement
  if (0 == m_updateTapeCopyStatement) {
    m_updateTapeCopyStatement = createStatement(s_updateTapeCopyStatementString);
  }
  // Update local object
  m_updateTapeCopyStatement->setInt64(1, 0 == obj->copy() ? 0 : obj->copy()->id());
  m_updateTapeCopyStatement->setInt64(2, obj->id());
  m_updateTapeCopyStatement->execute();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::cnv::DbSegmentCnv::fillObj(castor::IAddress* address,
                                            castor::IObject* object,
                                            unsigned int type,
                                            bool autocommit)
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
  if (autocommit) {
    cnvSvc()->commit();
  }
}
//------------------------------------------------------------------------------
// fillObjTape
//------------------------------------------------------------------------------
void castor::db::cnv::DbSegmentCnv::fillObjTape(castor::stager::Segment* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
  if (!rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 tapeId = rset->getInt64(16);
  // Close ResultSet
  delete rset;
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
void castor::db::cnv::DbSegmentCnv::fillObjTapeCopy(castor::stager::Segment* obj)
  throw (castor::exception::Exception) {
  // Check whether the statement is ok
  if (0 == m_selectStatement) {
    m_selectStatement = createStatement(s_selectStatementString);
  }
  // retrieve the object from the database
  m_selectStatement->setInt64(1, obj->id());
  castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
  if (!rset->next()) {
    castor::exception::NoEntry ex;
    ex.getMessage() << "No object found for id :" << obj->id();
    throw ex;
  }
  u_signed64 copyId = rset->getInt64(17);
  // Close ResultSet
  delete rset;
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
void castor::db::cnv::DbSegmentCnv::createRep(castor::IAddress* address,
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
      m_insertStatement->registerOutParam(18, castor::db::DBTYPE_INT64);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setInt(1, obj->fseq());
    m_insertStatement->setInt64(2, obj->offset());
    m_insertStatement->setInt64(3, obj->bytes_in());
    m_insertStatement->setInt64(4, obj->bytes_out());
    m_insertStatement->setInt64(5, obj->host_bytes());
    m_insertStatement->setString(6, obj->segmCksumAlgorithm());
    m_insertStatement->setInt(7, obj->segmCksum());
    m_insertStatement->setString(8, obj->errMsgTxt());
    m_insertStatement->setInt(9, obj->errorCode());
    m_insertStatement->setInt(10, obj->severity());
    m_insertStatement->setInt(11, obj->blockId0());
    m_insertStatement->setInt(12, obj->blockId1());
    m_insertStatement->setInt(13, obj->blockId2());
    m_insertStatement->setInt(14, obj->blockId3());
    m_insertStatement->setInt64(15, (type == OBJ_Tape && obj->tape() != 0) ? obj->tape()->id() : 0);
    m_insertStatement->setInt64(16, (type == OBJ_TapeCopy && obj->copy() != 0) ? obj->copy()->id() : 0);
    m_insertStatement->setInt(17, (int)obj->status());
    m_insertStatement->execute();
    obj->setId(m_insertStatement->getInt64(18));
    m_storeTypeStatement->setInt64(1, obj->id());
    m_storeTypeStatement->setInt64(2, obj->type());
    m_storeTypeStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in insert request :"
                    << std::endl << e.getMessage().str() << std::endl
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
void castor::db::cnv::DbSegmentCnv::updateRep(castor::IAddress* address,
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
    m_updateStatement->setInt64(2, obj->offset());
    m_updateStatement->setInt64(3, obj->bytes_in());
    m_updateStatement->setInt64(4, obj->bytes_out());
    m_updateStatement->setInt64(5, obj->host_bytes());
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
    m_updateStatement->setInt64(16, obj->id());
    m_updateStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
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
void castor::db::cnv::DbSegmentCnv::deleteRep(castor::IAddress* address,
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
    m_deleteTypeStatement->setInt64(1, obj->id());
    m_deleteTypeStatement->execute();
    m_deleteStatement->setInt64(1, obj->id());
    m_deleteStatement->execute();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
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
castor::IObject* castor::db::cnv::DbSegmentCnv::createObj(castor::IAddress* address)
  throw (castor::exception::Exception) {
  castor::BaseAddress* ad = 
    dynamic_cast<castor::BaseAddress*>(address);
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setInt64(1, ad->target());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << ad->target();
      throw ex;
    }
    // create the new Object
    castor::stager::Segment* object = new castor::stager::Segment();
    // Now retrieve and set members
    object->setFseq(rset->getInt(1));
    object->setOffset(rset->getInt64(2));
    object->setBytes_in(rset->getInt64(3));
    object->setBytes_out(rset->getInt64(4));
    object->setHost_bytes(rset->getInt64(5));
    object->setSegmCksumAlgorithm(rset->getString(6));
    object->setSegmCksum(rset->getInt(7));
    object->setErrMsgTxt(rset->getString(8));
    object->setErrorCode(rset->getInt(9));
    object->setSeverity(rset->getInt(10));
    object->setBlockId0(rset->getInt(11));
    object->setBlockId1(rset->getInt(12));
    object->setBlockId2(rset->getInt(13));
    object->setBlockId3(rset->getInt(14));
    object->setId(rset->getInt64(15));
    object->setStatus((enum castor::stager::SegmentStatusCodes)rset->getInt(18));
    delete rset;
    return object;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
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
void castor::db::cnv::DbSegmentCnv::updateObj(castor::IObject* obj)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statement is ok
    if (0 == m_selectStatement) {
      m_selectStatement = createStatement(s_selectStatementString);
    }
    // retrieve the object from the database
    m_selectStatement->setInt64(1, obj->id());
    castor::db::IDbResultSet *rset = m_selectStatement->executeQuery();
    if (!rset->next()) {
      castor::exception::NoEntry ex;
      ex.getMessage() << "No object found for id :" << obj->id();
      throw ex;
    }
    // Now retrieve and set members
    castor::stager::Segment* object = 
      dynamic_cast<castor::stager::Segment*>(obj);
    object->setFseq(rset->getInt(1));
    object->setOffset(rset->getInt64(2));
    object->setBytes_in(rset->getInt64(3));
    object->setBytes_out(rset->getInt64(4));
    object->setHost_bytes(rset->getInt64(5));
    object->setSegmCksumAlgorithm(rset->getString(6));
    object->setSegmCksum(rset->getInt(7));
    object->setErrMsgTxt(rset->getString(8));
    object->setErrorCode(rset->getInt(9));
    object->setSeverity(rset->getInt(10));
    object->setBlockId0(rset->getInt(11));
    object->setBlockId1(rset->getInt(12));
    object->setBlockId2(rset->getInt(13));
    object->setBlockId3(rset->getInt(14));
    object->setId(rset->getInt64(15));
    object->setStatus((enum castor::stager::SegmentStatusCodes)rset->getInt(18));
    delete rset;
  } catch (castor::exception::SQLError e) {
    // Always try to rollback
    try { cnvSvc()->rollback(); }
    catch(castor::exception::SQLError ignored) {}
    castor::exception::InvalidArgument ex;
    ex.getMessage() << "Error in update request :"
                    << std::endl << e.getMessage().str() << std::endl
                    << "Statement was :" << std::endl
                    << s_updateStatementString << std::endl
                    << "and id was " << obj->id() << std::endl;;
    throw ex;
  }
}


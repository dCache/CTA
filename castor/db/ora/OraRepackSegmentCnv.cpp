/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "OraRepackSegmentCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/repack/RepackSegment.hpp"
#include "castor/repack/RepackSubRequest.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraRepackSegmentCnv>* s_factoryOraRepackSegmentCnv =
  new castor::CnvFactory<castor::db::ora::OraRepackSegmentCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraRepackSegmentCnv::s_insertStatementString =
"INSERT INTO RepackSegment (fileid, compression, segsize, filesec, copyno, id, vid) VALUES (:1,:2,:3,:4,:5,ids_seq.nextval,:6) RETURNING id INTO :7";

/// SQL statement for request deletion
const std::string castor::db::ora::OraRepackSegmentCnv::s_deleteStatementString =
"DELETE FROM RepackSegment WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraRepackSegmentCnv::s_selectStatementString =
"SELECT fileid, compression, segsize, filesec, copyno, id, vid FROM RepackSegment WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraRepackSegmentCnv::s_updateStatementString =
"UPDATE RepackSegment SET fileid = :1, compression = :2, segsize = :3, filesec = :4, copyno = :5 WHERE id = :6";

/// SQL statement for type storage
const std::string castor::db::ora::OraRepackSegmentCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraRepackSegmentCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL existence statement for member vid
const std::string castor::db::ora::OraRepackSegmentCnv::s_checkRepackSubRequestExistStatementString =
"SELECT id from RepackSubRequest WHERE id = :1";

/// SQL update statement for member vid
const std::string castor::db::ora::OraRepackSegmentCnv::s_updateRepackSubRequestStatementString =
"UPDATE RepackSegment SET vid = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraRepackSegmentCnv::OraRepackSegmentCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_checkRepackSubRequestExistStatement(0),
  m_updateRepackSubRequestStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraRepackSegmentCnv::~OraRepackSegmentCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSegmentCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_checkRepackSubRequestExistStatement);
    deleteStatement(m_updateRepackSubRequestStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_checkRepackSubRequestExistStatement = 0;
  m_updateRepackSubRequestStatement = 0;
  }

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraRepackSegmentCnv::ObjType() {
  return castor::repack::RepackSegment::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraRepackSegmentCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSegmentCnv::fillRep(castor::IAddress* address,
                                                   castor::IObject* object,
                                                   unsigned int type,
                                                   bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackSegment* obj = 
    dynamic_cast<castor::repack::RepackSegment*>(object);
  try {
    switch (type) {
    case castor::OBJ_RepackSubRequest :
      fillRepRepackSubRequest(obj);
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
  } catch (oracle::occi::SQLException e) {
    cnvSvc()->handleException(e);
    castor::exception::Internal ex; 
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepRepackSubRequest
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSegmentCnv::fillRepRepackSubRequest(castor::repack::RepackSegment* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->vid()) {
    // Check checkRepackSubRequestExist statement
    if (0 == m_checkRepackSubRequestExistStatement) {
      m_checkRepackSubRequestExistStatement = createStatement(s_checkRepackSubRequestExistStatementString);
    }
    // retrieve the object from the database
    m_checkRepackSubRequestExistStatement->setDouble(1, obj->vid()->id());
    oracle::occi::ResultSet *rset = m_checkRepackSubRequestExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->vid(), false);
    }
    // Close resultset
    m_checkRepackSubRequestExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateRepackSubRequestStatement) {
    m_updateRepackSubRequestStatement = createStatement(s_updateRepackSubRequestStatementString);
  }
  // Update local object
  m_updateRepackSubRequestStatement->setDouble(1, 0 == obj->vid() ? 0 : obj->vid()->id());
  m_updateRepackSubRequestStatement->setDouble(2, obj->id());
  m_updateRepackSubRequestStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSegmentCnv::fillObj(castor::IAddress* address,
                                                   castor::IObject* object,
                                                   unsigned int type,
                                                   bool autocommit)
  throw (castor::exception::Exception) {
  castor::repack::RepackSegment* obj = 
    dynamic_cast<castor::repack::RepackSegment*>(object);
  switch (type) {
  case castor::OBJ_RepackSubRequest :
    fillObjRepackSubRequest(obj);
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
// fillObjRepackSubRequest
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSegmentCnv::fillObjRepackSubRequest(castor::repack::RepackSegment* obj)
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
  u_signed64 vidId = (u_signed64)rset->getDouble(7);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->vid() &&
      (0 == vidId ||
       obj->vid()->id() != vidId)) {
    obj->vid()->removeSegment(obj);
    obj->setVid(0);
  }
  // Update object or create new one
  if (0 != vidId) {
    if (0 == obj->vid()) {
      obj->setVid
        (dynamic_cast<castor::repack::RepackSubRequest*>
         (cnvSvc()->getObjFromId(vidId)));
    } else {
      cnvSvc()->updateObj(obj->vid());
    }
    obj->vid()->addSegment(obj);
  }
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSegmentCnv::createRep(castor::IAddress* address,
                                                     castor::IObject* object,
                                                     bool autocommit,
                                                     unsigned int type)
  throw (castor::exception::Exception) {
  castor::repack::RepackSegment* obj = 
    dynamic_cast<castor::repack::RepackSegment*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(7, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setDouble(1, obj->fileid());
    m_insertStatement->setInt(2, obj->compression());
    m_insertStatement->setDouble(3, obj->segsize());
    m_insertStatement->setDouble(4, obj->filesec());
    m_insertStatement->setInt(5, obj->copyno());
    m_insertStatement->setDouble(6, (type == OBJ_RepackSubRequest && obj->vid() != 0) ? obj->vid()->id() : 0);
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(7));
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (oracle::occi::SQLException e) {
      cnvSvc()->handleException(e);
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "Error in insert request :"
                      << std::endl << e.what() << std::endl
                      << "Statement was :" << std::endl
                      << s_insertStatementString << std::endl
                      << "and parameters' values were :" << std::endl
                      << "  fileid : " << obj->fileid() << std::endl
                      << "  compression : " << obj->compression() << std::endl
                      << "  segsize : " << obj->segsize() << std::endl
                      << "  filesec : " << obj->filesec() << std::endl
                      << "  copyno : " << obj->copyno() << std::endl
                      << "  id : " << obj->id() << std::endl
                      << "  vid : " << obj->vid() << std::endl;
      throw ex;
    }
}

  //----------------------------------------------------------------------------
  // updateRep
  //----------------------------------------------------------------------------
  void castor::db::ora::OraRepackSegmentCnv::updateRep(castor::IAddress* address,
                                                       castor::IObject* object,
                                                       bool autocommit)
    throw (castor::exception::Exception) {
    castor::repack::RepackSegment* obj = 
      dynamic_cast<castor::repack::RepackSegment*>(object);
    // check whether something needs to be done
    if (0 == obj) return;
    try {
      // Check whether the statements are ok
      if (0 == m_updateStatement) {
        m_updateStatement = createStatement(s_updateStatementString);
      }
      // Update the current object
      m_updateStatement->setDouble(1, obj->fileid());
      m_updateStatement->setInt(2, obj->compression());
      m_updateStatement->setDouble(3, obj->segsize());
      m_updateStatement->setDouble(4, obj->filesec());
      m_updateStatement->setInt(5, obj->copyno());
      m_updateStatement->setDouble(6, obj->id());
      m_updateStatement->executeUpdate();
      if (autocommit) {
        cnvSvc()->commit();
      }
    } catch (oracle::occi::SQLException e) {
        cnvSvc()->handleException(e);
        castor::exception::InvalidArgument ex;
        ex.getMessage() << "Error in update request :"
                        << std::endl << e.what() << std::endl
                        << "Statement was :" << std::endl
                        << s_updateStatementString << std::endl
                        << "and id was " << obj->id() << std::endl;;
        throw ex;
      }
}

    //--------------------------------------------------------------------------
    // deleteRep
    //--------------------------------------------------------------------------
    void castor::db::ora::OraRepackSegmentCnv::deleteRep(castor::IAddress* address,
                                                         castor::IObject* object,
                                                         bool autocommit)
      throw (castor::exception::Exception) {
      castor::repack::RepackSegment* obj = 
        dynamic_cast<castor::repack::RepackSegment*>(object);
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
          cnvSvc()->commit();
        }
      } catch (oracle::occi::SQLException e) {
          cnvSvc()->handleException(e);
          castor::exception::InvalidArgument ex;
          ex.getMessage() << "Error in delete request :"
                          << std::endl << e.what() << std::endl
                          << "Statement was :" << std::endl
                          << s_deleteStatementString << std::endl
                          << "and id was " << obj->id() << std::endl;;
          throw ex;
        }
}

      //------------------------------------------------------------------------
      // createObj
      //------------------------------------------------------------------------
      castor::IObject* castor::db::ora::OraRepackSegmentCnv::createObj(castor::IAddress* address)
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
          castor::repack::RepackSegment* object = new castor::repack::RepackSegment();
          // Now retrieve and set members
          object->setFileid((u_signed64)rset->getDouble(1));
          object->setCompression(rset->getInt(2));
          object->setSegsize((u_signed64)rset->getDouble(3));
          object->setFilesec((u_signed64)rset->getDouble(4));
          object->setCopyno(rset->getInt(5));
          object->setId((u_signed64)rset->getDouble(6));
          m_selectStatement->closeResultSet(rset);
          return object;
        } catch (oracle::occi::SQLException e) {
            cnvSvc()->handleException(e);
            castor::exception::InvalidArgument ex;
            ex.getMessage() << "Error in select request :"
                            << std::endl << e.what() << std::endl
                            << "Statement was :" << std::endl
                            << s_selectStatementString << std::endl
                            << "and id was " << ad->target() << std::endl;;
            throw ex;
          }
        }

        //----------------------------------------------------------------------
        // updateObj
        //----------------------------------------------------------------------
        void castor::db::ora::OraRepackSegmentCnv::updateObj(castor::IObject* obj)
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
            castor::repack::RepackSegment* object = 
              dynamic_cast<castor::repack::RepackSegment*>(obj);
            object->setFileid((u_signed64)rset->getDouble(1));
            object->setCompression(rset->getInt(2));
            object->setSegsize((u_signed64)rset->getDouble(3));
            object->setFilesec((u_signed64)rset->getDouble(4));
            object->setCopyno(rset->getInt(5));
            object->setId((u_signed64)rset->getDouble(6));
            m_selectStatement->closeResultSet(rset);
          } catch (oracle::occi::SQLException e) {
              cnvSvc()->handleException(e);
              castor::exception::InvalidArgument ex;
              ex.getMessage() << "Error in update request :"
                              << std::endl << e.what() << std::endl
                              << "Statement was :" << std::endl
                              << s_updateStatementString << std::endl
                              << "and id was " << obj->id() << std::endl;;
              throw ex;
            }
}


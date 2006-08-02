/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "OraRepackSubRequestCnv.hpp"
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
#include "castor/repack/RepackRequest.hpp"
#include "castor/repack/RepackSegment.hpp"
#include "castor/repack/RepackSubRequest.hpp"
#include <set>
#include <vector>

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraRepackSubRequestCnv>* s_factoryOraRepackSubRequestCnv =
  new castor::CnvFactory<castor::db::ora::OraRepackSubRequestCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraRepackSubRequestCnv::s_insertStatementString =
"INSERT INTO RepackSubRequest (vid, xsize, status, cuuid, filesMigrating, filesStaging, files, filesFailed, id, requestID) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,ids_seq.nextval,:9) RETURNING id INTO :10";

/// SQL statement for request deletion
const std::string castor::db::ora::OraRepackSubRequestCnv::s_deleteStatementString =
"DELETE FROM RepackSubRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraRepackSubRequestCnv::s_selectStatementString =
"SELECT vid, xsize, status, cuuid, filesMigrating, filesStaging, files, filesFailed, id, requestID FROM RepackSubRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraRepackSubRequestCnv::s_updateStatementString =
"UPDATE RepackSubRequest SET vid = :1, xsize = :2, status = :3, cuuid = :4, filesMigrating = :5, filesStaging = :6, files = :7, filesFailed = :8 WHERE id = :9";

/// SQL statement for type storage
const std::string castor::db::ora::OraRepackSubRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraRepackSubRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL select statement for member segment
const std::string castor::db::ora::OraRepackSubRequestCnv::s_selectRepackSegmentStatementString =
"SELECT id from RepackSegment WHERE vid = :1 FOR UPDATE";

/// SQL delete statement for member segment
const std::string castor::db::ora::OraRepackSubRequestCnv::s_deleteRepackSegmentStatementString =
"UPDATE RepackSegment SET vid = 0 WHERE id = :1";

/// SQL remote update statement for member segment
const std::string castor::db::ora::OraRepackSubRequestCnv::s_remoteUpdateRepackSegmentStatementString =
"UPDATE RepackSegment SET vid = :1 WHERE id = :2";

/// SQL existence statement for member requestID
const std::string castor::db::ora::OraRepackSubRequestCnv::s_checkRepackRequestExistStatementString =
"SELECT id from RepackRequest WHERE id = :1";

/// SQL update statement for member requestID
const std::string castor::db::ora::OraRepackSubRequestCnv::s_updateRepackRequestStatementString =
"UPDATE RepackSubRequest SET requestID = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraRepackSubRequestCnv::OraRepackSubRequestCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
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
castor::db::ora::OraRepackSubRequestCnv::~OraRepackSubRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSubRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_deleteRepackSegmentStatement);
    deleteStatement(m_selectRepackSegmentStatement);
    deleteStatement(m_remoteUpdateRepackSegmentStatement);
    deleteStatement(m_checkRepackRequestExistStatement);
    deleteStatement(m_updateRepackRequestStatement);
  } catch (oracle::occi::SQLException e) {};
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
const unsigned int castor::db::ora::OraRepackSubRequestCnv::ObjType() {
  return castor::repack::RepackSubRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraRepackSubRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSubRequestCnv::fillRep(castor::IAddress* address,
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
  } catch (oracle::occi::SQLException e) {
    cnvSvc()->handleException(e);
    castor::exception::Internal ex; 
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepRepackSegment
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSubRequestCnv::fillRepRepackSegment(castor::repack::RepackSubRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // check select statement
  if (0 == m_selectRepackSegmentStatement) {
    m_selectRepackSegmentStatement = createStatement(s_selectRepackSegmentStatementString);
  }
  // Get current database data
  std::set<int> segmentList;
  m_selectRepackSegmentStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectRepackSegmentStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    segmentList.insert(rset->getInt(1));
  }
  m_selectRepackSegmentStatement->closeResultSet(rset);
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
      m_remoteUpdateRepackSegmentStatement->setDouble(1, obj->id());
      m_remoteUpdateRepackSegmentStatement->setDouble(2, (*it)->id());
      m_remoteUpdateRepackSegmentStatement->executeUpdate();
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
    m_deleteRepackSegmentStatement->setDouble(1, *it);
    m_deleteRepackSegmentStatement->executeUpdate();
  }
}

//------------------------------------------------------------------------------
// fillRepRepackRequest
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSubRequestCnv::fillRepRepackRequest(castor::repack::RepackSubRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->requestID()) {
    // Check checkRepackRequestExist statement
    if (0 == m_checkRepackRequestExistStatement) {
      m_checkRepackRequestExistStatement = createStatement(s_checkRepackRequestExistStatementString);
    }
    // retrieve the object from the database
    m_checkRepackRequestExistStatement->setDouble(1, obj->requestID()->id());
    oracle::occi::ResultSet *rset = m_checkRepackRequestExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->requestID(), false);
    }
    // Close resultset
    m_checkRepackRequestExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateRepackRequestStatement) {
    m_updateRepackRequestStatement = createStatement(s_updateRepackRequestStatementString);
  }
  // Update local object
  m_updateRepackRequestStatement->setDouble(1, 0 == obj->requestID() ? 0 : obj->requestID()->id());
  m_updateRepackRequestStatement->setDouble(2, obj->id());
  m_updateRepackRequestStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraRepackSubRequestCnv::fillObj(castor::IAddress* address,
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
void castor::db::ora::OraRepackSubRequestCnv::fillObjRepackSegment(castor::repack::RepackSubRequest* obj)
  throw (castor::exception::Exception) {
  // Check select statement
  if (0 == m_selectRepackSegmentStatement) {
    m_selectRepackSegmentStatement = createStatement(s_selectRepackSegmentStatementString);
  }
  // retrieve the object from the database
  std::set<int> segmentList;
  m_selectRepackSegmentStatement->setDouble(1, obj->id());
  oracle::occi::ResultSet *rset = m_selectRepackSegmentStatement->executeQuery();
  while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
    segmentList.insert(rset->getInt(1));
  }
  // Close ResultSet
  m_selectRepackSegmentStatement->closeResultSet(rset);
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
void castor::db::ora::OraRepackSubRequestCnv::fillObjRepackRequest(castor::repack::RepackSubRequest* obj)
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
  u_signed64 requestIDId = (u_signed64)rset->getDouble(10);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
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
void castor::db::ora::OraRepackSubRequestCnv::createRep(castor::IAddress* address,
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
      m_insertStatement->registerOutParam(10, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->vid());
    m_insertStatement->setDouble(2, obj->xsize());
    m_insertStatement->setInt(3, obj->status());
    m_insertStatement->setString(4, obj->cuuid());
    m_insertStatement->setInt(5, obj->filesMigrating());
    m_insertStatement->setInt(6, obj->filesStaging());
    m_insertStatement->setInt(7, obj->files());
    m_insertStatement->setInt(8, obj->filesFailed());
    m_insertStatement->setDouble(9, (type == OBJ_RepackRequest && obj->requestID() != 0) ? obj->requestID()->id() : 0);
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(10));
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
                      << "  vid : " << obj->vid() << std::endl
                      << "  xsize : " << obj->xsize() << std::endl
                      << "  status : " << obj->status() << std::endl
                      << "  cuuid : " << obj->cuuid() << std::endl
                      << "  filesMigrating : " << obj->filesMigrating() << std::endl
                      << "  filesStaging : " << obj->filesStaging() << std::endl
                      << "  files : " << obj->files() << std::endl
                      << "  filesFailed : " << obj->filesFailed() << std::endl
                      << "  id : " << obj->id() << std::endl
                      << "  requestID : " << obj->requestID() << std::endl;
      throw ex;
    }
}

  //----------------------------------------------------------------------------
  // updateRep
  //----------------------------------------------------------------------------
  void castor::db::ora::OraRepackSubRequestCnv::updateRep(castor::IAddress* address,
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
      m_updateStatement->setDouble(2, obj->xsize());
      m_updateStatement->setInt(3, obj->status());
      m_updateStatement->setString(4, obj->cuuid());
      m_updateStatement->setInt(5, obj->filesMigrating());
      m_updateStatement->setInt(6, obj->filesStaging());
      m_updateStatement->setInt(7, obj->files());
      m_updateStatement->setInt(8, obj->filesFailed());
      m_updateStatement->setDouble(9, obj->id());
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
    void castor::db::ora::OraRepackSubRequestCnv::deleteRep(castor::IAddress* address,
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
      castor::IObject* castor::db::ora::OraRepackSubRequestCnv::createObj(castor::IAddress* address)
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
          castor::repack::RepackSubRequest* object = new castor::repack::RepackSubRequest();
          // Now retrieve and set members
          object->setVid(rset->getString(1));
          object->setXsize((u_signed64)rset->getDouble(2));
          object->setStatus(rset->getInt(3));
          object->setCuuid(rset->getString(4));
          object->setFilesMigrating(rset->getInt(5));
          object->setFilesStaging(rset->getInt(6));
          object->setFiles(rset->getInt(7));
          object->setFilesFailed(rset->getInt(8));
          object->setId((u_signed64)rset->getDouble(9));
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
        void castor::db::ora::OraRepackSubRequestCnv::updateObj(castor::IObject* obj)
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
            castor::repack::RepackSubRequest* object = 
              dynamic_cast<castor::repack::RepackSubRequest*>(obj);
            object->setVid(rset->getString(1));
            object->setXsize((u_signed64)rset->getDouble(2));
            object->setStatus(rset->getInt(3));
            object->setCuuid(rset->getString(4));
            object->setFilesMigrating(rset->getInt(5));
            object->setFilesStaging(rset->getInt(6));
            object->setFiles(rset->getInt(7));
            object->setFilesFailed(rset->getInt(8));
            object->setId((u_signed64)rset->getDouble(9));
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


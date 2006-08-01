/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "OraStageUpdateNextRequestCnv.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/IClient.hpp"
#include "castor/ICnvSvc.hpp"
#include "castor/IObject.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/NoEntry.hpp"
#include "castor/stager/FileRequest.hpp"
#include "castor/stager/StageUpdateNextRequest.hpp"
#include "castor/stager/SvcClass.hpp"

//------------------------------------------------------------------------------
// Instantiation of a static factory class - should never be used
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::db::ora::OraStageUpdateNextRequestCnv>* s_factoryOraStageUpdateNextRequestCnv =
  new castor::CnvFactory<castor::db::ora::OraStageUpdateNextRequestCnv>();

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------
/// SQL statement for request insertion
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_insertStatementString =
"INSERT INTO StageUpdateNextRequest (parentUuid, flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, id, parent, svcClass, client) VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,ids_seq.nextval,:14,:15,:16) RETURNING id INTO :17";

/// SQL statement for request deletion
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_deleteStatementString =
"DELETE FROM StageUpdateNextRequest WHERE id = :1";

/// SQL statement for request selection
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_selectStatementString =
"SELECT parentUuid, flags, userName, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, id, parent, svcClass, client FROM StageUpdateNextRequest WHERE id = :1";

/// SQL statement for request update
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_updateStatementString =
"UPDATE StageUpdateNextRequest SET parentUuid = :1, flags = :2, userName = :3, euid = :4, egid = :5, mask = :6, pid = :7, machine = :8, svcClassName = :9, userTag = :10, reqId = :11, lastModificationTime = :12 WHERE id = :13";

/// SQL statement for type storage
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_storeTypeStatementString =
"INSERT INTO Id2Type (id, type) VALUES (:1, :2)";

/// SQL statement for type deletion
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_deleteTypeStatementString =
"DELETE FROM Id2Type WHERE id = :1";

/// SQL statement for request insertion into newRequests table
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_insertNewReqStatementString =
"INSERT INTO newRequests (id, type, creation) VALUES (:1, :2, SYSDATE)";

/// SQL update statement for member parent
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_updateFileRequestStatementString =
"UPDATE StageUpdateNextRequest SET parent = :1 WHERE id = :2";

/// SQL existence statement for member svcClass
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_checkSvcClassExistStatementString =
"SELECT id from SvcClass WHERE id = :1";

/// SQL update statement for member svcClass
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_updateSvcClassStatementString =
"UPDATE StageUpdateNextRequest SET svcClass = :1 WHERE id = :2";

/// SQL update statement for member client
const std::string castor::db::ora::OraStageUpdateNextRequestCnv::s_updateIClientStatementString =
"UPDATE StageUpdateNextRequest SET client = :1 WHERE id = :2";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::db::ora::OraStageUpdateNextRequestCnv::OraStageUpdateNextRequestCnv(castor::ICnvSvc* cnvSvc) :
  OraBaseCnv(cnvSvc),
  m_insertStatement(0),
  m_deleteStatement(0),
  m_selectStatement(0),
  m_updateStatement(0),
  m_insertNewReqStatement(0),
  m_storeTypeStatement(0),
  m_deleteTypeStatement(0),
  m_updateFileRequestStatement(0),
  m_checkSvcClassExistStatement(0),
  m_updateSvcClassStatement(0),
  m_updateIClientStatement(0) {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::db::ora::OraStageUpdateNextRequestCnv::~OraStageUpdateNextRequestCnv() throw() {
  reset();
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraStageUpdateNextRequestCnv::reset() throw() {
  //Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_insertStatement);
    deleteStatement(m_deleteStatement);
    deleteStatement(m_selectStatement);
    deleteStatement(m_updateStatement);
    deleteStatement(m_insertNewReqStatement);
    deleteStatement(m_storeTypeStatement);
    deleteStatement(m_deleteTypeStatement);
    deleteStatement(m_updateFileRequestStatement);
    deleteStatement(m_checkSvcClassExistStatement);
    deleteStatement(m_updateSvcClassStatement);
    deleteStatement(m_updateIClientStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_insertStatement = 0;
  m_deleteStatement = 0;
  m_selectStatement = 0;
  m_updateStatement = 0;
  m_insertNewReqStatement = 0;
  m_storeTypeStatement = 0;
  m_deleteTypeStatement = 0;
  m_updateFileRequestStatement = 0;
  m_checkSvcClassExistStatement = 0;
  m_updateSvcClassStatement = 0;
  m_updateIClientStatement = 0;
  }

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraStageUpdateNextRequestCnv::ObjType() {
  return castor::stager::StageUpdateNextRequest::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::db::ora::OraStageUpdateNextRequestCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// fillRep
//------------------------------------------------------------------------------
void castor::db::ora::OraStageUpdateNextRequestCnv::fillRep(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            unsigned int type,
                                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageUpdateNextRequest* obj = 
    dynamic_cast<castor::stager::StageUpdateNextRequest*>(object);
  try {
    switch (type) {
    case castor::OBJ_FileRequest :
      fillRepFileRequest(obj);
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
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (oracle::occi::SQLException e) {
    castor::db::ora::OraBaseObj::handleException(e);
    castor::exception::Internal ex; 
    ex.getMessage() << "Error in fillRep for type " << type
                    << std::endl << e.what() << std::endl;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// fillRepFileRequest
//------------------------------------------------------------------------------
void castor::db::ora::OraStageUpdateNextRequestCnv::fillRepFileRequest(castor::stager::StageUpdateNextRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // Check update statement
  if (0 == m_updateFileRequestStatement) {
    m_updateFileRequestStatement = createStatement(s_updateFileRequestStatementString);
  }
  // Update local object
  m_updateFileRequestStatement->setDouble(1, 0 == obj->parent() ? 0 : obj->parent()->id());
  m_updateFileRequestStatement->setDouble(2, obj->id());
  m_updateFileRequestStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillRepSvcClass
//------------------------------------------------------------------------------
void castor::db::ora::OraStageUpdateNextRequestCnv::fillRepSvcClass(castor::stager::StageUpdateNextRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  if (0 != obj->svcClass()) {
    // Check checkSvcClassExist statement
    if (0 == m_checkSvcClassExistStatement) {
      m_checkSvcClassExistStatement = createStatement(s_checkSvcClassExistStatementString);
    }
    // retrieve the object from the database
    m_checkSvcClassExistStatement->setDouble(1, obj->svcClass()->id());
    oracle::occi::ResultSet *rset = m_checkSvcClassExistStatement->executeQuery();
    if (oracle::occi::ResultSet::END_OF_FETCH == rset->next()) {
      castor::BaseAddress ad;
      ad.setCnvSvcName("DbCnvSvc");
      ad.setCnvSvcType(castor::SVC_DBCNV);
      cnvSvc()->createRep(&ad, obj->svcClass(), false);
    }
    // Close resultset
    m_checkSvcClassExistStatement->closeResultSet(rset);
  }
  // Check update statement
  if (0 == m_updateSvcClassStatement) {
    m_updateSvcClassStatement = createStatement(s_updateSvcClassStatementString);
  }
  // Update local object
  m_updateSvcClassStatement->setDouble(1, 0 == obj->svcClass() ? 0 : obj->svcClass()->id());
  m_updateSvcClassStatement->setDouble(2, obj->id());
  m_updateSvcClassStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillRepIClient
//------------------------------------------------------------------------------
void castor::db::ora::OraStageUpdateNextRequestCnv::fillRepIClient(castor::stager::StageUpdateNextRequest* obj)
  throw (castor::exception::Exception, oracle::occi::SQLException) {
  // Check update statement
  if (0 == m_updateIClientStatement) {
    m_updateIClientStatement = createStatement(s_updateIClientStatementString);
  }
  // Update local object
  m_updateIClientStatement->setDouble(1, 0 == obj->client() ? 0 : obj->client()->id());
  m_updateIClientStatement->setDouble(2, obj->id());
  m_updateIClientStatement->executeUpdate();
}

//------------------------------------------------------------------------------
// fillObj
//------------------------------------------------------------------------------
void castor::db::ora::OraStageUpdateNextRequestCnv::fillObj(castor::IAddress* address,
                                                            castor::IObject* object,
                                                            unsigned int type,
                                                            bool autocommit)
  throw (castor::exception::Exception) {
  castor::stager::StageUpdateNextRequest* obj = 
    dynamic_cast<castor::stager::StageUpdateNextRequest*>(object);
  switch (type) {
  case castor::OBJ_FileRequest :
    fillObjFileRequest(obj);
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
  if (autocommit) {
    cnvSvc()->commit();
  }
}
//------------------------------------------------------------------------------
// fillObjFileRequest
//------------------------------------------------------------------------------
void castor::db::ora::OraStageUpdateNextRequestCnv::fillObjFileRequest(castor::stager::StageUpdateNextRequest* obj)
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
  u_signed64 parentId = (u_signed64)rset->getDouble(15);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
  // Check whether something should be deleted
  if (0 != obj->parent() &&
      (0 == parentId ||
       obj->parent()->id() != parentId)) {
    obj->setParent(0);
  }
  // Update object or create new one
  if (0 != parentId) {
    if (0 == obj->parent()) {
      obj->setParent
        (dynamic_cast<castor::stager::FileRequest*>
         (cnvSvc()->getObjFromId(parentId)));
    } else {
      cnvSvc()->updateObj(obj->parent());
    }
  }
}

//------------------------------------------------------------------------------
// fillObjSvcClass
//------------------------------------------------------------------------------
void castor::db::ora::OraStageUpdateNextRequestCnv::fillObjSvcClass(castor::stager::StageUpdateNextRequest* obj)
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
  u_signed64 svcClassId = (u_signed64)rset->getDouble(16);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
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
void castor::db::ora::OraStageUpdateNextRequestCnv::fillObjIClient(castor::stager::StageUpdateNextRequest* obj)
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
  u_signed64 clientId = (u_signed64)rset->getDouble(17);
  // Close ResultSet
  m_selectStatement->closeResultSet(rset);
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
void castor::db::ora::OraStageUpdateNextRequestCnv::createRep(castor::IAddress* address,
                                                              castor::IObject* object,
                                                              bool autocommit,
                                                              unsigned int type)
  throw (castor::exception::Exception) {
  castor::stager::StageUpdateNextRequest* obj = 
    dynamic_cast<castor::stager::StageUpdateNextRequest*>(object);
  // check whether something needs to be done
  if (0 == obj) return;
  if (0 != obj->id()) return;
  try {
    // Check whether the statements are ok
    if (0 == m_insertStatement) {
      m_insertStatement = createStatement(s_insertStatementString);
      m_insertStatement->registerOutParam(17, oracle::occi::OCCIDOUBLE);
    }
    if (0 == m_insertNewReqStatement) {
      m_insertNewReqStatement = createStatement(s_insertNewReqStatementString);
    }
    if (0 == m_storeTypeStatement) {
      m_storeTypeStatement = createStatement(s_storeTypeStatementString);
    }
    // Now Save the current object
    m_insertStatement->setString(1, obj->parentUuid());
    m_insertStatement->setDouble(2, obj->flags());
    m_insertStatement->setString(3, obj->userName());
    m_insertStatement->setInt(4, obj->euid());
    m_insertStatement->setInt(5, obj->egid());
    m_insertStatement->setInt(6, obj->mask());
    m_insertStatement->setInt(7, obj->pid());
    m_insertStatement->setString(8, obj->machine());
    m_insertStatement->setString(9, obj->svcClassName());
    m_insertStatement->setString(10, obj->userTag());
    m_insertStatement->setString(11, obj->reqId());
    m_insertStatement->setInt(12, time(0));
    m_insertStatement->setInt(13, time(0));
    m_insertStatement->setDouble(14, (type == OBJ_FileRequest && obj->parent() != 0) ? obj->parent()->id() : 0);
    m_insertStatement->setDouble(15, (type == OBJ_SvcClass && obj->svcClass() != 0) ? obj->svcClass()->id() : 0);
    m_insertStatement->setDouble(16, (type == OBJ_IClient && obj->client() != 0) ? obj->client()->id() : 0);
    m_insertStatement->executeUpdate();
    obj->setId((u_signed64)m_insertStatement->getDouble(17));
    m_storeTypeStatement->setDouble(1, obj->id());
    m_storeTypeStatement->setInt(2, obj->type());
    m_storeTypeStatement->executeUpdate();
    m_insertNewReqStatement->setDouble(1, obj->id());
    m_insertNewReqStatement->setInt(2, obj->type());
    m_insertNewReqStatement->executeUpdate();
    if (autocommit) {
      cnvSvc()->commit();
    }
  } catch (oracle::occi::SQLException e) {
      castor::db::ora::OraBaseObj::handleException(e);
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "Error in insert request :"
                      << std::endl << e.what() << std::endl
                      << "Statement was :" << std::endl
                      << s_insertStatementString << std::endl
                      << "and parameters' values were :" << std::endl
                      << "  parentUuid : " << obj->parentUuid() << std::endl
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
                      << "  id : " << obj->id() << std::endl
                      << "  parent : " << obj->parent() << std::endl
                      << "  svcClass : " << obj->svcClass() << std::endl
                      << "  client : " << obj->client() << std::endl;
      throw ex;
    }
}

  //----------------------------------------------------------------------------
  // updateRep
  //----------------------------------------------------------------------------
  void castor::db::ora::OraStageUpdateNextRequestCnv::updateRep(castor::IAddress* address,
                                                                castor::IObject* object,
                                                                bool autocommit)
    throw (castor::exception::Exception) {
    castor::stager::StageUpdateNextRequest* obj = 
      dynamic_cast<castor::stager::StageUpdateNextRequest*>(object);
    // check whether something needs to be done
    if (0 == obj) return;
    try {
      // Check whether the statements are ok
      if (0 == m_updateStatement) {
        m_updateStatement = createStatement(s_updateStatementString);
      }
      // Update the current object
      m_updateStatement->setString(1, obj->parentUuid());
      m_updateStatement->setDouble(2, obj->flags());
      m_updateStatement->setString(3, obj->userName());
      m_updateStatement->setInt(4, obj->euid());
      m_updateStatement->setInt(5, obj->egid());
      m_updateStatement->setInt(6, obj->mask());
      m_updateStatement->setInt(7, obj->pid());
      m_updateStatement->setString(8, obj->machine());
      m_updateStatement->setString(9, obj->svcClassName());
      m_updateStatement->setString(10, obj->userTag());
      m_updateStatement->setString(11, obj->reqId());
      m_updateStatement->setInt(12, time(0));
      m_updateStatement->setDouble(13, obj->id());
      m_updateStatement->executeUpdate();
      if (autocommit) {
        cnvSvc()->commit();
      }
    } catch (oracle::occi::SQLException e) {
        castor::db::ora::OraBaseObj::handleException(e);
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
    void castor::db::ora::OraStageUpdateNextRequestCnv::deleteRep(castor::IAddress* address,
                                                                  castor::IObject* object,
                                                                  bool autocommit)
      throw (castor::exception::Exception) {
      castor::stager::StageUpdateNextRequest* obj = 
        dynamic_cast<castor::stager::StageUpdateNextRequest*>(object);
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
        if (obj->client() != 0) {
          cnvSvc()->deleteRep(0, obj->client(), false);
        }
        if (autocommit) {
          cnvSvc()->commit();
        }
      } catch (oracle::occi::SQLException e) {
          castor::db::ora::OraBaseObj::handleException(e);
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
      castor::IObject* castor::db::ora::OraStageUpdateNextRequestCnv::createObj(castor::IAddress* address)
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
          castor::stager::StageUpdateNextRequest* object = new castor::stager::StageUpdateNextRequest();
          // Now retrieve and set members
          object->setParentUuid(rset->getString(1));
          object->setFlags((u_signed64)rset->getDouble(2));
          object->setUserName(rset->getString(3));
          object->setEuid(rset->getInt(4));
          object->setEgid(rset->getInt(5));
          object->setMask(rset->getInt(6));
          object->setPid(rset->getInt(7));
          object->setMachine(rset->getString(8));
          object->setSvcClassName(rset->getString(9));
          object->setUserTag(rset->getString(10));
          object->setReqId(rset->getString(11));
          object->setCreationTime((u_signed64)rset->getDouble(12));
          object->setLastModificationTime((u_signed64)rset->getDouble(13));
          object->setId((u_signed64)rset->getDouble(14));
          m_selectStatement->closeResultSet(rset);
          return object;
        } catch (oracle::occi::SQLException e) {
            castor::db::ora::OraBaseObj::handleException(e);
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
        void castor::db::ora::OraStageUpdateNextRequestCnv::updateObj(castor::IObject* obj)
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
            castor::stager::StageUpdateNextRequest* object = 
              dynamic_cast<castor::stager::StageUpdateNextRequest*>(obj);
            object->setParentUuid(rset->getString(1));
            object->setFlags((u_signed64)rset->getDouble(2));
            object->setUserName(rset->getString(3));
            object->setEuid(rset->getInt(4));
            object->setEgid(rset->getInt(5));
            object->setMask(rset->getInt(6));
            object->setPid(rset->getInt(7));
            object->setMachine(rset->getString(8));
            object->setSvcClassName(rset->getString(9));
            object->setUserTag(rset->getString(10));
            object->setReqId(rset->getString(11));
            object->setCreationTime((u_signed64)rset->getDouble(12));
            object->setLastModificationTime((u_signed64)rset->getDouble(13));
            object->setId((u_signed64)rset->getDouble(14));
            m_selectStatement->closeResultSet(rset);
          } catch (oracle::occi::SQLException e) {
              castor::db::ora::OraBaseObj::handleException(e);
              castor::exception::InvalidArgument ex;
              ex.getMessage() << "Error in update request :"
                              << std::endl << e.what() << std::endl
                              << "Statement was :" << std::endl
                              << s_updateStatementString << std::endl
                              << "and id was " << obj->id() << std::endl;;
              throw ex;
            }
}


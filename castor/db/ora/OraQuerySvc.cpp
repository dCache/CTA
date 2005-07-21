/******************************************************************************
 *                      OraQuerySvc.cpp
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
 * @(#)$RCSfile: OraQuerySvc.cpp,v $ $Revision: 1.17 $ $Release$ $Date: 2005/07/21 09:13:04 $ $Author: itglp $
 *
 * Implementation of the IQuerySvc for Oracle
 *
 * @author Sebastien Ponce
 *****************************************************************************/

// Include Files
#include "castor/IFactory.hpp"
#include "castor/SvcFactory.hpp"
#include "castor/db/ora/OraQuerySvc.hpp"
#include "castor/stager/DiskCopyInfo.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/stager/DiskCopyStatusCodes.hpp"
#include "castor/stager/TapeCopyStatusCodes.hpp"
#include "castor/stager/SegmentStatusCodes.hpp"

#include <sstream>
#include <string>
#include <list>

// -----------------------------------------------------------------------
// Instantiation of a static factory class
// -----------------------------------------------------------------------
static castor::SvcFactory<castor::db::ora::OraQuerySvc> s_factoryOraQuerySvc;
const castor::IFactory<castor::IService>&
OraQuerySvcFactory = s_factoryOraQuerySvc;

//------------------------------------------------------------------------------
// Static constants initialization
//------------------------------------------------------------------------------

const std::string castor::db::ora::OraQuerySvc::s_diskCopies4FileStatementString =
  "BEGIN fileIdStageQuery(:1, :2, :3, :4); END;";

const std::string castor::db::ora::OraQuerySvc::s_diskCopies4RequestStatementString =
  "BEGIN reqIdStageQuery(:1, :2, :3); END;";

const std::string castor::db::ora::OraQuerySvc::s_diskCopies4UsertagStatementString =
  "BEGIN userTagStageQuery(:1, :2, :3); END;";


// -----------------------------------------------------------------------
// OraQuerySvc
// -----------------------------------------------------------------------
castor::db::ora::OraQuerySvc::OraQuerySvc(const std::string name) :
  OraCommonSvc(name),
  m_diskCopies4FileStatement(0),
  m_diskCopies4RequestStatement(0),
  m_diskCopies4UsertagStatement(0) {}

// -----------------------------------------------------------------------
// ~OraQuerySvc
// -----------------------------------------------------------------------
castor::db::ora::OraQuerySvc::~OraQuerySvc() throw() {
  reset();
}

// -----------------------------------------------------------------------
// id
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraQuerySvc::id() const {
  return ID();
}

// -----------------------------------------------------------------------
// ID
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraQuerySvc::ID() {
  return castor::SVC_ORAQUERYSVC;
}

//------------------------------------------------------------------------------
// reset
//------------------------------------------------------------------------------
void castor::db::ora::OraQuerySvc::reset() throw() {
  // Here we attempt to delete the statements correctly
  // If something goes wrong, we just ignore it
  try {
    deleteStatement(m_diskCopies4FileStatement);
    deleteStatement(m_diskCopies4RequestStatement);
    deleteStatement(m_diskCopies4UsertagStatement);
  } catch (oracle::occi::SQLException e) {};
  // Now reset all pointers to 0
  m_diskCopies4FileStatement = 0;
  m_diskCopies4RequestStatement = 0;
  m_diskCopies4UsertagStatement = 0;
}

// -----------------------------------------------------------------------
// diskCopies4File
// -----------------------------------------------------------------------
std::list<castor::stager::DiskCopyInfo*>
castor::db::ora::OraQuerySvc::diskCopies4File
(std::string fileId, std::string nsHost,
 u_signed64 svcClassId)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_diskCopies4FileStatement) {
      m_diskCopies4FileStatement =
        createStatement(s_diskCopies4FileStatementString);
      m_diskCopies4FileStatement->registerOutParam
        (4, oracle::occi::OCCICURSOR);
    }
    // execute the statement and see whether we found something
    m_diskCopies4FileStatement->setString(1, fileId);
    m_diskCopies4FileStatement->setString(2, nsHost);
    m_diskCopies4FileStatement->setDouble(3, svcClassId);
    unsigned int nb = m_diskCopies4FileStatement->executeUpdate();
    if (0 == nb) {
      castor::exception::Internal ex;
      ex.getMessage()
        << "diskCopies4File : Unable to execute query.";
      throw ex;
    }
    oracle::occi::ResultSet *rset =
      m_diskCopies4FileStatement->getCursor(4);
    // Gather the results
    std::list<castor::stager::DiskCopyInfo*> result;
    while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
      castor::stager::DiskCopyInfo* item =
        new castor::stager::DiskCopyInfo();
      item->setFileId(rset->getInt(1));
      item->setNsHost(rset->getString(2));
      item->setId(rset->getInt(3));
      item->setDiskCopyPath(rset->getString(4));
      item->setSize(rset->getInt(5));
      item->setDiskCopyStatus((castor::stager::DiskCopyStatusCodes)
                              rset->getInt(6));
      item->setTapeCopyStatus((castor::stager::TapeCopyStatusCodes)
                              rset->getInt(7));
      item->setSegmentStatus((castor::stager::SegmentStatusCodes)
                             rset->getInt(8));
      item->setDiskServer(rset->getString(9));
      item->setMountPoint(rset->getString(10));
      result.push_back(item);
    }
    return result;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error caught in diskCopies4File."
                    << std::endl << e.what();
    throw ex;
  }
}



// -----------------------------------------------------------------------
// diskCopies4Request
// -----------------------------------------------------------------------
std::list<castor::stager::DiskCopyInfo*>
castor::db::ora::OraQuerySvc::diskCopies4Request
(std::string requestId, u_signed64 svcClassId)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_diskCopies4RequestStatement) {
      m_diskCopies4RequestStatement =
        createStatement(s_diskCopies4RequestStatementString);
      m_diskCopies4RequestStatement->registerOutParam
        (3, oracle::occi::OCCICURSOR);
    }
    // execute the statement and see whether we found something
    m_diskCopies4RequestStatement->setString(1, requestId);
    m_diskCopies4RequestStatement->setDouble(2, svcClassId);
    unsigned int nb = m_diskCopies4RequestStatement->executeUpdate();
    if (0 == nb) {
      castor::exception::Internal ex;
      ex.getMessage()
        << "diskCopies4File : Unable to execute query.";
      throw ex;
    }
    oracle::occi::ResultSet *rset =
      m_diskCopies4RequestStatement->getCursor(3);
    // Gather the results
    std::list<castor::stager::DiskCopyInfo*> result;
    while (oracle::occi::ResultSet::END_OF_FETCH != rset->next()) {
      castor::stager::DiskCopyInfo* item =
        new castor::stager::DiskCopyInfo();
      item->setFileId(rset->getInt(1));
      item->setNsHost(rset->getString(2));
      item->setId(rset->getInt(3));
      item->setDiskCopyPath(rset->getString(4));
      item->setSize(rset->getInt(5));
      item->setDiskCopyStatus((castor::stager::DiskCopyStatusCodes)
                              rset->getInt(6));
      item->setTapeCopyStatus((castor::stager::TapeCopyStatusCodes)
                              rset->getInt(7));
      item->setSegmentStatus((castor::stager::SegmentStatusCodes)
                             rset->getInt(8));
      item->setDiskServer(rset->getString(9));
      item->setMountPoint(rset->getString(10));
      result.push_back(item);
    }
    return result;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error caught in diskCopies4Request."
                    << std::endl << e.what();
    throw ex;
  }
}


// -----------------------------------------------------------------------
// diskCopies4Usertag
// -----------------------------------------------------------------------
std::list<castor::stager::DiskCopyInfo*>
castor::db::ora::OraQuerySvc::diskCopies4Usertag
(std::string usertag, u_signed64 svcClassId)
  throw (castor::exception::Exception) {
  try {
    // Check whether the statements are ok
    if (0 == m_diskCopies4UsertagStatement) {
      m_diskCopies4UsertagStatement =
        createStatement(s_diskCopies4UsertagStatementString);
      m_diskCopies4UsertagStatement->registerOutParam
        (3, oracle::occi::OCCICURSOR);
    }
    // execute the statement and see whether we found something
    m_diskCopies4UsertagStatement->setString(1, usertag);
    m_diskCopies4UsertagStatement->setDouble(2, svcClassId);
    unsigned int nb = m_diskCopies4UsertagStatement->executeUpdate();
    if (0 == nb) {
      castor::exception::Internal ex;
      ex.getMessage()
        << "diskCopies4File : Unable to execute query.";
      throw ex;
    }
    oracle::occi::ResultSet *rset =
      m_diskCopies4UsertagStatement->getCursor(3);
    // Gather the results
    std::list<castor::stager::DiskCopyInfo*> result;
    oracle::occi::ResultSet::Status status = rset->next();
    while(status == oracle::occi::ResultSet::DATA_AVAILABLE) {
      castor::stager::DiskCopyInfo* item =
        new castor::stager::DiskCopyInfo();

      item->setFileId(rset->getInt(1));
      item->setNsHost(rset->getString(2));
      item->setId(rset->getInt(3));
      item->setDiskCopyPath(rset->getString(4));
      item->setSize(rset->getInt(5));
      item->setDiskCopyStatus((castor::stager::DiskCopyStatusCodes)
                              rset->getInt(6));
      item->setTapeCopyStatus((castor::stager::TapeCopyStatusCodes)
                              rset->getInt(7));
      item->setSegmentStatus((castor::stager::SegmentStatusCodes)
                             rset->getInt(8));
      item->setDiskServer(rset->getString(9));
      item->setMountPoint(rset->getString(10));
      result.push_back(item);
      status = rset->next();
    }
    return result;
  } catch (oracle::occi::SQLException e) {
    castor::exception::Internal ex;
    ex.getMessage() << "Error caught in diskCopies4Usertag."
                    << std::endl << e.what();
    throw ex;
  }
}

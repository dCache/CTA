/******************************************************************************
 *                 castor/tape/tpcp/Migrator.cpp
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
 *
 *
 * @author Nicola.Bessone@cern.ch Steven.Murray@cern.ch
 *****************************************************************************/
 
#include "castor/Constants.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/tape/Constants.hpp"
#include "castor/tape/net/net.hpp"
#include "castor/tape/tapegateway/EndNotification.hpp"
#include "castor/tape/tapegateway/EndNotificationErrorReport.hpp"
#include "castor/tape/tapegateway/FileToMigrate.hpp"
#include "castor/tape/tapegateway/FileToMigrateRequest.hpp"
#include "castor/tape/tapegateway/FileMigratedNotification.hpp"
#include "castor/tape/tapegateway/NoMoreFiles.hpp"
#include "castor/tape/tapegateway/NotificationAcknowledge.hpp"
#include "castor/tape/tapegateway/PositionCommandCode.hpp"
#include "castor/tape/tapegateway/Volume.hpp"
#include "castor/tape/tpcp/Constants.hpp"
#include "castor/tape/tpcp/Migrator.hpp"
#include "castor/tape/tpcp/StreamHelper.hpp"
#include "castor/tape/utils/utils.hpp"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tpcp::Migrator::Migrator(const bool debug,
  TapeFseqRangeList &tapeFseqRanges, FilenameList &filenames,
  const vmgr_tape_info &vmgrTapeInfo, const char *const dgn,
  const int volReqId, castor::io::ServerSocket &callbackSocket) throw() :
  ActionHandler(debug, tapeFseqRanges, filenames, vmgrTapeInfo, dgn, volReqId,
    callbackSocket) {

  // Register the Aggregator message handler member functions
  ActionHandler::registerMsgHandler(OBJ_FileToMigrateRequest,
    &Migrator::handleFileToMigrateRequest, this);
  ActionHandler::registerMsgHandler(OBJ_FileMigratedNotification,
    &Migrator::handleFileMigratedNotification, this);
  ActionHandler::registerMsgHandler(OBJ_EndNotification,
    &Migrator::handleEndNotification, this);
  ActionHandler::registerMsgHandler(OBJ_EndNotificationErrorReport,
    &Migrator::handleEndNotificationErrorReport, this);
}


//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tpcp::Migrator::~Migrator() {
  // Do nothing
}


//------------------------------------------------------------------------------
// run
//------------------------------------------------------------------------------
void castor::tape::tpcp::Migrator::run() throw(castor::exception::Exception) {

  // Spin in the dispatch message loop until there is no more work
  while(ActionHandler::dispatchMessage()) {
    // Do nothing
  }
}


//------------------------------------------------------------------------------
// handleFileToMigrateRequest
//------------------------------------------------------------------------------
bool castor::tape::tpcp::Migrator::handleFileToMigrateRequest(
  castor::IObject *obj, castor::io::AbstractSocket &sock)
  throw(castor::exception::Exception) {

  tapegateway::FileToMigrateRequest *msg = NULL;

  castMessage(obj, msg, sock);
  displayReceivedMessageIfDebug(*msg);

  const bool anotherFile = m_filenameItor != m_filenames.end();

  if(anotherFile) {
    const std::string filename = *(m_filenameItor++);
    struct stat       buf;
    const int         rc = stat(filename.c_str(), &buf);
    if(rc <= 0){
      castor::exception::Exception ex(ENOENT);

      ex.getMessage()
        << "No such file or directory"
           ": file=" << filename.c_str();

      throw ex;
    }

    if(rc != 0) {
    }

    // Create FileToMigrate message for the aggregator
    tapegateway::FileToMigrate fileToMigrate;
    fileToMigrate.setMountTransactionId(m_volReqId);
    fileToMigrate.setFileTransactionId(m_fileTransactionId);

/*
    fileToMigrate.setFileSize();
    fileToMigrate.setLastKnownFilename();
    fileToMigrate.setLastModificationTime();
    fileToMigrate.setPath();
    fileToMigrate.setId();
*/

    // Update the map of current file transfers and increment the file
    // transaction ID
    {
      m_pendingFileTransfers[m_fileTransactionId] = filename;
      m_fileTransactionId++;
    }

    // Send the FileToMigrate message to the aggregator
    sock.sendObject(fileToMigrate);

    // If debug, then display sending of the FileToMigrate message
    if(m_debug) {
      std::ostream &os = std::cout;

      os << "Migrator: Sent FileToMigrate to aggregator = ";
      StreamHelper::write(os, fileToMigrate);
      os << std::endl;
    }

  // Else no more files
  } else {

    // Create the NoMoreFiles message for the aggregator
    castor::tape::tapegateway::NoMoreFiles noMore;
    noMore.setMountTransactionId(m_volReqId);

    // Send the NoMoreFiles message to the aggregator
    sock.sendObject(noMore);

    // If debug, then display sending of the NoMoreFiles message
    if(m_debug) {
      std::ostream &os = std::cout;

      utils::writeBanner(os, "Sent NoMoreFiles to aggregator");
      StreamHelper::write(os, noMore);
      os << std::endl;
    }
  }

  return true;
}


//------------------------------------------------------------------------------
// handleFileMigratedNotification
//------------------------------------------------------------------------------
bool castor::tape::tpcp::Migrator::handleFileMigratedNotification(
  castor::IObject *obj, castor::io::AbstractSocket &sock)
  throw(castor::exception::Exception) {

  tapegateway::FileMigratedNotification *msg = NULL;

  castMessage(obj, msg, sock);
  displayReceivedMessageIfDebug(*msg);

  return true;
}


//------------------------------------------------------------------------------
// handleEndNotification
//------------------------------------------------------------------------------
bool castor::tape::tpcp::Migrator::handleEndNotification(
  castor::IObject *obj, castor::io::AbstractSocket &sock)
  throw(castor::exception::Exception) {

  return ActionHandler::handleEndNotification(obj, sock);
}


//------------------------------------------------------------------------------
// handleEndNotificationErrorReport
//------------------------------------------------------------------------------
bool castor::tape::tpcp::Migrator::handleEndNotificationErrorReport(
  castor::IObject *obj, castor::io::AbstractSocket &sock)
  throw(castor::exception::Exception) {

  return ActionHandler::handleEndNotificationErrorReport(obj, sock);
}

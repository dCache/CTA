/******************************************************************************
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/
 
#include "castor/Constants.hpp"
#include "castor/io/io.hpp"
#include "castor/tape/Constants.hpp"
#include "castor/tape/tapegateway/EndNotification.hpp"
#include "castor/tape/tapegateway/EndNotificationErrorReport.hpp"
#include "castor/tape/tapegateway/FileRecallReportList.hpp"
#include "castor/tape/tapegateway/FilesToRecallList.hpp"
#include "castor/tape/tapegateway/FilesToRecallListRequest.hpp"
#include "castor/tape/tapegateway/FileToRecallStruct.hpp"
#include "castor/tape/tapegateway/NoMoreFiles.hpp"
#include "castor/tape/tapegateway/NotificationAcknowledge.hpp"
#include "castor/tape/tapegateway/PositionCommandCode.hpp"
#include "castor/tape/tapegateway/Volume.hpp"
#include "castor/tape/tpcp/Constants.hpp"
#include "castor/tape/tpcp/Helper.hpp"
#include "castor/tape/tpcp/ReadTpCommand.hpp"
#include "castor/tape/tpcp/TapeFileSequenceParser.hpp"
#include "castor/tape/utils/utils.hpp"
#include "castor/utils/utils.hpp"
#include "h/Ctape_constants.h"
#include "h/Cupv_api.h"

#include <errno.h>
#include <getopt.h>
#include <memory>
#include <sstream>
#include <time.h>
#include <sys/stat.h>


//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::tpcp::ReadTpCommand::ReadTpCommand() throw() :
  TpcpCommand("readtp"),
  m_umask(umask(0)),
  m_nbRecalledFiles(0) {

  // Restore original umask
  umask(m_umask);
}


//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::tpcp::ReadTpCommand::~ReadTpCommand() throw() {
  // Do nothing
}


//------------------------------------------------------------------------------
// usage
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::usage(std::ostream &os) const throw() {
  os <<
    "Usage:\n"
    "\t" << m_programName << " VID SEQUENCE [OPTIONS] [FILE]...\n"
    "\n"
    "Where:\n"
    "\n"
    "\tVID      The VID of the tape to be read from.\n"
    "\tSEQUENCE The sequence of tape file sequence numbers.\n"
    "\tFILE     A filename in the form [host:]local_path.\n"
    "\n"
    "Options:\n"
    "\n"
    "\t-d, --debug         Turn on the printing of debug information.\n"
    "\t-h, --help          Print this help messgae and exit.\n"
    "\t-s, --server server Specifies the tape server to be used, therefore\n"
    "\t                    overriding the drive scheduling of the VDQM.\n"
    "\t-f, --filelist file File containing a list of filenames in the form\n"
    "\t                    [host:]local_path.\n"
    "\t-n, --nodata        Send all data read from tape to the /dev/null of the\n"
    "\t                    tape server.\n"
    "\n"
    "Constraints:\n"
    "\n"
    "\tThe [FILE].. command-line arguments, the -f/--filelist option and\n"
    "\tthe -n/--nodata option are mutually exclusive\n"
    "\n"
    "Comments to: Castor.Support@cern.ch" << std::endl;
}


//------------------------------------------------------------------------------
// parseCommandLine
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::parseCommandLine(const int argc,
  char **argv)  {

  static struct option longopts[] = {
    {"debug"   , 0, NULL, 'd'},
    {"filelist", 1, NULL, 'f'},
    {"help"    , 0, NULL, 'h'},
    {"server"  , 1, NULL, 's'},
    {"nodata"  , 0, NULL, 'n'},
    {NULL      , 0, NULL,  0 }
  };

  optind = 1;
  opterr = 0;

  char c;

  while((c = getopt_long(argc, argv, ":df:hs:n", longopts, NULL)) != -1) {

    switch (c) {
    case 'd':
      m_cmdLine.debugSet = true;
      break;

    case 'f':
      m_cmdLine.fileListSet = true;
      m_cmdLine.fileListFilename = optarg;
      break;

    case 'h':
      m_cmdLine.helpSet = true;
      break;

    case 's':
      m_cmdLine.serverSet = true;
      try {
        castor::utils::copyString(m_cmdLine.server, optarg);
      } catch(castor::exception::Exception &ne) {
        castor::exception::Exception ex;
        ex.getMessage() <<
          "Failed to copy the argument of the server command-line option"
          " into the internal data structures"
          ": " << ne.getMessage().str();
        throw ex;
      }
      break;

    case 'n':
      m_cmdLine.nodataSet = true;
      break;

    case ':':
      {
        castor::exception::InvalidArgument ex;
        ex.getMessage() << "\tThe -" << (char)optopt
          << " option requires a parameter";
        throw ex;
      }
      break;

    case '?':
      {
        castor::exception::InvalidArgument ex;

        if(optopt == 0) {
          ex.getMessage() << "\tUnknown command-line option";
        } else {
          ex.getMessage() << "\tUnknown command-line option: -" << (char)optopt;
        }
        throw ex;
      }
      break;

    default:
      {
        castor::exception::Exception ex;
        ex.getMessage() <<
          "\tgetopt_long returned the following unknown value: 0x" <<
          std::hex << (int)c;
        throw ex;
      }
    } // switch (c)
  } // while ((c = getopt_long(argc, argv, "h", longopts, NULL)) != -1)

  // There is no need to continue parsing when the help option is set
  if( m_cmdLine.helpSet) {
    return;
  }

  // Calculate the number of non-option ARGV-elements
  const int nbArgs = argc-optind;

  // Check the VID has been specified
  if(nbArgs < 1) {
    castor::exception::InvalidArgument ex;

    ex.getMessage() <<
      "\tThe VID and SEQUENCE have not been specified";

    throw ex;
  }

  // Check the SEQUENCE of tape file sequence numbers has been specified
  if(nbArgs < 2) {
    castor::exception::InvalidArgument ex;

    ex.getMessage() <<
      "\tThe SEQUENCE of tape file sequence numbers has not been specified";

    throw ex;
  }

  // If no filenames have been specified on the command-line and the
  // -f/--filelist option has not been set and the
  // -n/--nodata option has not been set
  if(nbArgs < 3 && !m_cmdLine.fileListSet && !m_cmdLine.nodataSet){
    castor::exception::InvalidArgument ex;

    ex.getMessage() <<
      "\tNo destination file has been specified";

    throw ex;
  }

  // -2 = -1 for the VID and -1 for the SEQUENCE
  const int nbFilenamesOnCommandLine = argc - optind - 2;

  // Filenames on the command-line, the -f/--filelist option and the
  // -n/--nodata option are mutually exclusive
  if( (nbFilenamesOnCommandLine > 0 && m_cmdLine.fileListSet)||
      (nbFilenamesOnCommandLine > 0 && m_cmdLine.nodataSet)||
      (m_cmdLine.fileListSet && m_cmdLine.nodataSet) ){
    castor::exception::InvalidArgument ex;

    ex.getMessage() <<
      "\t[FILE].. command-line arguments, the -f/--filelist option\n"
      "\tand the -n/--nodata option are mutually exclusive";

    throw ex;
  }

  // Check the first command-line argument is syntactically a valid VID
  try {
    castor::utils::checkVidSyntax(argv[optind]);
  } catch(castor::exception::InvalidArgument &ex) {
    castor::exception::InvalidArgument ex2;

    ex2.getMessage() <<
      "\tFirst command-line argument must be a valid VID:\n"
      "\t" << ex.getMessage().str();

    throw ex2;
  }

  // Parse the VID command-line argument
  try {
    castor::utils::copyString(m_cmdLine.vid, argv[optind]);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Exception ex;
    ex.getMessage() <<
      "Failed to copy VID comand-line argument into the internal data"
      " structures"
      ": " << ne.getMessage().str();
    throw ex;
  }

  // Move on to the next command-line argument
  optind++;

  // Parse the SEQUENCE command-line argument
  TapeFileSequenceParser::parse(argv[optind], m_cmdLine.tapeFseqRanges);

  // Move on to the next command-line argument (there may not be one)
  optind++;

  // Parse any filenames on the command-line
  while(optind < argc) {
    m_cmdLine.filenames.push_back(argv[optind++]);
  }

  // Check that there is at least one file to be recalled
  if(m_cmdLine.tapeFseqRanges.size() == 0) {
    castor::exception::InvalidArgument ex;

    ex.getMessage() <<
      "\tThere must be at least one tape file sequence number";

    throw ex;
  }

  // Check that there is no more than one tape file sequence range which goes
  // to END of the tape
  const unsigned int nbRangesWithEnd = countNbRangesWithEnd();
  if(nbRangesWithEnd > 1) {
    castor::exception::InvalidArgument ex;

    ex.getMessage() <<
      "\tThere cannot be more than one tape file sequence range whose upper "
      "boundary is end of tape";

    throw ex;
  }
}


//------------------------------------------------------------------------------
// checkAccessToDisk
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::checkAccessToDisk()
  const  {
  // If there are no disk files then throw no exceptions and return
  if(m_cmdLine.nodataSet) {
    return;
  }

  // Determine the number of tape files based on the sequence of tape
  // file sequence numbers they would produce
  //
  // Note that the number maybe infinite if the user has requested to read
  // until the end of the tape
  TapeFseqRangeListSequence seq(&m_cmdLine.tapeFseqRanges);

  // Throw an exception if the number of tape files is finite and does not
  // match the number of disk files
  if(seq.isFinite() && seq.totalSize() != m_filenames.size()) {
    castor::exception::InvalidArgument ex;

    ex.getMessage() <<
      "The number of tape files does not match the number of disk files"
      ": Number of tape files=" << seq.totalSize() <<
      " Number of disk files=" << m_filenames.size();

    throw ex;
  }

  // Sanity check - there should be at least one file to be recalled
  if(m_filenames.empty()) {
    castor::exception::Exception ex;
    ex.getMessage() <<
      "List of filenames to be processed is unexpectedly empty";
    throw ex;
  }
}


//------------------------------------------------------------------------------
// checkAccessToTape
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::checkAccessToTape()
  const  {
  const bool userIsTapeOperator =
    Cupv_check(m_userId, m_groupId, m_hostname, "TAPE_SERVERS",
      P_TAPE_OPERATOR) == 0 ||
    Cupv_check(m_userId, m_groupId, m_hostname, NULL          ,
      P_TAPE_OPERATOR) == 0;

  // Only tape-operators can read disabled tapes
  if(m_vmgrTapeInfo.status & DISABLED) {
    if(!userIsTapeOperator) {
      castor::exception::Exception ex(ECANCELED);
      std::ostream &os = ex.getMessage();
      os << "Tape is not available for reading"
        ": Tape is DISABLED and user is not a tape-operator";
      throw ex;
    }
  }

  if(m_vmgrTapeInfo.status & EXPORTED ||
    m_vmgrTapeInfo.status & ARCHIVED) {
    castor::exception::Exception ex(ECANCELED);
    std::ostream &os = ex.getMessage();
    os << "Tape is not available for reading"
      ": Tape is";
    if(m_vmgrTapeInfo.status & EXPORTED) os << " EXPORTED";
    if(m_vmgrTapeInfo.status & ARCHIVED) os << " ARCHIVED";
    throw ex;
  }
}


//------------------------------------------------------------------------------
// requestDriveFromVdqm
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::requestDriveFromVdqm(
  char *const tapeServer)  {
  TpcpCommand::requestDriveFromVdqm(WRITE_DISABLE, tapeServer);
}


//------------------------------------------------------------------------------
// sendVolumeToTapeServer
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::sendVolumeToTapeServer(
  const tapegateway::VolumeRequest &volumeRequest,
  castor::io::AbstractTCPSocket &connection) const {
  castor::tape::tapegateway::Volume volumeMsg;
  volumeMsg.setVid(m_vmgrTapeInfo.vid);
  volumeMsg.setClientType(castor::tape::tapegateway::READ_TP);
  volumeMsg.setMode(castor::tape::tapegateway::READ);
  volumeMsg.setLabel(m_vmgrTapeInfo.lbltype);
  volumeMsg.setMountTransactionId(m_volReqId);
  volumeMsg.setAggregatorTransactionId(volumeRequest.aggregatorTransactionId());
  volumeMsg.setDensity(m_vmgrTapeInfo.density);

  // Send the volume message to the tape server
  connection.sendObject(volumeMsg);

  Helper::displaySentMsgIfDebug(volumeMsg, m_cmdLine.debugSet);
}


//------------------------------------------------------------------------------
// performTransfer
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::performTransfer() {

  m_tapeFseqSequence.reset(&m_cmdLine.tapeFseqRanges);

  // Spin in the wait for and dispatch message loop until there is no more work
  while(waitForMsgAndDispatchHandler()) {
    // Do nothing
  }

  const uint64_t nbRecallRequests      = m_fileTransactionId - 1;
  const uint64_t nbIncompleteTransfers = m_pendingFileTransfers.size();

  std::ostream &os = std::cout;

  time_t now = time(NULL);
  castor::utils::writeTime(os, now, TIMEFORMAT);
  os << " Finished reading from tape " << m_cmdLine.vid << std::endl
     << std::endl
     << "Number of files to be recalled = ";
  if(m_tapeFseqSequence.isFinite()) {
    os << m_tapeFseqSequence.totalSize();
  } else {
    os << "until EOT";
  }
  os << std::endl
     << "Number of recall requests      = " << nbRecallRequests  << std::endl
     << "Number of successfull recalls  = " << m_nbRecalledFiles << std::endl
     << "Number of incomplete transfers = " << nbIncompleteTransfers
     << std::endl;

  if(m_pendingFileTransfers.size() > 0) {
    os << std::endl;
  }

  for(FileTransferMap::iterator itor=m_pendingFileTransfers.begin();
    itor!=m_pendingFileTransfers.end(); itor++) {

    uint64_t     fileTransactionId = itor->first;
    FileTransfer &fileTransfer     = itor->second;

    os << "Incomplete transfer: fileTransactionId=" << fileTransactionId
       << " tapeFseq=" << fileTransfer.tapeFseq
       << " filename=" << fileTransfer.filename
       << std::endl;
  }

  os << std::endl;
}


//------------------------------------------------------------------------------
// dispatchMsgHandler
//------------------------------------------------------------------------------
bool castor::tape::tpcp::ReadTpCommand::dispatchMsgHandler(
  castor::IObject *const obj, castor::io::AbstractSocket &sock)
   {
  switch(obj->type()) {
  case OBJ_FilesToRecallListRequest:
    return handleFilesToRecallListRequest(obj, sock);
  case OBJ_FileRecallReportList:
    return handleFileRecallReportList(obj, sock);
  case OBJ_EndNotification:
    return handleEndNotification(obj, sock);
  case OBJ_EndNotificationErrorReport:
    return handleEndNotificationErrorReport(obj, sock);
  case OBJ_PingNotification:
    return handlePingNotification(obj, sock);
  default:
    {
      std::stringstream oss;

      oss <<
        "Received unexpected tape-server message"
        ": Message type = " << Helper::objectTypeToString(obj->type());

      const uint64_t transactionId = 0; // Unknown transaction ID
      sendEndNotificationErrorReport(transactionId, EBADMSG,
        oss.str(), sock);

      castor::exception::Exception ex;
      ex.getMessage() << oss.str();
      throw ex;
    }
  }
}


//------------------------------------------------------------------------------
// countNbRangesWithEnd
//------------------------------------------------------------------------------
unsigned int castor::tape::tpcp::ReadTpCommand::countNbRangesWithEnd() {

  unsigned int count = 0;

  // Loop through all ranges
  for(TapeFseqRangeList::const_iterator itor=
    m_cmdLine.tapeFseqRanges.begin();
    itor!=m_cmdLine.tapeFseqRanges.end(); itor++) {

    if(itor->upper() == 0 ){
        count++;
    }
  }
  return count;
}


//------------------------------------------------------------------------------
// handleFilesToRecallListRequest
//------------------------------------------------------------------------------
bool castor::tape::tpcp::ReadTpCommand::handleFilesToRecallListRequest(
  castor::IObject *const obj, castor::io::AbstractSocket &sock) {

  const tapegateway::FilesToRecallListRequest *msg = NULL;

  castMessage(obj, msg, sock);
  Helper::displayRcvdMsgIfDebug(*msg, m_cmdLine.debugSet);

  // Note that in case of -n/--nodata, m_filenames would by empty.
  const bool anotherFile = m_tapeFseqSequence.hasMore() &&
    (m_cmdLine.nodataSet ? true : m_filenameItor != m_filenames.end());

  if(anotherFile) {

    // If the opion -n/--nodata is set, the destination filename is hardcoded
    // to NODATAFILENAME = localhost:/dev/null
    const std::string filename = m_cmdLine.nodataSet ? NODATAFILENAME :
      *(m_filenameItor++);

    // Get the tape file sequence number and disk file-name
    const uint32_t tapeFseq = m_tapeFseqSequence.next();

    // Create a FilesToRecallList message for the tape server
    std::auto_ptr<tapegateway::FileToRecallStruct> file(
      new tapegateway::FileToRecallStruct());
    file->setFileTransactionId(m_fileTransactionId);
    file->setNshost("tpcp\0");
    file->setFileid(0);
    file->setFseq(tapeFseq);
    file->setPositionCommandCode(tapegateway::TPPOSIT_FSEQ);
    file->setPath(filename);
    file->setUmask(m_umask);
    file->setBlockId0(0);
    file->setBlockId1(0);
    file->setBlockId2(0);
    file->setBlockId3(0);
    tapegateway::FilesToRecallList fileList;
    fileList.setMountTransactionId(m_volReqId);
    fileList.setAggregatorTransactionId(msg->aggregatorTransactionId());
    fileList.addFilesToRecall(file.release());

    // Update the map of current file transfers and increment the file
    // transaction ID
    {
      FileTransfer fileTransfer = {tapeFseq, filename};
      m_pendingFileTransfers[m_fileTransactionId] = fileTransfer;
      m_fileTransactionId++;
    }

    // Send the FilesToRecallList message to the tape server
    sock.sendObject(fileList);

    {
      // Command-line user feedback
      std::ostream &os = std::cout;

      time_t now = time(NULL);
      castor::utils::writeTime(os, now, TIMEFORMAT);
      os <<
        " Recalling"
        " \"" << filename << "\"" <<
        " fseq=" << tapeFseq <<  std::endl;
    }

    Helper::displaySentMsgIfDebug(fileList, m_cmdLine.debugSet);

  // Else no more files
  } else {

    // Create the NoMoreFiles message for the tape server
    castor::tape::tapegateway::NoMoreFiles noMore;
    noMore.setMountTransactionId(m_volReqId);
    noMore.setAggregatorTransactionId(msg->aggregatorTransactionId());

    // Send the NoMoreFiles message to the tape server
    sock.sendObject(noMore);

    Helper::displaySentMsgIfDebug(noMore, m_cmdLine.debugSet);
  }

  return true;
}


//------------------------------------------------------------------------------
// handleFileRecallReportList
//------------------------------------------------------------------------------
bool castor::tape::tpcp::ReadTpCommand::handleFileRecallReportList(
  castor::IObject *const obj, castor::io::AbstractSocket &sock) {

  tapegateway::FileRecallReportList *msg = NULL;

  castMessage(obj, msg, sock);
  Helper::displayRcvdMsgIfDebug(*msg, m_cmdLine.debugSet);

  handleSuccessfulRecalls(msg->aggregatorTransactionId(),
    msg->successfulRecalls(), sock);

  TpcpCommand::handleFailedTransfers(msg->failedRecalls());

  // Create the NotificationAcknowledge message for the tape server
  castor::tape::tapegateway::NotificationAcknowledge acknowledge;
  acknowledge.setMountTransactionId(m_volReqId);
  acknowledge.setAggregatorTransactionId(msg->aggregatorTransactionId());

  // Send the NotificationAcknowledge message to the tape server
  sock.sendObject(acknowledge);

  Helper::displaySentMsgIfDebug(acknowledge, m_cmdLine.debugSet);

  return true;
}


//------------------------------------------------------------------------------
// handleSuccessfulRecalls
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::handleSuccessfulRecalls(
  const uint64_t transactionId,
  const std::vector<tapegateway::FileRecalledNotificationStruct*> &files,
  castor::io::AbstractSocket &sock) {
  for(std::vector<tapegateway::FileRecalledNotificationStruct*>::const_iterator
    itor = files.begin(); itor != files.end(); itor++) {

    if(NULL == *itor) {
      castor::exception::Exception ex;
      ex.getMessage() <<
        "Pointer to FileRecalledNotificationStruct is NULL";
      throw ex;
    }

    handleSuccessfulRecall(transactionId, *(*itor), sock);
  }
}


//------------------------------------------------------------------------------
// handleSuccessfulRecall
//------------------------------------------------------------------------------
void castor::tape::tpcp::ReadTpCommand::handleSuccessfulRecall(
  const uint64_t transactionId,
  const tapegateway::FileRecalledNotificationStruct &file,
  castor::io::AbstractSocket &sock) {
  // Check the file transaction ID
  {
    FileTransferMap::iterator itor =
      m_pendingFileTransfers.find(file.fileTransactionId());

    // If the fileTransactionId is unknown
    if(itor == m_pendingFileTransfers.end()) {
      std::stringstream oss;

      oss <<
        "Received unknown file transaction ID from the tape server"
        ": fileTransactionId=" << file.fileTransactionId();

      sendEndNotificationErrorReport(transactionId, EBADMSG,
        oss.str(), sock);

      castor::exception::Exception ex(ECANCELED);

      ex.getMessage() << oss.str();
      throw ex;
    }

    // Command-line user feedback
    {
      std::ostream &os           = std::cout;
      FileTransfer &fileTransfer = itor->second;

      time_t now = time(NULL);
      castor::utils::writeTime(os, now, TIMEFORMAT);
      os <<
        " Recalled"
        " \"" << fileTransfer.filename << "\""
        " fseq=" << fileTransfer.tapeFseq <<
        " size=" << file.fileSize() <<
        " checksumType=\"" << file.checksumName() << "\"" <<
        " checksum=0x" << std::hex << file.checksum() << std::dec <<
        std::endl;
    }

    // The file has been transfer so remove it from the map of pending
    // transfers
    m_pendingFileTransfers.erase(itor);
  }

  // Update the count of successfull recalls
  m_nbRecalledFiles++;
}


//------------------------------------------------------------------------------
// handleEndNotification
//------------------------------------------------------------------------------
bool castor::tape::tpcp::ReadTpCommand::handleEndNotification(
  castor::IObject *const obj, castor::io::AbstractSocket &sock) {
  return TpcpCommand::handleEndNotification(obj, sock);
}


//------------------------------------------------------------------------------
// handleEndNotificationErrorReport
//------------------------------------------------------------------------------
bool castor::tape::tpcp::ReadTpCommand::handleEndNotificationErrorReport(
  castor::IObject *const obj, castor::io::AbstractSocket &sock) {
  return TpcpCommand::handleEndNotificationErrorReport(obj,sock);
}


//------------------------------------------------------------------------------
// handlePingNotification
//------------------------------------------------------------------------------
bool castor::tape::tpcp::ReadTpCommand::handlePingNotification(
  castor::IObject *const obj, castor::io::AbstractSocket &sock) {
  return TpcpCommand::handlePingNotification(obj,sock);
}

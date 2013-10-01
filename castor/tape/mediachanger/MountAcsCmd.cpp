/******************************************************************************
 *                 castor/tape/mediachanger/MountAcsCmd.cpp
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#include "castor/tape/mediachanger/MountAcsCmd.hpp"
#include "castor/tape/utils/utils.hpp"

#include <getopt.h>
#include <iostream>
 
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::tape::mediachanger::MountAcsCmd::MountAcsCmd(
  std::istream &inStream, std::ostream &outStream, std::ostream &errStream,
  Acs &acs) throw():
  AcsCmd(inStream, outStream, errStream, acs) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::tape::mediachanger::MountAcsCmd::~MountAcsCmd() throw() {
  // Do nothing
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int castor::tape::mediachanger::MountAcsCmd::main(const int argc,
  char *const *const argv) throw() {
  try {
    m_cmdLine = parseCmdLine(argc, argv);
  } catch(castor::exception::InvalidArgument &ia) {
    m_err << "Aborting: Invalid command-line: " << ia.getMessage().str() <<
      std::endl;
    m_err << std::endl;
    usage(m_err);
    return 1;
  } catch(castor::exception::MissingOperand &mo) {
    m_err << "Aborting: Missing operand: " << mo.getMessage().str() <<
      std::endl;
    m_err << std::endl;
    usage(m_err);
    return 1;
  } catch(castor::exception::Internal &ie) {
    m_err << "Aborting: Internal error: " << ie.getMessage().str() <<
      std::endl;
    return 1;
  }

  // Display the usage message to standard out and exit with success if the
  // user requested help
  if(m_cmdLine.help) {
    usage(m_out);
    return 0;
  }

  if(m_cmdLine.debug) {
    m_out << "DEBUG: Using a query interval of " << m_cmdLine.queryInterval << 
      " seconds" << std::endl;
    m_out << "DEBUG: Using a readOnly value of " << 
      (m_cmdLine.readOnly ? "TRUE" : "FALSE") << std::endl;
    m_out << "DEBUG: Using a timeout of " << m_cmdLine.timeout << " seconds" << 
      std::endl;
    m_out << "DEBUG: Using volume identifier " << m_cmdLine.volId.external_label
      << std::endl;
    m_out << "DEBUG: Using drive " << m_acs.driveId2Str(m_cmdLine.driveId) <<
      std::endl;
  }

  try {
    syncMount();
  } catch(castor::exception::Exception &ex) {
    m_err << "Aborting: " << ex.getMessage().str() << std::endl;
    return 1;
  }

  return 0;
}

//------------------------------------------------------------------------------
// parseCmdLine
//------------------------------------------------------------------------------
castor::tape::mediachanger::MountAcsCmdLine
  castor::tape::mediachanger::MountAcsCmd::parseCmdLine(
  const int argc, char *const *const argv)
  throw(castor::exception::Internal, castor::exception::InvalidArgument,
    castor::exception::MissingOperand) {

  static struct option longopts[] = {
    {"debug", 0, NULL, 'd'},
    {"help" , 0, NULL, 'h'},
    {"query" , required_argument, NULL, 'q'},
    {"readonly" , 0, NULL, 'r'},
    {"timeout" , required_argument, NULL, 't'},
    {NULL, 0, NULL, 0}
  };
  MountAcsCmdLine cmdLine;
  char c;

  // Set the query option to default value of 10 seconds.
  cmdLine.queryInterval = 10;

  // Set timeout option to the default value of 600 seconds
  cmdLine.timeout = 600;

  // Prevent getopt() from printing an error message if it does not recognize
  // an option character
  opterr = 0;
  while((c = getopt_long(argc, argv, "dhq:rt:", longopts, NULL)) != -1) {

    switch (c) {
    case 'd':
      cmdLine.debug = TRUE;
      break;
    case 'h':
      cmdLine.help = TRUE;
      break;
    case 'q':
      cmdLine.queryInterval = atoi(optarg);
      if(0 >= cmdLine.queryInterval) {
        castor::exception::InvalidArgument ex;
        ex.getMessage() << "\tQuery value must be an integer greater than 0"
          ": value=" << cmdLine.queryInterval;
        throw ex;
      }
      break;
    case 'r':
      cmdLine.readOnly = TRUE;
      break;
    case 't':
      cmdLine.timeout = atoi(optarg);
      if(0 >= cmdLine.timeout) {
        castor::exception::InvalidArgument ex;
        ex.getMessage() << "\tTimeout value must be an integer greater than 0"
          ": value=" << cmdLine.timeout;
        throw ex;
      }
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
        castor::exception::Internal ex;
        ex.getMessage() <<
          "\tgetopt_long returned the following unknown value: 0x" <<
          std::hex << (int)c;
        throw ex;
      }
    } // switch (c)
  } // while ((c = getopt_long( ... )) != -1)

  // There is no need to continue parsing when the help option is set
  if(cmdLine.help) {
    return cmdLine;
  }

  // Calculate the number of non-option ARGV-elements
  const int nbArgs = argc - optind;

  // Check that both VID and DRIVE has been specified
  if(nbArgs < 2) {
    castor::exception::MissingOperand ex;
    ex.getMessage() << "Both VID and DRIVE must be specified";
    throw ex;
  }

  // Parse the VID command-line argument
  cmdLine.volId = m_acs.str2Volid(argv[optind]);

  // Move on to the next command-line argument
  optind++;

  // Parse the DRIVE command-line argument
  cmdLine.driveId = m_acs.str2DriveId(argv[optind]);

  return cmdLine;
}

//------------------------------------------------------------------------------
// usage
//------------------------------------------------------------------------------
void castor::tape::mediachanger::MountAcsCmd::usage(std::ostream &os)
  const throw() {
  os <<
  "Usage:\n"
  "  mountacs [options] VID DRIVE\n"
  "\n"
  "Where:\n"
  "\n"
  "  VID    The VID of the tape to be mounted.\n"
  "  DRIVE  The ID of the drive in which the tape is to be mounted.\n"
  "         Drive ID format is ACS:LSM:panel:transport\n"
  "\n"
  "Options:\n"
  "\n"
  "  -d|--debug            Turn on the printing of debug information.\n"
  "  -h|--help             Print this help message and exit.\n"
  "  -q|--query SECONDS    Time to wait between queries to ACS for responses.\n"
  "                        SECONDS must be an integer value greater than 0.\n"
  "                        The default value of SECONDS in 10.\n"
  "  -r|--readOnly         Request the tape is mounted for read-only access\n"
  "  -t|--timeout SECONDS  Time to wait for the mount to conclude.\n"
  "  -t|--timeout SECONDS  Time to wait for the dismount to conclude. SECONDS\n"
  "                        must be an integer value greater than 0.  The\n"
  "                        default value of SECONDS in 300.\n"
  "\n"
  "Comments to: Castor.Support@cern.ch" << std::endl;
}

//------------------------------------------------------------------------------
// syncMount
//------------------------------------------------------------------------------
void castor::tape::mediachanger::MountAcsCmd::syncMount()
  throw(castor::exception::MountFailed) {
  std::ostringstream action;
  action << "mount tape " << m_cmdLine.volId.external_label << " into drive " <<
    m_acs.driveId2Str(m_cmdLine.driveId) << ": readOnly=" <<
    (m_cmdLine.readOnly ? "TRUE" : "FALSE");

  const SEQ_NO mountSeqNumber = 1;
  const LOCKID mountLockId = 0; // No lock
  const BOOLEAN mountBypass = FALSE;
  if(m_cmdLine.debug) m_out << "DEBUG: Calling Acs::mount()" << std::endl;
  const STATUS mountStatus = m_acs.mount(mountSeqNumber, mountLockId,
    m_cmdLine.volId, m_cmdLine.driveId, m_cmdLine.readOnly, mountBypass);
  if(m_cmdLine.debug) m_out << "DEBUG: Acs::mount() returned " <<
    acs_status(mountStatus) << std::endl;
  if(STATUS_SUCCESS != mountStatus) {
    castor::exception::MountFailed ex;
    ex.getMessage() << "Failed to " << action << ": " <<
      acs_status(mountStatus);
    throw ex;
  }

  // Get all responses until RT_FINAL
  ALIGNED_BYTES responseBuf[MAX_MESSAGE_SIZE / sizeof(ALIGNED_BYTES)];
  SEQ_NO responseSeqNumber = (SEQ_NO)0;
  REQ_ID reqId = (REQ_ID)0;
  ACS_RESPONSE_TYPE responseType = RT_NONE;
  int elapsedMountTime = 0;
  do {
    if(m_cmdLine.debug) m_out << "DEBUG: Calling Acs::response()" << std::endl;
    const int remainingTime = m_cmdLine.timeout - elapsedMountTime;
    const int responseTimeout = remainingTime > m_cmdLine.queryInterval ?
      m_cmdLine.queryInterval : remainingTime;
    const time_t startTime = time(NULL);
    const STATUS status = m_acs.response(responseTimeout, responseSeqNumber,
      reqId, responseType, responseBuf);
    elapsedMountTime += time(NULL) - startTime;
    if(STATUS_SUCCESS != status && STATUS_PENDING != status) {
      castor::exception::MountFailed ex;
      ex.getMessage() << "Failed to " << action.str() <<
        ": Failed to request library response: " << acs_status(status);
      throw ex;
    }
    if(m_cmdLine.debug && STATUS_SUCCESS == status &&
      RT_ACKNOWLEDGE == responseType) {
      m_out << "DEBUG: Received RT_ACKNOWLEDGE: responseSeqNumber=" <<
        responseSeqNumber << " reqId=" << reqId << std::endl;
    }
    if(m_cmdLine.debug) m_out << "DEBUG: Acs::response() returned " <<
      acs_status(status) << std::endl;

    if(elapsedMountTime >= m_cmdLine.timeout) {
      castor::exception::MountFailed ex;
      ex.getMessage() << "Failed to " << action.str() << ": Timed out:"
        " mountTimeout=" << m_cmdLine.timeout << " seconds";
      throw(ex);
    }
  } while(RT_FINAL != responseType);
  if(m_cmdLine.debug) m_out << "DEBUG: Received RT_FINAL: responseSeqNumber=" <<
    responseSeqNumber << " reqId=" << reqId << std::endl;

  if(mountSeqNumber != responseSeqNumber) {
    castor::exception::MountFailed ex;
    ex.getMessage() << "Failed to " << action.str() << 
      ": Invalid RT_FINAL message: Sequence number mismatch: mountSeqNumber=" 
      << mountSeqNumber << " responseSeqNumber=" << responseSeqNumber;
    throw(ex);
  }

  const ACS_DISMOUNT_RESPONSE *const response =
    (ACS_DISMOUNT_RESPONSE *)responseBuf;

  if(STATUS_SUCCESS != response->dismount_status) {
    castor::exception::MountFailed mf;
    mf.getMessage() << "Failed to " << action.str() << ": " <<
      acs_status(response->dismount_status);
    throw(mf);
  }
}

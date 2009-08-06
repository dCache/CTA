/******************************************************************************
 *                      castor/tape/aggregator/utils.cpp
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
 *
 * @author Nicola.Bessone@cern.ch Steven.Murray@cern.ch
 *****************************************************************************/

#include "castor/Constants.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/tape/Constants.hpp"
#include "castor/tape/utils/utils.hpp"
#include "h/Castor_limits.h"
#include "h/rtcp_constants.h"

#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>


//-----------------------------------------------------------------------------
// writeTime
//-----------------------------------------------------------------------------
void castor::tape::utils::writeTime(std::ostream &os, const time_t time,
  const char* const format) {

  tm localTime;

  localtime_r(&time, &localTime);

  const std::time_put<char>& dateWriter =
    std::use_facet<std::time_put<char> >(os.getloc());
  const size_t n = strlen(format);

  if (dateWriter.put(os, os, ' ', &localTime, format, format + n).failed()){
    os << "UKNOWN";
  }
}

//-----------------------------------------------------------------------------
// boolToString
//-----------------------------------------------------------------------------
const char *castor::tape::utils::boolToString(const bool value) {
  return value ? "TRUE" : "FALSE";
}


//-----------------------------------------------------------------------------
// toHex
//-----------------------------------------------------------------------------
void castor::tape::utils::toHex(uint32_t number, char *buf, size_t len)
  throw(castor::exception::InvalidArgument) {

  if(len < 9) {
    castor::exception::InvalidArgument ex;

    ex.getMessage() <<
      "Failed to convert " << number << " to hex"
      ": The output string is too small: Actual=" << len <<
      " Minimum=9";

    throw(ex);
  }

  int digitAsInt = 0;

  for(int i=0; i<8; i++) {
    digitAsInt = number % 16;

    buf[7-i] = digitAsInt < 10 ? '0' + digitAsInt : 'a' + (digitAsInt-10);

    number /= 16;
  }

  buf[8] = '\0';
}


//-----------------------------------------------------------------------------
// splitString
//-----------------------------------------------------------------------------
void castor::tape::utils::splitString(const std::string &str,
  const char separator, std::vector<std::string> &result) throw() {

  std::string::size_type beginIndex = 0;
  std::string::size_type endIndex   = str.find(separator);

  while(endIndex != std::string::npos) {
    result.push_back(str.substr(beginIndex, endIndex - beginIndex));
    beginIndex = ++endIndex;
    endIndex = str.find(separator, endIndex);
  }

  if(endIndex == std::string::npos) {
    result.push_back(str.substr(beginIndex, str.length()));
  }
}


//-----------------------------------------------------------------------------
// countOccurrences
//-----------------------------------------------------------------------------
int castor::tape::utils::countOccurrences(const char ch, const char *str) {

  int  count   = 0;    // The number of occurences
  char current = '\0'; // The current character

  // For each character in the string
  for(current = *str; str != NULL; str++) {
    if(current == ch) {
      count++;
    }
  }

  return count;
}


//-----------------------------------------------------------------------------
// toHex
//-----------------------------------------------------------------------------
void castor::tape::utils::toHex(const uint64_t i, char *dst,
  size_t dstLen) throw(castor::exception::Exception) {

  // The largest 64-bit hexadecimal string "FFFFFFFFFFFFFFFF" would ocuppy 17
  // characters (17 characters = 16 x 'F' + 1 x '\0')
  const size_t minimumDstLen = 17;

  // If the destination character string cannot store the largest 64-bit
  // hexadecimal string
  if(dstLen < minimumDstLen) {
    castor::exception::Exception ex(EINVAL);

    ex.getMessage() << __FUNCTION__
      << ": Destination character array is too small"
         ": Minimum = " << minimumDstLen
      << ": Actual = " << dstLen;

    throw ex;
  }

  const char hexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'};
  char backwardsHexDigits[16];
  setBytes(backwardsHexDigits, '\0');
  uint64_t exponent = 0;
  uint64_t quotient = i;
  int nbDigits = 0;

  for(exponent=0; exponent<16; exponent++) {
    backwardsHexDigits[exponent] = hexDigits[quotient % 16];
    nbDigits++;

    quotient = quotient / 16;

    if(quotient== 0) {
      break;
    }
  }

  for(int d=0; d<nbDigits;d++) {
    dst[d] = backwardsHexDigits[nbDigits-1-d];
  }
  dst[nbDigits] = '\0';
}


//-----------------------------------------------------------------------------
// copyString
//-----------------------------------------------------------------------------
void castor::tape::utils::copyString(char *const dst,
  const char *src, const size_t n) throw(castor::exception::Exception) {

  const size_t srcLen = strlen(src);

  if(srcLen >= n) {
    castor::exception::Exception ex(EINVAL);

    ex.getMessage() << __FUNCTION__
      << ": Source string is longer than destination.  Source length: "
      << srcLen << " Max destination length: " << (n-1);

    throw ex;
  }

  strncpy(dst, src, n);
    *(dst+n-1) = '\0'; // Ensure destination is null terminated
}


//-----------------------------------------------------------------------------
// writeStrings
//-----------------------------------------------------------------------------
void castor::tape::utils::writeStrings(std::ostream &os,
  const char **strings, const int stringsLen, const char *const separator) {
  // For each string
  for(int i=0; i<stringsLen; i++) {
    // Add a separator if this is not the first string
    if(i > 0) {
      os << separator;
    }

    // Write the string to the output stream
    os << strings[i];
  }
}


//-----------------------------------------------------------------------------
// magicToString
//-----------------------------------------------------------------------------
const char *castor::tape::utils::magicToString(const uint32_t magic)
  throw() {
  switch(magic) {
  case RTCOPY_MAGIC_VERYOLD: return "RTCOPY_MAGIC_VERYOLD";
  case RTCOPY_MAGIC_SHIFT  : return "RTCOPY_MAGIC_SHIFT";
  case RTCOPY_MAGIC_OLD0   : return "RTCOPY_MAGIC_OLD0";
  case RTCOPY_MAGIC        : return "RTCOPY_MAGIC";
  case RFIO2TPREAD_MAGIC   : return "RFIO2TPREAD_MAGIC";
  default                  : return "UNKNOWN";
  }
}


//-----------------------------------------------------------------------------
// rtcopyReqTypeToString
//-----------------------------------------------------------------------------
const char *castor::tape::utils::rtcopyReqTypeToString(
  const uint32_t reqType) throw() {
  switch(reqType) {
  case GIVE_OUTP         : return "GIVE_OUTP";
  case RTCP_TAPE_REQ     : return "RTCP_TAPE_REQ";
  case RTCP_FILE_REQ     : return "RTCP_FILE_REQ";
  case RTCP_NOMORE_REQ   : return "RTCP_NOMORE_REQ";
  case RTCP_TAPEERR_REQ  : return "RTCP_TAPEERR_REQ";
  case RTCP_FILEERR_REQ  : return "RTCP_FILEERR_REQ";
  case RTCP_ENDOF_REQ    : return "RTCP_ENDOF_REQ";
  case RTCP_ABORT_REQ    : return "RTCP_ABORT_REQ";
  case RTCP_DUMP_REQ     : return "RTCP_DUMP_REQ";
  case RTCP_DUMPTAPE_REQ : return "RTCP_DUMPTAPE_REQ";
  case RTCP_KILLJID_REQ  : return "RTCP_KILLJID_REQ";
  case RTCP_RSLCT_REQ    : return "RTCP_RSLCT_REQ";
  case RTCP_PING_REQ     : return "RTCP_PING_REQ";
  case RTCP_HAS_MORE_WORK: return "RTCP_HAS_MORE_WORK";
  default                : return "UNKNOWN";
  }
}


//-----------------------------------------------------------------------------
// procStatusToString
//-----------------------------------------------------------------------------
const char *castor::tape::utils::procStatusToString(
  const uint32_t procStatus) throw() {
  switch(procStatus) {
  case RTCP_WAITING           : return "RTCP_WAITING";
  case RTCP_POSITIONED        : return "RTCP_POSITIONED";
  case RTCP_PARTIALLY_FINISHED: return "RTCP_PARTIALLY_FINISHED";
  case RTCP_FINISHED          : return "RTCP_FINISHED";
  case RTCP_EOV_HIT           : return "RTCP_EOV_HIT";
  case RTCP_UNREACHABLE       : return "RTCP_UNREACHABLE";
  case RTCP_REQUEST_MORE_WORK : return "RTCP_REQUEST_MORE_WORK";
  default                     : return "UNKNOWN";
  }
}


//-----------------------------------------------------------------------------
// volumeClientTypeToString
//-----------------------------------------------------------------------------
const char *castor::tape::utils::volumeClientTypeToString(
  const tapegateway::ClientType mode) throw() {

  switch(mode) {
  case tapegateway::TAPE_GATEWAY: return "TAPE_GATEWAY";
  case tapegateway::READ_TP     : return "READ_TP";
  case tapegateway::WRITE_TP    : return "WRITE_TP";
  case tapegateway::DUMP_TP     : return "DUMP_TP";
  default                       : return "UKNOWN";
  }
}


//-----------------------------------------------------------------------------
// volumeModeToString
//-----------------------------------------------------------------------------
const char *castor::tape::utils::volumeModeToString(
  const tapegateway::VolumeMode mode) throw() {

  switch(mode) {
  case tapegateway::READ : return "READ";
  case tapegateway::WRITE: return "WRITE";
  case tapegateway::DUMP : return "DUMP";
  default                : return "UKNOWN";
  }
}


//------------------------------------------------------------------------------
// isValidUInt
//------------------------------------------------------------------------------
bool castor::tape::utils::isValidUInt(const char *str)
  throw() {
  // An empty string is not a valid unsigned integer
  if(*str == '\0') {
    return false;
  }

  // For each character in the string
  for(;*str != '\0'; str++) {
    // If the current character is not a valid numerical digit
    if(*str < '0' || *str > '9') {
      return false;
    }
  }

  return true;
}


//------------------------------------------------------------------------------
// toUpper
//------------------------------------------------------------------------------
void castor::tape::utils::toUpper(char *str) {
  for(;*str != '\0'; str++) {
    *str = toupper(*str);
  }
}


//------------------------------------------------------------------------------
// toUpper
//------------------------------------------------------------------------------
void castor::tape::utils::toUpper(std::string &str) {
  for(std::string::iterator itor=str.begin(); itor!=str.end(); itor++) {
    *itor = toupper(*itor);
  }
}


//------------------------------------------------------------------------------
// drainFile
//------------------------------------------------------------------------------
ssize_t castor::tape::utils::drainFile(const int fd)
  throw(castor::exception::Exception) {

  char buf[1024];

  ssize_t rc    = 0;
  ssize_t total = 0;

  do {
    rc = read((int)fd, buf, sizeof(buf));

    if(rc == -1) {
      char codeStr[STRERRORBUFLEN];
      strerror_r(errno, codeStr, sizeof(codeStr));

      TAPE_THROW_EX(castor::exception::Internal,
        ": Failed to drain file"
        ": fd=" << fd <<
        ": Error=" << codeStr);
    } else {
      total += rc;
    }

  // while the end of file has not been reached
  } while(rc != 0);

  return total;
}


//------------------------------------------------------------------------------
// checkIdSyntax
//------------------------------------------------------------------------------
void castor::tape::utils::checkIdSyntax(const char *idString)
  throw(castor::exception::InvalidArgument) {

  const size_t len   = strlen(idString);
  char         c     = '\0';
  bool         valid = false;

  // For each character
  for(size_t i=0; i<len; i++) {
    c = idString[i];
    valid = (c >= '0' && c <='9') || (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') || c == '_';

    if(!valid) {
      castor::exception::InvalidArgument ex;
      ex.getMessage() << "Invalid character: " << c;
      throw ex;
    }
  }
}


//------------------------------------------------------------------------------
// checkVidSyntax
//------------------------------------------------------------------------------
void castor::tape::utils::checkVidSyntax(const char *vid)
  throw(castor::exception::InvalidArgument) {

  const size_t len = strlen(vid);

  if(len > CA_MAXVIDLEN) {
    castor::exception::InvalidArgument ex;

    ex.getMessage() << "VID is too long: Actual=" << len
      << " Expected maximum=" << CA_MAXVIDLEN;
    throw ex;
  }

  checkIdSyntax(vid);
}


//------------------------------------------------------------------------------
// checkDgnSyntax
//------------------------------------------------------------------------------
void castor::tape::utils::checkDgnSyntax(const char *dgn)
  throw(castor::exception::InvalidArgument) {

  const size_t len = strlen(dgn);

  if(len > CA_MAXDGNLEN) {
    castor::exception::InvalidArgument ex;

    ex.getMessage() << "DGN is too long: Actual=" << len
      << " Expected maximum=" << CA_MAXVIDLEN;
    throw ex;
  }

  checkIdSyntax(dgn);
}


//------------------------------------------------------------------------------
// objectTypeToString
//------------------------------------------------------------------------------
const char *castor::tape::utils::objectTypeToString(const unsigned int type) {
  if(type >= castor::ObjectsIdsNb) {
    return "UNKNOWN";
  }

  return castor::ObjectsIdStrings[type];
}


//------------------------------------------------------------------------------
// readFileIntoList
//------------------------------------------------------------------------------
void castor::tape::utils::readFileIntoList(const char *filename,
  std::list<std::string> &lines) throw(castor::exception::Exception) {

  std::ifstream file(filename);

  if(!file) {
    castor::exception::Exception ex(ECANCELED);

    ex.getMessage() << "Failed to open file: Filename=\"" << filename << "\"";

    throw ex;
  } 

  std::string line;

  while(!file.eof()) {
    std::getline(file, line, '\n');

    lines.push_back(line);

    line.clear();
  }
}


//------------------------------------------------------------------------------
// parseFileList
//------------------------------------------------------------------------------
void castor::tape::utils::parseFileList(const char *filename,
  std::list<std::string> &list) throw (castor::exception::Exception) {

  readFileIntoList(filename, list);

  std::list<std::string>::iterator itor=list.begin();

  while(itor!=list.end()) {
    std::string &line = *itor;

    // Left and right trim the line
    trimString(line);

    // Remove the line if it is an empty string or if it starts with the shell
    // comment character '#'
    if(line.empty() || (line.size() > 0 && line[0] == '#')) {
      itor = list.erase(itor);
    } else {
      itor++;
    }
  }
}


//------------------------------------------------------------------------------
// trimString
//------------------------------------------------------------------------------
void castor::tape::utils::trimString(std::string &str) throw() {
  const char *whitespace = " \t";

  std::string::size_type start = str.find_first_not_of(whitespace);
  std::string::size_type end   = str.find_last_not_of(whitespace);

  // If all the characters of the string are whitespace
  if(start == std::string::npos) {
    // The result is an empty string
    str = "";
  } else {
    // The result is what is in the middle
    str = str.substr(start, end - start + 1);
  }
}


//------------------------------------------------------------------------------
// writeBanner
//------------------------------------------------------------------------------
void castor::tape::utils::writeBanner(std::ostream &os,
  const char *const title) throw() {

  const size_t len = strlen(title);
  size_t       i   = 0;

  for(i=0; i<len+4; i++) {
    os << "=";
  }
  os << std::endl;

  os << "= ";
  os << title;
  os << " =" << std::endl;

  for(i=0; i<len+4; i++) {
    os << "=";
  }
  os << std::endl;
}

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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "castor/utils/utils.hpp"
#include "common/utils/strerror_r_wrapper.hpp"
#include "h/Castor_limits.h"

#include <algorithm>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/prctl.h>
#include <sys/socket.h>


//-----------------------------------------------------------------------------
// writeTime
//-----------------------------------------------------------------------------
void castor::utils::writeTime(std::ostream &os, const time_t time,
  const char* const format) {

  tm localTime;

  localtime_r(&time, &localTime);

  const std::time_put<char>& dateWriter =
    std::use_facet<std::time_put<char> >(os.getloc());
  const size_t n = strlen(format);

  if (dateWriter.put(os, os, ' ', &localTime, format, format + n).failed()){
    os << "UNKNOWN";
  }
}

//-----------------------------------------------------------------------------
// splitString
//-----------------------------------------------------------------------------
void castor::utils::splitString(const std::string &str,
  const char separator, std::vector<std::string> &result) throw() {

  if(str.empty()) {
    return;
  }

  std::string::size_type beginIndex = 0;
  std::string::size_type endIndex   = str.find(separator);

  while(endIndex != std::string::npos) {
    result.push_back(str.substr(beginIndex, endIndex - beginIndex));
    beginIndex = ++endIndex;
    endIndex = str.find(separator, endIndex);
  }

  // If no separator could not be found then simply append the whole input
  // string to the result
  if(endIndex == std::string::npos) {
    result.push_back(str.substr(beginIndex, str.length()));
  }
}

//-----------------------------------------------------------------------------
// trimString
//-----------------------------------------------------------------------------
std::string castor::utils::trimString(const std::string &s) throw() {
  const std::string& spaces="\t\n\v\f\r ";

  // Find first non white character
  size_t beginpos = s.find_first_not_of(spaces);
  std::string::const_iterator it1;
  if (std::string::npos != beginpos) {
    it1 = beginpos + s.begin();
  } else {
    it1 = s.begin();
  }

  // Find last non white chararacter
  std::string::const_iterator it2;
  size_t endpos = s.find_last_not_of(spaces);
  if (std::string::npos != endpos) {
    it2 = endpos + 1 + s.begin();
  } else {
    it2 = s.end();
  }

  return std::string(it1, it2);
}

//------------------------------------------------------------------------------
// singleSpaceString
//------------------------------------------------------------------------------
std::string castor::utils::singleSpaceString(const std::string &str) throw() {
  bool inWhitespace = false;
  bool strContainsNonWhiteSpace = false;

  // Output string stream used to construct the result
  std::ostringstream result;

  // For each character in the original string
  for(std::string::const_iterator itor = str.begin(); itor != str.end();
    itor++) {

    // If the character is a space or a tab
    if(*itor == ' ' || *itor == '\t') {

      // Remember we are in whitespace
      inWhitespace = true;

    // Else the character is not a space or a tab
    } else {

      // If we are leaving whitespace
      if(inWhitespace) {

        // Remember we have left whitespace
        inWhitespace = false;

        // Remember str contains non-whitespace
        strContainsNonWhiteSpace = true;

        // Insert a single space into the output string stream
        result << " ";
      }

      // Insert the character into the output string stream
      result << *itor;

    }
  }

  // If str is not emtpy and does not contain any non-whitespace characters
  // then nothing has been written to the result stream, therefore write a
  // single space
  if(!str.empty() && !strContainsNonWhiteSpace) {
    result << " ";
  }

  return result.str();
}

//------------------------------------------------------------------------------
// isValidUInt
//------------------------------------------------------------------------------
bool castor::utils::isValidUInt(const std::string &str)
  throw() {
  // An empty string is not a valid unsigned integer
  if(str.empty()) {
    return false;
  }

  // For each character in the string
  for(std::string::const_iterator itor = str.begin(); itor != str.end();
    itor++) {

    // If the current character is not a valid numerical digit
    if(*itor < '0' || *itor > '9') {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
// toUpper
//------------------------------------------------------------------------------
void castor::utils::toUpper(char *str) {
  for(;*str != '\0'; str++) {
    *str = toupper(*str);
  }
}

//------------------------------------------------------------------------------
// toUpper
//------------------------------------------------------------------------------
void castor::utils::toUpper(std::string &str) {
  for(std::string::iterator itor=str.begin(); itor!=str.end(); itor++) {
    *itor = toupper(*itor);
  }
}

//---------------------------------------------------------------------------
// getTimeOfDay
//---------------------------------------------------------------------------
void castor::utils::getTimeOfDay(struct timeval *const tv)  {
  if(0 != gettimeofday(tv, NULL)) {
    const int savedErrno = errno;
    cta::exception::Exception ex;
    ex.getMessage() << "Call to gettimeofday() failed: " <<
      errnoToString(savedErrno);
    throw ex;
  }
}

//---------------------------------------------------------------------------
// timevalGreaterThan
//---------------------------------------------------------------------------
bool castor::utils::timevalGreaterThan(const timeval &a, const timeval &b)
  throw() {
  if(a.tv_sec != b.tv_sec) {
    return a.tv_sec > b.tv_sec;
  } else {
    return a.tv_usec > b.tv_usec;
  }
}

//---------------------------------------------------------------------------
// timevalAbsDiff
//---------------------------------------------------------------------------
timeval castor::utils::timevalAbsDiff(const timeval &a, const timeval &b)
  throw() {
  timeval bigger  = {0, 0};
  timeval smaller = {0, 0};
  timeval result  = {0, 0};

  // If time-values a and b are equal
  if(a.tv_sec == b.tv_sec && a.tv_usec == b.tv_usec) {
    return result; // Result was initialised to {0, 0}
  }

  // The time-values are not equal, determine which is the bigger and which is
  // the smaller time-value
  if(timevalGreaterThan(a, b)) {
    bigger  = a;
    smaller = b;
  } else {
    bigger  = b;
    smaller = a;
  }

  // Subtract the smaller time-value from the bigger time-value carrying over
  // 1000000 micro-seconds from the seconds to the micro-seconds if necessay
  if(bigger.tv_usec >= smaller.tv_usec) {
    result.tv_usec = bigger.tv_usec - smaller.tv_usec;
    result.tv_sec  = bigger.tv_sec  - smaller.tv_sec;
  } else {
    result.tv_usec = bigger.tv_usec + 1000000 - smaller.tv_usec;
    result.tv_sec  = bigger.tv_sec - 1 - smaller.tv_sec;
  }

  return result;
}

//---------------------------------------------------------------------------
// timevalToDouble
//---------------------------------------------------------------------------
double castor::utils::timevalToDouble(const timeval &tv) throw() {
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

//-----------------------------------------------------------------------------
// copyString
//-----------------------------------------------------------------------------
void castor::utils::copyString(char *const dst, const size_t dstSize,
  const std::string &src) {

  if(dst == NULL) {
    cta::exception::Exception ex;

    ex.getMessage() << __FUNCTION__
      << ": Pointer to destination string is NULL";

    throw ex;
  }

  if(src.length() >= dstSize) {
    cta::exception::Exception ex;

    ex.getMessage() << __FUNCTION__
      << ": Source string is longer than destination.  Source length: "
      << src.length() << " Max destination length: " << (dstSize - 1);

    throw ex;
  }

  strncpy(dst, src.c_str(), dstSize);
  *(dst + dstSize -1) = '\0'; // Ensure destination string is null terminated
}

//------------------------------------------------------------------------------
// Tape DGNs and VIDs have the same rules about what characters they may
// contain.  This static and therefore hidden function implements this
// commonality.
//
// This function throws an InvalidArgument exception if the specified identifier
// string is syntactically incorrect.
//  
// The indentifier string is valid if each character is either a number (0-9),
// a letter (a-z, A-Z) or an underscore.
//    
// @param idTypeName The type name of the identifier, usually "DGN" or "VID".
// @param id The indentifier string to be checked.
// @param maxSize The maximum length the identifier string is permitted to have.
//------------------------------------------------------------------------------
static void checkDgnVidSyntax(const char *const idTypeName, const char *id,
  const size_t maxLen) {

  // Check the length of the identifier string
  const size_t len   = strlen(id);
  if(len > maxLen) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << idTypeName << " exceeds maximum length: actual=" << len
      << " max=" << maxLen;
    throw ex;
  }

  // Check each character of the identifier string
  char         c     = '\0';
  bool         valid = false;
  for(size_t i=0; i<len; i++) {
    c = id[i];
    valid = (c >= '0' && c <='9') || (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') || c == '_';

    if(!valid) {
      castor::exception::InvalidArgument ex;
      ex.getMessage() << idTypeName << " contains the invalid character '" << c
        << "'";
      throw ex;
    }
  }
}

//------------------------------------------------------------------------------
// checkDgnSyntax
//------------------------------------------------------------------------------
void castor::utils::checkDgnSyntax(const char *dgn) {
  checkDgnVidSyntax("DGN", dgn, CA_MAXDGNLEN);
}

//------------------------------------------------------------------------------
// checkVidSyntax
//------------------------------------------------------------------------------
void castor::utils::checkVidSyntax(const char *vid) {
  checkDgnVidSyntax("VID", vid, CA_MAXVIDLEN);
}

//------------------------------------------------------------------------------
// getDumpableProcessAttribute
//------------------------------------------------------------------------------
bool castor::utils::getDumpableProcessAttribute() {
  const int rc = prctl(PR_GET_DUMPABLE);
  switch(rc) {
  case -1:
    {
      const std::string errStr = errnoToString(errno);
      cta::exception::Exception ex;
      ex.getMessage() <<
        "Failed to get the dumpable attribute of the process: " << errStr;
      throw ex;
    }
  case 0: return false;
  case 1: return true;
  case 2: return true;
  default:
    {
      cta::exception::Exception ex;
      ex.getMessage() <<
        "Failed to get the dumpable attribute of the process"
        ": Unknown value returned by prctl(): rc=" << rc;
      throw ex;
    }
  }
}

//------------------------------------------------------------------------------
// setDumpableProcessAttribute
//------------------------------------------------------------------------------
 void castor::utils::setDumpableProcessAttribute(const bool dumpable) {
  const int rc = prctl(PR_SET_DUMPABLE, dumpable ? 1 : 0);
  switch(rc) {
  case -1:
    {
      const std::string errStr = errnoToString(errno);
      cta::exception::Exception ex;
      ex.getMessage() <<
        "Failed to set the dumpable attribute of the process: " << errStr;
      throw ex;
    }
  case 0: return;
  default:
    {
      cta::exception::Exception ex;
      ex.getMessage() <<
        "Failed to set the dumpable attribute of the process"
        ": Unknown value returned by prctl(): rc=" << rc;
      throw ex;
    }
  }
}

//------------------------------------------------------------------------------
// errnoToString
//------------------------------------------------------------------------------
std::string castor::utils::errnoToString(const int errnoValue) throw() {
  char buf[100];

  if(!strerror_r_wrapper(errnoValue, buf, sizeof(buf))) {
    return buf;
  } else {
    const int errnoSetByStrerror_r_wrapper = errno;
    std::ostringstream oss;

    switch(errnoSetByStrerror_r_wrapper) {
    case EINVAL:
      oss << "Failed to convert errnoValue to string: Invalid errnoValue"
        ": errnoValue=" << errnoValue;
      break;
    case ERANGE:
      oss << "Failed to convert errnoValue to string"
        ": Destination buffer for error string is too small"
        ": errnoValue=" << errnoValue;
      break;
    default:
      oss << "Failed to convert errnoValue to string"
        ": strerror_r_wrapper failed in an unknown way"
        ": errnoValue=" << errnoValue;
      break;
    }

    return oss.str();
  }
}

//------------------------------------------------------------------------------
// setProcessNameAndCmdLine
//------------------------------------------------------------------------------
void castor::utils::setProcessNameAndCmdLine(char *const argv0,
  const std::string &name) {
  try {
    setProcessName(name);
    setCmdLine(argv0, name);
  } catch(cta::exception::Exception &ne) {
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to set process name and command-line"
      ": " << ne.getMessage().str();
  }
}

//------------------------------------------------------------------------------
// setProcessName
//------------------------------------------------------------------------------
void castor::utils::setProcessName(const std::string &name) {
  char buf[16];
  strncpy(buf, name.c_str(), sizeof(buf));
  buf[sizeof(buf)-1] = '\0';

  if(prctl(PR_SET_NAME, buf)) {
    const std::string errMsg = errnoToString(errno);
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to set process name: " << errMsg;
    throw ex;
  }
}

//------------------------------------------------------------------------------
// setCmdLine
//------------------------------------------------------------------------------
void castor::utils::setCmdLine(char *const argv0, const std::string &cmdLine)
  throw() {
  const size_t argv0Len = strlen(argv0);
  strncpy(argv0, cmdLine.c_str(), argv0Len);
  argv0[argv0Len] = '\0';
}

//------------------------------------------------------------------------------
// getHostname
//------------------------------------------------------------------------------
std::string castor::utils::getHostName() {
  char buf[256];
  if(gethostname(buf, sizeof(buf))) {
    const std::string errnoStr = errnoToString(errno);
    cta::exception::Exception ex;
    ex.getMessage() << "Call to gethostname() failed: " << errnoStr;
    throw ex;
  }

  buf[sizeof(buf) - 1] = '\0';
  return buf;
}

//-----------------------------------------------------------------------------
// hexDump
//-----------------------------------------------------------------------------
std::string castor::utils::hexDump(const void * mem, unsigned int n ){
  std::ostringstream out;
  const unsigned char * p = reinterpret_cast< const unsigned char *>( mem );
  for ( unsigned int i = 0; i < n; i++ ) {
     if (0 != i) {
       out << " ";
     }
     out << std::uppercase << std::hex << std::setw(2) <<
       std::setfill( out.widen('0') ) << int(p[i]);

  }
  return out.str();
}

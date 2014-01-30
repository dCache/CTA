/******************************************************************************
 *                      castor/utils/utils.cpp
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#include "castor/utils/utils.hpp"

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
    os << "UKNOWN";
  }
}

//-----------------------------------------------------------------------------
// boolToString
//-----------------------------------------------------------------------------
const char *castor::utils::boolToString(const bool value) {
  return value ? "TRUE" : "FALSE";
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

//------------------------------------------------------------------------------
// isValidUInt
//------------------------------------------------------------------------------
bool castor::utils::isValidUInt(const char *str)
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

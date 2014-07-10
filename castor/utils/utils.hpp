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
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include "castor/exception/Exception.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/InvalidConfigEntry.hpp"
#include "castor/exception/InvalidConfiguration.hpp"
#include "castor/io/ServerSocket.hpp"

#include <ostream>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <time.h>
#include <vector>
#include <cxxabi.h>

namespace castor {
namespace utils  {

/**
 * Writes the specified time to the specified stream using the specified
 * format.
 *
 * @param os     The stream to be written to.
 * @param time   The time as the number of seconds since the Epoch
 *               (00:00:00 UTC, January 1, 1970).
 * @param format The time format specified using the 
 * recognized formatting characters of 'std::strftime'.
 */
void writeTime(std::ostream &os, const time_t time, const char* const format);

/**
 * Splits the specified string into a vector of strings using the specified
 * separator.
 *
 * Please note that the string to be split is NOT modified.
 *
 * @param str The string to be split.
 * @param separator The separator to be used to split the specified string.
 * @param result The vector when the result of spliting the string will be
 * stored.
 */
void splitString(const std::string &str, const char separator,
  std::vector<std::string> &result) throw();

/**
 * Returns the result of trimming both left and right white-space from the
 * specified string.
 *
 * @param s The string to be trimmed.
 * @return The result of trimming the string.
 */
std::string trimString(const std::string &s) throw();

template <class T> std::string toString(const T& t){
  std::ostringstream o;
  o<<t;
  return o.str();
}

/**
 * Checks if the specified string is a valid unsigned integer.
 *
 * @param str The string to be checked.
 * @returns true if the string is a valid unsigned integer, else false.
 */
bool isValidUInt(const char *str) throw();

/**
 * Converts the specified string to uppercase.
 */
void toUpper(char *str);

/**
 * Converts the specified string to uppercase.
 */
void toUpper(std::string &str);

/**
 * Simple C++ wrapper around the C function named gettimeofday.  The wrapper
 * simply converts the return of -1 and the setting of errno to an exception.
 *
 * @param tv See the manual page for gettimeofday.
 */
void getTimeOfDay(struct timeval *const tv);

/**
 * Compares two time-values, a and b, and returns true if time value a is
 * greater than time-value b, else returns false.
 *
 * @param a Time-value a.
 * @param b Time-value b.
 * @return True if time value a is greater than time value b, else false.
 */
bool timevalGreaterThan(const timeval &a, const timeval &b) throw();

/**
 * Calculates the absolute difference of the two specified time-values.
 *
 * @param a Time-value a.
 * @param b Time-value b.
 * @return The difference between time-values a and b.
 */
timeval timevalAbsDiff(const timeval &a, const timeval &b) throw();

/**
 * Returns the double version of the specified time-value.
 *
 * @param tv The time-value from which the double is to be calculated.
 * @param    The double version of the specified time-value.
 */
double timevalToDouble(const timeval &tv) throw();

/**
 * Safely copies source string into destination string.  The destination
 * will always be null terminated if this function is successful.
 *
 * @param dst     Destination string.
 * @param dstSize The size of the destination string including the terminating
 *                null character.
 * @param src     Source string.
 * destination.
 */
void copyString(char *const dst, const size_t dstSize, const std::string &src);

/**
 * Safely copies source string into destination string.  The destination
 * will always be null terminated if this function is successful.
 *
 * @param dst Destination string.
 * @param src Source string.
 */
template<size_t dstSize> void copyString(char (&dst)[dstSize],
  const std::string &src) {
  copyString(dst, dstSize, src);
}

/**
 * Sets all the bytes of the specified object to the value of c.
 *
 * @param object The object whose bytes are to be set.
 * @param c The value to set each byte of object.
 */
template<typename T> void setBytes(T &object, const int c) throw() {
  memset(&object, c, sizeof(object));
}

/**
 * Throws an InvalidArgument exception if the specified DGN is syntactically
 * invalid.
 *
 * @param dgn The DGN to be checked.
 */
void checkDgnSyntax(const char *dgn);

/**
 * Throws an InvalidArgument exception if the specified VID is syntactically
 * invalid.
 *
 * @param vid The VID to be checked.
 */
void checkVidSyntax(const char *vid);

/**
 * Returns true if the attributes of the current process indicate that it will
 * produce a core dump if it receives a signal whose behaviour is to produce a
 * core dump.
 *
 * This method is implemented using prctl().
 *
 * @return true if the current program is dumpable.
 */
bool getDumpableProcessAttribute();

/**
 * Sets the attributes of the current process to indicate hat it will produce a
 * core dump if it receives a signal whose behaviour is to produce a core dump.
 *
 * @param dumpable true if the current program should be dumpable.
 */
void setDumpableProcessAttribute(const bool dumpable);

  
template <class T>std::string demangledNameOf(const T&t){
  std::string responseType = typeid(t).name();
  int status = -1;
  char * demangled = abi::__cxa_demangle(responseType.c_str(), NULL, NULL, &status);
  if (!status) {
    responseType = demangled; 
  }
  free(demangled);
  
  return responseType;
}
} // namespace utils
} // namespace castor


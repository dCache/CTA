/*
 * The CERN Tape Archive(CTA) project
 * Copyright(C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

extern "C" {
#include "acssys.h"
#include "acsapi.h"
}

#include <string>

namespace cta {
namespace mediachanger {
namespace acs {

/**
 * Class containing the code common to the parsed command-line of the ACS
 * command-line tools provided by CASTOR.
 */
class AcsCmdLine {
public:

  /**
   * Parses the specified string and returns the corresponding drive ID object.
   *
   * This method throws a cta::exception::InvalidArgument if the syntax of
   * the string is invalid.
   *
   * @param str The string to be parsed.
   * @return The drive ID object.
   */
  static DRIVEID str2DriveId(const std::string &str);

protected:

  /**
   * Parses the specified query interval.
   *
   * @return The parse query interval.
   */
  int parseQueryInterval(const std::string &s);

  /**
   * Parses the specified timeout.
   *
   * @return The parse query interval.
   */
  int parseTimeout(const std::string &s);

  /**
   * Handles the specified parameter that is missing a parameter.
   *
   * @param opt The option.
   */
  void handleMissingParameter(const int opt);

  /**
   * Handles the specified unknown option.
   *
   * @param opt The option.
   */
  void handleUnknownOption(const int opt);

private:

  /**
   * Returns true if the specified string only contains numerals else false.
   *
   * @return True if the specified string only contains numerals else false.
   */
  static bool onlyContainsNumerals(const std::string &str);

}; // class AcsCmdLine

} // namespace acs
} // namespace mediachanger
} // namespace cta

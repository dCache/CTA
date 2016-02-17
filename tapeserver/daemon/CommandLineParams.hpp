/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
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

#include <string>

namespace cta { namespace daemon {
/// A class parsing the command line and turning it into a struct.
struct CommandLineParams{
  /**
   * Translates the command line parameters into a struct
   * @param argc
   * @param argv
   */
  CommandLineParams(int argc, char **argv);
  bool foreground;                  ///< Prevents daemonisation
  bool logToStdout;                 ///< Log to stdout instead of syslog. Foreground is required.
  std::string configFileLocation;   ///< Location of the configuration file. Defaults to /etc/cta/cta.conf
  bool helpRequested;               ///< Help requested: will print out help and exit.
};
}}
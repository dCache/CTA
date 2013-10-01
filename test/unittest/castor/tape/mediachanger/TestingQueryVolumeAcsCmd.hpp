/******************************************************************************
 *    test/unittest/castor/tape/mediachanger/TestingQueryVolumeAcsCmd.hpp
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

#ifndef TEST_UNITTEST_CASTOR_TAPE_MEDIACHANGER_TESTINGQUERYVOLUMEACSCMD_HPP
#define TEST_UNITTEST_CASTOR_TAPE_MEDIACHANGER_TESTINGQUERYVOLUMEACSCMD_HPP 1

#include "castor/tape/mediachanger/QueryVolumeAcsCmd.hpp"

namespace castor {
namespace tape {
namespace mediachanger {

class TestingQueryVolumeAcsCmd: public QueryVolumeAcsCmd {
public:
  /**
   * Constructor.
   *
   * @param inStream Standard input stream.
   * @param outStream Standard output stream.
   * @param errStream Standard error stream.
   * @param acs Wrapper around the ACSLS C-API.
   */
  TestingQueryVolumeAcsCmd(std::istream &inStream, std::ostream &outStream,
    std::ostream &errStream, Acs &acs) throw():
    QueryVolumeAcsCmd(inStream, outStream, errStream, acs) {
  }

  using QueryVolumeAcsCmd::parseCmdLine;

}; // class TestingQueryVolumeAcsCmd

} // namespace mediachanger
} // namespace tape
} // namespace castor

#endif // TEST_UNITTEST_CASTOR_TAPE_MEDIACHANGER_TESTINGQUERYVOLUMEACSCMD_HPP

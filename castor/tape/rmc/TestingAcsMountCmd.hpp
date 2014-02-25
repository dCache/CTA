/******************************************************************************
 *    castor/tape/rmc/TestingAcsMountCmd.hpp
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

#ifndef CASTOR_TAPE_RMC_TESTINGACSMOUNTCMD_HPP
#define CASTOR_TAPE_RMC_TESTINGACSMOUNTCMD_HPP 1

#include "castor/tape/rmc/AcsMountCmd.hpp"

namespace castor {
namespace tape {
namespace rmc {

class TestingAcsMountCmd: public AcsMountCmd {
public:
  /**
   * Constructor.
   *
   * @param inStream Standard input stream.
   * @param outStream Standard output stream.
   * @param errStream Standard error stream.
   * @param acs Wrapper around the ACSLS C-API.
   */
  TestingAcsMountCmd(std::istream &inStream, std::ostream &outStream,
    std::ostream &errStream, Acs &acs) throw():
    AcsMountCmd(inStream, outStream, errStream, acs) {
  }

  using AcsMountCmd::parseCmdLine;

}; // class TestingAcsMountCmd

} // namespace rmc
} // namespace tape
} // namespace castor

#endif // CASTOR_TAPE_RMC_TESTINGACSMOUNTCMD_HPP

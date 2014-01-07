/******************************************************************************
 *    test/unittest/castor/tape/rmc/TestingRmcDaemon.hpp
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

#ifndef TEST_UNITTEST_CASTOR_TAPE_RMC_TESTINGACSMOUNTCMD_HPP
#define TEST_UNITTEST_CASTOR_TAPE_RMC_TESTINGACSMOUNTCMD_HPP 1

#include "castor/tape/rmc/RmcDaemon.hpp"

namespace castor {
namespace tape {
namespace rmc {

class TestingRmcDaemon: public RmcDaemon {
public:

  TestingRmcDaemon(std::ostream &stdOut, std::ostream &stdIn,
    log::Logger &logger) throw(castor::exception::Exception):
    RmcDaemon(stdOut, stdIn, logger) {
  }

  using RmcDaemon::argvToString;
  using RmcDaemon::m_cmdLine;
  using RmcDaemon::parseCommandLine;


}; // class TestingAcsCmd

} // namespace rmc
} // namespace tape
} // namespace castor

#endif // TEST_UNITTEST_CASTOR_TAPE_RMC_TESTINGACSMOUNTCMD_HPP

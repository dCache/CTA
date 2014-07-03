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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "castor/legacymsg/CupvProxyDummy.hpp"
#include "castor/log/DummyLogger.hpp"
#include "castor/tape/reactor/DummyPollReactor.hpp"
#include "castor/tape/rmc/RmcDaemon.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <sstream>

namespace unitTest {

class castor_tape_rmc_RmcDaemonTest : public ::testing::Test {
protected:
  void *const m_zmqContext;

  castor_tape_rmc_RmcDaemonTest(): m_zmqContext(zmq_init(1)) {
  }

  ~castor_tape_rmc_RmcDaemonTest() {
    if(NULL != m_zmqContext) {
      zmq_term(m_zmqContext);
    }
  }

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
}; // class castor_tape_rmc_RmcDaemonTest

TEST_F(castor_tape_rmc_RmcDaemonTest, zmqContext) {
  ASSERT_NE((void *const)0, m_zmqContext);
}

TEST_F(castor_tape_rmc_RmcDaemonTest, constructor) {
  using namespace castor::tape::rmc;

  std::ostringstream stdOut;
  std::ostringstream stdErr;
  castor::log::DummyLogger logger("unittest");
  castor::tape::reactor::ZMQReactor reactor(logger);
  const bool isGrantedReturnValue = true;
  castor::legacymsg::CupvProxyDummy cupv(isGrantedReturnValue);
  
  std::auto_ptr<RmcDaemon> daemon;
  daemon.reset(new RmcDaemon(stdOut, stdErr, logger, reactor, cupv));
}

} // namespace unitTests

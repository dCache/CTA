/******************************************************************************
 *         castor/tape/tapeserver/daemon/TapeDaemonTest.cpp
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

#include "castor/legacymsg/RmcProxyDummyFactory.hpp"
#include "castor/legacymsg/VdqmProxyDummy.hpp"
#include "castor/legacymsg/VmgrProxyDummy.hpp"
#include "castor/log/DummyLogger.hpp"
#include "castor/messages/TapeserverProxyDummyFactory.hpp"
#include "castor/tape/reactor/DummyPollReactor.hpp"
#include "castor/tape/tapeserver/daemon/CapabilityUtilsDummy.hpp"
#include "castor/tape/tapeserver/daemon/TapeDaemon.hpp"
#include "castor/tape/utils/utils.hpp"
#include "castor/utils/utils.hpp"
#include "castor/legacymsg/NsProxyDummyFactory.hpp"

#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>

namespace unitTests {

class castor_tape_tapeserver_daemon_TapeDaemonTest : public ::testing::Test {
protected:

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(castor_tape_tapeserver_daemon_TapeDaemonTest, constructor) {
  using namespace castor::tape::tapeserver::daemon;

  int argc = 1;
  char argv0[1024];
  memset(argv0, '\0', sizeof(argv0));
  char *argv[2] = {argv0, NULL};
  std::ostringstream stdOut;
  std::ostringstream stdErr;
  const std::string programName = "unittests";
  castor::log::DummyLogger log(programName);
  castor::legacymsg::RtcpJobRqstMsgBody job;
  job.volReqId = 1111;
  job.clientPort = 2222;
  job.clientEuid = 3333;
  job.clientEgid = 4444;
  castor::utils::copyString(job.clientHost, "CLIENT_HOST");
  castor::utils::copyString(job.dgn, "DGN");
  castor::utils::copyString(job.driveUnit, "UNIT");
  castor::utils::copyString(job.clientUserName, "USER");
  castor::tape::utils::DriveConfigMap driveConfigs;
  castor::legacymsg::VdqmProxyDummy vdqm;
  castor::legacymsg::VmgrProxyDummy vmgr;
  castor::legacymsg::RmcProxyDummyFactory rmcFactory;
  castor::messages::TapeserverProxyDummyFactory tpsFactory;
  castor::legacymsg::NsProxyDummyFactory nsFactory;
  zmq::context_t ctx;
  castor::tape::reactor::ZMQReactor reactor(log,ctx);
  CapabilityUtilsDummy capUtils;
  std::auto_ptr<TapeDaemon> daemon;
  ASSERT_NO_THROW(daemon.reset(new TapeDaemon(argc, argv, stdOut, stdErr, log,
    driveConfigs, vdqm, vmgr, rmcFactory, tpsFactory, nsFactory,
    reactor, capUtils)));
}

} // namespace unitTests

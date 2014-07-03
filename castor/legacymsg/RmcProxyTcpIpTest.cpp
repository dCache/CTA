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

#include "castor/log/DummyLogger.hpp"
#include "castor/legacymsg/RmcProxyTcpIp.hpp"

#include <gtest/gtest.h>

namespace unitTests {

class castor_tape_tapeserver_daemon_RmcProxyTcpIpTest : public ::testing::Test {
protected:

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(castor_tape_tapeserver_daemon_RmcProxyTcpIpTest, getLibrarySlotType) {
  using namespace castor::legacymsg;

  const std::string programName = "unittests";
  castor::log::DummyLogger log(programName);
  const int netTimeout = 1; // Timeout in seconds
  
  RmcProxyTcpIp rmc(log, netTimeout);

  ASSERT_EQ(RmcProxyTcpIp::RMC_LIBRARY_SLOT_TYPE_ACS, rmc.getLibrarySlotType("acs@rmc_host,1,2,3,4"));
  ASSERT_EQ(RmcProxyTcpIp::RMC_LIBRARY_SLOT_TYPE_MANUAL, rmc.getLibrarySlotType("manual"));
  ASSERT_EQ(RmcProxyTcpIp::RMC_LIBRARY_SLOT_TYPE_SCSI, rmc.getLibrarySlotType("smc@rmc_host,1"));
  ASSERT_EQ(RmcProxyTcpIp::RMC_LIBRARY_SLOT_TYPE_UNKNOWN, rmc.getLibrarySlotType("nonsense"));
}

} // namespace unitTests

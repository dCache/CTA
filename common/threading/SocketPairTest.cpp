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

#include <gtest/gtest.h>
#include <algorithm>

#include "common/threading/SocketPair.hpp"
#include "common/exception/Errnum.hpp"

namespace unitTests {

TEST(cta_threading_SocketPair, BasicTest) {
  using cta::server::SocketPair;
  cta::server::SocketPair sp0, sp1;
  SocketPair::pollMap pollList;
  pollList["0"] = &sp0;
  pollList["1"] = &sp1;
  sp0.send("C2P0", SocketPair::Side::parent);
  sp0.send("P2C0", SocketPair::Side::child);
  // We should have something to read
  SocketPair::poll(pollList, 0, SocketPair::Side::parent);
  ASSERT_TRUE(sp0.pollFlag());
  ASSERT_FALSE(sp1.pollFlag());
  ASSERT_EQ("P2C0", sp0.receive(SocketPair::Side::parent));
  // Nothing to read (= timeout)
  ASSERT_THROW(SocketPair::poll(pollList, 0, SocketPair::Side::parent), cta::server::SocketPair::Timeout);
  // We should have something to read from child.
  SocketPair::poll(pollList, 0, SocketPair::Side::child);
  ASSERT_TRUE(sp0.pollFlag());
  ASSERT_FALSE(sp1.pollFlag());
  ASSERT_EQ("C2P0", sp0.receive(SocketPair::Side::child));
  ASSERT_THROW(sp0.receive(SocketPair::Side::child), SocketPair::NothingToReceive);
}

TEST(cta_threading_SocketPair, Multimessages) {
  using cta::server::SocketPair;
  cta::server::SocketPair sp;
  SocketPair::pollMap pollList;
  pollList["0"] = &sp;
  sp.send("C2P0", SocketPair::Side::parent);
  sp.send("C2P1", SocketPair::Side::parent);
  sp.send("C2P2", SocketPair::Side::parent);
  // We should have something to read
  SocketPair::poll(pollList, 0, SocketPair::Side::child);
  ASSERT_TRUE(sp.pollFlag());
  // Read 2 messages
  ASSERT_EQ("C2P0", sp.receive(SocketPair::Side::child));
  ASSERT_EQ("C2P1", sp.receive(SocketPair::Side::child));
  // We should still something to read
  SocketPair::poll(pollList, 0, SocketPair::Side::child);
  ASSERT_TRUE(sp.pollFlag());
  // Read 2 messages (2nd should fail)
  ASSERT_EQ("C2P2", sp.receive(SocketPair::Side::child));
  ASSERT_THROW(sp.receive(SocketPair::Side::child), SocketPair::NothingToReceive);
  // Nothing to read (= timeout)
  ASSERT_THROW(SocketPair::poll(pollList, 0, SocketPair::Side::child), cta::server::SocketPair::Timeout);
}

TEST(cta_threading_SocketPair, MaxLength) {
  // We should be able to read up to 2048 bytes (this is an internal limit that
  // could be raised)
  // Limit to send is higher
  // 1) prepare messages.
  std::string smallMessage = "Hello!";
  std::string maxMessage;
  int i = 0;
  maxMessage.resize(2048, '.');
  std::for_each(maxMessage.begin(), maxMessage.end(), [&](char &c){ c='A' + (i++ % 26);});
  std::string oversizeMessage;
  oversizeMessage.resize(2049, '.');
  // 2) send/receive them
  using cta::server::SocketPair;
  cta::server::SocketPair sp;
  sp.send(smallMessage, SocketPair::Side::parent);
  sp.send(maxMessage, SocketPair::Side::parent);
  sp.send(oversizeMessage, SocketPair::Side::parent);
  sp.send(smallMessage, SocketPair::Side::parent);
  ASSERT_EQ(smallMessage, sp.receive(SocketPair::Side::child));
  ASSERT_EQ(maxMessage, sp.receive(SocketPair::Side::child));
  ASSERT_THROW(sp.receive(SocketPair::Side::child), SocketPair::Overflow);
  ASSERT_EQ(smallMessage, sp.receive(SocketPair::Side::child));
}

}

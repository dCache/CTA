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

#include "common/exception/Exception.hpp"
#include "rdbms/ConnPool.hpp"

#include <iostream>
#include <memory>

namespace cta {
namespace rdbms {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
ConnPool::ConnPool(ConnFactory &connFactory, const uint64_t maxNbConns):
  m_connFactory(connFactory),
  m_maxNbConns(maxNbConns),
  m_nbConnsOnLoan(0) {
  try {
    createConns(m_maxNbConns);
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// createConns
//------------------------------------------------------------------------------
void ConnPool::createConns(const uint64_t nbConns) {
  try {
    for(uint64_t i = 0; i < nbConns; i++) {
      m_conns.push_back(m_connFactory.create());
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// getConn
//------------------------------------------------------------------------------
PooledConn ConnPool::getConn() {
  std::unique_lock<std::mutex> lock(m_connsMutex);
  while(m_conns.size() == 0) {
    m_connsCv.wait(lock);
  }

  std::unique_ptr<Conn> conn = std::move(m_conns.front());
  m_conns.pop_front();
  m_nbConnsOnLoan++;
  if(conn->isOpen()) {
    return PooledConn(std::move(conn), this);
  } else {
    return PooledConn(m_connFactory.create(), this);
  }
}

//------------------------------------------------------------------------------
// returnConn
//------------------------------------------------------------------------------
void ConnPool::returnConn(Conn *const conn) {
  try {
    // If the connection is open
    if(conn->isOpen()) {

      // Commit the connection and put it back in the pool
      conn->commit();
      std::unique_lock<std::mutex> lock(m_connsMutex);
      if(0 == m_nbConnsOnLoan) {
        throw exception::Exception("Would have reached -1 connections on loan");
      }
      m_nbConnsOnLoan--;
      m_conns.emplace_back(conn);
      m_connsCv.notify_one();

    // Else the connection is closed
    } else {

      // Close all connections in the pool and put the returning connection back
      // in the pool.  A closed connection within the pool is reopened when it is
      // pulled from the pool.
      std::unique_lock<std::mutex> lock(m_connsMutex);
      for(auto &connInPool : m_conns) {
        connInPool->close();
      }
      if(0 == m_nbConnsOnLoan) {
        throw exception::Exception("Would have reached -1 connections on loan");
      }
      m_nbConnsOnLoan--;
      m_conns.emplace_back(conn);
      m_connsCv.notify_one();
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

} // namespace rdbms
} // namespace cta

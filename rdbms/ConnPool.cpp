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
#include "common/make_unique.hpp"
#include "common/threading/MutexLocker.hpp"
#include "rdbms/ConnPool.hpp"
#include "rdbms/wrapper/ConnFactoryFactory.hpp"

#include <memory>

namespace cta {
namespace rdbms {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
ConnPool::ConnPool(const Login &login, const uint64_t maxNbConns):
  m_connFactory(wrapper::ConnFactoryFactory::create(login)),
  m_maxNbConns(maxNbConns),
  m_nbConnsOnLoan(0){
}

//------------------------------------------------------------------------------
// createConns
//------------------------------------------------------------------------------
void ConnPool::createConns(const uint64_t nbConns) {
  try {
    for(uint64_t i = 0; i < nbConns; i++) {
      auto connAndStmts = cta::make_unique<ConnAndStmts>();
      connAndStmts->conn = m_connFactory->create();
      connAndStmts->stmtPool = cta::make_unique<StmtPool>();
      m_connsAndStmts.push_back(std::move(connAndStmts));
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

//------------------------------------------------------------------------------
// getConn
//------------------------------------------------------------------------------
Conn ConnPool::getConn() {
  threading::MutexLocker locker(m_connsAndStmtsMutex);

  while(m_connsAndStmts.size() == 0 && m_nbConnsOnLoan == m_maxNbConns) {
    m_connsAndStmtsCv.wait(locker);
  }

  if(m_connsAndStmts.size() == 0) {
    auto connAndStmts = cta::make_unique<ConnAndStmts>();
    connAndStmts->conn = m_connFactory->create();
    connAndStmts->stmtPool = cta::make_unique<StmtPool>();
    m_connsAndStmts.push_back(std::move(connAndStmts));
  }

  std::unique_ptr<ConnAndStmts> connAndStmts = std::move(m_connsAndStmts.front());
  m_connsAndStmts.pop_front();
  m_nbConnsOnLoan++;
  if(connAndStmts->conn->isOpen()) {
    return Conn(std::move(connAndStmts), this);
  } else {
    auto newConnAndStmts = cta::make_unique<ConnAndStmts>();
    newConnAndStmts->conn = m_connFactory->create();
    newConnAndStmts->stmtPool = cta::make_unique<StmtPool>();
    return Conn(std::move(newConnAndStmts), this);
  }
}

//------------------------------------------------------------------------------
// returnConn
//------------------------------------------------------------------------------
void ConnPool::returnConn(std::unique_ptr<ConnAndStmts> connAndStmts) {
  try {
    // If the connection is open
    if(connAndStmts->conn->isOpen()) {

      // Try to commit the connection and put it back in the pool
      try {
        connAndStmts->conn->commit();
      } catch(...) {
        // If the commit failed then destroy any prepare statements and then
        // close the connection
        try {
          connAndStmts->stmtPool->clear();
          connAndStmts->conn->close();
        } catch(...) {
          // Ignore any exceptions
        }

        // A closed connection is rare and usually means the underlying TCP/IP
        // connection, if there is one, has been lost.  Delete all the connections
        // currently in the pool because their underlying TCP/IP connections may
        // also have been lost.
        threading::MutexLocker locker(m_connsAndStmtsMutex);
        while(!m_connsAndStmts.empty()) {
          m_connsAndStmts.pop_front();
        }
        if(0 == m_nbConnsOnLoan) {
          throw exception::Exception("Would have reached -1 connections on loan");
        }
        m_nbConnsOnLoan--;
        m_connsAndStmtsCv.signal();
        return;
      }

      // Sets the autocommit mode of the connection to AUTOCOMMIT_ON because
      // this is the default value of a newly created connection
      connAndStmts->conn->setAutocommitMode(AutocommitMode::AUTOCOMMIT_ON);

      threading::MutexLocker locker(m_connsAndStmtsMutex);
      if(0 == m_nbConnsOnLoan) {
        throw exception::Exception("Would have reached -1 connections on loan");
      }
      m_nbConnsOnLoan--;
      m_connsAndStmts.push_back(std::move(connAndStmts));
      m_connsAndStmtsCv.signal();

    // Else the connection is closed
    } else {

      // A closed connection is rare and usually means the underlying TCP/IP
      // connection, if there is one, has been lost.  Delete all the connections
      // currently in the pool because their underlying TCP/IP connections may
      // also have been lost.
      threading::MutexLocker locker(m_connsAndStmtsMutex);
      while(!m_connsAndStmts.empty()) {
        m_connsAndStmts.pop_front();
      }
      if(0 == m_nbConnsOnLoan) {
        throw exception::Exception("Would have reached -1 connections on loan");
      }
      m_nbConnsOnLoan--;
      m_connsAndStmtsCv.signal();
    }
  } catch(exception::Exception &ex) {
    throw exception::Exception(std::string(__FUNCTION__) + " failed: " + ex.getMessage().str());
  }
}

} // namespace rdbms
} // namespace cta

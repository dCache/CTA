/******************************************************************************
 *                      BaseThreadPool.cpp
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2004  CERN
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
 * @(#)$RCSfile: BaseThreadPool.cpp,v $ $Revision: 1.3 $ $Release$ $Date: 2005/12/06 18:13:38 $ $Author: itglp $
 *
 *
 *
 * @author Giuseppe Lo Presti
 *****************************************************************************/

// Include Files
#include <signal.h>
#include "castor/server/BaseThreadPool.hpp"
#include "castor/MsgSvc.hpp"
#include "castor/Services.hpp"
#include "castor/exception/Internal.hpp"
#include "Cinit.h"
#include "Cuuid.h"
#include "Cpool_api.h"
#include "castor/logstream.h"
#include <sstream>
#include <iomanip>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::server::BaseThreadPool::BaseThreadPool(const std::string poolName,
                                               castor::server::IThread* thread) throw() :
  BaseObject(), m_foreground(false), m_threadPoolId(-1),
  m_nbThreads(castor::server::DEFAULT_THREAD_NUMBER),
  m_poolName(poolName), m_thread(thread) {}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::server::BaseThreadPool::~BaseThreadPool() throw()
{
  if(m_thread != 0) {
    m_thread->stop();
    delete m_thread;
  }
}

//------------------------------------------------------------------------------
// init
//------------------------------------------------------------------------------
void castor::server::BaseThreadPool::init() throw (castor::exception::Exception)
{
  // create threads if in multithreaded mode
  int actualNbThreads;
  m_threadPoolId = Cpool_create(m_nbThreads, &actualNbThreads);
  if (m_threadPoolId < 0) {
    castor::exception::Internal ex;
    ex.getMessage() << "Thread pool '" << m_poolName << "' creation error: "
           << m_threadPoolId << std::endl;
    clog() << ALERT << ex.getMessage().str();
    throw ex;
  }
  else {
    clog() << DEBUG << "Thread pool created: "
           << m_threadPoolId << ", "
           << actualNbThreads << std::endl;
    m_nbThreads = actualNbThreads;
  }
}


//------------------------------------------------------------------------------
// run
//------------------------------------------------------------------------------
void castor::server::BaseThreadPool::run()
{
  threadAssign(0);   // starts thread->run() in a forked thread
}

//------------------------------------------------------------------------------
// threadAssign
//------------------------------------------------------------------------------
int castor::server::BaseThreadPool::threadAssign(void *param)
{
  // Initializing the arguments to pass to the static request processor
  struct threadArgs *args = new threadArgs();
  args->handler = this;
  args->param = param;

  if (m_nbThreads > 0) {   // always true
  // for debugging purposes it could be useful to run the user thread code in the same thread. 
    int assign_rc = Cpool_assign(m_threadPoolId,
                                 &castor::server::_thread_run,
                                 args,
                                 -1);
    if (assign_rc < 0) {
      clog() << "Error while forking thread in pool " << m_poolName << std::endl;
      return -1;
    }
  } else {
    castor::server::_thread_run(args);
  }
  return 0;
}


//------------------------------------------------------------------------------
// setForeground
//------------------------------------------------------------------------------
void castor::server::BaseThreadPool::setForeground(bool value)
{
  m_foreground = value;
}

//------------------------------------------------------------------------------
// setNbThreads
//------------------------------------------------------------------------------
void castor::server::BaseThreadPool::setNbThreads(int value)
{
  m_nbThreads = value;
}


//------------------------------------------------------------------------------
// _thread_run
//------------------------------------------------------------------------------
void* castor::server::_thread_run(void* param)
{
  castor::server::BaseThreadPool* pool = 0;
  castor::server::IThread* thread = 0;
  struct threadArgs *args;

  if (param == NULL) {
    return 0;
  }

  // Recovering the processRequestArgs
  args = (struct threadArgs*)param;
  pool = dynamic_cast<castor::server::BaseThreadPool *>(args->handler);
  if (pool == 0) {
    delete args;
    return 0;
  }
  thread = pool->getThread();
  if (thread == 0) {
    delete args;
    return 0;
  }

  // Executing the thread
  try {
    thread->init(args->param);
    thread->run();
  } catch(...) {
    // XXX for the time being, it is the responsibility of the thread
    // to log any error.
  }
  delete args;
  return 0;
}


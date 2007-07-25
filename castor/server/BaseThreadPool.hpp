/******************************************************************************
 *                      BaseThreadPool.hpp
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
 * @(#)$RCSfile: BaseThreadPool.hpp,v $ $Revision: 1.14 $ $Release$ $Date: 2007/07/25 15:30:39 $ $Author: itglp $
 *
 * Abstract CASTOR thread pool
 *
 * @author Giuseppe Lo Presti
 *****************************************************************************/

#ifndef CASTOR_SERVER_BASETHREADPOOL_HPP
#define CASTOR_SERVER_BASETHREADPOOL_HPP 1

#include <iostream>
#include <string>
#include "castor/server/IThread.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/BaseObject.hpp"

namespace castor {

 namespace server {

  /**
   * default number of threads in the pool
   */
  static const int DEFAULT_THREAD_NUMBER = 20;

  /**
   * Basic CASTOR thread pool. Should not be instantiated by
   * user applications; other pools provide full functionality to run threads.
   */
  class BaseThreadPool : public BaseObject {

  public:

    /**
     * empty constructor
     */
    BaseThreadPool() : 
       BaseObject(), m_poolName(""), m_thread(0) {};

    /**
     * constructor
     */
    BaseThreadPool(const std::string poolName,
                   castor::server::IThread* thread);

    /*
     * destructor
     */
    virtual ~BaseThreadPool() throw();

    /**
     * Initializes the pool. This implementation throws an error.
     */
    virtual void init() throw (castor::exception::Exception);

    /**
     * Runs the pool. This function is supposed to spawn
     * the threads and immediately return.
     * Specialized pools implement it according to their needs,
     */
    virtual void run() throw (castor::exception::Exception) {};
    
    /**
     * Performs a graceful shutdown of the pool. This function is
     * supposed to asynchronously signal all threads in the pool 
     * in order to stop them. This implementation only calls the
     * thread's stop() method.
     * @return true if the pool is idle, false if it is not: in the
     * second case, the caller is supposed to wait and try again later.
     */
    virtual bool shutdown() throw();
     

    /**
     * Sets the number of threads
     */
    void setNbThreads(int value);

    /**
     * Gets the message service log stream
     * Note that the service has to be released after usage
     * @return a pointer to the message service or 0 if none
     * is available.
     */
    std::ostream& log() throw (castor::exception::Exception);


    /**
     * Gets the underlying working thread
     * @return pointer to the thread
     */
    castor::server::IThread* getThread() {
      return m_thread;
    }

    /**
     * Gets the the pool name initial as identifier for this pool
     */
    const char getPoolId() {
      return m_poolName[0];
    }


  protected:

    /**
     * The id of the pool created
     */
    int m_threadPoolId;

    /**
     * Number of threads in the pool.
     * The pool uses the Cthread API even if this is 1.
     */
    int m_nbThreads;

    /**
     * Name of the pool
     */
    std::string m_poolName;

    /**
     * The working thread
     */
    IThread* m_thread;
    
    /// Thread entrypoint made friend to access private fields.
    friend void* _thread_run(void* param);

  };


  /**
   * External entrypoint for the thread.
   * It is passed to Cpool_assign or Cthread_create_detached
   */
  void* _thread_run(void* param);

 } // end of namespace server

} // end of namespace castor

/**
 * Structure used to pass arguments to the thread through Cpool_assign
 */
struct threadArgs {
  castor::server::BaseThreadPool *handler;
  void *param;
};


#endif // CASTOR_SERVER_BASETHREADPOOL_HPP

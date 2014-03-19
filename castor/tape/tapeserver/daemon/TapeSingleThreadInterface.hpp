/* 
 * File:   TapeSingleThreadInterface.hpp
 * Author: dcome
 *
 * Created on March 18, 2014, 4:28 PM
 */

#ifndef TAPESINGLETHREADINTERFACE_HPP
#define	TAPESINGLETHREADINTERFACE_HPP

#include "castor/tape/tapeserver/threading/Threading.hpp"
#include "castor/tape/tapeserver/drive/Drive.hpp"

template <class Task> class TapeSingleThreadInterface : private castor::tape::threading::Thread
{
protected:
  class endOfSession: public Task {
    virtual bool endOfWork() { return true; }
  };
  castor::tape::threading::BlockingQueue<Task *> m_tasks;
  castor::tape::drives::DriveInterface & m_drive;
public:
  void finish() { m_tasks.push(new endOfSession); }
  void push(Task * t) { m_tasks.push(t); }
  
  virtual void startThreads(){ start(); }
  virtual void waitThreads() { wait(); } 
  
  TapeSingleThreadInterface(castor::tape::drives::DriveInterface & drive):
  m_drive(drive)
  {}
};

#endif	/* TAPESINGLETHREADINTERFACE_HPP */

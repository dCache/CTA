/* 
 * File:   DiskThreadPoolInterface.h
 * Author: dcome
 *
 * Created on March 18, 2014, 3:53 PM
 */

#pragma once

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
  
template <class Task> class DiskThreadPoolInterface
{
protected :
  castor::tape::threading::BlockingQueue<Task*> m_tasks;
  
public :
  virtual ~DiskThreadPoolInterface(){}
  
  virtual void push(Task *t)=0;
  virtual void finish()=0;
};

}}}}


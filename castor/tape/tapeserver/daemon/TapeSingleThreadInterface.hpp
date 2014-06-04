/* 
 * File:   TapeSingleThreadInterface.hpp
 * Author: dcome
 *
 * Created on March 18, 2014, 4:28 PM
 */

#pragma once

#include "castor/tape/tapeserver/threading/Threading.hpp"
#include "castor/tape/tapeserver/threading/BlockingQueue.hpp"
#include "castor/tape/tapeserver/drive/Drive.hpp"
#include "castor/tape/tapeserver/client/ClientInterface.hpp"
#include "castor/log/LogContext.hpp"
#include "castor/tape/tapeserver/daemon/CapabilityUtils.hpp"

namespace castor {
namespace legacymsg {
  class RmcProxy;
}
namespace tape {
namespace tapeserver {
namespace daemon {
  // Forward declaration
  class GlobalStatusReporter;
  /** 
   * This class is the base class for the 2 classes that will be executing 
   * all tape-{read|write} tasks. The template parameter Task is the type of 
   * task we are expecting : TapeReadTask or TapeWriteTask
   */
template <class Task>
class TapeSingleThreadInterface : private castor::tape::threading::Thread
{
private :
  /**
   * Utility to change the capabilities of the current tape thread
   */
  CapabilityUtils &m_capUtils;
protected:
  ///the queue of tasks 
  castor::tape::threading::BlockingQueue<Task *> m_tasks;
  
  /**
   * An interface to manipulate the drive to manipulate the tape
   * with the requested vid 
   */
  castor::tape::drives::DriveInterface & m_drive;
  
  /** Reference to the mount interface */
  castor::legacymsg::RmcProxy & m_rmc;
  
  /** Reference to the Global reporting interface */
  GlobalStatusReporter & m_gsr;
  
  ///The volumeID of the tape on which we want to operate  
  const std::string m_vid;

  ///log context, for ... logging purpose, copied du to thread mechanism 
  castor::log::LogContext m_logContext;
  
  client::ClientInterface::VolumeInfo m_volInfo;
  
  /**
   Integer to notify is the tapeserver if the drive has to be put down
   * 0 means everything all right, any other value means we have to put it down
   */
  int m_hardarwareStatus;
 
  /**
   * This function will try to set the cap_sys_rawio capability that is needed
   * for by tape thread to access /dev/nst
   */
  void setCapabilities(){
    try {
      m_capUtils.capSetProcText("cap_sys_rawio+ep");
      log::LogContext::ScopedParam sp(m_logContext,
              log::Param("capabilities", m_capUtils.capGetProcText()));
      m_logContext.log(LOG_INFO, "Set process capabilities for using tape");
    } catch(const castor::exception::Exception &ne) {
      m_logContext.log(LOG_ERR,
              "Failed to set process capabilities for using the tape ");
    }
  }
public:
  
  int getHardwareStatus() const {
    return m_hardarwareStatus;
  }
  /**
   * Push into the class a sentinel value to trigger to end the the thread.
   */
  void finish() { m_tasks.push(NULL); }
  
  /**
   * Push a new task into the internal queue
   * @param t the task to push
   */
  void push(Task * t) { m_tasks.push(t); }
  
  /**
   * Start the threads
   */
  virtual void startThreads(){ start(); }
  
  /**
   *  Wait for the thread to finish
   */
  virtual void waitThreads() { wait(); } 
  /**
   * Constructor
   * @param drive An interface to manipulate the drive to manipulate the tape
   * with the requested vid
   * @param rmc The media changer (=robot) that will (un)load/(un)mount the tape
   * @param gsr
   * @param volInfo All we need to know about the tape we are manipulating 
   * @param capUtils
   * @param lc lc The log context, later on copied
   */
  TapeSingleThreadInterface(castor::tape::drives::DriveInterface & drive,
    castor::legacymsg::RmcProxy & rmc,
    GlobalStatusReporter & gsr,
    const client::ClientInterface::VolumeInfo& volInfo,
    CapabilityUtils &capUtils,castor::log::LogContext & lc):m_capUtils(capUtils),
    m_drive(drive), m_rmc(rmc), m_gsr(gsr), m_vid(volInfo.vid), m_logContext(lc),
    m_volInfo(volInfo),m_hardarwareStatus(0) {

  }
};

}
}
}
}


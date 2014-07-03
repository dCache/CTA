/******************************************************************************
 *                      TapeWriteSingleThread.hpp
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

#pragma once

#include "castor/legacymsg/RmcProxy.hpp"
#include "castor/server/ProcessCap.hpp"
#include "castor/tape/tapeserver/daemon/MigrationReportPacker.hpp"
#include "castor/tape/tapeserver/daemon/TapeSingleThreadInterface.hpp"
#include "castor/tape/tapeserver/daemon/TapeWriteTask.hpp"
#include "castor/tape/tapeserver/daemon/TaskWatchDog.hpp"
#include "castor/tape/tapeserver/drive/Drive.hpp"
#include "castor/tape/tapeserver/threading/BlockingQueue.hpp"
#include "castor/tape/tapeserver/threading/Threading.hpp"
#include "castor/tape/utils/Timer.hpp"
#include "castor/tape/tapeserver/file/File.hpp"
#include <iostream>
#include <stdio.h>

namespace castor     {
namespace tape       {
namespace tapeserver {
namespace daemon     {
  
// forward declaration
class TapeServerReporter;

class TapeWriteSingleThread : public TapeSingleThreadInterface<TapeWriteTask> {
public:
  /**
   * Constructor
   * @param drive an interface for manipulating the drive in order 
   * to write on the tape 
   * @param vid the volume ID of the tape on which we are going to write
   * @param lc 
   * @param repPacker the object that will send reports to the client
   * @param filesBeforeFlush  how many file written before flushing on tape
   * @param bytesBeforeFlush how many bytes written before flushing on tape
   * @param lastFseq the last fSeq 
   */
  TapeWriteSingleThread(
    castor::tape::drives::DriveInterface & drive, 
    castor::legacymsg::RmcProxy & rmc,
    TapeServerReporter & tsr,
    const client::ClientInterface::VolumeInfo& volInfo,
    castor::log::LogContext & lc, MigrationReportPacker & repPacker,
    castor::server::ProcessCap &capUtils,
    uint64_t filesBeforeFlush, uint64_t bytesBeforeFlush);
    
  /**
   * 
   * @param lastFseq
   */
  void setlastFseq(uint64_t lastFseq);
private:
    class TapeCleaning{
    TapeWriteSingleThread& m_this;
  public:
    TapeCleaning(TapeWriteSingleThread& parent):m_this(parent){}
    ~TapeCleaning(){
      try{
      // Do the final cleanup
      m_this.m_drive.unloadTape();
      m_this.m_logContext.log(LOG_INFO, "TapeWriteSingleThread : Tape unloaded");
      // And return the tape to the library
      m_this.m_rmc.unmountTape(m_this.m_volInfo.vid, m_this.m_drive.librarySlot);
      m_this.m_logContext.log(LOG_INFO, "TapeWriteSingleThread : tape unmounted");
      m_this.m_tsr.tapeUnmounted();
              
      }
      catch(const castor::exception::Exception& ex){
        //set it to -1 to notify something failed during the cleaning 
        m_this.m_hardarwareStatus = -1;
        castor::log::ScopedParamContainer scoped(m_this.m_logContext);
        scoped.add("exception_message", ex.getMessageValue())
        .add("exception_code",ex.code());
        m_this.m_logContext.log(LOG_ERR, "Exception in TapeWriteSingleThread-TapeCleaning");
      } catch (...) {
          //set it to -1 to notify something failed during the cleaning 
          m_this.m_hardarwareStatus = -1;
          m_this.m_logContext.log(LOG_ERR, "Non-Castor exception in TapeWriteSingleThread-TapeCleaning when unmounting the tape");
      }
      
      //then we terminate the global status reporter
      m_this.m_tsr.finish();
    }
  };
  
  /**
   * Function to open the WriteSession 
   * If successful, returns a std::auto_ptr on it. A copy of that std::auto_ptr
   * will give the caller the ownership of the opened session (see auto_ptr 
   * copy constructor, which has a move semantic)
   * @return the WriteSession we need to write on tape
   */
  std::auto_ptr<castor::tape::tapeFile::WriteSession> openWriteSession();
  /**
   * Execute flush on tape, do some log and report the flush to the client
   * @param message the message the log will register
   * @param bytes the number of bytes that have been written since the last flush  
   * (for logging)
   * @param files the number of files that have been written since the last flush  
   * (also for logging)
   */
  void tapeFlush(const std::string& message,uint64_t bytes,uint64_t files);
  

  virtual void run() ;
  
  //m_filesBeforeFlush and m_bytesBeforeFlush are thresholds for flushing 
  //the first one crossed will trigger the flush on tape
  
  ///how many file written before flushing on tape
  const uint64_t m_filesBeforeFlush;
  
  ///how many bytes written before flushing on tape
  const uint64_t m_bytesBeforeFlush;

  ///an interface for manipulating all type of drives
  castor::tape::drives::DriveInterface& m_drive;
  
  ///the object that will send reports to the client
  MigrationReportPacker & m_reportPacker;
  
  /**
   * the last fseq that has been written on the tape = the starting point 
   * of our session. The last Fseq is computed by subtracting 1 to fSeg
   * of the first file to migrate we receive. That part is done by the 
   * MigrationTaskInjector.::synchronousInjection. Thus, we compute it into 
   * that function and retrieve/set it within DataTransferSession executeWrite
   * after we make sure synchronousInjection returned true. 
   * 
   * It should be const, but it cant 
   * (because there is no mutable function member in c++)
   */
   uint64_t m_lastFseq;

  /**
   * Should the compression be enabled ? This is currently hard coded to true 
   */
  const bool m_compress;

}; // class TapeWriteSingleThread

} // namespace daemon
} // namespace tapeserver
} // namsepace tape
} // namespace castor

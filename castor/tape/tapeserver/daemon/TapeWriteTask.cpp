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


#include "castor/tape/tapeserver/daemon/TapeWriteTask.hpp"
#include "castor/tape/tapeserver/daemon/DataPipeline.hpp"
#include "castor/tape/tapeserver/daemon/MigrationMemoryManager.hpp"
#include "castor/tape/tapeserver/daemon/DataConsumer.hpp"
#include "castor/tape/tapeserver/utils/suppressUnusedVariable.hpp"
#include "castor/tape/tapeserver/file/File.hpp" 
#include "castor/tape/tapeserver/daemon/AutoReleaseBlock.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Errnum.hpp"
#include "castor/tape/tapeserver/daemon/MigrationReportPacker.hpp"
#include "castor/tape/tapeserver/daemon/TapeSessionStats.hpp"
#include "castor/tape/tapeserver/daemon/MemBlock.hpp"
#include "castor/tape/tapeserver/daemon/ErrorFlag.hpp"
namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
  TapeWriteTask::TapeWriteTask(int blockCount, tapegateway::FileToMigrateStruct* file,
          MigrationMemoryManager& mm,castor::server::AtomicFlag& errorFlag): 
  m_fileToMigrate(file),m_memManager(mm), m_fifo(blockCount),
          m_blockCount(blockCount),m_errorFlag(errorFlag)
  {
    //register its fifo to the memory manager as a client in order to get mem block
    mm.addClient(&m_fifo); 
  }
//------------------------------------------------------------------------------
// fileSize
//------------------------------------------------------------------------------
   int TapeWriteTask::fileSize() { 
    return m_fileToMigrate->fileSize(); 
  }
//------------------------------------------------------------------------------
// execute
//------------------------------------------------------------------------------  
   void TapeWriteTask::execute(castor::tape::tapeFile::WriteSession & session,
           MigrationReportPacker & reportPacker,castor::log::LogContext& lc,
           utils::Timer & timer) {
    using castor::log::LogContext;
    using castor::log::Param;
    using castor::log::ScopedParamContainer;
    // Add to our logs the informations on the file
    ScopedParamContainer params(lc);
    params.add("NSFILEID",m_fileToMigrate->fileid())
          .add("lastKnownFileName",m_fileToMigrate->lastKnownFilename())
          .add("fileSize",m_fileToMigrate->fileSize())
          .add("fileTransactionId",m_fileToMigrate->fileTransactionId())
          .add("FSEQ",m_fileToMigrate->fseq())
          .add("path",m_fileToMigrate->path());
    
    // We will clock the stats for the file itself, and eventually add those
    // stats to the session's.
    
    utils::Timer localTime;
    
    unsigned long ckSum = Payload::zeroAdler32();
    
    uint32_t memBlockId  = 0;
    try {
      //try to open the session
      std::auto_ptr<castor::tape::tapeFile::WriteFile> output(openWriteFile(session,lc));
      m_taskStats.transferTime += timer.secs(utils::Timer::resetCounter);
      m_taskStats.headerVolume += TapeSessionStats::headerVolumePerFile;
      while(!m_fifo.finished()) {
        MemBlock* const mb = m_fifo.popDataBlock();
        m_taskStats.waitDataTime += timer.secs(utils::Timer::resetCounter);
        AutoReleaseBlock<MigrationMemoryManager> releaser(mb,m_memManager);
        
        //will throw (thus exiting the loop) if something is wrong
        checkErrors(mb,memBlockId,lc);
        
        ckSum =  mb->m_payload.adler32(ckSum);
        m_taskStats.checksumingTime += timer.secs(utils::Timer::resetCounter);
        mb->m_payload.write(*output);
        
        m_taskStats.transferTime += timer.secs(utils::Timer::resetCounter);
        m_taskStats.dataVolume += mb->m_payload.size();
        
        ++memBlockId;
      }
      
      //finish the writing of the file on tape
      //put the trailer
      output->close();
      m_taskStats.transferTime += timer.secs(utils::Timer::resetCounter);
      m_taskStats.headerVolume += TapeSessionStats::headerVolumePerFile;
      m_taskStats.filesCount ++;
      reportPacker.reportCompletedJob(*m_fileToMigrate,ckSum,output->getBlockId());
      m_taskStats.waitReportingTime += timer.secs(utils::Timer::resetCounter);
      // Log the successful transfer      
      logWithStats(LOG_INFO, "File successfully transmitted to drive",
              localTime.secs(),lc);
    } 
    catch(const castor::tape::tapeserver::daemon::ErrorFlag&){
      // We end up there because another task has failed 
      // so we just log, circulate blocks and don't even send a report 
      lc.log(LOG_INFO,"TapeWriteTask: a previous file has failed for migration "
      "Do nothing except circulating blocks");
      circulateMemBlocks();
      
      // We throw again because we want TWST to stop all tasks from execution 
      // and go into a degraded mode operation.
      throw;
    }
    catch(const castor::exception::Exception& e){
      //we can end up there because
      //we failed to open the WriteFile
      //we received a bad block or a block written failed
      //close failed
      
      //first set the error flag: we can't proceed any further with writes.
      m_errorFlag.set();

      //log and circulate blocks
      //We want to report internal error most of the time to avoid wrong
      // interpretation down the chain. Nevertheless, if the exception
      // if of type Errnum AND the errorCode is ENOSPC, we will propagate it.
      // This is how we communicate the fact that a tape is full to the client.
      int errorCode = e.code();
      try {
        const castor::exception::Errnum & errnum = 
            dynamic_cast<const castor::exception::Errnum &> (e);
        if (ENOSPC == errnum.errorNumber())
          errorCode = ENOSPC;
      } catch (...) {}
      LogContext::ScopedParam sp(lc, Param("exceptionCode",errorCode));
      LogContext::ScopedParam sp1(lc, Param("exceptionMessage", e.getMessageValue()));
      lc.log(LOG_ERR,"An error occurred for this file. End of migrations.");
      circulateMemBlocks();
      reportPacker.reportFailedJob(*m_fileToMigrate,e.getMessageValue(),errorCode);
  
      //we throw again because we want TWST to stop all tasks from execution 
      //and go into a degraded mode operation.
      throw;
    } 
   }
//------------------------------------------------------------------------------
// getFreeBlock
//------------------------------------------------------------------------------    
  MemBlock * TapeWriteTask::getFreeBlock() { 
    return m_fifo.getFreeBlock(); 
  }
//------------------------------------------------------------------------------
// checkErrors
//------------------------------------------------------------------------------  
  void TapeWriteTask::checkErrors(MemBlock* mb,int memBlockId,castor::log::LogContext& lc){
    using namespace castor::log;
    if(m_fileToMigrate->fileid() != static_cast<unsigned int>(mb->m_fileid)
            || memBlockId != mb->m_fileBlock
            || mb->isFailed()
            || mb->isCanceled()) {
      LogContext::ScopedParam sp[]={
        LogContext::ScopedParam(lc, Param("received_NSFILEID", mb->m_fileid)),
        LogContext::ScopedParam(lc, Param("expected_NSFBLOCKId", memBlockId)),
        LogContext::ScopedParam(lc, Param("received_NSFBLOCKId", mb->m_fileBlock)),
        LogContext::ScopedParam(lc, Param("failed_Status", mb->isFailed()))
      };
      tape::utils::suppresUnusedVariable(sp);
      std::string errorMsg;
      int errorCode;
      if(mb->isFailed()){
        errorMsg=mb->errorMsg();
        errorCode=mb->errorCode();
      } else if (mb->isCanceled()) {
        errorMsg="Received a block marked as cancelled";
        errorCode=SEINTERNAL;
      } else{
        errorMsg="Mismatch between expected and received file id or blockid";
        errorCode=SEINTERNAL;
      }
      // Set the error flag for the session (in case of mismatch)
      m_errorFlag.set();
      lc.log(LOG_ERR,errorMsg);
      throw castor::exception::Exception(errorCode,errorMsg);
    }
  }
//------------------------------------------------------------------------------
// pushDataBlock
//------------------------------------------------------------------------------   
   void TapeWriteTask::pushDataBlock(MemBlock *mb) {
    castor::server::MutexLocker ml(&m_producerProtection);
    m_fifo.pushDataBlock(mb);
  }
  
//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------   
   TapeWriteTask::~TapeWriteTask() {
    castor::server::MutexLocker ml(&m_producerProtection);
  }
//------------------------------------------------------------------------------
// openWriteFile
//------------------------------------------------------------------------------
   std::auto_ptr<tapeFile::WriteFile> TapeWriteTask::openWriteFile(
   tape::tapeFile::WriteSession & session, log::LogContext& lc){
     std::auto_ptr<tape::tapeFile::WriteFile> output;
     try{
       const uint64_t tapeBlockSize = 256*1024;
       output.reset(new tape::tapeFile::WriteFile(&session, *m_fileToMigrate,tapeBlockSize));
       lc.log(LOG_DEBUG, "Successfully opened the tape file for writing");
     }
     catch(const castor::exception::Exception & ex){
       log::LogContext::ScopedParam sp(lc, log::Param("exceptionCode",ex.code()));
       log::LogContext::ScopedParam sp1(lc, log::Param("exceptionMessage", ex.getMessageValue()));
       lc.log(LOG_ERR, "Failed to open tape file for writing");
       throw;
     }
     return output;
   }
//------------------------------------------------------------------------------
// circulateMemBlocks
//------------------------------------------------------------------------------   
   void TapeWriteTask::circulateMemBlocks(){
     while(!m_fifo.finished()) {
        m_memManager.releaseBlock(m_fifo.popDataBlock());
//        watchdog.notify();
     }
   }
//------------------------------------------------------------------------------
// hasAnotherTaskTailed
//------------------------------------------------------------------------------      
   void TapeWriteTask::hasAnotherTaskTailed() const {
    //if a task has signaled an error, we stop our job
    if(m_errorFlag){
      throw  castor::tape::tapeserver::daemon::ErrorFlag();
    }
  }
   
   void TapeWriteTask::logWithStats(int level, const std::string& msg,
   double fileTime,log::LogContext& lc) const{
     log::ScopedParamContainer params(lc);
     params.addTiming("transferTime", m_taskStats.transferTime)
           .addTiming("checksumingTime",m_taskStats.checksumingTime)
           .addTiming("waitDataTime",m_taskStats.waitDataTime)
           .addTiming("waitReportingTime",m_taskStats.waitReportingTime)
           .add("dataVolume",m_taskStats.dataVolume)
           .add("headerVolume",m_taskStats.headerVolume)
           .addTiming("totalTime", fileTime)
           .add("driveTransferSpeedMiB/s",
                   (m_taskStats.dataVolume+m_taskStats.headerVolume)
           /1024/1024
           /m_taskStats.transferTime)
           .add("payloadTransferSpeedMB/s",
                   1.0*m_taskStats.dataVolume/1024/1024/fileTime)
           .add("fileSize",m_fileToMigrate->fileSize())
           .add("fileid",m_fileToMigrate->fileid())
           .add("fseq",m_fileToMigrate->fseq())
           .add("fileTransactionId",m_fileToMigrate->fileTransactionId())
           .add("lastKnownFilename",m_fileToMigrate->lastKnownFilename())
           .add("lastModificationTime",m_fileToMigrate->lastModificationTime());
     
     lc.log(level, msg);

   }
//------------------------------------------------------------------------------
//   getTaskStats
//------------------------------------------------------------------------------
const TapeSessionStats TapeWriteTask::getTaskStats() const {
  return m_taskStats;
}
}}}}



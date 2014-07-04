/******************************************************************************
 *                      TapeWriteFileTask.cpp
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
#include "castor/tape/tapeserver/exception/Exception.hpp"
#include "castor/tape/tapeserver/daemon/MigrationReportPacker.hpp"
#include "castor/tape/tapeserver/daemon/SessionStats.hpp"

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
  TapeWriteTask::TapeWriteTask(int blockCount, tapegateway::FileToMigrateStruct* file,
          MigrationMemoryManager& mm,castor::tape::threading::AtomicFlag& errorFlag): 
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
           SessionStats & stats, utils::Timer & timer) {
    using castor::log::LogContext;
    using castor::log::Param;
    
    // We will clock the stats for the file itself, and eventually add those
    // stats to the session's.
    SessionStats localStats;
    utils::Timer localTime;
    unsigned long ckSum = Payload::zeroAdler32();
    int blockId  = 0;
    try {
      //we first check here to not even try to move the tape  if a previous task has failed
      //because the tape- could the very reason why the previous one failed, 
      //so dont do the same mistake twice !
      hasAnotherTaskTailed();
      
      //try to open the session
      std::auto_ptr<castor::tape::tapeFile::WriteFile> output(openWriteFile(session,lc));
      localStats.transferTime += timer.secs(utils::Timer::resetCounter);
      localStats.headerVolume += (3*80);
      while(!m_fifo.finished()) {

        //if someone screw somewhere else, we stop
        hasAnotherTaskTailed();
        
        MemBlock* const mb = m_fifo.popDataBlock();
        localStats.waitDataTime += timer.secs(utils::Timer::resetCounter);
        AutoReleaseBlock<MigrationMemoryManager> releaser(mb,m_memManager);
        
        if(m_fileToMigrate->fileid() != static_cast<unsigned int>(mb->m_fileid)
                         || blockId != mb->m_fileBlock  || mb->m_failed ){
          LogContext::ScopedParam sp[]={
            LogContext::ScopedParam(lc, Param("received_NSFILEID", mb->m_fileid)),
            LogContext::ScopedParam(lc, Param("expected_NSFBLOCKId", blockId)),
            LogContext::ScopedParam(lc, Param("received_NSFBLOCKId", mb->m_fileBlock)),
            LogContext::ScopedParam(lc, Param("failed_Status", mb->m_failed))
          };
          tape::utils::suppresUnusedVariable(sp);
          lc.log(LOG_ERR,"Failed to read from disk");
          throw castor::tape::Exception("Failed to read from disk");
        }

        ckSum =  mb->m_payload.adler32(ckSum);
        localStats.checksumingTime += timer.secs(utils::Timer::resetCounter);
        mb->m_payload.write(*output);
        localStats.transferTime += timer.secs(utils::Timer::resetCounter);
        localStats.dataVolume += mb->m_payload.size();
        ++blockId;
      }
      
      //finish the writing of the file on tape
      //put the trailer
      output->close();
      localStats.transferTime += timer.secs(utils::Timer::resetCounter);
      localStats.headerVolume += (3*80);
      localStats.filesCount ++;
      reportPacker.reportCompletedJob(*m_fileToMigrate,ckSum);
      localStats.waitReportingTime += timer.secs(utils::Timer::resetCounter);
      // Log the successful transfer
      double fileTime = localTime.secs();
      log::ScopedParamContainer params(lc);
      params.add("transferTime", localStats.transferTime)
            .add("checksumingTime",localStats.checksumingTime)
            .add("waitDataTime",localStats.waitDataTime)
            .add("waitReportingTime",localStats.waitReportingTime)
            .add("dataVolume",localStats.dataVolume)
            .add("headerVolume",localStats.headerVolume)
            .add("totalTime", fileTime)
            .add("driveTransferSpeedMiB/s",
                    (localStats.dataVolume+localStats.headerVolume)
                     /1024/1024
                     /localStats.transferTime)
            .add("payloadTransferSpeedMB/s",
                     1.0*localStats.dataVolume/1024/1024/fileTime)
            .add("fileSize",m_fileToMigrate->fileSize())
            .add("fileid",m_fileToMigrate->fileid())
            .add("fseq",m_fileToMigrate->fseq())
            .add("fileTransactionId",m_fileToMigrate->fileTransactionId())
            .add("lastKnownFilename",m_fileToMigrate->lastKnownFilename())
            .add("lastModificationTime",m_fileToMigrate->lastModificationTime());
      lc.log(LOG_INFO, "File successfully transmitted to drive");
      // Add the local counts to the session's
      stats.add(localStats);
    } 
    catch(const castor::tape::exceptions::ErrorFlag&){
     //we end up there because another task has failed 
      //so we just log, circulate blocks and don't even send a report 
      lc.log(LOG_INFO,"TapeWriteTask: a previous file has failed for migration "
      "Do nothing except circulating blocks");
      circulateMemBlocks();
      
      //we throw again because we want TWST to stop all tasks from execution 
      //and go into a degraded mode operation.
      throw;
    }
    catch(const castor::exception::Exception& e){
      //we can end up there because
      //we failed to open the WriteFile
      //we received a bad block or a block written failed
      //close failed
      
      //first set the error flag 
      m_errorFlag.set();

      //log and circulate blocks
      LogContext::ScopedParam sp(lc, Param("exceptionCode",e.code()));
      LogContext::ScopedParam sp1(lc, Param("exceptionMessage", e.getMessageValue()));
      lc.log(LOG_ERR,"Circulating blocks into TapeWriteTask::execute");
      circulateMemBlocks();
      reportPacker.reportFailedJob(*m_fileToMigrate,e.getMessageValue(),e.code());
  
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
// pushDataBlock
//------------------------------------------------------------------------------   
   void TapeWriteTask::pushDataBlock(MemBlock *mb) {
    castor::tape::threading::MutexLocker ml(&m_producerProtection);
    m_fifo.pushDataBlock(mb);
  }
  
//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------   
   TapeWriteTask::~TapeWriteTask() {
    castor::tape::threading::MutexLocker ml(&m_producerProtection);
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
   void TapeWriteTask::circulateMemBlocks(log::LogContext& lc){
     if(!m_errorFlag){
       lc.log(LOG_ERR,"Trying to force circulating of all mem block in TapeWriteTask, "
               "but nothing went wrong (or not record of it) = buggy software ! ");
       return;
     }
     circulateMemBlocks();
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
}}}}



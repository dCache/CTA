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

#include "common/log/LogContext.hpp"
#include "castor/tape/tapeserver/daemon/DiskWriteTask.hpp"
#include "castor/tape/tapeserver/daemon/AutoReleaseBlock.hpp"
#include "castor/tape/tapeserver/daemon/MemBlock.hpp"
#include "common/Timer.hpp"

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
  
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
DiskWriteTask::DiskWriteTask(cta::RetrieveJob *retrieveJob, RecallMemoryManager& mm): 
m_retrieveJob(retrieveJob),m_memManager(mm){

}

//------------------------------------------------------------------------------
// DiskWriteTask::execute
//------------------------------------------------------------------------------
bool DiskWriteTask::execute(RecallReportPacker& reporter,cta::log::LogContext&  lc,
    diskFile::DiskFileFactory & fileFactory, RecallWatchDog & watchdog) {
  using cta::log::LogContext;
  using cta::log::Param;
  cta::utils::Timer localTime;
  cta::utils::Timer totalTime(localTime);
  cta::utils::Timer transferTime(localTime);
  cta::log::ScopedParamContainer URLcontext(lc);
  URLcontext.add("archiveFileID",m_retrieveJob->retrieveRequest.archiveFileID)
            .add("dstURL", m_retrieveJob->retrieveRequest.dstURL)
            .add("fSeq",m_retrieveJob->selectedTapeFile().fSeq);
  // This out-of-try-catch variables allows us to record the stage of the 
  // process we're in, and to count the error if it occurs.
  // We will not record errors for an empty string. This will allow us to
  // prevent counting where error happened upstream.
  std::string currentErrorToCount = "";
  try{
    currentErrorToCount = "";
    // Placeholder for the disk file. We will open it only
    // after getting a first correct memory block.
    std::unique_ptr<tape::diskFile::WriteFile> writeFile;
    
    int blockId  = 0;
    unsigned long checksum = Payload::zeroAdler32();
    while(1) {
      if(MemBlock* const mb = m_fifo.pop()) {
        m_stats.waitDataTime+=localTime.secs(cta::utils::Timer::resetCounter);
        AutoReleaseBlock<RecallMemoryManager> releaser(mb,m_memManager);
        if(mb->isCanceled()) {
          // If the tape side got canceled, we report nothing and count
          // it as a success.
          lc.log(cta::log::DEBUG, "File transfer canceled");
          return true;
        }
        
        //will throw (thus exiting the loop) if something is wrong
        checkErrors(mb,blockId,lc);
        m_stats.checkingErrorTime += localTime.secs(cta::utils::Timer::resetCounter);
        // If we got that far on the first pass, it's now good enough to open
        // the disk file for writing...
        if (!writeFile.get()) {
          lc.log(cta::log::DEBUG, "About to open disk file for writing");
          // Synchronise the counter with the open time counter.
          currentErrorToCount = "Error_diskOpenForWrite";
          transferTime = localTime;
          writeFile.reset(fileFactory.createWriteFile(m_retrieveJob->retrieveRequest.dstURL));
          URLcontext.add("actualURL", writeFile->URL());
          lc.log(cta::log::INFO, "Opened disk file for writing");
          m_stats.openingTime+=localTime.secs(cta::utils::Timer::resetCounter);
        }
        
        // Write the data.
        currentErrorToCount = "Error_diskWrite";
        m_stats.dataVolume+=mb->m_payload.size();
        if (mb->m_payload.size())
          mb->m_payload.write(*writeFile);
        m_stats.readWriteTime+=localTime.secs(cta::utils::Timer::resetCounter);
        
        checksum = mb->m_payload.adler32(checksum);
        m_stats.checksumingTime+=localTime.secs(cta::utils::Timer::resetCounter);
        currentErrorToCount = "";
       
        blockId++;
      } //end if block non NULL
      else { 
        //close has to be explicit, because it may throw. 
        //A close is done  in WriteFile's destructor, but it may lead to some 
        //silent data loss
        currentErrorToCount = "Error_diskCloseAfterWrite";
        writeFile->close();
        m_stats.closingTime +=localTime.secs(cta::utils::Timer::resetCounter);
        m_stats.filesCount++;
        break;
        currentErrorToCount = "";
      }
    } //end of while(1)
    logWithStat(cta::log::INFO, "File successfully transfered to disk",lc);
    m_retrieveJob->transferredSize = m_stats.dataVolume;
    m_retrieveJob->transferredChecksumType = "ADLER32";
    { 
      std::stringstream cs;
      cs << std::hex << std::nouppercase << std::setfill('0') << std::setw(8) << (uint32_t)checksum;
      m_retrieveJob->transferredChecksumValue = cs.str();
    }
    reporter.reportCompletedJob(std::move(m_retrieveJob));
    m_stats.waitReportingTime+=localTime.secs(cta::utils::Timer::resetCounter);
    m_stats.transferTime = transferTime.secs();
    m_stats.totalTime = totalTime.secs();
    
    //everything went well, return true
    return true;
  } //end of try
  catch(const cta::exception::Exception& e){
    /*
     *We might end up there because ;
     * -- WriteFile failed 
     * -- A desynchronization between tape read and disk write
     * -- An error in tape read
     * -- An error while writing the file
     */
    
    //there might still be some blocks into m_fifo
    // We need to empty it
    releaseAllBlock();
    
    // Propagate the error to the watchdog
    if (currentErrorToCount.size()) {
      watchdog.addToErrorCount(currentErrorToCount);
    }
    
    m_stats.waitReportingTime+=localTime.secs(cta::utils::Timer::resetCounter);
    cta::log::ScopedParamContainer params(lc);
    params.add("errorMessage", e.getMessageValue());
    logWithStat(cta::log::ERR, "File writing to disk failed.", lc);
    lc.logBacktrace(cta::log::ERR, e.backtrace());
    m_retrieveJob->failureMessage = e.getMessageValue();
    reporter.reportFailedJob(std::move(m_retrieveJob));

    
    //got an exception, return false
    return false;
  }
}

//------------------------------------------------------------------------------
// DiskWriteTask::getFreeBlock
//------------------------------------------------------------------------------
MemBlock *DiskWriteTask::getFreeBlock() { 
  throw cta::exception::Exception("DiskWriteTask::getFreeBlock should mot be called");
}

//------------------------------------------------------------------------------
// DiskWriteTask::pushDataBlock
//------------------------------------------------------------------------------
void DiskWriteTask::pushDataBlock(MemBlock *mb) {
  cta::threading::MutexLocker ml(m_producerProtection);
  m_fifo.push(mb);
}

//------------------------------------------------------------------------------
// DiskWriteTask::~DiskWriteTask
//------------------------------------------------------------------------------
DiskWriteTask::~DiskWriteTask() { 
  volatile cta::threading::MutexLocker ml(m_producerProtection); 
}

//------------------------------------------------------------------------------
// DiskWriteTask::releaseAllBlock
//------------------------------------------------------------------------------
void DiskWriteTask::releaseAllBlock(){
  while(1){
    if(MemBlock* mb=m_fifo.pop())
      AutoReleaseBlock<RecallMemoryManager> release(mb,m_memManager);
    else 
      break;
  }
}

//------------------------------------------------------------------------------
// checkErrors
//------------------------------------------------------------------------------  
  void DiskWriteTask::checkErrors(MemBlock* mb,int blockId,cta::log::LogContext&  lc){
    using namespace cta::log;
    if(m_retrieveJob->retrieveRequest.archiveFileID != static_cast<unsigned int>(mb->m_fileid)
            || blockId != mb->m_fileBlock  || mb->isFailed() ){
      LogContext::ScopedParam sp[]={
        LogContext::ScopedParam(lc, Param("received_archiveFileID", mb->m_fileid)),
        LogContext::ScopedParam(lc, Param("expected_NSBLOCKId", blockId)),
        LogContext::ScopedParam(lc, Param("received_NSBLOCKId", mb->m_fileBlock)),
        LogContext::ScopedParam(lc, Param("failed_Status", mb->isFailed()))
      };
      tape::utils::suppresUnusedVariable(sp);
      std::string errorMsg;
      //int errCode;
      if(mb->isFailed()){
        errorMsg=mb->errorMsg();
        
        //disabled temporarily (see comment in MemBlock)
        //errCode=mb->errorCode();
      }
      else{
        errorMsg="Mismatch between expected and received filed or blockid";
        //errCode=666;
      }
      lc.log(cta::log::ERR,errorMsg);
      throw cta::exception::Exception(errorMsg);
    }
  }

//------------------------------------------------------------------------------
// getTiming
//------------------------------------------------------------------------------  
const DiskStats DiskWriteTask::getTaskStats() const{
  return m_stats;
}
//------------------------------------------------------------------------------
// logWithStat
//------------------------------------------------------------------------------  
void DiskWriteTask::logWithStat(int level,const std::string& msg,cta::log::LogContext&  lc){
  cta::log::ScopedParamContainer params(lc);
     params.add("readWriteTime", m_stats.readWriteTime)
           .add("checksumingTime",m_stats.checksumingTime)
           .add("waitDataTime",m_stats.waitDataTime)
           .add("waitReportingTime",m_stats.waitReportingTime)
           .add("checkingErrorTime",m_stats.checkingErrorTime)
           .add("openingTime",m_stats.openingTime)
           .add("closingTime",m_stats.closingTime)
           .add("transferTime", m_stats.transferTime)
           .add("totalTime", m_stats.totalTime)
           .add("dataVolume", m_stats.dataVolume)
           .add("globalPayloadTransferSpeedMBps",
              m_stats.totalTime?1.0*m_stats.dataVolume/1000/1000/m_stats.totalTime:0)
           .add("diskPerformanceMBps",
              m_stats.transferTime?1.0*m_stats.dataVolume/1000/1000/m_stats.transferTime:0)
           .add("openRWCloseToTransferTimeRatio", 
              m_stats.transferTime?(m_stats.openingTime+m_stats.readWriteTime+m_stats.closingTime)/m_stats.transferTime:0.0)
           .add("archiveFileID",m_retrieveJob->retrieveRequest.archiveFileID)
           .add("dstURL",m_retrieveJob->retrieveRequest.dstURL);
    lc.log(level,msg);
}
}}}}


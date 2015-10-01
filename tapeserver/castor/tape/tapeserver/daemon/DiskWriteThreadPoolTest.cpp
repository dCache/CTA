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
#include "castor/tape/tapeserver/daemon/DiskWriteThreadPool.hpp"
#include "castor/tape/tapeserver/daemon/RecallTaskInjector.hpp"
#include "castor/tape/tapeserver/daemon/RecallReportPacker.hpp"
#include "castor/tape/tapeserver/daemon/ReportPackerInterface.hpp"
#include "castor/log/LogContext.hpp"
#include "castor/log/StringLogger.hpp"
#include "castor/tape/tapeserver/daemon/MigrationMemoryManager.hpp"
#include "castor/tape/tapeserver/daemon/MemBlock.hpp"
//#include "scheduler/mockDB/MockSchedulerDatabase.hpp"
#include <gtest/gtest.h>

namespace unitTests{
  
  class TestingDatabaseRetrieveMount: public cta::SchedulerDatabase::RetrieveMount {
    virtual const MountInfo & getMountInfo() { throw std::runtime_error("Not implemented"); }
    virtual std::unique_ptr<cta::SchedulerDatabase::RetrieveJob> getNextJob() { throw std::runtime_error("Not implemented");}
    virtual void complete(time_t completionTime) { throw std::runtime_error("Not implemented"); }
  };
  
  class TestingRetrieveMount: public cta::RetrieveMount {
  public:
    TestingRetrieveMount(std::unique_ptr<cta::SchedulerDatabase::RetrieveMount> dbrm): RetrieveMount(std::move(dbrm)) {
    }
  };
  
  class TestingRetrieveJob: public cta::RetrieveJob {
  public:
    TestingRetrieveJob(): cta::RetrieveJob(*((cta::RetrieveMount *)NULL),
    cta::ArchiveFile(), 
    std::string(), cta::NameServerTapeFile(),
    cta::PositioningMethod::ByBlock) {}
  };
  

  
  using namespace castor::tape::tapeserver::daemon;
  using namespace castor::tape::tapeserver::client;
  struct MockRecallReportPacker : public RecallReportPacker {
    void reportCompletedJob(std::unique_ptr<cta::RetrieveJob> successfulRetrieveJob, u_int32_t checksum, u_int64_t size) {
      reportCompletedJob_(successfulRetrieveJob, checksum, size);
    }
    
    void reportFailedJob(std::unique_ptr<cta::RetrieveJob> failedRetrieveJob, const castor::exception::Exception &ex) {
      reportFailedJob_(failedRetrieveJob, ex);
    }
    MOCK_METHOD3(reportCompletedJob_,void(std::unique_ptr<cta::RetrieveJob> &successfulRetrieveJob, u_int32_t checksum, u_int64_t size));
    MOCK_METHOD2(reportFailedJob_, void(std::unique_ptr<cta::RetrieveJob> &failedRetrieveJob, const castor::exception::Exception &ex));
    MOCK_METHOD0(reportEndOfSession, void());
    MOCK_METHOD2(reportEndOfSessionWithErrors, void(const std::string,int));
    MockRecallReportPacker(cta::RetrieveMount *rm, castor::log::LogContext lc):
    RecallReportPacker(rm,lc){}
  };
  
  struct MockTaskInjector : public RecallTaskInjector{
    MOCK_METHOD3(requestInjection, void(int maxFiles, int maxBlocks, bool lastCall));
  };
  
  TEST(castor_tape_tapeserver_daemon, DiskWriteThreadPoolTest){
    using ::testing::_;
    
    castor::log::StringLogger log("castor_tape_tapeserver_daemon_DiskWriteThreadPoolTest");
    castor::log::LogContext lc(log);
    
    std::unique_ptr<cta::SchedulerDatabase::RetrieveMount> dbrm(new TestingDatabaseRetrieveMount);
    TestingRetrieveMount trm(std::move(dbrm));
    MockRecallReportPacker report(&trm,lc);
    
    EXPECT_CALL(report,reportCompletedJob_(_,_,_)).Times(5);
    EXPECT_CALL(report,reportEndOfSession()).Times(1);     
    
    RecallMemoryManager mm(10,100,lc);
    
    DiskWriteThreadPool dwtp(2,report,*((RecallWatchDog*)NULL),lc,"RFIO","/dev/null",0);
    dwtp.startThreads();
       
    for(int i=0;i<5;++i){
      std::unique_ptr<TestingRetrieveJob> fileToRecall(new TestingRetrieveJob());
      fileToRecall->archiveFile.path = "/dev/null";
      fileToRecall->archiveFile.fileId = i+1;
      fileToRecall->nameServerTapeFile.tapeFileLocation.blockId = 1;
      DiskWriteTask* t=new DiskWriteTask(fileToRecall.release(),mm);
      MemBlock* mb=mm.getFreeBlock();
      mb->m_fileid=i+1;
      mb->m_fileBlock=0;
      t->pushDataBlock(mb);
      t->pushDataBlock(NULL);
      dwtp.push(t);
    }
     
    dwtp.finish();
    dwtp.waitThreads(); 
  }
}


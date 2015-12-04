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
#include "castor/messages/TapeserverProxyDummy.hpp"
//#include "scheduler/mockDB/MockSchedulerDatabase.hpp"

#include "serrno.h"
#include "scheduler/Scheduler.hpp"
#include "nameserver/mockNS/MockNameServer.hpp"
#include "remotens/MockRemoteNS.hpp"

#include <memory>
#include <gtest/gtest.h>

namespace unitTests{
  class TestingDatabaseRetrieveMount: public cta::SchedulerDatabase::RetrieveMount {
    virtual const MountInfo & getMountInfo() { throw std::runtime_error("Not implemented"); }
    virtual std::unique_ptr<cta::SchedulerDatabase::RetrieveJob> getNextJob() { throw std::runtime_error("Not implemented");}
    virtual void complete(time_t completionTime) { throw std::runtime_error("Not implemented"); }
    virtual void setDriveStatus(cta::DriveStatus status, time_t completionTime) { throw std::runtime_error("Not implemented"); }
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
  using namespace castor::tape::diskFile;
  struct MockRecallReportPacker : public RecallReportPacker {
    void reportCompletedJob(std::unique_ptr<cta::RetrieveJob> successfulRetrieveJob) {
      castor::server::MutexLocker ml(&m_mutex);
      completeJobs++;
    }
    void reportFailedJob(std::unique_ptr<cta::RetrieveJob> failedRetrieveJob) {
      castor::server::MutexLocker ml(&m_mutex);
      failedJobs++;
    }
    void disableBulk() {}
    void reportEndOfSession() {
      castor::server::MutexLocker ml(&m_mutex);
      endSessions++;
    }
    void reportEndOfSessionWithErrors(const std::string msg, int error_code) {
      castor::server::MutexLocker ml(&m_mutex);
      endSessionsWithError++;
    }
    MockRecallReportPacker(cta::RetrieveMount *rm,castor::log::LogContext lc):
     RecallReportPacker(rm,lc), completeJobs(0), failedJobs(0),
      endSessions(0), endSessionsWithError(0) {}
    castor::server::Mutex m_mutex;
    int completeJobs;
    int failedJobs;
    int endSessions;
    int endSessionsWithError;
  };
  
  TEST(castor_tape_tapeserver_daemon, DiskWriteTaskFailedBlock){
    using ::testing::_;
    
    castor::log::StringLogger log("castor_tape_tapeserver_daemon_DiskWriteTaskFailedBlock");
    castor::log::LogContext lc(log);
    
    std::unique_ptr<cta::SchedulerDatabase::RetrieveMount> dbrm(new TestingDatabaseRetrieveMount());
    TestingRetrieveMount trm(std::move(dbrm));
    MockRecallReportPacker report(&trm,lc);
    RecallMemoryManager mm(10,100,lc);
    DiskFileFactory fileFactory("RFIO","",0);
    
    std::unique_ptr<TestingRetrieveJob> fileToRecall(new TestingRetrieveJob());
    fileToRecall->archiveFile.path = "/dev/null";
    fileToRecall->archiveFile.fileId = 0;
    DiskWriteTask t(fileToRecall.release(),mm);
    for(int i=0;i<6;++i){
      MemBlock* mb=mm.getFreeBlock();
      mb->m_fileid=0;
      mb->m_fileBlock=i;
      if(5==i){
        mb->markAsFailed("Test error",SEINTERNAL);
      }
      t.pushDataBlock(mb);
    }
    MemBlock* mb=mm.getFreeBlock();

    t.pushDataBlock(mb);
    t.pushDataBlock(NULL);
    castor::messages::TapeserverProxyDummy tspd;
    RecallWatchDog rwd(1,1,tspd,"", lc);
    t.execute(report,lc,fileFactory,rwd);
    ASSERT_EQ(1, report.failedJobs);
  }
}


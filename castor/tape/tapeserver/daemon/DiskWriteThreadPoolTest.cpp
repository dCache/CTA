#include "castor/tape/tapeserver/daemon/DiskWriteThreadPool.hpp"
#include "castor/tape/tapeserver/daemon/RecallTaskInjector.hpp"
#include "castor/tape/tapeserver/daemon/RecallReportPacker.hpp"
#include "castor/tape/tapegateway/FileToRecallStruct.hpp"
#include "castor/tape/tapeserver/daemon/ReportPackerInterface.hpp"
#include "castor/tape/tapeserver/client/ClientInterface.hpp"
#include "castor/tape/tapeserver/client/FakeClient.hpp"
#include "castor/tape/tapeserver/client/ClientInterface.hpp"
#include "castor/log/LogContext.hpp"
#include "castor/log/StringLogger.hpp"
#include "castor/tape/tapeserver/daemon/MemManager.hpp"
#include <gtest/gtest.h>

namespace unitTests{
  using namespace castor::tape::tapeserver::daemon;
  using namespace castor::tape::tapeserver::client;
  struct MockRecallReportPacker : public ReportPackerInterface<detail::Recall>{
    MOCK_METHOD1(reportCompletedJob,void(const FileStruct&));
    MOCK_METHOD3(reportFailedJob, void(const FileStruct& ,const std::string&,int));
    MOCK_METHOD0(reportEndOfSession, void());
    MOCK_METHOD2(reportEndOfSessionWithErrors, void(const std::string,int));
    MockRecallReportPacker(ClientInterface& client,castor::log::LogContext lc):
      ReportPackerInterface<detail::Recall>(client,lc){}
  };
  
  struct MockTaskInjector : public TaskInjector{
    MOCK_METHOD3(requestInjection, void(int maxFiles, int maxBlocks, bool lastCall));
  };
  
  TEST(castor_tape_tapeserver_daemon, DiskWriteThreadPoolTest){
    using ::testing::_;
    MockTaskInjector tskInjectorl;
    MockClient client;
    castor::log::StringLogger log("castor_tape_tapeserver_daemon_DiskWriteThreadPoolTest");
    castor::log::LogContext lc(log);
    
    MockRecallReportPacker report(client,lc);
    EXPECT_CALL(report,reportCompletedJob(_)).Times(5);
    //EXPECT_CALL(tskInjectorl,requestInjection(_,_,_)).Times(2);
    EXPECT_CALL(report,reportEndOfSession()).Times(1);     
    
    RecallMemoryManager mm(10,100);
    
    DiskWriteThreadPool dwtp(2,5,500,report,lc);
    dwtp.setJobInjector(&tskInjectorl);
    dwtp.startThreads();
    
    castor::tape::tapegateway::FileToRecallStruct file;
    file.setPath("/dev/null");
    file.setBlockId3(1);
       
     for(int i=0;i<5;++i){
       DiskWriteTask* t=new DiskWriteTask(dynamic_cast<tapegateway::FileToRecallStruct*>(file.clone()),mm);
       MemBlock* mb=mm.getFreeBlock();
       mb->m_fileid=0;
       mb->m_fileBlock=0;
       t->pushDataBlock(mb);
       t->pushDataBlock(NULL);
       dwtp.push(t);
     }
     
    dwtp.finish();
    dwtp.waitThreads(); 
  }
}


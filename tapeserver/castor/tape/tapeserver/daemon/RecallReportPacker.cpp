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

#include "castor/tape/tapeserver/daemon/RecallReportPacker.hpp"
#include "castor/tape/tapeserver/daemon/TaskWatchDog.hpp"
#include "castor/tape/tapegateway/FileRecalledNotificationStruct.hpp"
#include "castor/tape/tapegateway/FileRecalledNotificationStruct.hpp"
#include "castor/log/Logger.hpp"
#include "log.h"
#include "serrno.h"

#include <signal.h>

namespace{
  struct failedReportRecallResult : public castor::exception::Exception{
    failedReportRecallResult(const std::string& s): Exception(s){}
  };
}

using castor::log::LogContext;
using castor::log::Param;

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
//------------------------------------------------------------------------------
//Constructor
//------------------------------------------------------------------------------
RecallReportPacker::RecallReportPacker(cta::RetrieveMount *retrieveMount, 
    unsigned int reportFilePeriod,log::LogContext lc):
ReportPackerInterface<detail::Recall>(lc),
        m_workerThread(*this),m_reportFilePeriod(reportFilePeriod),m_errorHappened(false), m_retrieveMount(retrieveMount){

}
//------------------------------------------------------------------------------
//Destructor
//------------------------------------------------------------------------------
RecallReportPacker::~RecallReportPacker(){
  castor::server::MutexLocker ml(&m_producterProtection);
}
//------------------------------------------------------------------------------
//reportCompletedJob
//------------------------------------------------------------------------------
void RecallReportPacker::reportCompletedJob(const FileStruct& recalledFile,
  u_int32_t checksum, u_int64_t size){
  std::unique_ptr<Report> rep(new ReportSuccessful(recalledFile,checksum,size));
  castor::server::MutexLocker ml(&m_producterProtection);
  m_fifo.push(rep.release());
}
//------------------------------------------------------------------------------
//reportFailedJob
//------------------------------------------------------------------------------  
void RecallReportPacker::reportFailedJob(const FileStruct & recalledFile
,const std::string& msg,int error_code){
  std::unique_ptr<Report> rep(new ReportError(recalledFile,msg,error_code));
  castor::server::MutexLocker ml(&m_producterProtection);
  m_fifo.push(rep.release());
}
//------------------------------------------------------------------------------
//reportEndOfSession
//------------------------------------------------------------------------------
void RecallReportPacker::reportEndOfSession(){
  castor::server::MutexLocker ml(&m_producterProtection);
  m_fifo.push(new ReportEndofSession());
}
  
//------------------------------------------------------------------------------
//reportEndOfSessionWithErrors
//------------------------------------------------------------------------------
void RecallReportPacker::reportEndOfSessionWithErrors(const std::string msg,int error_code){
  castor::server::MutexLocker ml(&m_producterProtection);
  m_fifo.push(new ReportEndofSessionWithErrors(msg,error_code));
}

//------------------------------------------------------------------------------
//ReportSuccessful::execute
//------------------------------------------------------------------------------
void RecallReportPacker::ReportSuccessful::execute(RecallReportPacker& parent){
//  std::unique_ptr<FileSuccessStruct> successRecall(new FileSuccessStruct);
//  
//  successRecall->setFseq(m_recalledFile.fseq());
//  successRecall->setFileTransactionId(m_recalledFile.fileTransactionId());
//  successRecall->setId(m_recalledFile.id());
//  successRecall->setNshost(m_recalledFile.nshost());
//  successRecall->setFileid(m_recalledFile.fileid());
//  successRecall->setPath(m_recalledFile.path());
//  successRecall->setFileSize(m_size);
//
//  //WARNING : ad hoc name of checksum algorithm
//  successRecall->setChecksumName("adler32");
//  successRecall->setChecksum(m_checksum);
//  
//  parent.m_listReports->addSuccessfulRecalls(successRecall.release());
  parent.m_successfulRetrieveJobs.push(std::move(m_successfulRetrieveJob));
}
//------------------------------------------------------------------------------
//flush
//------------------------------------------------------------------------------
void RecallReportPacker::flush(){
  //we dont want to send empty reports
  unsigned int totalSize = m_listReports->failedRecalls().size() +
                           m_listReports->successfulRecalls().size();
  if(totalSize==0) {
    return;
  }
 
  client::ClientInterface::RequestReport chrono;
  try{
//    m_client.reportRecallResults(*m_listReports,chrono);
    while(m_successfulRetrieveJobs.size()) {
      std::unique_ptr<cta::RetrieveJob> successfulRetrieveJob = std::move(m_successfulRetrieveJobs.front());
      m_successfulRetrieveJobs.pop();
      successfulRetrieveJob->complete(0,0); //TODO: put size and checksum
    }
    {
      log::ScopedParamContainer params(m_lc);
      params.add("successCount", m_listReports->successfulRecalls().size())
            .add("failureCount", m_listReports->failedRecalls().size());
      logRequestReport(chrono,"RecallReportList successfully transmitted to client (contents follow)");
    }
    logReport(m_listReports->failedRecalls(),"Reported failed recall to client");
    logReport(m_listReports->successfulRecalls(),"Reported successful recall to client");
  } catch(const castor::exception::Exception& e){
    LogContext::ScopedParam s(m_lc, Param("exceptionCode",e.code()));
    LogContext::ScopedParam ss(m_lc, Param("exceptionMessageValue", e.getMessageValue()));
    LogContext::ScopedParam sss(m_lc, Param("exceptionWhat",e.what()));
    const std::string msg_error="An exception was caught trying to call reportRecallResults";
    m_lc.log(LOG_ERR,msg_error);
    throw failedReportRecallResult(msg_error);
  }
  //delete the old pointer and replace it with the new one provided
  //that way, all the reports that have been send are deleted (by FileReportList's destructor)
  m_listReports.reset(new FileReportList);
}
//------------------------------------------------------------------------------
//ReportEndofSession::execute
//------------------------------------------------------------------------------
void RecallReportPacker::ReportEndofSession::execute(RecallReportPacker& parent){
  client::ClientInterface::RequestReport chrono;
    if(!parent.errorHappened()){
//      parent.m_client.reportEndOfSession(chrono);
      parent.m_retrieveMount->complete();
      parent.logRequestReport(chrono,"Nominal RecallReportPacker::EndofSession has been reported",LOG_INFO);
      if (parent.m_watchdog) {
        parent.m_watchdog->addParameter(log::Param("status","success"));
        // We have a race condition here between the processing of this message by
        // the initial process and the printing of the end-of-session log, triggered
        // by the end our process. To delay the latter, we sleep half a second here.
        usleep(500*1000);
      }
    }
    else {
      const std::string& msg ="RecallReportPacker::EndofSession has been reported  but an error happened somewhere in the process";
      parent.m_lc.log(LOG_ERR,msg);
//      parent.m_client.reportEndOfSessionWithError(msg,SEINTERNAL,chrono);
      parent.m_retrieveMount->failed(cta::exception::Exception(msg));
      parent.logRequestReport(chrono,"reporting EndOfSessionWithError done",LOG_ERR);
      if (parent.m_watchdog) {
        parent.m_watchdog->addParameter(log::Param("status","failure"));
        // We have a race condition here between the processing of this message by
        // the initial process and the printing of the end-of-session log, triggered
        // by the end our process. To delay the latter, we sleep half a second here.
        usleep(500*1000);
      }
    }
}
//------------------------------------------------------------------------------
//ReportEndofSessionWithErrors::execute
//------------------------------------------------------------------------------
void RecallReportPacker::ReportEndofSessionWithErrors::execute(RecallReportPacker& parent){
  client::ClientInterface::RequestReport chrono;
  if(parent.m_errorHappened) {
//  parent.m_client.reportEndOfSessionWithError(m_message,m_error_code,chrono); 
  parent.m_retrieveMount->failed(cta::exception::Exception(m_message));    
  LogContext::ScopedParam(parent.m_lc,Param("errorCode",m_error_code));
  parent.m_lc.log(LOG_ERR,m_message);
  }
  else{
   const std::string& msg ="RecallReportPacker::EndofSessionWithErrors has been reported  but NO error was detected during the process";
   parent.m_lc.log(LOG_ERR,msg);
//   parent.m_client.reportEndOfSessionWithError(msg,SEINTERNAL,chrono);  
   parent.m_retrieveMount->failed(cta::exception::Exception(msg));    
  }
  if (parent.m_watchdog) {
    parent.m_watchdog->addParameter(log::Param("status","failure"));
    // We have a race condition here between the processing of this message by
    // the initial process and the printing of the end-of-session log, triggered
    // by the end our process. To delay the latter, we sleep half a second here.
    usleep(500*1000);
  }
}
//------------------------------------------------------------------------------
//ReportError::execute
//------------------------------------------------------------------------------
void RecallReportPacker::ReportError::execute(RecallReportPacker& parent){
   
//  std::unique_ptr<FileErrorStruct> failed(new FileErrorStruct);
//  //failedMigration->setFileMigrationReportList(parent.m_listReports.get());
//  failed->setErrorCode(m_error_code);
//  failed->setErrorMessage(m_error_msg);
//  failed->setFseq(m_recalledFile.fseq());
//  failed->setFileTransactionId(m_recalledFile.fileTransactionId());
//  failed->setId(m_recalledFile.id());
//  failed->setNshost(m_recalledFile.nshost());
//  
//  parent.m_listReports->addFailedRecalls(failed.release());
  parent.m_errorHappened=true;
  m_failedRetrieveJob->failed(cta::exception::Exception(m_error_msg));
}
//------------------------------------------------------------------------------
//WorkerThread::WorkerThread
//------------------------------------------------------------------------------
RecallReportPacker::WorkerThread::WorkerThread(RecallReportPacker& parent):
m_parent(parent) {
}
//------------------------------------------------------------------------------
//WorkerThread::run
//------------------------------------------------------------------------------
void RecallReportPacker::WorkerThread::run(){
  m_parent.m_lc.pushOrReplace(Param("thread", "RecallReportPacker"));
  m_parent.m_lc.log(LOG_DEBUG, "Starting RecallReportPacker thread");
  client::ClientInterface::RequestReport chrono;
  try{
      while(1) {    
        std::unique_ptr<Report> rep (m_parent.m_fifo.pop());    
        
        /*
         * this boolean is only true if it is the last turn of the loop
         * == rep is ReportEndOFSession or ReportEndOFSessionWithError
         */
        bool isItTheEnd = rep->goingToEnd();
        
        /*
         * if it is not the last turn, we want to execute the report 
         * (= insert the file into thr right list of results) BEFORE (trying to) 
         * flush
         */
        if(!isItTheEnd){
          rep->execute(m_parent);
        }
        //how mane files we have globally treated 
        unsigned int totalSize = m_parent.m_listReports->failedRecalls().size() +
                                 m_parent.m_listReports->successfulRecalls().size();
        
        //If we have enough reports or we are going to end the loop
        // or it is the end (== unconditional flush ) 
        // or we bypass the queuing system if the client is readtp
        // then we flush        
        if(totalSize >= m_parent.m_reportFilePeriod || isItTheEnd ||
           detail::ReportByFile == m_parent.m_reportBatching)
        {
        
          try{
            m_parent.flush();
          }
          catch(const failedReportRecallResult& e){
            //got there because we failed to report the recall results
            //we have to try to close the connection. 
            //reportEndOfSessionWithError might throw 
//            m_parent.m_client.reportEndOfSessionWithError(e.getMessageValue(),SEINTERNAL,chrono); 
            m_parent.m_retrieveMount->failed(e);    
            m_parent.logRequestReport(chrono,"Successfully closed client's session after the failed report RecallResult");
            if (m_parent.m_watchdog) {
              m_parent.m_watchdog->addToErrorCount("Error_clientCommunication");
              m_parent.m_watchdog->addParameter(log::Param("status","failure"));
            }
            // We need to wait until the end of session is signaled from upsteam
            while (!isItTheEnd) {
              std::unique_ptr<Report> r(m_parent.m_fifo.pop());
              isItTheEnd = r->goingToEnd();
            }
            break;
          }
        }
        
         /* 
          * It is the last turn of loop, we are going to send 
          * an EndOfSession (WithError) to the client. We need to have flushed 
          * all leftover BEFORE. Because as soon as we report the end, we can not 
          * report any the longer the success or failure of any job
         */
        if(isItTheEnd) {
          rep->execute(m_parent);
          break;
        }
    }
  }
  catch(const castor::exception::Exception& e){
    //we get there because to tried to close the connection and it failed
    //either from the catch a few lines above or directly from rep->execute
    m_parent.logRequestReport(chrono,"tried to report endOfSession(WithError) and got an exception, cant do much more",LOG_ERR);
    if (m_parent.m_watchdog) {
      m_parent.m_watchdog->addToErrorCount("Error_clientCommunication");
      m_parent.m_watchdog->addParameter(log::Param("status","failure"));
    }
  }
  m_parent.m_lc.log(LOG_DEBUG, "Finishing RecallReportPacker thread");
  
  //When we end up there, we might have still 
}

//------------------------------------------------------------------------------
//errorHappened()
//------------------------------------------------------------------------------
bool RecallReportPacker::errorHappened() {
  return m_errorHappened || (m_watchdog && m_watchdog->errorHappened());
}

}}}}

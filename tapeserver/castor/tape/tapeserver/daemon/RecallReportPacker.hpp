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

#pragma once

#include "castor/tape/tapeserver/daemon/ReportPackerInterface.hpp"
#include "castor/log/LogContext.hpp"
#include "castor/server/Threading.hpp"
#include "castor/server/BlockingQueue.hpp"
#include "scheduler/RetrieveJob.hpp"
#include "scheduler/RetrieveMount.hpp"

#include <memory>

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
  
class RecallReportPacker : public ReportPackerInterface<detail::Recall> {
public:
  /**
   * Constructor
   * @param tg the client to whom we report the success/failures
   * @param lc log context, copied du to threads
   */
  RecallReportPacker(cta::RetrieveMount *retrieveMount, log::LogContext lc);
  
  virtual ~RecallReportPacker();
  
 /**
   * Create into the MigrationReportPacker a report for the successful migration
   * of migratedFile
   * @param migratedFile the file successfully migrated
   * @param checksum the checksum the DWT has computed for the file 
   */
  virtual void reportCompletedJob(std::unique_ptr<cta::RetrieveJob> successfulRetrieveJob, u_int32_t checksum, u_int64_t size);
  
  /**
   * Create into the MigrationReportPacker a report for the failed migration
   * of migratedFile
   * @param migratedFile the file which failed 
   * @param ex the reason for the failure
   */
  virtual void reportFailedJob(std::unique_ptr<cta::RetrieveJob> failedRetrieveJob, const castor::exception::Exception& ex);
       
  /**
   * Create into the MigrationReportPacker a report for the nominal end of session
   */
  virtual void reportEndOfSession();
  
  /**
   * Create into the MigrationReportPacker a report for an erroneous end of session
   * @param msg The error message 
   * @param error_code The error code given by the drive
   */
  virtual void reportEndOfSessionWithErrors(const std::string msg,int error_code); 

  /**
   * Start the inner thread
   */
  void startThreads() { m_workerThread.start(); }
  
  /**
   * Stop the inner thread
   */
  void waitThread() { m_workerThread.wait(); }
  
  /**
   * Was there an error?
   */
  bool errorHappened();
  
private:
  //inner classes use to store content while receiving a report 
  class Report {
    const bool m_endNear;
  public:
    Report(bool b):m_endNear(b){}
    virtual ~Report(){}
    virtual void execute(RecallReportPacker& packer)=0;
    bool goingToEnd() const {return m_endNear;};
  };
  class ReportSuccessful :  public Report {
    u_int32_t m_checksum;
    u_int64_t m_size;
    
    /**
     * The successful retrieve job to be reported immediately
     */
    std::unique_ptr<cta::RetrieveJob> m_successfulRetrieveJob;
  public:
    ReportSuccessful(std::unique_ptr<cta::RetrieveJob> successfulRetrieveJob,u_int32_t checksum,
      u_int64_t size): 
    Report(false),m_checksum(checksum),m_size(size), m_successfulRetrieveJob(std::move(successfulRetrieveJob)){}
    virtual void execute(RecallReportPacker& reportPacker);
  };
  class ReportError : public Report {
    const castor::exception::Exception m_ex;
    
    /**
     * The failed retrieve job to be reported immediately
     */
    std::unique_ptr<cta::RetrieveJob> m_failedRetrieveJob;
  public:
    ReportError(std::unique_ptr<cta::RetrieveJob> failedRetrieveJob, const castor::exception::Exception &ex):
    Report(false),
    m_ex(ex),
    m_failedRetrieveJob(std::move(failedRetrieveJob)) {
    }

    virtual void execute(RecallReportPacker& reportPacker);
  };
  
  class ReportEndofSession : public Report {
  public:
    ReportEndofSession():Report(true){}
    virtual void execute(RecallReportPacker& reportPacker);
  };
  class ReportEndofSessionWithErrors : public Report {
    std::string m_message;
    int m_error_code;
  public:
    ReportEndofSessionWithErrors(std::string msg,int error_code):
    Report(true),m_message(msg),m_error_code(error_code){}
  
    virtual void execute(RecallReportPacker& reportPacker);
  };
  
  class WorkerThread: public castor::server::Thread {
    RecallReportPacker & m_parent;
  public:
    WorkerThread(RecallReportPacker& parent);
    virtual void run();
  } m_workerThread;
  
  castor::server::Mutex m_producterProtection;
  
  /** 
   * m_fifo is holding all the report waiting to be processed
   */
  castor::server::BlockingQueue<Report*> m_fifo;
  
  /**
   * Is set as true as soon as we process a reportFailedJob
   * That we can do a sanity check to make sure we always call 
   * the right end of the session  
   */
  bool m_errorHappened;
  
  /**
   * The mount object used to send reports
   */
  cta::RetrieveMount * m_retrieveMount;
};

}}}}



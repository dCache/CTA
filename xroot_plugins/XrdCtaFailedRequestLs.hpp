/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          CTA Frontend Archive File Ls stream implementation
 * @copyright      Copyright 2017 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <XrdSsiPbOStreamBuffer.hpp>
#include <scheduler/Scheduler.hpp>
//#include <scheduler/RetrieveJob.hpp>



namespace cta { namespace xrd {

/*!
 * Stream object which implements "af ls" command.
 */
class FailedRequestLsStream : public XrdSsiStream
{
public:
   FailedRequestLsStream(Scheduler &scheduler, bool is_archive, bool is_retrieve,
      bool is_log_entries, bool is_summary, cta::log::LogContext &lc) :
      XrdSsiStream(XrdSsiStream::isActive),
      m_scheduler(scheduler),
      m_isArchive(is_archive),
      m_isRetrieve(is_retrieve),
      m_isLogEntries(is_log_entries),
      m_isSummary(is_summary),
      m_lc(lc)
   {
      XrdSsiPb::Log::Msg(XrdSsiPb::Log::DEBUG, LOG_SUFFIX, "FailedRequestLsStream() constructor");
   }

   virtual ~FailedRequestLsStream() {
      XrdSsiPb::Log::Msg(XrdSsiPb::Log::DEBUG, LOG_SUFFIX, "~FailedRequestLsStream() destructor");
   }

   /*!
    * Synchronously obtain data from an active stream
    *
    * Active streams can only exist on the server-side. This XRootD SSI Stream class is marked as an
    * active stream in the constructor.
    *
    * @param[out]       eInfo   The object to receive any error description.
    * @param[in,out]    dlen    input:  the optimal amount of data wanted (this is a hint)
    *                           output: the actual amount of data returned in the buffer.
    * @param[in,out]    last    input:  should be set to false.
    *                           output: if true it indicates that no more data remains to be returned
    *                                   either for this call or on the next call.
    *
    * @return    Pointer to the Buffer object that contains a pointer to the the data (see below). The
    *            buffer must be returned to the stream using Buffer::Recycle(). The next member is usable.
    * @retval    0    No more data remains or an error occurred:
    *                 last = true:  No more data remains.
    *                 last = false: A fatal error occurred, eRef has the reason.
    */
   virtual Buffer *GetBuff(XrdSsiErrInfo &eInfo, int &dlen, bool &last) override {
      XrdSsiPb::Log::Msg(XrdSsiPb::Log::DEBUG, LOG_SUFFIX, "GetBuff(): XrdSsi buffer fill request (", dlen, " bytes)");

      XrdSsiPb::OStreamBuffer<Data> *streambuf;

      try {
         if(!m_isSummary && true) {
            // Nothing more to send, close the stream
            last = true;
            return nullptr;
         }

         streambuf = new XrdSsiPb::OStreamBuffer<Data>(dlen);

         // Special handling for -S option
         if(m_isSummary) {
            GetBuffSummary(streambuf);
            dlen = streambuf->Size();
            last = true;
            XrdSsiPb::Log::Msg(XrdSsiPb::Log::DEBUG, LOG_SUFFIX, "GetBuff(): Returning buffer with ", dlen, " bytes of data.");
            return streambuf;
         }
#if 0
   m_scheduler.listQueueItems(m_cliIdentity.username, "failed queue", m_lc);
   auto archiveJobFailedList = m_scheduler.getNextArchiveJobsFailedBatch(10,m_lc);
   cmdlineOutput << "Failed archive jobs: " << archiveJobFailedList.size() << std::endl;
   auto retrieveJobFailedList = m_scheduler.getRetrieveJobsFailedSummary(m_lc);
   cmdlineOutput << "Failed retrieve jobs: " << retrieveJobFailedList.size() << std::endl;
#endif

#if 0
         for(bool is_buffer_full = false; m_archiveFileItor.hasMore() && !is_buffer_full; )
         {
            const cta::common::dataStructures::FailedRequest archiveFile = m_archiveFileItor.next();

            for(auto jt = archiveFile.tapeFiles.cbegin(); jt != archiveFile.tapeFiles.cend(); jt++) {
               Data record;

               // Copy number
               record.mutable_afls_item()->set_copy_nb(jt->first);

               // Archive file
               auto af = record.mutable_afls_item()->mutable_af();
               af->set_archive_id(archiveFile.archiveFileID);
               af->set_disk_instance(archiveFile.diskInstance);
               af->set_disk_id(archiveFile.diskFileId);
               af->set_size(archiveFile.fileSize);
               af->mutable_cs()->set_type(archiveFile.checksumType);
               af->mutable_cs()->set_value(archiveFile.checksumValue);
               af->set_storage_class(archiveFile.storageClass);
               af->mutable_df()->set_owner(archiveFile.diskFileInfo.owner);
               af->mutable_df()->set_group(archiveFile.diskFileInfo.group);
               af->mutable_df()->set_path(archiveFile.diskFileInfo.path);
               af->set_creation_time(archiveFile.creationTime);

               // Tape file
               auto tf = record.mutable_afls_item()->mutable_tf();
               tf->set_vid(jt->second.vid);
               tf->set_f_seq(jt->second.fSeq);
               tf->set_block_id(jt->second.blockId);

               // is_buffer_full is set to true when we have one full block of data in the buffer, i.e.
               // enough data to send to the client. The actual buffer size is double the block size,
               // so we can keep writing a few additional records after is_buffer_full is true. These
               // will be sent on the next iteration. If we exceed the hard limit of double the block
               // size, Push() will throw an exception.
               is_buffer_full = streambuf->Push(record);
            }
         }
#endif
         dlen = streambuf->Size();
         XrdSsiPb::Log::Msg(XrdSsiPb::Log::DEBUG, LOG_SUFFIX, "GetBuff(): Returning buffer with ", dlen, " bytes of data.");
      } catch(cta::exception::Exception &ex) {
         std::ostringstream errMsg;
         errMsg << __FUNCTION__ << " failed: Caught CTA exception: " << ex.what();
         eInfo.Set(errMsg.str().c_str(), ECANCELED);
         delete streambuf;
      } catch(std::exception &ex) {
         std::ostringstream errMsg;
         errMsg << __FUNCTION__ << " failed: " << ex.what();
         eInfo.Set(errMsg.str().c_str(), ECANCELED);
         delete streambuf;
      } catch(...) {
         std::ostringstream errMsg;
         errMsg << __FUNCTION__ << " failed: Caught an unknown exception";
         eInfo.Set(errMsg.str().c_str(), ECANCELED);
         delete streambuf;
      }
      return streambuf;
   }

   void GetBuffSummary(XrdSsiPb::OStreamBuffer<Data> *streambuf) {
      SchedulerDatabase::JobsFailedSummary archive_summary;
      SchedulerDatabase::JobsFailedSummary retrieve_summary;

      if(m_isArchive) {
         Data record;
         archive_summary = m_scheduler.getArchiveJobsFailedSummary(m_lc);
         record.mutable_frls_summary()->set_request_type(cta::admin::RequestType::ARCHIVE_REQUEST);
         record.mutable_frls_summary()->set_total_files(archive_summary.totalFiles);
         record.mutable_frls_summary()->set_total_size(archive_summary.totalBytes);
         streambuf->Push(record);
      }
      if(m_isRetrieve) {
         Data record;
         retrieve_summary = m_scheduler.getRetrieveJobsFailedSummary(m_lc);
         record.mutable_frls_summary()->set_request_type(cta::admin::RequestType::RETRIEVE_REQUEST);
         record.mutable_frls_summary()->set_total_files(retrieve_summary.totalFiles);
         record.mutable_frls_summary()->set_total_size(retrieve_summary.totalBytes);
         streambuf->Push(record);
      }
      if(m_isArchive && m_isRetrieve) {
         Data record;
         record.mutable_frls_summary()->set_request_type(cta::admin::RequestType::TOTAL);
         record.mutable_frls_summary()->set_total_files(archive_summary.totalFiles + retrieve_summary.totalFiles);
         record.mutable_frls_summary()->set_total_size(archive_summary.totalBytes + retrieve_summary.totalBytes);
         streambuf->Push(record);
      }

      m_isSummary = false;
   }

private:
   cta::Scheduler &m_scheduler;            //!< Reference to CTA Scheduler

   bool m_isArchive;                       //!< List failed archive requests
   bool m_isRetrieve;                      //!< List failed retrieve requests
   bool m_isLogEntries;                    //!< Show failure log messages (verbose)
   bool m_isSummary;                       //!< Short summary of number of failures

   cta::log::LogContext &m_lc;             //!< Reference to CTA Log Context

   static constexpr const char* const LOG_SUFFIX  = "FailedRequestLsStream";    //!< Identifier for SSI log messages
};

}} // namespace cta::xrd

